/**
 * @file wm_drv_ops_rsa.c
 *
 * @brief DRV_RSA Module
 *
 */

/**
 *  Copyright 2022-2024 Beijing WinnerMicroelectronics Co.,Ltd.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"){}
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#include "wmsdk_config.h"
#include "wm_types.h"
#include "wm_error.h"
#include "wm_osal.h"
#include "wm_drv_rcc.h"

#include "wm_hal_rsa.h"

typedef struct {
    int (*init)(wm_device_t *dev);
    int (*deinit)(wm_device_t *dev);
    int (*expt_mod)(wm_device_t *dev, mbedtls_mpi *X, const mbedtls_mpi *A, const mbedtls_mpi *E, const mbedtls_mpi *N);
} wm_drv_rsa_ops_t;

/**
  * @brief  RSA driver layer device struct
  */
typedef struct {
    wm_hal_rsa_dev_t hal_dev;
    wm_os_mutex_t *mutex;
    wm_device_t *clock_dev;
} wm_drv_rsa_data_t;

/**
  * @brief  RSA device struct in dt
  */
typedef struct {
    char *name;               /**< device name                      */
    wm_dt_hw_rsa_t *hw;       /**< hardware info, @ref wm_dt_hw_rsa_t  */
    wm_drv_rsa_ops_t *ops;    /**< device operation interface       */
    wm_drv_rsa_data_t *drv;   /**< driver context data              */
    wm_device_status_t state; /**< device state                     */
    void *priv;               /**< user private data                */
} wm_dt_rsa_dev_t;

/**
  *     RSA & iflash share 1K memory,
  *     therefore, the rsa and iflash should acquire the mutex at the beginning of
  *     the function and release the mutex at the end of the function.
  */
static wm_device_t *iflash_dev;
extern wm_os_status_t wm_drv_internal_flash_acquire_mutex(wm_device_t *flash_dev, uint32_t wait_time);
extern wm_os_status_t wm_drv_internal_flash_release_mutex(wm_device_t *flash_dev);

static int wm_w800_drv_rsa_init(wm_device_t *device)
{
    wm_dt_rsa_dev_t *dev       = (wm_dt_rsa_dev_t *)device;
    wm_drv_rsa_data_t *drv_dev = NULL;
    wm_hal_rsa_dev_t *hal_dev  = NULL;

    drv_dev = (wm_drv_rsa_data_t *)dev->drv;

    /* ensure that the module has not been initialized and is not in the process of initialization. */
    if (dev->state == WM_DEV_ST_INITED) {
        return WM_ERR_ALREADY_INITED;
    }
    if (dev->state == WM_DEV_ST_INITING) {
        WM_DRV_RSA_TRACE_FAIL();
        return WM_ERR_BUSY;
    }
    dev->state = WM_DEV_ST_INITING;

    /* calloc driver layer device drv */
    if (drv_dev == NULL) {
        drv_dev = calloc(1, sizeof(wm_drv_rsa_data_t));
        if (!drv_dev) {
            WM_DRV_RSA_TRACE_FAIL();
            dev->state = WM_DEV_ST_UNINIT;
            return WM_ERR_NO_MEM;
        }
    }

    hal_dev = &(drv_dev->hal_dev);

    /* get rsa clock*/
    if ((drv_dev->clock_dev = wm_dt_get_device_by_name(dev->hw->rcc_device_name)) == NULL) {
        WM_DRV_RSA_TRACE_FAIL();
        dev->state = WM_DEV_ST_UNINIT;
        return WM_ERR_FAILED;
    }

    /* init hal dev attribute with hw */
    hal_dev->reg_base = (wm_rsa_reg_t *)(dev->hw->reg_base);

    /* change state to WM_DEV_ST_INITED */
    dev->state = WM_DEV_ST_INITED;

    iflash_dev = wm_dt_get_device_by_name("iflash");

    dev->drv = drv_dev;

    return WM_ERR_SUCCESS;
}

static int wm_w800_drv_rsa_deinit(wm_device_t *device)
{
    wm_dt_rsa_dev_t *dev = (wm_dt_rsa_dev_t *)device;

    /* make sure self module is already inited */
    if (dev->state == WM_DEV_ST_UNINIT || dev->drv == NULL) {
        WM_DRV_LOG_WARN();
        return WM_ERR_NO_INITED;
    }

    /*free driver layer device */
    free(dev->drv);
    dev->drv = NULL;

    /* change state to WM_DEV_ST_UNINIT */
    dev->state = WM_DEV_ST_UNINIT;

    return WM_ERR_SUCCESS;
}

int wm_w800_drv_rsa_expt_mod(wm_device_t *device, mbedtls_mpi *X, const mbedtls_mpi *A, const mbedtls_mpi *E,
                             const mbedtls_mpi *N)
{
    wm_dt_rsa_dev_t *dev = (wm_dt_rsa_dev_t *)device;

    wm_hal_rsa_dev_t *hal;
    int ret;

    WM_DRV_LOG_INFO();

    /* check some input params */
    WM_DRV_RSA_ASSERT_PARAM(dev && dev->drv);

    /* ensure qflash is not in using, acquire mutex */
    if (wm_drv_internal_flash_acquire_mutex(iflash_dev, WM_OS_WAIT_TIME_MAX) != WM_OS_STATUS_SUCCESS) {
        WM_DRV_RSA_TRACE_FAIL();
        return WM_ERR_BUSY;
    }

    /* enable rsa clock */
    wm_drv_clock_enable(dev->drv->clock_dev, WM_RCC_RSA_GATE_EN);

    /* resort to hal function */
    hal = &dev->drv->hal_dev;
    ret = wm_hal_rsa_expt_mod(hal, X, A, E, N);

    /*disable clock*/
    wm_drv_clock_disable(dev->drv->clock_dev, WM_RCC_RSA_GATE_EN);

    /* ensure qflash is not in using, release mutex */
    if (wm_drv_internal_flash_release_mutex(iflash_dev) != WM_OS_STATUS_SUCCESS) {
        WM_DRV_RSA_TRACE_FAIL();
        return WM_ERR_FAILED;
    }

    WM_DRV_LOG_INFO();

    return ret;
}

/* RSA driver operations */
const wm_drv_rsa_ops_t wm_drv_rsa_ops = {
    .init     = wm_w800_drv_rsa_init,
    .deinit   = wm_w800_drv_rsa_deinit,
    .expt_mod = wm_w800_drv_rsa_expt_mod,
};

/**
 * @file wm_drv_ops_crc.c
 *
 * @brief DRV_CRC Module
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

#include "wm_hal_crc.h"

typedef struct {
    int (*init)(wm_device_t *dev);
    int (*deinit)(wm_device_t *dev);
    int (*crc_cfg)(wm_device_t *dev, wm_drv_crc_cfg_t *ctx, uint32_t state, uint8_t type, uint8_t reverse);
    int (*crc_update)(wm_device_t *dev, wm_drv_crc_cfg_t *ctx, unsigned char *in, uint32_t len);
    int (*crc_final)(wm_device_t *dev, wm_drv_crc_cfg_t *ctx, uint32_t *crc_val);
} wm_drv_crc_ops_t;

/**
  * @brief  CRC driver layer device struct
  */
typedef struct {
    wm_hal_crc_dev_t hal_dev;
    wm_os_mutex_t *mutex;
    wm_device_t *clock_dev;
} wm_drv_crc_data_t;

/**
  * @brief  CRC device struct in dt
  */
typedef struct {
    char *name;               /**< device name                      */
    wm_dt_hw_crc_t *hw;       /**< hardware info, @ref wm_dt_hw_crc_t  */
    wm_drv_crc_ops_t *ops;    /**< device operation interface       */
    wm_drv_crc_data_t *drv;   /**< driver context data              */
    wm_device_status_t state; /**< device state                     */
    void *priv;               /**< user private data                */
} wm_dt_crc_dev_t;

/**
  * @brief gpsec mutex ops
  * Note:In W800, RNG/SHA/MD5/CRC/AES/DES/3DES share the same hardware module,
  *      therefore, only one function can be implemented at any given time.
  */
extern wm_os_status_t wm_drv_creat_gpsec_mutex(void);
extern wm_os_status_t wm_drv_acquire_gpsec_mutex(uint32_t wait_time);
extern wm_os_status_t wm_drv_release_gpsec_mutex(void);

static int wm_w800_drv_crc_init(wm_device_t *device)
{
    wm_dt_crc_dev_t *dev       = (wm_dt_crc_dev_t *)device;
    wm_drv_crc_data_t *drv_dev = NULL;
    wm_hal_crc_dev_t *hal_dev  = NULL;

    /* creat gpsec mutex if needed */
    if (wm_drv_creat_gpsec_mutex() != WM_OS_STATUS_SUCCESS) {
        WM_DRV_CRC_TRACE_FAIL();
        return WM_ERR_FAILED;
    }

    /* ensure that the module has not been initialized and is not in the process of initialization. */
    if (dev->state == WM_DEV_ST_INITED) {
        return WM_ERR_ALREADY_INITED;
    }
    if (dev->state == WM_DEV_ST_INITING) {
        WM_DRV_CRC_TRACE_FAIL();
        return WM_ERR_BUSY;
    }
    dev->state = WM_DEV_ST_INITING;

    /* calloc driver layer device drv */
    drv_dev = (wm_drv_crc_data_t *)dev->drv;
    if (drv_dev == NULL) {
        drv_dev = calloc(1, sizeof(wm_drv_crc_data_t));
        if (!drv_dev) {
            WM_DRV_CRC_TRACE_FAIL();
            dev->state = WM_DEV_ST_UNINIT;
            return WM_ERR_NO_MEM;
        }
    }

    /* get crc clock*/
    if ((drv_dev->clock_dev = wm_dt_get_device_by_name(dev->hw->rcc_device_name)) == NULL) {
        free(drv_dev);
        drv_dev = NULL;
        WM_DRV_CRC_TRACE_FAIL();
        dev->state = WM_DEV_ST_UNINIT;
        return WM_ERR_FAILED;
    }

    /* init hal dev attribute with hw */
    hal_dev            = &(drv_dev->hal_dev);
    hal_dev->gpsec_reg = (wm_gpsec_reg_t *)(dev->hw->reg_base);

    dev->drv = drv_dev;
    /* change state to WM_DEV_ST_INITED */
    dev->state = WM_DEV_ST_INITED;

    return WM_ERR_SUCCESS;
}

static int wm_w800_drv_crc_deinit(wm_device_t *device)
{
    wm_dt_crc_dev_t *dev = (wm_dt_crc_dev_t *)device;

    /* make sure self module is already inited */
    if (dev->state == WM_DEV_ST_UNINIT || dev->drv == NULL) {
        WM_DRV_LOG_WARN();
        return WM_ERR_NO_INITED;
    }

    /* disable cryp clock */
    wm_drv_clock_disable(dev->drv->clock_dev, WM_RCC_GPSEC_GATE_EN);

    /*free driver layer device */
    free(dev->drv);
    dev->drv = NULL;

    /* change state to WM_DEV_ST_UNINIT */
    dev->state = WM_DEV_ST_UNINIT;

    return WM_ERR_SUCCESS;
}

static int wm_w800_drv_crc_cfg(wm_device_t *device, wm_drv_crc_cfg_t *ctx, uint32_t state, uint8_t type, uint8_t reverse)
{
    (void)device;
    ctx->state   = state;
    ctx->type    = type;
    ctx->reverse = reverse;
    return WM_ERR_SUCCESS;
}

static int wm_w800_drv_crc_update(wm_device_t *device, wm_drv_crc_cfg_t *ctx, unsigned char *in, uint32_t len)
{
    wm_dt_crc_dev_t *dev = (wm_dt_crc_dev_t *)(device);
    wm_hal_crc_dev_t *hal_dev;
    int ret;

    /* check some input params */
    WM_DRV_CRC_ASSERT_PARAM(dev->drv);

    if (wm_drv_acquire_gpsec_mutex(WM_OS_WAIT_TIME_MAX) != WM_OS_STATUS_SUCCESS) {
        WM_DRV_CRC_TRACE_FAIL();
        return WM_ERR_FAILED;
    }

    /* enable cryp clock */
    wm_drv_clock_enable(dev->drv->clock_dev, WM_RCC_GPSEC_GATE_EN);

    /* set configuration to hal dev */
    hal_dev = &dev->drv->hal_dev;
    memcpy(&hal_dev->crc_cfg, ctx, sizeof(wm_drv_crc_cfg_t));

    /* resort to hal function */
    ret = wm_hal_crc_update(hal_dev, in, len);

    /* update state */
    ctx->state = hal_dev->crc_cfg.state;

    /* release gpsec mutex */
    if (wm_drv_release_gpsec_mutex() != WM_OS_STATUS_SUCCESS) {
        WM_DRV_CRC_TRACE_FAIL();
        return WM_ERR_FAILED;
    }

    return ret;
}

static int wm_w800_drv_crc_final(wm_device_t *device, wm_drv_crc_cfg_t *ctx, uint32_t *crc_val)
{
    (void)device;
    *crc_val = ctx->state;
    return WM_ERR_SUCCESS;
}

/* CRC driver operations */
const wm_drv_crc_ops_t wm_drv_crc_ops = {
    .init       = wm_w800_drv_crc_init,
    .deinit     = wm_w800_drv_crc_deinit,
    .crc_cfg    = wm_w800_drv_crc_cfg,
    .crc_update = wm_w800_drv_crc_update,
    .crc_final  = wm_w800_drv_crc_final,
};

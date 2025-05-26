/**
 *  Copyright 2022-2024 Beijing WinnerMicroelectronics Co.,Ltd.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
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
#include <string.h>
#include "wm_types.h"
#include "wm_osal.h"
#include "wm_drv_gpio.h"
#include "wm_dt_hw.h"
#include "wm_drv_rcc.h"
#include "wm_drv_spi_master.h"
#include "wm_drv_ops_spi_master.h"

#define LOG_TAG "drv_spim"
#include "wm_log.h"

// below is API for upper layer
int wm_drv_spim_transceive_sync(wm_device_t *dev, const wm_dt_hw_spim_dev_cfg_t *config, spim_transceive_t *desc,
                                uint32_t ms_to_wait)
{
    wm_drv_spim_ops_t *ops = NULL;
    int ret                = WM_ERR_INVALID_PARAM;
    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    ops = dev->ops;
    if (ops && ops->transceive_sync) {
        ret = ops->transceive_sync(dev, config, desc, ms_to_wait);
    }

    return ret;
}

int wm_drv_spim_transceive_async(wm_device_t *dev, const wm_dt_hw_spim_dev_cfg_t *config, spim_transceive_t *desc,
                                 wm_spim_callback_t callback, void *usr_data)
{
    wm_drv_spim_ops_t *ops = NULL;
    int ret                = WM_ERR_INVALID_PARAM;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    ops = dev->ops;
    if (ops && ops->transceive_async) {
        ret = ops->transceive_async(dev, config, desc, callback, usr_data);
    }

    return ret;
}

wm_device_t *wm_drv_spim_init(const char *dev_name)
{
    wm_device_t *spim_dev  = NULL;
    wm_drv_spim_ops_t *ops = NULL;
    int ret                = WM_ERR_FAILED;

    do {
        spim_dev = wm_dt_get_device_by_name(dev_name);
        if (spim_dev == NULL) {
            wm_log_error("get spim device fail\n");
            break;
        }

        if (spim_dev->drv == NULL) {
            spim_dev->drv = (wm_drv_spim_ctx_t *)wm_os_internal_malloc(sizeof(wm_drv_spim_ctx_t));
            memset(spim_dev->drv, 0, sizeof(wm_drv_spim_ctx_t));
            if (spim_dev->drv == NULL) {
                wm_log_error("allock spim drv data fail\n");
                break;
            }
        }

        if (spim_dev->ops) {
            ops = spim_dev->ops;
            if (ops->init) {
                ret = ops->init(spim_dev);
            }
        }
    } while (0);

    return ret == WM_ERR_SUCCESS ? spim_dev : NULL;
}

int wm_drv_spim_deinit(wm_device_t *dev)
{
    wm_drv_spim_ops_t *ops = NULL;
    int ret                = WM_ERR_INVALID_PARAM;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    ops = dev->ops;
    if (ops && ops->deinit) {
        ret = ops->deinit(dev);
    }

    if (dev->drv == NULL) {
        wm_os_internal_free(dev->drv);
        dev->drv = NULL;
    }

    dev = NULL;

    return ret;
}

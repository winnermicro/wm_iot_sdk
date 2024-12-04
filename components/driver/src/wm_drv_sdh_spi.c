/**
 * @file wm_drv_sdh_spi.c
 *
 * @brief DRV_SDH_SPI Module
 *
 */

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
#include "wm_drv_sdh_spi.h"
#include "wm_dt_hw.h"
#include "wm_drv_ops_sdh_spi.c"

wm_device_t *wm_drv_sdh_spi_init(char *dev_name)
{
    int ret                   = WM_ERR_SUCCESS;
    wm_device_t *sdspi_dev    = NULL;
    wm_drv_sdh_spi_ops_t *ops = NULL;

    sdspi_dev = wm_dt_get_device_by_name(dev_name);
    if (sdspi_dev && sdspi_dev->ops) {
        ops = sdspi_dev->ops;
        if (ops->init) {
            ret = ops->init(sdspi_dev);
            if (ret == WM_ERR_SUCCESS) {
                return sdspi_dev;
            }
        }
    }

    return NULL;
}

int wm_drv_sdh_spi_deinit(wm_device_t *dev)
{
    int ret                   = WM_ERR_SUCCESS;
    wm_drv_sdh_spi_ops_t *ops = NULL;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    ops = dev->ops;
    if (ops && ops->deinit) {
        ret = ops->deinit(dev);
    } else {
        return WM_ERR_INVALID_PARAM;
    }

    return ret;
}

int wm_drv_sdh_spi_transceive_sync(wm_device_t *dev, const wm_dt_hw_spim_dev_cfg_t *config, spim_transceive_t *desc,
                                   uint32_t ms_to_wait)
{
    int ret                   = WM_ERR_SUCCESS;
    wm_drv_sdh_spi_ops_t *ops = NULL;

    if (dev == NULL || desc == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    ops = dev->ops;
    if (ops && ops->transceive_sync) {
        ret = ops->transceive_sync(dev, config, desc, ms_to_wait);
    } else {
        return WM_ERR_INVALID_PARAM;
    }

    return ret;
}

int wm_drv_sdh_spi_transceive_async(wm_device_t *dev, const wm_dt_hw_spim_dev_cfg_t *config, spim_transceive_t *desc,
                                    wm_spim_callback_t callback, void *usr_data)
{
    int ret                   = WM_ERR_SUCCESS;
    wm_drv_sdh_spi_ops_t *ops = NULL;

    if (dev == NULL || desc == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    ops = dev->ops;
    if (ops && ops->transceive_async) {
        ret = ops->transceive_async(dev, config, desc, callback, usr_data);
    } else {
        return WM_ERR_INVALID_PARAM;
    }

    return ret;
}

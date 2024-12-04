/**
 * @file wm_drv_sdh_sdmmc.c
 *
 * @brief DRV_SDH_SDMMC Module
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
#include "wm_drv_sdh_sdmmc.h"
#include "wm_dt_hw.h"
#include "wm_drv_dma.h"
#include "wm_drv_ops_sdh_sdmmc.c"

wm_device_t *wm_drv_sdh_sdmmc_init(char *dev_name)
{
    int ret                     = WM_ERR_SUCCESS;
    wm_device_t *sdmmc_dev      = NULL;
    wm_drv_sdh_sdmmc_ops_t *ops = NULL;

    sdmmc_dev = wm_dt_get_device_by_name(dev_name);
    if (sdmmc_dev && sdmmc_dev->ops) {
        ops = sdmmc_dev->ops;
        if (ops->init) {
            ret = ops->init(sdmmc_dev);
            if (ret == WM_ERR_SUCCESS) {
                return sdmmc_dev;
            }
        }
    }

    return NULL;
}

int wm_drv_sdh_sdmmc_deinit(wm_device_t *dev)
{
    int ret                     = WM_ERR_SUCCESS;
    wm_drv_sdh_sdmmc_ops_t *ops = NULL;

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

int wm_drv_sdh_sdmmc_read_blocks(wm_device_t *dev, uint8_t *buf, uint32_t start_block, uint32_t num_blocks)
{
    int ret                     = WM_ERR_SUCCESS;
    wm_drv_sdh_sdmmc_ops_t *ops = NULL;

    if (dev == NULL || buf == NULL || num_blocks == 0 || num_blocks > WM_DRV_SDH_SDMMC_MAX_NUM_BLOCKS) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    ops = dev->ops;
    if (ops && ops->read_blocks) {
        ret = ops->read_blocks(dev, buf, start_block, num_blocks);
    } else {
        return WM_ERR_INVALID_PARAM;
    }

    return ret;
}

int wm_drv_sdh_sdmmc_write_blocks(wm_device_t *dev, const uint8_t *buf, uint32_t start_block, uint32_t num_blocks)
{
    int ret                     = WM_ERR_SUCCESS;
    wm_drv_sdh_sdmmc_ops_t *ops = NULL;

    if (dev == NULL || buf == NULL || num_blocks == 0 || num_blocks > WM_DRV_SDH_SDMMC_MAX_NUM_BLOCKS) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    ops = dev->ops;
    if (ops && ops->write_blocks) {
        ret = ops->write_blocks(dev, buf, start_block, num_blocks);
    } else {
        return WM_ERR_INVALID_PARAM;
    }

    return ret;
}

int wm_drv_sdh_sdmmc_get_card_info(wm_device_t *dev, wm_drv_sdh_sdmmc_card_info_t *card_info)
{
    int ret                     = WM_ERR_SUCCESS;
    wm_drv_sdh_sdmmc_ops_t *ops = NULL;

    if (dev == NULL || card_info == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    ops = dev->ops;
    if (ops && ops->get_card_info) {
        ret = ops->get_card_info(dev, card_info);
    } else {
        return WM_ERR_INVALID_PARAM;
    }

    return ret;
}

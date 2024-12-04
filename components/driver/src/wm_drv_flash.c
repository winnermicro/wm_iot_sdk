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
#include "wm_drv_flash.h"
#include "wm_drv_flash_ops.h"

#include "wm_dt_hw.h"
#if CONFIG_COMPONENT_DRIVER_INTERNAL_FLASH_ENABLED
#include "wm_drv_ops_internal_flash.c"
#endif
#if CONFIG_COMPONENT_DRIVER_EXTERNAL_FLASH_ENABLED
#include "wm_drv_ops_external_flash.c"
#endif

#undef LOG_TAG
#define LOG_TAG "drv_fls"

#include "wm_log.h"

int wm_drv_flash_write_with_erase(wm_device_t *dev, uint32_t addr, uint8_t *wr_buf, uint32_t wr_len)
{
    wm_drv_flash_ops_t *ops = NULL;
    int ret                 = WM_ERR_INVALID_PARAM;

    if (dev == NULL || wr_buf == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    ops = dev->ops;
    if (ops && ops->write_with_erase) {
        ret = ops->write_with_erase(dev, addr, wr_buf, wr_len);
    }

    return ret;
}

int wm_drv_flash_write(wm_device_t *dev, uint32_t addr, uint8_t *wr_buf, uint32_t wr_len)
{
    wm_drv_flash_ops_t *ops = NULL;
    int ret                 = WM_ERR_INVALID_PARAM;

    if (dev == NULL || wr_buf == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    ops = dev->ops;
    if (ops && ops->write) {
        ret = ops->write(dev, addr, wr_buf, wr_len);
    }

    return ret;
}

int wm_drv_flash_read(wm_device_t *dev, uint32_t addr, uint8_t *rd_buf, uint32_t rd_len)
{
    wm_drv_flash_ops_t *ops = NULL;
    int ret                 = WM_ERR_INVALID_PARAM;

    if (dev == NULL || rd_buf == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    ops = dev->ops;
    if (ops && ops->read) {
        ret = ops->read(dev, addr, rd_buf, rd_len);
    }

    return ret;
}

int wm_drv_flash_get_device_info(wm_device_t *dev, wm_drv_flash_info_t *info)
{
    wm_drv_flash_ops_t *ops = NULL;
    int ret                 = WM_ERR_INVALID_PARAM;

    if (dev == NULL || info == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    ops = dev->ops;
    if (ops && ops->get_info) {
        ret = ops->get_info(dev, info);
    }

    return ret;
}

int wm_drv_flash_erase_region(wm_device_t *dev, uint32_t addr, uint32_t erase_len)
{
    wm_drv_flash_ops_t *ops = NULL;
    int ret                 = WM_ERR_INVALID_PARAM;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    ops = dev->ops;
    if (ops && ops->erase_region) {
        ret = ops->erase_region(dev, addr, erase_len);
    }

    return ret;
}

int wm_drv_flash_erase_sector(wm_device_t *dev, uint32_t sector_idx, uint32_t sector_count)
{
    wm_drv_flash_ops_t *ops = NULL;
    int ret                 = WM_ERR_INVALID_PARAM;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    ops = dev->ops;
    if (ops && ops->erase_sector) {
        ret = ops->erase_sector(dev, sector_idx, sector_count);
    }

    return ret;
}

int wm_drv_flash_erase_chip(wm_device_t *dev)
{
    wm_drv_flash_ops_t *ops = NULL;
    int ret                 = WM_ERR_INVALID_PARAM;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    ops = dev->ops;
    if (ops && ops->erase_chip) {
        ret = ops->erase_chip(dev);
    }

    return ret;
}

int wm_drv_flash_deinit(wm_device_t *dev)
{
    wm_drv_flash_ops_t *ops = NULL;
    int ret                 = WM_ERR_SUCCESS;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    ops = dev->ops;
    if (ops && ops->deinit) {
        ret = ops->deinit(dev);
    } else {
        ret = WM_ERR_FAILED;
    }

    return ret;
}

wm_device_t *wm_drv_flash_init(char *dev_name)
{
    wm_device_t *flash_dev  = NULL;
    wm_drv_flash_ops_t *ops = NULL;
    int ret                 = WM_ERR_SUCCESS;

    flash_dev = wm_dt_get_device_by_name(dev_name);
    if (!flash_dev) {
        wm_log_error("get flash dev fail\n");
        ret = WM_ERR_INVALID_PARAM;
    }

    if (flash_dev) {
        ops = flash_dev->ops;
        if (ops->init) {
            ret = ops->init(flash_dev);
        } else {
            ret = WM_ERR_FAILED;
        }
    }

    return ret == WM_ERR_SUCCESS ? flash_dev : NULL;
}

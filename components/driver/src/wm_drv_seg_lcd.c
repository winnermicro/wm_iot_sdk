/**
 * @file wm_drv_seg_lcd.c
 *
 * @brief DRV_SEG_LCD Module
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
#include "wm_drv_seg_lcd.h"
#include "wm_dt_hw.h"
#include "wm_drv_ops_seg_lcd.c"

#define LOG_TAG "drv_seg_lcd"
#include "wm_log.h"

wm_device_t *wm_drv_seg_lcd_init(char *dev_name)
{
    int ret                   = WM_ERR_SUCCESS;
    wm_device_t *seg_lcd_dev  = NULL;
    wm_drv_seg_lcd_ops_t *ops = NULL;

    seg_lcd_dev = wm_dt_get_device_by_name(dev_name);
    if (seg_lcd_dev && seg_lcd_dev->ops) {
        ops = seg_lcd_dev->ops;
        if (ops->init) {
            ret = ops->init(seg_lcd_dev);
            if (ret == WM_ERR_SUCCESS) {
                return seg_lcd_dev;
            }
        }
    }

    return NULL;
}

int wm_drv_seg_lcd_deinit(wm_device_t *dev)
{
    int ret                   = WM_ERR_SUCCESS;
    wm_drv_seg_lcd_ops_t *ops = NULL;

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

int wm_drv_seg_lcd_clear(wm_device_t *dev)
{
    int ret                   = WM_ERR_SUCCESS;
    wm_drv_seg_lcd_ops_t *ops = NULL;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    ops = dev->ops;
    if (ops && ops->clear) {
        ret = ops->clear(dev);
    } else {
        return WM_ERR_INVALID_PARAM;
    }

    return ret;
}

int wm_drv_seg_lcd_display(wm_device_t *dev, uint8_t com_id, uint32_t seg_mask, uint32_t seg_data)
{
    int ret                   = WM_ERR_SUCCESS;
    wm_drv_seg_lcd_ops_t *ops = NULL;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    ops = dev->ops;
    if (ops && ops->display) {
        ret = ops->display(dev, com_id, seg_mask, seg_data);
    } else {
        return WM_ERR_INVALID_PARAM;
    }

    return ret;
}

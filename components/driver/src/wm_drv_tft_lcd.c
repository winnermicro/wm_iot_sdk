/**
 * @file wm_drv_lcd.h
 *
 * @brief Driver LCD(TFT) Module
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
#include "wm_types.h"
#include "wm_error.h"
#include "wm_attr.h"
#include "wm_osal.h"
#include "wm_drv_tft_lcd.h"
#include "wm_drv_ops_tft_lcd.h"

wm_device_t *wm_drv_tft_lcd_init(const char *dev_name)
{
    int ret = WM_ERR_SUCCESS;

    wm_device_t *dev = wm_dt_get_device_by_name(dev_name);
    if ((dev == NULL) || (dev->state != WM_DEV_ST_UNINIT)) {
        return NULL;
    }

    dev->state = WM_DEV_ST_INITING;

    ret = ((wm_drv_tft_lcd_ops_t *)dev->ops)->lcd_init(dev);
    if (ret == WM_ERR_SUCCESS) {
        dev->state = WM_DEV_ST_INITED;
        return dev;
    }

    dev->state = WM_DEV_ST_UNINIT;
    return NULL;
}

int wm_drv_tft_lcd_deinit(wm_device_t *dev)
{
    int ret = WM_ERR_SUCCESS;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    ret = ((wm_drv_tft_lcd_ops_t *)dev->ops)->lcd_deinit(dev);
    if (ret == WM_ERR_SUCCESS) {
        dev->state = WM_DEV_ST_UNINIT;
    }

    return ret;
}

int wm_drv_tft_lcd_draw_bitmap(wm_device_t *dev, wm_lcd_data_desc_t data_desc)
{
    int ret                   = WM_ERR_SUCCESS;
    wm_drv_tft_lcd_ops_t *ops = NULL;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    ops = dev->ops;
    if (ops && ops->draw_bitmap) {
        ret = ops->draw_bitmap(dev, data_desc);
    } else {
        ret = WM_ERR_INVALID_PARAM;
    }

    return ret;
}

int wm_drv_tft_lcd_set_rotation(wm_device_t *dev, wm_lcd_rotate_t degree)
{
    int ret                   = WM_ERR_SUCCESS;
    wm_drv_tft_lcd_ops_t *ops = NULL;

    if (dev == NULL || degree >= LCD_ROTATION_MAX) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    ops = dev->ops;
    if (ops && ops->lcd_set_rotation && degree < LCD_ROTATION_MAX) {
        ret = ops->lcd_set_rotation(dev, degree);
    } else {
        ret = WM_ERR_INVALID_PARAM;
    }

    return ret;
}

int wm_drv_tft_lcd_set_backlight(wm_device_t *dev, bool on_off)
{
    int ret                   = WM_ERR_SUCCESS;
    wm_drv_tft_lcd_ops_t *ops = NULL;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    ops = dev->ops;
    if (ops && ops->lcd_set_backlight) {
        ret = ops->lcd_set_backlight(dev, on_off);
    } else {
        ret = WM_ERR_INVALID_PARAM;
    }

    return ret;
}

int wm_drv_tft_lcd_get_capability(wm_device_t *dev, wm_lcd_capabilitys_t *cap)
{
    int ret                   = WM_ERR_SUCCESS;
    wm_drv_tft_lcd_ops_t *ops = NULL;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    ops = dev->ops;
    if (ops && ops->lcd_get_capability) {
        ret = ops->lcd_get_capability(dev, cap);
    } else {
        ret = WM_ERR_INVALID_PARAM;
    }

    return ret;
}

int wm_drv_tft_lcd_send_cmd_data(wm_device_t *dev, uint16_t cmd, wm_lcd_cmd_type_t cmd_type, uint8_t *data, uint16_t data_len)
{
    int ret                   = WM_ERR_SUCCESS;
    wm_drv_tft_lcd_ops_t *ops = NULL;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    ops = dev->ops;
    if (ops && ops->lcd_cmd_data) {
        ret = ops->lcd_cmd_data(dev, cmd, cmd_type, data, data_len);
    } else {
        ret = WM_ERR_INVALID_PARAM;
    }

    return ret;
}

int wm_drv_tft_lcd_register_tx_callback(wm_device_t *dev, wm_lcd_tx_callback_t callback, void *arg)
{
    wm_drv_tft_lcd_ops_t *ops = NULL;
    int err                   = WM_ERR_INVALID_PARAM;

    if (dev) {
        if (dev->state != WM_DEV_ST_INITED) {
            return WM_ERR_NO_INITED;
        }

        ops = dev->ops;
        if (ops && ops->lcd_add_tx_callback) {
            err = ops->lcd_add_tx_callback(dev, callback, arg);
        }
    }

    return err;
}

int wm_drv_tft_lcd_unregister_tx_callback(wm_device_t *dev)
{
    wm_drv_tft_lcd_ops_t *ops = NULL;
    int err                   = WM_ERR_INVALID_PARAM;

    if (dev) {
        if (dev->state != WM_DEV_ST_INITED) {
            return WM_ERR_NO_INITED;
        }

        ops = dev->ops;
        if (ops && ops->lcd_remove_tx_callback) {
            err = ops->lcd_remove_tx_callback(dev);
        }
    }

    return err;
}
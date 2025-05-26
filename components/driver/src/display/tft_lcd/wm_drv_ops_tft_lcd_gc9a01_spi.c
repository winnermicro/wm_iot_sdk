/**
 * @file wm_drv_ops_tft_lcd_gc9a01_spi.c
 *
 * @brief DRV TFT LCD OPS Module for gc9a01 with SPI Interface
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

#include "wm_drv_tft_lcd.h"
#include "wm_dt.h"
#include "wm_dt_hw.h"
#include "wm_soc_cfgs.h"

#include "wm_gc9a01.h"
#include "wm_drv_gpio.h"
#include "wm_utils.h"
#include "wm_drv_sdh_spi.h"
#include "wm_drv_ops_tft_lcd_spi.h"

int wm_gc9a01_set_windows(wm_device_t *dev, uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end)
{
    int ret        = WM_ERR_FAILED;
    uint8_t win[4] = { 0, 0, 0, 0 };

    if ((!dev) || (!dev->hw)) {
        return WM_ERR_INVALID_PARAM;
    }

    WM_DRV_TFT_LCD_LOG_I("wm_gc9a01_set_windows %d(0x%x),%d(0x%x),%d(0x%x),%d(0x%x)", x_start, x_start, y_start, y_start, x_end,
                         x_end, y_end, y_end);

    win[0] = x_start >> 8;
    win[1] = x_start & 0x00FF;
    win[2] = x_end >> 8;
    win[3] = x_end & 0x00FF;

    ret = lcd_send_command(dev, GC9A01_CASET, LCD_CMD_TYPE_8BIT);
    WM_DRV_LCD_TFT_CHECK_RETURN(ret);

    ret = lcd_send_data(dev, win, sizeof(win));
    WM_DRV_LCD_TFT_CHECK_RETURN(ret);

    win[0] = y_start >> 8;
    win[1] = y_start & 0x00FF;
    win[2] = y_end >> 8;
    win[3] = y_end & 0x00FF;

    ret = lcd_send_command(dev, GC9A01_RASET, LCD_CMD_TYPE_8BIT);
    WM_DRV_LCD_TFT_CHECK_RETURN(ret);

    ret = lcd_send_data(dev, win, sizeof(win));
    WM_DRV_LCD_TFT_CHECK_RETURN(ret);

    ret = lcd_send_command(dev, GC9A01_RAMWR, LCD_CMD_TYPE_8BIT);
    WM_DRV_LCD_TFT_CHECK_RETURN(ret);

    return ret;
}

static int wm_drv_ops_gc9a01_spi_init(wm_device_t *dev)
{
    int ret                    = WM_ERR_FAILED;
    wm_drv_tft_lcd_spi_t *drv  = NULL;
    wm_dt_hw_tft_lcd_spi_t *hw = NULL;
    wm_device_t *gpio_dev      = NULL;

    if ((!dev) || (!dev->hw)) {
        return WM_ERR_INVALID_PARAM;
    }

    hw = (wm_dt_hw_tft_lcd_spi_t *)dev->hw;

    drv = (wm_drv_tft_lcd_spi_t *)wm_os_internal_malloc(sizeof(wm_drv_tft_lcd_spi_t));
    if (!drv) {
        return WM_ERR_NO_MEM;
    }
    memset(drv, 0x00, sizeof(wm_drv_tft_lcd_spi_t));

    drv->spi_dev = wm_dt_get_device_by_name(hw->spi_device_name);
    WM_DRV_LCD_TFT_CHECK_PARAM_RET(drv->spi_dev, WM_ERR_NO_INITED);

    /*TODO: replace by lookup table*/
    drv->spi_cfg = &hw->spi_cfg;
    dev->drv     = drv;

    drv->panel_cfg.rotation = WM_CFG_TFT_LCD_ROTATION;

#if ((WM_CFG_TFT_LCD_ROTATION == LCD_ROTATION_NONE) || (WM_CFG_TFT_LCD_ROTATION == LCD_ROTATION_180_DEGREE))
    drv->panel_cfg.x_resolution = WM_CFG_TFT_LCD_X_RESOLUTION;
    drv->panel_cfg.y_resolution = WM_CFG_TFT_LCD_Y_RESOLUTION;
#else
    drv->panel_cfg.x_resolution = WM_CFG_TFT_LCD_Y_RESOLUTION;
    drv->panel_cfg.y_resolution = WM_CFG_TFT_LCD_X_RESOLUTION;
#endif

    /* init gpio driver */
    if (hw->gpio_device_name != NULL) {
        gpio_dev = wm_dt_get_device_by_name(hw->gpio_device_name);
        if ((!gpio_dev) || (gpio_dev->state != WM_DEV_ST_INITED)) {
            gpio_dev = wm_drv_gpio_init(hw->gpio_device_name);
        }
    }
    WM_DRV_LCD_TFT_CHECK_PARAM_RET(gpio_dev, WM_ERR_NO_INITED);

    /* panel control gpio initilization */
    if (hw->io_lcd_reset < WM_GPIO_NUM_MAX) {
        wm_drv_gpio_iomux_func_sel(hw->io_lcd_reset, WM_GPIO_IOMUX_FUN5);
        wm_drv_gpio_set_dir(hw->io_lcd_reset, WM_GPIO_DIR_OUTPUT);
    }
    if (hw->io_lcd_led < WM_GPIO_NUM_MAX) {
        wm_drv_gpio_iomux_func_sel(hw->io_lcd_led, WM_GPIO_IOMUX_FUN5);
        wm_drv_gpio_set_dir(hw->io_lcd_led, WM_GPIO_DIR_OUTPUT);
    }
    if (hw->io_lcd_dcx < WM_GPIO_NUM_MAX) {
        wm_drv_gpio_iomux_func_sel(hw->io_lcd_dcx, WM_GPIO_IOMUX_FUN5);
        wm_drv_gpio_set_dir(hw->io_lcd_dcx, WM_GPIO_DIR_OUTPUT);
    }
    if (hw->io_lcd_te < WM_GPIO_NUM_MAX) {
        wm_drv_gpio_iomux_func_sel(hw->io_lcd_te, WM_GPIO_IOMUX_FUN5);
        wm_drv_gpio_set_dir(hw->io_lcd_te, WM_GPIO_DIR_INPUT);
    }

    ret = lcd_io_backlight_off(hw);
    WM_DRV_LCD_TFT_CHECK_RETURN(ret);

    ret = lcd_io_reset(hw);
    WM_DRV_LCD_TFT_CHECK_RETURN(ret);

    ret = lcd_io_dcx_stop(hw);
    WM_DRV_LCD_TFT_CHECK_RETURN(ret);

    ret = lcd_init_cmd(dev, gc9a01_init_seq, sizeof(gc9a01_init_seq));
    WM_DRV_LCD_TFT_CHECK_RETURN(ret);

    return ret;
}

static int wm_drv_ops_gc9a01_spi_draw_bitmap(wm_device_t *dev, wm_lcd_data_desc_t data_desc)
{
    int ret                   = WM_ERR_FAILED;
    wm_drv_tft_lcd_spi_t *drv = NULL;

    if (!dev->drv) {
        return WM_ERR_INVALID_PARAM;
    }

    drv = (wm_drv_tft_lcd_spi_t *)dev->drv;

    WM_DRV_TFT_LCD_LOCK(drv->mutex);

    /* TODO: 2 is PIXEL width which need to be adjusted according to the pixel format */
    if ((data_desc.x_end >= drv->panel_cfg.x_resolution) || (data_desc.y_end >= drv->panel_cfg.y_resolution) ||
        (data_desc.x_end < data_desc.x_start) || (data_desc.y_end < data_desc.y_start) ||
        ((data_desc.x_end - data_desc.x_start + 1) * (data_desc.y_end - data_desc.y_start + 1) * 2 < data_desc.buf_size)) {
        //WM_DRV_TFT_LCD_LOG_E("draw xs(%d),ys(%d), xe(%d),ye(%d),buf=(0x%p),len(%d)", data_desc.x_start,
        //                data_desc.y_start, data_desc.x_end, data_desc.y_end, data_desc.buf, data_desc.buf_size);
        ret = WM_ERR_INVALID_PARAM;
        WM_DRV_LCD_TFT_CHECK_GOTO(ret, exit);
    }

    ret = wm_gc9a01_set_windows(dev, data_desc.x_start, data_desc.y_start, data_desc.x_end, data_desc.y_end);
    WM_DRV_LCD_TFT_CHECK_GOTO(ret, exit);

    ret = lcd_send_data(dev, data_desc.buf, data_desc.buf_size);
    WM_DRV_LCD_TFT_CHECK_GOTO(ret, exit);

exit:
    WM_DRV_TFT_LCD_UNLOCK(drv->mutex);
    return ret;
}

static int wm_drv_ops_gc9a01_spi_deinit(wm_device_t *dev)
{
    int ret                   = WM_ERR_SUCCESS;
    wm_drv_tft_lcd_spi_t *drv = NULL;

    if (!dev->drv) {
        return WM_ERR_INVALID_PARAM;
    }

    drv = (wm_drv_tft_lcd_spi_t *)dev->drv;

    WM_DRV_TFT_LCD_LOCK(drv->mutex);

    /*TODO: replace by lookup table*/
    //set GPIO as default state

    WM_DRV_TFT_LCD_UNLOCK(drv->mutex);
    wm_os_internal_mutex_delete(drv->mutex);

    wm_os_internal_free(dev->drv);
    dev->drv = NULL;

    return ret;
}

static int wm_drv_ops_gc9a01_spi_set_bl(wm_device_t *dev, bool on_off)
{
    int ret                    = WM_ERR_SUCCESS;
    wm_drv_tft_lcd_spi_t *drv  = NULL;
    wm_dt_hw_tft_lcd_spi_t *hw = NULL;

    if (!dev->drv || !dev->hw) {
        return WM_ERR_INVALID_PARAM;
    }

    drv = (wm_drv_tft_lcd_spi_t *)dev->drv;
    hw  = (wm_dt_hw_tft_lcd_spi_t *)dev->hw;

    WM_DRV_TFT_LCD_LOCK(drv->mutex);

    if (on_off) {
        ret = lcd_io_backlight_on(hw);
    } else {
        ret = lcd_io_backlight_off(hw);
    }
    WM_DRV_TFT_LCD_UNLOCK(drv->mutex);

    WM_DRV_LCD_TFT_CHECK_RETURN(ret);

    return ret;
}

static int wm_drv_ops_gc9a01_spi_set_rotation(wm_device_t *dev, wm_lcd_rotate_t degree)
{
    int ret                   = WM_ERR_SUCCESS;
    wm_drv_tft_lcd_spi_t *drv = NULL;

    uint8_t rotate = 0x0;

    if ((!dev->drv) || (!dev->hw)) {
        return WM_ERR_INVALID_PARAM;
    }

    drv = (wm_drv_tft_lcd_spi_t *)dev->drv;

    WM_DRV_TFT_LCD_LOCK(drv->mutex);

    switch (degree) {
        case LCD_ROTATION_90_DEGREE:
        {
            rotate = GC9A01_CFG_MADCTL_MY | GC9A01_CFG_MADCTL_MV | GC9A01_CFG_MADCTL_MX;
            break;
        }
        case LCD_ROTATION_180_DEGREE:
        {
            rotate = GC9A01_CFG_MADCTL_MY;
            break;
        }
        case LCD_ROTATION_270_DEGREE:
        {
            rotate = GC9A01_CFG_MADCTL_MV;
            break;
        }
        default: // case LCD_ROTATION_NONE:
        {
            rotate = GC9A01_CFG_MADCTL_MX;
            break;
        }
    }

    rotate |= GC9A01_CFG_MADCTL_BGR;

    ret = lcd_send_command(dev, GC9A01_MADCTL, LCD_CMD_TYPE_8BIT);
    WM_DRV_LCD_TFT_CHECK_RETURN(ret);

    ret = lcd_send_data(dev, &rotate, 1);
    WM_DRV_LCD_TFT_CHECK_RETURN(ret);

    drv->panel_cfg.rotation = degree;

    if (degree == LCD_ROTATION_90_DEGREE || degree == LCD_ROTATION_270_DEGREE) {
        drv->panel_cfg.x_resolution = WM_CFG_TFT_LCD_Y_RESOLUTION;
        drv->panel_cfg.y_resolution = WM_CFG_TFT_LCD_X_RESOLUTION;
    } else {
        drv->panel_cfg.x_resolution = WM_CFG_TFT_LCD_X_RESOLUTION;
        drv->panel_cfg.y_resolution = WM_CFG_TFT_LCD_Y_RESOLUTION;
    }

    WM_DRV_TFT_LCD_UNLOCK(drv->mutex);
    return ret;
}

static int wm_drv_ops_gc9a01_spi_get_caps(wm_device_t *dev, wm_lcd_capabilitys_t *cap)
{
    int ret                   = WM_ERR_SUCCESS;
    wm_drv_tft_lcd_spi_t *drv = NULL;

    if (!dev->drv || !dev->hw) {
        return WM_ERR_INVALID_PARAM;
    }

    drv = (wm_drv_tft_lcd_spi_t *)dev->drv;

    cap->rotation     = (wm_lcd_rotate_t)drv->panel_cfg.rotation;
    cap->x_resolution = drv->panel_cfg.x_resolution;
    cap->y_resolution = drv->panel_cfg.y_resolution;

    return ret;
}

static int wm_drv_ops_gc9a01_spi_cmd_data(wm_device_t *dev, uint16_t cmd, wm_lcd_cmd_type_t cmd_type, uint8_t *data,
                                          uint16_t data_len)
{
    int ret                   = WM_ERR_SUCCESS;
    wm_drv_tft_lcd_spi_t *drv = NULL;

    if (!dev->drv) {
        return WM_ERR_INVALID_PARAM;
    }

    drv = (wm_drv_tft_lcd_spi_t *)dev->drv;

    WM_DRV_TFT_LCD_LOCK(drv->mutex);

    ret = lcd_send_command(dev, cmd, cmd_type);
    WM_DRV_LCD_TFT_CHECK_GOTO(ret, exit);

    if (data != NULL && data_len != 0) {
        ret = lcd_send_data(dev, data, data_len);
        WM_DRV_LCD_TFT_CHECK_GOTO(ret, exit);
    }

exit:
    WM_DRV_TFT_LCD_UNLOCK(drv->mutex);
    return ret;
}

int wm_drv_ops_gc9a01_add_tx_cb(wm_device_t *dev, wm_lcd_tx_callback_t callback, void *usr_data)
{
    int ret                   = WM_ERR_SUCCESS;
    wm_drv_tft_lcd_spi_t *drv = NULL;

    if (dev == NULL || callback == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    drv = (wm_drv_tft_lcd_spi_t *)dev->drv;
    WM_DRV_LCD_TFT_CHECK_PARAM(drv);

    drv->tx_done_cb = callback;
    drv->usr_data   = usr_data;

    return ret;
}

int wm_drv_ops_gc9a01_remove_tx_cb(wm_device_t *dev)
{
    int ret                   = WM_ERR_SUCCESS;
    wm_drv_tft_lcd_spi_t *drv = NULL;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    drv = (wm_drv_tft_lcd_spi_t *)dev->drv;

    drv->tx_done_cb = NULL;
    drv->usr_data   = NULL;

    return ret;
}

const wm_drv_tft_lcd_ops_t wm_drv_tft_lcd_gc9a01_ops = {
    .lcd_init               = wm_drv_ops_gc9a01_spi_init,
    .lcd_deinit             = wm_drv_ops_gc9a01_spi_deinit,
    .draw_bitmap            = wm_drv_ops_gc9a01_spi_draw_bitmap,
    .lcd_set_backlight      = wm_drv_ops_gc9a01_spi_set_bl,
    .lcd_set_rotation       = wm_drv_ops_gc9a01_spi_set_rotation,
    .lcd_get_capability     = wm_drv_ops_gc9a01_spi_get_caps,
    .lcd_cmd_data           = wm_drv_ops_gc9a01_spi_cmd_data,
    .lcd_add_tx_callback    = wm_drv_ops_gc9a01_add_tx_cb,
    .lcd_remove_tx_callback = wm_drv_ops_gc9a01_remove_tx_cb,
};
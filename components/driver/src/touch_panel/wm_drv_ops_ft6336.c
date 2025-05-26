/**
 * @file wm_drv_ops_ft6336.c
 *
 * @brief driver ft6336 Module
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

#include <stdio.h>
#include <string.h>
#include "wmsdk_config.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wm_drv_i2c.h"
#include "wm_drv_gpio.h"
#include "wm_drv_touch_panel.h"
#include "wm_error.h"
#include "wm_log.h"
#include "wm_osal.h"
// Change the current path of wm-d t_hw. h to the path in your own project
#include "wm_dt_hw.h"
#include "wm_soc_cfgs.h"
#include "wm_drv_touch_panel_cal.h"
#include "wm_ft6336.h"

typedef struct {
    wm_os_mutex_t *drv_mutex;
    wm_device_t *i2c_controler;
} wm_drv_ft6336_drv_t;

typedef struct {
    char *name;                    // device name
    wm_dt_hw_touch_panel_t *hw;    // hardware info
    wm_drv_touch_panel_ops_t *ops; // device operation interface
    wm_drv_ft6336_drv_t *drv;      // driver context data
    wm_device_status_t state;      // device state
    void *priv;                    // user private data
} wm_drv_ft6336_dev_t;

// Define a structure to hold touch coordinates and status
typedef struct {
    uint16_t x[FT6336_TOUCH_MAX_POINT_NUMBER]; // X coordinate of the touch point
    uint16_t y[FT6336_TOUCH_MAX_POINT_NUMBER]; // Y coordinate of the touch point
} ft6336_touch_data_t;

// Static instance of the touch data structure
static ft6336_touch_data_t g_touch_data;

static uint16_t ft6336_convert_address(uint32_t reg_addr, uint8_t *addr)
{
    if (I2C_ADDRESS_WIDTH > 8) {
        addr[0] = (uint8_t)((reg_addr >> 8) & WM_BIT_MASK(I2C_ADDRESS_WIDTH - 8));
        addr[1] = (uint8_t)(reg_addr & 0xff);
    } else {
        addr[0] = reg_addr & WM_BIT_MASK(I2C_ADDRESS_WIDTH);
    }

    return I2C_ADDRESS + (reg_addr >> I2C_ADDRESS_WIDTH);
}

static int ft6336_read(wm_device_t *ft6336_device, uint32_t reg_addr, uint8_t *buf, uint8_t read_len)
{
    wm_drv_ft6336_dev_t *dev        = (wm_drv_ft6336_dev_t *)ft6336_device;
    wm_drv_ft6336_drv_t *device_drv = dev->drv;
    uint8_t addr[2];
    uint16_t i2c_addr;
    int ret = WM_ERR_SUCCESS;

    wm_drv_i2c_config_t config = {
        .addr     = I2C_ADDRESS,
        .speed_hz = FT_CHIP_SPEED,
    };

    WM_DRV_FT6336_CHECK_FAIL(device_drv->drv_mutex);
    WM_DRV_FT6336_CHECK_FAIL(device_drv->i2c_controler);

    WM_DRV_FT6336_LOCK(device_drv->drv_mutex);

    i2c_addr    = ft6336_convert_address(reg_addr, addr);
    config.addr = i2c_addr;

    ret = wm_drv_i2c_read(device_drv->i2c_controler, &config, addr, WM_DRV_FT6336_DIV_ROUND_UP(I2C_ADDRESS_WIDTH, 8), buf,
                          read_len);

    WM_DRV_FT6336_UNLOCK(device_drv->drv_mutex);

    return ret;
}

static void ft6336_reset(wm_device_t *ft6336_device)
{
    wm_drv_ft6336_dev_t *dev   = (wm_drv_ft6336_dev_t *)ft6336_device;
    wm_dt_hw_touch_panel_t *hw = dev->hw;

    wm_drv_gpio_data_reset(hw->reset_pin.pin_num);
    vTaskDelay(pdMS_TO_TICKS(20));
    wm_drv_gpio_data_set(hw->reset_pin.pin_num);
    vTaskDelay(pdMS_TO_TICKS(50));
}

static int ft6336_touch_init(wm_device_t *ft6336_device)
{
    int ret                  = WM_ERR_SUCCESS;
    wm_drv_ft6336_dev_t *dev = (wm_drv_ft6336_dev_t *)ft6336_device;
    wm_drv_ft6336_drv_t *device_drv;
    wm_dt_hw_touch_panel_t *hw = dev->hw;
    wm_os_mutex_t *mutex;
    wm_drv_touch_panel_set_cfg_t *dev_scfg;
    wm_device_t *i2c_controler;

    if (dev->state == WM_DEV_ST_INITED) {
        // initialized before
        return WM_ERR_ALREADY_INITED;
    }

    // Allocate memory
    dev_scfg = (wm_drv_touch_panel_set_cfg_t *)malloc(sizeof(wm_drv_touch_panel_set_cfg_t));
    //Check if memory allocation is successful
    if (dev_scfg == NULL) {
        wm_log_error("dev_scfg failed to allocate space\n");
        return WM_ERR_FAILED;
    }

    device_drv = (wm_drv_ft6336_drv_t *)malloc(sizeof(wm_drv_ft6336_drv_t));
    //Check if memory allocation is successful
    if (device_drv == NULL) {
        wm_log_error("device_drv failed to allocate space\n");
        return WM_ERR_FAILED;
    }

    // create mutex for api calling
    if (wm_os_internal_recursive_mutex_create(&mutex) != WM_OS_STATUS_SUCCESS) {
        return WM_ERR_FAILED;
    }

    i2c_controler = wm_dt_get_device_by_name(hw->if_dev_name);

    dev->priv                 = (wm_drv_touch_panel_set_cfg_t *)dev_scfg;
    dev->drv                  = (wm_drv_ft6336_drv_t *)device_drv;
    device_drv->drv_mutex     = (void *)mutex;
    device_drv->i2c_controler = i2c_controler;

    // Communication protocol initialization
    if (strstr(hw->if_dev_name, "i2c") != NULL) {
        wm_drv_i2c_init(hw->if_dev_name);
    }

    // Initialize reset pin
    if (WM_ERR_SUCCESS != (ret = wm_drv_gpio_iomux_func_sel(hw->reset_pin.pin_num, hw->reset_pin.pin_mux)) ||
        WM_ERR_SUCCESS != (ret = wm_drv_gpio_set_dir(hw->reset_pin.pin_num, hw->reset_pin.pin_dir)) ||
        WM_ERR_SUCCESS != (ret = wm_drv_gpio_set_pullmode(hw->reset_pin.pin_num, hw->reset_pin.pin_pupd))) {
        wm_log_error("%d cfg ret %d", hw->reset_pin.pin_num, ret);
    }

    // Reset the FT6336 chip
    ft6336_reset((wm_device_t *)ft6336_device);
    dev->state = WM_DEV_ST_INITED;

    return ret;
}

static int ft6336_touch_deinit(wm_device_t *ft6336_device)
{
    int ret                                = WM_ERR_SUCCESS;
    wm_drv_ft6336_dev_t *dev               = (wm_drv_ft6336_dev_t *)ft6336_device;
    wm_drv_touch_panel_set_cfg_t *dev_scfg = dev->priv;
    wm_drv_ft6336_drv_t *device_drv        = dev->drv;

    WM_DRV_FT6336_CHECK_FAIL(dev_scfg);
    WM_DRV_FT6336_CHECK_FAIL(device_drv);

    if (dev->state == WM_DEV_ST_INITED && device_drv->drv_mutex && dev->priv) {
        // Reset all members to zero
        g_touch_data = (ft6336_touch_data_t){ 0 };
        wm_os_internal_recursive_mutex_delete((wm_os_mutex_t *)device_drv->drv_mutex);
        device_drv->drv_mutex     = NULL;
        device_drv->i2c_controler = NULL;
        free(dev_scfg);
        dev_scfg = NULL;
        free(device_drv);
        device_drv = NULL;
        dev->state = WM_DEV_ST_UNINIT;
    } else {
        ret = WM_ERR_FAILED;
    }

    return ret;
}

static int ft6336_touch_set_config(wm_device_t *ft6336_device, wm_drv_touch_panel_set_cfg_t *ft6336_device_scfg)
{
    wm_drv_ft6336_dev_t *dev               = (wm_drv_ft6336_dev_t *)ft6336_device;
    wm_drv_touch_panel_set_cfg_t *dev_scfg = dev->priv;

    WM_DRV_FT6336_CHECK_FAIL(dev_scfg);

    if (dev_scfg == NULL) {
        return WM_ERR_FAILED;
    }

    if (ft6336_device_scfg->rotation >= 0 && ft6336_device_scfg->rotation <= 3) {
        dev_scfg->rotation = ft6336_device_scfg->rotation;
    } else {
        return WM_ERR_FAILED;
    }

    return WM_ERR_SUCCESS;
}

static int ft6336_touch_get_config(wm_device_t *ft6336_device, wm_drv_touch_panel_get_cfg_t *ft6336_device_gcfg)
{
    wm_drv_ft6336_dev_t *dev               = (wm_drv_ft6336_dev_t *)ft6336_device;
    wm_drv_touch_panel_set_cfg_t *dev_scfg = dev->priv;
    wm_dt_hw_touch_panel_t *hw             = dev->hw;

    WM_DRV_FT6336_CHECK_FAIL(dev_scfg);
    WM_DRV_FT6336_CHECK_FAIL(hw);

    ft6336_device_gcfg->rotation = dev_scfg->rotation;
    ft6336_device_gcfg->height   = hw->height;
    ft6336_device_gcfg->width    = hw->width;

    return WM_ERR_SUCCESS;
}

static int ft6336_touch_get_coords(wm_device_t *ft6336_device, wm_drv_touch_panel_points_t *ft6336_tp_points)
{
    wm_drv_touch_panel_points_t *tp_points                = (wm_drv_touch_panel_points_t *)ft6336_tp_points;
    wm_drv_ft6336_dev_t *dev                              = (wm_drv_ft6336_dev_t *)ft6336_device;
    wm_drv_touch_panel_set_cfg_t *dev_scfg                = dev->priv;
    wm_dt_hw_touch_panel_t *hw                            = dev->hw;
    uint8_t touch_data[FT6336_TOUCH_MAX_POINT_NUMBER * 2] = { 0 };

    WM_DRV_FT6336_CHECK_FAIL(dev_scfg);
    WM_DRV_FT6336_CHECK_FAIL(hw);

    // Read the number of touch points
    if (ft6336_read((wm_device_t *)dev, FT_REG_NUM_FINGER, &tp_points->cur_points, 1) == WM_ERR_FAILED) {
        tp_points->cur_points = 0;
        return WM_ERR_FAILED;
    }

    if (tp_points->cur_points > FT6336_TOUCH_MAX_POINT_NUMBER) {
        tp_points->cur_points = 0;
        return WM_ERR_FAILED;
    }

    // Set initial value 0xFF
    for (int i = 0; i < FT6336_TOUCH_MAX_POINT_NUMBER; ++i) {
        tp_points->curx[i] = 0xFF;
        tp_points->cury[i] = 0xFF;
    }

    // Read data for the first touch point
    ft6336_read((wm_device_t *)dev, FT_TP1_REG, touch_data, 4);
    g_touch_data.x[0] = ((uint16_t)(touch_data[0] & 0x0F) << 8) + touch_data[1];
    g_touch_data.y[0] = ((uint16_t)(touch_data[2] & 0x0F) << 8) + touch_data[3];

    // Read data for the second touch point
    ft6336_read((wm_device_t *)dev, FT_TP2_REG, touch_data, 4);
    g_touch_data.x[1] = ((uint16_t)(touch_data[0] & 0x0F) << 8) + touch_data[1];
    g_touch_data.y[1] = ((uint16_t)(touch_data[2] & 0x0F) << 8) + touch_data[3];

    // Coordinate rotation processing
    for (int i = 0; i < tp_points->cur_points; ++i) {
        // mirror symmetry processing
        wm_drv_tp_adjust_mirror_coord(hw, &g_touch_data.x[i], &g_touch_data.y[i]);
        // coordinate axis rotation processing
        wm_drv_tp_adjust_rotate_coord(hw, dev_scfg->rotation, &g_touch_data.x[i], &g_touch_data.y[i]);
        tp_points->curx[i] = g_touch_data.x[i];
        tp_points->cury[i] = g_touch_data.y[i];
    }

    return WM_ERR_SUCCESS;
}

/*
FT6336 api ops
*/
const wm_drv_touch_panel_ops_t wm_drv_ft6336_ops = {
    .init       = ft6336_touch_init,
    .deinit     = ft6336_touch_deinit,
    .set_cfg    = ft6336_touch_set_config,
    .get_cfg    = ft6336_touch_get_config,
    .get_coords = ft6336_touch_get_coords,
};
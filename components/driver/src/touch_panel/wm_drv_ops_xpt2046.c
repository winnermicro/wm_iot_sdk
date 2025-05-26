/**
 * @file wm_drv_ops_xpt2046.c
 *
 * @brief driver xpt2046 Module
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
#include "freertos/queue.h"
#include "freertos/task.h"
#include "wm_drv_gpio.h"
#include "wm_log.h"
#include "wm_drv_spi_master.h"
#include "wm_dt_hw.h"
#include "wm_drv_touch_panel.h"
#include "wm_soc_cfgs.h"
#include "wm_drv_touch_panel_cal.h"
#include "wm_xpt2046.h"

typedef struct {
    bool is_pressed;
    uint16_t x;
    uint16_t y;
} xpt2046_touch_data_t;

typedef struct {
    wm_os_mutex_t *drv_mutex;
    wm_device_t *spi_controler;
    wm_dt_hw_spim_dev_cfg_t *spi_config; // spi config
} wm_drv_xpt2046_drv_t;

typedef struct {
    bool cal_done;
    xpt2046_touch_data_t *touch_data; // xpt2046 driver internal use
    wm_drv_touch_panel_set_cfg_t *scfg;
} wm_drv_xpt2046_prv_t;

typedef struct {
    char *name;                    // device name
    wm_dt_hw_touch_panel_t *hw;    // hardware info
    wm_drv_touch_panel_ops_t *ops; // device operation interface
    wm_drv_xpt2046_drv_t *drv;     // driver context data
    wm_device_status_t state;      // device state
    wm_drv_xpt2046_prv_t *priv;    // user private data
} wm_drv_xpt2046_dev_t;

static int xpt2046_read(wm_device_t *xpt2046_device, uint8_t cmd, uint16_t *data)
{
    wm_drv_xpt2046_dev_t *dev = (wm_drv_xpt2046_dev_t *)xpt2046_device;
    spim_transceive_t desc    = { .flags = SPI_TRANS_BIG_ENDIAN };

    uint8_t tx_buf[XPT_CMD_SWAP_NUM] = { cmd, 0, 0 };
    uint8_t rx_buf[XPT_CMD_SWAP_NUM] = { 0, 0, 0 };

    desc.rx_buf = rx_buf;
    desc.rx_len = XPT_CMD_SWAP_NUM;
    desc.tx_buf = tx_buf;
    desc.tx_len = XPT_CMD_SWAP_NUM;

    WM_DRV_XPT2046_CHECK_FAIL(dev->drv->drv_mutex);
    WM_DRV_XPT2046_CHECK_FAIL(dev->drv->spi_controler);
    WM_DRV_XPT2046_CHECK_FAIL(dev->drv->spi_config);

    WM_DRV_XPT2046_LOCK(dev->drv->drv_mutex);
    wm_drv_spim_transceive_sync(dev->drv->spi_controler, dev->drv->spi_config, &desc, 10000);
    WM_DRV_XPT2046_UNLOCK(dev->drv->drv_mutex);
    *data = (((rx_buf[1] << 8) | rx_buf[2]) >> 3);
    return WM_ERR_SUCCESS;
}

// read the original coordinates
static int xpt2046_raw_coord(wm_device_t *xpt2046_device, uint16_t *x, uint16_t *y)
{
    uint16_t temp_x, temp_y;
    xpt2046_read(xpt2046_device, XPT_CMD_X_COORDINATE, &temp_x);
    xpt2046_read(xpt2046_device, XPT_CMD_Y_COORDINATE, &temp_y);

    *x = temp_x;
    *y = temp_y;
    return WM_ERR_SUCCESS;
}

// pressure value detection
int xpt2046_is_pressed(wm_device_t *xpt2046_device)
{
    uint16_t z1;

    xpt2046_read(xpt2046_device, XPT_CMD_PRESSURE_Z1, &z1);

    if (z1 < XPT2046_THRESHOLD_Z1)
        return WM_ERR_FAILED;
    else
        return WM_ERR_SUCCESS;
}

static int xpt2046_touch_init(wm_device_t *xpt2046_device)
{
    int ret                    = WM_ERR_SUCCESS;
    wm_drv_xpt2046_dev_t *dev  = (wm_drv_xpt2046_dev_t *)xpt2046_device;
    wm_dt_hw_touch_panel_t *hw = dev->hw;
    wm_device_t *spi_controler;
    wm_os_mutex_t *mutex;

    if (!dev->hw) {
        wm_log_error("Invalid hardware config");
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state == WM_DEV_ST_INITED) {
        // initialized before
        return WM_ERR_ALREADY_INITED;
    }

    // create mutex for api calling
    if (wm_os_internal_recursive_mutex_create(&mutex) != WM_OS_STATUS_SUCCESS) {
        return WM_ERR_FAILED;
    }

    // Allocate memory
    dev->priv             = (wm_drv_xpt2046_prv_t *)malloc(sizeof(wm_drv_xpt2046_prv_t));
    dev->drv              = (wm_drv_xpt2046_drv_t *)malloc(sizeof(wm_drv_xpt2046_drv_t));
    dev->drv->spi_config  = (wm_dt_hw_spim_dev_cfg_t *)malloc(sizeof(wm_dt_hw_spim_dev_cfg_t));
    dev->priv->touch_data = (xpt2046_touch_data_t *)malloc(sizeof(xpt2046_touch_data_t));
    if ((!dev->priv) || (!dev->drv) || (!dev->drv->spi_config) || (!dev->priv->touch_data)) {
        wm_log_error("memory allocation failed");
        ret = WM_ERR_NO_MEM;
        goto cleanup;
    }

    // Communication protocol initialization
    spi_controler = wm_dt_get_device_by_name(hw->if_dev_name);
    if ((spi_controler != NULL) && (spi_controler->state != WM_DEV_ST_INITED)) {
        spi_controler = wm_drv_spim_init(hw->if_dev_name);
        if (spi_controler == NULL) {
            wm_log_info("SPI controler %s not found or not initialized", hw->if_dev_name);
            ret = WM_ERR_FAILED;
            goto cleanup;
        }
    }

    dev->drv->spi_controler = spi_controler;
    dev->drv->drv_mutex     = (void *)mutex;
    *dev->drv->spi_config   = hw->spi_cfg;
    dev->priv->cal_done     = false;

    //default CS is 1
    ret = wm_drv_gpio_data_set(dev->drv->spi_config->pin_cs.pin_num);

    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("xpt2046 GPIO init failed!");
        ret = WM_ERR_FAILED;
        goto cleanup;
    }

    dev->state = WM_DEV_ST_INITED;
    return WM_ERR_SUCCESS;

cleanup:
    if (dev->priv) {
        if (dev->priv->touch_data) {
            free(dev->priv->touch_data);
            dev->priv->touch_data = NULL;
        }
        free(dev->priv);
        dev->priv = NULL;
    }
    if (dev->drv) {
        if (dev->drv->spi_config) {
            free(dev->drv->spi_config);
            dev->drv->spi_config = NULL;
        }
        free(dev->drv);
        dev->drv = NULL;
    }
    return ret;
}

static int xpt2046_touch_deinit(wm_device_t *xpt2046_device)
{
    wm_drv_xpt2046_dev_t *dev = (wm_drv_xpt2046_dev_t *)xpt2046_device;
    WM_DRV_XPT2046_CHECK_FAIL(dev->priv);
    WM_DRV_XPT2046_CHECK_FAIL(dev->drv);
    if (dev->state == WM_DEV_ST_INITED && dev->drv && dev->priv) {
        dev->priv->touch_data = (xpt2046_touch_data_t *){ 0 };
        if (dev->drv) {
            wm_os_internal_recursive_mutex_delete(dev->drv->drv_mutex);
            dev->drv->drv_mutex     = NULL;
            dev->drv->spi_controler = NULL;
            if (dev->drv->spi_config) {
                free(dev->drv->spi_config);
                dev->drv->spi_config = NULL;
            }
            free(dev->drv);
            dev->drv = NULL;
        }
        if (dev->priv->touch_data) {
            free(dev->priv->touch_data);
            dev->priv->touch_data = NULL;
        }
        free(dev->priv);
        dev->priv  = NULL;
        dev->state = WM_DEV_ST_UNINIT;
        return WM_ERR_SUCCESS;
    }
    return WM_ERR_FAILED;
}

static int xpt2046_touch_set_config(wm_device_t *xpt2046_device, wm_drv_touch_panel_set_cfg_t *xpt2046_device_scfg)
{
    wm_drv_xpt2046_dev_t *dev = (wm_drv_xpt2046_dev_t *)xpt2046_device;
    WM_DRV_XPT2046_CHECK_FAIL(dev->priv);

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_FAILED;
    }

    if (dev->priv == NULL) {
        return WM_ERR_FAILED;
    }

    if (xpt2046_device_scfg->rotation < 0 || xpt2046_device_scfg->rotation > 3) {
        return WM_ERR_FAILED;
    }

    dev->priv->cal_done = xpt2046_device_scfg->cal_done;
    dev->priv->scfg     = xpt2046_device_scfg;

    return WM_ERR_SUCCESS;
}

static int xpt2046_touch_get_config(wm_device_t *xpt2046_device, wm_drv_touch_panel_get_cfg_t *xpt2046_device_gcfg)
{
    wm_drv_xpt2046_dev_t *dev = (wm_drv_xpt2046_dev_t *)xpt2046_device;
    WM_DRV_XPT2046_CHECK_FAIL(dev->priv);
    WM_DRV_XPT2046_CHECK_FAIL(dev->hw);

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_FAILED;
    }

    xpt2046_device_gcfg->rotation = dev->priv->scfg->rotation;
    xpt2046_device_gcfg->width    = dev->hw->width;
    xpt2046_device_gcfg->height   = dev->hw->height;
    memcpy(&xpt2046_device_gcfg->cal_data, &dev->priv->scfg->cal_data, sizeof(xpt2046_device_gcfg->cal_data));

    return WM_ERR_SUCCESS;
}

// get real coordinates
static int xpt2046_touch_get_coords(wm_device_t *xpt2046_device, wm_drv_touch_panel_points_t *xpt2046_tp_points)
{
    wm_drv_xpt2046_dev_t *dev               = (wm_drv_xpt2046_dev_t *)xpt2046_device;
    wm_drv_touch_panel_points_t *tp_points  = (wm_drv_touch_panel_points_t *)xpt2046_tp_points;
    xpt2046_touch_data_t *touch_data        = dev->priv->touch_data;
    wm_drv_touch_panel_set_cfg_t *priv_scfg = dev->priv->scfg;

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_FAILED;
    }

    if (xpt2046_is_pressed(xpt2046_device) == WM_ERR_FAILED) {
        touch_data->is_pressed = false;
        tp_points->cur_points  = 0;
        return WM_ERR_FAILED;
    }

    uint16_t raw_x, raw_y;
    if (xpt2046_raw_coord((wm_device_t *)dev, &raw_x, &raw_y) == 0) {
        if (dev->priv->cal_done == true) {
            touch_data->x = raw_x * priv_scfg->cal_data.ax + raw_y * priv_scfg->cal_data.bx + priv_scfg->cal_data.cx;
            touch_data->y = raw_x * priv_scfg->cal_data.ay + raw_y * priv_scfg->cal_data.by + priv_scfg->cal_data.cy;
            if (touch_data->x > dev->hw->width || touch_data->y > dev->hw->height) {
                return WM_ERR_FAILED;
            }
            // mirror symmetry processing
            wm_drv_tp_adjust_mirror_coord(dev->hw, &touch_data->x, &touch_data->y);
            // coordinate axis rotation processing
            wm_drv_tp_adjust_rotate_coord(dev->hw, priv_scfg->rotation, &touch_data->x, &touch_data->y);
        } else {
            touch_data->x = raw_x;
            touch_data->y = raw_y;
        }
    }

    tp_points->cur_points = 1;
    tp_points->curx[0]    = touch_data->x;
    tp_points->cury[0]    = touch_data->y;

    return WM_ERR_SUCCESS;
}

/*
XPT2046 api ops
*/
const wm_drv_touch_panel_ops_t wm_drv_xpt2046_ops = {
    .init       = xpt2046_touch_init,
    .deinit     = xpt2046_touch_deinit,
    .set_cfg    = xpt2046_touch_set_config,
    .get_cfg    = xpt2046_touch_get_config,
    .get_coords = xpt2046_touch_get_coords,
};
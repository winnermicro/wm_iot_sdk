/**
 * @file wm_drv_ops_i2c.c
 *
 * @brief i2c ops Module
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

#include "wmsdk_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wm_types.h"
#include "wm_error.h"
#include "wm_attr.h"

#include "wm_osal.h"

#include "wm_irq.h"
#include "wm_drv_rcc.h"
#include "wm_drv_gpio.h"
#include "wm_drv_irq.h"
#include "wm_hal_i2c.h"
#include "wm_drv_i2c.h"
#include "wm_dt_hw.h"

/**
  * @brief  mutex lock for api
  */
#define WM_DRV_I2C_LOCK(ctx)                                                                               \
    do {                                                                                                   \
        if (wm_os_internal_recursive_mutex_acquire((ctx)->mutex, WM_OS_WAIT_TIME_MAX) != WM_ERR_SUCCESS) { \
            return WM_ERR_FAILED;                                                                          \
        }                                                                                                  \
    } while (0)

/**
  * @brief  mutex unlock for api
  */
#define WM_DRV_I2C_UNLOCK(ctx)      wm_os_internal_recursive_mutex_release((ctx)->mutex)

/**
  * @brief  transfer timeout
  */
#define WM_DRV_I2C_TRANSFER_TIMEOUT 5000

/**
  * @brief  max 7 bits address
  */
#define WM_DRV_I2C_MAX_ADDRESS      0x7F

/**
  * @brief  driver context struct
  */
typedef struct {
    wm_os_mutex_t *mutex; /**< mutex for lock api       */
    uint32_t last_speed;  /**< last speed, unit hz      */
} wm_drv_i2c_ctx_t;

/**
  * @brief  driver data struct
  */
typedef struct {
    wm_hal_i2c_dev_t hal_dev; /**< hal device     */
    wm_drv_i2c_ctx_t ctx;     /**< driver context */
} wm_drv_i2c_data_t;

/**
  * @brief  I2C device struct
  */
typedef struct {
    char *name;               /**< device name                      */
    wm_dt_hw_i2c_t *hw;       /**< hardware info, @ref wm_dt_hw_i2c_t  */
    wm_drv_i2c_ops_t *ops;    /**< device operation interface       */
    wm_drv_i2c_data_t *drv;   /**< driver context data              */
    wm_device_status_t state; /**< device state                     */
    void *priv;               /**< user private data                */
} wm_drv_i2c_dev_t;

static int wm_drv_i2c_pin_config(wm_dt_hw_pin_cfg_t *pin_cfg, int count)
{
    wm_gpio_num_t pin;

    for (int i = 0; i < count; i++) {
        pin = pin_cfg[i].pin_num;
        wm_drv_gpio_iomux_func_sel(pin, pin_cfg[i].pin_mux);
    }

    return WM_ERR_SUCCESS;
}

static int i2c_init(wm_device_t *device)
{
    wm_drv_i2c_dev_t *dev = (wm_drv_i2c_dev_t *)device;
    wm_hal_i2c_dev_t *hal;
    wm_drv_i2c_ctx_t *ctx;
    wm_hal_i2c_config_t cfg;

    uint32_t max_clock;

    if (dev->state == WM_DEV_ST_INITED) {
        /*init before*/
        return WM_ERR_ALREADY_INITED;
    }

    if (!dev->hw->rcc_device_name) {
        return WM_ERR_INVALID_PARAM;
    }

    /*check max clock*/
    max_clock = dev->hw->i2c_cfg.max_clock;
    if (!(max_clock >= WM_I2C_SPEED_MIN && max_clock <= WM_I2C_SPEED_MAX)) {
        return WM_ERR_FAILED;
    }

    dev->drv = calloc(1, sizeof(wm_drv_i2c_data_t));
    if (!dev->drv) {
        return WM_ERR_NO_MEM;
    }

    hal = &(dev->drv->hal_dev);
    ctx = &(dev->drv->ctx);

    /* create mutex for api calling*/
    if (wm_os_internal_recursive_mutex_create(&ctx->mutex) != WM_OS_STATUS_SUCCESS) {
        free(dev->drv);
        return WM_ERR_FAILED;
    }

    /* config SDA and SCL pin*/
    wm_drv_i2c_pin_config(dev->hw->pin_cfg, dev->hw->pin_cfg_count);

    /* enable i2c clock*/
    wm_drv_clock_enable(wm_dt_get_device_by_name(dev->hw->rcc_device_name), WM_RCC_I2C_GATE_EN);

    /* init hal dev attribute */
    hal->reg          = (wm_i2c_reg_t *)(dev->hw->reg_base);
    hal->irq_num      = dev->hw->irq_cfg.irq_num;
    hal->irq_priority = (uint8_t)(dev->hw->irq_cfg.irq_priority);

    /* init config attribute */
    cfg.ref_clock = wm_drv_rcc_get_config_clock(wm_dt_get_device_by_name(dev->hw->rcc_device_name), WM_RCC_TYPE_APB) * UNIT_MHZ;
    cfg.speed     = max_clock;
    cfg.addr_bits = dev->hw->i2c_cfg.addr_10_bits ? 10 : 7;

    /* init hal i2c */
    if (wm_hal_i2c_init(hal, &cfg) != WM_ERR_SUCCESS) {
        wm_os_internal_recursive_mutex_delete(ctx->mutex);
        free(dev->drv);
        dev->drv = NULL;
        return WM_ERR_FAILED;
    }

    /*remember current clock speed*/
    ctx->last_speed = max_clock;
    dev->state      = WM_DEV_ST_INITED;

    return WM_ERR_SUCCESS;
}

static int i2c_deinit(wm_device_t *device)
{
    wm_drv_i2c_dev_t *dev = (wm_drv_i2c_dev_t *)device;

    if (dev->state == WM_DEV_ST_INITED && dev->drv) {
        /*disable clock*/
        wm_drv_clock_disable(wm_dt_get_device_by_name(dev->hw->rcc_device_name), WM_RCC_I2C_GATE_EN);

        /*deinit hal i2c*/
        wm_hal_i2c_deinit(&dev->drv->hal_dev);

        /*delete mutex*/
        wm_os_internal_recursive_mutex_delete(dev->drv->ctx.mutex);
        free(dev->drv);
        dev->drv   = NULL;
        dev->state = WM_DEV_ST_UNINIT;

        return WM_ERR_SUCCESS;
    } else {
        return WM_ERR_FAILED;
    }
}

static int i2c_set_speed_if_change(wm_drv_i2c_config_t *config, wm_drv_i2c_ctx_t *ctx, wm_hal_i2c_dev_t *hal)
{
    int ret = WM_ERR_SUCCESS;

    if (config->speed_hz != ctx->last_speed) {
        ret             = wm_hal_i2c_set_speed(hal, config->speed_hz);
        ctx->last_speed = config->speed_hz;
    }

    return ret;
}

static int i2c_read(wm_device_t *device, wm_drv_i2c_config_t *config, const void *sub_addr, uint32_t sub_addr_size,
                    void *read_buf, uint32_t read_size)
{
    wm_drv_i2c_dev_t *dev = (wm_drv_i2c_dev_t *)(device);
    wm_drv_i2c_ctx_t *ctx;
    wm_hal_i2c_dev_t *hal;
    int ret;

    if (!(dev->drv && config->addr <= WM_DRV_I2C_MAX_ADDRESS)) {
        return WM_ERR_INVALID_PARAM;
    }

    if (!(config->speed_hz >= WM_I2C_SPEED_MIN && config->speed_hz <= dev->hw->i2c_cfg.max_clock)) {
        return WM_ERR_INVALID_PARAM;
    }

    ctx = &dev->drv->ctx;
    hal = &dev->drv->hal_dev;

    WM_DRV_I2C_LOCK(ctx);

    /*check speed change and set*/
    ret = i2c_set_speed_if_change(config, ctx, hal);

    if (ret == WM_ERR_SUCCESS) {
        /*write sub address*/
        if (sub_addr && sub_addr_size > 0) {
            ret = wm_hal_i2c_tx_polling(hal, config->addr, sub_addr, sub_addr_size, WM_HAL_I2C_FLAG_NOSTOP,
                                        WM_DRV_I2C_TRANSFER_TIMEOUT);
        }

        if (ret == WM_ERR_SUCCESS) {
            /*read  data*/
            ret = wm_hal_i2c_rx_polling(hal, config->addr, read_buf, read_size, 0, WM_DRV_I2C_TRANSFER_TIMEOUT);
        }
    }

    WM_DRV_I2C_UNLOCK(ctx);

    return ret;
}

static int i2c_write(wm_device_t *device, wm_drv_i2c_config_t *config, const void *sub_addr, uint32_t sub_addr_size,
                     const void *data, uint32_t data_size)
{
    wm_drv_i2c_dev_t *dev = (wm_drv_i2c_dev_t *)(device);
    wm_drv_i2c_ctx_t *ctx;
    wm_hal_i2c_dev_t *hal;
    int write_flag;
    int ret;

    if (!(dev->drv && config->addr <= WM_DRV_I2C_MAX_ADDRESS)) {
        return WM_ERR_INVALID_PARAM;
    }

    if (!(config->speed_hz >= WM_I2C_SPEED_MIN && config->speed_hz <= dev->hw->i2c_cfg.max_clock)) {
        return WM_ERR_INVALID_PARAM;
    }

    write_flag = ((data && data_size > 0) ? WM_HAL_I2C_FLAG_NOSTOP : 0);
    ctx        = &dev->drv->ctx;
    hal        = &dev->drv->hal_dev;

    WM_DRV_I2C_LOCK(ctx);

    /*check speed change and set*/
    ret = i2c_set_speed_if_change(config, ctx, hal);

    if (ret == WM_ERR_SUCCESS) {
        /*write sub address*/
        ret = wm_hal_i2c_tx_polling(hal, config->addr, sub_addr, sub_addr_size, write_flag, WM_DRV_I2C_TRANSFER_TIMEOUT);
        if (ret == WM_ERR_SUCCESS && write_flag) {
            /*write data*/
            ret =
                wm_hal_i2c_tx_polling(hal, config->addr, data, data_size, WM_HAL_I2C_FLAG_NOSTART, WM_DRV_I2C_TRANSFER_TIMEOUT);
        }
    }

    WM_DRV_I2C_UNLOCK(ctx);

    return ret;
}

/*internal interface ops*/
const wm_drv_i2c_ops_t wm_drv_i2c_ops = {
    .init   = i2c_init,
    .deinit = i2c_deinit,
    .read   = i2c_read,
    .write  = i2c_write,
};

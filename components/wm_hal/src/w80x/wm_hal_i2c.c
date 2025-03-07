/**
 * @file wm_hal_i2c.c
 *
 * @brief hal i2c Module
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
#include <stdlib.h>

#include "wm_types.h"
#include "wm_error.h"
#include "wm_soc.h"

#include "wm_ll_i2c.h"
#include "wm_hal_i2c.h"
#include "wm_hal_irq.h"

#define LOG_TAG "hal_i2c"
#include "wm_log.h"

/**
 * @brief Ack check timeout
 */
#define WM_HAL_I2C_ACK_TIMEOUT 1024

/**
 * @brief Check params
 */
#define WM_HAL_I2C_CHECK_PARAM(p)    \
    if (!(p)) {                      \
        WM_HAL_I2C_TRACE_FAIL();     \
        return WM_ERR_INVALID_PARAM; \
    }

#define WM_HAL_I2C_LOGE         wm_log_error
#define WM_HAL_I2C_TRACE_FAIL() WM_HAL_I2C_LOGE("err line %d", __LINE__)

/**
 * @brief i2c context attibutes
 */
typedef struct {
    uint32_t ref_clock; /**< ref_clock, unit Hz */
    uint32_t speed;     /**< i2c speed ,unit Hz */
    uint8_t addr_bits;  /**< address bits       */
} wm_hal_i2c_ctx_t;

int wm_hal_i2c_init(wm_hal_i2c_dev_t *dev, wm_hal_i2c_config_t *cfg)
{
    wm_hal_i2c_ctx_t *ctx;

    WM_HAL_I2C_CHECK_PARAM(dev && cfg && cfg->ref_clock > 0 && cfg->addr_bits == 7);

    if (dev->ctx) {
        /*initialized before*/
        WM_HAL_I2C_TRACE_FAIL();
        return WM_ERR_ALREADY_INITED;
    }

    ctx = calloc(1, sizeof(wm_hal_i2c_ctx_t));
    if (!ctx) {
        WM_HAL_I2C_TRACE_FAIL();
        return WM_ERR_NO_MEM;
    }

    /*save config params*/
    ctx->addr_bits = cfg->addr_bits;
    ctx->ref_clock = cfg->ref_clock;
    ctx->speed     = cfg->speed;
    dev->ctx       = ctx;

    /*set clock speed*/
    if (wm_hal_i2c_set_speed(dev, ctx->speed) == WM_ERR_SUCCESS) {
        /*enable i2c*/
        wm_ll_i2c_set_enable(dev->reg, 0);
        wm_ll_i2c_set_enable(dev->reg, 1);

        /*disable i2c iterrupt*/
        wm_ll_i2c_disable_int(dev->reg);

        return WM_ERR_SUCCESS;
    } else {
        dev->ctx = NULL;
        free(ctx);
        WM_HAL_I2C_TRACE_FAIL();
        return WM_ERR_INVALID_PARAM;
    }
}

int wm_hal_i2c_deinit(wm_hal_i2c_dev_t *dev)
{
    WM_HAL_I2C_CHECK_PARAM(dev);

    if (dev->ctx) {
        wm_ll_i2c_set_enable(dev->reg, 0);
        free(dev->ctx);
        dev->ctx = NULL;
        return WM_ERR_SUCCESS;
    } else {
        return WM_ERR_FAILED;
    }
}

int wm_hal_i2c_set_speed(wm_hal_i2c_dev_t *dev, wm_i2c_speed_t speed_hz)
{
    wm_hal_i2c_ctx_t *ctx;
    uint32_t div;

    WM_HAL_I2C_CHECK_PARAM(dev && (ctx = dev->ctx) && (speed_hz >= WM_I2C_SPEED_STANDARD && speed_hz <= WM_I2C_SPEED_MAX));

    /*
    Each clk will add half of the clock after frequency division (waiting for the slave to release),
    so the coefficient in the formula should be changed from 5 to 5.5
    */
    div = ctx->ref_clock * 2 / (11 * speed_hz) - 1;
    wm_ll_i2c_set_clock_div_low(dev->reg, div & 0xff);
    wm_ll_i2c_set_clock_div_high(dev->reg, (div >> 8) & 0xff);
    ctx->speed = speed_hz;

    return WM_ERR_SUCCESS;
}

int wm_hal_i2c_get_speed(wm_hal_i2c_dev_t *dev, wm_i2c_speed_t *speed_hz)
{
    wm_hal_i2c_ctx_t *ctx;

    WM_HAL_I2C_CHECK_PARAM(dev && (ctx = dev->ctx) && speed_hz);

    *speed_hz = ctx->speed;

    return WM_ERR_SUCCESS;
}

int wm_hal_i2c_recover(wm_hal_i2c_dev_t *dev)
{
    wm_hal_i2c_ctx_t *ctx;

    WM_HAL_I2C_CHECK_PARAM(dev && (ctx = dev->ctx));

    /*set clock speed*/
    wm_hal_i2c_set_speed(dev, ctx->speed);

    /*enable i2c*/
    wm_ll_i2c_set_enable(dev->reg, 0);
    wm_ll_i2c_set_enable(dev->reg, 1);

    return WM_ERR_SUCCESS;
}

static void wm_hal_i2c_stop(wm_hal_i2c_dev_t *dev)
{
    /*ctrl stop i2c controler*/
    wm_ll_i2c_set_ctrl(dev->reg, WM_LL_I2C_CR_STO);

    /*wait xfer end*/
    while (wm_ll_i2c_is_in_xfer(dev->reg)) {
        ;
    }
}

static void wm_hal_i2c_write_byte(wm_hal_i2c_dev_t *dev, uint8_t data, uint32_t ctrl)
{
    /*send byte*/
    wm_ll_i2c_send_one_byte(dev->reg, data);
    wm_ll_i2c_set_ctrl(dev->reg, ctrl);

    /*wait transfer done*/
    while (wm_ll_i2c_is_in_xfer(dev->reg)) {
        ;
    }
}

static uint8_t wm_hal_i2c_read_byte(wm_hal_i2c_dev_t *dev, uint32_t ctrl)
{
    wm_ll_i2c_set_ctrl(dev->reg, ctrl);

    /*wait transfer done*/
    while (wm_ll_i2c_is_in_xfer(dev->reg)) {
        ;
    }

    /*receive byte*/
    return wm_ll_i2c_recieve_one_byte(dev->reg);
}

static int wm_hal_i2c_wait_ack(wm_hal_i2c_dev_t *dev)
{
    uint32_t err_times = 0;

    /*wait transfer done*/
    while (wm_ll_i2c_is_in_xfer(dev->reg)) {
        ;
    }

    /*wait read ack*/
    while (wm_ll_i2c_get_state_rx_ack(dev->reg)) {
        err_times++;
        if (err_times > WM_HAL_I2C_ACK_TIMEOUT) {
            wm_hal_i2c_stop(dev);
            return WM_ERR_FAILED;
        }
    }

    return WM_ERR_SUCCESS;
}

static int wm_hal_i2c_send_address(wm_hal_i2c_dev_t *dev, uint16_t dev_addr, uint32_t flag)
{
    int ret = WM_ERR_SUCCESS;
    if (!(flag & WM_HAL_I2C_FLAG_NOSTART)) {
        /*start, send write address*/
        wm_hal_i2c_write_byte(dev, dev_addr, WM_LL_I2C_CR_STA | WM_LL_I2C_CR_WR);
        if (wm_hal_i2c_wait_ack(dev) != WM_ERR_SUCCESS) {
            WM_HAL_I2C_TRACE_FAIL();
            ret = WM_ERR_FAILED;
        }
    }

    return ret;
}

int wm_hal_i2c_tx_polling(wm_hal_i2c_dev_t *dev, uint16_t dev_addr, const uint8_t *data, uint32_t size, uint32_t flag,
                          uint32_t timeout)
{
    int ret = WM_ERR_SUCCESS;

    wm_hal_i2c_ctx_t *ctx;

    WM_HAL_I2C_CHECK_PARAM(dev && (ctx = dev->ctx) && dev_addr < 0x80 && data && size > 0);

    ret = wm_hal_i2c_send_address(dev, dev_addr << 1, flag);

    if (ret == WM_ERR_SUCCESS) {
        /*write datas*/
        while (size > 0) {
            /*write byte*/
            wm_hal_i2c_write_byte(dev, *data++, WM_LL_I2C_CR_WR);

            /*wait ack*/
            if (wm_hal_i2c_wait_ack(dev) != WM_ERR_SUCCESS) {
                ret = WM_ERR_FAILED;
                WM_HAL_I2C_TRACE_FAIL();
                break;
            }
            size--;
        }
    }

    if ((!(flag & WM_HAL_I2C_FLAG_NOSTOP)) || (ret != WM_ERR_SUCCESS)) {
        /*need stop*/
        wm_hal_i2c_stop(dev);
    }

    return ret;
}

int wm_hal_i2c_rx_polling(wm_hal_i2c_dev_t *dev, uint16_t dev_addr, uint8_t *buffer, uint32_t size, uint32_t flag,
                          uint32_t timeout)
{
    int ret = WM_ERR_SUCCESS;

    wm_hal_i2c_ctx_t *ctx;
    uint32_t ctrl = WM_LL_I2C_CR_RD;

    WM_HAL_I2C_CHECK_PARAM(dev && (ctx = dev->ctx) && dev_addr < 0x80 && buffer && size > 0);

    /*send read address*/
    ret = wm_hal_i2c_send_address(dev, (dev_addr << 1) | 1, flag);

    while (size > 1) {
        /*read bytes*/
        *buffer++ = wm_hal_i2c_read_byte(dev, ctrl);
        size--;
    }

    /*set last byte NAK, stop*/
    ctrl |= WM_LL_I2C_CR_NAK;
    if (!(flag & WM_HAL_I2C_FLAG_NOSTOP)) {
        ctrl |= WM_LL_I2C_CR_STO;
    }

    /*read last byte*/
    *buffer++ = wm_hal_i2c_read_byte(dev, ctrl);

    return ret;
}

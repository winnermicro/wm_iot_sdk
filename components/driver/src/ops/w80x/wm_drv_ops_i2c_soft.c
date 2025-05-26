/**
 * @file wm_drv_ops_i2c_soft.c
 *
 * @brief i2c_soft ops Module.
 * Implement the I2C driver through software simulation using GPIO.
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
#include "wm_hal_irq.h"
#include "wm_drv_i2c.h"
#include "wm_dt_hw.h"
#define LOG_TAG "i2c_soft"
#include "wm_log.h"

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
 * Utilize a critical area to safeguard the byte writing and byte reading operations.
 */
#define SOFT_I2C_ENTER_CRITIAL()    wm_hal_irq_set_critical()
#define SOFT_I2C_EXIT_CRITIAL()     wm_hal_irq_release_critical()

/**
  * @brief  transfer timeout
  */
#define WM_DRV_I2C_TRANSFER_TIMEOUT 5000

/**
  * @brief  max 7 bits address
  */
#define WM_DRV_I2C_MAX_ADDRESS      0x7F

/**
 * @brief Local api types
 */
typedef struct {
    wm_os_mutex_t *mutex; /**< mutex for lock api       */
    wm_gpio_num_t scl;    /**< scl pin                  */
    wm_gpio_num_t sda;    /**< sda pin                  */
    uint32_t last_speed;  /**< last speed, unit hz      */
    int delay_us;         /**< delay time, us           */
} wm_drv_i2c_ctx_t;

/**
  * @brief  driver data struct
  */
typedef struct {
    wm_drv_i2c_ctx_t ctx; /**< driver context */
} wm_drv_i2c_data_t;

/**
  * @brief  I2C device struct
  */
typedef struct {
    char *name;               /**< device name                          */
    wm_dt_hw_i2c_t *hw;       /**< hardware info, @ref wm_dt_hw_i2c_t   */
    wm_drv_i2c_ops_t *ops;    /**< device operation interface           */
    wm_drv_i2c_data_t *drv;   /**< driver context data                  */
    wm_device_status_t state; /**< device state                         */
    void *priv;               /**< user private data                    */
} wm_drv_i2c_dev_t;

extern void udelay(uint32_t us); /* TODO use header file */

/**
 * @brief Calculate the delay time based on the frequency
 * @param freq Frequency in Hz
 * @return Delay time in microseconds
 */
static uint32_t i2c_soft_freq_to_delay(int freq)
{
    if (freq > WM_I2C_SPEED_STANDARD) {
        return 2;
    } else if (freq == WM_I2C_SPEED_STANDARD) {
        return 5;
    } else {
        return 10;
    }
}

/**
 * @brief Set the SCL pin level and delay
 * @param ctx Pointer to the I2C context
 * @param value Pin level value
 * @param delay_us Delay time in microseconds
 */
static inline void i2c_soft_set_scl(wm_drv_i2c_ctx_t *ctx, int value, int delay_us)
{
    wm_drv_gpio_fast_write(ctx->scl, value);
    udelay(delay_us);
}

/**
 * @brief Set the SDA pin level and delay
 * @param ctx Pointer to the I2C context
 * @param value Pin level value
 * @param delay_us Delay time in microseconds
 */
static inline void i2c_soft_set_sda(wm_drv_i2c_ctx_t *ctx, int value, int delay_us)
{
    wm_drv_gpio_fast_write(ctx->sda, value);
    udelay(delay_us);
}

/**
 * @brief Read the SDA pin level
 * @param ctx Pointer to the I2C context
 * @return SDA pin level value
 */
static inline int i2c_soft_get_sda(wm_drv_i2c_ctx_t *ctx)
{
    return wm_drv_gpio_fast_read(ctx->sda);
}

/**
 * @brief Initialize the I2C software simulation
 * @param ctx Pointer to the I2C context
 */
static void i2c_soft_init(wm_drv_i2c_ctx_t *ctx)
{
    int delay = ctx->delay_us;

    wm_drv_gpio_fast_set_dir(ctx->scl, WM_GPIO_DIR_OUTPUT);
    wm_drv_gpio_fast_set_dir(ctx->sda, WM_GPIO_DIR_OUTPUT);

    /* Set both SDA and SCL to high*/
    i2c_soft_set_scl(ctx, 1, delay);
    i2c_soft_set_sda(ctx, 1, delay);
}

/**
 * @brief Send the I2C start signal
 * @param ctx Pointer to the I2C context
 */
static void i2c_soft_start(wm_drv_i2c_ctx_t *ctx)
{
    int delay = ctx->delay_us;

    /* Start signal */
    i2c_soft_set_scl(ctx, 1, delay);
    i2c_soft_set_sda(ctx, 1, delay);
    i2c_soft_set_sda(ctx, 0, delay);
}

/**
 * @brief Send the I2C stop signal
 * @param ctx Pointer to the I2C context
 */
static void i2c_soft_stop(wm_drv_i2c_ctx_t *ctx)
{
    int delay = ctx->delay_us;

    /* Stop signal */
    i2c_soft_set_scl(ctx, 0, delay);
    i2c_soft_set_sda(ctx, 0, delay);

    i2c_soft_set_scl(ctx, 1, delay);
    i2c_soft_set_sda(ctx, 1, delay);
}

/**
 * @brief Write a byte to the I2C bus
 * @param ctx Pointer to the I2C context
 * @param data Byte data to be written
 * @return WM_ERR_SUCCESS if successful, WM_ERR_FAILED otherwise
 */
static int i2c_soft_write_byte(wm_drv_i2c_ctx_t *ctx, uint8_t data)
{
    int delay = ctx->delay_us;
    int i;
    int ret = WM_ERR_SUCCESS;
    uint8_t bit;

    SOFT_I2C_ENTER_CRITIAL();

    for (i = 7; i >= 0; i--) {
        bit = (data >> i) & 1;

        /* send one bit */
        i2c_soft_set_scl(ctx, 0, delay);
        i2c_soft_set_sda(ctx, bit, delay);
        i2c_soft_set_scl(ctx, 1, delay);
    }

    /* send a bit to read ACK/NACK */
    i2c_soft_set_scl(ctx, 0, delay);
    i2c_soft_set_sda(ctx, 1, delay);

    wm_drv_gpio_fast_set_dir(ctx->sda, WM_GPIO_DIR_INPUT);
    i2c_soft_set_scl(ctx, 1, delay);
    /* Get the SDA bit */
    ret = (i2c_soft_get_sda(ctx) == 0 ? WM_ERR_SUCCESS : WM_ERR_FAILED);
    wm_drv_gpio_fast_set_dir(ctx->sda, WM_GPIO_DIR_OUTPUT);

    /* seet clock low */
    i2c_soft_set_scl(ctx, 0, delay);

    SOFT_I2C_EXIT_CRITIAL();

    return ret;
}

/**
 * @brief Read a byte from the I2C bus
 * @param ctx Pointer to the I2C context
 * @param send_ack Whether to send ACK signal
 * @return The byte data read from the I2C bus
 */
static uint8_t i2c_soft_read_byte(wm_drv_i2c_ctx_t *ctx, int send_ack)
{
    uint8_t ret = 0;
    int delay   = ctx->delay_us;
    int i;
    int in_bit;

    SOFT_I2C_ENTER_CRITIAL();

    i2c_soft_set_scl(ctx, 0, delay);
    i2c_soft_set_sda(ctx, 1, delay);

    wm_drv_gpio_fast_set_dir(ctx->sda, WM_GPIO_DIR_INPUT);

    for (i = 7; i >= 0; i--) {
        /* Set SCL to high */
        i2c_soft_set_scl(ctx, 1, delay);

        /* Get SDA bit */
        in_bit = i2c_soft_get_sda(ctx);

        /* Set SCL to low */
        i2c_soft_set_scl(ctx, 0, delay);

        ret = (ret << 1) | (uint32_t)in_bit;
    }

    wm_drv_gpio_fast_set_dir(ctx->sda, WM_GPIO_DIR_OUTPUT);

    /* Send one more bit to set ACK/NACK */
    i2c_soft_set_scl(ctx, 0, delay);
    i2c_soft_set_sda(ctx, !send_ack, delay);
    i2c_soft_set_scl(ctx, 1, delay);

    SOFT_I2C_EXIT_CRITIAL();

    return ret;
}

/**
 * @brief Transmit data in polling mode
 * @param ctx Pointer to the I2C context
 * @param dev_addr Device address
 * @param data Pointer to the data to be transmitted
 * @param size Size of the data to be transmitted
 * @param flag Transmission flags
 * @param timeout Transmission timeout in milliseconds
 * @return WM_ERR_SUCCESS if successful, WM_ERR_FAILED otherwise
 */
static int i2c_soft_tx_polling(wm_drv_i2c_ctx_t *ctx, uint16_t dev_addr, const uint8_t *data, uint32_t size, uint32_t flag,
                               uint32_t timeout)
{
    int ret = WM_ERR_SUCCESS;
    int err;

    if (!(flag & WM_HAL_I2C_FLAG_NOSTART)) {
        /*start, send address*/
        i2c_soft_start(ctx);
        ret = i2c_soft_write_byte(ctx, (dev_addr << 1));
    }

    if (ret == 0) {
        /*write datas*/
        while (size > 0) {
            /*write byte*/
            err = i2c_soft_write_byte(ctx, *data++);

            if (err != WM_ERR_SUCCESS) {
                ret = WM_ERR_FAILED;
                wm_log_error("write error");
            }
            size--;
        }
    }

    if ((!(flag & WM_HAL_I2C_FLAG_NOSTOP)) || (ret != WM_ERR_SUCCESS)) {
        /*need stop*/
        i2c_soft_stop(ctx);
    }

    return ret;
}

/**
 * @brief Receive data in polling mode
 * @param ctx Pointer to the I2C context
 * @param dev_addr Device address
 * @param buffer Pointer to the buffer to store the received data
 * @param size Size of the data to be received
 * @param flag Reception flags
 * @param timeout Reception timeout in milliseconds
 * @return WM_ERR_SUCCESS if successful, WM_ERR_FAILED otherwise
 */
static int i2c_soft_rx_polling(wm_drv_i2c_ctx_t *ctx, uint16_t dev_addr, uint8_t *buffer, uint32_t size, uint32_t flag,
                               uint32_t timeout)
{
    int ret = WM_ERR_SUCCESS;
    int i;
    int send_ack;

    if (!(flag & WM_HAL_I2C_FLAG_NOSTART)) {
        /*start, send write address*/
        i2c_soft_start(ctx);
        ret = i2c_soft_write_byte(ctx, (dev_addr << 1) | 1);
    }

    if (ret != WM_ERR_SUCCESS) {
        return ret;
    } else {
        for (i = 0; i < size; i++) {
            /* need ack for each byte */
            send_ack = 1;
            if (i == (size - 1) && !(flag & WM_HAL_I2C_FLAG_NOSTOP)) {
                /* need stop now , the last ack not needed */
                send_ack = 0;
            }
            /* write byte */
            buffer[i] = i2c_soft_read_byte(ctx, send_ack);
        }
    }

    return ret;
}

/**
 * @brief Configure the I2C pins
 * @param pin_cfg Pointer to the pin configuration array
 * @param count Number of pin configurations
 * @return WM_ERR_SUCCESS if successful, WM_ERR_FAILED otherwise
 */
static int wm_drv_i2c_pin_config(wm_dt_hw_pin_cfg_t *pin_cfg, int count)
{
    wm_gpio_num_t pin;

    if (count != 2) {
        return WM_ERR_FAILED;
    }

    for (int i = 0; i < count; i++) {
        pin = pin_cfg[i].pin_num;
        wm_drv_gpio_iomux_func_sel(pin, pin_cfg[i].pin_mux);
        wm_drv_gpio_set_pullmode(pin, WM_GPIO_PULL_UP);
        wm_drv_gpio_set_dir(pin, WM_GPIO_DIR_OUTPUT);
    }

    return WM_ERR_SUCCESS;
}

/**
 * @brief Initialize the I2C device
 * @param device Pointer to the device structure
 * @return WM_ERR_SUCCESS if successful, other error codes otherwise
 */
static int i2c_init(wm_device_t *device)
{
    wm_drv_i2c_dev_t *dev = (wm_drv_i2c_dev_t *)device;
    wm_drv_i2c_ctx_t *ctx;

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

    ctx = &(dev->drv->ctx);

    /* create mutex for api calling*/
    if (wm_os_internal_recursive_mutex_create(&ctx->mutex) != WM_OS_STATUS_SUCCESS) {
        free(dev->drv);
        return WM_ERR_FAILED;
    }

    /* config SDA and SCL pin*/
    if (wm_drv_i2c_pin_config(dev->hw->pin_cfg, dev->hw->pin_cfg_count) != WM_ERR_SUCCESS) {
        wm_os_internal_recursive_mutex_delete(ctx->mutex);
        free(dev->drv);
        return WM_ERR_FAILED;
    }

    /*remember current clock speed*/
    ctx->last_speed = max_clock;
    ctx->delay_us   = i2c_soft_freq_to_delay(ctx->last_speed);
    ctx->scl        = dev->hw->pin_cfg[0].pin_num; /*force 0 as SCL pin */
    ctx->sda        = dev->hw->pin_cfg[1].pin_num; /*force 1 as SDA pin */

    i2c_soft_init(ctx);

    dev->state = WM_DEV_ST_INITED;

    return WM_ERR_SUCCESS;
}

/**
 * @brief Deinitialize the I2C device
 * @param device Pointer to the device structure
 * @return WM_ERR_SUCCESS if successful, WM_ERR_FAILED otherwise
 */
static int i2c_deinit(wm_device_t *device)
{
    wm_drv_i2c_dev_t *dev = (wm_drv_i2c_dev_t *)device;

    if (dev->state == WM_DEV_ST_INITED && dev->drv) {
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

/**
 * @brief Set the I2C speed if it has changed
 * @param config Pointer to the I2C configuration structure
 * @param ctx Pointer to the I2C context
 * @return WM_ERR_SUCCESS if successful, other error codes otherwise
 */
static int i2c_set_speed_if_change(wm_drv_i2c_config_t *config, wm_drv_i2c_ctx_t *ctx)
{
    int ret = WM_ERR_SUCCESS;

    if (config->speed_hz != ctx->last_speed) {
        ctx->last_speed = config->speed_hz;
        ctx->delay_us   = i2c_soft_freq_to_delay(ctx->last_speed);
    }

    return ret;
}

/**
 * @brief Read data from the I2C device
 * @param device Pointer to the device structure
 * @param config Pointer to the I2C configuration structure
 * @param sub_addr Pointer to the sub-address
 * @param sub_addr_size Size of the sub-address
 * @param read_buf Pointer to the buffer to store the read data
 * @param read_size Size of the data to be read
 * @return WM_ERR_SUCCESS if successful, other error codes otherwise
 */
static int i2c_read(wm_device_t *device, wm_drv_i2c_config_t *config, const void *sub_addr, uint32_t sub_addr_size,
                    void *read_buf, uint32_t read_size)
{
    wm_drv_i2c_dev_t *dev = (wm_drv_i2c_dev_t *)(device);
    wm_drv_i2c_ctx_t *ctx;
    int ret;

    if (!(dev->drv && config->addr <= WM_DRV_I2C_MAX_ADDRESS)) {
        return WM_ERR_INVALID_PARAM;
    }

    if (!(config->speed_hz >= WM_I2C_SPEED_MIN && config->speed_hz <= dev->hw->i2c_cfg.max_clock)) {
        return WM_ERR_INVALID_PARAM;
    }

    ctx = &dev->drv->ctx;

    WM_DRV_I2C_LOCK(ctx);

    /*check speed change and set*/
    ret = i2c_set_speed_if_change(config, ctx);

    if (ret == WM_ERR_SUCCESS) {
        /*write sub address*/
        if (sub_addr && sub_addr_size > 0) {
            ret = i2c_soft_tx_polling(ctx, config->addr, sub_addr, sub_addr_size, WM_HAL_I2C_FLAG_NOSTOP,
                                      WM_DRV_I2C_TRANSFER_TIMEOUT);
        }

        if (ret == WM_ERR_SUCCESS) {
            /*read  data*/
            ret = i2c_soft_rx_polling(ctx, config->addr, read_buf, read_size, 0, WM_DRV_I2C_TRANSFER_TIMEOUT);
        }
    }

    WM_DRV_I2C_UNLOCK(ctx);

    return ret;
}

/**
 * @brief Write data to the I2C device
 * @param device Pointer to the device structure
 * @param config Pointer to the I2C configuration structure
 * @param sub_addr Pointer to the sub-address
 * @param sub_addr_size Size of the sub-address
 * @param data Pointer to the data to be written
 * @param data_size Size of the data to be written
 * @return WM_ERR_SUCCESS if successful, other error codes otherwise
 */
static int i2c_write(wm_device_t *device, wm_drv_i2c_config_t *config, const void *sub_addr, uint32_t sub_addr_size,
                     const void *data, uint32_t data_size)
{
    wm_drv_i2c_dev_t *dev = (wm_drv_i2c_dev_t *)(device);
    wm_drv_i2c_ctx_t *ctx;
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

    WM_DRV_I2C_LOCK(ctx);

    /*check speed change and set*/
    ret = i2c_set_speed_if_change(config, ctx);

    if (ret == WM_ERR_SUCCESS) {
        /*write sub address*/
        ret = i2c_soft_tx_polling(ctx, config->addr, sub_addr, sub_addr_size, write_flag, WM_DRV_I2C_TRANSFER_TIMEOUT);
        if (ret == WM_ERR_SUCCESS && write_flag) {
            /*write data*/
            ret = i2c_soft_tx_polling(ctx, config->addr, data, data_size, WM_HAL_I2C_FLAG_NOSTART, WM_DRV_I2C_TRANSFER_TIMEOUT);
        }
    }

    WM_DRV_I2C_UNLOCK(ctx);

    return ret;
}

/*ops for soft i2c*/
const wm_drv_i2c_ops_t wm_drv_i2c_soft_ops = {
    .init   = i2c_init,
    .deinit = i2c_deinit,
    .read   = i2c_read,
    .write  = i2c_write,
};

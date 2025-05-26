/**
 * @file wm_drv_ops_spi_master_soft.c
 *
 * @brief SPI Master Module
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
#include <string.h>
#include "wm_types.h"
#include "wm_osal.h"
#include "wm_soc.h"
#include "wm_drv_gpio.h"
#include "wm_dt_hw.h"
#include "wm_drv_spi_master.h"
#include "wm_drv_ops_spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#ifndef LOG_TAG
#define LOG_TAG "drv_soft_spi_soft"
#endif
#include "wm_log.h"

#define WM_DRV_SPI_MUTEX_TIMEOUT 10000
#define SOFT_SPI_IO_LOW          0
#define SOFT_SPI_IO_HIGH         1

static int w80x_soft_spim_init(wm_device_t *dev)
{
    int ret;
    wm_dt_hw_spim_soft_t *hw    = NULL;
    wm_drv_spim_ctx_t *spim_drv = NULL;

    if (!dev || !dev->drv || !dev->hw) {
        return WM_ERR_INVALID_PARAM;
    }

    spim_drv = (wm_drv_spim_ctx_t *)dev->drv;
    hw       = (wm_dt_hw_spim_soft_t *)dev->hw;

    if (spim_drv->is_init) {
        return WM_ERR_SUCCESS;
    }

    do {
        /** set gpio config */
        for (int i = 0; i < hw->pin_cfg_count; i++) {
            wm_log_debug("pin_num:%d, pin_mux:%d, pin_dir:%d, pin_pupd:%d", hw->pin_cfg[i].pin_num, hw->pin_cfg[i].pin_mux,
                         hw->pin_cfg[i].pin_dir, hw->pin_cfg[i].pin_pupd);
            wm_drv_gpio_iomux_func_sel(hw->pin_cfg[i].pin_num, hw->pin_cfg[i].pin_mux);
            wm_drv_gpio_set_dir(hw->pin_cfg[i].pin_num, hw->pin_cfg[i].pin_dir);
            wm_drv_gpio_set_pullmode(hw->pin_cfg[i].pin_num, hw->pin_cfg[i].pin_pupd);
        }

        ret = wm_os_internal_mutex_create(&(spim_drv->lock));
        if (ret != WM_OS_STATUS_SUCCESS) {
            wm_log_error("create lock fail\n");
            break;
        }
    } while (0);

    spim_drv->is_init = true;

    return ret;
}

static int w80x_soft_spim_deinit(wm_device_t *dev)
{
    int ret                     = WM_ERR_SUCCESS;
    wm_drv_spim_ctx_t *spim_drv = NULL;

    if (!dev || !dev->drv || !dev->hw) {
        return WM_ERR_INVALID_PARAM;
    }

    spim_drv = (wm_drv_spim_ctx_t *)dev->drv;

    if (!spim_drv->is_init) {
        return WM_ERR_SUCCESS;
    }

    // get lock
    if (wm_os_internal_mutex_acquire(spim_drv->lock, WM_DRV_SPI_MUTEX_TIMEOUT) != WM_OS_STATUS_SUCCESS) {
        return WM_ERR_TIMEOUT;
    }

    wm_os_internal_mutex_release(spim_drv->lock);

    //delete lock
    wm_os_internal_mutex_delete(spim_drv->lock);
    spim_drv->is_init = false;
    dev->state        = WM_DEV_ST_UNINIT;

    return ret;
}

static int spim_transceive_check_arg(wm_device_t *dev, const wm_dt_hw_spim_dev_cfg_t *config, spim_transceive_t *desc)
{
    if (!dev || !dev->drv || !dev->hw || !config || !desc) {
        return WM_ERR_INVALID_PARAM;
    }

    if (!desc->rx_buf && !desc->tx_buf && !desc->flags && !(((spim_transceive_ex_t *)desc)->cmd_len)) {
        wm_log_error("both tx buf and rx buf and cmd are null\n");
        return WM_ERR_INVALID_PARAM;
    }

    if ((desc->tx_buf && !desc->tx_len) || (!desc->tx_buf && desc->tx_len)) {
        wm_log_error("tx buf and tx len not match\n");
        return WM_ERR_INVALID_PARAM;
    }

    if ((desc->rx_buf && !desc->rx_len) || (!desc->rx_buf && desc->rx_len)) {
        wm_log_error("rx buf and rx len not match\n");
        return WM_ERR_INVALID_PARAM;
    }

    if (desc->tx_buf && ((uint32_t)desc->tx_buf < CONFIG_SRAM_BASE_ADDR ||
                         (uint32_t)desc->tx_buf > (CONFIG_PSRAM_BASE_ADDR + CONFIG_PSRAM_MAX_LENGTH))) {
        wm_log_error("tx buf not in sram address: 0x%x\n", (uint32_t)desc->tx_buf);
        return WM_ERR_INVALID_PARAM;
    }

    if (desc->rx_buf && ((uint32_t)desc->rx_buf < CONFIG_SRAM_BASE_ADDR ||
                         (uint32_t)desc->rx_buf > (CONFIG_PSRAM_BASE_ADDR + CONFIG_PSRAM_MAX_LENGTH))) {
        wm_log_error("rx buf not in sram address: 0x%x\n", (uint32_t)desc->rx_buf);
        return WM_ERR_INVALID_PARAM;
    }

    if (desc->flags & SPI_TRANS_VARIABLE_CMD &&
        ((spim_transceive_ex_t *)desc)->cmd_len > sizeof(((spim_transceive_ex_t *)desc)->cmd)) {
        wm_log_error("cmd_len to large, cmd:0x%x, cmd_len:%d\n", ((spim_transceive_ex_t *)desc)->cmd,
                     ((spim_transceive_ex_t *)desc)->cmd_len);
        return WM_ERR_INVALID_PARAM;
    }

    if (desc->flags & SPI_TRANS_VARIABLE_ADDR &&
        ((spim_transceive_ex_t *)desc)->addr_len > sizeof(((spim_transceive_ex_t *)desc)->addr)) {
        wm_log_error("addr_len to large, addr:0x%x, addr_len:%d\n", ((spim_transceive_ex_t *)desc)->addr,
                     ((spim_transceive_ex_t *)desc)->addr_len);
        return WM_ERR_INVALID_PARAM;
    }

    if (config->mode >= SPI_MODE_MAX) {
        wm_log_error("not support current SPI mode: %d\n", config->mode);
        return WM_ERR_INVALID_PARAM;
    }

    if (config->freq > WM_SPIM_MAX_CLOCK || config->freq < 1) {
        wm_log_error("freq: [1 %d]\n", WM_SPIM_MAX_CLOCK);
        return WM_ERR_INVALID_PARAM;
    }

    return WM_ERR_SUCCESS;
}

ATTRIBUTE_INLINE void wm_spim_gpio_byte(wm_dt_hw_pin_cfg_t *pin_cfg, const wm_dt_hw_spim_dev_cfg_t *cfg, bool big_endian,
                                        uint8_t tx, uint8_t *rx)
{
    uint8_t i;
    uint8_t tmp         = 0;
    uint32_t delay_time = (500 * 1000 / cfg->freq);

    for (i = 0; i < 8u; i++) {
        uint8_t tx_bit;
        uint8_t rx_bit = 0;

        if (big_endian) {
            tx_bit = tx & 0x80 ? SOFT_SPI_IO_HIGH : SOFT_SPI_IO_LOW;
            tx <<= 1u;
        } else {
            tx_bit = tx & 0x01 ? SOFT_SPI_IO_HIGH : SOFT_SPI_IO_LOW;
            tx >>= 1u;
        }

        switch (cfg->mode) {
            case SPI_MODE_0:
            {
                wm_drv_gpio_fast_write(pin_cfg[2].pin_num, tx_bit);
                wm_drv_gpio_fast_write(pin_cfg[0].pin_num, SOFT_SPI_IO_HIGH);
                udelay(delay_time);

                rx_bit = wm_drv_gpio_fast_read(pin_cfg[1].pin_num) == SOFT_SPI_IO_HIGH ? 1 : 0;
                wm_drv_gpio_fast_write(pin_cfg[0].pin_num, SOFT_SPI_IO_LOW);
                udelay(delay_time);
                break;
            }
            case SPI_MODE_1:
            {
                wm_drv_gpio_fast_write(pin_cfg[2].pin_num, tx_bit);
                wm_drv_gpio_fast_write(pin_cfg[0].pin_num, SOFT_SPI_IO_HIGH);
                udelay(delay_time);

                wm_drv_gpio_fast_write(pin_cfg[0].pin_num, SOFT_SPI_IO_LOW);
                udelay(delay_time);
                rx_bit = wm_drv_gpio_fast_read(pin_cfg[1].pin_num) == SOFT_SPI_IO_HIGH ? 1 : 0;
                break;
            }
            case SPI_MODE_2:
            {
                wm_drv_gpio_fast_write(pin_cfg[2].pin_num, tx_bit);
                wm_drv_gpio_fast_write(pin_cfg[0].pin_num, SOFT_SPI_IO_LOW);
                udelay(delay_time);

                rx_bit = wm_drv_gpio_fast_read(pin_cfg[1].pin_num) == SOFT_SPI_IO_HIGH ? 1 : 0;
                wm_drv_gpio_fast_write(pin_cfg[0].pin_num, SOFT_SPI_IO_HIGH);
                udelay(delay_time);
                break;
            }
            case SPI_MODE_3:
            {
                wm_drv_gpio_fast_write(pin_cfg[2].pin_num, tx_bit);
                wm_drv_gpio_fast_write(pin_cfg[0].pin_num, SOFT_SPI_IO_LOW);
                udelay(delay_time);

                wm_drv_gpio_fast_write(pin_cfg[0].pin_num, SOFT_SPI_IO_HIGH);
                udelay(delay_time);
                rx_bit = wm_drv_gpio_fast_read(pin_cfg[1].pin_num) == SOFT_SPI_IO_HIGH ? 1 : 0;
                break;
            }
            default:
            {
                break;
            }
        }

        if (big_endian) {
            tmp <<= 1;
            tmp += (rx_bit ? 1 : 0);
        } else {
            tmp >>= 1;
            tmp += (rx_bit ? 0x80 : 0);
        }
    }

    if (rx) {
        *rx = tmp;
    }
}

static int wm_spim_gpio_polling(wm_dt_hw_spim_soft_t *hw, const wm_dt_hw_spim_dev_cfg_t *config, spim_transceive_t *desc,
                                uint32_t timeout_ms)
{
    bool big_endian = desc->flags & SPI_TRANS_BIG_ENDIAN;
    uint8_t *tx_buf = desc->tx_buf;
    uint32_t tx_len = desc->tx_len;
    uint8_t *rx_buf = desc->rx_buf;
    uint32_t rx_len = desc->rx_len;
    uint32_t start  = wm_os_internal_get_time_ms();

    wm_log_debug("tx_len:%d, rx_len:%d", tx_len, rx_len);
    for (int i = 0; i < tx_len || i < rx_len; i++) {
        wm_spim_gpio_byte(hw->pin_cfg, config, big_endian, i < tx_len ? tx_buf[i] : 0, i < rx_len ? rx_buf + i : NULL);

        if (wm_os_internal_get_time_ms() - start > timeout_ms) {
            wm_log_error("spim gpio polling timeout");
            return WM_ERR_FAILED;
        }
    }

    return WM_ERR_SUCCESS;
}

static int wm_soft_spim_tx_rx_polling(wm_dt_hw_spim_soft_t *hw, const wm_dt_hw_spim_dev_cfg_t *config, spim_transceive_t *desc,
                                      uint32_t timeout_ms)
{
    int32_t ret = WM_ERR_SUCCESS;

    assert(desc && hw && config);

    if (desc->flags & (SPI_TRANS_DUMMY_BITS | SPI_TRANS_VARIABLE_CMD | SPI_TRANS_VARIABLE_ADDR)) {
        spim_transceive_ex_t *desc_ex = (spim_transceive_ex_t *)desc;
        if (desc_ex->dummy_bits || desc_ex->cmd_len || desc_ex->addr_len) {
            /** sent dummy, cmd, or addr */
            int front_len;
            uint8_t *front_buf = NULL;

            front_len = desc_ex->dummy_bits;
            front_len = (front_len + 7) / 8 + desc_ex->cmd_len + desc_ex->addr_len;

            wm_log_debug("front_len:%d, dummy_bits:%d, cmd_len:%d, addr_len:%d", front_len, desc_ex->dummy_bits,
                         desc_ex->cmd_len, desc_ex->addr_len);

            front_buf = wm_os_internal_malloc(front_len);
            if (front_buf) {
                memset(front_buf, 0xFF, front_len);

                if (desc_ex->cmd_len) {
                    memcpy(front_buf, &(desc_ex->cmd), desc_ex->cmd_len);
                }

                if (desc_ex->addr_len) {
                    memcpy(front_buf + desc_ex->cmd_len, &(desc_ex->addr), desc_ex->addr_len);
                }

                spim_transceive_t temp_desc = { 0 };
                temp_desc.flags             = desc->flags;
                temp_desc.tx_buf            = front_buf;
                temp_desc.tx_len            = front_len;

                ret = wm_spim_gpio_polling(hw, config, &temp_desc, timeout_ms);
                wm_os_internal_free(front_buf);

                if (ret != WM_ERR_SUCCESS) {
                    return ret;
                }
            } else {
                wm_log_error("cmd malloc fail");
                ret = WM_ERR_NO_MEM;
            }
        }
    }

    if (ret == WM_ERR_SUCCESS && (desc->tx_len || desc->rx_len)) {
        ret = wm_spim_gpio_polling(hw, config, desc, timeout_ms);

        if (ret != WM_ERR_SUCCESS) {
            return ret;
        }
    }

    return ret;
}

static int w80x_soft_spim_transceive_sync(wm_device_t *dev, const wm_dt_hw_spim_dev_cfg_t *config, spim_transceive_t *desc,
                                          uint32_t ms_to_wait)
{
    int ret                     = WM_ERR_SUCCESS;
    wm_drv_spim_ctx_t *spim_drv = NULL;
    wm_dt_hw_spim_soft_t *hw    = NULL;

    if (!ms_to_wait) {
        return WM_ERR_INVALID_PARAM;
    }

    if ((ret = spim_transceive_check_arg(dev, config, desc)) != WM_ERR_SUCCESS) {
        return ret;
    }

    spim_drv = (wm_drv_spim_ctx_t *)dev->drv;
    hw       = (wm_dt_hw_spim_soft_t *)dev->hw;
    if (!spim_drv->is_init) {
        wm_log_error("spim soft not init\n");
        return WM_ERR_NO_INITED;
    }

    wm_log_debug("cs:%d, clk:%d, miso:%d, mosi:%d", config->pin_cs.pin_num, hw->pin_cfg[0].pin_num, hw->pin_cfg[1].pin_num,
                 hw->pin_cfg[2].pin_num);
    wm_log_debug("freq:%.3f KHz, mode:%d, flag:0x%x", ((float)config->freq) / 1000, config->mode, desc->flags);

    if (config->mode == SPI_MODE_0 || config->mode == SPI_MODE_1) {
        wm_drv_gpio_fast_write(hw->pin_cfg[0].pin_num, SOFT_SPI_IO_LOW);
    } else {
        wm_drv_gpio_fast_write(hw->pin_cfg[0].pin_num, SOFT_SPI_IO_HIGH);
    }

    //select cs as valid, PULL down
    if (WM_GPIO_PIN_VALID(config->pin_cs.pin_num)) {
        wm_drv_gpio_data_reset(config->pin_cs.pin_num);
    }

    //step1 get lock
    if (wm_os_internal_mutex_acquire(spim_drv->lock, ms_to_wait ? ms_to_wait : WM_DRV_SPI_MUTEX_TIMEOUT) !=
        WM_OS_STATUS_SUCCESS) {
        return WM_ERR_TIMEOUT;
    }

    //step2 call hal polling tx/rx api
    ret = wm_soft_spim_tx_rx_polling(hw, config, desc, ms_to_wait);

    //if user not ask keep alive, set CS as invalid: PULL up
    if (!(desc->flags & SPI_TRANS_CS_KEEP_ACTIVE) && WM_GPIO_PIN_VALID(config->pin_cs.pin_num)) {
        wm_drv_gpio_data_set(config->pin_cs.pin_num);
    }

    wm_os_internal_mutex_release(spim_drv->lock);

    return ret;
}

typedef struct {
    wm_device_t *dev;
    wm_dt_hw_spim_dev_cfg_t config;
    spim_transceive_ex_t desc;
    wm_spim_callback_t callback;
    void *usr_data;
} soft_spim_async_param;

static void soft_spim_async(void *pvParameters)
{
    soft_spim_async_param *async_param = (soft_spim_async_param *)pvParameters;

    assert(async_param);

    int ret =
        w80x_soft_spim_transceive_sync(async_param->dev, &async_param->config, (spim_transceive_t *)&async_param->desc, -1);

    if (async_param->callback) {
        async_param->callback(ret, async_param->usr_data);
    }

    free(async_param);

    vTaskDelete(NULL);
}

static int w80x_soft_spim_transceive_async(wm_device_t *dev, const wm_dt_hw_spim_dev_cfg_t *config, spim_transceive_t *desc,
                                           wm_spim_callback_t callback, void *usr_data)
{
    int ret = WM_ERR_SUCCESS;
    soft_spim_async_param *async_param;

    if ((ret = spim_transceive_check_arg(dev, config, desc)) != WM_ERR_SUCCESS) {
        return ret;
    }

    async_param = malloc(sizeof(soft_spim_async_param));
    if (async_param == NULL) {
        return WM_ERR_NO_MEM;
    }

    async_param->dev = dev;
    memcpy(&(async_param->config), config, sizeof(*config));

    if (desc->flags & (SPI_TRANS_DUMMY_BITS | SPI_TRANS_VARIABLE_CMD | SPI_TRANS_VARIABLE_ADDR)) {
        memcpy(&(async_param->desc), desc, sizeof(spim_transceive_ex_t));
    } else {
        memcpy(&(async_param->desc), desc, sizeof(spim_transceive_t));
    }
    async_param->callback = callback;
    async_param->usr_data = usr_data;

    if (pdPASS != xTaskCreate(soft_spim_async, "spim_soft", 1024, async_param, 5, NULL)) {
        free(async_param);
        return WM_ERR_FAILED;
    }

    return WM_ERR_SUCCESS;
}

const wm_drv_spim_ops_t wm_drv_spim_soft_ops = {
    .init             = w80x_soft_spim_init,
    .deinit           = w80x_soft_spim_deinit,
    .transceive_sync  = w80x_soft_spim_transceive_sync,
    .transceive_async = w80x_soft_spim_transceive_async,
};

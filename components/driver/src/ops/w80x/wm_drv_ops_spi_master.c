/**
 * @file wm_drv_ops_spi_master.c
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
#include "wm_drv_gpio.h"
#include "wm_dt_hw.h"
#include "wm_drv_rcc.h"
#include "wm_drv_dma.h"
#include "wm_drv_spi_master.h"

#define LOG_TAG "drv_spi"
#include "wm_log.h"

#define WM_DRV_SPI_MUTEX_TIMEOUT       1000
#define WM_SPIM_TRX_WITH_DMA_THRESHOLD 20
#define WM_SPIM_MAX_WAIT_TIME          8 // max 65535 bit / (10000Hz) = 6.535s, max time wait hal tx/rx done

typedef struct {
    bool is_init;
    wm_os_mutex_t *lock;
    wm_device_t *rcc_dev;
    wm_device_t *dma_dev;
    wm_hal_spim_dev_t hal_dev;
    wm_os_sem_t *sync_async_sem; //use to notice async done
    void *priv;
} wm_drv_spim_ctx_t;

typedef struct {
    wm_spim_callback_t callback;
    void *usr_callback_data;
    bool trx_with_dma;
    wm_dt_hw_spim_dev_cfg_t config;
    bool xfer_continue;
    wm_os_sem_t *xfer_sem; //use to transceive tx/rx length over hw capibilty in one time
    uint8_t transceive_flag;
} spim_drv_priv_t;

typedef struct {
    int (*init)(wm_device_t *dev);
    int (*deinit)(wm_device_t *dev);
    int (*transceive_sync)(wm_device_t *dev, const wm_dt_hw_spim_dev_cfg_t *config, spim_transceive_t *desc,
                           uint32_t ms_to_wait);
    int (*transceive_async)(wm_device_t *dev, const wm_dt_hw_spim_dev_cfg_t *config, spim_transceive_t *desc,
                            wm_spim_callback_t callback, void *usr_data);
} wm_drv_spim_ops_t;

static int wm_drv_spim_pin_config(wm_dt_hw_pin_cfg_t *pin_cfg, int count)
{
    wm_gpio_num_t pin;

    for (int i = 0; i < count; i++) {
        pin = pin_cfg[i].pin_num;
        wm_drv_gpio_iomux_func_sel(pin, pin_cfg[i].pin_mux);
        wm_drv_gpio_set_dir(pin, pin_cfg[i].pin_dir);
    }

    return WM_ERR_SUCCESS;
}

static void wm_drv_spim_update_config(wm_device_t *dev, const wm_dt_hw_spim_dev_cfg_t *config)
{
    uint32_t divider                = 0;
    uint32_t freq                   = 0;
    uint32_t apb_clk                = 0;
    wm_drv_spim_ctx_t *spim_drv     = NULL;
    wm_hal_spim_dev_t *spim_hal_dev = NULL;

    spim_drv     = (wm_drv_spim_ctx_t *)dev->drv;
    spim_hal_dev = &(spim_drv->hal_dev);

    assert(config != NULL);
    assert(spim_drv->rcc_dev != NULL);

    //set SPI clock, if user not set clock, use default clock
    freq = config->freq;
    if (!config->freq) {
        freq = WM_SPIM_DEFAULT_CLOCK;
    }
    apb_clk = wm_drv_rcc_get_config_clock(spim_drv->rcc_dev, WM_RCC_TYPE_WLAN) / 4;
    divider = (apb_clk * 1000000) / (freq * 2) - 1;
    wm_hal_spim_set_cmds(spim_hal_dev, SPIM_CMD_FREQ, divider);

    //set SPI mode
    wm_hal_spim_set_cmds(spim_hal_dev, SPIM_CMD_MODE, config->mode);
}

//w800 spi master driver implement
int w800_spim_init(wm_device_t *dev)
{
    int ret                         = WM_ERR_FAILED;
    wm_dt_hw_spim_t *hw             = NULL;
    wm_device_t *dma_dev            = NULL;
    wm_drv_clock_ctx_t *rcc_drv_ctx = NULL;
    wm_drv_spim_ctx_t *spim_drv     = NULL;
    wm_hal_spim_dev_t *spim_hal     = NULL;
    hal_spim_config_t spim_config   = { 0 };
    spim_drv_priv_t *drv_priv       = NULL;

    if (!dev || !dev->drv || !dev->hw) {
        return WM_ERR_INVALID_PARAM;
    }

    spim_drv = (wm_drv_spim_ctx_t *)dev->drv;
    spim_hal = &(spim_drv->hal_dev);
    hw       = (wm_dt_hw_spim_t *)dev->hw;

    if (spim_drv->is_init) {
        return WM_ERR_SUCCESS;
    }

    do {
        dma_dev = wm_dt_get_device_by_name(hw->dma_device_name);
        if (dma_dev) {
            spim_drv->dma_dev       = dma_dev;
            wm_dma_data_t *dma_data = (wm_dma_data_t *)dma_dev->drv;
            spim_hal->dma_hal_dev   = &(dma_data->hal_dev);
        }

        if ((spim_drv->rcc_dev = wm_dt_get_device_by_name(hw->rcc_device_name)) == NULL) {
            wm_log_error("get rcc device fail\n");
            break;
        }
        rcc_drv_ctx           = (wm_drv_clock_ctx_t *)(spim_drv->rcc_dev)->drv;
        spim_hal->rcc_hal_dev = &(rcc_drv_ctx->hal_dev);

        if (hw == NULL || hw->pin_cfg == NULL) {
            wm_log_error("hw info or pin cfg is null\n");
            break;
        }

        if (wm_os_internal_mutex_create(&(spim_drv->lock)) != WM_OS_STATUS_SUCCESS) {
            wm_log_error("create lock fail\n");
            break;
        }

        //enable spi clock
        wm_drv_clock_enable(spim_drv->rcc_dev, WM_RCC_LS_SPI_GATE_EN);

        //set gpio pin
        wm_drv_spim_pin_config(hw->pin_cfg, hw->pin_cfg_count);

        spim_config.cs_control_by_sw = 1; //always contrl by SW
        spim_config.cs_active        = 0; //CS pin LOW is vavlid
        spim_hal->irq_no             = hw->irq_cfg.irq_num;
        spim_hal->register_base      = hw->reg_base;
        ret                          = wm_hal_spim_init(spim_hal, &spim_config);
        if (ret != WM_ERR_SUCCESS) {
            break;
        }

        spim_drv->priv = (spim_drv_priv_t *)wm_os_internal_malloc(sizeof(spim_drv_priv_t));
        if (!spim_drv->priv) {
            wm_log_error("alloc mem fail\n");
            ret = WM_ERR_NO_MEM;
            break;
        }

        drv_priv = (spim_drv_priv_t *)spim_drv->priv;
        if (wm_os_internal_sem_create(&drv_priv->xfer_sem, 0) != WM_OS_STATUS_SUCCESS) {
            wm_log_error("create xfer_sem fail\n");
            ret = WM_ERR_FAILED;
            break;
        }

        if (wm_os_internal_sem_create(&spim_drv->sync_async_sem, 1) != WM_OS_STATUS_SUCCESS) {
            wm_log_error("create xfer_sem fail\n");
            ret = WM_ERR_FAILED;
            break;
        } else {
            dev->state        = WM_DEV_ST_INITED;
            spim_drv->is_init = true;
        }

    } while (0);

    return ret;
}

int w800_spim_deinit(wm_device_t *dev)
{
    int ret                     = WM_ERR_SUCCESS;
    wm_drv_spim_ctx_t *spim_drv = NULL;
    wm_hal_spim_dev_t *spim_hal = NULL;
    spim_drv_priv_t *drv_priv   = NULL;

    if (!dev || !dev->drv || !dev->hw) {
        return WM_ERR_INVALID_PARAM;
    }

    spim_drv = (wm_drv_spim_ctx_t *)dev->drv;
    spim_hal = &(spim_drv->hal_dev);
    drv_priv = spim_drv->priv;

    if (!spim_drv->is_init) {
        return WM_ERR_SUCCESS;
    }

    //step1 get lock
    if (wm_os_internal_mutex_acquire(spim_drv->lock, WM_DRV_SPI_MUTEX_TIMEOUT) != WM_OS_STATUS_SUCCESS) {
        return WM_ERR_TIMEOUT;
    }

    //call hal deinit
    ret = wm_hal_spim_deinit(spim_hal);

    //disable spi control clock
    wm_drv_clock_disable(spim_drv->rcc_dev, WM_RCC_LS_SPI_GATE_EN);

    wm_os_internal_mutex_release(spim_drv->lock);

    //delete lock
    wm_os_internal_mutex_delete(spim_drv->lock);
    wm_os_internal_sem_delete(drv_priv->xfer_sem);
    wm_os_internal_sem_delete(spim_drv->sync_async_sem);
    wm_os_internal_free(drv_priv);
    spim_drv->priv    = NULL;
    spim_drv->is_init = false;
    dev->state        = WM_DEV_ST_UNINIT;

    return ret;
}

static int spim_transceive_check_arg(wm_device_t *dev, const wm_dt_hw_spim_dev_cfg_t *config, spim_transceive_t *desc)
{
    if (!dev || !dev->drv || !dev->hw || !config || !desc) {
        return WM_ERR_INVALID_PARAM;
    }

    if (!desc->rx_buf && !desc->tx_buf && !desc->flags & !(((spim_transceive_ex_t *)desc)->cmd_len)) {
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

    if (config->mode > SPI_MODE_MAX) {
        wm_log_error("not support current SPI mode: %d\n", config->mode);
        return WM_ERR_INVALID_PARAM;
    }

    if (config->freq > WM_SPIM_MAX_CLOCK || config->freq < WM_SPIM_MIN_CLOCK) {
        wm_log_error("freq: [%d %d]\n", WM_SPIM_MIN_CLOCK, WM_SPIM_MAX_CLOCK);
        return WM_ERR_INVALID_PARAM;
    }

    return WM_ERR_SUCCESS;
}

int w800_spim_transceive_sync(wm_device_t *dev, const wm_dt_hw_spim_dev_cfg_t *config, spim_transceive_t *desc,
                              uint32_t ms_to_wait)
{
    bool use_dma                = true;
    int ret                     = WM_ERR_SUCCESS;
    wm_drv_spim_ctx_t *spim_drv = NULL;
    wm_hal_spim_dev_t *spim_hal = NULL;
    uint32_t remain_tx_len      = 0;
    uint32_t remain_rx_len      = 0;
    uint8_t *tx_buf             = NULL;
    uint8_t *rx_buf             = NULL;
    uint32_t cur_tx_len         = 0;
    uint32_t cur_rx_len         = 0;

    if (!ms_to_wait) {
        return WM_ERR_INVALID_PARAM;
    }
    if ((ret = spim_transceive_check_arg(dev, config, desc)) != WM_ERR_SUCCESS) {
        return ret;
    }

    spim_drv = (wm_drv_spim_ctx_t *)dev->drv;
    spim_hal = &(spim_drv->hal_dev);
    if (!spim_drv->is_init) {
        wm_log_error("spim not init\n");
        return WM_ERR_NO_INITED;
    }

    remain_tx_len = desc->tx_len;
    remain_rx_len = desc->rx_len;
    tx_buf        = desc->tx_buf;
    rx_buf        = desc->rx_buf;

    wm_drv_spim_update_config(dev, config);
    //select cs as valid, PULL down
    if (WM_GPIO_PIN_VALID(config->pin_cs.pin_num)) {
        wm_drv_gpio_data_reset(config->pin_cs.pin_num);
    }

    //step1 get lock
    if (wm_os_internal_mutex_acquire(spim_drv->lock, WM_DRV_SPI_MUTEX_TIMEOUT) != WM_OS_STATUS_SUCCESS) {
        return WM_ERR_TIMEOUT;
    }

    //wait async transceive done
    ret = wm_os_internal_sem_acquire(spim_drv->sync_async_sem, HZ * WM_SPIM_MAX_WAIT_TIME);
    if (ret != WM_OS_STATUS_SUCCESS) {
        wm_log_error("wait sync_async_sem timeout\n");
        return WM_ERR_TIMEOUT;
    }

    //step2 check whether use DMA
    if ((desc->tx_len < WM_SPIM_TRX_WITH_DMA_THRESHOLD && desc->rx_len < WM_SPIM_TRX_WITH_DMA_THRESHOLD) ||
        (desc->tx_len < WM_SPIM_TRX_WITH_DMA_THRESHOLD && desc->tx_len != 0 && desc->rx_len > WM_SPIM_TRX_WITH_DMA_THRESHOLD) ||
        (desc->rx_len < WM_SPIM_TRX_WITH_DMA_THRESHOLD && desc->rx_len != 0 && desc->tx_len > WM_SPIM_TRX_WITH_DMA_THRESHOLD) ||
        ((uint32_t)(desc->tx_buf) % 4) || ((uint32_t)(desc->rx_buf) % 4) || !spim_drv->dma_dev) {
        use_dma = false;
    }
    if (desc->flags & SPI_TRANS_DUMMY_BITS) {
        spim_hal->dummy_bits = ((spim_transceive_ex_t *)desc)->dummy_bits;
    } else {
        spim_hal->dummy_bits = 0;
    }

    if (desc->flags & SPI_TRANS_VARIABLE_CMD) {
        spim_hal->cmd     = ((spim_transceive_ex_t *)desc)->cmd;
        spim_hal->cmd_len = ((spim_transceive_ex_t *)desc)->cmd_len;
    }

    if (desc->flags & SPI_TRANS_VARIABLE_ADDR) {
        spim_hal->addr     = ((spim_transceive_ex_t *)desc)->addr;
        spim_hal->addr_len = ((spim_transceive_ex_t *)desc)->addr_len;
    }

    if (desc->flags & SPI_TRANS_BIG_ENDIAN) {
        spim_hal->big_endian = true;
        wm_hal_spim_set_cmds(spim_hal, SPIM_CMD_BIG_ENDIAN, 1);
    } else {
        spim_hal->big_endian = false;
        wm_hal_spim_set_cmds(spim_hal, SPIM_CMD_BIG_ENDIAN, 0);
    }

    if (use_dma) {
        if (desc->tx_buf) {
            if (wm_drv_dma_request_ch(spim_drv->dma_dev, &spim_hal->tx_dma_channel, WM_DRV_SPI_MUTEX_TIMEOUT) !=
                WM_DRV_DMA_STATUS_SUCCESS) {
                use_dma = false;
            }
        }

        if (desc->rx_buf && use_dma) {
            if (wm_drv_dma_request_ch(spim_drv->dma_dev, &spim_hal->rx_dma_channel, WM_DRV_SPI_MUTEX_TIMEOUT) !=
                WM_DRV_DMA_STATUS_SUCCESS) {
                wm_drv_dma_release_ch(spim_drv->dma_dev, spim_hal->tx_dma_channel, WM_DRV_SPI_MUTEX_TIMEOUT);
                use_dma = false;
            }
        }

        if (use_dma) {
            wm_hal_spim_register_xfer_done_callback(spim_hal, NULL, NULL);
            while (remain_tx_len || remain_rx_len) {
                if (remain_tx_len > WM_SPI_MAX_TXRX_LEN) {
                    cur_tx_len = WM_SPI_MAX_TXRX_LEN;
                    remain_tx_len -= WM_SPI_MAX_TXRX_LEN;
                } else {
                    cur_tx_len    = remain_tx_len;
                    remain_tx_len = 0;
                }
                if (remain_rx_len > WM_SPI_MAX_TXRX_LEN) {
                    cur_rx_len = WM_SPI_MAX_TXRX_LEN;
                    remain_rx_len -= WM_SPI_MAX_TXRX_LEN;
                } else {
                    cur_rx_len    = remain_rx_len;
                    remain_rx_len = 0;
                }

                ret = wm_hal_spim_tx_rx_dma_sync(spim_hal, tx_buf, cur_tx_len, rx_buf, cur_rx_len, ms_to_wait);
                if (ret == WM_ERR_SUCCESS) {
                    tx_buf += cur_tx_len;
                    rx_buf += cur_rx_len;
                } else {
                    break;
                }
            }

            if (desc->tx_buf) {
                wm_drv_dma_release_ch(spim_drv->dma_dev, spim_hal->tx_dma_channel, WM_DRV_SPI_MUTEX_TIMEOUT);
            }
            if (desc->rx_buf) {
                wm_drv_dma_release_ch(spim_drv->dma_dev, spim_hal->rx_dma_channel, WM_DRV_SPI_MUTEX_TIMEOUT);
            }
            spim_hal->tx_dma_channel = WM_DMA_CH_MAX;
            spim_hal->rx_dma_channel = WM_DMA_CH_MAX;
        }
    }
    if (!use_dma) {
        //step3 call hal polling tx/rx api
        ret = wm_hal_spim_tx_rx_polling(spim_hal, desc->tx_buf, desc->tx_len, desc->rx_buf, desc->rx_len, ms_to_wait);
    }
    //if user not ask keep alive, set CS as invalid: PULL up
    if (!(desc->flags & SPI_TRANS_CS_KEEP_ACTIVE) && WM_GPIO_PIN_VALID(config->pin_cs.pin_num)) {
        wm_drv_gpio_data_set(config->pin_cs.pin_num);
    }

    wm_os_internal_sem_release(spim_drv->sync_async_sem);
    wm_os_internal_mutex_release(spim_drv->lock);

    return ret;
}

void w800_spim_transceive_async_done_callback(void *user_data, int succ)
{
    wm_device_t *dev            = (wm_device_t *)user_data;
    wm_drv_spim_ctx_t *spim_drv = (wm_drv_spim_ctx_t *)dev->drv;
    spim_drv_priv_t *drv_priv   = (spim_drv_priv_t *)spim_drv->priv;
    wm_hal_spim_dev_t *spim_hal = &(spim_drv->hal_dev);

    if (spim_drv->dma_dev && drv_priv->trx_with_dma) {
        if (drv_priv->xfer_continue) {
            wm_os_internal_sem_release(drv_priv->xfer_sem);
        } else {
            //if user not ask keep alive, set CS as invalid: PULL up
            if (!(drv_priv->transceive_flag & SPI_TRANS_CS_KEEP_ACTIVE) && WM_GPIO_PIN_VALID(drv_priv->config.pin_cs.pin_num)) {
                wm_drv_gpio_data_set(drv_priv->config.pin_cs.pin_num);
            }

            if (spim_hal->tx_dma_channel != WM_DMA_CH_MAX) {
                wm_drv_dma_release_ch(spim_drv->dma_dev, spim_hal->tx_dma_channel, WM_DRV_SPI_MUTEX_TIMEOUT);
                spim_hal->tx_dma_channel = WM_DMA_CH_MAX;
            }
            if (spim_hal->rx_dma_channel != WM_DMA_CH_MAX) {
                wm_drv_dma_release_ch(spim_drv->dma_dev, spim_hal->rx_dma_channel, WM_DRV_SPI_MUTEX_TIMEOUT);
                spim_hal->rx_dma_channel = WM_DMA_CH_MAX;
            }

            if (drv_priv->callback) {
                drv_priv->callback(succ, drv_priv->usr_callback_data);
            }
        }
    } else { //use interrupt xfer
        if (drv_priv->callback) {
            drv_priv->callback(succ, drv_priv->usr_callback_data);
        }
    }

    wm_os_internal_sem_release(spim_drv->sync_async_sem);
}

int w800_spim_transceive_async(wm_device_t *dev, const wm_dt_hw_spim_dev_cfg_t *config, spim_transceive_t *desc,
                               wm_spim_callback_t callback, void *usr_data)
{
    int ret                     = WM_ERR_SUCCESS;
    bool use_dma                = true;
    wm_drv_spim_ctx_t *spim_drv = NULL;
    wm_hal_spim_dev_t *spim_hal = NULL;
    spim_drv_priv_t *drv_priv   = NULL;
    uint32_t remain_tx_len      = 0;
    uint32_t remain_rx_len      = 0;
    uint8_t *tx_buf             = NULL;
    uint8_t *rx_buf             = NULL;
    uint32_t cur_tx_len         = 0;
    uint32_t cur_rx_len         = 0;

    if ((ret = spim_transceive_check_arg(dev, config, desc)) != WM_ERR_SUCCESS) {
        return ret;
    }

    spim_drv = (wm_drv_spim_ctx_t *)dev->drv;
    spim_hal = &(spim_drv->hal_dev);
    drv_priv = (spim_drv_priv_t *)spim_drv->priv;
    if (!spim_drv->is_init) {
        wm_log_error("spim not init\n");
        return WM_ERR_NO_INITED;
    }

    remain_tx_len = desc->tx_len;
    remain_rx_len = desc->rx_len;
    tx_buf        = desc->tx_buf;
    rx_buf        = desc->rx_buf;

    //step1 get lock
    if (wm_os_internal_mutex_acquire(spim_drv->lock, WM_DRV_SPI_MUTEX_TIMEOUT) != WM_OS_STATUS_SUCCESS) {
        wm_log_error("get lock timeout\n");
        return WM_ERR_TIMEOUT;
    }

    //if last async transceive not commplte, wait it's done
    ret = wm_os_internal_sem_acquire(spim_drv->sync_async_sem, HZ * WM_SPIM_MAX_WAIT_TIME);
    if (ret != WM_OS_STATUS_SUCCESS) {
        wm_log_error("wait sync_async_sem timeout\n");
        return WM_ERR_TIMEOUT;
    }

    drv_priv->callback          = callback;
    drv_priv->usr_callback_data = usr_data;
    memcpy(&drv_priv->config, config, sizeof(wm_dt_hw_spim_dev_cfg_t)); ///user maybe free argument, so need copy to local
    drv_priv->transceive_flag = desc->flags;

    wm_drv_spim_update_config(dev, config);
    //select cs as valid, PULL down
    if (WM_GPIO_PIN_VALID(config->pin_cs.pin_num)) {
        wm_drv_gpio_data_reset(config->pin_cs.pin_num);
    }
    //step2 check whether use DMA
    if ((desc->tx_len < WM_SPIM_TRX_WITH_DMA_THRESHOLD && desc->rx_len < WM_SPIM_TRX_WITH_DMA_THRESHOLD) ||
        (desc->tx_len < WM_SPIM_TRX_WITH_DMA_THRESHOLD && desc->tx_len != 0 && desc->rx_len > WM_SPIM_TRX_WITH_DMA_THRESHOLD) ||
        (desc->rx_len < WM_SPIM_TRX_WITH_DMA_THRESHOLD && desc->rx_len != 0 && desc->tx_len > WM_SPIM_TRX_WITH_DMA_THRESHOLD) ||
        ((uint32_t)(desc->tx_buf) % 4) || ((uint32_t)(desc->rx_buf) % 4) || !spim_drv->dma_dev) {
        use_dma = false;
    }

    if (desc->flags & SPI_TRANS_DUMMY_BITS) {
        spim_hal->dummy_bits = ((spim_transceive_ex_t *)desc)->dummy_bits;
    } else {
        spim_hal->dummy_bits = 0;
    }

    if (desc->flags & SPI_TRANS_VARIABLE_CMD) {
        spim_hal->cmd     = ((spim_transceive_ex_t *)desc)->cmd;
        spim_hal->cmd_len = ((spim_transceive_ex_t *)desc)->cmd_len;
    }

    if (desc->flags & SPI_TRANS_VARIABLE_ADDR) {
        spim_hal->addr     = ((spim_transceive_ex_t *)desc)->addr;
        spim_hal->addr_len = ((spim_transceive_ex_t *)desc)->addr_len;
    }

    if (desc->flags & SPI_TRANS_BIG_ENDIAN) {
        spim_hal->big_endian = true;
        wm_hal_spim_set_cmds(spim_hal, SPIM_CMD_BIG_ENDIAN, 1);
    } else {
        spim_hal->big_endian = false;
        wm_hal_spim_set_cmds(spim_hal, SPIM_CMD_BIG_ENDIAN, 0);
    }

    if (use_dma) {
        spim_hal->tx_dma_channel = WM_DMA_CH_MAX;
        spim_hal->rx_dma_channel = WM_DMA_CH_MAX;
        //step3 get dma tx/rx channel,  TODO  need call driver API
        if (desc->tx_buf) {
            if (wm_drv_dma_request_ch(spim_drv->dma_dev, &spim_hal->tx_dma_channel, WM_DRV_SPI_MUTEX_TIMEOUT) !=
                WM_DRV_DMA_STATUS_SUCCESS) {
                use_dma = false;
            }
        }

        if (desc->rx_buf && use_dma) {
            if (wm_drv_dma_request_ch(spim_drv->dma_dev, &spim_hal->rx_dma_channel, WM_DRV_SPI_MUTEX_TIMEOUT) !=
                WM_DRV_DMA_STATUS_SUCCESS) {
                wm_drv_dma_release_ch(spim_drv->dma_dev, spim_hal->tx_dma_channel, WM_DRV_SPI_MUTEX_TIMEOUT);
                use_dma = false;
            }
        }

        if (use_dma) {
            drv_priv->trx_with_dma = true;
            //step4 register xfer done callback
            wm_hal_spim_register_xfer_done_callback(spim_hal, w800_spim_transceive_async_done_callback, dev);

            while (remain_tx_len || remain_rx_len) {
                if (remain_tx_len > WM_SPI_MAX_TXRX_LEN) {
                    cur_tx_len = WM_SPI_MAX_TXRX_LEN;
                    remain_tx_len -= WM_SPI_MAX_TXRX_LEN;
                } else {
                    cur_tx_len    = remain_tx_len;
                    remain_tx_len = 0;
                }
                if (remain_rx_len > WM_SPI_MAX_TXRX_LEN) {
                    cur_rx_len = WM_SPI_MAX_TXRX_LEN;
                    remain_rx_len -= WM_SPI_MAX_TXRX_LEN;
                } else {
                    cur_rx_len    = remain_rx_len;
                    remain_rx_len = 0;
                }

                if (remain_tx_len || remain_rx_len) {
                    drv_priv->xfer_continue = true;
                } else {
                    drv_priv->xfer_continue = false;
                }

                //remain tx/rx length too short to use DMA, replace with IT
                if ((cur_tx_len && cur_tx_len < WM_SPIM_TRX_WITH_DMA_THRESHOLD) ||
                    (cur_rx_len && cur_rx_len < WM_SPIM_TRX_WITH_DMA_THRESHOLD)) {
                    use_dma                 = false;
                    drv_priv->xfer_continue = false;
                    desc->tx_buf            = tx_buf;
                    desc->tx_len            = cur_tx_len;
                    desc->rx_buf            = rx_buf;
                    desc->rx_len            = cur_rx_len;
                    break;
                }

                //step6 call hal tx/rx API
                ret = wm_hal_spim_tx_rx_dma(spim_hal, tx_buf, cur_tx_len, rx_buf, cur_rx_len);
                if (ret == WM_ERR_SUCCESS) {
                    tx_buf += cur_tx_len;
                    rx_buf += cur_rx_len;
                } else {
                    break;
                }

                //wait current tx and rx done callback
                if (drv_priv->xfer_continue) {
                    ret = wm_os_internal_sem_acquire(drv_priv->xfer_sem, HZ * WM_SPIM_MAX_WAIT_TIME);
                    if (ret != WM_OS_STATUS_SUCCESS) {
                        break;
                    }
                }
            }
            if (ret != WM_ERR_SUCCESS) {
                if (spim_hal->tx_dma_channel != WM_DMA_CH_MAX) {
                    wm_drv_dma_release_ch(spim_drv->dma_dev, spim_hal->tx_dma_channel, WM_DRV_SPI_MUTEX_TIMEOUT);
                    spim_hal->tx_dma_channel = WM_DMA_CH_MAX;
                }

                if (spim_hal->rx_dma_channel != WM_DMA_CH_MAX) {
                    wm_drv_dma_release_ch(spim_drv->dma_dev, spim_hal->rx_dma_channel, WM_DRV_SPI_MUTEX_TIMEOUT);
                    spim_hal->rx_dma_channel = WM_DMA_CH_MAX;
                }

                if (drv_priv->callback) {
                    drv_priv->callback(ret, drv_priv->usr_callback_data);
                }
            }
        }
    }

    //request dma maybe fail
    if (!use_dma) {
        drv_priv->trx_with_dma = false;
        //step5 register xfer done callback
        wm_hal_spim_register_xfer_done_callback(spim_hal, w800_spim_transceive_async_done_callback, dev);

        //step5 call hal tx/rx api
        ret = wm_hal_spim_tx_rx_it(spim_hal, desc->tx_buf, desc->tx_len, desc->rx_buf, desc->rx_len);
    }

    if (ret != WM_ERR_SUCCESS) {
        w800_spim_transceive_async_done_callback(dev, ret);
    }

    wm_os_internal_mutex_release(spim_drv->lock);

    return ret;
}

const wm_drv_spim_ops_t wm_drv_spim_ops = {
    .init             = w800_spim_init,
    .deinit           = w800_spim_deinit,
    .transceive_sync  = w800_spim_transceive_sync,
    .transceive_async = w800_spim_transceive_async,
};

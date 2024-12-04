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
#include "wm_hal_spi_master.h"
#include "core_804.h"
#include "wm_error.h"
#include "wm_irq.h"

#define WM_HAL_SPIM_DEBUG 1
#if WM_HAL_SPIM_DEBUG
#undef LOG_TAG
#define LOG_TAG                "spim_hal"
#define wm_spim_log_info(...)  wm_log_info(__VA_ARGS__)
#define wm_spim_log_debug(...) wm_log_debug(__VA_ARGS__)
#define wm_spim_log_error(...) wm_log_error(__VA_ARGS__)
#define WM_SPIM_LOG_MARKER     wm_spim_log_debug("[%s %d]\n", __func__, __LINE__);
#else
#define wm_spim_log_info(...)
#define wm_spim_log_debug(...)
#define wm_spim_log_error(...)
#define WM_SPIM_LOG_MARKER
#endif

#include "wm_log.h"

#define WM_SPI_TX_RX_FIFO_DEPTH    32       //unit is byte
#define WM_WAIT_SPI_IDLE_TIME      500000   //unit us, 500ms
#define WM_SPI_MAX_WAIT_XFER_DOONE 5 * 1000 //5s
#define WM_SPI_USE_DMA_MIN_SIZE    4

typedef struct {
    uint8_t rx_invalid_bit;
    uint8_t *tx_buf;
    uint32_t tx_len;
    uint32_t remain_tx_len;
    uint8_t *rx_buf;
    uint32_t rx_len;
    uint32_t remain_rx_len;
    void *xfer_callback_arg;
    wm_hal_spim_xfer_callback_t xfer_done_callback;
    void (*rx_it_handler)(wm_hal_spim_dev_t *dev, uint8_t *rx_buf, uint32_t rx_len);
    void (*tx_rx_it_handler)(wm_hal_spim_dev_t *dev, uint8_t *tx_buf, uint32_t tx_len, uint8_t *rx_buf, uint32_t rx_len);
} wm_spi_hal_priv_t;

static wm_spi_hal_priv_t spi_priv;

#if 0
static void dump_spi_regs(wm_spi_reg_t *spi_reg)
{
    wm_spim_log_debug("spi regs as below.....\n");
    wm_spim_log_debug("ch_cfg(0):0x%x\n", spi_reg->ch_cfg.val);

    wm_spim_log_debug("cfg(4):0x%x\n", spi_reg->cfg.val);
    wm_spim_log_debug("clk_cfg(8):0x%x\n", spi_reg->clk_cfg.val);
    wm_spim_log_debug("mode_cfg(c):0x%x\n", spi_reg->mode_cfg.val);
    wm_spim_log_debug("int_mask(10):0x%x\n", spi_reg->int_mask.val);

    wm_spim_log_debug("int_status(14):0x%x\n", spi_reg->int_status.val);
    wm_spim_log_debug("status:(18)0x%x\n", spi_reg->status.val);
    wm_spim_log_debug("timeout_cfg(1c):0x%x\n", spi_reg->timeout_cfg.val);

    wm_spim_log_debug("tx_data(20):0x%x\n", spi_reg->tx_data.val);
    wm_spim_log_debug("trans_mode(24):0x%x\n", spi_reg->trans_mode.val);
    wm_spim_log_debug("slv_xmit_len(28):0x%x\n", spi_reg->slv_xmit_len.val);
    wm_spim_log_debug("rx_data(30):0x%x\n", spi_reg->rx_data.val);
}

void dump_data(uint8_t *buf, uint32_t len)
{
    for (int i = 0; i < len; i++) {
        if (i % 16 == 0) {
            printf("\n");
        }
        printf("[%d]0x%x ", i, buf[i]);
    }

    printf("\n");
}
#endif

static int wm_wait_spi_idle(wm_hal_spim_dev_t *dev, uint32_t timeout_us)
{
    uint32_t load_value = csi_coret_get_load(); //W800 is 479999,
    uint32_t start_tick = 0, cur_tick = 0;
    uint32_t timeout_us_cnt = 0, cur_pass_cnt = 0;
    uint32_t cpu_clock      = 0;
    uint32_t max_support_us = 0; //W800 CPU clock 240Mhz, max: 479999/240 = 1990us
    int ret                 = WM_ERR_SUCCESS;
    uint32_t turnaround_cnt = 0;
    uint32_t stat_case      = 0;
    wm_spi_reg_t *spi_reg   = NULL;

    assert(dev->rcc_hal_dev != NULL);

    cpu_clock      = wm_hal_rcc_get_clock_config(dev->rcc_hal_dev, WM_RCC_TYPE_CPU);
    max_support_us = load_value / cpu_clock;
    spi_reg        = (wm_spi_reg_t *)dev->register_base;

    if (max_support_us > timeout_us) {
        timeout_us_cnt = timeout_us * cpu_clock;
        start_tick     = csi_coret_get_value();

        while (cur_pass_cnt < timeout_us_cnt && wm_ll_spi_get_status_is_busy(spi_reg)) {
            cur_tick = csi_coret_get_value();
            if (start_tick >= cur_tick) {
                cur_pass_cnt = start_tick - cur_tick;
            } else {
                cur_pass_cnt = start_tick + load_value - cur_tick;
            }
        }

        ret = (cur_pass_cnt < timeout_us_cnt) ? WM_ERR_SUCCESS : WM_ERR_TIMEOUT;
    } else {
        turnaround_cnt = timeout_us / max_support_us;
        start_tick     = csi_coret_get_load() / 2;
        turnaround_cnt += turnaround_cnt;
        cur_tick = csi_coret_get_value();
        if (cur_tick > start_tick)
            stat_case = 1;
        while (turnaround_cnt && wm_ll_spi_get_status_is_busy(spi_reg)) {
            cur_tick = csi_coret_get_value();
            if (stat_case == 1) {
                if (start_tick > cur_tick) {
                    stat_case = 2;
                    turnaround_cnt--;
                }
            } else {
                if (start_tick <= cur_tick) {
                    stat_case = 1;
                    turnaround_cnt--;
                }
            }
        }
        ret = turnaround_cnt ? WM_ERR_SUCCESS : WM_ERR_TIMEOUT;
    }

    return ret;
}

static uint32_t wm_hal_spi_fill_tx_fifo(wm_hal_spim_dev_t *dev, uint8_t *data_buf, uint32_t data_len)
{
    uint8_t fifo_remain_space = 0;
    uint8_t left_byte         = 0;
    uint8_t *p                = data_buf;
    uint32_t max_fill_size    = 0;
    uint32_t tmp_value        = 0;
    wm_spi_reg_t *spi_reg     = NULL;
    int i                     = 0;

    spi_reg           = (wm_spi_reg_t *)dev->register_base;
    fifo_remain_space = WM_SPI_TX_RX_FIFO_DEPTH - wm_ll_spi_get_status_tx_fifo_level(spi_reg);
    max_fill_size     = ((data_len < fifo_remain_space) ? data_len : fifo_remain_space);

    //fill with word
    for (i = 0; i < (max_fill_size / 4); i++) {
        tmp_value = p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);
        wm_ll_spi_set_tx_data_address(spi_reg, tmp_value);
        p += 4;
    }

    //fill left bytes
    left_byte = max_fill_size % 4;
    if (left_byte) {
        tmp_value = 0;
        if (dev->big_endian) {
            for (i = 0; i < left_byte; i++) {
                tmp_value |= (p[i] << ((4 - left_byte + i) * 8));
            }
        } else {
            for (i = 0; i < left_byte; i++) {
                tmp_value |= (p[i] << (i * 8));
            }
        }

        wm_ll_spi_set_tx_data_address(spi_reg, tmp_value);
    }

    return max_fill_size;
}

static uint32_t wm_hal_spi_get_rx_fifo(wm_hal_spim_dev_t *dev, uint8_t *rx_buf, uint32_t remain_rx_len)
{
    uint8_t fifo_level = 0;
    uint8_t rw_words;
    uint8_t rw_bytes;
    uint32_t data32;
    uint32_t rx_len       = 0;
    wm_spi_reg_t *spi_reg = NULL;

    spi_reg    = (wm_spi_reg_t *)dev->register_base;
    fifo_level = wm_ll_spi_get_status_rx_fifo_level(spi_reg);

    rw_words = fifo_level / 4;
    rw_bytes = fifo_level % 4;

    for (int i = 0; i < rw_words; i++) {
        data32 = wm_ll_spi_get_rx_data(spi_reg);
        if (remain_rx_len >= 4) {
            rx_buf[rx_len++] = data32 & 0xFF;
            rx_buf[rx_len++] = (data32 >> 8) & 0xFF;
            rx_buf[rx_len++] = (data32 >> 16) & 0xFF;
            rx_buf[rx_len++] = (data32 >> 24) & 0xFF;
            remain_rx_len -= 4;
        } else {
            if (!dev->big_endian) {
                for (int i = 0; i < remain_rx_len; i++) {
                    rx_buf[rx_len + i] = (data32 >> (i * 8)) & 0xFF;
                }
            } else {
                for (int i = 0; i < remain_rx_len; i++) {
                    rx_buf[rx_len + i] = (data32 >> ((3 - i) * 8)) & 0xFF;
                }
            }
            rx_len += remain_rx_len;
            remain_rx_len = 0;
            break;
        }
    }

    if (rw_bytes && remain_rx_len) {
        data32 = wm_ll_spi_get_rx_data(spi_reg);
        if (!dev->big_endian) {
            for (int i = 0; i < rw_bytes && i < remain_rx_len; i++) {
                rx_buf[rx_len++] = (data32 >> (i * 8)) & 0xFF;
            }
        } else {
            for (int i = 0; i < rw_bytes && i < remain_rx_len; i++) {
                rx_buf[rx_len++] = (data32 >> ((3 - i) * 8)) & 0xFF;
            }
        }
    }

    return rx_len;
}

void wm_hal_spim_interrupt_handler(wm_irq_no_t irq, void *arg)
{
    wm_hal_spim_dev_t *dev  = (wm_hal_spim_dev_t *)arg;
    wm_spi_reg_t *spi_reg   = (wm_spi_reg_t *)(dev->register_base);
    uint32_t int_source     = wm_ll_spi_get_int_status(spi_reg);
    uint32_t int_mask       = wm_ll_spi_int_mask_get_val(spi_reg);
    wm_spi_hal_priv_t *priv = (wm_spi_hal_priv_t *)dev->priv;

    int_source &= ~int_mask;

    wm_ll_spi_reset_int_status(spi_reg, int_source); //clear interrupt

    if (int_source & SPI_INT_TX_FIFO_RDY) {
        if (priv->tx_rx_it_handler && priv->tx_buf) {
            priv->tx_rx_it_handler(dev, priv->tx_buf, priv->remain_tx_len, priv->rx_buf, priv->remain_rx_len);
        }
    }

    if (int_source & SPI_INT_RX_FIFO_RDY) {
        if (priv->rx_it_handler && priv->rx_buf) {
            priv->rx_it_handler(dev, priv->rx_buf, priv->remain_rx_len);
        }
    }

    if (int_source & SPI_INT_TRANSFER_DONE) {
        if (priv->tx_rx_it_handler && (priv->tx_buf || priv->rx_buf)) {
            priv->tx_rx_it_handler(dev, priv->tx_buf, priv->remain_tx_len, priv->rx_buf, priv->remain_rx_len);
        }
    }

    if (int_source & SPI_INT_TIMEOUT) {
        wm_spim_log_debug("SPI_INT_TIMEOUT\n");
    }
}

static void wm_hal_spim_tx_rx_it_handler(wm_hal_spim_dev_t *dev, uint8_t *tx_buf, uint32_t tx_len, uint8_t *rx_buf,
                                         uint32_t rx_len)
{
    uint8_t cur_rx_len      = 0;
    uint32_t tx_fill_len    = 0;
    wm_spi_reg_t *spi_reg   = NULL;
    wm_spi_hal_priv_t *priv = (wm_spi_hal_priv_t *)dev->priv;

    spi_reg = (wm_spi_reg_t *)(dev->register_base);
    if (wm_wait_spi_idle(dev, WM_WAIT_SPI_IDLE_TIME) != WM_ERR_SUCCESS) {
        wm_ll_spi_set_ch_cfg_clr_fifo(spi_reg, 1);
        wm_ll_spi_set_int_mask_cfg_done_en(spi_reg, 1); //disable tx rx done interrupt
        wm_ll_spi_set_ch_cfg_tx_ch_on(spi_reg, 0);
        wm_ll_spi_set_ch_cfg_rx_ch_on(spi_reg, 0);
        priv->tx_rx_it_handler = NULL;
        if (priv->xfer_done_callback) {
            priv->xfer_done_callback(priv->xfer_callback_arg, WM_ERR_TIMEOUT);
        }
        return;
    }

    if (rx_len) {
        cur_rx_len = wm_hal_spi_get_rx_fifo(dev, rx_buf, priv->remain_rx_len);
        priv->remain_rx_len -= cur_rx_len;
        priv->rx_buf += cur_rx_len;
        wm_ll_spi_set_ch_cfg_rx_invalid_bit(spi_reg, 0);
        if (!priv->remain_rx_len) {
            wm_ll_spi_set_ch_cfg_rx_ch_on(spi_reg, 0);
        }
    } else {
        wm_ll_spi_set_ch_cfg_clr_fifo(spi_reg, 1);
        wm_ll_spi_set_ch_cfg_rx_ch_on(spi_reg, 0);
    }

    if (tx_len) {
        tx_fill_len = wm_hal_spi_fill_tx_fifo(dev, tx_buf, tx_len);
        if (priv->remain_tx_len >= tx_fill_len) {
            priv->remain_tx_len -= tx_fill_len;
        } else {
            priv->remain_tx_len = 0;
        }
        priv->tx_buf += tx_fill_len;
        wm_ll_spi_set_ch_cfg_tx_rx_len(spi_reg, tx_fill_len * 8);
    } else {
        tx_fill_len = (WM_SPI_TX_RX_FIFO_DEPTH < priv->remain_rx_len) ? WM_SPI_TX_RX_FIFO_DEPTH : priv->remain_rx_len;
        wm_ll_spi_set_ch_cfg_tx_rx_len(spi_reg, tx_fill_len * 8);
    }

    if ((!tx_len) && (!rx_len || !priv->remain_rx_len)) { //tx && rx done
        wm_ll_spi_set_ch_cfg_clr_fifo(spi_reg, 1);
        wm_ll_spi_set_int_mask_cfg_done_en(spi_reg, 1); //disable tx rx done interrupt
        wm_ll_spi_set_ch_cfg_tx_ch_on(spi_reg, 0);
        wm_ll_spi_set_ch_cfg_rx_ch_on(spi_reg, 0);
        priv->tx_rx_it_handler = NULL;
        if (priv->xfer_done_callback) {
            priv->xfer_done_callback(priv->xfer_callback_arg, 0);
        }
        return;
    } else {
        wm_ll_spi_set_ch_cfg_start(spi_reg, 1);
    }
}

static void wm_hal_spim_tx_it_handler(wm_hal_spim_dev_t *dev, uint8_t *tx_buf, uint32_t tx_len, uint8_t *rx_buf,
                                      uint32_t rx_len)
{
    wm_spi_reg_t *spi_reg   = NULL;
    uint32_t tx_fill_len    = 0;
    wm_spi_hal_priv_t *priv = (wm_spi_hal_priv_t *)dev->priv;

    spi_reg = (wm_spi_reg_t *)(dev->register_base);

    if (wm_wait_spi_idle(dev, WM_WAIT_SPI_IDLE_TIME) != WM_ERR_SUCCESS) {
        wm_ll_spi_set_ch_cfg_clr_fifo(spi_reg, 1);
        priv->tx_rx_it_handler = NULL;
        if (priv->xfer_done_callback) {
            priv->xfer_done_callback(priv->xfer_callback_arg, WM_ERR_TIMEOUT);
        }
        return;
    }

    if (priv->remain_tx_len == 0 || tx_len == 0) {             //tx done
        wm_ll_spi_set_int_mask_cfg_tx_fifo_rdy_en(spi_reg, 1); //disable tx fifo ready interrupt
        wm_ll_spi_set_ch_cfg_tx_ch_on(spi_reg, 0);
        wm_ll_spi_set_ch_cfg_rx_ch_on(spi_reg, 0);
        priv->tx_rx_it_handler = NULL;
        if (priv->xfer_done_callback) {
            priv->xfer_done_callback(priv->xfer_callback_arg, 0);
        }

        return;
    }
    if (priv->remain_tx_len || tx_len) {
        tx_fill_len = wm_hal_spi_fill_tx_fifo(dev, tx_buf, tx_len);
        priv->remain_tx_len -= tx_fill_len;
        priv->tx_buf += tx_fill_len;
        wm_ll_spi_set_ch_cfg_tx_rx_len(spi_reg, tx_fill_len * 8);
    }
    wm_ll_spi_set_ch_cfg_start(spi_reg, 1);
}

static void wm_hal_spim_rx_it_handler(wm_hal_spim_dev_t *dev, uint8_t *rx_buf, uint32_t rx_len)
{
    uint8_t cur_rx_len      = 0;
    wm_spi_reg_t *spi_reg   = NULL;
    wm_spi_hal_priv_t *priv = (wm_spi_hal_priv_t *)dev->priv;

    spi_reg = (wm_spi_reg_t *)(dev->register_base);

    if (wm_wait_spi_idle(dev, WM_WAIT_SPI_IDLE_TIME) != WM_ERR_SUCCESS) {
        wm_ll_spi_set_ch_cfg_clr_fifo(spi_reg, 1);
        priv->tx_rx_it_handler = NULL;
        if (priv->xfer_done_callback) {
            priv->xfer_done_callback(priv->xfer_callback_arg, WM_ERR_TIMEOUT);
        }
        return;
    }

    if (rx_len) {
        cur_rx_len = wm_hal_spi_get_rx_fifo(dev, rx_buf, rx_len);
        priv->remain_rx_len -= cur_rx_len;
        priv->rx_buf += cur_rx_len;
        wm_ll_spi_set_ch_cfg_rx_invalid_bit(spi_reg, 0);
    }

    if (priv->remain_rx_len) {
        //tx_fill_len = (priv->remain_tx_len < WM_SPI_TX_RX_FIFO_DEPTH) ? priv->remain_tx_len : WM_SPI_TX_RX_FIFO_DEPTH;
        wm_ll_spi_set_ch_cfg_tx_rx_len(spi_reg, WM_SPI_TX_RX_FIFO_DEPTH * 8);
        wm_ll_spi_set_ch_cfg_start(spi_reg, 1);
    } else {                                                   //rx done
        wm_ll_spi_set_int_mask_cfg_rx_fifo_rdy_en(spi_reg, 1); //disable rx fifo ready interrupt
        wm_ll_spi_set_ch_cfg_tx_ch_on(spi_reg, 0);
        wm_ll_spi_set_ch_cfg_rx_ch_on(spi_reg, 0);
        priv->tx_rx_it_handler = NULL;
        if (priv->xfer_done_callback) {
            priv->xfer_done_callback(priv->xfer_callback_arg, 0);
        }
    }
}

int wm_hal_spim_register_xfer_done_callback(wm_hal_spim_dev_t *dev, wm_hal_spim_xfer_callback_t spim_callback, void *user_data)
{
    wm_spi_hal_priv_t *priv = (wm_spi_hal_priv_t *)dev->priv;

    if (priv == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    priv->xfer_done_callback = spim_callback;
    priv->xfer_callback_arg  = user_data;

    return WM_ERR_SUCCESS;
}

static void wm_hal_spim_tx_dma_done_callback(wm_dma_ch_t ch, uint32_t sts, void *user_data)
{
    wm_hal_spim_dev_t *dev          = (wm_hal_spim_dev_t *)user_data;
    wm_spi_hal_priv_t *priv         = dev->priv;
    wm_spi_reg_t *spi_reg           = NULL;
    wm_dma_sts_info_t tx_dma_status = { .sts = WM_DMA_RUNNIG, .xfer_cnt = 0 };
    uint32_t tmp_tx_data            = 0;
    uint8_t *tx_buf_addr            = NULL;
    uint8_t remain_tx_len           = 0;
    uint8_t tx_fifo_cnt             = 0;
    uint8_t *p                      = NULL;
    int ret                         = WM_ERR_SUCCESS;

    spi_reg = (wm_spi_reg_t *)dev->register_base;

    if (ch == dev->tx_dma_channel) {
        wm_hal_dma_unregister_callback(dev->dma_hal_dev, dev->tx_dma_channel);
        wm_hal_dma_stop(dev->dma_hal_dev, dev->tx_dma_channel);
        if (wm_wait_spi_idle(dev, WM_WAIT_SPI_IDLE_TIME) != WM_ERR_SUCCESS) {
            wm_spim_log_error("tx dma done wait spi idle timeout\n");
            wm_ll_spi_set_ch_cfg_clr_fifo(spi_reg, 1);
            priv->tx_rx_it_handler = NULL;
            if (priv->xfer_done_callback) {
                priv->xfer_done_callback(priv->xfer_callback_arg, WM_ERR_TIMEOUT);
            }
            return;
        }

        wm_ll_spi_set_mode_cfg_tx_dma_on(spi_reg, 0); //disable tx with dma
        tx_fifo_cnt = wm_ll_spi_get_status_tx_fifo_level(spi_reg);
        wm_hal_dma_get_status(dev->dma_hal_dev, dev->tx_dma_channel, &tx_dma_status);
        remain_tx_len = priv->remain_tx_len - tx_dma_status.xfer_cnt;

        if (remain_tx_len > 0) {
            p           = (uint8_t *)&tmp_tx_data;
            tx_buf_addr = priv->tx_buf + (priv->tx_len - remain_tx_len);
            for (int i = 0; i < remain_tx_len; i++) {
                p[i] = tx_buf_addr[i];
            }
            wm_ll_spi_set_tx_data_address(spi_reg, tmp_tx_data);
            wm_ll_spi_set_ch_cfg_tx_rx_len(spi_reg, (tx_fifo_cnt + remain_tx_len) * 8);
            wm_ll_spi_set_ch_cfg_start(spi_reg, 1);
            if (wm_wait_spi_idle(dev, WM_WAIT_SPI_IDLE_TIME) != WM_ERR_SUCCESS) {
                wm_ll_spi_set_ch_cfg_clr_fifo(spi_reg, 1);
                ret = WM_ERR_TIMEOUT;
            }
        }
        if (priv->xfer_done_callback) {
            priv->xfer_done_callback(priv->xfer_callback_arg, ret);
        }

        wm_ll_spi_set_ch_cfg_rx_invalid_bit(spi_reg, 0);
        priv->tx_buf        = NULL;
        priv->tx_len        = 0;
        priv->remain_tx_len = 0;
    }
}

static void wm_hal_spim_rx_dma_done_callback(wm_dma_ch_t ch, uint32_t sts, void *user_data)
{
    wm_hal_spim_dev_t *dev          = (wm_hal_spim_dev_t *)user_data;
    wm_spi_hal_priv_t *priv         = (wm_spi_hal_priv_t *)dev->priv;
    wm_spi_reg_t *spi_reg           = NULL;
    uint8_t rx_fifo_cnt             = 0;
    wm_dma_sts_info_t rx_dma_status = { .sts = WM_DMA_RUNNIG, .xfer_cnt = 0 };
    uint32_t remain_rx_cnt          = 0;
    uint32_t rx_data                = 0;
    int ret                         = WM_ERR_SUCCESS;
    uint32_t total_rx_len = 0, cur_rx_len = 0;

    spi_reg = (wm_spi_reg_t *)dev->register_base;
    if (ch == dev->rx_dma_channel) {
        if (wm_wait_spi_idle(dev, WM_WAIT_SPI_IDLE_TIME) != WM_ERR_SUCCESS) {
            wm_hal_dma_stop(dev->dma_hal_dev, dev->rx_dma_channel);
            wm_hal_dma_unregister_callback(dev->dma_hal_dev, dev->rx_dma_channel);
            wm_ll_spi_set_ch_cfg_clr_fifo(spi_reg, 1);
            priv->tx_rx_it_handler = NULL;
            if (priv->xfer_done_callback) {
                priv->xfer_done_callback(priv->xfer_callback_arg, WM_ERR_TIMEOUT);
            }
            return;
        }

        wm_ll_spi_set_mode_cfg_rx_dma_on(spi_reg, 0); //disable rx with dma
        wm_hal_dma_unregister_callback(dev->dma_hal_dev, dev->rx_dma_channel);
        wm_ll_spi_set_ch_cfg_rx_invalid_bit(spi_reg, 0);

        rx_fifo_cnt = wm_ll_spi_get_status_rx_fifo_level(spi_reg);
        wm_hal_dma_get_status(dev->dma_hal_dev, dev->rx_dma_channel, &rx_dma_status);
        remain_rx_cnt = priv->remain_rx_len - rx_dma_status.xfer_cnt;

        total_rx_len = priv->rx_len - remain_rx_cnt;

        if (remain_rx_cnt) { //for not 4byte alian case
            if (rx_fifo_cnt) {
                cur_rx_len = wm_hal_spi_get_rx_fifo(dev, priv->rx_buf + total_rx_len, remain_rx_cnt);
                remain_rx_cnt -= cur_rx_len;
            }

            if (remain_rx_cnt) {
                wm_ll_spi_set_ch_cfg_tx_rx_len(spi_reg, remain_rx_cnt * 8);
                wm_ll_spi_set_ch_cfg_start(spi_reg, 1);
                ret = wm_wait_spi_idle(dev, WM_WAIT_SPI_IDLE_TIME);
                if (!ret) {
                    rx_data = wm_ll_spi_get_rx_data(spi_reg);
                    for (int i = 0; i < remain_rx_cnt; i++) {
                        priv->rx_buf[rx_dma_status.xfer_cnt + i] = (rx_data >> (i * 8)) & 0xFF;
                    }
                }
            }
        }
        priv->rx_buf        = NULL;
        priv->remain_rx_len = 0;
        priv->rx_len        = 0;

        if (priv->xfer_done_callback) {
            priv->xfer_done_callback(priv->xfer_callback_arg, ret);
        }

        wm_ll_spi_set_ch_cfg_clr_fifo(spi_reg, 1);
    }
}

static void wm_hal_spim_tx_rx_dma_done_callback(wm_dma_ch_t ch, uint32_t sts, void *user_data)
{
    wm_hal_spim_dev_t *dev          = (wm_hal_spim_dev_t *)user_data;
    wm_spi_hal_priv_t *priv         = (wm_spi_hal_priv_t *)dev->priv;
    wm_spi_reg_t *spi_reg           = NULL;
    uint8_t rx_fifo_cnt             = 0;
    wm_dma_sts_info_t rx_dma_status = { .sts = WM_DMA_RUNNIG, .xfer_cnt = 0 };
    wm_dma_sts_info_t tx_dma_status = { .sts = WM_DMA_RUNNIG, .xfer_cnt = 0 };
    uint32_t remain_rx_len          = 0;
    uint32_t remain_tx_len          = 0;
    int ret                         = WM_ERR_SUCCESS;
    uint32_t cur_rx_len             = 0;

    spi_reg = (wm_spi_reg_t *)dev->register_base;
    wm_ll_spi_set_ch_cfg_rx_invalid_bit(spi_reg, 0);
    if (ch == dev->tx_dma_channel) {
        if (wm_wait_spi_idle(dev, WM_WAIT_SPI_IDLE_TIME) != WM_ERR_SUCCESS) {
            wm_spim_log_error("trx dma wait spi idle timeout\n");
            wm_ll_spi_set_ch_cfg_clr_fifo(spi_reg, 1);
            priv->tx_rx_it_handler = NULL;
            if (priv->xfer_done_callback) {
                priv->xfer_done_callback(priv->xfer_callback_arg, WM_ERR_TIMEOUT);
            }
            return;
        }

        wm_hal_dma_unregister_callback(dev->dma_hal_dev, dev->tx_dma_channel);
        wm_hal_dma_stop(dev->dma_hal_dev, dev->tx_dma_channel);
        wm_ll_spi_set_mode_cfg_tx_dma_on(spi_reg, 0); //disable tx with dma

        wm_hal_dma_get_status(dev->dma_hal_dev, dev->tx_dma_channel, &tx_dma_status);
        remain_tx_len = priv->remain_tx_len - tx_dma_status.xfer_cnt;
        priv->tx_buf += tx_dma_status.xfer_cnt;
        while (rx_dma_status.sts == WM_DMA_RUNNIG) {
            wm_hal_dma_get_status(dev->dma_hal_dev, dev->rx_dma_channel, &rx_dma_status);
        }
        wm_hal_dma_unregister_callback(dev->dma_hal_dev, dev->rx_dma_channel);
        wm_hal_dma_stop(dev->dma_hal_dev, dev->rx_dma_channel);
        wm_ll_spi_set_mode_cfg_rx_dma_on(spi_reg, 0); //disable rx with dma

        remain_rx_len = priv->remain_rx_len - rx_dma_status.xfer_cnt;
        priv->rx_buf += rx_dma_status.xfer_cnt;
        rx_fifo_cnt = wm_ll_spi_get_status_rx_fifo_level(spi_reg);

        if (remain_rx_len) {
            if (rx_fifo_cnt) {
                cur_rx_len = wm_hal_spi_get_rx_fifo(dev, priv->rx_buf, remain_rx_len);
                remain_rx_len -= cur_rx_len;
                priv->rx_buf += cur_rx_len;
            }
        }

        ret = wm_hal_spim_tx_rx_polling(dev, priv->tx_buf, remain_tx_len, priv->rx_buf, remain_rx_len,
                                        WM_SPI_MAX_WAIT_XFER_DOONE);

        priv->tx_buf        = NULL;
        priv->tx_len        = 0;
        priv->remain_tx_len = 0;
        priv->rx_buf        = NULL;
        priv->rx_len        = 0;
        priv->remain_rx_len = 0;

        if (priv->xfer_done_callback) {
            priv->xfer_done_callback(priv->xfer_callback_arg, ret);
        }
    }
}

static int wm_hal_spim_dma_prepare_tx(wm_hal_spim_dev_t *dev, uint8_t *tx_buf, uint32_t tx_len, bool en_int, bool en_callback)
{
    int ret                       = WM_ERR_SUCCESS;
    wm_spi_reg_t *spi_reg         = NULL;
    wm_hal_dma_desc_t *tx_dma_cfg = NULL;
    tx_dma_cfg                    = &dev->tx_dma_cfg;
    spi_reg                       = (wm_spi_reg_t *)dev->register_base;

    do {
        tx_dma_cfg->ctrl.nonchain.auto_reload   = WM_DMA_RELOAD_DISABLE;
        tx_dma_cfg->ctrl.nonchain.src_addr_inc  = WM_DMA_ADDR_INC;
        tx_dma_cfg->ctrl.nonchain.dest_addr_inc = WM_DMA_ADDR_FIXED;
        tx_dma_cfg->ctrl.nonchain.data_unit     = WM_DMA_XFER_UNIT_WORD;
        tx_dma_cfg->ctrl.nonchain.burst_size    = WM_DMA_BURST_1UNIT;
        tx_dma_cfg->ctrl.nonchain.len           = tx_len / 4 * 4; //must 4 byte algin

        if (en_int) {
            tx_dma_cfg->extend_ctrl.int_en = WM_DMA_CH_EN_CTRL_ENABLE;
        } else {
            tx_dma_cfg->extend_ctrl.int_en = WM_DMA_CH_EN_CTRL_DISABLE;
        }
        tx_dma_cfg->extend_ctrl.int_type     = WM_DMA_INT_XFER_DONE;
        tx_dma_cfg->extend_ctrl.mode         = WM_DMA_HW_MODE;
        tx_dma_cfg->extend_ctrl.chain_mode   = WM_DMA_CHAIN_MODE_NORMAL;
        tx_dma_cfg->extend_ctrl.warp_mode_en = WM_DMA_WARP_CTRL_DISABLE;
        wm_hal_dma_clr_req(dev->dma_hal_dev, tx_dma_cfg->extend_ctrl.req_sel);
        tx_dma_cfg->extend_ctrl.req_sel = WM_DMA_LSPI_TX_REQ;
        tx_dma_cfg->extend_ctrl.ch      = dev->tx_dma_channel;

        tx_dma_cfg->src_addr  = (uint32_t)tx_buf;
        tx_dma_cfg->dest_addr = wm_ll_spi_get_tx_data_address(spi_reg);

        if (en_callback) {
            if (wm_hal_dma_register_callback(dev->dma_hal_dev, dev->tx_dma_channel, wm_hal_spim_tx_dma_done_callback, dev) !=
                WM_HAL_DMA_STATUS_SUCCESS) {
                ret = WM_ERR_FAILED;
                break;
            }
        }

        if (wm_hal_dma_config(dev->dma_hal_dev, tx_dma_cfg) != WM_HAL_DMA_STATUS_SUCCESS) {
            ret = WM_ERR_FAILED;
            break;
        }
    } while (0);

    return ret;
}

static int wm_hal_spim_dma_prepare_rx(wm_hal_spim_dev_t *dev, uint8_t *rx_buf, uint32_t rx_len, bool en_int, bool en_callback)
{
    int ret                       = WM_ERR_SUCCESS;
    wm_spi_reg_t *spi_reg         = NULL;
    wm_hal_dma_desc_t *rx_dma_cfg = NULL;

    rx_dma_cfg = &dev->rx_dma_cfg;
    spi_reg    = (wm_spi_reg_t *)dev->register_base;

    do {
        //configure DMA
        rx_dma_cfg->ctrl.nonchain.auto_reload   = WM_DMA_RELOAD_DISABLE;
        rx_dma_cfg->ctrl.nonchain.src_addr_inc  = WM_DMA_ADDR_FIXED;
        rx_dma_cfg->ctrl.nonchain.dest_addr_inc = WM_DMA_ADDR_INC;
        rx_dma_cfg->ctrl.nonchain.data_unit     = WM_DMA_XFER_UNIT_WORD;
        rx_dma_cfg->ctrl.nonchain.burst_size    = WM_DMA_BURST_1UNIT;
        rx_dma_cfg->ctrl.nonchain.len           = rx_len / 4 * 4; //must 4 byte align

        rx_dma_cfg->extend_ctrl.mode         = WM_DMA_HW_MODE;
        rx_dma_cfg->extend_ctrl.chain_mode   = WM_DMA_CHAIN_MODE_NORMAL;
        rx_dma_cfg->extend_ctrl.warp_mode_en = WM_DMA_WARP_CTRL_DISABLE;
        wm_hal_dma_clr_req(dev->dma_hal_dev, rx_dma_cfg->extend_ctrl.req_sel);
        rx_dma_cfg->extend_ctrl.req_sel = WM_DMA_LSPI_RX_REQ;
        rx_dma_cfg->extend_ctrl.ch      = dev->rx_dma_channel;
        if (en_int) {
            rx_dma_cfg->extend_ctrl.int_en = WM_DMA_CH_EN_CTRL_ENABLE;
        } else {
            rx_dma_cfg->extend_ctrl.int_en = WM_DMA_CH_EN_CTRL_DISABLE;
        }
        rx_dma_cfg->extend_ctrl.int_type = WM_DMA_INT_XFER_DONE;
        rx_dma_cfg->src_addr             = wm_ll_spi_get_rx_address(spi_reg);
        rx_dma_cfg->dest_addr            = (uint32_t)rx_buf;

        if (en_callback) {
            wm_hal_dma_register_callback(dev->dma_hal_dev, dev->rx_dma_channel, wm_hal_spim_rx_dma_done_callback, dev);
        }

        if (wm_hal_dma_config(dev->dma_hal_dev, rx_dma_cfg) != WM_HAL_DMA_STATUS_SUCCESS) {
            ret = WM_ERR_FAILED;
            break;
        }
    } while (0);

    return ret;
}

int wm_hal_spim_tx_polling(wm_hal_spim_dev_t *dev, uint8_t *tx_buf, uint32_t tx_len, uint32_t timeout_ms)
{
    uint32_t tx_fill_len    = 0;
    uint32_t int_status     = 0;
    wm_spi_reg_t *spi_reg   = NULL;
    uint8_t stat_case       = 0;
    uint32_t turnaround_cnt = 0;
    uint32_t max_support_us = 0;
    uint32_t load_value     = 0;
    uint32_t cpu_clock      = 0;
    uint32_t start_tick = 0, cur_tick = 0;

    if (!dev || !tx_buf) {
        return WM_ERR_INVALID_PARAM;
    }

    spi_reg = (wm_spi_reg_t *)dev->register_base;
    wm_hal_irq_disable(dev->irq_no);

    if (wm_wait_spi_idle(dev, WM_WAIT_SPI_IDLE_TIME) != WM_ERR_SUCCESS) {
        wm_ll_spi_set_ch_cfg_clr_fifo(spi_reg, 1);
        return WM_ERR_TIMEOUT;
    }

    wm_ll_spi_reset_int_mask(spi_reg, 0xFF); //disable all interrupt
    wm_ll_spi_set_ch_cfg_tx_ch_on(spi_reg, 1);
    wm_ll_spi_set_mode_cfg_tx_dma_on(spi_reg, 0);
    wm_ll_spi_set_ch_cfg_rx_ch_on(spi_reg, 0);
    wm_ll_spi_set_ch_cfg_clr_fifo(spi_reg, 1);

    cpu_clock      = wm_hal_rcc_get_clock_config(dev->rcc_hal_dev, WM_RCC_TYPE_CPU);
    load_value     = csi_coret_get_load();
    max_support_us = load_value / cpu_clock;
    turnaround_cnt = timeout_ms * 1000 / max_support_us;
    start_tick     = csi_coret_get_load() / 2;
    turnaround_cnt += turnaround_cnt;
    cur_tick = csi_coret_get_value();

    while (tx_len && turnaround_cnt) {
        tx_fill_len = wm_hal_spi_fill_tx_fifo(dev, tx_buf, tx_len);
        wm_ll_spi_set_ch_cfg_tx_rx_len(spi_reg, tx_fill_len * 8);
        wm_ll_spi_set_ch_cfg_start(spi_reg, 1);
        tx_len -= tx_fill_len;
        tx_buf += tx_fill_len;
        if (wm_wait_spi_idle(dev, WM_WAIT_SPI_IDLE_TIME) != WM_ERR_SUCCESS) {
            wm_ll_spi_set_ch_cfg_clr_fifo(spi_reg, 1);
            return WM_ERR_TIMEOUT;
        }

        cur_tick = csi_coret_get_value();
        if (stat_case == 1) {
            if (start_tick > cur_tick) {
                stat_case = 2;
                turnaround_cnt--;
            }
        } else {
            if (start_tick <= cur_tick) {
                stat_case = 1;
                turnaround_cnt--;
            }
        }
    }

    wm_ll_spi_set_ch_cfg_tx_ch_on(spi_reg, 0);
    //clear interrupt
    int_status = wm_ll_spi_get_int_status(spi_reg);
    wm_ll_spi_reset_int_status(spi_reg, int_status);

    wm_hal_irq_enable(dev->irq_no);

    return turnaround_cnt ? WM_ERR_SUCCESS : WM_ERR_TIMEOUT;
}

int wm_hal_spim_tx_it(wm_hal_spim_dev_t *dev, uint8_t *tx_buf, uint32_t tx_len)
{
    uint32_t tx_fill_len    = 0;
    wm_spi_reg_t *spi_reg   = NULL;
    wm_spi_hal_priv_t *priv = (wm_spi_hal_priv_t *)dev->priv;

    if (!dev || !tx_buf || tx_len > WM_SPI_MAX_TXRX_LEN) {
        return WM_ERR_INVALID_PARAM;
    }

    spi_reg = (wm_spi_reg_t *)dev->register_base;

    if (wm_wait_spi_idle(dev, WM_WAIT_SPI_IDLE_TIME) != WM_ERR_SUCCESS) {
        wm_ll_spi_set_ch_cfg_clr_fifo(spi_reg, 1);
        return WM_ERR_TIMEOUT;
    }

    wm_ll_spi_set_ch_cfg_clr_fifo(spi_reg, 1);
    wm_ll_spi_set_mode_cfg_tx_trig_level(spi_reg, 1);
    wm_ll_spi_reset_int_mask(spi_reg, 0xFF);               //disable all interrupt
    wm_ll_spi_set_int_mask_cfg_tx_fifo_rdy_en(spi_reg, 0); //enable TX FIFO ready interrupt

    tx_fill_len = wm_hal_spi_fill_tx_fifo(dev, tx_buf, tx_len);
    wm_ll_spi_set_ch_cfg_tx_rx_len(spi_reg, tx_fill_len * 8);
    wm_ll_spi_set_ch_cfg_rx_ch_on(spi_reg, 0);
    wm_ll_spi_set_mode_cfg_tx_dma_on(spi_reg, 0); //close dma tx
    wm_ll_spi_set_ch_cfg_tx_ch_on(spi_reg, 1);

    wm_hal_irq_enable(dev->irq_no);
    if (tx_len > tx_fill_len) {
        priv->tx_rx_it_handler = wm_hal_spim_tx_it_handler;
        priv->tx_buf           = tx_buf + tx_fill_len;
        priv->remain_tx_len    = tx_len - tx_fill_len;
        priv->rx_buf           = NULL;
        priv->remain_rx_len    = 0;
        wm_ll_spi_set_ch_cfg_start(spi_reg, 1);
    } else {                                     //tx done, len < max fifo level
        wm_ll_spi_reset_int_mask(spi_reg, 0xFF); //disable all interrupt
        wm_ll_spi_set_ch_cfg_start(spi_reg, 1);
        if (wm_wait_spi_idle(dev, WM_WAIT_SPI_IDLE_TIME) != WM_ERR_SUCCESS) {
            wm_ll_spi_set_ch_cfg_clr_fifo(spi_reg, 1);
            return WM_ERR_TIMEOUT;
        }
        priv->tx_rx_it_handler = NULL;
        priv->remain_tx_len    = 0;
        priv->remain_rx_len    = 0;
        if (priv->xfer_done_callback) {
            priv->xfer_done_callback(priv->xfer_callback_arg, 0);
            wm_spim_log_debug("tx interrupt done.\n");
        }
    }

    return WM_ERR_SUCCESS;
}

int wm_hal_spim_tx_dma(wm_hal_spim_dev_t *dev, uint8_t *tx_buf, uint32_t tx_len)
{
    int ret                       = WM_ERR_SUCCESS;
    wm_spi_reg_t *spi_reg         = NULL;
    wm_hal_dma_desc_t *tx_dma_cfg = NULL;
    uint32_t tmp_tx_data          = 0;
    uint8_t tx_addr_offst         = (uint32_t)tx_buf % 4;
    uint8_t tx_byte               = 0;

    if (!dev || !tx_buf || !tx_len || tx_len > WM_SPI_MAX_TXRX_LEN) {
        return WM_ERR_INVALID_PARAM;
    }
    spi_reg    = (wm_spi_reg_t *)dev->register_base;
    tx_dma_cfg = &dev->tx_dma_cfg;
    memset(tx_dma_cfg, 0, sizeof(wm_hal_dma_desc_t));

    wm_ll_spi_reset_int_mask(spi_reg, 0xFF); //disable all interrupt
    wm_ll_spi_set_mode_cfg_tx_trig_level(spi_reg, 3);
    wm_ll_spi_set_mode_cfg_tx_dma_on(spi_reg, 1);
    wm_ll_spi_set_ch_cfg_rx_ch_on(spi_reg, 0);
    wm_ll_spi_set_ch_cfg_tx_ch_on(spi_reg, 1);
    wm_hal_irq_disable(dev->irq_no);

    if (tx_addr_offst) {
        tx_byte = 4 - tx_addr_offst;

        for (int i = 0; i < tx_byte; i++) {
            tmp_tx_data |= tx_buf[i] << (i * 8);
        }
        wm_ll_spi_set_tx_data_address(spi_reg, tmp_tx_data);
        wm_ll_spi_set_ch_cfg_tx_rx_len(spi_reg, (tx_byte) * 8);
        wm_ll_spi_set_ch_cfg_start(spi_reg, 1);
        if (wm_wait_spi_idle(dev, WM_WAIT_SPI_IDLE_TIME) != WM_ERR_SUCCESS) {
            wm_ll_spi_set_ch_cfg_clr_fifo(spi_reg, 1);
            return WM_ERR_TIMEOUT;
        }
        tx_len -= tx_byte;
        tx_buf += tx_byte;
    }

    ret = wm_hal_spim_dma_prepare_tx(dev, tx_buf, tx_len, true, true);
    if (ret == WM_ERR_SUCCESS) {
        ret = wm_hal_dma_start(dev->dma_hal_dev, dev->tx_dma_channel);
    }

    if (ret == WM_ERR_SUCCESS) {
        wm_ll_spi_set_ch_cfg_tx_rx_len(spi_reg, tx_dma_cfg->ctrl.nonchain.len * 8);

        wm_ll_spi_set_ch_cfg_start(spi_reg, 1);
    }

    return ret;
}

int wm_hal_spim_rx_polling(wm_hal_spim_dev_t *dev, uint8_t *rx_buf, uint32_t rx_len, uint32_t timeout_ms)
{
    uint32_t int_status     = 0;
    uint32_t cur_rx_len     = 0;
    wm_spi_reg_t *spi_reg   = NULL;
    uint8_t stat_case       = 0;
    uint32_t turnaround_cnt = 0;
    uint32_t max_support_us = 0;
    uint32_t load_value     = 0;
    uint32_t cpu_clock      = 0;
    uint32_t start_tick = 0, cur_tick = 0;

    if (!dev || !rx_buf || !dev->rcc_hal_dev) {
        return WM_ERR_INVALID_PARAM;
    }

    cpu_clock      = wm_hal_rcc_get_clock_config(dev->rcc_hal_dev, WM_RCC_TYPE_CPU);
    load_value     = csi_coret_get_load();
    max_support_us = load_value / cpu_clock;
    turnaround_cnt = timeout_ms * 1000 / max_support_us;

    spi_reg = (wm_spi_reg_t *)dev->register_base;
    if (wm_wait_spi_idle(dev, WM_WAIT_SPI_IDLE_TIME) != WM_ERR_SUCCESS) {
        wm_ll_spi_set_ch_cfg_clr_fifo(spi_reg, 1);
        return WM_ERR_TIMEOUT;
    }

    wm_ll_spi_reset_int_mask(spi_reg, 0xFF); //disable all interrupt
    wm_ll_spi_set_ch_cfg_tx_ch_on(spi_reg, 1);
    wm_ll_spi_set_mode_cfg_tx_dma_on(spi_reg, 0);
    wm_ll_spi_set_ch_cfg_rx_ch_on(spi_reg, 1);
    wm_ll_spi_set_ch_cfg_continue_mode(spi_reg, 1);
    wm_ll_spi_set_ch_cfg_clr_fifo(spi_reg, 1);

    wm_hal_irq_disable(dev->irq_no);

    start_tick = csi_coret_get_load() / 2;
    turnaround_cnt += turnaround_cnt;
    cur_tick = csi_coret_get_value();

    while (rx_len && turnaround_cnt) {
        wm_ll_spi_set_ch_cfg_tx_rx_len(spi_reg, WM_SPI_TX_RX_FIFO_DEPTH * 8);
        wm_ll_spi_set_ch_cfg_start(spi_reg, 1);
        if (wm_wait_spi_idle(dev, WM_WAIT_SPI_IDLE_TIME) != WM_ERR_SUCCESS) {
            wm_ll_spi_set_ch_cfg_clr_fifo(spi_reg, 1);
            return WM_ERR_TIMEOUT;
        }
        cur_rx_len = wm_hal_spi_get_rx_fifo(dev, rx_buf, rx_len);
        wm_ll_spi_set_ch_cfg_rx_invalid_bit(spi_reg, 0);
        rx_len -= cur_rx_len;
        rx_buf += cur_rx_len;

        cur_tick = csi_coret_get_value();
        if (stat_case == 1) {
            if (start_tick > cur_tick) {
                stat_case = 2;
                turnaround_cnt--;
            }
        } else {
            if (start_tick <= cur_tick) {
                stat_case = 1;
                turnaround_cnt--;
            }
        }
    }

    wm_ll_spi_set_ch_cfg_rx_ch_on(spi_reg, 0);
    //clear interrupt
    int_status = wm_ll_spi_get_int_status(spi_reg);
    wm_ll_spi_reset_int_status(spi_reg, int_status);
    wm_hal_irq_enable(dev->irq_no);

    return turnaround_cnt ? WM_ERR_SUCCESS : WM_ERR_TIMEOUT;
}

int wm_hal_spim_rx_it(wm_hal_spim_dev_t *dev, uint8_t *rx_buf, uint32_t rx_len)
{
    uint32_t fill_len       = 0;
    wm_spi_reg_t *spi_reg   = NULL;
    wm_spi_hal_priv_t *priv = (wm_spi_hal_priv_t *)dev->priv;

    if (!dev || !rx_buf) {
        return WM_ERR_INVALID_PARAM;
    }

    spi_reg = (wm_spi_reg_t *)dev->register_base;

    if (wm_wait_spi_idle(dev, WM_WAIT_SPI_IDLE_TIME) != WM_ERR_SUCCESS) {
        wm_ll_spi_set_ch_cfg_clr_fifo(spi_reg, 1);
        return WM_ERR_TIMEOUT;
    }

    wm_ll_spi_set_ch_cfg_clr_fifo(spi_reg, 1);
    wm_ll_spi_set_mode_cfg_rx_trig_level(spi_reg, 7);
    wm_ll_spi_reset_int_mask(spi_reg, 0xFF);               //disable all interrupt
    wm_ll_spi_set_int_mask_cfg_rx_fifo_rdy_en(spi_reg, 0); //enable RX FIFO ready interrupt

    fill_len = (rx_len < WM_SPI_TX_RX_FIFO_DEPTH) ? rx_len : WM_SPI_TX_RX_FIFO_DEPTH;
    wm_ll_spi_set_ch_cfg_tx_rx_len(spi_reg, fill_len * 8);
    wm_ll_spi_set_ch_cfg_rx_ch_on(spi_reg, 1);
    wm_ll_spi_set_ch_cfg_tx_ch_on(spi_reg, 1);
    wm_ll_spi_set_ch_cfg_continue_mode(spi_reg, 1);

    priv->rx_it_handler = wm_hal_spim_rx_it_handler;
    priv->rx_buf        = rx_buf;
    priv->remain_rx_len = rx_len;
    priv->rx_len        = rx_len;

    wm_hal_irq_enable(dev->irq_no);

    wm_ll_spi_set_ch_cfg_start(spi_reg, 1);

    return WM_ERR_SUCCESS;
}

int wm_hal_spim_rx_dma(wm_hal_spim_dev_t *dev, uint8_t *rx_buf, uint32_t rx_len)
{
    uint8_t rx_addr_offst         = (uint32_t)rx_buf % 4;
    uint8_t rx_byte               = 0;
    int ret                       = WM_ERR_SUCCESS;
    wm_spi_reg_t *spi_reg         = NULL;
    wm_hal_dma_desc_t *rx_dma_cfg = NULL;

    if (!dev || !rx_buf || !rx_len) {
        return WM_ERR_INVALID_PARAM;
    }

    spi_reg    = (wm_spi_reg_t *)dev->register_base;
    rx_dma_cfg = &dev->rx_dma_cfg;
    memset(rx_dma_cfg, 0, sizeof(wm_hal_dma_desc_t));

    wm_ll_spi_set_ch_cfg_clr_fifo(spi_reg, 1);
    wm_ll_spi_set_ch_cfg_rx_ch_on(spi_reg, 1);
    wm_ll_spi_set_ch_cfg_tx_ch_on(spi_reg, 1);
    wm_ll_spi_set_ch_cfg_continue_mode(spi_reg, 1);

    if (rx_addr_offst) {
        rx_byte = 4 - rx_addr_offst;
        wm_ll_spi_set_ch_cfg_tx_rx_len(spi_reg, rx_byte * 8);
        wm_ll_spi_set_ch_cfg_start(spi_reg, 1);
        if (wm_wait_spi_idle(dev, WM_WAIT_SPI_IDLE_TIME) != WM_ERR_SUCCESS) {
            wm_ll_spi_set_ch_cfg_clr_fifo(spi_reg, 1);
            return WM_ERR_TIMEOUT;
        }
        rx_byte = wm_hal_spi_get_rx_fifo(dev, rx_buf, rx_len);
        rx_len -= rx_byte;
        rx_buf += rx_byte;
    }

    wm_hal_irq_disable(dev->irq_no);

    ret = wm_hal_spim_dma_prepare_rx(dev, rx_buf, rx_len, true, true);
    if (ret == WM_ERR_SUCCESS) {
        ret = wm_hal_dma_start(dev->dma_hal_dev, dev->rx_dma_channel);
        wm_ll_spi_set_ch_cfg_tx_rx_len(spi_reg, rx_dma_cfg->ctrl.nonchain.len * 8);
        wm_ll_spi_set_mode_cfg_rx_dma_on(spi_reg, 1);
        wm_ll_spi_set_ch_cfg_rx_ch_on(spi_reg, 1);
        wm_ll_spi_set_mode_cfg_rx_trig_level(spi_reg, 0);
        wm_ll_spi_set_ch_cfg_start(spi_reg, 1);
    }

    return ret;
}

static uint8_t wm_hal_spim_tx_handler_cmd_addr(wm_hal_spim_dev_t *dev)
{
    uint8_t rx_invalid_bits = 0;
    uint8_t *data_buf       = NULL;
    uint8_t *tmp_buf        = NULL;
    uint8_t data_len        = 0;
    uint8_t fill_len        = 0;
    uint8_t idx             = 0;
    wm_spi_reg_t *spi_reg   = (wm_spi_reg_t *)dev->register_base;

    rx_invalid_bits = dev->cmd_len * 8 + dev->addr_len * 8 + dev->dummy_bits;

    data_len = (dev->dummy_bits % 8 ? 1 : 0);
    data_len += dev->cmd_len + dev->addr_len + dev->dummy_bits / 8;
    fill_len = data_len;
    if (data_len % 4) {
        fill_len += (4 - data_len % 4); //fill with word
    }

    if (data_len) {
        if ((tmp_buf = wm_os_internal_malloc(fill_len)) == NULL) {
            return 0;
        }
        if ((data_buf = wm_os_internal_malloc(fill_len)) == NULL) {
            wm_os_internal_free(tmp_buf);
            return 0;
        }
        memset(data_buf, 0xFF, fill_len);
    }

    if (dev->cmd_len) {
        memcpy(data_buf, &(dev->cmd), dev->cmd_len);
        idx += dev->cmd_len;
    }

    if (dev->addr_len) {
        memcpy(data_buf + idx, &(dev->addr), dev->addr_len);
    }

    dev->addr_len   = 0;
    dev->cmd_len    = 0;
    dev->dummy_bits = 0;

    wm_ll_spi_set_ch_cfg_rx_invalid_bit(spi_reg, rx_invalid_bits);

    if (data_buf) {
        if (dev->big_endian) {
            if (data_len < 4) { //for cmd+addr less 4byte in big endian
                tmp_buf[0] = data_buf[3];
                tmp_buf[1] = data_buf[2];
                tmp_buf[2] = data_buf[1];
                tmp_buf[3] = data_buf[0];
            } else {
                for (int i = 0; i < fill_len; i += 4) { //command must be in the first word if more than 4byte
                    tmp_buf[i]     = data_buf[fill_len - i - 4];
                    tmp_buf[i + 1] = data_buf[fill_len - i - 3];
                    tmp_buf[i + 2] = data_buf[fill_len - i - 2];
                    tmp_buf[i + 3] = data_buf[fill_len - i - 1];
                }
            }
            wm_hal_spi_fill_tx_fifo(dev, tmp_buf, fill_len);
        } else {
            wm_hal_spi_fill_tx_fifo(dev, data_buf, fill_len);
        }
        wm_os_internal_free(data_buf);
        wm_os_internal_free(tmp_buf);
    }

    return data_len;
}

int wm_hal_spim_tx_rx_polling(wm_hal_spim_dev_t *dev, uint8_t *tx_buf, uint32_t tx_len, uint8_t *rx_buf, uint32_t rx_len,
                              uint32_t timeout_ms)
{
    uint16_t tx_rx_len      = 0;
    uint32_t cur_rx_len     = 0;
    int32_t remain_rx_len   = rx_len;
    int32_t ret             = WM_ERR_SUCCESS;
    uint32_t tx_fill_len    = 0;
    uint32_t int_status     = 0;
    wm_spi_reg_t *spi_reg   = NULL;
    wm_spi_hal_priv_t *priv = NULL;
    uint8_t stat_case       = 0;
    uint32_t turnaround_cnt = 0;
    uint32_t max_support_us = 0;
    uint32_t load_value     = 0;
    uint32_t cpu_clock      = 0;
    uint32_t start_tick = 0, cur_tick = 0;
    uint8_t cmd_fill_len = 0;

    if (!dev) {
        return WM_ERR_INVALID_PARAM;
    }

    priv    = (wm_spi_hal_priv_t *)dev->priv;
    spi_reg = (wm_spi_reg_t *)dev->register_base;

    cpu_clock      = wm_hal_rcc_get_clock_config(dev->rcc_hal_dev, WM_RCC_TYPE_CPU);
    load_value     = csi_coret_get_load();
    max_support_us = load_value / cpu_clock;
    turnaround_cnt = timeout_ms * 1000 / max_support_us;

    if (wm_wait_spi_idle(dev, WM_WAIT_SPI_IDLE_TIME) != WM_ERR_SUCCESS) {
        wm_ll_spi_set_ch_cfg_clr_fifo(spi_reg, 1);
        return WM_ERR_TIMEOUT;
    }

    wm_ll_spi_reset_int_mask(spi_reg, 0xFF); //disable all interrupt
    wm_ll_spi_set_ch_cfg_clr_fifo(spi_reg, 1);
    if (rx_len) {
        wm_ll_spi_set_ch_cfg_rx_ch_on(spi_reg, 1);
    } else {
        wm_ll_spi_set_ch_cfg_rx_ch_on(spi_reg, 0);
    }
    wm_ll_spi_set_ch_cfg_tx_ch_on(spi_reg, 1);
    wm_ll_spi_set_ch_cfg_continue_mode(spi_reg, 1);

    wm_hal_irq_disable(dev->irq_no);

    cmd_fill_len = wm_hal_spim_tx_handler_cmd_addr(dev);

    start_tick = csi_coret_get_load() / 2;
    turnaround_cnt += turnaround_cnt;
    cur_tick = csi_coret_get_value();

    while (((tx_len || remain_rx_len > 0) && turnaround_cnt) || cmd_fill_len) {
        if (tx_len) {
            tx_fill_len = wm_hal_spi_fill_tx_fifo(dev, tx_buf, tx_len);
            tx_len -= tx_fill_len;
            tx_buf += tx_fill_len;
            tx_rx_len = tx_fill_len + cmd_fill_len;
        } else {
            tx_rx_len = (WM_SPI_TX_RX_FIFO_DEPTH < remain_rx_len) ? WM_SPI_TX_RX_FIFO_DEPTH : remain_rx_len;
            if (tx_rx_len < cmd_fill_len) {
                tx_rx_len = cmd_fill_len; //case: tx and rx is NULL
            } else {
                tx_rx_len += cmd_fill_len;
            }
        }
        cmd_fill_len = 0;

        if (!remain_rx_len) {
            wm_ll_spi_set_ch_cfg_rx_ch_on(spi_reg, 0); /// disable rx channel
        }
        wm_ll_spi_set_ch_cfg_tx_rx_len(spi_reg, tx_rx_len * 8);
        wm_ll_spi_set_ch_cfg_start(spi_reg, 1);
        if (wm_wait_spi_idle(dev, WM_WAIT_SPI_IDLE_TIME) != WM_ERR_SUCCESS) {
            wm_ll_spi_set_ch_cfg_clr_fifo(spi_reg, 1);
            return WM_ERR_TIMEOUT;
        }

        if (rx_buf && remain_rx_len) {
            cur_rx_len = wm_hal_spi_get_rx_fifo(dev, rx_buf, remain_rx_len);
            if (cur_rx_len <= remain_rx_len) {
                remain_rx_len -= cur_rx_len;
            } else {
                remain_rx_len = 0;
            }
            rx_buf += cur_rx_len;
            wm_ll_spi_set_ch_cfg_rx_invalid_bit(spi_reg, 0);
            priv->rx_invalid_bit = 0;
        }

        cur_tick = csi_coret_get_value();
        if (stat_case == 1) {
            if (start_tick > cur_tick) {
                stat_case = 2;
                turnaround_cnt--;
            }
        } else {
            if (start_tick <= cur_tick) {
                stat_case = 1;
                turnaround_cnt--;
            }
        }
    }
    wm_hal_irq_enable(dev->irq_no);

    wm_ll_spi_set_ch_cfg_rx_ch_on(spi_reg, 0);
    wm_ll_spi_set_ch_cfg_tx_ch_on(spi_reg, 0);
    wm_ll_spi_set_ch_cfg_continue_mode(spi_reg, 0);

    //clear interrupt
    int_status = wm_ll_spi_get_int_status(spi_reg);
    wm_ll_spi_reset_int_status(spi_reg, int_status);

    if (turnaround_cnt == 0) {
        ret = WM_ERR_TIMEOUT;
    }

    return ret;
}

int wm_hal_spim_tx_rx_it(wm_hal_spim_dev_t *dev, uint8_t *tx_buf, uint32_t tx_len, uint8_t *rx_buf, uint32_t rx_len)
{
    int ret                 = WM_ERR_SUCCESS;
    uint32_t tx_fill_len    = 0;
    wm_spi_reg_t *spi_reg   = NULL;
    wm_spi_hal_priv_t *priv = (wm_spi_hal_priv_t *)dev->priv;
    uint8_t cmd_fill_len    = 0;

    if (!dev) {
        return WM_ERR_INVALID_PARAM;
    }

    spi_reg = (wm_spi_reg_t *)dev->register_base;
    if (wm_wait_spi_idle(dev, WM_WAIT_SPI_IDLE_TIME) != WM_ERR_SUCCESS) {
        wm_ll_spi_set_ch_cfg_clr_fifo(spi_reg, 1);
        return WM_ERR_TIMEOUT;
    }

    wm_ll_spi_set_ch_cfg_clr_fifo(spi_reg, 1);
    wm_ll_spi_set_mode_cfg_tx_trig_level(spi_reg, 1);
    wm_ll_spi_reset_int_mask(spi_reg, 0xFF);        //disable all interrupt
    wm_ll_spi_set_int_mask_cfg_done_en(spi_reg, 0); //enable TX & RX done interrupt
    wm_ll_spi_set_ch_cfg_rx_ch_on(spi_reg, 1);
    wm_ll_spi_set_ch_cfg_tx_ch_on(spi_reg, 1);
    wm_ll_spi_set_ch_cfg_continue_mode(spi_reg, 1);

    cmd_fill_len = wm_hal_spim_tx_handler_cmd_addr(dev);

    // enable SPI interrupt
    wm_hal_irq_enable(dev->irq_no);

    if (tx_buf && tx_len) {
        tx_fill_len         = wm_hal_spi_fill_tx_fifo(dev, tx_buf, tx_len);
        priv->remain_tx_len = tx_len - tx_fill_len;
        priv->tx_buf        = tx_buf + tx_fill_len;
        tx_fill_len += cmd_fill_len;
    } else {
        priv->remain_tx_len = 0;
        priv->tx_buf        = NULL;
        tx_fill_len         = (WM_SPI_TX_RX_FIFO_DEPTH < rx_len) ? WM_SPI_TX_RX_FIFO_DEPTH : rx_len;
        if (tx_fill_len < cmd_fill_len) {
            tx_fill_len = cmd_fill_len; //case: tx and rx is NULL
        } else {
            tx_fill_len += cmd_fill_len;
        }
    }

    wm_ll_spi_set_ch_cfg_tx_rx_len(spi_reg, tx_fill_len * 8);

    if (rx_buf && rx_len) {
        priv->rx_buf        = rx_buf;
        priv->remain_rx_len = rx_len;
    } else {
        priv->remain_rx_len = 0;
        priv->rx_buf        = NULL;
    }

    priv->tx_rx_it_handler = wm_hal_spim_tx_rx_it_handler;
    wm_ll_spi_set_ch_cfg_start(spi_reg, 1);

    return ret;
}

int wm_hal_spim_tx_rx_dma(wm_hal_spim_dev_t *dev, uint8_t *tx_buf, uint32_t tx_len, uint8_t *rx_buf, uint32_t rx_len)
{
    int ret                       = WM_ERR_SUCCESS;
    uint32_t tx_rx_len            = 0;
    wm_spi_reg_t *spi_reg         = NULL;
    wm_hal_dma_desc_t *tx_dma_cfg = NULL;
    wm_hal_dma_desc_t *rx_dma_cfg = NULL;
    wm_spi_hal_priv_t *priv       = NULL;
    bool tx_with_dma              = false;
    bool rx_with_dma              = false;
    uint32_t rx_with_dma_len      = 0;
    uint32_t tx_with_dma_len      = 0;
    uint8_t cmd_fill_len          = 0;

    if (!dev || (!tx_buf && !rx_buf) || (uint32_t)tx_buf % 4 || (uint32_t)rx_buf % 4) {
        return WM_ERR_INVALID_PARAM;
    }

    if ((tx_len > WM_SPI_MAX_TXRX_LEN || rx_len > WM_SPI_MAX_TXRX_LEN) || (tx_len && (tx_len < WM_SPI_USE_DMA_MIN_SIZE)) ||
        (rx_len && (rx_len < WM_SPI_USE_DMA_MIN_SIZE))) {
        return WM_ERR_INVALID_PARAM;
    }

    priv = (wm_spi_hal_priv_t *)dev->priv;

    tx_dma_cfg = &dev->tx_dma_cfg;
    memset(tx_dma_cfg, 0, sizeof(wm_hal_dma_desc_t));
    rx_dma_cfg = &dev->rx_dma_cfg;
    memset(rx_dma_cfg, 0, sizeof(wm_hal_dma_desc_t));
    spi_reg = (wm_spi_reg_t *)dev->register_base;
    if (wm_wait_spi_idle(dev, WM_WAIT_SPI_IDLE_TIME) != WM_ERR_SUCCESS) {
        wm_ll_spi_set_ch_cfg_clr_fifo(spi_reg, 1);
        return WM_ERR_TIMEOUT;
    }

    wm_ll_spi_set_ch_cfg_clr_fifo(spi_reg, 1);
    wm_ll_spi_set_ch_cfg_rx_ch_on(spi_reg, 0);
    wm_ll_spi_set_ch_cfg_tx_ch_on(spi_reg, 1);
    wm_ll_spi_set_ch_cfg_continue_mode(spi_reg, 1);
    wm_ll_spi_reset_int_mask(spi_reg, 0xFF); //disable all interrupt

    cmd_fill_len = wm_hal_spim_tx_handler_cmd_addr(dev);

    if (rx_buf && rx_len) {
        wm_ll_spi_set_ch_cfg_rx_ch_on(spi_reg, 1);

        if (tx_len) {
            rx_with_dma_len = rx_len < tx_len ? rx_len : tx_len;
        } else {
            rx_with_dma_len = rx_len;
        }

        if (rx_with_dma_len >= 4) {
            wm_hal_spim_dma_prepare_rx(dev, rx_buf, rx_with_dma_len, true, true);
            wm_ll_spi_set_mode_cfg_rx_dma_on(spi_reg, 1);
            wm_ll_spi_set_ch_cfg_rx_ch_on(spi_reg, 1);
            wm_ll_spi_set_mode_cfg_rx_trig_level(spi_reg, 0);
            rx_with_dma = true;
        }
    }

    if (tx_buf && tx_len) {
        if (rx_len >= 4) {
            tx_with_dma_len = tx_len < rx_len ? tx_len : rx_len;
        } else {
            tx_with_dma_len = tx_len;
        }

        if (tx_with_dma_len >= 4) {
            wm_ll_spi_set_mode_cfg_tx_trig_level(spi_reg, 0);
            wm_ll_spi_set_mode_cfg_tx_dma_on(spi_reg, 1);
            wm_hal_spim_dma_prepare_tx(dev, tx_buf, tx_with_dma_len, true, true);
            tx_with_dma = true;
        }
    }

    if (tx_with_dma && rx_with_dma) {
        wm_hal_dma_register_callback(dev->dma_hal_dev, dev->tx_dma_channel, wm_hal_spim_tx_rx_dma_done_callback, dev);
        wm_hal_dma_unregister_callback(dev->dma_hal_dev, dev->rx_dma_channel);
    }

    priv->rx_buf        = rx_buf;
    priv->rx_len        = rx_len;
    priv->remain_rx_len = rx_len;
    priv->tx_buf        = tx_buf;
    priv->tx_len        = tx_len;
    priv->remain_tx_len = tx_len;

    if (tx_with_dma) {
        tx_rx_len = tx_dma_cfg->ctrl.nonchain.len;
    } else if (rx_with_dma) {
        tx_rx_len = rx_dma_cfg->ctrl.nonchain.len;
    }

    if (tx_with_dma) {
        wm_hal_dma_start(dev->dma_hal_dev, dev->tx_dma_channel);
    }

    if (rx_with_dma) {
        wm_hal_dma_start(dev->dma_hal_dev, dev->rx_dma_channel);
    }

    if (tx_rx_len) {
        wm_ll_spi_set_ch_cfg_tx_rx_len(spi_reg, (tx_rx_len + cmd_fill_len) * 8);
        //start SPI
        wm_ll_spi_set_ch_cfg_start(spi_reg, 1);
    }

    return ret;
}

int wm_hal_spim_tx_rx_dma_sync(wm_hal_spim_dev_t *dev, uint8_t *tx_buf, uint32_t tx_len, uint8_t *rx_buf, uint32_t rx_len,
                               uint32_t timeout_ms)
{
    uint8_t rx_byte        = 0;
    uint32_t tx_rx_len     = 0;
    uint32_t remain_tx_len = 0, remain_rx_len = 0;
    int ret                         = WM_ERR_SUCCESS;
    wm_spi_reg_t *spi_reg           = NULL;
    wm_hal_dma_desc_t *tx_dma_cfg   = NULL;
    wm_hal_dma_desc_t *rx_dma_cfg   = NULL;
    wm_dma_sts_info_t tx_dma_status = { .sts = WM_DMA_RUNNIG, .xfer_cnt = 0 };
    wm_dma_sts_info_t rx_dma_status = { .sts = WM_DMA_RUNNIG, .xfer_cnt = 0 };
    bool tx_with_dma                = false;
    bool rx_with_dma                = false;
    uint32_t rx_with_dma_len        = 0;
    uint32_t tx_with_dma_len        = 0;
    uint8_t cmd_fill_len            = 0;

    if (!dev || (!tx_buf && !rx_buf) || (uint32_t)tx_buf % 4 || (uint32_t)rx_buf % 4) {
        return WM_ERR_INVALID_PARAM;
    }

    if (tx_len > WM_SPI_MAX_TXRX_LEN || rx_len > WM_SPI_MAX_TXRX_LEN) {
        return WM_ERR_INVALID_PARAM;
    }

    tx_dma_cfg = &dev->tx_dma_cfg;
    memset(tx_dma_cfg, 0, sizeof(wm_hal_dma_desc_t));
    rx_dma_cfg = &dev->rx_dma_cfg;
    memset(rx_dma_cfg, 0, sizeof(wm_hal_dma_desc_t));
    spi_reg = (wm_spi_reg_t *)dev->register_base;
    ret     = wm_wait_spi_idle(dev, WM_WAIT_SPI_IDLE_TIME);

    if (ret != WM_ERR_SUCCESS) {
        return ret;
    }

    wm_ll_spi_set_ch_cfg_clr_fifo(spi_reg, 1);
    wm_ll_spi_set_ch_cfg_rx_ch_on(spi_reg, 0);
    wm_ll_spi_set_ch_cfg_tx_ch_on(spi_reg, 1);
    wm_ll_spi_set_ch_cfg_continue_mode(spi_reg, 1);
    wm_ll_spi_reset_int_mask(spi_reg, 0xFF); //disable all interrupt

    cmd_fill_len = wm_hal_spim_tx_handler_cmd_addr(dev);

    if (rx_buf && rx_len) {
        wm_ll_spi_set_ch_cfg_rx_ch_on(spi_reg, 1);

        if (tx_len) {
            rx_with_dma_len = rx_len < tx_len ? rx_len : tx_len;
        } else {
            rx_with_dma_len = rx_len;
        }

        if (rx_with_dma_len >= 4) {
            wm_hal_spim_dma_prepare_rx(dev, rx_buf, rx_with_dma_len, false, false);
            wm_ll_spi_set_mode_cfg_rx_dma_on(spi_reg, 1);
            wm_ll_spi_set_ch_cfg_rx_ch_on(spi_reg, 1);
            wm_ll_spi_set_mode_cfg_rx_trig_level(spi_reg, 0);
            rx_with_dma = true;
        }
    }

    if (tx_buf && tx_len) {
        if (rx_len >= 4) {
            tx_with_dma_len = tx_len < rx_len ? tx_len : rx_len;
        } else {
            tx_with_dma_len = tx_len;
        }
        if (tx_with_dma_len >= 4) {
            wm_ll_spi_set_mode_cfg_tx_trig_level(spi_reg, 0);
            wm_ll_spi_set_mode_cfg_tx_dma_on(spi_reg, 1);
            wm_hal_spim_dma_prepare_tx(dev, tx_buf, tx_with_dma_len, false, false);
            tx_with_dma = true;
        }
    }

    if (tx_with_dma) {
        tx_rx_len = tx_dma_cfg->ctrl.nonchain.len;
    } else if (rx_with_dma) {
        tx_rx_len = rx_dma_cfg->ctrl.nonchain.len;
    }

    if (tx_rx_len) {
        wm_ll_spi_set_ch_cfg_tx_rx_len(spi_reg, (tx_rx_len + cmd_fill_len) * 8);
    }

    if (tx_with_dma) {
        wm_hal_dma_start(dev->dma_hal_dev, dev->tx_dma_channel);
    }

    if (tx_rx_len) {
        //start SPI
        wm_ll_spi_set_ch_cfg_start(spi_reg, 1);
    }

    if (rx_with_dma) {
        wm_hal_dma_start(dev->dma_hal_dev, dev->rx_dma_channel);
    }

    //wait dma TX done
    if (tx_with_dma) {
        while (tx_dma_status.sts == WM_DMA_RUNNIG) {
            wm_hal_dma_get_status(dev->dma_hal_dev, dev->tx_dma_channel, &tx_dma_status);
        }
    }

    //wait dma RX done
    if (rx_with_dma) {
        while (rx_dma_status.sts == WM_DMA_RUNNIG) {
            wm_hal_dma_get_status(dev->dma_hal_dev, dev->rx_dma_channel, &rx_dma_status);
        }
    }

    //wait SPI idle
    if (wm_wait_spi_idle(dev, timeout_ms * 1000) != WM_ERR_SUCCESS) {
        wm_ll_spi_set_ch_cfg_clr_fifo(spi_reg, 1);
        return WM_ERR_TIMEOUT;
    }

    //tx or rx done,
    wm_ll_spi_set_ch_cfg_rx_invalid_bit(spi_reg, 0);
    wm_ll_spi_set_mode_cfg_tx_dma_on(spi_reg, 0); //disable tx with dma
    wm_ll_spi_set_mode_cfg_rx_dma_on(spi_reg, 0); //disable tx with dma

    if (tx_len) {
        remain_tx_len = tx_len - tx_dma_status.xfer_cnt;
    }
    if (rx_len) {
        remain_rx_len = rx_len - rx_dma_status.xfer_cnt;
    }

    if (remain_tx_len || remain_rx_len) {
        if (remain_tx_len) {
            tx_buf += tx_len - remain_tx_len;
        } else {
            tx_buf = NULL;
        }

        if (remain_rx_len) {
            rx_buf += rx_len - remain_rx_len;
            rx_byte = wm_hal_spi_get_rx_fifo(dev, rx_buf, remain_rx_len);
            remain_rx_len -= rx_byte;
            rx_buf += rx_byte;
        } else {
            rx_buf = NULL;
        }
        ret = wm_hal_spim_tx_rx_polling(dev, tx_buf, remain_tx_len, rx_buf, remain_rx_len, timeout_ms);
    }

    return ret;
}

int wm_hal_spim_set_cmds(wm_hal_spim_dev_t *dev, spim_cmd_e cmd, uint32_t value)
{
    int ret                 = WM_ERR_SUCCESS;
    wm_spi_reg_t *spi_reg   = NULL;
    wm_spi_hal_priv_t *priv = NULL;

    if (!dev) {
        return WM_ERR_INVALID_PARAM;
    }
    priv = (wm_spi_hal_priv_t *)dev->priv;

    spi_reg = (wm_spi_reg_t *)dev->register_base;

    switch (cmd) {
        case SPIM_CMD_FRAM_FORMAT:
        {
            wm_ll_spi_set_cfg_frame_format(spi_reg, value);
            break;
        }
        case SPIM_CMD_RX_INVALID_BIT:
        {
            wm_ll_spi_set_ch_cfg_rx_invalid_bit(spi_reg, value);
            priv->rx_invalid_bit = value;
            break;
        }

        case SPIM_CMD_DATA_OUT_DELAY:
        {
            wm_ll_spi_set_cfg_spi_out_delay(spi_reg, value);
            break;
        }

        case SPIM_CMD_FRMAE_INTERVAL:
        {
            wm_ll_spi_set_cfg_frame_delay(spi_reg, value);
            break;
        }

        case SPIM_CMD_BIG_ENDIAN:
        {
            wm_ll_spi_set_cfg_big_endian(spi_reg, value);
            break;
        }

        case SPIM_CMD_CS_CTRL_BY_SW:
        {
            wm_ll_spi_set_ch_cfg_cs_out(spi_reg, value);
            break;
        }

        case SPIM_CMD_CS_INVALID_VALUE:
        {
            wm_ll_spi_set_ch_cfg_chip_sel(spi_reg, value);
            break;
        }

        case SPIM_CMD_FREQ:
        {
            wm_ll_spi_set_clk_cfg_divider(spi_reg, value);
            break;
        }

        case SPIM_CMD_MODE:
        {
            if (value == 0) { //mode 0
                wm_ll_spi_set_cfg_cpol(spi_reg, 0);
                wm_ll_spi_set_cfg_cpha(spi_reg, 0);
            } else if (value == 1) { //mode 1
                wm_ll_spi_set_cfg_cpol(spi_reg, 0);
                wm_ll_spi_set_cfg_cpha(spi_reg, 1);
            } else if (value == 2) { //mode 2
                wm_ll_spi_set_cfg_cpol(spi_reg, 1);
                wm_ll_spi_set_cfg_cpha(spi_reg, 0);
            } else if (value == 3) { //mode 3
                wm_ll_spi_set_cfg_cpol(spi_reg, 1);
                wm_ll_spi_set_cfg_cpha(spi_reg, 1);
            } else {
                ret = WM_ERR_INVALID_PARAM;
            }
            break;
        }

        default:
        {
            wm_spim_log_error("unspport set cmd: %d\n", cmd);
            ret = WM_ERR_INVALID_PARAM;
            break;
        }
    }

    return ret;
}

int wm_hal_spim_get_cmd_value(wm_hal_spim_dev_t *dev, spim_cmd_e cmd, uint16_t *value)
{
    int ret               = WM_ERR_SUCCESS;
    wm_spi_reg_t *spi_reg = NULL;

    if (!dev) {
        return WM_ERR_INVALID_PARAM;
    }

    spi_reg = (wm_spi_reg_t *)dev->register_base;

    switch (cmd) {
        case SPIM_CMD_FRAM_FORMAT:
        {
            *value = wm_ll_spi_get_cfg_frame_format(spi_reg);
            break;
        }
        case SPIM_CMD_RX_INVALID_BIT:
        {
            *value = wm_ll_spi_get_ch_cfg_rx_invalid_bit(spi_reg);
            break;
        }

        case SPIM_CMD_DATA_OUT_DELAY:
        {
            *value = wm_ll_spi_get_cfg_spi_out_delay(spi_reg);
            break;
        }

        case SPIM_CMD_FRMAE_INTERVAL:
        {
            *value = wm_ll_spi_get_cfg_frame_delay(spi_reg);
            break;
        }

        case SPIM_CMD_BIG_ENDIAN:
        {
            *value = wm_ll_spi_get_cfg_big_endian(spi_reg);
            break;
        }

        case SPIM_CMD_CS_CTRL_BY_SW:
        {
            *value = wm_ll_spi_get_ch_cfg_cs_out(spi_reg);
            break;
        }

        case SPIM_CMD_CS_INVALID_VALUE:
        {
            *value = wm_ll_spi_get_ch_cfg_chip_sel(spi_reg);
            break;
        }

        default:
        {
            wm_spim_log_error("unspport get cmd: %d\n", cmd);
            ret = WM_ERR_INVALID_PARAM;
            break;
        }
    }

    return ret;
}

int wm_hal_spim_init(wm_hal_spim_dev_t *dev, hal_spim_config_t *config)
{
    int ret               = WM_ERR_SUCCESS;
    wm_spi_reg_t *spi_reg = NULL;

    if (!dev || !dev->register_base) {
        return WM_ERR_INVALID_PARAM;
    }

    spi_reg = (wm_spi_reg_t *)dev->register_base;

    //clear tx & rx FIFO and reset chip
    wm_ll_spi_set_ch_cfg_clr_fifo(spi_reg, 1);

    //set endian
    wm_ll_spi_set_cfg_big_endian(spi_reg, 0);

    //set SPI_CS
    wm_ll_spi_set_ch_cfg_chip_sel(spi_reg, config->cs_active);
    wm_ll_spi_set_ch_cfg_cs_out(spi_reg, config->cs_control_by_sw);

    //set tx rx trigger level
    wm_ll_spi_set_mode_cfg_rx_trig_level(spi_reg, 0);
    wm_ll_spi_set_mode_cfg_tx_trig_level(spi_reg, 0);

    //open tx and rx channel
    wm_ll_spi_set_ch_cfg_tx_ch_on(spi_reg, 1);
    wm_ll_spi_set_ch_cfg_rx_ch_on(spi_reg, 1);

    wm_ll_spi_set_cfg_cs_setup_time(spi_reg, 1);
    wm_ll_spi_set_cfg_hold_time(spi_reg, 1);
    wm_ll_spi_set_cfg_spi_out_delay(spi_reg, 1);

    //set role is master
    wm_ll_spi_set_cfg_role(spi_reg, 1);

    dev->priv = &spi_priv;

    //enable SPI IRQ
    ret = wm_hal_irq_attach_sw_vector(dev->irq_no, wm_hal_spim_interrupt_handler, dev);
    if (!ret) {
        ret = wm_hal_irq_enable(dev->irq_no);
    }

    return ret;
}

//need disable spi interrupt and spi clock in driver layer
int wm_hal_spim_deinit(wm_hal_spim_dev_t *dev)
{
    int ret               = WM_ERR_SUCCESS;
    wm_spi_reg_t *spi_reg = NULL;

    if (!dev) {
        return WM_ERR_INVALID_PARAM;
    }

    wm_ll_spi_set_ch_cfg_clr_fifo(spi_reg, 1); //clear tx & rx FIFO and reset chip

    //disable interrupt
    ret = wm_hal_irq_detach_sw_vector(dev->irq_no);
    if (!ret) {
        ret = wm_hal_irq_disable(dev->irq_no);
    }

    return ret;
}

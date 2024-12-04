/*
 * @file wm_hal_sdh.c
 *
 * @brief sd/sdio host hardware abstraction layer implementation
 *
 */

/*
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
#include "wm_soc.h"
#include "wm_reg_op.h"
#include "wm_hal_sdh.h"
#include "core_804.h"

#define LOG_TAG "SDH HAL"
#include "wm_log.h"

#define WM_HAL_SDH_DEBUG 0
#if WM_HAL_SDH_DEBUG
#define LOG_TAG "SDH HAL"
#include "wm_log.h"
#define WM_HAL_SDH_LOG_D         wm_log_debug
#define WM_HAL_SDH_LOG_I         wm_log_info
#define WM_HAL_SDH_LOG_W         wm_log_warn
#define WM_HAL_SDH_LOG_E         wm_log_error
#define WM_HAL_SDH_LOG_DUMP      wm_log_dump
#define WM_HAL_SDH_FUNC_EXIT(fn) WM_HAL_SDH_LOG_D("%s exit", fn)
#define WM_HAL_SDH_FUNC_NAME     __func__
#else
#define WM_HAL_SDH_LOG_D(...)
#define WM_HAL_SDH_LOG_I(...)
#define WM_HAL_SDH_LOG_W(...)
#define WM_HAL_SDH_LOG_E(...)
#define WM_HAL_SDH_LOG_DUMP(...)
#define WM_HAL_SDH_FUNC_EXIT(fn)
#endif

int wm_hal_sdh_set_clock(wm_hal_sdh_dev_t *dev, uint32_t clock);

int wm_hal_sdh_init(wm_hal_sdh_dev_t *dev)
{
    int ret                   = WM_ERR_SUCCESS;
    wm_sdh_reg_t *reg         = dev->reg_base;
    wm_hal_sdh_cfg_t *sdh_cfg = &dev->sdh_cfg;

    if (dev == NULL || reg == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (sdh_cfg->port_mode >= WM_HAL_SDH_PORT_MODE_MAX || sdh_cfg->xfer_mode >= WM_HAL_SDH_XFER_MODE_MAX ||
        sdh_cfg->bus_width >= WM_SDH_BUS_WIDTH_MAX || sdh_cfg->sd_mode >= WM_HAL_SDH_SD_MODE_MAX) {
        return WM_ERR_INVALID_PARAM;
    }

    /*
     * SDMMC:
     * reg->mmc_cardsel.val  = 0xC0 | 14;
     * reg->mmc_ctrl.val     = 0x13 | (1 << 7);
     * reg->mmc_int_mask.val = 0x100;
     * reg->mmc_crcctl.val   = 0xC0;
     * reg->timeoutcnt.val   = 0xFF;
     * reg->mmc_io_mbctl.val = reg->mmc_io_mbctl.val | 0xF0;
     *
     * SPI:
     * reg->mmc_cardsel.val  = 0xC0 | 14;
     * reg->mmc_ctrl.val     = 0x542 | (1 << 3);
     * reg->mmc_int_mask.val = 0x100;
     * reg->mmc_crcctl.val   = 0x00;
     * reg->timeoutcnt.val   = 0;
     * reg->mmc_bytecntl.val = 0;
     */

    /* MMC_CARDSEL */
    ret = wm_hal_sdh_set_clock(dev, dev->sdh_cfg.clock_hz);
    if (ret != WM_ERR_SUCCESS) {
        return ret;
    }
    wm_ll_sdh_mmc_set_controller_enable(reg, WM_HAL_SDH_ENABLE);
    wm_ll_sdh_mmc_set_enable_card_clk(reg, WM_HAL_SDH_ENABLE);

    /* MMC_CTL */
    if (sdh_cfg->port_mode == WM_HAL_SDH_PORT_MODE_MMC) {
        wm_ll_sdh_mmc_set_sdio_read_wait_en(reg, WM_HAL_SDH_DISABLE);
    } else if (sdh_cfg->port_mode == WM_HAL_SDH_PORT_MODE_SPI) {
        wm_ll_sdh_mmc_set_sdio_read_wait_en(reg, WM_HAL_SDH_ENABLE);
    }
    wm_ll_sdh_mmc_set_sdio_mode_en(reg, sdh_cfg->sd_mode);
    wm_ll_sdh_mmc_set_sdio_transfer_mode(reg, sdh_cfg->xfer_mode);
    wm_ll_sdh_mmc_set_sdio_bus_width(reg, sdh_cfg->bus_width);
    wm_ll_sdh_mmc_set_sdio_drive_mode_sel(reg, WM_HAL_SDH_DRIVER_MODE_PUSH_PULL); //?
    wm_ll_sdh_mmc_set_sig_mode(reg, WM_HAL_SDH_SIGNAL_MODE_AUTO);
    wm_ll_sdh_mmc_set_port_mode(reg, sdh_cfg->port_mode);

    /* MMC_INT_MASK */
    wm_ll_sdh_mmc_set_sdio_data1_int_mask(reg, WM_HAL_SDH_ENABLE);

    /* MMC_CRCCTL */
    if (sdh_cfg->port_mode == WM_HAL_SDH_PORT_MODE_MMC) {
        wm_ll_sdh_mmc_set_cmd_line_crc_en(reg, WM_HAL_SDH_ENABLE);
        wm_ll_sdh_mmc_set_data_line_crc_en(reg, WM_HAL_SDH_ENABLE);
    } else if (sdh_cfg->port_mode == WM_HAL_SDH_PORT_MODE_SPI) {
        wm_ll_sdh_mmc_set_cmd_line_crc_en(reg, WM_HAL_SDH_DISABLE);
        wm_ll_sdh_mmc_set_data_line_crc_en(reg, WM_HAL_SDH_DISABLE);
    }

    /* MMC_TIMEOUTCNT */
    if (sdh_cfg->port_mode == WM_HAL_SDH_PORT_MODE_MMC) {
        wm_ll_sdh_set_timeout_count(reg, WM_HAL_SDH_MMC_TIMEOUTCNT);
    } else if (sdh_cfg->port_mode == WM_HAL_SDH_PORT_MODE_SPI) {
        wm_ll_sdh_set_timeout_count(reg, WM_HAL_SDH_SPI_TIMEOUTCNT);
    }

    /* MMC_IO_MBCTL */
    if (sdh_cfg->port_mode == WM_HAL_SDH_PORT_MODE_MMC) {
        wm_ll_sdh_mmc_set_nac_timeout_scale(reg, WM_HAL_SDH_NAC_TIMEOUT_INT_SCALE_1_S);
        wm_ll_sdh_mmc_set_busy_timeout_scale(reg, WM_HAL_SDH_BUSY_TIMEOUT_INT_SCALE_1_S);
    }

    /* MMC_BYTECNTL */
    if (sdh_cfg->port_mode == WM_HAL_SDH_PORT_MODE_SPI) {
        wm_ll_sdh_mmc_set_byte_count(reg, 0);
    }

    WM_HAL_SDH_FUNC_EXIT(WM_HAL_SDH_FUNC_NAME);

    WM_HAL_SDH_LOG_D("MMC_CARDSEL: %08X", reg->mmc_cardsel);
    WM_HAL_SDH_LOG_D("MMC_CTL: %08X", reg->mmc_ctrl);
    WM_HAL_SDH_LOG_D("MMC_INT_MASK: %08X", reg->mmc_int_mask);
    WM_HAL_SDH_LOG_D("MMC_CRCCTL: %08X", reg->mmc_crcctl);
    WM_HAL_SDH_LOG_D("MMC_TIMEOUTCNT: %08X", reg->timeoutcnt);
    WM_HAL_SDH_LOG_D("MMC_IO_MBCTL: %08X", reg->mmc_io_mbctl);
    WM_HAL_SDH_LOG_D("MMC_BYTECNTL: %08X", reg->mmc_bytecntl);

    return ret;
}

int wm_hal_sdh_deinit(wm_hal_sdh_dev_t *dev)
{
    int ret           = WM_ERR_SUCCESS;
    wm_sdh_reg_t *reg = dev->reg_base;

    if (dev == NULL || reg == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    wm_ll_sdh_mmc_set_enable_card_clk(reg, WM_HAL_SDH_DISABLE);
    wm_ll_sdh_mmc_set_controller_enable(reg, WM_HAL_SDH_DISABLE);

    WM_HAL_SDH_MEM_ZERO(dev, sizeof(wm_hal_sdh_dev_t));

    return ret;
}

int wm_hal_sdh_set_bus_width(wm_hal_sdh_dev_t *dev, wm_sdh_bus_width_t width)
{
    int ret                   = WM_ERR_SUCCESS;
    wm_sdh_reg_t *reg         = dev->reg_base;
    wm_hal_sdh_cfg_t *sdh_cfg = &dev->sdh_cfg;

    if (dev == NULL || reg == NULL || width >= WM_SDH_BUS_WIDTH_MAX) {
        return WM_ERR_INVALID_PARAM;
    }

    wm_ll_sdh_mmc_set_sdio_bus_width(reg, width);
    sdh_cfg->bus_width = width;

    return ret;
}

int wm_hal_sdh_set_clock(wm_hal_sdh_dev_t *dev, uint32_t clock)
{
    int ret                   = WM_ERR_SUCCESS;
    wm_sdh_reg_t *reg         = dev->reg_base;
    wm_hal_sdh_cfg_t *sdh_cfg = &dev->sdh_cfg;
    uint32_t div              = 0;
    uint32_t port_clk_sel     = 0;
    uint32_t base_clk         = 0;

    if (dev == NULL || reg == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (clock == 0 || sdh_cfg->xfer_mode > WM_HAL_SDH_XFER_MODE_MAX) {
        return WM_ERR_INVALID_PARAM;
    }

    /*
     * When `mmc_ctrl[6] = 1`, the controller operates in high-speed mode, and the base clock is `hclk`.
     * When `mmc_ctrl[6] = 0`, the controller operates in low-speed mode, and the base clock is `clklm`.
     * The clock `Clk1m` is calculated as `Fhclk / ((mmc_cardsel[5:0] + 1) * 2)`.
     * 1MHz = Fhclk / ((mmc_cardsel [5:0] + 1) * 2)
     * mmc_cardsel [5:0] = Fhclk / 2 -1
     */
    wm_ll_sdh_mmc_set_time_base_coefficient(reg, (dev->fhclk >> 1) - 1);
    if (sdh_cfg->xfer_mode == WM_HAL_SDH_XFER_MODE_SPEED_LOW) {
        base_clk = WM_HAL_SDH_CLK_1M;
    } else {
        base_clk = dev->fhclk * WM_HAL_SDH_CLK_1M;
    }
    wm_ll_sdh_mmc_set_sdio_transfer_mode(reg, sdh_cfg->xfer_mode);
    div = base_clk / clock;
    if (div < 2) {
        div = 2;
    }
    port_clk_sel = (div >> 1) - 1;
    if (port_clk_sel >= WM_HAL_SDH_PORT_CLK_SEL_MAX) {
        port_clk_sel = WM_HAL_SDH_PORT_CLK_SEL_MAX - 1;
    }
    wm_ll_sdh_mmc_set_sdio_port_clock_sel(reg, port_clk_sel);
    WM_HAL_SDH_LOG_D("wm_hal_sdh_set_clock: %d-%d-%d", dev->fhclk, div, port_clk_sel);

    return ret;
}

static void wm_hal_sdh_get_us(wm_hal_sdh_dev_t *dev, long *total_us)
{
    static uint32_t last_tick = 0;
    uint32_t now_tick         = 0;
    uint32_t tick_us          = 0;
    uint32_t cpu_clock        = 0;
    now_tick                  = csi_coret_get_value();
    cpu_clock                 = wm_hal_rcc_get_clock_config(dev->rcc_hal_dev, WM_RCC_TYPE_CPU);

    if (*total_us == 0) {
        last_tick = 0;
    }

    if (now_tick > last_tick) {
        tick_us = (now_tick - last_tick) / cpu_clock;
    }

    last_tick = now_tick;
    *total_us += tick_us;
}

static int wm_hal_sdh_wait_int(wm_hal_sdh_dev_t *dev, uint8_t int_pos, uint32_t timeout)
{
    int ret              = WM_ERR_SUCCESS;
    wm_sdh_reg_t *reg    = dev->reg_base;
    uint32_t mmc_int_sts = 0;
    long tick_us         = 0;

    while (!(mmc_int_sts & WM_BIT(int_pos))) {
        wm_hal_sdh_get_us(dev, &tick_us);
        if (tick_us > timeout * 1000) {
            ret = WM_ERR_TIMEOUT;
            WM_HAL_SDH_LOG_E("CMD TIMEOUT:%d", tick_us);
            break;
        }
        mmc_int_sts = wm_ll_sdh_get_int_sts(reg);
    }

    if (mmc_int_sts & WM_BIT(WM_HAL_SDH_INT_CMD_RSP_NCR_TIMEOUT)) {
        ret = WM_ERR_TIMEOUT;
    }

    wm_ll_sdh_set_int_sts(reg, WM_BIT(int_pos));

    return ret;
}

static int wm_hal_sdh_wait_data_done(wm_hal_sdh_dev_t *dev, uint32_t timeout)
{
    int ret           = WM_ERR_SUCCESS;
    wm_sdh_reg_t *reg = dev->reg_base;
    long tick_us      = 0;

    while (wm_ll_sdh_mmc_get_auto_data_trans(reg)) {
        wm_hal_sdh_get_us(dev, &tick_us);
        if (tick_us > timeout * 1000) {
            WM_HAL_SDH_LOG_E("Tran done timeout:%d", tick_us);
            ret = WM_ERR_TIMEOUT;
            break;
        }
    }

    return ret;
}

static int wm_hal_sdh_wait_multi_blocks_done(wm_hal_sdh_dev_t *dev, uint32_t timeout)
{
    int ret           = WM_ERR_SUCCESS;
    wm_sdh_reg_t *reg = dev->reg_base;
    long tick_us      = 0;

    while (wm_ll_sdh_mmc_get_auto_multi_block_tran(reg)) {
        wm_hal_sdh_get_us(dev, &tick_us);
        if (tick_us > timeout * 1000) {
            WM_HAL_SDH_LOG_E("Tran done timeout:%d", tick_us);
            ret = WM_ERR_TIMEOUT;
            break;
        }
    }

    return ret;
}

static int wm_hal_sdh_send_cmd_read_rsp(wm_hal_sdh_dev_t *dev, wm_hal_sdh_cmd_t *cmd)
{
    int ret                   = WM_ERR_SUCCESS;
    wm_sdh_reg_t *reg         = dev->reg_base;
    wm_hal_sdh_cfg_t *sdh_cfg = &dev->sdh_cfg;
    uint32_t mmc_io           = cmd->cmd_io;
    uint32_t cmd_arg          = cmd->cmd_arg;
    uint32_t cmd_timeout      = cmd->cmd_timeout;
    uint32_t *rsp_buf         = &cmd->cmd_rsp[0];
    uint8_t cmd_id            = cmd->cmd_id | WM_HAL_SDH_CMD_NUM_BASE;
    uint8_t cmd_rsp           = cmd->cmd_rsp_type;

    if (cmd_rsp >= WM_HAL_SDH_CMD_RSP_MAX) {
        return WM_ERR_INVALID_PARAM;
    }
    wm_ll_sdh_mmc_set_sdio_transfer_mode(reg, WM_HAL_SDH_XFER_MODE_SPEED_LOW);
    /* send cmd to bus */
    wm_ll_sdh_set_cmd_buf(reg, WM_HAL_SDH_CMD_BUF_ID_4, cmd_id);
    wm_ll_sdh_set_cmd_buf(reg, WM_HAL_SDH_CMD_BUF_ID_3, (cmd_arg >> 24) & 0xFF);
    wm_ll_sdh_set_cmd_buf(reg, WM_HAL_SDH_CMD_BUF_ID_2, (cmd_arg >> 16) & 0xFF);
    wm_ll_sdh_set_cmd_buf(reg, WM_HAL_SDH_CMD_BUF_ID_1, (cmd_arg >> 8) & 0xFF);
    wm_ll_sdh_set_cmd_buf(reg, WM_HAL_SDH_CMD_BUF_ID_0, (cmd_arg >> 0) & 0xFF);
    wm_ll_sdh_mmc_set_cmd_complete_int_mask(reg, WM_HAL_SDH_ENABLE);
    wm_ll_sdh_set_int_sts(reg, WM_HAL_SDH_CLR_MMC_INT_ALL);

    if (mmc_io) {
        wm_ll_sdh_mmc_set_mmc_io(reg, mmc_io);
    } else {
        wm_ll_sdh_mmc_set_cmd_abort_flag(reg, WM_HAL_SDH_CMD12_IO_NOT_ABORT);
        wm_ll_sdh_mmc_set_cmd_attr(reg, WM_HAL_SDH_CMD_ATTRIBUTE_NO_DATA);

        if (cmd_id == (WM_HAL_SDH_CMD_NUM_BASE | WM_HAL_SDH_CMD_NUM_9) ||
            cmd_id == (WM_HAL_SDH_CMD_NUM_BASE | WM_HAL_SDH_CMD_NUM_17) ||
            cmd_id == (WM_HAL_SDH_CMD_NUM_BASE | WM_HAL_SDH_CMD_NUM_18) ||
            cmd_id == (WM_HAL_SDH_CMD_NUM_BASE | WM_HAL_SDH_CMD_NUM_24) ||
            cmd_id == (WM_HAL_SDH_CMD_NUM_BASE | WM_HAL_SDH_CMD_NUM_25)) {
            wm_ll_sdh_mmc_set_auto_null_clk_gen_resp_cmd_trans(reg, WM_HAL_SDH_DISABLE);
        } else {
            wm_ll_sdh_mmc_set_auto_null_clk_gen_resp_cmd_trans(reg, WM_HAL_SDH_ENABLE);
        }
        if (cmd_id == (WM_HAL_SDH_CMD_NUM_BASE | WM_HAL_SDH_CMD_NUM_0)) {
            wm_ll_sdh_mmc_set_auto_resp_recv(reg, WM_HAL_SDH_DISABLE);
        } else {
            wm_ll_sdh_mmc_set_auto_resp_recv(reg, WM_HAL_SDH_ENABLE);
        }

        wm_ll_sdh_mmc_set_port_clk_gen(reg, WM_HAL_SDH_AUTO_GEN_8_CLK_SEL_RESPONSE_OR_CMD);

        if (cmd_id == (WM_HAL_SDH_CMD_NUM_BASE | WM_HAL_SDH_CMD_NUM_2) ||
            cmd_id == (WM_HAL_SDH_CMD_NUM_BASE | WM_HAL_SDH_CMD_NUM_9) ||
            cmd_id == (WM_HAL_SDH_CMD_NUM_BASE | WM_HAL_SDH_CMD_NUM_10)) {
            wm_ll_sdh_mmc_set_cid_csd_read(reg, WM_HAL_SDH_ENABLE);
        } else {
            wm_ll_sdh_mmc_set_cid_csd_read(reg, WM_HAL_SDH_DISABLE);
        }

        wm_ll_sdh_mmc_set_cmd_sel(reg, WM_HAL_SDH_AUTO_GEN_8_CLK_SEL_TYPE_SEND_CMD);
        wm_ll_sdh_mmc_set_data_dir(reg, WM_HAL_SDH_DATA_XFER_DIR_WRITE);
        wm_ll_sdh_mmc_set_auto_data_trans(reg, WM_HAL_SDH_DISABLE);
        wm_ll_sdh_mmc_set_auto_8_null_clk_gen(reg, WM_HAL_SDH_ENABLE);
    }
    WM_HAL_SDH_LOG_D("CMD:%d, MMC_IO:0x%02X", cmd_id - WM_HAL_SDH_CMD_NUM_BASE, wm_ll_sdh_mmc_get_mmc_io(reg));

    /* wait cmd done */
    ret = wm_hal_sdh_wait_int(dev, WM_HAL_SDH_INT_CMD_DONE, cmd_timeout);
    if (ret != WM_ERR_SUCCESS) {
        return ret;
    }

    /* check and recive cmd rsp if need */
    if (cmd_rsp == WM_HAL_SDH_CMD_WAIT_RSP) {
        for (uint8_t i = 0; i < cmd->cmd_rsp_len; i++) {
            rsp_buf[i] = (wm_ll_sdh_get_cmd_buf(reg, (i << 2) + 0) << 0) | (wm_ll_sdh_get_cmd_buf(reg, (i << 2) + 1) << 8) |
                         (wm_ll_sdh_get_cmd_buf(reg, (i << 2) + 2) << 16) | (wm_ll_sdh_get_cmd_buf(reg, (i << 2) + 3) << 24);
        }
    }
    wm_ll_sdh_mmc_set_sdio_transfer_mode(reg, sdh_cfg->xfer_mode);

    return ret;
}

static int wm_hal_sdh_tx_data_polling(wm_hal_sdh_dev_t *dev, wm_hal_sdh_data_desc_t *tx)
{
    int ret             = WM_ERR_SUCCESS;
    wm_sdh_reg_t *reg   = dev->reg_base;
    uint32_t send_len   = 0;
    uint32_t total_sent = 0;
    uint8_t *p_buf      = tx->buf;

    if (tx == NULL || tx->buf == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    /* use CPU */
    /* buf control */
    wm_ll_sdh_set_data_flush_en(reg, WM_HAL_SDH_ENABLE);
    wm_ll_sdh_set_dma_req_mask(reg, WM_HAL_SDH_ENABLE);
    wm_ll_sdh_set_buf_access_dir(reg, WM_HAL_SDH_BUF_ACCESS_DIR_WRITE);
    wm_ll_sdh_set_data_pipeline_set(reg, WM_HAL_SDH_BUF_CTRL_PIPELINE_8);
    wm_ll_sdh_set_dma_hw_intf_en(reg, WM_HAL_SDH_DISABLE);
    wm_ll_sdh_set_fifo_status_mask(reg, WM_HAL_SDH_DISABLE);

    while (total_sent < tx->len) {
        send_len = ((tx->len - total_sent) > WM_SDH_DATA_BUF_BYTES) ? WM_SDH_DATA_BUF_BYTES : (tx->len - total_sent);

        /* write to sd buf */
        wm_ll_sdh_set_data_buf(reg, p_buf, send_len);

        /* set transfer len */
        wm_ll_sdh_mmc_set_byte_count(reg, send_len);

        wm_ll_sdh_set_int_sts(reg, WM_HAL_SDH_CLR_MMC_INT_ALL);

        /* start tx transfer */
        wm_ll_sdh_mmc_set_data_dir(reg, WM_HAL_SDH_DATA_XFER_DIR_WRITE);
        wm_ll_sdh_mmc_set_auto_data_trans(reg, WM_HAL_SDH_ENABLE);

        ret = wm_hal_sdh_wait_data_done(dev, tx->timeout);
        if (ret != WM_ERR_SUCCESS) {
            return ret;
        }
        total_sent += send_len;
        p_buf += send_len;
    }

    return ret;
}

static int wm_hal_sdh_rx_data_polling(wm_hal_sdh_dev_t *dev, wm_hal_sdh_data_desc_t *rx)
{
    int ret                 = WM_ERR_SUCCESS;
    wm_sdh_reg_t *reg       = dev->reg_base;
    uint32_t receive_len    = 0;
    uint32_t total_received = 0;
    uint8_t *p_buf          = rx->buf;

    if (rx == NULL || rx->buf == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    /* use CPU */
    /* buf control */
    wm_ll_sdh_set_data_flush_en(reg, WM_HAL_SDH_ENABLE);
    wm_ll_sdh_set_dma_req_mask(reg, WM_HAL_SDH_ENABLE);
    wm_ll_sdh_set_buf_access_dir(reg, WM_HAL_SDH_BUF_ACCESS_DIR_READ);
    wm_ll_sdh_set_data_pipeline_set(reg, WM_HAL_SDH_BUF_CTRL_PIPELINE_1);
    wm_ll_sdh_set_dma_hw_intf_en(reg, WM_HAL_SDH_DISABLE);
    wm_ll_sdh_set_fifo_status_mask(reg, WM_HAL_SDH_DISABLE);

    while (total_received < rx->len) {
        receive_len = ((rx->len - total_received) > WM_SDH_DATA_BUF_BYTES) ? WM_SDH_DATA_BUF_BYTES : (rx->len - total_received);

        wm_ll_sdh_set_int_sts(reg, WM_HAL_SDH_CLR_MMC_INT_ALL);

        /* start rx transfer */
        wm_ll_sdh_mmc_set_data_dir(reg, WM_HAL_SDH_DATA_XFER_DIR_READ);
        wm_ll_sdh_mmc_set_auto_data_trans(reg, WM_HAL_SDH_ENABLE);

        ret = wm_hal_sdh_wait_data_done(dev, rx->timeout);
        if (ret != WM_ERR_SUCCESS) {
            return ret;
        }
        /* get data buf */
        wm_ll_sdh_get_data_buf(reg, rx->buf, rx->len);
        total_received += receive_len;
        p_buf += receive_len;
    }

    return ret;
}

int wm_hal_sdh_xfer_polling(wm_hal_sdh_dev_t *dev, wm_hal_sdh_xfer_desc_t *xfer)
{
    int ret                                     = WM_ERR_SUCCESS;
    wm_hal_sdh_port_mode_t port_mode            = WM_HAL_SDH_XFER_MODE_MAX;
    wm_hal_sdh_spi_xfer_desc_t *spi_xfer_desc   = NULL;
    wm_hal_sdh_sdio_xfer_desc_t *sdio_xfer_desc = NULL;

    if (dev == NULL || xfer == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    port_mode = xfer->port_mode;
    if (port_mode > WM_HAL_SDH_PORT_MODE_MAX) {
        return WM_ERR_INVALID_PARAM;
    }

    if (port_mode == WM_HAL_SDH_PORT_MODE_MMC) {
        sdio_xfer_desc = &xfer->sdio_xfer_desc;
        if (sdio_xfer_desc == NULL) {
            return WM_ERR_INVALID_PARAM;
        }

        /* transfer cmd */
        ret = wm_hal_sdh_send_cmd_read_rsp(dev, &sdio_xfer_desc->sd_cmd);
        if (ret != WM_ERR_SUCCESS) {
            WM_HAL_SDH_LOG_E("send cmd fail with err code:%d", ret);
        }

        /* transfer data */
        if (sdio_xfer_desc->mode == WM_HAL_SDH_SDMMC_TX) {
            ret = wm_hal_sdh_tx_data_polling(dev, &sdio_xfer_desc->sd_data.tx);
            if (ret != WM_ERR_SUCCESS) {
                WM_HAL_SDH_LOG_E("tx data fail with err code:%d", ret);
            }
        }
        if (sdio_xfer_desc->mode == WM_HAL_SDH_SDMMC_RX) {
            ret = wm_hal_sdh_rx_data_polling(dev, &sdio_xfer_desc->sd_data.rx);
            if (ret != WM_ERR_SUCCESS) {
                WM_HAL_SDH_LOG_E("rx data fail with err code:%d", ret);
            }
        } else {
            WM_HAL_SDH_LOG_I("sd cmd only");
        }
    } else if (port_mode == WM_HAL_SDH_PORT_MODE_SPI) {
        /* sdh spi tranfer mode */
        spi_xfer_desc = &xfer->spi_xfer_desc;
        if (spi_xfer_desc == NULL) {
            return WM_ERR_INVALID_PARAM;
        }

        /* transfer data */
        if (spi_xfer_desc->mode == WM_HAL_SDH_SPI_TX) {
            ret = wm_hal_sdh_tx_data_polling(dev, &spi_xfer_desc->spi_data.tx);
            if (ret != WM_ERR_SUCCESS) {
                WM_HAL_SDH_LOG_E("tx data fail with err code:%d", ret);
            }
        } else if (spi_xfer_desc->mode == WM_HAL_SDH_SPI_RX) {
            ret = wm_hal_sdh_rx_data_polling(dev, &spi_xfer_desc->spi_data.rx);
            if (ret != WM_ERR_SUCCESS) {
                WM_HAL_SDH_LOG_E("rx data fail with err code:%d", ret);
            }
        } else {
            WM_HAL_SDH_LOG_I("sd cmd only");
        }
    }

    return ret;
}

int wm_hal_sdh_tx_register_callback(wm_hal_sdh_dev_t *dev, wm_hal_sdh_callback_t xfer_cb, void *user_priv)
{
    int ret = WM_ERR_SUCCESS;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    dev->ctx.tx_cb.cb      = xfer_cb;
    dev->ctx.tx_cb.cb_priv = user_priv;
    WM_HAL_SDH_LOG_I("sdh register callback for tx cb:0x%x", (uint32_t)xfer_cb);

    return ret;
}

int wm_hal_sdh_tx_unregister_callback(wm_hal_sdh_dev_t *dev)
{
    int ret = WM_ERR_SUCCESS;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    dev->ctx.tx_cb.cb      = NULL;
    dev->ctx.tx_cb.cb_priv = NULL;

    return ret;
}

int wm_hal_sdh_rx_register_callback(wm_hal_sdh_dev_t *dev, wm_hal_sdh_callback_t xfer_cb, void *user_priv)
{
    int ret = WM_ERR_SUCCESS;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    dev->ctx.rx_cb.cb      = xfer_cb;
    dev->ctx.rx_cb.cb_priv = user_priv;
    WM_HAL_SDH_LOG_I("sdh register callback for rx cb:0x%x", (uint32_t)xfer_cb);

    return ret;
}

int wm_hal_sdh_rx_unregister_callback(wm_hal_sdh_dev_t *dev)
{
    int ret = WM_ERR_SUCCESS;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    dev->ctx.rx_cb.cb      = NULL;
    dev->ctx.rx_cb.cb_priv = NULL;

    return ret;
}

static void wm_hal_sdh_dma_done_callback(wm_dma_ch_t ch, uint32_t sts, void *user_data)
{
    wm_hal_sdh_dev_t *dev            = (wm_hal_sdh_dev_t *)user_data;
    wm_sdh_reg_t *reg                = dev->reg_base;
    uint32_t buf_access_dir          = 0;
    wm_hal_sdh_evt_t evt             = { 0 };
    wm_hal_sdh_port_mode_t port_mode = 0;

    wm_hal_dma_stop(dev->dma_hal_dev, ch);
    port_mode = wm_ll_sdh_mmc_get_port_mode(reg);
    if (port_mode == WM_HAL_SDH_PORT_MODE_SPI) {
        wm_hal_sdh_wait_data_done(dev, WM_HAL_SDH_WAIT_SINGLE_BLOCK_TRAN_DONE_TIMEOUT);
    } else if (port_mode == WM_HAL_SDH_PORT_MODE_MMC) {
        wm_hal_sdh_wait_multi_blocks_done(dev, WM_HAL_SDH_WAIT_MULTI_BLOCKS_TRAN_DONE_TIMEOUT);
    }
    buf_access_dir = wm_ll_sdh_get_buf_access_dir(reg);
    if (buf_access_dir == WM_HAL_SDH_BUF_ACCESS_DIR_WRITE) {
        if (dev->ctx.tx_cb.cb != NULL) {
            evt.type       = WM_HAL_SDH_EVT_WRITE_DONE;
            evt.is_dma_evt = true;
            dev->ctx.tx_cb.cb(dev, &evt, dev->ctx.tx_cb.cb_priv);
        }
    } else if (buf_access_dir == WM_HAL_SDH_BUF_ACCESS_DIR_READ) {
        if (dev->ctx.rx_cb.cb != NULL) {
            evt.type       = WM_HAL_SDH_EVT_READ_DONE;
            evt.is_dma_evt = true;
            dev->ctx.rx_cb.cb(dev, &evt, dev->ctx.rx_cb.cb_priv);
        }
    }
}

static int wm_hal_sdh_config_dma(wm_hal_sdh_dev_t *dev, wm_hal_sdh_buf_access_dir_t dir, wm_hal_sdh_data_desc_t *data)
{
    int ret                   = WM_ERR_SUCCESS;
    wm_sdh_reg_t *reg         = dev->reg_base;
    wm_hal_dma_desc_t dma_cfg = { 0 };

    if (dev->dma_hal_dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    ret = (int)wm_hal_dma_register_callback(dev->dma_hal_dev, dev->dma_ch, wm_hal_sdh_dma_done_callback, dev);
    if (ret != WM_ERR_SUCCESS) {
        return ret;
    }

    if (dir == WM_HAL_SDH_BUF_ACCESS_DIR_WRITE) {
        dma_cfg.src_addr                    = (uint32_t)data->buf;
        dma_cfg.dest_addr                   = (uint32_t)wm_ll_sdh_get_data_buf_addr(reg);
        dma_cfg.ctrl.nonchain.src_addr_inc  = WM_DMA_ADDR_INC;
        dma_cfg.ctrl.nonchain.dest_addr_inc = WM_DMA_ADDR_FIXED;
    } else if (dir == WM_HAL_SDH_BUF_ACCESS_DIR_READ) {
        dma_cfg.src_addr                    = (uint32_t)wm_ll_sdh_get_data_buf_addr(reg);
        dma_cfg.dest_addr                   = (uint32_t)data->buf;
        dma_cfg.ctrl.nonchain.src_addr_inc  = WM_DMA_ADDR_FIXED;
        dma_cfg.ctrl.nonchain.dest_addr_inc = WM_DMA_ADDR_INC;
    }
    dma_cfg.ctrl.nonchain.len         = data->len;
    dma_cfg.ctrl.nonchain.auto_reload = WM_DMA_RELOAD_DISABLE;
    dma_cfg.extend_ctrl.ch            = dev->dma_ch;
    dma_cfg.extend_ctrl.int_en        = WM_DMA_CH_EN_CTRL_ENABLE;
    dma_cfg.extend_ctrl.int_type      = WM_DMA_INT_XFER_DONE;
    dma_cfg.extend_ctrl.mode          = WM_DMA_HW_MODE;
    dma_cfg.extend_ctrl.chain_mode    = WM_DMA_CHAIN_MODE_NORMAL;
    wm_hal_dma_clr_req(dev->dma_hal_dev, dma_cfg.extend_ctrl.req_sel);
    dma_cfg.extend_ctrl.req_sel      = WM_DMA_SDIOH_REQ;
    dma_cfg.extend_ctrl.warp_mode_en = WM_DMA_WARP_CTRL_DISABLE;
    dma_cfg.ctrl.nonchain.data_unit  = WM_DMA_XFER_UNIT_WORD;
    dma_cfg.ctrl.nonchain.burst_size = WM_DMA_BURST_1UNIT;

    ret = (int)wm_hal_dma_config(dev->dma_hal_dev, &dma_cfg);
    if (ret != WM_ERR_SUCCESS) {
        return ret;
    }

    return ret;
}

static int wm_hal_sdh_tx_data_dma(wm_hal_sdh_dev_t *dev, wm_hal_sdh_data_desc_t *tx)
{
    int ret                   = WM_ERR_SUCCESS;
    wm_sdh_reg_t *reg         = dev->reg_base;
    wm_hal_sdh_cfg_t *sdh_cfg = &dev->sdh_cfg;
    uint32_t block_count      = 0;

    if (tx == NULL || tx->buf == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    /* use DMA */
    /* disable dma */
    wm_ll_sdh_set_dma_req_mask(reg, WM_HAL_SDH_ENABLE);
    /* config dma */
    ret = wm_hal_sdh_config_dma(dev, WM_HAL_SDH_BUF_ACCESS_DIR_WRITE, tx);
    if (ret != WM_ERR_SUCCESS) {
        return ret;
    }
    /*
     * SDMMC:
     * reg->buf_ctrl.val = 0xC20;
     * reg->blockcnt.val = tx->len / WM_SDH_BLOCK_SIZE;
     * reg->mmc_int_mask.val |= 0x7ff;
     * reg->mmc_io_mbctl.val = 0xa1;
     *
     * SPI:
     * reg->buf_ctrl.val = 0xC20;
     * reg->mmc_int_mask.val |= 0x7ff;
     * reg->mmc_bytecntl = tx->len;
     * reg->mmc_io.val = 0x01;
     */

    /* buf_ctrl */
    wm_ll_sdh_set_data_flush_en(reg, WM_HAL_SDH_ENABLE);
    wm_ll_sdh_set_dma_req_mask(reg, WM_HAL_SDH_DISABLE);
    wm_ll_sdh_set_buf_access_dir(reg, WM_HAL_SDH_BUF_ACCESS_DIR_WRITE);
    wm_ll_sdh_set_data_pipeline_set(reg, WM_HAL_SDH_BUF_CTRL_PIPELINE_8);
    wm_ll_sdh_set_dma_hw_intf_en(reg, WM_HAL_SDH_ENABLE);

    /* blockcnt */
    if (sdh_cfg->port_mode == WM_HAL_SDH_PORT_MODE_MMC) {
        block_count = tx->len / WM_SDH_BLOCK_SIZE;
        wm_ll_sdh_set_block_count(reg, block_count);
    }

    /* mmc_int_mask */
    wm_ll_sdh_set_int_sts(reg, WM_HAL_SDH_CLR_MMC_INT_ALL);

    /* mmc_io_mbctl */
    if (sdh_cfg->port_mode == WM_HAL_SDH_PORT_MODE_MMC) {
        wm_ll_sdh_mmc_set_busy_timeout_scale(reg, WM_HAL_SDH_BUSY_TIMEOUT_INT_SCALE_10_MS);
        wm_ll_sdh_mmc_set_nac_timeout_scale(reg, WM_HAL_SDH_NAC_TIMEOUT_INT_SCALE_10_MS);
        wm_ll_sdh_mmc_set_multi_block_direction(reg, WM_HAL_SDH_DATA_BLOCK_XFER_DIR_WRITE);
        wm_ll_sdh_mmc_set_auto_multi_block_tran(reg, WM_HAL_SDH_ENABLE);
    }

    /* mmc_bytecntl */
    if (sdh_cfg->port_mode == WM_HAL_SDH_PORT_MODE_SPI) {
        wm_ll_sdh_mmc_set_byte_count(reg, tx->len);
    }

    /* mmc_io */
    if (sdh_cfg->port_mode == WM_HAL_SDH_PORT_MODE_SPI) {
        wm_ll_sdh_mmc_set_data_dir(reg, WM_HAL_SDH_DATA_XFER_DIR_WRITE);
        wm_ll_sdh_mmc_set_auto_data_trans(reg, WM_HAL_SDH_ENABLE);
    }

    ret = (int)wm_hal_dma_start(dev->dma_hal_dev, dev->dma_ch);
    if (ret != WM_ERR_SUCCESS) {
        return ret;
    }

    WM_HAL_SDH_LOG_D("BUF_CTL: %08X", reg->buf_ctrl);
    WM_HAL_SDH_LOG_D("MMC_BLOCKCNT: %08X", reg->blockcnt);
    WM_HAL_SDH_LOG_D("MMC_IO_MBCTL: %08X", reg->mmc_io_mbctl);
    WM_HAL_SDH_LOG_D("MMC_BYTECNTL: %08X", reg->mmc_bytecntl);
    WM_HAL_SDH_LOG_D("MMC_IO: %08X", reg->mmc_io);

    return ret;
}

static int wm_hal_sdh_rx_data_dma(wm_hal_sdh_dev_t *dev, wm_hal_sdh_data_desc_t *rx)
{
    int ret                   = WM_ERR_SUCCESS;
    wm_sdh_reg_t *reg         = dev->reg_base;
    wm_hal_sdh_cfg_t *sdh_cfg = &dev->sdh_cfg;
    uint32_t block_count      = 0;

    if (rx == NULL || rx->buf == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    /* use DMA */
    /* disable dma */
    wm_ll_sdh_set_dma_req_mask(reg, WM_HAL_SDH_ENABLE);
    /* config dma */
    ret = wm_hal_sdh_config_dma(dev, WM_HAL_SDH_BUF_ACCESS_DIR_READ, rx);
    if (ret != WM_ERR_SUCCESS) {
        return ret;
    }

    /*
     * SDMMC:
     * reg->buf_ctrl.val = 0x404;
     * reg->blockcnt.val = rx->len / WM_SDH_BLOCK_SIZE;
     * reg->mmc_int_mask.val |= 0x7ff;
     * reg->mmc_io_mbctl.val = 0xa3;
     *
     * SPI:
     * reg->buf_ctrl.val = 0x404;
     * reg->mmc_int_mask.val |= 0x7ff;
     * reg->mmc_bytecntl = rx->len;
     * reg->mmc_io.val = 0x03;
     */

    /* buf_ctrl */
    wm_ll_sdh_set_data_flush_en(reg, WM_HAL_SDH_ENABLE);
    wm_ll_sdh_set_dma_req_mask(reg, WM_HAL_SDH_DISABLE);
    wm_ll_sdh_set_buf_access_dir(reg, WM_HAL_SDH_BUF_ACCESS_DIR_READ);
    wm_ll_sdh_set_data_pipeline_set(reg, WM_HAL_SDH_BUF_CTRL_PIPELINE_1);
    wm_ll_sdh_set_dma_hw_intf_en(reg, WM_HAL_SDH_ENABLE);

    /* blockcnt */
    if (sdh_cfg->port_mode == WM_HAL_SDH_PORT_MODE_MMC) {
        block_count = rx->len / WM_SDH_BLOCK_SIZE;
        wm_ll_sdh_set_block_count(reg, block_count);
    }

    /* mmc_int_mask */
    wm_ll_sdh_set_int_sts(reg, WM_HAL_SDH_CLR_MMC_INT_ALL);

    /* mmc_io_mbctl */
    if (sdh_cfg->port_mode == WM_HAL_SDH_PORT_MODE_MMC) {
        wm_ll_sdh_mmc_set_busy_timeout_scale(reg, WM_HAL_SDH_BUSY_TIMEOUT_INT_SCALE_10_MS);
        wm_ll_sdh_mmc_set_nac_timeout_scale(reg, WM_HAL_SDH_NAC_TIMEOUT_INT_SCALE_10_MS);
        wm_ll_sdh_mmc_set_multi_block_direction(reg, WM_HAL_SDH_DATA_BLOCK_XFER_DIR_READ);
        wm_ll_sdh_mmc_set_auto_multi_block_tran(reg, WM_HAL_SDH_ENABLE);
    }

    /* mmc_bytecntl */
    if (sdh_cfg->port_mode == WM_HAL_SDH_PORT_MODE_SPI) {
        wm_ll_sdh_mmc_set_byte_count(reg, rx->len);
    }

    /* mmc_io */
    if (sdh_cfg->port_mode == WM_HAL_SDH_PORT_MODE_SPI) {
        wm_ll_sdh_mmc_set_data_dir(reg, WM_HAL_SDH_DATA_XFER_DIR_READ);
        wm_ll_sdh_mmc_set_auto_data_trans(reg, WM_HAL_SDH_ENABLE);
    }

    ret = (int)wm_hal_dma_start(dev->dma_hal_dev, dev->dma_ch);
    if (ret != WM_ERR_SUCCESS) {
        return ret;
    }

    WM_HAL_SDH_LOG_D("BUF_CTL: %08X", reg->buf_ctrl);
    WM_HAL_SDH_LOG_D("MMC_BLOCKCNT: %08X", reg->blockcnt);
    WM_HAL_SDH_LOG_D("MMC_IO_MBCTL: %08X", reg->mmc_io_mbctl);
    WM_HAL_SDH_LOG_D("MMC_BYTECNTL: %08X", reg->mmc_bytecntl);
    WM_HAL_SDH_LOG_D("MMC_IO: %08X", reg->mmc_io);

    return ret;
}

int wm_hal_sdh_xfer_dma(wm_hal_sdh_dev_t *dev, wm_hal_sdh_xfer_desc_t *xfer)
{
    int ret                                     = WM_ERR_SUCCESS;
    wm_hal_sdh_sdio_xfer_desc_t *sdio_xfer_desc = NULL;
    wm_hal_sdh_spi_xfer_desc_t *spi_xfer_desc   = NULL;
    wm_hal_sdh_port_mode_t port_mode            = WM_HAL_SDH_PORT_MODE_MAX;

    if (dev == NULL || xfer == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    port_mode = xfer->port_mode;
    if (port_mode >= WM_HAL_SDH_PORT_MODE_MAX) {
        return WM_ERR_INVALID_PARAM;
    }

    if (port_mode == WM_HAL_SDH_PORT_MODE_MMC) {
        sdio_xfer_desc = &xfer->sdio_xfer_desc;
        if (sdio_xfer_desc == NULL) {
            return WM_ERR_INVALID_PARAM;
        }

        /* transfer cmd */
        ret = wm_hal_sdh_send_cmd_read_rsp(dev, &sdio_xfer_desc->sd_cmd);
        if (ret != WM_ERR_SUCCESS) {
            WM_HAL_SDH_LOG_E("send cmd fail with err code:%d", ret);
            return ret;
        }

        /* transfer data */
        if (sdio_xfer_desc->mode == WM_HAL_SDH_SDMMC_TX) {
            ret = wm_hal_sdh_tx_data_dma(dev, &sdio_xfer_desc->sd_data.tx);
            if (ret != WM_ERR_SUCCESS) {
                WM_HAL_SDH_LOG_E("tx data fail with err code:%d", ret);
                return ret;
            }
        } else if (sdio_xfer_desc->mode == WM_HAL_SDH_SDMMC_RX) {
            ret = wm_hal_sdh_rx_data_dma(dev, &sdio_xfer_desc->sd_data.rx);
            if (ret != WM_ERR_SUCCESS) {
                WM_HAL_SDH_LOG_E("rx data fail with err code:%d", ret);
                return ret;
            }
        } else {
            WM_HAL_SDH_LOG_I("sd cmd only");
        }
    } else if (port_mode == WM_HAL_SDH_PORT_MODE_SPI) {
        /* sdh spi tranfer mode */
        spi_xfer_desc = &xfer->spi_xfer_desc;
        if (spi_xfer_desc == NULL) {
            return WM_ERR_INVALID_PARAM;
        }

        /* transfer data */
        if (spi_xfer_desc->mode == WM_HAL_SDH_SPI_TX) {
            spi_xfer_desc->spi_data.tx.len = (spi_xfer_desc->spi_data.tx.len >> 2) << 2; //must 4 byte algin
            if (spi_xfer_desc->spi_data.tx.len == 0) {
                return WM_ERR_INVALID_PARAM;
            }
            ret = wm_hal_sdh_tx_data_dma(dev, &spi_xfer_desc->spi_data.tx);
            if (ret != WM_ERR_SUCCESS) {
                WM_HAL_SDH_LOG_E("tx data fail with err code:%d", ret);
            }
        } else if (spi_xfer_desc->mode == WM_HAL_SDH_SPI_RX) {
            spi_xfer_desc->spi_data.rx.len = (spi_xfer_desc->spi_data.rx.len >> 2) << 2; //must 4 byte algin
            if (spi_xfer_desc->spi_data.rx.len == 0) {
                return WM_ERR_INVALID_PARAM;
            }
            ret = wm_hal_sdh_rx_data_dma(dev, &spi_xfer_desc->spi_data.rx);
            if (ret != WM_ERR_SUCCESS) {
                WM_HAL_SDH_LOG_E("rx data fail with err code:%d", ret);
            }
        } else {
            WM_HAL_SDH_LOG_I("sd cmd only");
        }
    } else {
        /* reserved for feture use */
    }

    return ret;
}

static void wm_hal_sdh_irq_handler(wm_irq_no_t irq, void *arg)
{
    wm_hal_sdh_dev_t *dev   = (wm_hal_sdh_dev_t *)arg;
    wm_sdh_reg_t *reg       = dev->reg_base;
    uint32_t int_sts        = 0;
    uint32_t buf_access_dir = 0;
    wm_hal_sdh_evt_t evt    = { 0 };

    /* get interrupt and clear interrupt */
    int_sts = wm_ll_sdh_get_int_sts(reg);
    wm_ll_sdh_set_int_sts(reg, int_sts);

    WM_HAL_SDH_LOG_I("wm_hal_sdh_irq_handler: %d", int_sts);

    buf_access_dir = wm_ll_sdh_get_buf_access_dir(reg);
    if (int_sts & WM_BIT(WM_HAL_SDH_INT_DATA_DONE)) {
        if (buf_access_dir == WM_HAL_SDH_BUF_ACCESS_DIR_READ) {
            wm_ll_sdh_get_data_buf(reg, dev->ctx.sd_rx_data->buf, dev->ctx.sd_rx_data->len);
            if (dev->ctx.rx_cb.cb != NULL) {
                evt.type       = WM_HAL_SDH_EVT_READ_DONE;
                evt.is_dma_evt = false;
                dev->ctx.rx_cb.cb(dev, &evt, dev->ctx.rx_cb.cb_priv);
            }
        } else if (buf_access_dir == WM_HAL_SDH_BUF_ACCESS_DIR_WRITE) {
            if (dev->ctx.tx_cb.cb != NULL) {
                evt.type       = WM_HAL_SDH_EVT_WRITE_DONE;
                evt.is_dma_evt = false;
                dev->ctx.tx_cb.cb(dev, &evt, dev->ctx.tx_cb.cb_priv);
            }
        }
    }

    if ((int_sts & WM_BIT(WM_HAL_SDH_INT_DATA_CRC_ERR)) || (int_sts & WM_BIT(WM_HAL_SDH_INT_CRC_TOKEN_ERR))) {
        if (buf_access_dir == WM_HAL_SDH_BUF_ACCESS_DIR_READ) {
            if (dev->ctx.rx_cb.cb != NULL) {
                evt.type       = WM_HAL_SDH_EVT_READ_ERR;
                evt.is_dma_evt = false;
                dev->ctx.rx_cb.cb(dev, &evt, dev->ctx.rx_cb.cb_priv);
            }
        } else if (buf_access_dir == WM_HAL_SDH_BUF_ACCESS_DIR_WRITE) {
            if (dev->ctx.tx_cb.cb != NULL) {
                evt.type       = WM_HAL_SDH_EVT_WRITE_ERR;
                evt.is_dma_evt = false;
                dev->ctx.tx_cb.cb(dev, &evt, dev->ctx.tx_cb.cb_priv);
            }
        }
    }
}

static int wm_hal_sdh_tx_data_it(wm_hal_sdh_dev_t *dev, wm_hal_sdh_data_desc_t *tx)
{
    int ret           = WM_ERR_SUCCESS;
    wm_sdh_reg_t *reg = dev->reg_base;

    if (tx == NULL || tx->buf == NULL || tx->len > WM_SDH_DATA_BUF_BYTES) {
        return WM_ERR_INVALID_PARAM;
    }

    /* use CPU + it */
    /* register cb to sdh dev with irq hal */
    ret = wm_hal_irq_attach_sw_vector(WM_IRQ_SIDO_HOST, wm_hal_sdh_irq_handler, dev);
    if (ret != WM_ERR_SUCCESS) {
        return ret;
    }

    /* buf control */
    wm_ll_sdh_set_data_flush_en(reg, WM_HAL_SDH_ENABLE);
    wm_ll_sdh_set_dma_req_mask(reg, WM_HAL_SDH_ENABLE);
    wm_ll_sdh_set_buf_access_dir(reg, WM_HAL_SDH_BUF_ACCESS_DIR_WRITE);
    wm_ll_sdh_set_data_pipeline_set(reg, WM_HAL_SDH_BUF_CTRL_PIPELINE_8);
    wm_ll_sdh_set_dma_hw_intf_en(reg, WM_HAL_SDH_DISABLE);
    wm_ll_sdh_set_fifo_status_mask(reg, WM_HAL_SDH_DISABLE);

    /* write to sd buf */
    wm_ll_sdh_set_data_buf(reg, tx->buf, tx->len);

    /* set transfer len */
    wm_ll_sdh_mmc_set_byte_count(reg, tx->len);

    wm_ll_sdh_set_int_sts(reg, WM_HAL_SDH_CLR_MMC_INT_ALL);

    /* unmask sdio irq and enable sdio host irq */
    wm_ll_sdh_mmc_set_data_complete_int_mask(reg, WM_HAL_SDH_ENABLE);

    ret = wm_hal_irq_enable(WM_IRQ_SIDO_HOST);
    if (ret != WM_ERR_SUCCESS) {
        return ret;
    }

    /* start tx transfer */
    wm_ll_sdh_mmc_set_data_dir(reg, WM_HAL_SDH_DATA_XFER_DIR_WRITE);
    wm_ll_sdh_mmc_set_auto_data_trans(reg, WM_HAL_SDH_ENABLE);

    return ret;
}

static int wm_hal_sdh_rx_data_it(wm_hal_sdh_dev_t *dev, wm_hal_sdh_data_desc_t *rx)
{
    wm_sdh_reg_t *reg = dev->reg_base;
    int ret           = WM_ERR_SUCCESS;

    if (rx == NULL || rx->buf == NULL || rx->len > WM_SDH_DATA_BUF_BYTES) {
        return WM_ERR_INVALID_PARAM;
    }

    /* use CPU + it */
    /* register cb to sdh dev with irq hal */
    ret = wm_hal_irq_attach_sw_vector(WM_IRQ_SIDO_HOST, wm_hal_sdh_irq_handler, dev);
    if (ret != WM_ERR_SUCCESS) {
        return ret;
    }

    /* buf control */
    wm_ll_sdh_set_data_flush_en(reg, WM_HAL_SDH_ENABLE);
    wm_ll_sdh_set_dma_req_mask(reg, WM_HAL_SDH_ENABLE);
    wm_ll_sdh_set_buf_access_dir(reg, WM_HAL_SDH_BUF_ACCESS_DIR_READ);
    wm_ll_sdh_set_data_pipeline_set(reg, WM_HAL_SDH_BUF_CTRL_PIPELINE_1);
    wm_ll_sdh_set_dma_hw_intf_en(reg, WM_HAL_SDH_DISABLE);
    wm_ll_sdh_set_fifo_status_mask(reg, WM_HAL_SDH_DISABLE);

    wm_ll_sdh_set_int_sts(reg, WM_HAL_SDH_CLR_MMC_INT_ALL);

    /* unmask sdio irq and enable sdio host irq */
    wm_ll_sdh_mmc_set_data_complete_int_mask(reg, WM_HAL_SDH_ENABLE);

    dev->ctx.sd_rx_data = rx;

    ret = wm_hal_irq_enable(WM_IRQ_SIDO_HOST);
    if (ret != WM_ERR_SUCCESS) {
        return ret;
    }

    /* start rx transfer */
    wm_ll_sdh_mmc_set_data_dir(reg, WM_HAL_SDH_DATA_XFER_DIR_READ);
    wm_ll_sdh_mmc_set_auto_data_trans(reg, WM_HAL_SDH_ENABLE);

    return ret;
}

int wm_hal_sdh_xfer_it(wm_hal_sdh_dev_t *dev, wm_hal_sdh_xfer_desc_t *xfer)
{
    int ret                                     = WM_ERR_SUCCESS;
    wm_hal_sdh_port_mode_t port_mode            = WM_HAL_SDH_XFER_MODE_MAX;
    wm_hal_sdh_sdio_xfer_desc_t *sdio_xfer_desc = NULL;
    wm_hal_sdh_spi_xfer_desc_t *spi_xfer_desc   = NULL;

    if (dev == NULL || xfer == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    port_mode = xfer->port_mode;
    if (port_mode >= WM_HAL_SDH_PORT_MODE_MAX) {
        return WM_ERR_INVALID_PARAM;
    }

    if (port_mode == WM_HAL_SDH_PORT_MODE_MMC) {
        sdio_xfer_desc = &xfer->sdio_xfer_desc;
        if (sdio_xfer_desc == NULL) {
            return WM_ERR_INVALID_PARAM;
        }

        /* transfer cmd */
        ret = wm_hal_sdh_send_cmd_read_rsp(dev, &sdio_xfer_desc->sd_cmd);
        if (ret != WM_ERR_SUCCESS) {
            WM_HAL_SDH_LOG_E("send cmd fail with err code:%d", ret);
        }

        /* transfer data */
        if (sdio_xfer_desc->mode == WM_HAL_SDH_SDMMC_TX) {
            ret = wm_hal_sdh_tx_data_it(dev, &sdio_xfer_desc->sd_data.tx);
            if (ret != WM_ERR_SUCCESS) {
                WM_HAL_SDH_LOG_E("tx data fail with err code:%d", ret);
            }
        } else if (sdio_xfer_desc->mode == WM_HAL_SDH_SDMMC_RX) {
            ret = wm_hal_sdh_rx_data_it(dev, &sdio_xfer_desc->sd_data.rx);
            if (ret != WM_ERR_SUCCESS) {
                WM_HAL_SDH_LOG_E("rx data fail with err code:%d", ret);
            }
        } else {
            WM_HAL_SDH_LOG_I("sd cmd only");
        }

    } else if (port_mode == WM_HAL_SDH_PORT_MODE_SPI) {
        /* sdh spi tranfer mode */
        spi_xfer_desc = &xfer->spi_xfer_desc;
        if (spi_xfer_desc == NULL) {
            return WM_ERR_INVALID_PARAM;
        }
    } else {
        /* reserved for feture use */
    }

    return ret;
}

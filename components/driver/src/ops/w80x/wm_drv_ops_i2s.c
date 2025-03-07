/**
 * @file wm_drv_ops_i2s.c
 *
 * @brief DRV_OPS_I2S Module
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

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "wm_dt.h"
#include "wm_irq.h"
#include "wm_list.h"
#include "wm_hal_i2s.h"
#include "wm_osal.h"
#include "wm_soc.h"
#include "wm_soc_cfgs.h"
#include "wm_types.h"
#include "wm_error.h"
#include "wm_drv_dma.h"
#include "wm_drv_i2s.h"
#include "wm_drv_rcc.h"
#include "wm_drv_gpio.h"
#include "wm_drv_irq.h"
#include "wm_dt_hw.h"

#define LOG_TAG "DRV_I2S"
#include "wm_log.h"

#define WM_DRV_I2S_USE_RX(cfg)           ((cfg)->dir == WM_I2S_DIR_IN || (cfg)->dir == WM_I2S_DIR_INOUT)
#define WM_DRV_I2S_USE_TX(cfg)           ((cfg)->dir == WM_I2S_DIR_OUT || (cfg)->dir == WM_I2S_DIR_INOUT)

#define WM_DRV_I2S_MALLOC(size)          wm_os_internal_malloc(size)
#define WM_DRV_I2S_CALLOC(nelem, elsize) wm_os_internal_calloc(nelem, elsize)
#define WM_DRV_I2S_FREE(ptr)             wm_os_internal_free(ptr)

/**
 * @brief I2S context
 */
typedef struct {
    wm_os_mutex_t *mutex;              /**< I2S mutex                         */
    wm_device_t *rcc_dev;              /**< rcc device                        */
    wm_drv_i2s_cfg_t cfg;              /**< I2S configure                     */
    wm_drv_i2s_format_t fmt;           /**< I2S format                        */
    wm_drv_i2s_callback_t tx_callback; /**< tx callback                       */
    wm_drv_i2s_callback_t rx_callback; /**< rx callback                       */
    uint32_t mclk_hz;                  /**< master clock value in HZ          */
    bool extra_enable;                 /**< use external mclk                 */
    uint32_t bitrates;                 /**< the bit rates in unit bps         */
    uint32_t bclk_div;                 /**< the bits clock divider value      */
    uint32_t mclk_div;                 /**< the master clock divider value    */

} wm_drv_i2s_ctx_t;

/**
 * @brief I2S data
 */
typedef struct {
    wm_hal_i2s_dev_t dev; /**< I2S hal device     */
    wm_drv_i2s_ctx_t ctx; /**< I2S driver context */
} wm_drv_i2s_data_t;

/**
 * @brief I2S ops
 */
typedef struct {
    int (*init)(wm_device_t *dev, wm_drv_i2s_cfg_t *cfg);
    int (*deinit)(wm_device_t *device);
    int (*write_async)(wm_device_t *device, void *buf, int len);
    int (*write_pause)(wm_device_t *device);
    int (*write_resume)(wm_device_t *device);
    int (*write_stop)(wm_device_t *device);
    int (*read_async)(wm_device_t *device, void *buf, int len);
    int (*read_pause)(wm_device_t *device);
    int (*read_resume)(wm_device_t *device);
    int (*read_stop)(wm_device_t *device);
    int (*set_format)(wm_device_t *device, uint32_t sample_rate_hz, enum wm_i2s_bits fmt, enum wm_i2s_chan_type ch);
    int (*set_mclk)(wm_device_t *device, uint32_t mclk_hz);
    int (*ioctl)(wm_device_t *device, wm_drv_i2s_ioctl_cmd_t cmd, bool enable, uint32_t arg);
} wm_drv_i2s_ops_t;

static void i2s_rx_callback(void *master_dev, wm_hal_i2s_event_t *event)
{
    wm_drv_i2s_data_t *data           = ((wm_device_t *)master_dev)->drv;
    wm_drv_i2s_callback_t rx_callback = data->ctx.rx_callback;

    if (rx_callback) {
        /* Notify user the read data ready, user can add a new receiv buffer now */
        rx_callback((wm_device_t *)master_dev, (wm_drv_i2s_event_t *)event);
    }
}

static void i2s_tx_callback(void *master_dev, wm_hal_i2s_event_t *event)
{
    wm_drv_i2s_data_t *data           = ((wm_device_t *)master_dev)->drv;
    wm_drv_i2s_callback_t tx_callback = data->ctx.tx_callback;

    if (tx_callback) {
        /* Notify user the data has been sent, user can write a new data now */
        tx_callback((wm_device_t *)master_dev, (wm_drv_i2s_event_t *)event);
    }
}

static void i2s_enable_clock(wm_drv_i2s_data_t *data, bool en)
{
    if (en) {
        wm_drv_clock_enable(data->ctx.rcc_dev, WM_RCC_I2S_GATE_EN);
    } else {
        if (data->ctx.cfg.dir == WM_I2S_DIR_INOUT) {
            /*used , not disable*/
            if (wm_hal_i2s_is_rx_enabled(&data->dev) || wm_hal_i2s_is_tx_enabled(&data->dev)) {
                return;
            }
        }
#if 0
        wm_drv_clock_disable(data->ctx.rcc_dev, WM_RCC_I2S_GATE_EN);
#endif
    }
}

static int i2s_write_async(wm_device_t *device, void *buf, int len)
{
    wm_drv_i2s_data_t *data = device->drv;
    wm_drv_i2s_cfg_t *cfg   = &data->ctx.cfg;

    if (!WM_DRV_I2S_USE_TX(cfg)) {
        wm_log_debug("not use tx, can't write");
        return WM_ERR_NOT_ALLOWED;
    }

    /* len align to 4 , DMA xfer size must less than 2^16 */
    if (len <= 0 || len > 0xffff || (len & 3)) {
        wm_log_debug("bad data len=%d", len);
        return WM_ERR_NOT_ALLOWED;
    }

    i2s_enable_clock(data, true);

    return wm_hal_i2s_tx_dma_buf_append(&((wm_drv_i2s_data_t *)(device->drv))->dev, buf, len);
}

static int i2s_write_stop(wm_device_t *device)
{
    int err = wm_hal_i2s_tx_stop(&((wm_drv_i2s_data_t *)(device->drv))->dev);

    i2s_enable_clock((wm_drv_i2s_data_t *)device->drv, false);

    return err;
}

static int i2s_write_pause(wm_device_t *device)
{
    return wm_hal_i2s_tx_pause(&((wm_drv_i2s_data_t *)(device->drv))->dev);
}

static int i2s_write_resume(wm_device_t *device)
{
    return wm_hal_i2s_tx_resume(&((wm_drv_i2s_data_t *)(device->drv))->dev);
}

static int i2s_read_async(wm_device_t *device, void *buf, int len)
{
    wm_drv_i2s_data_t *data = device->drv;
    wm_drv_i2s_ctx_t *ctx   = &data->ctx;
    wm_drv_i2s_cfg_t *cfg   = &ctx->cfg;
    wm_hal_i2s_dev_t *dev   = &data->dev;

    if (!WM_DRV_I2S_USE_RX(cfg)) {
        wm_log_debug("not use rx, can't read");
        return WM_ERR_NOT_ALLOWED;
    }

    if (len != cfg->rx_pkt_size) {
        wm_log_debug("err: len=%d,pkg_size=%u", len, cfg->rx_pkt_size);
        return WM_ERR_NOT_ALLOWED;
    }

    i2s_enable_clock(data, true);

    /*add receive buffer to hal*/
    return wm_hal_i2s_rx_dma_buf_append(dev, buf, len);
}

static int i2s_read_stop(wm_device_t *device)
{
    int err = wm_hal_i2s_rx_stop(&((wm_drv_i2s_data_t *)(device->drv))->dev);

    i2s_enable_clock((wm_drv_i2s_data_t *)device->drv, false);

    return err;
}

static int i2s_read_pause(wm_device_t *device)
{
    return wm_hal_i2s_rx_pause(&((wm_drv_i2s_data_t *)(device->drv))->dev);
}

static int i2s_read_resume(wm_device_t *device)
{
    return wm_hal_i2s_rx_resume(&((wm_drv_i2s_data_t *)(device->drv))->dev);
}

static int i2s_check_cfg(wm_drv_i2s_cfg_t *cfg)
{
    if (!(cfg->mode >= 0 && cfg->mode < WM_I2S_MODE_MAX)) {
        wm_log_debug("bad mode");
        return WM_ERR_INVALID_PARAM;
    }

    if (!(cfg->dir >= 0 && cfg->dir < WM_I2S_DIR_MAX)) {
        wm_log_debug("bad dir");
        return WM_ERR_INVALID_PARAM;
    }

    if (!(cfg->std >= 0 && cfg->std < WM_I2S_STD_MAX)) {
        wm_log_debug("bad std");
        return WM_ERR_INVALID_PARAM;
    }

    if (!(cfg->xtype >= 0 && cfg->xtype < WM_I2S_XFER_MAX)) {
        wm_log_debug("bad xtype");
        return WM_ERR_INVALID_PARAM;
    }

    if ((cfg->rx_pkt_size < WM_DRV_I2S_RX_MIN_PKT_LEN || cfg->rx_pkt_size > WM_DRV_I2S_RX_MAX_PKT_LEN)) {
        wm_log_debug("bad rx pkt size");
        return WM_ERR_INVALID_PARAM;
    }

    if (cfg->rx_pkt_size & 3) {
        wm_log_debug("rx pkt size must align 4 bytes");
        return WM_ERR_INVALID_PARAM;
    }

    if (cfg->rx_pkt_num < WM_DRV_I2S_RX_MIN_PKT || cfg->rx_pkt_num > WM_DRV_I2S_RX_MAX_PKT) {
        wm_log_debug("bad rx pkt num");
        return WM_ERR_INVALID_PARAM;
    }

    if (cfg->tx_pkt_num < WM_DRV_I2S_TX_MIN_PKT || cfg->tx_pkt_num > WM_DRV_I2S_TX_MAX_PKT) {
        wm_log_debug("bad tx pkt num");
        return WM_ERR_INVALID_PARAM;
    }

    return WM_ERR_SUCCESS;
}

static uint32_t i2s_get_bitrate(wm_drv_i2s_ctx_t *ctx, wm_drv_i2s_format_t *fmt)
{
    uint32_t s = fmt->sample_rate_hz;

    uint8_t w = fmt->bits;

    uint8_t c = 2;

    return (s * w * c);
}

static int i2s_set_clock(wm_drv_i2s_ctx_t *ctx, uint32_t ref_clock_hz)
{
    ctx->bclk_div = (ref_clock_hz + ctx->bitrates) / (ctx->bitrates) - 1;
    ctx->mclk_div = ref_clock_hz / ctx->mclk_hz;
    if (ctx->mclk_div > 0x3F) {
        ctx->mclk_div = 0x3F;
    }

    wm_log_debug("clock:%u, bitrate:%d, mclk:%u, extal:%d, mdiv:%d, bdiv:%d", ref_clock_hz, ctx->bitrates, ctx->mclk_hz,
                 ctx->extra_enable, ctx->mclk_div, ctx->bclk_div);

    return wm_drv_rcc_set_i2s_clock(ctx->rcc_dev, ctx->extra_enable, true, ctx->mclk_div, ctx->bclk_div);
}

static int i2s_init_clock(wm_drv_i2s_ctx_t *ctx)
{
    int err;
    uint32_t ref_clock_hz;

    ctx->bitrates = i2s_get_bitrate(ctx, &ctx->fmt);

    if (ctx->bitrates == 0) {
        return WM_ERR_FAILED;
    }

    err = wm_drv_clock_enable(ctx->rcc_dev, WM_RCC_I2S_GATE_EN);
    if (err != WM_ERR_SUCCESS) {
        return WM_ERR_FAILED;
    }

    ref_clock_hz = UNIT_MHZ * wm_drv_rcc_get_config_clock(ctx->rcc_dev, WM_RCC_TYPE_PERIPHERAL);

    err = wm_drv_clock_enable(ctx->rcc_dev, WM_RCC_I2S_GATE_EN);
    if (err != WM_ERR_SUCCESS) {
        return WM_ERR_FAILED;
    }

    return i2s_set_clock(ctx, ref_clock_hz);
}

static int i2s_init_hal_cfg(wm_dt_hw_i2s_t *hw, wm_drv_i2s_cfg_t *cfg, wm_hal_i2s_cfg_t *hal_cfg)
{
    wm_device_t *dma_dev = NULL;
    uint8_t dma_ch;
    int err;

    hal_cfg->mode        = cfg->mode;
    hal_cfg->dir         = cfg->dir;
    hal_cfg->std         = cfg->std;
    hal_cfg->xtype       = cfg->xtype;
    hal_cfg->rx_callback = i2s_rx_callback;
    hal_cfg->tx_callback = i2s_tx_callback;

    dma_dev = wm_dt_get_device_by_name(hw->dma_device_name);
    if (!dma_dev) {
        return WM_ERR_FAILED;
    }

    if (cfg->xtype == WM_I2S_XFER_DMA) {
        if (WM_DRV_I2S_USE_TX(cfg)) {
            wm_log_debug("cfg use tx");
            err = wm_drv_dma_request_ch(dma_dev, &dma_ch, 0);
            if (err == WM_ERR_SUCCESS) {
                hal_cfg->tx_dma     = &((wm_dma_data_t *)dma_dev->drv)->hal_dev;
                hal_cfg->tx_dma_ch  = dma_ch;
                hal_cfg->tx_pkt_num = cfg->tx_pkt_num;
            } else {
                wm_log_debug("request dma ch for tx fail,err=%d", err);
                return WM_ERR_FAILED;
            }
        }

        if (WM_DRV_I2S_USE_RX(cfg)) {
            wm_log_debug("cfg use rx");
            err = wm_drv_dma_request_ch(dma_dev, &dma_ch, 0);
            if (err == WM_ERR_SUCCESS) {
                hal_cfg->rx_dma      = &((wm_dma_data_t *)dma_dev->drv)->hal_dev;
                hal_cfg->rx_dma_ch   = dma_ch;
                hal_cfg->rx_pkt_num  = cfg->rx_pkt_num;
                hal_cfg->rx_pkt_size = cfg->rx_pkt_size;
            } else {
                wm_log_debug("request dma ch for rx fail,err=%d", err);
                return WM_ERR_FAILED;
            }
        }
    } else {
        return WM_ERR_INVALID_PARAM;
    }

    return WM_ERR_SUCCESS;
}

static int i2s_pin_config(wm_dt_hw_pin_cfg_t *pin_cfg, int count)
{
    wm_gpio_num_t pin;

    for (int i = 0; i < count; i++) {
        pin = pin_cfg[i].pin_num;
        wm_drv_gpio_iomux_func_sel(pin, pin_cfg[i].pin_mux);
    }

    return WM_ERR_SUCCESS;
}

static int i2s_init(wm_device_t *device, wm_drv_i2s_cfg_t *cfg)
{
    int err = WM_ERR_SUCCESS;

    wm_dt_hw_i2s_t *hw = device->hw;

    wm_drv_i2s_data_t *data   = device->drv;
    wm_drv_i2s_ctx_t *ctx     = &data->ctx;
    wm_hal_i2s_dev_t *hal_dev = &data->dev;

    wm_hal_i2s_cfg_t hal_cfg = { 0 };

    ctx->cfg = *cfg;

    /*check config param */
    err = i2s_check_cfg(cfg);
    if (err != WM_ERR_SUCCESS) {
        wm_log_debug("check cfg fail");
        return err;
    }

    /*init i2s pin */
    err = i2s_pin_config(hw->pin_cfg, hw->pin_cfg_count);
    if (err != WM_ERR_SUCCESS) {
        return err;
    }

    ctx->mclk_hz      = hw->i2s_cfg.mclk_hz;
    ctx->extra_enable = hw->i2s_cfg.extal_clock_en;

    ctx->rcc_dev = wm_dt_get_device_by_name(hw->rcc_device_name);
    if (!ctx->rcc_dev) {
        return WM_ERR_FAILED;
    }

    /*prepare hal i2s configuration*/
    err = i2s_init_hal_cfg(hw, cfg, &hal_cfg);
    if (err != WM_ERR_SUCCESS) {
        return err;
    }

    hal_dev->master_dev = (void *)device;
    hal_dev->reg_dev    = (wm_i2s_reg_t *)(hw->reg_base);
    hal_dev->irq_num    = hw->irq_cfg.irq_num;

    /*init hal i2s*/
    err = wm_hal_i2s_init(hal_dev, &hal_cfg);
    if (err != WM_ERR_SUCCESS) {
        wm_log_debug("init hal fail");
        return err;
    }

    return err;
}

static int i2s_deinit(wm_device_t *device)
{
    wm_device_t *dma_dev = NULL;

    wm_drv_i2s_data_t *data   = device->drv;
    wm_hal_i2s_dev_t *hal_dev = &data->dev;
    wm_drv_i2s_cfg_t *cfg     = &data->ctx.cfg;
    wm_dt_hw_i2s_t *hw        = device->hw;

    uint8_t rx_dma_ch = hal_dev->cfg.rx_dma_ch;
    uint8_t tx_dma_ch = hal_dev->cfg.tx_dma_ch;

    /*deinit hal i2s*/
    wm_hal_i2s_deinit(hal_dev);

    if (cfg->xtype == WM_I2S_XFER_DMA) {
        /*release dma channels*/
        dma_dev = wm_dt_get_device_by_name(hw->dma_device_name);
        if (dma_dev) {
            if (WM_DRV_I2S_USE_TX(cfg)) {
                wm_drv_dma_release_ch(dma_dev, tx_dma_ch, WM_OS_WAIT_TIME_MAX);
            }

            if (WM_DRV_I2S_USE_RX(cfg)) {
                wm_drv_dma_release_ch(dma_dev, rx_dma_ch, WM_OS_WAIT_TIME_MAX);
            }
        }
    }

    /*disable i2s clock*/
    return wm_drv_clock_disable(data->ctx.rcc_dev, WM_RCC_I2S_GATE_EN);
}

static int i2s_ioctl(wm_device_t *device, wm_drv_i2s_ioctl_cmd_t cmd, bool enable, uint32_t arg)
{
    wm_drv_i2s_data_t *data = device->drv;
    wm_hal_i2s_dev_t *dev   = &data->dev;
    wm_drv_i2s_cfg_t *cfg   = &data->ctx.cfg;

    int err = WM_ERR_SUCCESS;

    switch (cmd) {
        case WM_DRV_I2S_CMD_SET_TX_BITSHIFT:
        {
            if (WM_DRV_I2S_USE_TX(cfg)) {
                wm_hal_i2s_set_tx_clock_inv_en(dev, enable);
            } else {
                err = WM_ERR_FAILED;
            }
            break;
        }
        case WM_DRV_I2S_CMD_SET_RX_BITSHIFT:
        {
            if (WM_DRV_I2S_USE_RX(cfg)) {
                wm_hal_i2s_set_rx_clock_inv_en(dev, enable);
            } else {
                err = WM_ERR_FAILED;
            }
            break;
        }
        case WM_DRV_I2S_CMD_SET_LZC:
        {
            wm_hal_i2s_set_left_zero_cross_en(dev, enable);
            break;
        }
        case WM_DRV_I2S_CMD_SET_RZC:
        {
            wm_hal_i2s_set_right_zero_cross_en(dev, enable);
            break;
        }
        default:
            err = WM_ERR_INVALID_PARAM;
            break;
    }

    return err;
}

int i2s_set_format(wm_device_t *device, uint32_t sample_rate_hz, enum wm_i2s_bits bits, enum wm_i2s_chan_type channel)
{
    int err;

    wm_drv_i2s_data_t *data  = device->drv;
    wm_hal_i2s_dev_t *dev    = &data->dev;
    wm_drv_i2s_ctx_t *ctx    = &data->ctx;
    wm_drv_i2s_format_t *fmt = &ctx->fmt;

    if (!(bits >= 0 && bits < WM_I2S_BITS_MAX)) {
        wm_log_debug("bad fmt");
        return WM_ERR_INVALID_PARAM;
    }

    if (!(channel >= 0 && channel < WM_I2S_CHAN_TYPE_MAX)) {
        wm_log_debug("bad ctype");
        return WM_ERR_INVALID_PARAM;
    }

    if (!(sample_rate_hz >= WM_DRV_I2S_SP_8000 && sample_rate_hz < WM_DRV_I2S_SP_MAX)) {
        wm_log_debug("bad sample rate");
        return WM_ERR_INVALID_PARAM;
    }

    if (WM_DRV_I2S_USE_TX(&(ctx->cfg)) && wm_hal_i2s_is_tx_enabled(dev)) {
        wm_log_debug("tx is using");
        return WM_ERR_BUSY;
    }

    if (WM_DRV_I2S_USE_RX(&(ctx->cfg)) && wm_hal_i2s_is_rx_enabled(dev)) {
        wm_log_debug("rx is using");
        return WM_ERR_BUSY;
    }

    fmt->sample_rate_hz = sample_rate_hz;
    fmt->bits           = bits;
    fmt->ctype          = channel;

    err = i2s_init_clock(ctx);
    wm_log_debug("set clock err=%d", err);
    err |= wm_hal_i2s_set_format(dev, bits, channel);
    wm_log_debug("set format err=%d", err);
    return err;
}

int i2s_set_mclk(wm_device_t *device, uint32_t mclk_hz)
{
    wm_drv_i2s_data_t *data = device->drv;
    wm_drv_i2s_ctx_t *ctx   = &data->ctx;

    if (!(mclk_hz > 0)) {
        return WM_ERR_INVALID_PARAM;
    }

    ctx->mclk_hz = mclk_hz;

    return i2s_init_clock(ctx);
}

/*internal interface ops*/
const wm_drv_i2s_ops_t wm_drv_i2s_ops = {
    .init         = i2s_init,
    .deinit       = i2s_deinit,
    .write_async  = i2s_write_async,
    .write_stop   = i2s_write_stop,
    .write_pause  = i2s_write_pause,
    .write_resume = i2s_write_resume,
    .read_async   = i2s_read_async,
    .read_stop    = i2s_read_stop,
    .read_pause   = i2s_read_pause,
    .read_resume  = i2s_read_resume,
    .set_format   = i2s_set_format,
    .set_mclk     = i2s_set_mclk,
    .ioctl        = i2s_ioctl,
};

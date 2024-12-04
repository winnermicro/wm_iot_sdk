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

#define WM_DRV_I2S_DEBUG

#ifdef WM_DRV_I2S_DEBUG
#include "wm_log.h"
#undef LOG_TAG
#define LOG_TAG               "I2S DRV"
#define WM_DRV_I2S_LOG_E(...) wm_log_error(__VA_ARGS__)
#define WM_DRV_I2S_LOG_W(...) wm_log_warn(__VA_ARGS__)
#define WM_DRV_I2S_LOG_I(...) wm_log_info(__VA_ARGS__)
#define WM_DRV_I2S_LOG_D(...) wm_log_info(__VA_ARGS__)
#define WM_DRV_I2S_ASSERT(v)  assert(v)
#define WM_DRV_I2S_LOG_MARKER WM_DRV_I2S_LOG_E("%s@%d", __func__, __LINE__)
#else //WM_DRV_I2S_DEBUG
#define WM_DRV_I2S_LOG_E(...)
#define WM_DRV_I2S_LOG_W(...)
#define WM_DRV_I2S_LOG_I(...)
#define WM_DRV_I2S_LOG_D(...)
#define WM_DRV_I2S_ASSERT(...)
#define WM_DRV_I2S_LOG_MARKER
#endif //WM_DRV_I2S_DEBUG

#define WM_I2S_PRE_VALID() (!!device && device->drv && !!((wm_drv_i2s_data_t *)(device->drv))->ctx.lock)

#define WM_DRV_I2S_TRY_LOCK() \
    (WM_ERR_SUCCESS == wm_os_internal_mutex_acquire(((wm_drv_i2s_data_t *)(device->drv))->ctx.lock, WM_DRV_I2S_TIMEOUT))

#define WM_DRV_I2S_UNLOCK()   wm_os_internal_mutex_release(((wm_drv_i2s_data_t *)(device->drv))->ctx.lock)

#define WM_DRV_I2S_OPS(d)     ((wm_drv_i2s_ops_t *)(d->ops))

#define WM_DRV_I2S_TIMEOUT    1000
#define WM_DRV_I2S_RBUF_MAGIC 0x5c5c5a5a
#define WM_DRV_I2S_RBUF_PENDING \
    32 /**< max number of bufs to be free, total size will be:
                                            WM_DRV_I2S_RBUF_PENDING * rx_pkt_size */

#define WM_DRV_I2S_MS_FACTOR         1000
#define WM_DRV_I2S_4BYTE_ALIGN(addr) ((addr) & 0x3)
#define WM_DRV_I2S_MCLK_COFACTOR     (256) /**< coefficient factor to multiple sample rate */
#define WM_DRV_I2S_USE_WRITE_EXT_API 0     /**< if use wm_drv_i2s_write_async_ext() or not */

/**
 * @brief node buffer index container
 */
typedef struct {
    uint8_t wf : 1; /**< wi flag, warp flag indicate loop back to head */
    uint8_t wi : 7; /**< the index of the node to be written. */
    uint8_t rf : 1; /**< ri flag, warp flag indicate loop back to head */
    uint8_t ri : 7; /**< the index of the node to be pop to release. */
    uint8_t pf : 1; /**< rf flag, warp flag indicate loop back to head */
    uint8_t pi : 7; /**< the index of the node to be pick for DMA desc using */
} wm_drv_i2s_nbi_t;

/**
 * @brief transmit node buffer container for user ready buffers
 */
typedef struct wm_drv_i2s_tbnc {
    /* bool running; */
    wm_drv_i2s_nbi_t nbi;
    bool running;
    wm_drv_i2s_tx_nb_t nb[WM_DRV_I2S_NBC_TDEPTH]; /** ring buffer nodes */
} wm_drv_i2s_tx_nbc_t;

/**
 * @brief receive node buffer container for user ready buffers
 */
typedef struct wm_drv_i2s_rbnc {
    wm_drv_i2s_nbi_t nbi;
    bool running;
    uint8_t rx_buf_release; /**< total release buf cnt by user */
    uint8_t rx_buf_request; /**< total request buffer number by driver */
    uint8_t rx_buf_oncache; /**< total cnt on the cache list to ready to report to user */
    uint64_t rx_total_bytes;
    wm_drv_i2s_rxbuf_t *nb[WM_DRV_I2S_NBC_RDEPTH]; /** ring buffer nodes */
    wm_drv_i2s_rxbuf_t *rx_ready_head;
    wm_drv_i2s_rxbuf_t *rx_ready_tail;
} wm_drv_i2s_rx_nbc_t;

/**
 * @brief private configuration items for I2S
 */
typedef struct {
    uint32_t mclk_hz;  /**< external clock source value in HZ */
    uint32_t bitrates; /**< the bit rates in unit bps */
    uint32_t bclk_div; /**< the bits clock divider value */
    uint32_t mclk_div; /**< the master clock divider value */
} wm_drv_i2s_cfg_priv_t;

typedef struct {
    wm_os_mutex_t *lock;
    wm_device_t *i2s_dev;
    wm_device_t *dma_dev;
    wm_device_t *clock_dev;
    wm_drv_i2s_tx_nbc_t tnbc;
    wm_drv_i2s_rx_nbc_t rnbc;
    wm_drv_i2s_cfg_t cfg;
    wm_drv_i2s_cfg_priv_t priv_cfg;
    wm_dt_hw_i2s_t *hw;
    i2s_rx_ready_cb_t user_rx_cb;
    i2s_tx_done_cb_t user_tx_cb;
    bool write_pause;
    bool read_pause;
    int8_t tx_desc_depth;
    int8_t rx_desc_depth;
    uint32_t i2s_clock_hz;
} wm_drv_i2s_ctx_t;

typedef struct {
    wm_hal_i2s_dev_t dev;
    wm_drv_i2s_ctx_t ctx;
} wm_drv_i2s_data_t;

typedef struct {
    int (*init)(wm_device_t *dev, wm_drv_i2s_cfg_t *cfg);
    int (*deinit)(wm_device_t *device);
    int (*write_async)(wm_device_t *device, void *buf, int len, i2s_free_buf_cb_t cb);
    int (*write_pause)(wm_device_t *device);
    int (*write_resume)(wm_device_t *device);
    int (*write_stop)(wm_device_t *device);
    int (*read_async)(wm_device_t *device);
    int (*read_pause)(wm_device_t *device);
    int (*read_resume)(wm_device_t *device);
    int (*read_stop)(wm_device_t *device);
    int (*ioctl)(wm_device_t *device, wm_drv_i2s_ioctl_args_t *args);
} wm_drv_i2s_ops_t;

typedef void (*en_t)(wm_hal_i2s_dev_t *dev, bool en);

static int wm_drv_i2s_w800_rnb_online_cnt(wm_drv_i2s_rx_nbc_t *rnbc);
static int wm_drv_i2s_w800_tnbc_push(wm_drv_i2s_data_t *data, i2s_tx_done_cb_t cb, void *buf, int len);
static wm_drv_i2s_tx_nb_t *wm_drv_i2s_w800_tnbc_pick(wm_drv_i2s_data_t *data);
static wm_drv_i2s_tx_nb_t *wm_drv_i2s_w800_tnbc_pop(wm_drv_i2s_data_t *data);
static int wm_drv_i2s_w800_rnbc_push(wm_drv_i2s_data_t *data);
static wm_drv_i2s_rxbuf_t *wm_drv_i2s_w800_rnbc_pick(wm_drv_i2s_data_t *data);
static wm_drv_i2s_rxbuf_t *wm_drv_i2s_w800_rnbc_pop(wm_drv_i2s_data_t *data);

static int wm_drv_i2s_w800_read_stop(wm_device_t *device);

static const en_t en_handls[WM_DRV_I2S_CMD_SET_MAX][WM_HAL_I2S_TR_MAX] = {
    [WM_DRV_I2S_CMD_SET_MUTE]      = { wm_hal_i2s_set_tx_mute_en,          wm_hal_i2s_set_rx_en               },
    [WM_DRV_I2S_CMD_SET_BITSHIFT]  = { wm_hal_i2s_set_tx_clock_inv_en,     wm_hal_i2s_set_rx_clock_inv_en     },
    [WM_DRV_I2S_CMD_SET_LZC]       = { wm_hal_i2s_set_left_zero_cross_en,  wm_hal_i2s_set_left_zero_cross_en  },
    [WM_DRV_I2S_CMD_SET_RZC]       = { wm_hal_i2s_set_right_zero_cross_en, wm_hal_i2s_set_right_zero_cross_en },
    [WM_DRV_I2S_CMD_SET_MONO_LEFT] = { wm_hal_i2s_set_mono_left_en,        wm_hal_i2s_set_mono_left_en        },
};

static void *desc_calloc(size_t nelem, size_t elsize)
{
    return wm_os_internal_calloc(nelem, elsize);
}

static void desc_release(void *p)
{
    if (p == NULL) {
        WM_DRV_I2S_ASSERT(0);
    }

    wm_os_internal_free(p);
}

static void wm_drv_i2s_w800_slient_clock(wm_drv_i2s_data_t *data, bool en)
{
    if (data->ctx.tnbc.running != false || data->ctx.rnbc.running != false) {
        return;
    }

    if (en) {
        wm_drv_clock_enable(data->ctx.clock_dev, WM_RCC_I2S_GATE_EN);
    } else {
        wm_drv_clock_disable(data->ctx.clock_dev, WM_RCC_I2S_GATE_EN);
    }
}

static uint32_t wm_drv_i2s_w800_get_bitrate(wm_drv_i2s_data_t *data)
{
    wm_drv_i2s_ctx_t *ctx = &data->ctx;
    wm_drv_i2s_cfg_t *cfg = &ctx->cfg;

    uint32_t s = cfg->sample_rate_hz;

    uint8_t w = (cfg->fmt + 1) << 3;

    uint8_t c = cfg->ctype;

    return (s * w * c);
}

static bool wm_drv_i2s_w800_check_trx_support(wm_drv_i2s_cfg_t *cfg, enum wm_i2s_dir dir)
{
    if (cfg->dir == WM_I2S_DIR_INOUT) {
        return true;
    }

    if (cfg->dir == dir) {
        return true;
    }

    return false;
}

/* TODO: should use common API from GPIO module to reset the PIN */
static void wm_drv_i2s_w800_gpio_reset(wm_drv_i2s_data_t *data)
{
    for (int i = 0; i < data->ctx.hw->pin_cfg_count; i++) {
        wm_drv_gpio_iomux_func_sel(data->ctx.hw->pin_cfg[i].pin_num, WM_GPIO_IOMUX_FUN5);
    }
}

static int wm_drv_i2s_w800_set_clock(wm_drv_i2s_ctx_t *ctx, bool extal)
{
    uint32_t mclk_div = WM_I2S_MCLK_DIV_MIN, bclk_div = 0;

    uint32_t master_clock_required = (WM_DRV_I2S_MCLK_COFACTOR * ctx->cfg.sample_rate_hz);

    if ((ctx->i2s_clock_hz % ctx->priv_cfg.bitrates) != 0) {
        WM_DRV_I2S_LOG_D("Cannot generate an integer bits clock division factor!");
    }

    if ((ctx->i2s_clock_hz % master_clock_required) != 0) {
        WM_DRV_I2S_LOG_D("Cannot generate an integer master clock division factor!");
    }

    bclk_div = (ctx->i2s_clock_hz / ctx->priv_cfg.bitrates);
    if (bclk_div == 0 || bclk_div > WM_I2S_BCLK_DIV_MAX) {
        WM_DRV_I2S_LOG_E("Not support, bclk division error!");
        return WM_ERR_NO_SUPPORT;
    }

    mclk_div = ctx->i2s_clock_hz / (ctx->priv_cfg.mclk_hz);
    if (mclk_div < WM_I2S_MCLK_DIV_MIN || mclk_div > WM_I2S_MCLK_DIV_MAX) {
        WM_DRV_I2S_LOG_E("mclk division error!");
        return WM_ERR_NO_SUPPORT;
    }

    ctx->priv_cfg.bclk_div = bclk_div;
    ctx->priv_cfg.mclk_div = mclk_div;

    WM_DRV_I2S_LOG_D("clock:%u, bitrate:%d, mclk:%u, extal:%d, mdiv:%d, bdiv:%d", ctx->i2s_clock_hz, ctx->priv_cfg.bitrates,
                     ctx->priv_cfg.mclk_hz, extal, mclk_div, bclk_div);

    return wm_drv_rcc_set_i2s_clock(ctx->clock_dev, extal, true, mclk_div, bclk_div);
}

static int wm_drv_i2s_w800_init_clock(wm_drv_i2s_data_t *data, wm_dt_hw_i2s_t *hw)
{
    wm_drv_i2s_ctx_t *ctx = &data->ctx;

    memset(&ctx->priv_cfg, 0x0, sizeof(ctx->priv_cfg));

    ctx->priv_cfg.bitrates = wm_drv_i2s_w800_get_bitrate(data);
    if (ctx->priv_cfg.bitrates == 0) {
        WM_DRV_I2S_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    ctx->i2s_clock_hz = UNIT_MHZ * wm_drv_rcc_get_config_clock(ctx->clock_dev, WM_RCC_TYPE_PERIPHERAL);
    if (ctx->i2s_clock_hz == 0) {
        WM_DRV_I2S_LOG_E("Peripheral clock is not ready!");
        return WM_ERR_NOT_ALLOWED;
    }

    if (WM_ERR_SUCCESS != wm_drv_clock_enable(ctx->clock_dev, WM_RCC_I2S_GATE_EN)) {
        WM_DRV_I2S_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    if (WM_ERR_SUCCESS != wm_drv_clock_reset(ctx->clock_dev, WM_RCC_I2S_RST)) {
        WM_DRV_I2S_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    ctx->priv_cfg.mclk_hz = hw->i2s_cfg.mclk_hz;

    return wm_drv_i2s_w800_set_clock(ctx, hw->i2s_cfg.extal_clock_en);
}

static int wm_drv_i2s_w800_try_tx_dma(wm_drv_i2s_data_t *data)
{
    int ret;

    wm_drv_i2s_tx_nb_t *node = NULL;

    /* try to append to DMA desc list, we don't pop if fail */
    node = wm_drv_i2s_w800_tnbc_pick(data);
    if (node == NULL) {
        return WM_ERR_FAILED;
    }

    ret = wm_hal_i2s_tx_dma_buf_append(&data->dev, node->data, node->len);
    if (ret != WM_ERR_SUCCESS) {
        node = wm_drv_i2s_w800_tnbc_pop(data);
        if (node && node->data) {
#if WM_DRV_I2S_USE_WRITE_EXT_API
            if (node->free) {
                node->free(node->data);
            }
#else
            if (data->ctx.user_tx_cb) {
                data->ctx.user_tx_cb(node->data);
            }
#endif
        }
        WM_DRV_I2S_ASSERT(0);
        return WM_ERR_FAILED;
    }

    return WM_ERR_SUCCESS;
}

static int wm_drv_i2s_w800_try_rx_dma(wm_drv_i2s_data_t *data)
{
    int ret = WM_ERR_SUCCESS;

    wm_drv_i2s_rxbuf_t *node;

    wm_hal_i2s_dev_t *dev = &data->dev;

    node = wm_drv_i2s_w800_rnbc_pick(data);
    if (node == NULL) {
        WM_DRV_I2S_LOG_MARKER;
        WM_DRV_I2S_ASSERT(0);
        return WM_ERR_FAILED;
    }

    ret = wm_hal_i2s_rx_dma_buf_append(dev, node->data, node->len);
    if (ret != WM_ERR_SUCCESS) {
        node = wm_drv_i2s_w800_rnbc_pop(data);
        if (node->free) {
            node->free(node);
        }
        WM_DRV_I2S_LOG_MARKER;
        WM_DRV_I2S_ASSERT(0);
        return WM_ERR_FAILED;
    }

    return WM_ERR_SUCCESS;
}

static void wm_drv_i2s_w800_tx_done_cb(void *master_dev, wm_hal_i2s_trx_param_t *param)
{
    int i, try_cnt = 0;

    wm_device_t *dev = (wm_device_t *)master_dev;

    wm_drv_i2s_data_t *data      = dev->drv;
    wm_drv_i2s_ctx_t *ctx        = &data->ctx;
    wm_drv_i2s_tx_nb_t *consumed = NULL;

    while (param->nodes_consumed > 0) {
        consumed = wm_drv_i2s_w800_tnbc_pop(data);
        if (consumed->data == NULL) {
            WM_DRV_I2S_ASSERT(0);
            break;
        }

        if (param->start_buf && consumed->data != param->start_buf) {
            WM_DRV_I2S_ASSERT(0);
        }

        param->start_buf = NULL;

        /* call user registered tx done callback to notification */
        if (ctx->user_tx_cb) {
            ctx->user_tx_cb(consumed->data);
        }

#if WM_DRV_I2S_USE_WRITE_EXT_API
        /* call user free callback from write async function parameter */
        if (consumed->free) {
            consumed->free(consumed->data);
        }
#endif

        consumed->data = NULL;

        param->nodes_consumed--;
    }

    /* XXX:
     * check node buffer container for new user buffer to append to desc
     * we will try to mount all valid buffer to dma desc to keep dma busy
     *
     * need to check `wm_hal_i2s_is_txdma_zombie`
     * */
    for (i = 0; i < param->slots_remained; i++) {
        if (WM_ERR_SUCCESS != wm_drv_i2s_w800_try_tx_dma(data)) {
            break;
        }

        try_cnt++;
    }

    if (wm_hal_i2s_is_txdma_zombie(&data->dev)) {
        wm_hal_i2s_tx_dma_start(&data->dev);
    }
}

static void wm_drv_i2s_w800_rx_ready_cb(void *master_dev, wm_hal_i2s_trx_param_t *param)
{
    int i, try_cnt = 0;

    uint8_t online_cnt;

    wm_device_t *dev = (wm_device_t *)master_dev;

    wm_drv_i2s_data_t *data      = dev->drv;
    wm_drv_i2s_ctx_t *ctx        = &data->ctx;
    wm_drv_i2s_rx_nbc_t *rnbc    = &ctx->rnbc;
    wm_drv_i2s_rxbuf_t *consumed = NULL;

    if (rnbc->running == false) {
        return;
    }

    while (param->nodes_consumed > 0) {
        consumed = wm_drv_i2s_w800_rnbc_pop(data);
        if (consumed == NULL) {
            WM_DRV_I2S_ASSERT(0);
            break;
        }

        if (param->start_buf && consumed->data != param->start_buf) {
            WM_DRV_I2S_ASSERT(0);
        }

        param->start_buf = NULL;

        if (WM_ERR_SUCCESS != wm_drv_i2s_w800_rnbc_push(data)) {
            WM_DRV_I2S_ASSERT(0);
            break;
        }

        consumed->len = param->len;
        rnbc->rx_total_bytes += param->len;

        if (rnbc->rx_ready_head == NULL) {
            rnbc->rx_ready_tail = consumed;
            rnbc->rx_ready_head = rnbc->rx_ready_tail;
        } else {
            rnbc->rx_ready_tail->next = consumed;
            rnbc->rx_ready_tail       = consumed;
        }

        rnbc->rx_ready_tail->next = NULL;
        rnbc->rx_buf_oncache++;

        param->nodes_consumed--;
    }

    /* call user registered rx done callback to notification */
    if (rnbc->rx_buf_oncache > ctx->cfg.rx_ready_th) {
        if (ctx->user_rx_cb) {
            /* SHOULD NOT be blocked */
            ctx->user_rx_cb(rnbc->rx_ready_head);
            rnbc->rx_buf_oncache = 0;
            rnbc->rx_ready_head  = NULL;
            rnbc->rx_ready_tail  = NULL;
        }
    }

    online_cnt = wm_drv_i2s_w800_rnb_online_cnt(rnbc);
    if (online_cnt > WM_DRV_I2S_RBUF_PENDING) {
        WM_DRV_I2S_LOG_E("Stop I2S due to too many bufs need to be free[%d]!", online_cnt);
        wm_drv_i2s_w800_read_stop(ctx->i2s_dev);
    }

    /* XXX:
     * check node buffer container for new user buffer to append to desc
     * we will try to mount all valid buffer to dma desc to keep dma busy
     *
     * need to check `wm_hal_i2s_is_txdma_zombie`
     * */
    for (i = 0; i < param->slots_remained; i++) {
        if (WM_ERR_SUCCESS != wm_drv_i2s_w800_try_rx_dma(data)) {
            break;
        }

        try_cnt++;
    }

    if (try_cnt && wm_hal_i2s_is_rxdma_zombie(&data->dev)) {
        wm_hal_i2s_rx_dma_start(&data->dev);
    }
}

static int wm_drv_i2s_w800_free_rbuf_cb(void *buf)
{
    wm_drv_i2s_rx_nbc_t *rnbc;

    wm_drv_i2s_rxbuf_t *rnb = buf;

    if (buf == NULL) {
        return WM_ERR_FAILED;
    }

    if (rnb->magic != WM_DRV_I2S_RBUF_MAGIC) {
        WM_DRV_I2S_ASSERT(0);
        return WM_ERR_FAILED;
    }

    if (rnb->priv == NULL) {
        return WM_ERR_FAILED;
    }

    rnbc = rnb->priv;

    rnbc->rx_buf_release++;

    rnb->priv  = NULL;
    rnb->magic = 0;

    wm_os_internal_free(buf);

    return WM_ERR_SUCCESS;
}

static int wm_drv_i2s_w800_trnb_valid_count(wm_drv_i2s_nbi_t *nbi, uint8_t depth)
{
    if (nbi->wi == nbi->ri) {
        if (nbi->wf == nbi->rf) {
            return 0;
        } else {
            return depth;
        }
    } else if (nbi->wi > nbi->ri) {
        return nbi->wi - nbi->ri;
    } else {
        return depth - (nbi->ri - nbi->wi);
    }
}

static int wm_drv_i2s_w800_trnb_ready_count(wm_drv_i2s_nbi_t *nbi, uint8_t depth)
{
    if (nbi->wi == nbi->pi) {
        if (nbi->wf == nbi->pf) {
            return 0;
        } else {
            return depth;
        }
    } else if (nbi->wi > nbi->pi) {
        return nbi->wi - nbi->pi;
    } else {
        return depth - (nbi->pi - nbi->wi);
    }
}

static int wm_drv_i2s_w800_rnb_online_cnt(wm_drv_i2s_rx_nbc_t *rnbc)
{
    /* XXX:
     * There is no need to consider the situation where the variable request
     * catches up after being reversed, because the SW setting allows
     * the default maximum number of buffers that a user can hold to be 32.
     * */
    if (rnbc->rx_buf_release > rnbc->rx_buf_request) {
        return (uint8_t)((uint8_t)-1 - rnbc->rx_buf_release + rnbc->rx_buf_request);
    } else {
        return (uint8_t)(rnbc->rx_buf_request - rnbc->rx_buf_release);
    }
}

static void wm_drv_i2s_w800_nbi_push(wm_drv_i2s_nbi_t *nbi, uint8_t depth)
{
    uint8_t next_wi = nbi->wi + 1;
    if (next_wi >= depth) {
        nbi->wf = ~nbi->wf;
        nbi->wi = 0;
    } else {
        nbi->wi = next_wi;
    }
}

static void wm_drv_i2s_w800_nbi_pop(wm_drv_i2s_nbi_t *nbi, uint8_t depth)
{
    uint8_t next_ri = nbi->ri + 1;
    if (next_ri >= depth) {
        nbi->rf = ~nbi->rf;
        nbi->ri = 0;
    } else {
        nbi->ri = next_ri;
    }
}

static void wm_drv_i2s_w800_nbi_pick(wm_drv_i2s_nbi_t *nbi, uint8_t depth)
{
    uint8_t next_pi = nbi->pi + 1;
    if (next_pi >= depth) {
        nbi->pf = ~nbi->pf;
        nbi->pi = 0;
    } else {
        nbi->pi = next_pi;
    }
}

static void wm_drv_i2s_w800_nbi_reset(wm_drv_i2s_nbi_t *nbi)
{
    nbi->pf = nbi->wf;
    nbi->pi = nbi->wi;
    nbi->rf = nbi->wf;
    nbi->ri = nbi->wi;
}

static int wm_drv_i2s_w800_tnbc_push(wm_drv_i2s_data_t *data, i2s_free_buf_cb_t cb, void *buf, int len)
{
    uint8_t free_count = 0;

    int8_t used_cnt = 0;

    wm_drv_i2s_ctx_t *ctx = &data->ctx;

    wm_drv_i2s_tx_nbc_t *tnbc = &ctx->tnbc;

    wm_drv_i2s_tx_nb_t *node = &tnbc->nb[tnbc->nbi.wi];

    used_cnt = wm_drv_i2s_w800_trnb_valid_count(&tnbc->nbi, WM_DRV_I2S_NBC_TDEPTH);

    free_count = WM_DRV_I2S_NBC_TDEPTH - used_cnt;

    if (!free_count) {
        return WM_ERR_NO_MEM;
    }

    node->data = buf;
    node->len  = len;
#if WM_DRV_I2S_USE_WRITE_EXT_API
    node->free = cb;
#endif

    wm_drv_i2s_w800_nbi_push(&tnbc->nbi, WM_DRV_I2S_NBC_TDEPTH);

    return WM_ERR_SUCCESS;
}

static wm_drv_i2s_tx_nb_t *wm_drv_i2s_w800_tnbc_pop(wm_drv_i2s_data_t *data)
{
    uint8_t valid_count = 0;

    wm_drv_i2s_ctx_t *ctx = &data->ctx;

    wm_drv_i2s_tx_nbc_t *tnbc = &ctx->tnbc;

    wm_drv_i2s_tx_nb_t *node = &tnbc->nb[tnbc->nbi.ri];

    valid_count = wm_drv_i2s_w800_trnb_valid_count(&tnbc->nbi, WM_DRV_I2S_NBC_TDEPTH);
    if (!valid_count) {
        return NULL;
    }

    wm_drv_i2s_w800_nbi_pop(&tnbc->nbi, WM_DRV_I2S_NBC_TDEPTH);

    return node;
}

static wm_drv_i2s_tx_nb_t *wm_drv_i2s_w800_tnbc_pick(wm_drv_i2s_data_t *data)
{
    uint8_t ready_count = 0;

    wm_drv_i2s_ctx_t *ctx = &data->ctx;

    wm_drv_i2s_tx_nbc_t *tnbc = &ctx->tnbc;

    wm_drv_i2s_tx_nb_t *node = &tnbc->nb[tnbc->nbi.pi];

    ready_count = wm_drv_i2s_w800_trnb_ready_count(&tnbc->nbi, WM_DRV_I2S_NBC_TDEPTH);
    if (!ready_count) {
        return NULL;
    }

    wm_drv_i2s_w800_nbi_pick(&tnbc->nbi, WM_DRV_I2S_NBC_TDEPTH);

    return node;
}

static int wm_drv_i2s_w800_rnbc_push(wm_drv_i2s_data_t *data)
{
    int len;

    int8_t used_cnt = 0;

    uint8_t free_count = 0;

    wm_drv_i2s_ctx_t *ctx = &data->ctx;

    wm_drv_i2s_rx_nbc_t *rnbc = &ctx->rnbc;

    wm_drv_i2s_cfg_t *cfg = &ctx->cfg;

    used_cnt = wm_drv_i2s_w800_trnb_valid_count(&rnbc->nbi, WM_DRV_I2S_NBC_RDEPTH);

    free_count = WM_DRV_I2S_NBC_RDEPTH - used_cnt;
    if (!free_count) {
        return WM_ERR_NO_MEM;
    }

    len = sizeof(wm_drv_i2s_rxbuf_t) + cfg->rx_pkt_size;

    wm_drv_i2s_rxbuf_t *new_node = wm_os_internal_calloc(1, len);
    if (new_node == NULL) {
        WM_DRV_I2S_ASSERT(0);
        return WM_ERR_NO_MEM;
    }

    new_node->len   = cfg->rx_pkt_size;
    new_node->magic = WM_DRV_I2S_RBUF_MAGIC;
    new_node->free  = wm_drv_i2s_w800_free_rbuf_cb;
    new_node->priv  = rnbc;

    rnbc->nb[rnbc->nbi.wi] = new_node;

    wm_drv_i2s_w800_nbi_push(&rnbc->nbi, WM_DRV_I2S_NBC_RDEPTH);

    rnbc->rx_buf_request++;

    return WM_ERR_SUCCESS;
}

static wm_drv_i2s_rxbuf_t *wm_drv_i2s_w800_rnbc_pop(wm_drv_i2s_data_t *data)
{
    uint8_t valid_count = 0;

    wm_drv_i2s_ctx_t *ctx = &data->ctx;

    wm_drv_i2s_rx_nbc_t *rnbc = &ctx->rnbc;

    wm_drv_i2s_rxbuf_t *node = rnbc->nb[rnbc->nbi.ri];

    valid_count = wm_drv_i2s_w800_trnb_valid_count(&rnbc->nbi, WM_DRV_I2S_NBC_RDEPTH);
    if (!valid_count) {
        return NULL;
    }

    wm_drv_i2s_w800_nbi_pop(&rnbc->nbi, WM_DRV_I2S_NBC_RDEPTH);

    return node;
}

static wm_drv_i2s_rxbuf_t *wm_drv_i2s_w800_rnbc_pick(wm_drv_i2s_data_t *data)
{
    uint8_t ready_count = 0;

    wm_drv_i2s_ctx_t *ctx = &data->ctx;

    wm_drv_i2s_rx_nbc_t *rnbc = &ctx->rnbc;

    wm_drv_i2s_rxbuf_t *node = rnbc->nb[rnbc->nbi.pi];

    ready_count = wm_drv_i2s_w800_trnb_ready_count(&rnbc->nbi, WM_DRV_I2S_NBC_RDEPTH);
    if (!ready_count) {
        return NULL;
    }

    wm_drv_i2s_w800_nbi_pick(&rnbc->nbi, WM_DRV_I2S_NBC_RDEPTH);

    return node;
}

static int wm_drv_i2s_w800_alloc_dma_ch(char *dma_name, wm_drv_i2s_ctx_t *ctx, wm_hal_i2s_cfg_t *cfg)
{
    int ret;

    uint8_t *tx_ch, *rx_ch;

    if ((ctx->dma_dev = wm_dt_get_device_by_name(dma_name)) == NULL) {
        WM_DRV_I2S_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    if (cfg->dir == WM_I2S_DIR_INOUT || cfg->dir == WM_I2S_DIR_OUT) {
        tx_ch = &cfg->dma_ch[WM_HAL_I2S_TR_TX];
        ret   = wm_drv_dma_request_ch(ctx->dma_dev, tx_ch, WM_DRV_I2S_TIMEOUT);
        if (ret != WM_ERR_SUCCESS) {
            WM_DRV_I2S_LOG_MARKER;
            return WM_ERR_FAILED;
        }
    }

    if (cfg->dir == WM_I2S_DIR_INOUT || cfg->dir == WM_I2S_DIR_IN) {
        rx_ch = &cfg->dma_ch[WM_HAL_I2S_TR_RX];
        ret   = wm_drv_dma_request_ch(ctx->dma_dev, rx_ch, WM_DRV_I2S_TIMEOUT);
        if (ret != WM_ERR_SUCCESS) {
            WM_DRV_I2S_LOG_MARKER;
            goto fail_free_dma_ch_tx;
        }
    }

    return WM_ERR_SUCCESS;

fail_free_dma_ch_tx:
    wm_drv_dma_request_ch(ctx->dma_dev, tx_ch, WM_DRV_I2S_TIMEOUT);

    return WM_ERR_FAILED;
}

static int wm_drv_i2s_w800_ioctl_clock(wm_drv_i2s_ctx_t *ctx, wm_drv_i2s_ioctl_args_t *arg)
{
    ctx->priv_cfg.mclk_hz = arg->mclk_hz;
    return wm_drv_i2s_w800_set_clock(ctx, arg->extal_en);
}

static int wm_drv_i2s_w800_ioctl(wm_device_t *device, wm_drv_i2s_ioctl_args_t *arg)
{
    int ret = WM_ERR_FAILED;

    uint8_t sel = arg->tx ? 0 : 1;

    wm_drv_i2s_data_t *data = device->drv;
    wm_drv_i2s_ctx_t *ctx   = &data->ctx;

    wm_hal_i2s_dev_t *dev = &data->dev;

    switch (arg->cmd) {
        case WM_DRV_I2S_CMD_SET_MUTE:
        case WM_DRV_I2S_CMD_SET_BITSHIFT:
        case WM_DRV_I2S_CMD_SET_LZC:
        case WM_DRV_I2S_CMD_SET_RZC:
        case WM_DRV_I2S_CMD_SET_MONO_LEFT:
        {
            en_handls[arg->cmd][sel](dev, arg->en);
            ret = WM_ERR_SUCCESS;
            break;
        }
        case WM_DRV_I2S_CMD_SET_CLK_SRC:
        {
            ret = wm_drv_i2s_w800_ioctl_clock(ctx, arg);
            break;
        }
        default:
        {
            break;
        }
    }

    return ret;
}

static int wm_drv_i2s_w800_write_async(wm_device_t *device, void *buf, int len, i2s_free_buf_cb_t cb)
{
    int ret;
    int ready_cnt;

    wm_drv_i2s_data_t *data   = device->drv;
    wm_drv_i2s_cfg_t *cfg     = &data->ctx.cfg;
    wm_drv_i2s_tx_nbc_t *tnbc = &data->ctx.tnbc;

    wm_hal_i2s_dev_t *dev = &data->dev;

    if (data->ctx.write_pause) {
        return WM_ERR_NOT_ALLOWED;
    }

    if (!wm_drv_i2s_w800_check_trx_support(cfg, WM_I2S_DIR_OUT)) {
        WM_DRV_I2S_LOG_MARKER;
        return WM_ERR_NO_SUPPORT;
    }

    /* len align to 4 , and less than 2^16 */
    if (len <= 0 || (len & 3) || len > 0xffff) {
        WM_DRV_I2S_LOG_E("bad data len=%d", len);
        return WM_ERR_NOT_ALLOWED;
    }

    ret = wm_drv_i2s_w800_tnbc_push(data, cb, buf, len);
    if (ret != WM_ERR_SUCCESS) {
        return ret;
    }

    /* for power saving */
    wm_drv_i2s_w800_slient_clock(data, true);

    tnbc->running = true;

    /* XXX: if DMA is running no need & SHOULD NOT append buffer */
    if (!wm_hal_i2s_is_txdma_zombie(dev)) {
        return WM_ERR_SUCCESS;
    }

    /* one valid buffer exist at least */
    ready_cnt = wm_drv_i2s_w800_trnb_valid_count(&tnbc->nbi, WM_DRV_I2S_NBC_TDEPTH);
    if (ready_cnt > cfg->tx_ready_th) {
        do {
            ret = wm_drv_i2s_w800_try_tx_dma(data);
            if (WM_ERR_SUCCESS != ret) {
                break;
            }
        } while (1);

        if (WM_ERR_SUCCESS != wm_hal_i2s_tx_dma_start(dev)) {
            WM_DRV_I2S_ASSERT(0);
            return WM_ERR_FAILED;
        }
    }

    /* XXX: disable mute for tx master */
    if (cfg->mode == WM_I2S_MODE_MASTER) {
        wm_hal_i2s_set_tx_mute_en(dev, false);
    }

    return WM_ERR_SUCCESS;
}

static int wm_drv_i2s_w800_write_stop(wm_device_t *device)
{
    int ret                      = WM_ERR_SUCCESS;
    wm_drv_i2s_data_t *data      = device->drv;
    wm_drv_i2s_ctx_t *ctx        = &data->ctx;
    wm_drv_i2s_cfg_t *cfg        = &ctx->cfg;
    wm_drv_i2s_tx_nb_t *consumed = NULL;
    wm_hal_i2s_dev_t *dev        = &data->dev;

    if (data->ctx.tnbc.running == false) {
        return WM_ERR_SUCCESS;
    }

    if (cfg->xtype == WM_I2S_XFER_DMA) {
        ret = wm_hal_i2s_tx_dma_stop(dev);
    } else {
        /* TODO: interrupt */;
    }

    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_I2S_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    /* free buffer of user, should MAKE SURE HW has been stopped */
    for (; /* for each all the node */;) {
        consumed = wm_drv_i2s_w800_tnbc_pop(data);
        /* WM_DRV_I2S_LOG_MARKER; */
        if (consumed != NULL && consumed->data != NULL) {
#if WM_DRV_I2S_USE_WRITE_EXT_API
            if (consumed->free) {
                consumed->free(consumed->data);
            }
#else
            if (ctx->user_tx_cb) {
                ctx->user_tx_cb(consumed->data);
            }
#endif
        } else {
            break;
        }
    }

    wm_drv_i2s_w800_nbi_reset(&data->ctx.tnbc.nbi);

    /* TODO: XXX: mute for tx master */
    if (cfg->mode == WM_I2S_MODE_MASTER) {
        wm_hal_i2s_set_tx_mute_en(dev, true);
    }

    data->ctx.tnbc.running = false;
    ctx->write_pause       = false;

    /* for power saving */
    wm_drv_i2s_w800_slient_clock(data, false);

    return ret;
}

static int wm_drv_i2s_w800_write_pause(wm_device_t *device)
{
    int ret                 = WM_ERR_SUCCESS;
    wm_drv_i2s_data_t *data = device->drv;
    wm_drv_i2s_ctx_t *ctx   = &data->ctx;

    if (ctx->write_pause) {
        return WM_ERR_FAILED;
    }

    ctx->write_pause = true;

    return ret;
}

static int wm_drv_i2s_w800_write_resume(wm_device_t *device)
{
    int ret                 = WM_ERR_SUCCESS;
    wm_drv_i2s_data_t *data = device->drv;
    wm_drv_i2s_ctx_t *ctx   = &data->ctx;

    if (!ctx->write_pause) {
        return WM_ERR_FAILED;
    }

    ctx->write_pause = false;

    return ret;
}

static int wm_drv_i2s_w800_read_async(wm_device_t *device)
{
    int i;

    wm_drv_i2s_data_t *data = device->drv;

    wm_drv_i2s_ctx_t *ctx = &data->ctx;

    if (!wm_drv_i2s_w800_check_trx_support(&ctx->cfg, WM_I2S_DIR_IN)) {
        WM_DRV_I2S_LOG_MARKER;
        return WM_ERR_NO_SUPPORT;
    }

    if (ctx->rnbc.running || ctx->read_pause) {
        WM_DRV_I2S_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    /* for power saving */
    wm_drv_i2s_w800_slient_clock(data, true);

    ctx->rnbc.running = true;

    for (i = 0; i < WM_DRV_I2S_NBC_RDEPTH; i++) {
        if (WM_ERR_SUCCESS != wm_drv_i2s_w800_rnbc_push(data)) {
            break;
        }
    }

    for (i = 0; i < ctx->rx_desc_depth; i++) {
        if (WM_ERR_SUCCESS != wm_drv_i2s_w800_try_rx_dma(data)) {
            WM_DRV_I2S_LOG_E("Append buf to DMA for RX failed!");
            goto fail;
        }
    }

    if (WM_ERR_SUCCESS != wm_hal_i2s_rx_dma_start(&data->dev)) {
        WM_DRV_I2S_LOG_E("Start DMA for RX failed!");
        goto fail;
    }

    return WM_ERR_SUCCESS;

fail:
    /* free buffer of user, should MAKE SURE HW has been stopped */
    for (; /* for each all the node */;) {
        wm_drv_i2s_rxbuf_t *consumed = wm_drv_i2s_w800_rnbc_pop(data);
        if (consumed != NULL) {
            if (consumed->free) {
                consumed->free(consumed);
            }
        } else {
            break;
        }
    }

    data->ctx.rnbc.running = false;

    return WM_ERR_FAILED;
}

static int wm_drv_i2s_w800_read_stop(wm_device_t *device)
{
    int ret                      = WM_ERR_SUCCESS;
    wm_drv_i2s_data_t *data      = device->drv;
    wm_drv_i2s_cfg_t *cfg        = &data->ctx.cfg;
    wm_drv_i2s_rxbuf_t *consumed = NULL;
    wm_hal_i2s_dev_t *dev        = &data->dev;

    if (!data->ctx.rnbc.running) {
        return WM_ERR_SUCCESS;
    }

    if (cfg->xtype == WM_I2S_XFER_DMA) {
        ret = wm_hal_i2s_rx_dma_stop(dev);

    } else {
        /* TODO: interrupt */;
    }

    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_I2S_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    data->ctx.rnbc.running = false;
    data->ctx.read_pause   = false;

    /* free buffer of user, should MAKE SURE HW has been stopped */
    for (; /* for each all the node */;) {
        consumed = wm_drv_i2s_w800_rnbc_pop(data);
        if (consumed != NULL) {
            if (consumed->free) {
                consumed->free(consumed);
            }
        } else {
            break;
        }
    }

    wm_drv_i2s_w800_nbi_reset(&data->ctx.rnbc.nbi);

    data->ctx.rnbc.rx_buf_request = 0;
    data->ctx.rnbc.rx_buf_release = 0;
    data->ctx.rnbc.rx_buf_oncache = 0;

    /* for power saving */
    wm_drv_i2s_w800_slient_clock(data, false);

    return ret;
}

static int wm_drv_i2s_w800_read_pause(wm_device_t *device)
{
    int ret = WM_ERR_SUCCESS;

    wm_drv_i2s_data_t *data = device->drv;
    wm_drv_i2s_ctx_t *ctx   = &data->ctx;

    wm_hal_i2s_dev_t *dev = &data->dev;

    if (ctx->read_pause || !data->ctx.rnbc.running) {
        return WM_ERR_FAILED;
    }

    ctx->read_pause = true;

    wm_hal_i2s_set_rx_en(dev, false);

    return ret;
}

static int wm_drv_i2s_w800_read_resume(wm_device_t *device)
{
    int ret = WM_ERR_SUCCESS;

    wm_drv_i2s_data_t *data = device->drv;
    wm_drv_i2s_ctx_t *ctx   = &data->ctx;

    wm_hal_i2s_dev_t *dev = &data->dev;

    if (!ctx->read_pause || !ctx->rnbc.running) {
        return WM_ERR_FAILED;
    }

    ctx->read_pause = false;

    wm_hal_i2s_set_rx_en(dev, true);

    return ret;
}

static int wm_drv_i2s_w800_init(wm_device_t *device, wm_drv_i2s_cfg_t *cfg)
{
    int ret = WM_ERR_SUCCESS;

    wm_dt_hw_i2s_t *hw = device->hw;

    wm_drv_i2s_data_t *data = device->drv;
    wm_drv_i2s_ctx_t *ctx   = &data->ctx;

    wm_hal_i2s_dev_t *hal_i2s_dev = &data->dev;
    wm_hal_i2s_cfg_t hal_cfg      = { 0 };

    ctx->clock_dev = wm_dt_get_device_by_name(hw->rcc_device_name);
    if (ctx->clock_dev == NULL) {
        WM_DRV_I2S_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    if (hw->pin_cfg_count == 0 || hw->pin_cfg == NULL) {
        WM_DRV_I2S_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    if (cfg->rx_pkt_size == 0) {
        cfg->rx_pkt_size = WM_DRV_I2S_DESC_DEF_BUF_LEN;
    }

    if (cfg->rx_pkt_size < WM_DRV_I2S_DESC_MIN_BUF_LEN || cfg->rx_pkt_size > WM_DRV_I2S_DESC_MAX_BUF_LEN) {
        WM_DRV_I2S_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    if (cfg->rx_pkt_size & 3) {
        WM_DRV_I2S_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    ctx->tx_desc_depth = WM_DRV_I2S_DESC_TDEPTH;
    ctx->rx_desc_depth = WM_DRV_I2S_DESC_RDEPTH;

    hal_cfg.mode                              = cfg->mode;
    hal_cfg.dir                               = cfg->dir;
    hal_cfg.std                               = cfg->std;
    hal_cfg.fmt                               = cfg->fmt;
    hal_cfg.ctype                             = cfg->ctype;
    hal_cfg.xtype                             = cfg->xtype;
    hal_cfg.desc_node_depth[WM_HAL_I2S_TR_TX] = WM_DRV_I2S_DESC_TDEPTH;
    hal_cfg.desc_node_depth[WM_HAL_I2S_TR_RX] = WM_DRV_I2S_DESC_RDEPTH;
    hal_cfg.desc_rxbuf_size                   = cfg->rx_pkt_size;
    hal_cfg.desc_calloc                       = desc_calloc;
    hal_cfg.desc_free                         = desc_release;
    hal_cfg.cb[WM_HAL_I2S_TR_TX]              = wm_drv_i2s_w800_tx_done_cb;
    hal_cfg.cb[WM_HAL_I2S_TR_RX]              = wm_drv_i2s_w800_rx_ready_cb;

    if (cfg->xtype == WM_I2S_XFER_DMA) {
        ret = wm_drv_i2s_w800_alloc_dma_ch(hw->dma_device_name, ctx, &hal_cfg);

        wm_dma_data_t *dma_data = ctx->dma_dev->drv;
        hal_i2s_dev->dma_dev    = &dma_data->hal_dev;
    } else {
        WM_DRV_I2S_ASSERT(0);
    }

    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_I2S_LOG_MARKER;
        goto fail;
    }

    hal_i2s_dev->master_dev = device;
    hal_i2s_dev->irq_num    = hw->irq_cfg.irq_num;
    hal_i2s_dev->reg_dev    = (wm_i2s_reg_t *)hw->reg_base;

    ctx->hw = hw;
    for (int i = 0; i < hw->pin_cfg_count; i++) {
        ret = wm_drv_gpio_iomux_func_sel(hw->pin_cfg[i].pin_num, hw->pin_cfg[i].pin_mux);
        if (ret != WM_ERR_SUCCESS) {
            WM_DRV_I2S_LOG_MARKER;
            goto fail_hal_i2s_init;
        }
        WM_DRV_I2S_LOG_D("GPIO[%02d] configured!", hw->pin_cfg[i].pin_num);
    }

    if (WM_ERR_SUCCESS != wm_drv_i2s_w800_init_clock(data, hw)) {
        WM_DRV_I2S_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    wm_drv_i2s_w800_slient_clock(data, true);

    ret = wm_hal_i2s_init(hal_i2s_dev, &hal_cfg);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_I2S_LOG_MARKER;
        goto fail_i2s_gpio_init;
    }

    WM_DRV_I2S_LOG_I("Driver %s was successfully initialized.", device->name);

    return WM_ERR_SUCCESS;

fail_hal_i2s_init:
    wm_drv_i2s_w800_gpio_reset(data);

fail_i2s_gpio_init:
    wm_hal_i2s_deinit(hal_i2s_dev);

fail:
    wm_drv_clock_disable(ctx->clock_dev, WM_RCC_I2S_GATE_EN);

    return WM_ERR_FAILED;
}

static int wm_drv_i2s_w800_deinit(wm_device_t *device)
{
    uint8_t tx_ch, rx_ch;

    wm_drv_i2s_data_t *data = device->drv;
    wm_drv_i2s_ctx_t *ctx   = &data->ctx;
    wm_drv_i2s_cfg_t *cfg   = &ctx->cfg;

    wm_hal_i2s_dev_t *dev = &data->dev;

    wm_hal_i2s_cfg_t *hal_cfg = dev->cfg;

    wm_drv_i2s_w800_gpio_reset(data);

    if (cfg->dir == WM_I2S_DIR_INOUT || cfg->dir == WM_I2S_DIR_OUT) {
        wm_drv_i2s_w800_write_stop(device);
    }

    if (cfg->dir == WM_I2S_DIR_INOUT || cfg->dir == WM_I2S_DIR_IN) {
        wm_drv_i2s_w800_read_stop(device);
    }

    /* release DMA channel which is request during initialization */
    if (cfg->xtype == WM_I2S_XFER_DMA) {
        if (cfg->dir == WM_I2S_DIR_INOUT || cfg->dir == WM_I2S_DIR_OUT) {
            tx_ch = hal_cfg->dma_ch[WM_HAL_I2S_TR_TX];
            wm_drv_dma_release_ch(ctx->dma_dev, tx_ch, WM_DRV_I2S_TIMEOUT);
        }

        if (cfg->dir == WM_I2S_DIR_INOUT || cfg->dir == WM_I2S_DIR_IN) {
            rx_ch = hal_cfg->dma_ch[WM_HAL_I2S_TR_RX];
            wm_drv_dma_release_ch(ctx->dma_dev, rx_ch, WM_DRV_I2S_TIMEOUT);
        }
    }

    if (WM_ERR_SUCCESS != wm_hal_i2s_deinit(dev)) {
        return WM_ERR_FAILED;
    }

    return wm_drv_clock_disable(ctx->clock_dev, WM_RCC_I2S_GATE_EN);
}

const wm_drv_i2s_ops_t wm_drv_i2s_ops = { .init         = wm_drv_i2s_w800_init,
                                          .deinit       = wm_drv_i2s_w800_deinit,
                                          .write_async  = wm_drv_i2s_w800_write_async,
                                          .write_stop   = wm_drv_i2s_w800_write_stop,
                                          .write_pause  = wm_drv_i2s_w800_write_pause,
                                          .write_resume = wm_drv_i2s_w800_write_resume,
                                          .read_async   = wm_drv_i2s_w800_read_async,
                                          .read_stop    = wm_drv_i2s_w800_read_stop,
                                          .read_pause   = wm_drv_i2s_w800_read_pause,
                                          .read_resume  = wm_drv_i2s_w800_read_resume,
                                          .ioctl        = wm_drv_i2s_w800_ioctl };

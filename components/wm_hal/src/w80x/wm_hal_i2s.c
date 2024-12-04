/**
 * @file wm_hal_i2s.c
 *
 * @brief HAL_I2S Module
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
#include <stdbool.h>
#include "wm_error.h"
#include "wm_hal_dma.h"
#include "wm_hal_irq.h"
#include "wm_ll_i2s.h"
#include "wm_reg_op.h"
#include "wm_soc_cfgs.h"
#include "wm_hal_i2s.h"

#define WM_HAL_I2S_DEBUG 1

#if WM_HAL_I2S_DEBUG
#include "wm_log.h"
#undef LOG_TAG
#define LOG_TAG               "I2S HAL"
#define WM_HAL_I2S_LOG_D(...) wm_log_debug(__VA_ARGS__)
#define WM_HAL_I2S_LOG_I(...) wm_log_info(__VA_ARGS__)
#define WM_HAL_I2S_LOG_E(...) wm_log_error(__VA_ARGS__)
#define WM_HAL_I2S_LOG_MARKER WM_HAL_I2S_LOG_E("%s %d", __func__, __LINE__)
#else
#define WM_HAL_I2S_LOG_D(...) \
    do {                      \
    } while (0)
#define WM_HAL_I2S_LOG_I(...) \
    do {                      \
    } while (0)
#define WM_HAL_I2S_LOG_E(...) \
    do {                      \
    } while (0)
#define WM_HAL_I2S_LOG_MARKER

#endif

#define WM_HAL_I2S_ASSERT(v)    assert(v)

#define WM_HAL_I2S_INIT_CHECK() (dev == NULL || dev->cfg == NULL)

/* XXX:
 * Because only DMA is currently used and its byte mode is employed
 * for data transfer,the I2S FIFO threshold can be set to 0 to enhance
 * the hardware data transfer rate and simplify the software processing.
 *
 * Of course, if using interrupt mode, it is necessary to consider changing
 * this value to greater than 0 to reduce the frequency of system interrupts.
 * */
#define WM_HAL_I2S_TXTH         4 /**< TXFIFO threshold */
#define WM_HAL_I2S_RXTH         4 /**< RXFIFO threshold */

typedef void (*dma_isr_handler_t)(wm_dma_ch_t ch, uint32_t sts, void *user_data);
typedef void (*set_en_t)(wm_hal_i2s_dev_t *dev, bool en);

static void wm_hal_i2s_set_tx_zombie(wm_hal_i2s_dev_t *dev, bool en);
static void wm_hal_i2s_set_rx_zombie(wm_hal_i2s_dev_t *dev, bool en);
static void wm_hal_i2s_dma_tx_isr_cb(wm_dma_ch_t ch, uint32_t sts, void *user_data);
static void wm_hal_i2s_dma_rx_isr_cb(wm_dma_ch_t ch, uint32_t sts, void *user_data);
static int wm_hal_i2s_rx_done_irq(wm_hal_i2s_dev_t *dev);

static const set_en_t set_zombie[WM_HAL_I2S_TR_MAX] = {
    [WM_HAL_I2S_TR_TX] = wm_hal_i2s_set_tx_zombie,
    [WM_HAL_I2S_TR_RX] = wm_hal_i2s_set_rx_zombie,
};

static const set_en_t set_trx_en[WM_HAL_I2S_TR_MAX] = {
    [WM_HAL_I2S_TR_TX] = wm_hal_i2s_set_tx_en,
    [WM_HAL_I2S_TR_RX] = wm_hal_i2s_set_rx_en,
};

static const dma_isr_handler_t dma_isr_cbs[WM_HAL_I2S_TR_MAX] = {
    wm_hal_i2s_dma_tx_isr_cb,
    wm_hal_i2s_dma_rx_isr_cb,
};

static void wm_hal_i2s_dump_regs(wm_hal_i2s_dev_t *dev)
{
    uint8_t cnt       = 0;
    wm_i2s_reg_t *reg = dev->reg_dev;

    /* To prevent the compiler from issuing warnings. */
    (void)cnt;
    (void)reg;

    WM_HAL_I2S_LOG_I("I2S REG CTRL:0x%08x", reg->control.val);
    WM_HAL_I2S_LOG_I("I2S REG IMSK:0x%08x", reg->imask.val);
    WM_HAL_I2S_LOG_I("I2S REG FLAG:0x%08x", reg->int_flag.val);
    WM_HAL_I2S_LOG_I("I2S REG STAT:0x%08x", reg->status.val);

    cnt = wm_ll_i2s_get_txfifo_count(dev->reg_dev);
    WM_HAL_I2S_LOG_I("I2S REG TXFIFO(word):%d", cnt);
    cnt = wm_ll_i2s_get_rxfifo_count(dev->reg_dev);
    WM_HAL_I2S_LOG_I("I2S REG RXFIFO(word):%d", cnt);
    cnt = wm_ll_i2s_get_valid_bytes_in_last_word(dev->reg_dev);
    WM_HAL_I2S_LOG_I("I2S REG RXFIFO last valid cnt(bytes):%d", cnt);
}

static void wm_hal_i2s_dump_dma_desc(wm_hal_dma_desc_t *desc, int index, const char *name)
{
    WM_HAL_I2S_LOG_I("%s[%2d:%p->%p] vld:0x%08x, chain:0x%08x,"
                     "len:%d, src: 0x%08x, dst: 0x%08x",
                     name, index, desc, desc->next, desc->vld, desc->ctrl.chain.val, desc->ctrl.chain.len, desc->src_addr,
                     desc->dest_addr);
}

static int wm_hal_i2s_reset(wm_hal_i2s_dev_t *dev)
{
    enum wm_i2s_dir dir = dev->cfg->dir;

    wm_ll_i2s_reset_ctrl(dev->reg_dev);

    /* disable I2S module first */
    wm_ll_i2s_set_en(dev->reg_dev, false);
    wm_ll_i2s_set_rx_en(dev->reg_dev, false);
    wm_ll_i2s_set_tx_en(dev->reg_dev, false);
    wm_ll_i2s_set_mute_en(dev->reg_dev, false);
    wm_ll_i2s_set_data_fmt(dev->reg_dev, WM_I2S_FMT_8BIT);
    wm_ll_i2s_set_tx_clock_phase(dev->reg_dev, false);
    wm_ll_i2s_set_rx_clock_phase(dev->reg_dev, false);
    /* set to 4 for FIFO threshold by default */
    wm_ll_i2s_set_txfifo_threshold(dev->reg_dev, WM_HAL_I2S_TXTH);
    wm_ll_i2s_set_rxfifo_threshold(dev->reg_dev, WM_HAL_I2S_RXTH);
    /* disable right channel zero cross check */
    wm_ll_i2s_rzc_set_en(dev->reg_dev, false);
    /* disable left channel zero cross check */
    wm_ll_i2s_lzc_set_en(dev->reg_dev, false);
    wm_ll_i2s_clear_tx_fifo(dev->reg_dev);
    wm_ll_i2s_clear_rx_fifo(dev->reg_dev);

    if (dir == WM_I2S_DIR_OUT || dir == WM_I2S_DIR_INOUT) {
        wm_ll_i2s_set_tx_dma_en(dev->reg_dev, false);
    }
    if (dir == WM_I2S_DIR_IN || dir == WM_I2S_DIR_INOUT) {
        wm_ll_i2s_set_rx_dma_en(dev->reg_dev, false);
    }
    /* use stero by default */
    wm_ll_i2s_set_chan_type(dev->reg_dev, WM_I2S_CHAN_TYPE_STEREO);
    wm_ll_i2s_set_mono_left(dev->reg_dev, true);
    wm_ll_i2s_set_data_std(dev->reg_dev, WM_I2S_STD_I2S);
    /* TODO XXX need check, it have no effect when test  */
    /* wm_ll_i2s_set_rx_timeout_en(dev->reg_dev, true); */
    wm_ll_i2s_set_duplex_en(dev->reg_dev, false);
    /* set to master by default */
    wm_ll_i2s_set_mode(dev->reg_dev, WM_I2S_MODE_MASTER);

    /* disable all interrupt source */
    wm_ll_i2s_disable_all_irq(dev->reg_dev);
    /* clear all interrupt flag */
    wm_ll_i2s_clear_all_irq_flag(dev->reg_dev);

    wm_ll_i2s_clear_rx_fifo(dev->reg_dev);
    wm_ll_i2s_clear_tx_fifo(dev->reg_dev);
    /*
    The last package maybe less than RX DMA size, use rxdone irq to get the last package.
    */
    wm_ll_i2s_enable_irq(dev->reg_dev, WM_I2S_RXDONE_IRQ_MASK);

    return WM_ERR_SUCCESS;
}

static int wm_hal_i2s_cfg_init(wm_hal_i2s_dev_t *dev)
{
    wm_hal_i2s_cfg_t *cfg = dev->cfg;

    if (WM_ERR_SUCCESS != wm_hal_i2s_reset(dev)) {
        WM_HAL_I2S_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    wm_ll_i2s_set_mode(dev->reg_dev, cfg->mode);
    if (cfg->dir == WM_I2S_DIR_INOUT) {
        wm_ll_i2s_set_duplex_en(dev->reg_dev, true);
    }

    wm_ll_i2s_set_data_std(dev->reg_dev, cfg->std);
    wm_ll_i2s_set_data_fmt(dev->reg_dev, cfg->fmt);
    wm_ll_i2s_set_chan_type(dev->reg_dev, cfg->ctype);
    wm_ll_i2s_set_mono_left(dev->reg_dev, false);
    wm_ll_i2s_lzc_set_en(dev->reg_dev, false);
    wm_ll_i2s_rzc_set_en(dev->reg_dev, false);

    wm_ll_i2s_set_txfifo_threshold(dev->reg_dev, WM_HAL_I2S_TXTH);
    wm_ll_i2s_set_rxfifo_threshold(dev->reg_dev, WM_HAL_I2S_RXTH);

    if (cfg->xtype == WM_I2S_XFER_DMA) {
        if (cfg->dir == WM_I2S_DIR_OUT || cfg->dir == WM_I2S_DIR_INOUT) {
            wm_ll_i2s_set_tx_dma_en(dev->reg_dev, true);
        }
        if (cfg->dir == WM_I2S_DIR_IN || cfg->dir == WM_I2S_DIR_INOUT) {
            wm_ll_i2s_set_rx_dma_en(dev->reg_dev, true);
        }
    }

    return WM_ERR_SUCCESS;
}

static bool wm_hal_i2s_check_desc_valid(wm_hal_dma_desc_t *desc)
{
    return !!(desc->vld & WM_BIT(WM_DMA_LIST_VLD));
}

/* SHOULD and CAN only be called when DMA is stopped */
static void wm_hal_i2s_set_desc_invalid(wm_hal_dma_desc_t *desc)
{
    desc->vld &= ~WM_BIT(WM_DMA_LIST_VLD);
}

static void wm_hal_i2s_set_desc_valid(wm_hal_dma_desc_t *desc)
{
    desc->vld |= WM_BIT(WM_DMA_LIST_VLD);
}

static void wm_hal_i2s_isr_handler(wm_irq_no_t irq, void *arg)
{
    wm_hal_i2s_dev_t *dev = (wm_hal_i2s_dev_t *)arg;
    bool isr;

    /*get rxdone irq*/
    isr = wm_ll_i2s_get_irq_flag(dev->reg_dev);

    /*clear irq flag*/
    wm_ll_i2s_clear_all_irq_flag(dev->reg_dev);

    if ((isr | WM_I2S_RXDONE_FLAG)) {
        /*RX done*/

        /* in case that user stop by calling read_stop */
        if (wm_ll_i2s_is_rx_enabled(dev->reg_dev)) {
            wm_hal_i2s_rx_done_irq(dev);
        }
    }
}

static void wm_hal_i2s_dma_isr_cb(wm_dma_ch_t ch, uint32_t sts, void *user_data, wm_hal_i2s_tr_type_t type)
{
    int i              = 0;
    int nodes_consumed = 0;

    uint8_t desc_cid, desc_depth;

    uint32_t addr = 0;
    uint32_t *taddr[WM_HAL_I2S_TR_MAX];

    wm_hal_i2s_trx_param_t param;
    wm_hal_i2s_dev_t *dev = (wm_hal_i2s_dev_t *)user_data;
    wm_hal_i2s_cfg_t *cfg = dev->cfg;
    wm_hal_dma_desc_t *desc;

    desc       = dev->priv.desc[type];
    desc_depth = cfg->desc_node_depth[type];

    /* XXX:
     * In actual operation, DMA can only be started after initialization,
     * and resources will only be released after DMA is stopped,
     * hence there is no need to check (txdesc == NULL || cfg == NULL) here
     * to reduce the time overhead in the ISR function.
     */

    for (i = 0; i < desc_depth /* try check all nodes */; i++) {
        if (dev->priv.desc_cnt[type] == 0) {
            set_zombie[type](dev, true);
            break;
        }

        desc_cid = dev->priv.desc_cid[type];

        taddr[WM_HAL_I2S_TR_TX] = &desc[desc_cid].src_addr;
        taddr[WM_HAL_I2S_TR_RX] = &desc[desc_cid].dest_addr;

        /* has not been handled done by dma */
        if (wm_hal_i2s_check_desc_valid(&desc[desc_cid])) {
            break;
        }

        if (addr == 0) {
            addr = *taddr[type];
        }

        *taddr[type] = 0;

        dev->priv.desc_cnt[type]--;
        dev->priv.desc_cid[type]++;
        dev->priv.desc_cid[type] %= desc_depth;
        dev->priv.xfer_cnt[type]++;

        nodes_consumed++;
    }

    param.start_buf      = (void *)addr;
    param.len            = cfg->desc_rxbuf_size;
    param.nodes_consumed = nodes_consumed;
    param.slots_remained = desc_depth - dev->priv.desc_cnt[type];

    cfg->cb[type](dev->master_dev, &param);
}

static void wm_hal_i2s_dma_rx_isr_cb(wm_dma_ch_t ch, uint32_t sts, void *user_data)
{
    wm_hal_i2s_dma_isr_cb(ch, sts, user_data, WM_HAL_I2S_TR_RX);
}

static void wm_hal_i2s_dma_tx_isr_cb(wm_dma_ch_t ch, uint32_t sts, void *user_data)
{
    wm_hal_i2s_dma_isr_cb(ch, sts, user_data, WM_HAL_I2S_TR_TX);
}

static int wm_hal_i2s_dma_desc_list_set(wm_hal_i2s_dev_t *dev, wm_hal_dma_desc_t *desc, wm_hal_i2s_tr_type_t type)
{
    wm_hal_i2s_cfg_t *cfg = dev->cfg;

    desc->vld                   = 0;
    desc->ctrl.chain.len        = 0;
    desc->ctrl.chain.data_unit  = WM_DMA_XFER_UNIT_WORD;
    desc->ctrl.chain.burst_size = WM_DMA_BURST_1UNIT;
    desc->extend_ctrl.ch        = cfg->dma_ch[type];

    if (type == WM_HAL_I2S_TR_TX) {
        desc->dest_addr                = wm_ll_i2s_get_tx_fifo_addr(dev->reg_dev);
        desc->extend_ctrl.req_sel      = WM_DMA_I2S_TX_REQ;
        desc->ctrl.chain.src_addr_inc  = WM_DMA_ADDR_INC;
        desc->ctrl.chain.dest_addr_inc = WM_DMA_ADDR_FIXED;
    } else if (type == WM_HAL_I2S_TR_RX) {
        desc->src_addr                 = wm_ll_i2s_get_rx_fifo_addr(dev->reg_dev);
        desc->extend_ctrl.req_sel      = WM_DMA_I2S_RX_REQ;
        desc->ctrl.chain.dest_addr_inc = WM_DMA_ADDR_INC;
        desc->ctrl.chain.src_addr_inc  = WM_DMA_ADDR_FIXED;
    } else {
        WM_HAL_I2S_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    desc->extend_ctrl.int_en   = WM_DMA_CH_EN_CTRL_ENABLE;
    desc->extend_ctrl.int_type = WM_DMA_INT_XFER_DONE;
    desc->extend_ctrl.mode     = WM_DMA_HW_MODE;

    desc->extend_ctrl.chain_mode    = WM_DMA_CHAIN_MODE_LIST;
    desc->extend_ctrl.warp_mode_en  = WM_DMA_WARP_CTRL_DISABLE;
    desc->extend_ctrl.chain_mode_en = WM_DMA_CHAIN_MODE_ENABLE;

    return WM_ERR_SUCCESS;
}

static int wm_hal_i2s_desc_dir_scope(wm_hal_i2s_cfg_t *cfg, uint8_t *start, uint8_t *end)
{
    if (cfg->dir == WM_I2S_DIR_INOUT) {
        *start = WM_HAL_I2S_TR_TX;
        *end   = WM_HAL_I2S_TR_MAX;
    } else if (cfg->dir == WM_I2S_DIR_IN) {
        *start = WM_HAL_I2S_TR_RX;
        *end   = WM_HAL_I2S_TR_MAX;
    } else if (cfg->dir == WM_I2S_DIR_OUT) {
        *start = WM_HAL_I2S_TR_TX;
        *end   = WM_HAL_I2S_TR_RX;
    } else {
        return WM_ERR_FAILED;
    }

    return WM_ERR_SUCCESS;
}

static int wm_hal_i2s_dma_desc_init(wm_hal_i2s_dev_t *dev)
{
    int i = 0, j = 0;
    int ret = WM_ERR_SUCCESS, depth;

    uint8_t start, end;

    wm_hal_i2s_cfg_t *cfg    = dev->cfg;
    wm_hal_i2s_priv_t *priv  = &dev->priv;
    wm_hal_dma_desc_t **desc = { NULL };

    if (cfg->xtype != WM_I2S_XFER_DMA) {
        WM_HAL_I2S_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    desc = priv->desc;

    if (WM_ERR_SUCCESS != wm_hal_i2s_desc_dir_scope(cfg, &start, &end)) {
        WM_HAL_I2S_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    for (i = start; i < end; i++) {
        depth   = cfg->desc_node_depth[i];
        desc[i] = (wm_hal_dma_desc_t *)cfg->desc_calloc(depth, sizeof(*desc[i]));
        if (desc[i] == NULL) {
            WM_HAL_I2S_LOG_MARKER;
            goto fail_desc;
        }
    }

    for (i = start; i < end; i++) {
        for (j = 0; j < cfg->desc_node_depth[i] - 1; j++) {
            desc[i][j].next = &desc[i][j + 1];
            ret             = wm_hal_i2s_dma_desc_list_set(dev, &desc[i][j], i);
            if (WM_ERR_SUCCESS != ret) {
                WM_HAL_I2S_LOG_MARKER;
                goto fail_desc;
            }
        }

        desc[i][j].next = &desc[i][0];
        ret             = wm_hal_i2s_dma_desc_list_set(dev, &desc[i][j], i);
        if (WM_ERR_SUCCESS != ret) {
            WM_HAL_I2S_LOG_MARKER;
            goto fail_desc;
        }

        wm_hal_dma_register_callback(dev->dma_dev, cfg->dma_ch[i], dma_isr_cbs[i], dev);
    }

    return WM_ERR_SUCCESS;

fail_desc:
    for (i = start; i < end; i++) {
        if (desc[i]) {
            cfg->desc_free(desc[i]);
        }
    }

    return WM_ERR_FAILED;
}

static int wm_hal_i2s_dma_desc_deinit(wm_hal_i2s_dev_t *dev, wm_hal_i2s_tr_type_t type)
{
    wm_hal_i2s_cfg_t *cfg   = dev->cfg;
    wm_hal_i2s_priv_t *priv = &dev->priv;

    wm_hal_dma_clr_req(dev->dma_dev, WM_DMA_I2S_RX_REQ);
    wm_hal_dma_clr_req(dev->dma_dev, WM_DMA_I2S_TX_REQ);

    wm_hal_dma_unregister_callback(dev->dma_dev, cfg->dma_ch[type]);

    if (priv->desc[type]) {
        cfg->desc_free(priv->desc[type]);
    }

    return WM_ERR_SUCCESS;
}

static int wm_hal_i2s_dma_deinit(wm_hal_i2s_dev_t *dev)
{
    int i;

    uint8_t start, end;

    wm_hal_i2s_cfg_t *cfg = dev->cfg;

    if (cfg->xtype != WM_I2S_XFER_DMA) {
        return WM_ERR_SUCCESS;
    }

    wm_hal_i2s_tx_dma_stop(dev);
    wm_hal_i2s_rx_dma_stop(dev);

    if (WM_ERR_SUCCESS != wm_hal_i2s_desc_dir_scope(cfg, &start, &end)) {
        WM_HAL_I2S_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    for (i = start; i < end; i++) {
        if (WM_ERR_SUCCESS != wm_hal_i2s_dma_desc_deinit(dev, i)) {
            return WM_ERR_FAILED;
        }
    }

    return WM_ERR_SUCCESS;
}

static int wm_hal_i2s_dma_buf_append(wm_hal_i2s_dev_t *dev, void *buf, int len, wm_hal_i2s_tr_type_t type)
{
    uint32_t *taddr[WM_HAL_I2S_TR_MAX];
    uint32_t update_node_idx, node_depth;

    wm_hal_i2s_cfg_t *cfg = NULL;
    cfg                   = dev->cfg;

    if (WM_HAL_I2S_INIT_CHECK()) {
        WM_HAL_I2S_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    if (dev->dma_dev == NULL) {
        WM_HAL_I2S_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    wm_hal_dma_desc_t *desc = dev->priv.desc[type];
    if (desc == NULL) {
        WM_HAL_I2S_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    node_depth = cfg->desc_node_depth[type];
    if (dev->priv.desc_cnt[type] >= node_depth) {
        WM_HAL_I2S_LOG_MARKER;
        return WM_ERR_NO_MEM;
    }

    update_node_idx = (dev->priv.desc_cid[type] + dev->priv.desc_cnt[type]) % node_depth;

    taddr[WM_HAL_I2S_TR_TX] = &desc[update_node_idx].src_addr;
    taddr[WM_HAL_I2S_TR_RX] = &desc[update_node_idx].dest_addr;

    *taddr[type] = (uint32_t)buf;

    desc[update_node_idx].ctrl.chain.len = len;
    wm_hal_i2s_set_desc_valid(&desc[update_node_idx]);

    dev->priv.desc_cnt[type]++;
    dev->priv.desc_push_cnt[type]++;

    return WM_ERR_SUCCESS;
}

static int wm_hal_i2s_dma_start(wm_hal_i2s_dev_t *dev, wm_hal_i2s_tr_type_t type)
{
    wm_hal_dma_desc_t *curr_desc = NULL;
    wm_hal_dma_desc_t *txdesc    = dev->priv.desc[type];

    /* start from current desc */
    curr_desc = &txdesc[dev->priv.desc_cid[type]];
    /* wm_hal_i2s_dump_dma_desc(curr_desc, 0, type == WM_HAL_I2S_TR_TX ? "T" : "R"); */
    if (!wm_hal_i2s_check_desc_valid(curr_desc)) {
        return WM_ERR_FAILED;
    }

    set_zombie[type](dev, false);

    if (WM_ERR_SUCCESS != wm_hal_dma_config(dev->dma_dev, curr_desc)) {
        WM_HAL_I2S_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    dev->priv.desc_start_cnt[type]++;

    wm_hal_dma_start(dev->dma_dev, dev->cfg->dma_ch[type]);

    set_trx_en[type](dev, true);

    wm_ll_i2s_set_en(dev->reg_dev, true);

    return WM_ERR_SUCCESS;
}

static int wm_hal_i2s_dma_stop(wm_hal_i2s_dev_t *dev, wm_hal_i2s_tr_type_t type)
{
    uint8_t desc_depth;

    wm_hal_dma_desc_t *desc;
    wm_hal_i2s_cfg_t *cfg = dev->cfg;

    desc       = dev->priv.desc[type];
    desc_depth = cfg->desc_node_depth[type];

    wm_hal_dma_stop(dev->dma_dev, dev->cfg->dma_ch[type]);

    for (int i = 0; i < desc_depth; i++) {
        if (dev->priv.desc_cnt[type] == 0) {
            break;
        }

        desc[i].vld = 0;
        if (type == WM_HAL_I2S_TR_RX) {
            desc[i].dest_addr = 0;
        } else {
            desc[i].src_addr = 0;
        }
    }

    dev->priv.desc_cnt[type] = 0;
    dev->priv.desc_cid[type] = 0;

    WM_HAL_I2S_LOG_D("Stop %s DMA", type == WM_HAL_I2S_TR_TX ? "tx" : "rx");

    return WM_ERR_SUCCESS;
}

static void wm_hal_i2s_set_tx_zombie(wm_hal_i2s_dev_t *dev, bool en)
{
    /* There is no need to stop the HW (DMA, I2S, etc.) here;
     * this function only changes the state of the SW part.
     * The state of the HW is determined by the HW itself or
     * by a separate STOP interface.
     * */
    dev->priv.desc_en[WM_HAL_I2S_TR_TX] = !en;
}

static void wm_hal_i2s_set_rx_zombie(wm_hal_i2s_dev_t *dev, bool en)
{
    /* There is no need to stop the HW (DMA, I2S, etc.) here;
     * this function only changes the state of the SW part.
     * The state of the HW is determined by the HW itself or
     * by a separate STOP interface.
     * */
    dev->priv.desc_en[WM_HAL_I2S_TR_RX] = !en;
}

static int wm_hal_i2s_check_cfg(wm_hal_i2s_cfg_t *cfg)
{
    if (cfg->xtype == WM_I2S_XFER_DMA) {
        if (cfg->desc_calloc == NULL) {
            WM_HAL_I2S_LOG_MARKER;
            return WM_ERR_INVALID_PARAM;
        }
        if (cfg->desc_free == NULL) {
            WM_HAL_I2S_LOG_MARKER;
            return WM_ERR_INVALID_PARAM;
        }
    }

    if (cfg->dir == WM_I2S_DIR_IN || cfg->dir == WM_I2S_DIR_INOUT) {
        if (cfg->cb[WM_HAL_I2S_TR_RX] == NULL) {
            WM_HAL_I2S_LOG_MARKER;
            return WM_ERR_FAILED;
        }
    } else if (cfg->dir == WM_I2S_DIR_OUT || cfg->dir == WM_I2S_DIR_INOUT) {
        if (cfg->cb[WM_HAL_I2S_TR_TX] == NULL) {
            WM_HAL_I2S_LOG_MARKER;
            return WM_ERR_FAILED;
        }
    } else {
        WM_HAL_I2S_ASSERT(0);
    }

    return WM_ERR_SUCCESS;
}

void wm_hal_i2s_set_tx_clock_inv_en(wm_hal_i2s_dev_t *dev, bool en)
{
    wm_ll_i2s_set_tx_clock_phase(dev->reg_dev, en);
}

void wm_hal_i2s_set_rx_clock_inv_en(wm_hal_i2s_dev_t *dev, bool en)
{
    wm_ll_i2s_set_rx_clock_phase(dev->reg_dev, en);
}

void wm_hal_i2s_set_tx_mute_en(wm_hal_i2s_dev_t *dev, bool mute)
{
    wm_ll_i2s_set_mute_en(dev->reg_dev, mute);
}

void wm_hal_i2s_set_left_zero_cross_en(wm_hal_i2s_dev_t *dev, bool en)
{
    wm_ll_i2s_lzc_set_en(dev->reg_dev, en);
}

void wm_hal_i2s_set_right_zero_cross_en(wm_hal_i2s_dev_t *dev, bool en)
{
    wm_ll_i2s_rzc_set_en(dev->reg_dev, en);
}

void wm_hal_i2s_set_mono_left_en(wm_hal_i2s_dev_t *dev, bool en)
{
    wm_ll_i2s_set_mono_left(dev->reg_dev, en);
}

bool wm_hal_i2s_is_txdma_zombie(wm_hal_i2s_dev_t *dev)
{
    wm_dma_sts_info_t sts;
    wm_hal_i2s_cfg_t *cfg = dev->cfg;

    if (WM_HAL_I2S_INIT_CHECK()) {
        WM_HAL_I2S_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    if (!dev->priv.desc_en[WM_HAL_I2S_TR_TX]) {
        return true;
    }

    sts.sts = WM_DMA_RUNNIG;

    wm_hal_dma_get_status(dev->dma_dev, cfg->dma_ch[WM_HAL_I2S_TR_TX], &sts);

    return !(sts.sts == WM_DMA_RUNNIG);
}

bool wm_hal_i2s_is_rxdma_zombie(wm_hal_i2s_dev_t *dev)
{
    wm_dma_sts_info_t sts;
    wm_hal_i2s_cfg_t *cfg = dev->cfg;

    if (WM_HAL_I2S_INIT_CHECK()) {
        return true;
    }

    if (!dev->priv.desc_en[WM_HAL_I2S_TR_RX]) {
        return true;
    }

    sts.sts = WM_DMA_RUNNIG;

    wm_hal_dma_get_status(dev->dma_dev, cfg->dma_ch[WM_HAL_I2S_TR_RX], &sts);

    return !(sts.sts == WM_DMA_RUNNIG);
}

void wm_hal_i2s_set_tx_en(wm_hal_i2s_dev_t *dev, bool en)
{
    wm_ll_i2s_set_tx_en(dev->reg_dev, en);
}

void wm_hal_i2s_set_rx_en(wm_hal_i2s_dev_t *dev, bool en)
{
    wm_ll_i2s_set_rx_en(dev->reg_dev, en);
}

int wm_hal_i2s_rx_done_irq(wm_hal_i2s_dev_t *dev)
{
    int ret;
    int desc_depth;

    uint8_t desc_cid;
    uint32_t addr;

    wm_dma_sts_info_t sts = { WM_DMA_RUNNIG, 0 };

    wm_hal_i2s_cfg_t *cfg = dev->cfg;
    wm_hal_i2s_trx_param_t param;
    wm_hal_dma_desc_t *desc = dev->priv.desc[WM_HAL_I2S_TR_RX];

    desc_cid   = dev->priv.desc_cid[WM_HAL_I2S_TR_RX];
    desc_depth = cfg->desc_node_depth[WM_HAL_I2S_TR_RX];

    dev->priv.rx_flush_total++;

    /* dma will restart in drv rx_ready callback */
    wm_hal_dma_stop(dev->dma_dev, cfg->dma_ch[WM_HAL_I2S_TR_RX]);
    set_zombie[WM_HAL_I2S_TR_RX](dev, true);
    wm_hal_dma_get_status(dev->dma_dev, cfg->dma_ch[WM_HAL_I2S_TR_RX], &sts);

    if (sts.xfer_cnt == 0) {
        wm_hal_dma_start(dev->dma_dev, cfg->dma_ch[WM_HAL_I2S_TR_RX]);
        set_zombie[WM_HAL_I2S_TR_RX](dev, false);
        return WM_ERR_FAILED;
    }

    int pack_size     = desc->ctrl.chain.len;
    int left_word     = wm_ll_i2s_get_rxfifo_count(dev->reg_dev);
    uint32_t *rx_addr = (uint32_t *)wm_ll_i2s_get_rx_fifo_addr(dev->reg_dev);
    uint32_t *pbuf    = (uint32_t *)desc[desc_cid].dest_addr;

    while (left_word-- > 0) {
        if (sts.xfer_cnt < pack_size) {
            pbuf[sts.xfer_cnt / 4] = *rx_addr;
            sts.xfer_cnt += 4;
        } else {
            break;
        }
    }

    dev->priv.desc_cnt[WM_HAL_I2S_TR_RX]--;
    dev->priv.desc_cid[WM_HAL_I2S_TR_RX]++;
    dev->priv.desc_cid[WM_HAL_I2S_TR_RX] %= desc_depth;
    dev->priv.xfer_cnt[WM_HAL_I2S_TR_RX]++;

    addr = (uint32_t)pbuf;

    desc[desc_cid].dest_addr = 0;
    wm_hal_i2s_set_desc_invalid(&desc[desc_cid]);

    param.start_buf      = (void *)addr;
    param.len            = sts.xfer_cnt;
    param.nodes_consumed = 1;
    param.slots_remained = desc_depth - dev->priv.desc_cnt[WM_HAL_I2S_TR_RX];

    cfg->cb[WM_HAL_I2S_TR_RX](dev->master_dev, &param);

    ret = WM_ERR_SUCCESS;

    return ret;
}

int wm_hal_i2s_tx_dma_start(wm_hal_i2s_dev_t *dev)
{
    wm_hal_i2s_cfg_t *cfg = dev->cfg;

    if (WM_HAL_I2S_INIT_CHECK()) {
        WM_HAL_I2S_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    if (dev->dma_dev == NULL) {
        WM_HAL_I2S_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    if (cfg->dir == WM_I2S_DIR_IN) {
        WM_HAL_I2S_LOG_MARKER;
        return WM_ERR_NO_SUPPORT;
    }

    return wm_hal_i2s_dma_start(dev, WM_HAL_I2S_TR_TX);
}

int wm_hal_i2s_rx_dma_start(wm_hal_i2s_dev_t *dev)
{
    wm_hal_i2s_cfg_t *cfg = dev->cfg;

    if (WM_HAL_I2S_INIT_CHECK()) {
        WM_HAL_I2S_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    if (dev->dma_dev == NULL) {
        WM_HAL_I2S_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    if (cfg->dir == WM_I2S_DIR_OUT) {
        return WM_ERR_NO_SUPPORT;
    }

    return wm_hal_i2s_dma_start(dev, WM_HAL_I2S_TR_RX);
}

int wm_hal_i2s_tx_dma_stop(wm_hal_i2s_dev_t *dev)
{
    wm_hal_i2s_cfg_t *cfg = dev->cfg;

    if (WM_HAL_I2S_INIT_CHECK()) {
        WM_HAL_I2S_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    if (dev->dma_dev == NULL) {
        WM_HAL_I2S_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    if (cfg->dir == WM_I2S_DIR_IN) {
        return WM_ERR_NO_SUPPORT;
    }

    if (WM_ERR_SUCCESS != wm_hal_i2s_dma_stop(dev, WM_HAL_I2S_TR_TX)) {
        WM_HAL_I2S_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    set_trx_en[WM_HAL_I2S_TR_TX](dev, false);

    set_zombie[WM_HAL_I2S_TR_RX](dev, true);

    wm_ll_i2s_clear_tx_fifo(dev->reg_dev);

    return WM_ERR_SUCCESS;
}

int wm_hal_i2s_rx_dma_stop(wm_hal_i2s_dev_t *dev)
{
    wm_hal_i2s_cfg_t *cfg = dev->cfg;

    if (WM_HAL_I2S_INIT_CHECK()) {
        WM_HAL_I2S_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    if (dev->dma_dev == NULL) {
        WM_HAL_I2S_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    if (cfg->dir == WM_I2S_DIR_OUT) {
        return WM_ERR_NO_SUPPORT;
    }

    set_trx_en[WM_HAL_I2S_TR_RX](dev, false);

    if (WM_ERR_SUCCESS != wm_hal_i2s_dma_stop(dev, WM_HAL_I2S_TR_RX)) {
        WM_HAL_I2S_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    set_zombie[WM_HAL_I2S_TR_RX](dev, true);

    wm_ll_i2s_clear_rx_fifo(dev->reg_dev);

    return WM_ERR_SUCCESS;
}

int wm_hal_i2s_rx_dma_buf_append(wm_hal_i2s_dev_t *dev, void *buf, int len)
{
    return wm_hal_i2s_dma_buf_append(dev, buf, len, WM_HAL_I2S_TR_RX);
}

/* XXX:
 * This function needs to be in the same context as `wm_hal_i2s_dma_tx_isr_cb`
 * to prevent competitive access on the variable `desc_tx_cnt`, of course,
 * except the case when the DMA/interrupt is stopped.
 *
 * Typically invoked by the DMA ISR, it should be called by the user only
 * when `wm_hal_i2s_is_txdma_zombie` return true which means DMA is stopped
 * */
int wm_hal_i2s_tx_dma_buf_append(wm_hal_i2s_dev_t *dev, void *buf, int len)
{
    return wm_hal_i2s_dma_buf_append(dev, buf, len, WM_HAL_I2S_TR_TX);
}

int wm_hal_i2s_init(wm_hal_i2s_dev_t *dev, wm_hal_i2s_cfg_t *cfg)
{
    if (dev == NULL || cfg == NULL) {
        WM_HAL_I2S_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->cfg) {
        WM_HAL_I2S_LOG_E("I2S HAL has been initialized already!");
        return WM_ERR_FAILED;
    }

    if (WM_ERR_SUCCESS != wm_hal_i2s_check_cfg(cfg)) {
        WM_HAL_I2S_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    dev->priv.cfg = *cfg;
    dev->cfg      = &dev->priv.cfg;

    /* I2S configure */
    if (WM_ERR_SUCCESS != wm_hal_i2s_cfg_init(dev)) {
        WM_HAL_I2S_LOG_MARKER;
        goto fail;
    }

    /* DMA desc and link configure */
    if (WM_ERR_SUCCESS != wm_hal_i2s_dma_desc_init(dev)) {
        WM_HAL_I2S_LOG_MARKER;
        goto fail;
    }

    if (wm_hal_irq_attach_sw_vector(dev->irq_num, wm_hal_i2s_isr_handler, dev)) {
        WM_HAL_I2S_LOG_MARKER;
        goto fail;
    }

    wm_hal_irq_enable(WM_IRQ_I2S);

    return WM_ERR_SUCCESS;

fail:
    dev->cfg = NULL;
    return WM_ERR_FAILED;
}

int wm_hal_i2s_deinit(wm_hal_i2s_dev_t *dev)
{
    int ret = WM_ERR_SUCCESS;

    if (WM_HAL_I2S_INIT_CHECK()) {
        WM_HAL_I2S_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    if (WM_ERR_SUCCESS != wm_hal_i2s_dma_deinit(dev)) {
        WM_HAL_I2S_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    /* stop dma first before stop I2S TX for tx */
    if (WM_ERR_SUCCESS != wm_hal_i2s_reset(dev)) {
        WM_HAL_I2S_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    memset(dev->cfg, 0x0, sizeof(*dev->cfg));
    dev->cfg = NULL;

    return ret;
}

int wm_hal_i2s_dump_desc(wm_hal_i2s_dev_t *dev)
{
    int i;

    if (WM_HAL_I2S_INIT_CHECK()) {
        WM_HAL_I2S_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    wm_hal_i2s_cfg_t *cfg = dev->cfg;

    wm_hal_i2s_priv_t *priv = &dev->priv;

    wm_hal_dma_desc_t *txdesc = priv->desc[WM_HAL_I2S_TR_TX];
    wm_hal_dma_desc_t *rxdesc = priv->desc[WM_HAL_I2S_TR_RX];

    if (cfg->dir == WM_I2S_DIR_INOUT || cfg->dir == WM_I2S_DIR_OUT) {
        WM_HAL_I2S_LOG_I("desc tx  en:%s", priv->desc_en[WM_HAL_I2S_TR_TX] ? "True" : "False");
        WM_HAL_I2S_LOG_I("desc tx cid:%d", priv->desc_cid[WM_HAL_I2S_TR_TX]);
        WM_HAL_I2S_LOG_I("desc tx cnt:%d", priv->desc_cnt[WM_HAL_I2S_TR_TX]);
        for (i = 0; i < cfg->desc_node_depth[WM_HAL_I2S_TR_TX]; i++) {
            wm_hal_i2s_dump_dma_desc(&txdesc[i], i, "TX");
        }
    }

    if (cfg->dir == WM_I2S_DIR_INOUT || cfg->dir == WM_I2S_DIR_IN) {
        WM_HAL_I2S_LOG_I("desc rx  en:%s", priv->desc_en[WM_HAL_I2S_TR_RX] ? "True" : "False");
        WM_HAL_I2S_LOG_I("desc rx cid:%d", priv->desc_cid[WM_HAL_I2S_TR_RX]);
        WM_HAL_I2S_LOG_I("desc rx cnt:%d", priv->desc_cnt[WM_HAL_I2S_TR_RX]);
        for (i = 0; i < cfg->desc_node_depth[WM_HAL_I2S_TR_RX]; i++) {
            wm_hal_i2s_dump_dma_desc(&rxdesc[i], i, "RX");
        }
    }

    wm_hal_i2s_dump_regs(dev);

    return WM_ERR_SUCCESS;
}

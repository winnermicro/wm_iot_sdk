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

#define LOG_TAG "HAL_I2S"
#include "wm_log.h"

#define WM_HAL_I2S_LOGD(...)        ((void)0)

#define WM_HAL_I2S_USE_RX(cfg)      ((cfg)->dir == WM_I2S_DIR_IN || (cfg)->dir == WM_I2S_DIR_INOUT)
#define WM_HAL_I2S_USE_TX(cfg)      ((cfg)->dir == WM_I2S_DIR_OUT || (cfg)->dir == WM_I2S_DIR_INOUT)

#define WM_HAL_I2S_TXTH             4 /**< TXFIFO threshold */
#define WM_HAL_I2S_RXTH             0 /**< RXFIFO threshold */

#define WM_HAL_I2S_ALIGN4(v)        (((v) & 3) == 0)

/**
 * @brief DMA empty flag for receive
 */
#define WM_I2S_DMA_STATUS_BUF_EMPTY (1UL << 0x1f)

static int wm_hal_i2s_check_cfg(wm_hal_i2s_cfg_t *cfg)
{
    if (cfg->xtype != WM_I2S_XFER_DMA) {
        WM_HAL_I2S_LOGD("bad xtype");
        return WM_ERR_INVALID_PARAM;
    }

    if (WM_HAL_I2S_USE_RX(cfg)) {
        if (!(cfg->rx_callback && cfg->rx_dma)) {
            WM_HAL_I2S_LOGD("bad rx dma or cb");
            return WM_ERR_INVALID_PARAM;
        }
    } else if (WM_HAL_I2S_USE_TX(cfg)) {
        if (!(cfg->tx_callback && cfg->tx_dma)) {
            WM_HAL_I2S_LOGD("bad tx dma or cb");
            return WM_ERR_INVALID_PARAM;
        }
    } else {
        return WM_ERR_INVALID_PARAM;
    }

    return WM_ERR_SUCCESS;
}

static void wm_hal_i2s_dma_tx_done_callback(wm_dma_ch_t ch, uint32_t sts, void *user_data)
{
    wm_hal_i2s_dev_t *dev   = user_data;
    wm_hal_i2s_xfer_t *tx   = &dev->tx;
    wm_hal_dma_desc_t *read = tx->r;
    wm_hal_i2s_cfg_t *cfg   = &dev->cfg;

    wm_hal_i2s_event_t evt;

    while (!read->vld && read->src_addr) {
        tx->xfer_pack_cnt++;
        tx->xfer_size += read->ctrl.chain.len;

        WM_HAL_I2S_LOGD("tx dma done");

        evt.type = WM_HAL_I2S_EVENT_TX_DONE;
        evt.buf  = (void *)read->src_addr;
        evt.len  = read->ctrl.chain.len;
        evt.priv = NULL;

        /*remove buffer*/
        read->src_addr = 0;

        /*In extreme cases, receiving two blocks will be interrupted once,so need process next block*/
        read = read->next;

        if (cfg->tx_callback) {
            /*notify use tx done*/
            cfg->tx_callback(dev->master_dev, &evt);
        }
    }
    tx->r = read;
}

static void wm_hal_i2s_dma_rx_done_callback(wm_dma_ch_t ch, uint32_t sts, void *user_data)
{
    wm_hal_i2s_dev_t *dev   = user_data;
    wm_hal_i2s_xfer_t *rx   = &dev->rx;
    wm_hal_dma_desc_t *read = rx->r;
    wm_hal_i2s_cfg_t *cfg   = &dev->cfg;
    wm_hal_i2s_event_t evt;

    /*In extreme cases, receiving two blocks will be interrupted once,so need process multi block*/
    while (!read->vld && read->dest_addr) {
        rx->xfer_pack_cnt++;
        rx->xfer_size += read->ctrl.chain.len;

        /*RX data ready event*/
        evt.type = WM_HAL_I2S_EVENT_RX_READY;
        evt.buf  = (void *)read->dest_addr;
        evt.len  = read->ctrl.chain.len;
        evt.priv = NULL;

        /*remove buffer*/
        read->dest_addr = 0;

        /*next dma node*/
        read = read->next;

        if (cfg->rx_callback) {
            /*notify use rx ready*/
            cfg->rx_callback(dev->master_dev, &evt);
        }
    }
    rx->r = read;
}

static void wm_hal_i2s_irq_rx_done_process(wm_hal_i2s_dev_t *dev)
{
    wm_hal_i2s_xfer_t *rx   = &dev->rx;
    wm_hal_dma_desc_t *read = rx->r;
    wm_hal_i2s_cfg_t *cfg   = &dev->cfg;

    wm_hal_i2s_event_t evt;
    wm_dma_sts_info_t status;

    if (read->vld && read->dest_addr) {
        /*get dma receive cnt int current buffer.*/
        wm_hal_dma_get_status(cfg->rx_dma, cfg->rx_dma_ch, &status);

        if (status.xfer_cnt > 0) {
            rx->xfer_pack_cnt++;
            rx->xfer_size += status.xfer_cnt;

            /*data ready event*/
            evt.type = WM_HAL_I2S_EVENT_RX_READY;
            evt.buf  = (void *)read->dest_addr;
            evt.len  = status.xfer_cnt;
            evt.priv = NULL;

            /*reset current desc*/
            read->dest_addr = 0;
            read->vld       = 0;
            read            = read->next;
            rx->r           = read;

            if (cfg->rx_callback) {
                /*notify user process the left data*/
                cfg->rx_callback(dev->master_dev, &evt);
            }
        }
    }

    /*RX done event*/
    evt.type = WM_HAL_I2S_EVENT_RX_DONE;
    evt.buf  = NULL;
    evt.len  = 0;
    evt.priv = NULL;

    if (dev->cfg.rx_callback) {
        /*notify use rx done*/
        dev->cfg.rx_callback(dev->master_dev, &evt);
    }
}

static void wm_hal_i2s_isr_handler(wm_irq_no_t irq, void *arg)
{
    wm_hal_i2s_dev_t *dev = (wm_hal_i2s_dev_t *)arg;
    uint32_t isr;

    /*get rxdone irq*/
    isr = wm_ll_i2s_get_irq_flag(dev->reg_dev);

    /*clear irq flag*/
    wm_ll_i2s_clear_all_irq_flag(dev->reg_dev);

    if ((isr | WM_I2S_RXDONE_FLAG)) {
        /*RX done*/

        /* in case that user stop by calling read_stop */
        if (wm_ll_i2s_is_rx_enabled(dev->reg_dev)) {
            wm_hal_i2s_irq_rx_done_process(dev);
        }
    }
}

static int wm_hal_i2s_reset(wm_hal_i2s_dev_t *dev, wm_hal_i2s_cfg_t *cfg)
{
    wm_ll_i2s_reset_ctrl(dev->reg_dev);

    /* STEP 3
        set master/slave mode
        transfer format
        sound channel
        data width
        zero cross
        tx/rx enable
     */

    wm_ll_i2s_set_mode(dev->reg_dev, cfg->mode);
    if (cfg->dir == WM_I2S_DIR_INOUT) {
        wm_ll_i2s_set_duplex_en(dev->reg_dev, true);
    }

    wm_ll_i2s_set_data_std(dev->reg_dev, cfg->std);
    wm_ll_i2s_rzc_set_en(dev->reg_dev, false);
    wm_ll_i2s_lzc_set_en(dev->reg_dev, false);

    /* STEP 4 irq enable
       The last package maybe less than RX DMA size, use rxdone irq to get the last package.
    */

    wm_ll_i2s_disable_all_irq(dev->reg_dev);
    wm_ll_i2s_clear_all_irq_flag(dev->reg_dev);

    if (WM_HAL_I2S_USE_RX(cfg)) {
        wm_ll_i2s_enable_irq(dev->reg_dev, WM_I2S_RXDONE_IRQ_MASK);
    }

    WM_HAL_I2S_LOGD("reset ok");

    return WM_ERR_SUCCESS;
}

static int wm_hal_i2s_dma_init(wm_hal_i2s_dev_t *dev, wm_hal_i2s_cfg_t *cfg)
{
    /*STEP 5 set dma*/
    wm_ll_i2s_set_txfifo_threshold(dev->reg_dev, WM_HAL_I2S_TXTH);
    wm_ll_i2s_set_rxfifo_threshold(dev->reg_dev, WM_HAL_I2S_RXTH);

    if (cfg->xtype == WM_I2S_XFER_DMA) {
        if (WM_HAL_I2S_USE_TX(cfg)) {
            wm_ll_i2s_set_tx_dma_en(dev->reg_dev, true);
        }
        if (WM_HAL_I2S_USE_RX(cfg)) {
            wm_ll_i2s_set_rx_dma_en(dev->reg_dev, true);
        }
    }

    WM_HAL_I2S_LOGD("dma init");

    return WM_ERR_SUCCESS;
}

static int wm_hal_i2s_start_rx_dma(wm_hal_i2s_dev_t *dev, wm_hal_dma_desc_t *tail)
{
    wm_hal_i2s_cfg_t *cfg = &dev->cfg;

    wm_log_debug("rx dma start");

    wm_hal_dma_register_callback(cfg->rx_dma, cfg->rx_dma_ch, wm_hal_i2s_dma_rx_done_callback, dev);
    wm_hal_dma_config(cfg->rx_dma, tail);

    wm_ll_i2s_clear_rx_fifo(dev->reg_dev);

    wm_hal_dma_start(cfg->rx_dma, cfg->rx_dma_ch);

    wm_ll_i2s_set_rx_dma_en(dev->reg_dev, true);
    wm_ll_i2s_set_rx_en(dev->reg_dev, true);
    wm_ll_i2s_set_en(dev->reg_dev, true);

    return WM_ERR_SUCCESS;
}

static int wm_hal_i2s_start_tx_dma(wm_hal_i2s_dev_t *dev, wm_hal_dma_desc_t *tail)
{
    wm_hal_i2s_cfg_t *cfg = &dev->cfg;

    wm_log_debug("tx dma start");

    wm_hal_dma_register_callback(cfg->tx_dma, cfg->tx_dma_ch, wm_hal_i2s_dma_tx_done_callback, dev);
    wm_hal_dma_config(cfg->tx_dma, tail);

    wm_hal_dma_start(cfg->tx_dma, cfg->tx_dma_ch);

    wm_ll_i2s_set_tx_dma_en(dev->reg_dev, true);
    wm_ll_i2s_set_tx_en(dev->reg_dev, true);
    wm_ll_i2s_set_en(dev->reg_dev, true);

    return WM_ERR_SUCCESS;
}

int wm_hal_i2s_init(wm_hal_i2s_dev_t *dev, wm_hal_i2s_cfg_t *cfg)
{
    int err;

    if (!(dev && cfg)) {
        WM_HAL_I2S_LOGD("bad init param");
        return WM_ERR_INVALID_PARAM;
    }

    /*Check whether it has been initialized*/
    if (wm_ll_i2s_is_enabled(dev->reg_dev)) {
        WM_HAL_I2S_LOGD("aready init");
        return WM_ERR_ALREADY_INITED;
    }

    /*Check config params*/
    err = wm_hal_i2s_check_cfg(cfg);
    if (err != WM_ERR_SUCCESS) {
        WM_HAL_I2S_LOGD("check cfg fail");
        return err;
    }

    memset(&dev->rx, 0, sizeof(dev->rx));
    memset(&dev->tx, 0, sizeof(dev->tx));

    dev->cfg = *cfg;

    /*reset register*/
    wm_hal_i2s_reset(dev, cfg);

    /*init dma*/
    wm_hal_i2s_dma_init(dev, cfg);

    /*attach irq handler*/
    if (wm_hal_irq_attach_sw_vector(dev->irq_num, wm_hal_i2s_isr_handler, dev)) {
        return WM_ERR_FAILED;
    }

    /*enable irq*/
    wm_hal_irq_enable(WM_IRQ_I2S);

    WM_HAL_I2S_LOGD("init ok");

    return WM_ERR_SUCCESS;
}

int wm_hal_i2s_deinit(wm_hal_i2s_dev_t *dev)
{
    if (!(dev)) {
        WM_HAL_I2S_LOGD("deinit bad param");
        return WM_ERR_INVALID_PARAM;
    }

    /*disable irq and detach irq handler*/
    wm_hal_irq_disable(dev->irq_num);
    wm_hal_irq_detach_sw_vector(dev->irq_num);

    /*stop tx,rx dma*/
    wm_hal_i2s_tx_stop(dev);
    wm_hal_i2s_rx_stop(dev);

    /*disable i2s*/
    wm_ll_i2s_set_en(dev->reg_dev, 0);

    WM_HAL_I2S_LOGD("deinit ok");

    return WM_ERR_SUCCESS;
}

void wm_hal_i2s_set_left_zero_cross_en(wm_hal_i2s_dev_t *dev, bool en)
{
    if (dev) {
        WM_HAL_I2S_LOGD("set left zero cross %d", en);
        wm_ll_i2s_lzc_set_en(dev->reg_dev, en);
    }
}

void wm_hal_i2s_set_right_zero_cross_en(wm_hal_i2s_dev_t *dev, bool en)
{
    if (dev) {
        WM_HAL_I2S_LOGD("set right zero cross %d", en);
        wm_ll_i2s_rzc_set_en(dev->reg_dev, en);
    }
}

void wm_hal_i2s_set_mono_left_en(wm_hal_i2s_dev_t *dev, bool en)
{
    if (dev) {
        WM_HAL_I2S_LOGD("set mono left %d", en);
        wm_ll_i2s_set_mono_left(dev->reg_dev, en);
    }
}

void wm_hal_i2s_set_tx_en(wm_hal_i2s_dev_t *dev, bool en)
{
    if (dev) {
        WM_HAL_I2S_LOGD("set tx en %d", en);
        wm_ll_i2s_set_tx_en(dev->reg_dev, en);
    }
}

void wm_hal_i2s_set_rx_en(wm_hal_i2s_dev_t *dev, bool en)
{
    if (dev) {
        WM_HAL_I2S_LOGD("set rx en %d", en);
        wm_ll_i2s_set_rx_en(dev->reg_dev, en);
    }
}

void wm_hal_i2s_set_tx_clock_inv_en(wm_hal_i2s_dev_t *dev, bool en)
{
    if (dev) {
        WM_HAL_I2S_LOGD("set tx clock inverse %d", en);
        wm_ll_i2s_set_tx_clock_phase(dev->reg_dev, en);
    }
}

void wm_hal_i2s_set_rx_clock_inv_en(wm_hal_i2s_dev_t *dev, bool en)
{
    if (dev) {
        WM_HAL_I2S_LOGD("set rx clock inverse %d", en);
        wm_ll_i2s_set_rx_clock_phase(dev->reg_dev, en);
    }
}

void wm_hal_i2s_set_tx_mute_en(wm_hal_i2s_dev_t *dev, bool mute)
{
    if (dev) {
        WM_HAL_I2S_LOGD("set tx mute %d", mute);
        wm_ll_i2s_set_mute_en(dev->reg_dev, mute);
    }
}

bool wm_hal_i2s_is_tx_dma_running(wm_hal_i2s_dev_t *dev)
{
    wm_dma_sts_info_t status;
    bool ret = false;
    if (dev) {
        wm_hal_dma_get_status(dev->cfg.tx_dma, dev->cfg.tx_dma_ch, &status);
        if (status.sts == WM_DMA_RUNNIG) {
            ret = true;
        }
    }
    return ret;
}

bool wm_hal_i2s_is_rx_dma_running(wm_hal_i2s_dev_t *dev)
{
    wm_dma_sts_info_t status;
    bool ret = false;

    if (dev) {
        wm_hal_dma_get_status(dev->cfg.rx_dma, dev->cfg.rx_dma_ch, &status);
        if (status.sts == WM_DMA_RUNNIG) {
            ret = true;
        }
    }
    return ret;
}

bool wm_hal_i2s_is_tx_enabled(wm_hal_i2s_dev_t *dev)
{
    return wm_ll_i2s_is_tx_enabled(dev->reg_dev);
}

bool wm_hal_i2s_is_rx_enabled(wm_hal_i2s_dev_t *dev)
{
    return wm_ll_i2s_is_rx_enabled(dev->reg_dev);
}

static int wm_hal_i2s_init_dma_desc(wm_hal_i2s_dev_t *dev, wm_hal_dma_desc_t *desc, bool is_tx)
{
    desc->vld                   = 0;
    desc->ctrl.chain.len        = 0;
    desc->ctrl.chain.data_unit  = WM_DMA_XFER_UNIT_WORD;
    desc->ctrl.chain.burst_size = WM_DMA_BURST_1UNIT;

    if (is_tx) {
        desc->extend_ctrl.ch           = dev->cfg.tx_dma_ch;
        desc->dest_addr                = wm_ll_i2s_get_tx_fifo_addr(dev->reg_dev);
        desc->extend_ctrl.req_sel      = WM_DMA_I2S_TX_REQ;
        desc->ctrl.chain.src_addr_inc  = WM_DMA_ADDR_INC;
        desc->ctrl.chain.dest_addr_inc = WM_DMA_ADDR_FIXED;
    } else {
        desc->extend_ctrl.ch           = dev->cfg.rx_dma_ch;
        desc->src_addr                 = wm_ll_i2s_get_rx_fifo_addr(dev->reg_dev);
        desc->extend_ctrl.req_sel      = WM_DMA_I2S_RX_REQ;
        desc->ctrl.chain.dest_addr_inc = WM_DMA_ADDR_INC;
        desc->ctrl.chain.src_addr_inc  = WM_DMA_ADDR_FIXED;
    }

    desc->extend_ctrl.int_en   = WM_DMA_CH_EN_CTRL_ENABLE;
    desc->extend_ctrl.int_type = WM_DMA_INT_XFER_DONE;
    desc->extend_ctrl.mode     = WM_DMA_HW_MODE;

    desc->extend_ctrl.chain_mode    = WM_DMA_CHAIN_MODE_LIST;
    desc->extend_ctrl.warp_mode_en  = WM_DMA_WARP_CTRL_DISABLE;
    desc->extend_ctrl.chain_mode_en = WM_DMA_CHAIN_MODE_ENABLE;

    return WM_ERR_SUCCESS;
}

static int wm_hal_i2s_rx_init_chain(wm_hal_i2s_dev_t *dev)
{
    int i;
    wm_hal_dma_desc_t *p = NULL;

    p = wm_os_internal_calloc(1, dev->cfg.rx_pkt_num * sizeof(wm_hal_dma_desc_t));

    if (!p) {
        return WM_ERR_NO_MEM;
    }

    for (i = 0; i < dev->cfg.rx_pkt_num; i++) {
        wm_hal_i2s_init_dma_desc(dev, &p[i], false);

        if (i < dev->cfg.rx_pkt_num - 1) {
            p[i].next = &p[i + 1];
        } else {
            p[i].next = &p[0];
        }
    }

    dev->rx.w    = p;
    dev->rx.r    = p;
    dev->rx.desc = p;

    WM_HAL_I2S_LOGD("rx init chain");

    return WM_ERR_SUCCESS;
}

static int wm_hal_i2s_tx_init_chain(wm_hal_i2s_dev_t *dev)
{
    int i;
    wm_hal_dma_desc_t *p = NULL;

    p = calloc(1, dev->cfg.tx_pkt_num * sizeof(wm_hal_dma_desc_t));

    if (!p) {
        return WM_ERR_NO_MEM;
    }

    for (i = 0; i < dev->cfg.tx_pkt_num; i++) {
        wm_hal_i2s_init_dma_desc(dev, &p[i], true);

        if (i < dev->cfg.tx_pkt_num - 1) {
            p[i].next = &p[i + 1];
        } else {
            p[i].next = &p[0];
        }
    }

    dev->tx.w    = p;
    dev->tx.r    = p;
    dev->tx.desc = p;

    WM_HAL_I2S_LOGD("tx init chain");

    return WM_ERR_SUCCESS;
}

int wm_hal_i2s_rx_start(wm_hal_i2s_dev_t *dev)
{
    wm_ll_i2s_set_rx_en(dev->reg_dev, 1);
    return WM_ERR_SUCCESS;
}

int wm_hal_i2s_rx_stop(wm_hal_i2s_dev_t *dev)
{
    wm_hal_i2s_event_t evt;
    int err;

    wm_hal_dma_desc_t *read = dev->rx.r;

    if (!(dev && dev->cfg.rx_dma)) {
        return WM_ERR_INVALID_PARAM;
    }

    wm_ll_i2s_set_rx_en(dev->reg_dev, 0);

    if (!dev->rx.desc) {
        /*Not start append receive buffer or stoped before.*/
        return WM_ERR_SUCCESS;
    }

    err = wm_hal_dma_stop(dev->cfg.rx_dma, dev->cfg.rx_dma_ch);

    do {
        if (read->dest_addr) {
            WM_HAL_I2S_LOGD("remove rx buf");
            evt.type = WM_HAL_I2S_EVENT_RX_READY;
            evt.buf  = (void *)read->dest_addr;
            evt.len  = 0;
            evt.priv = NULL;

            /*notify use free buffer*/
            if (dev->cfg.rx_callback) {
                dev->cfg.rx_callback(dev->master_dev, &evt);
            }
        }

        read = read->next;
    } while (read != dev->rx.r);

    wm_os_internal_free(dev->rx.desc);

    memset(&dev->rx, 0, sizeof(dev->rx));

    WM_HAL_I2S_LOGD("stop rx");

    return err;
}

int wm_hal_i2s_rx_dma_get_idle_desc_num(wm_hal_i2s_dev_t *dev)
{
    int num = 0;

    if (dev) {
        if (dev->rx.desc) {
            wm_hal_dma_desc_t *start = dev->rx.w;
            wm_hal_dma_desc_t *cur   = start;

            while (1) {
                if (!cur->dest_addr) {
                    num++;
                }
                cur = cur->next;

                if (cur == start) {
                    break;
                }
            }
        } else {
            /*first time, desc not init*/
            num = dev->cfg.rx_pkt_num;
        }
    }

    return num;
}

int wm_hal_i2s_rx_dma_buf_append(wm_hal_i2s_dev_t *dev, void *buf, int len)
{
    wm_dma_sts_info_t status;
    wm_hal_dma_desc_t *tail = NULL;

    if (!(dev && buf && len > 0 && WM_HAL_I2S_ALIGN4(len))) {
        WM_HAL_I2S_LOGD("dev=%p,buf=%p,len=%d", dev, buf, len);
        return WM_ERR_INVALID_PARAM;
    }

    wm_ll_i2s_set_rx_en(dev->reg_dev, 1);

    if (!dev->rx.desc) {
        /*first use, init chain*/
        wm_hal_i2s_rx_init_chain(dev);
    }

    tail = dev->rx.w;

    if (tail->dest_addr) {
        /*Can't append any more.*/
        WM_HAL_I2S_LOGD("Rx can't append. chain=%p\n", tail);
        return WM_ERR_NO_MEM;
    }

    /*set address and size*/
    tail->ctrl.chain.len = len;
    tail->dest_addr      = (uint32_t)buf;
    tail->vld            = WM_I2S_DMA_STATUS_BUF_EMPTY; /**< enable dma receiving */
    dev->rx.w            = tail->next;

    WM_HAL_I2S_LOGD("append buf, chain=%p,len=%u,addr=%x,next=%p\n", tail, tail->ctrl.chain.len, tail->dest_addr, tail->next);

    /*check and start dma*/
    wm_hal_dma_get_status(dev->cfg.rx_dma, dev->cfg.rx_dma_ch, &status);
    if (status.sts != WM_DMA_RUNNIG) {
        wm_hal_i2s_start_rx_dma(dev, tail);
    }

    return WM_ERR_SUCCESS;
}

int wm_hal_i2s_rx_pause(wm_hal_i2s_dev_t *dev)
{
    if (dev) {
        WM_HAL_I2S_LOGD("rx pause");
        wm_ll_i2s_set_rx_en(dev->reg_dev, false);
        return WM_ERR_SUCCESS;
    } else {
        return WM_ERR_INVALID_PARAM;
    }
}

int wm_hal_i2s_rx_resume(wm_hal_i2s_dev_t *dev)
{
    if (dev) {
        WM_HAL_I2S_LOGD("rx resume");
        wm_ll_i2s_set_rx_en(dev->reg_dev, true);
        return WM_ERR_SUCCESS;
    } else {
        return WM_ERR_INVALID_PARAM;
    }
}

int wm_hal_i2s_tx_pause(wm_hal_i2s_dev_t *dev)
{
    if (dev) {
        WM_HAL_I2S_LOGD("tx pause");
        wm_ll_i2s_set_tx_en(dev->reg_dev, false);
        return WM_ERR_SUCCESS;
    } else {
        return WM_ERR_INVALID_PARAM;
    }
}

int wm_hal_i2s_tx_resume(wm_hal_i2s_dev_t *dev)
{
    if (dev) {
        WM_HAL_I2S_LOGD("tx resume");
        wm_ll_i2s_set_tx_en(dev->reg_dev, true);
        return WM_ERR_SUCCESS;
    } else {
        return WM_ERR_INVALID_PARAM;
    }
}

int wm_hal_i2s_tx_start(wm_hal_i2s_dev_t *dev)
{
    wm_ll_i2s_set_tx_en(dev->reg_dev, 1);
    return WM_ERR_SUCCESS;
}

int wm_hal_i2s_tx_stop(wm_hal_i2s_dev_t *dev)
{
    int err;
    wm_hal_i2s_event_t evt;
    wm_hal_dma_desc_t *read = dev->tx.r;

    if (!(dev && dev->cfg.tx_dma)) {
        return WM_ERR_INVALID_PARAM;
    }

    wm_ll_i2s_set_tx_en(dev->reg_dev, 0);

    if (!dev->tx.desc) {
        /*Not append any buffer or stoped before.*/
        return WM_ERR_SUCCESS;
    }

    err = wm_hal_dma_stop(dev->cfg.tx_dma, dev->cfg.tx_dma_ch);

    do {
        if (read->src_addr) {
            WM_HAL_I2S_LOGD("remove tx buf");

            evt.type = WM_HAL_I2S_EVENT_TX_DONE;
            evt.buf  = (void *)read->src_addr;
            evt.len  = 0;
            evt.priv = NULL;

            /*notify use free buffer*/
            if (dev->cfg.tx_callback) {
                dev->cfg.tx_callback(dev->master_dev, &evt);
            }
        }

        read = read->next;
    } while (read != dev->tx.r);

    wm_os_internal_free(dev->tx.desc);

    memset(&dev->tx, 0, sizeof(dev->tx));

    WM_HAL_I2S_LOGD("stop tx");

    return err;
}

int wm_hal_i2s_tx_dma_buf_append(wm_hal_i2s_dev_t *dev, void *buf, int len)
{
    wm_dma_sts_info_t status;
    wm_hal_dma_desc_t *tail = NULL;

    if (!(dev && buf && len > 0 && WM_HAL_I2S_ALIGN4(len))) {
        return WM_ERR_INVALID_PARAM;
    }

    wm_ll_i2s_set_tx_en(dev->reg_dev, 1);

    if (!dev->tx.desc) {
        /*first use, init chain*/
        wm_hal_i2s_tx_init_chain(dev);
    }

    tail = dev->tx.w;

    if (tail->src_addr) {
        /*Can't append any more.*/
        WM_HAL_I2S_LOGD("Tx can't append. chain=%p\n", tail);
        return WM_ERR_NO_MEM;
    }

    /*set address and size*/
    tail->ctrl.chain.len = len;
    tail->src_addr       = (uint32_t)buf;
    tail->vld            = WM_I2S_DMA_STATUS_BUF_EMPTY; /**< enable dma receiving */
    dev->tx.w            = tail->next;

    WM_HAL_I2S_LOGD("append buf, chain=%p,len=%u,addr=%x,next=%p\n", tail, tail->ctrl.chain.len, tail->src_addr, tail->next);

    /*check and start dma*/
    wm_hal_dma_get_status(dev->cfg.tx_dma, dev->cfg.tx_dma_ch, &status);
    if (status.sts != WM_DMA_RUNNIG) {
        wm_hal_i2s_start_tx_dma(dev, tail);
    }

    return WM_ERR_SUCCESS;
}

int wm_hal_i2s_set_format(wm_hal_i2s_dev_t *dev, enum wm_i2s_bits bits, enum wm_i2s_chan_type channel)
{
    dev->fmt.bits    = bits;
    dev->fmt.channel = channel;
    wm_ll_i2s_set_chan_type(dev->reg_dev, channel);
    wm_ll_i2s_set_data_bits(dev->reg_dev, bits);

    return WM_ERR_SUCCESS;
}

int wm_hal_i2s_print_cfg(wm_hal_i2s_cfg_t *cfg)
{
    wm_log_info("mode=%d", cfg->mode);
    wm_log_info("dir=%d", cfg->dir);
    wm_log_info("std=%d", cfg->std);
    wm_log_info("xtype=%d", cfg->xtype);
    wm_log_info("rx_dma=%p", cfg->rx_dma);
    wm_log_info("tx_dma=%p", cfg->tx_dma);

    wm_log_info("rx_dma_ch=%d", cfg->rx_dma_ch);
    wm_log_info("tx_dma_ch=%d", cfg->tx_dma_ch);
    wm_log_info("rx_pkt_num=%d", cfg->rx_pkt_num);
    wm_log_info("tx_pkt_num=%d", cfg->tx_pkt_num);

    wm_log_info("rx_callback=%p", cfg->rx_callback);
    wm_log_info("tx_callback=%p", cfg->tx_callback);

    return WM_ERR_SUCCESS;
}

int wm_hal_i2s_dump_info(wm_hal_i2s_dev_t *dev)
{
    uint8_t cnt       = 0;
    wm_i2s_reg_t *reg = NULL;
    wm_dma_sts_info_t status;

    if (!dev) {
        return WM_ERR_INVALID_PARAM;
    }

    reg = dev->reg_dev;

    wm_log_info("I2S REG CTRL:0x%08x", reg->control.val);
    wm_log_info("I2S REG IMSK:0x%08x", reg->imask.val);
    wm_log_info("I2S REG FLAG:0x%08x", reg->int_flag.val);
    wm_log_info("I2S REG STAT:0x%08x", reg->status.val);

    cnt = wm_ll_i2s_get_txfifo_count(dev->reg_dev);
    wm_log_info("I2S REG TXFIFO(word):%d", cnt);

    cnt = wm_ll_i2s_get_rxfifo_count(dev->reg_dev);
    wm_log_info("I2S REG RXFIFO(word):%d", cnt);

    cnt = wm_ll_i2s_get_valid_bytes_in_last_word(dev->reg_dev);
    wm_log_info("I2S REG RXFIFO last valid cnt(bytes):%d", cnt);

    if (dev->cfg.rx_dma) {
        if (wm_hal_dma_get_status(dev->cfg.rx_dma, dev->cfg.rx_dma_ch, &status) == WM_ERR_SUCCESS) {
            wm_log_info("rx dma status=%d,cnt=%u", status.sts, status.xfer_cnt);
        }
    }

    if (dev->cfg.tx_dma) {
        if (wm_hal_dma_get_status(dev->cfg.tx_dma, dev->cfg.tx_dma_ch, &status) == WM_ERR_SUCCESS) {
            wm_log_info("tx dma status=%d,cnt=%u", status.sts, status.xfer_cnt);
        }
    }

    wm_log_info("TX: send_pack_cnt=%u,send_size=%llu", dev->tx.xfer_pack_cnt, dev->tx.xfer_size);
    wm_log_info("RX: recv_pack_cnt=%u,recv_size=%llu", dev->rx.xfer_pack_cnt, dev->rx.xfer_size);

    return WM_ERR_SUCCESS;
}

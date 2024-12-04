/**
 * @file wm_hal_dma.c
 *
 * @brief dma hardware abstraction layer implementation
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
#include "wm_soc.h"
#include "wm_log.h"
#include "wm_reg_op.h"
#include "wm_hal_dma.h"

#undef LOG_TAG
#define LOG_TAG           "DMA HAL"

#define WM_HAL_DMA_ASSERT assert
// #define WM_HAL_DMA_LOG_EN
#ifdef WM_HAL_DMA_LOG_EN
#define WM_HAL_DMA_LOG_D         wm_log_debug
#define WM_HAL_DMA_LOG_I         wm_log_info
#define WM_HAL_DMA_LOG_W         wm_log_warn
#define WM_HAL_DMA_LOG_E         wm_log_error
#define WM_HAL_DMA_LOG_DUMP      wm_log_dump
#define WM_HAL_DMA_FUNC_EXIT(fn) WM_HAL_DMA_LOG_D("%s exit", fn)
#else
#define WM_HAL_DMA_LOG_D(...)    ((void)0)
#define WM_HAL_DMA_LOG_I(...)    ((void)0)
#define WM_HAL_DMA_LOG_W(...)    ((void)0)
#define WM_HAL_DMA_LOG_E(...)    ((void)0)
#define WM_HAL_DMA_LOG_DUMP(...) ((void)0)
#define WM_HAL_DMA_FUNC_EXIT(fn) WM_HAL_DMA_LOG_D("%s exit", fn)
#endif

#define WM_HAL_DMA_FUNC_NAME  __func__
#define WM_HAL_DMA_DUMP_WIDTH (0xf)

WM_HAL_DMA_STATIC wm_hal_dma_ch_list_t hal_dma_ch_list[WM_DMA_CH_MAX] = { 0 };

WM_HAL_DMA_STATIC wm_hal_dma_irq_priv_t hal_irq_priv[WM_DMA_CH_IRQ_MAX] = { 0 };

WM_HAL_DMA_STATIC wm_dma_ch_irq_t hal_dma_ch2irq[WM_DMA_CH_MAX] = {
    WM_DMA_CH_IRQ_0,   WM_DMA_CH_IRQ_1,   WM_DMA_CH_IRQ_2,   WM_DMA_CH_IRQ_3,
    WM_DMA_CH_IRQ_4_7, WM_DMA_CH_IRQ_4_7, WM_DMA_CH_IRQ_4_7, WM_DMA_CH_IRQ_4_7,
};

WM_HAL_DMA_STATIC wm_hal_dma_ch_list_t *wm_hal_dma_get_ch_list(void)
{
    return &hal_dma_ch_list[0];
}

WM_HAL_DMA_STATIC wm_hal_dma_irq_priv_t *wm_hal_dma_get_irq_priv(uint8_t irq_idx)
{
    return &hal_irq_priv[irq_idx];
}

WM_HAL_DMA_STATIC wm_dma_ch_irq_t wm_hal_dma_get_ch2irq(uint8_t ch)
{
    return hal_dma_ch2irq[ch];
}

wm_hal_dma_status_t wm_hal_dma_init(wm_hal_dma_dev_t *dma_dev)
{
    wm_hal_dma_ch_list_t *ch_list = NULL;
    wm_hal_dma_status_t ret       = WM_HAL_DMA_STATUS_SUCCESS;

    /* interrupt reset */
    wm_ll_dma_set_int_mask(dma_dev->reg_base, WM_DMA_CH07_INT_MAP);
    wm_ll_dma_set_int_sts(dma_dev->reg_base, WM_DMA_CH07_INT_MAP);

    /* init dma hal ch list */
    WM_HAL_DMA_LIST_INIT(&dma_dev->ch_list);
    ch_list = wm_hal_dma_get_ch_list();
    for (uint8_t ch_idx = 0; ch_idx < WM_DMA_CH_MAX; ch_idx++) {
        ch_list[ch_idx].ch = ch_idx;
        WM_HAL_DMA_LIST_PUSH(&dma_dev->ch_list, &ch_list[ch_idx].list);
        wm_hal_dma_unregister_callback(dma_dev, ch_idx);
    }
    /* used for dma ch alloc/release method 3 */
    //dma_dev->dma_ch = WM_DMA_CH_0;

    for (uint8_t irq_idx = WM_DMA_CH_IRQ_0; irq_idx < WM_DMA_CH_IRQ_MAX; irq_idx++) {
        dma_dev->irq_priv[irq_idx]         = wm_hal_dma_get_irq_priv(irq_idx);
        dma_dev->irq_priv[irq_idx]->dev    = dma_dev;
        dma_dev->irq_priv[irq_idx]->ch_irq = dma_dev->irq_num + irq_idx;
        WM_HAL_DMA_LOG_I("hal dev:0x%x attach irq:%d", dma_dev, dma_dev->irq_priv[irq_idx]->ch_irq);
        wm_hal_irq_attach_sw_vector(dma_dev->irq_priv[irq_idx]->ch_irq, wm_hal_dma_isr_entry,
                                    (void *)dma_dev->irq_priv[irq_idx]);
    }

    WM_HAL_DMA_MEM_ZERO(&dma_dev->desc_list, sizeof(dma_dev->desc_list));

    WM_HAL_DMA_FUNC_EXIT(WM_HAL_DMA_FUNC_NAME);
    return ret;
}

wm_hal_dma_status_t wm_hal_dma_deinit(wm_hal_dma_dev_t *dma_dev)
{
    wm_hal_dma_ch_list_t *ch_list = NULL;
    wm_hal_dma_status_t ret       = WM_HAL_DMA_STATUS_SUCCESS;

    /* interrupt reset */
    wm_ll_dma_set_int_mask(dma_dev->reg_base, WM_DMA_CH07_INT_MAP);
    wm_ll_dma_set_int_sts(dma_dev->reg_base, WM_DMA_CH07_INT_MAP);

    /* deinit dma hal ch list */
    for (uint8_t ch_idx = 0; ch_idx < WM_DMA_CH_MAX; ch_idx++) {
        ch_list = (wm_hal_dma_ch_list_t *)WM_HAL_DMA_LIST_FIRST(&dma_dev->ch_list, struct wm_hal_dma_ch_list_s, list);
        WM_HAL_DMA_LIST_DEL(&ch_list->list);
        WM_HAL_DMA_MEM_ZERO(&ch_list->list, sizeof(wm_hal_dma_ch_list_t));
        wm_hal_dma_unregister_callback(dma_dev, ch_idx);
    }

    for (uint8_t irq_idx = WM_DMA_CH_IRQ_0; irq_idx < WM_DMA_CH_IRQ_MAX; irq_idx++) {
        WM_HAL_DMA_LOG_I("detach irq:%d", dma_dev->irq_num + irq_idx);
        wm_hal_irq_disable(dma_dev->irq_num + irq_idx);
        wm_hal_irq_detach_sw_vector(dma_dev->irq_num + irq_idx);
        WM_HAL_DMA_MEM_ZERO(&dma_dev->irq_priv[0], sizeof(wm_hal_dma_irq_priv_t));
    }

    WM_HAL_DMA_MEM_ZERO(&dma_dev->desc_list, sizeof(dma_dev->desc_list));
    WM_HAL_DMA_FUNC_EXIT(WM_HAL_DMA_FUNC_NAME);
    return ret;
}

void wm_hal_dma_ch_isr_entry(wm_hal_dma_dev_t *dma_dev, uint8_t ch, uint32_t sts)
{
    uint8_t auto_reload = 0;

    wm_ll_dma_set_int_sts(dma_dev->reg_base, WM_BIT(ch * WM_DMA_INT_MAX + WM_DMA_INT_BURST_DONE) |
                                                 WM_BIT(ch * WM_DMA_INT_MAX + WM_DMA_INT_XFER_DONE));

    /* HW workaourd: stop and start dma in ISR to fix crash issue when auto reload enable */
    auto_reload = wm_ll_dma_get_auto_reload(dma_dev->reg_base, ch);
    if (auto_reload) {
        wm_hal_dma_stop(dma_dev, ch);
        wm_hal_dma_start(dma_dev, ch);
    }
    WM_HAL_DMA_LOG_I("ch:%d, int_sts:0x%x", ch, wm_ll_dma_get_int_sts(dma_dev->reg_base));
    WM_HAL_DMA_ASSERT(dma_dev->dma_user_cb[ch].cb != NULL);
    dma_dev->dma_user_cb[ch].cb(ch, sts, dma_dev->dma_user_cb[ch].cb_priv);
}

void wm_hal_dma_isr_entry(wm_irq_no_t irq, void *priv)
{
    wm_hal_dma_irq_priv_t *irq_priv = (wm_hal_dma_irq_priv_t *)priv;
    wm_hal_dma_dev_t *dma_dev;
    //wm_hal_dma_desc_t *desc = NULL;
    uint32_t int_sts = 0;
    uint32_t sts     = 0; //TODO
    uint8_t ch       = WM_DMA_CH_MAX;
    //uint8_t list_en = 0;

    WM_HAL_DMA_ASSERT(irq_priv != NULL);
    dma_dev = (wm_hal_dma_dev_t *)irq_priv->dev;
    WM_HAL_DMA_ASSERT(dma_dev != NULL);
    WM_HAL_DMA_ASSERT(irq_priv->ch_irq == irq);
    //TBD, more check for int sts with ch
    int_sts = wm_ll_dma_get_int_sts(dma_dev->reg_base);
    wm_ll_dma_set_int_mask(dma_dev->reg_base, wm_ll_dma_get_int_mask(dma_dev->reg_base) | int_sts);
    WM_HAL_DMA_LOG_I("irq enter for irq:%d, hal dev:0x%x, ch_irq:%d, int_sts:0x%x", irq, (uint32_t)dma_dev, irq_priv->ch_irq,
                     int_sts);

    if (irq_priv->ch_irq >= (dma_dev->irq_num + WM_DMA_CH_IRQ_0) && irq_priv->ch_irq <= (dma_dev->irq_num + WM_DMA_CH_IRQ_3)) {
        ch = irq_priv->ch_use.ch;
        WM_HAL_DMA_ASSERT(ch == irq - dma_dev->irq_num);
        WM_HAL_DMA_ASSERT(ch >= WM_DMA_CH_0 && ch <= WM_DMA_CH_3);
        wm_hal_dma_ch_isr_entry(dma_dev, ch, sts);
    } else {
        /* ch4~ch7 share one */
        for (uint8_t idx = 0; idx <= (WM_DMA_CH_7 - WM_DMA_CH_4); idx++) {
            ch = idx + WM_DMA_CH_4;
            if ((int_sts >> WM_DMA_CH_4 * WM_DMA_INT_MAX) & WM_BIT(idx * WM_DMA_INT_MAX + WM_DMA_INT_XFER_DONE)) {
                if (irq_priv->ch_use.ch_map & WM_BIT(idx)) {
                    wm_hal_dma_ch_isr_entry(dma_dev, ch, sts);
                } else {
                    wm_ll_dma_set_int_sts(dma_dev->reg_base, WM_BIT(ch * WM_DMA_INT_MAX + WM_DMA_INT_BURST_DONE) |
                                                                 WM_BIT(ch * WM_DMA_INT_MAX + WM_DMA_INT_XFER_DONE));
                }
            }
        }
    }

    wm_ll_dma_set_int_mask(dma_dev->reg_base, wm_ll_dma_get_int_mask(dma_dev->reg_base) & ~int_sts);
}

WM_HAL_DMA_STATIC void wm_hal_dma_user_isr_dummy(wm_dma_ch_t ch, uint32_t sts, void *user_data)
{
    WM_HAL_DMA_LOG_W("user not regiser cb for ch:%d", ch);
}

wm_hal_dma_status_t wm_hal_dma_register_callback(wm_hal_dma_dev_t *dma_dev, wm_dma_ch_t ch, wm_hal_dma_callback_t dma_callback,
                                                 void *user_data)
{
    wm_hal_dma_status_t ret = WM_HAL_DMA_STATUS_SUCCESS;

    WM_HAL_DMA_ASSERT(ch >= WM_DMA_CH_0 && ch < WM_DMA_CH_MAX);
    WM_HAL_DMA_ASSERT(dma_callback != NULL);

    dma_dev->dma_user_cb[ch].cb      = dma_callback;
    dma_dev->dma_user_cb[ch].cb_priv = user_data;

    WM_HAL_DMA_LOG_I("dma register callback for ch:%d, cb_priv:0x%x", ch, (uint32_t)user_data);

    return ret;
}

wm_hal_dma_status_t wm_hal_dma_unregister_callback(wm_hal_dma_dev_t *dma_dev, wm_dma_ch_t ch)
{
    wm_hal_dma_status_t ret = WM_HAL_DMA_STATUS_SUCCESS;

    WM_HAL_DMA_ASSERT(ch >= WM_DMA_CH_0 && ch < WM_DMA_CH_MAX);

    dma_dev->dma_user_cb[ch].cb      = wm_hal_dma_user_isr_dummy;
    dma_dev->dma_user_cb[ch].cb_priv = (void *)wm_hal_dma_user_isr_dummy;

    WM_HAL_DMA_LOG_I("dma unregister callback for ch:%d", ch);

    wm_ll_dma_set_int_mask(dma_dev->reg_base,
                           wm_ll_dma_get_int_mask(dma_dev->reg_base) | (WM_BIT(ch * WM_DMA_INT_MAX + WM_DMA_INT_XFER_DONE)));

    return ret;
}

wm_hal_dma_status_t wm_hal_dma_config(wm_hal_dma_dev_t *dma_dev, wm_hal_dma_desc_t *dma_desc)
{
    wm_dma_reg_t *reg_base  = dma_dev->reg_base;
    wm_dma_ch_t ch          = dma_desc->extend_ctrl.ch;
    wm_hal_dma_status_t ret = WM_HAL_DMA_STATUS_SUCCESS;

    //TODO, sanity check for more param
    WM_HAL_DMA_ASSERT(ch >= WM_DMA_CH_0 && ch < WM_DMA_CH_MAX);

    /* config DMA HW */
    WM_HAL_DMA_MEM_ZERO((void *)&reg_base->dma_ch[ch], sizeof(wm_dma_ch_reg_t));
    if (dma_desc->extend_ctrl.warp_mode_en) {
        wm_ll_dma_set_src_warp_addr(reg_base, dma_desc->src_addr, ch);
        wm_ll_dma_set_dest_warp_addr(reg_base, dma_desc->dest_addr, ch);
        wm_ll_dma_set_src_warp_len(reg_base, dma_desc->warp_info.src_warp_len, ch);
        wm_ll_dma_set_dest_warp_len(reg_base, dma_desc->warp_info.dest_warp_len, ch);
    } else {
        wm_ll_dma_set_src_addr(reg_base, dma_desc->src_addr, ch);
        wm_ll_dma_set_dest_addr(reg_base, dma_desc->dest_addr, ch);
    }
    wm_ll_dma_set_dma_mode(reg_base, dma_desc->extend_ctrl.mode, ch);
    wm_ll_dma_set_dma_select(reg_base, dma_desc->extend_ctrl.req_sel, ch);
    wm_ll_dma_set_dma_chain_mode(reg_base, dma_desc->extend_ctrl.chain_mode, ch);
    wm_ll_dma_set_chain_link(reg_base, dma_desc->extend_ctrl.chain_mode_en, ch);
    wm_ll_dma_set_uart_ch_sel(reg_base, wm_ll_dma_get_uart_ch_sel(reg_base) | dma_desc->extend_ctrl.uart_dma_ch);

    if (dma_desc->extend_ctrl.chain_mode == WM_DMA_CHAIN_MODE_LIST) {
        dma_dev->desc_list = dma_desc;
        /* from DMA RTL, DMA HW will move desc.ctrl[22:0] to reg.ctrl[23:1], SW need transfer first */
        /* add wm_hal_dma_ctrl_t union, do not shift */
        /* while (desc_list != NULL) {
            desc_list->ctrl.val = (desc_list->ctrl.val >> 0x1);
            // TODO, delete when all other IP test finish
            WM_HAL_DMA_LOG_DUMP(WM_LOG_LEVEL_INFO,"dma cfg desc list:", WM_HAL_DMA_DUMP_WIDTH, (void *)desc_list, sizeof(wm_hal_dma_desc_t));
            desc_list = desc_list->next;
        } */
        wm_ll_dma_set_desc_addr(reg_base, (uint32_t)dma_desc, ch);
    } else {
        wm_ll_dma_set_xfer_uint(reg_base, dma_desc->ctrl.nonchain.data_unit, ch);
        wm_ll_dma_set_burst_uint(reg_base, dma_desc->ctrl.nonchain.burst_size, ch);
        wm_ll_dma_set_src_addr_inc_mode(reg_base, dma_desc->ctrl.nonchain.src_addr_inc, ch);
        wm_ll_dma_set_dest_addr_inc_mode(reg_base, dma_desc->ctrl.nonchain.dest_addr_inc, ch);
        wm_ll_dma_set_xfer_bytes(reg_base, dma_desc->ctrl.nonchain.len, ch);
        wm_ll_dma_set_auto_reload(reg_base, dma_desc->ctrl.nonchain.auto_reload, ch);
    }

    /* interrupt control */
    if (dma_desc->extend_ctrl.int_en) {
        wm_ll_dma_set_int_mask(dma_dev->reg_base, wm_ll_dma_get_int_mask(dma_dev->reg_base) &
                                                      ~(WM_BIT(ch * WM_DMA_INT_MAX + dma_desc->extend_ctrl.int_type)));
        if (ch >= WM_DMA_CH_0 && ch <= WM_DMA_CH_3) {
            dma_dev->irq_priv[wm_hal_dma_get_ch2irq(ch)]->ch_use.ch = ch;
        } else {
            dma_dev->irq_priv[wm_hal_dma_get_ch2irq(ch)]->ch_use.ch_map |= WM_BIT(ch - WM_DMA_CH_4);
        }

        WM_HAL_DMA_LOG_I("int:0x%x, ch:0x%x", wm_ll_dma_get_int_mask(dma_dev->reg_base),
                         dma_dev->irq_priv[wm_hal_dma_get_ch2irq(ch)]->ch_use.ch);
        wm_hal_irq_enable(dma_dev->irq_num + wm_hal_dma_get_ch2irq(ch));
    } else {
        wm_ll_dma_set_int_mask(dma_dev->reg_base, wm_ll_dma_get_int_mask(dma_dev->reg_base) |
                                                      WM_BIT(ch * WM_DMA_INT_MAX + dma_desc->extend_ctrl.int_type));
        if (ch >= WM_DMA_CH_0 && ch <= WM_DMA_CH_3) {
            dma_dev->irq_priv[wm_hal_dma_get_ch2irq(ch)]->ch_use.ch = ch;
        } else {
            dma_dev->irq_priv[wm_hal_dma_get_ch2irq(ch)]->ch_use.ch_map &= ~(WM_BIT(ch - WM_DMA_CH_4));
        }

        WM_HAL_DMA_LOG_I("int:0x%x, ch:0x%x", wm_ll_dma_get_int_mask(dma_dev->reg_base),
                         dma_dev->irq_priv[wm_hal_dma_get_ch2irq(ch)]->ch_use.ch);
        wm_hal_irq_disable(dma_dev->irq_num + wm_hal_dma_get_ch2irq(ch));
    }

    WM_HAL_DMA_LOG_DUMP(WM_LOG_LEVEL_INFO, "dma cfg desc:", WM_HAL_DMA_DUMP_WIDTH, (void *)dma_desc, sizeof(wm_hal_dma_desc_t));
    /* TODO, delete, now just for debug */
    {
        WM_HAL_DMA_LOG_I("ctrl:0x%x", reg_base->dma_ch[ch].ctrl.val);
        WM_HAL_DMA_LOG_DUMP(WM_LOG_LEVEL_INFO, "cfged reg:", WM_HAL_DMA_DUMP_WIDTH, (void *)&reg_base->dma_ch[ch],
                            sizeof(wm_dma_ch_reg_t));
        WM_HAL_DMA_LOG_DUMP(WM_LOG_LEVEL_INFO, "cfged reg common:", WM_HAL_DMA_DUMP_WIDTH, (void *)&reg_base->int_msk, 0xf);
    }
    return ret;
}

wm_hal_dma_status_t wm_hal_dma_config_partial(wm_hal_dma_dev_t *dma_dev, wm_hal_dma_cfg_partial_desc_t *partial_desc)
{
    wm_dma_reg_t *reg_base  = dma_dev->reg_base;
    wm_dma_ch_t ch          = partial_desc->ch;
    wm_hal_dma_status_t ret = WM_HAL_DMA_STATUS_SUCCESS;

    WM_HAL_DMA_ASSERT(ch >= WM_DMA_CH_0 && ch < WM_DMA_CH_MAX);

    /* reload DMA HW */
    wm_ll_dma_set_src_addr(reg_base, partial_desc->src_addr, ch);
    wm_ll_dma_set_dest_addr(reg_base, partial_desc->dest_addr, ch);
    wm_ll_dma_set_xfer_bytes(reg_base, partial_desc->len, ch);

    // can set log level to disable this logging
    WM_HAL_DMA_LOG_DUMP(WM_LOG_LEVEL_INFO, "after partial config, reg:", WM_HAL_DMA_DUMP_WIDTH, (void *)&reg_base->dma_ch[ch],
                        sizeof(wm_dma_ch_reg_t));

    return ret;
}

wm_hal_dma_status_t wm_hal_dma_start(wm_hal_dma_dev_t *dma_dev, wm_dma_ch_t ch)
{
    wm_hal_dma_status_t ret = WM_HAL_DMA_STATUS_SUCCESS;

    WM_HAL_DMA_ASSERT(ch >= WM_DMA_CH_0 && ch < WM_DMA_CH_MAX);
    wm_ll_dma_set_dma_start(dma_dev->reg_base, WM_DMA_START_EN, ch);

    WM_HAL_DMA_LOG_I("dma start for ch:%d", ch);

    return ret;
}

wm_hal_dma_status_t wm_hal_dma_stop(wm_hal_dma_dev_t *dma_dev, wm_dma_ch_t ch)
{
    wm_hal_dma_status_t ret = WM_HAL_DMA_STATUS_SUCCESS;

    WM_HAL_DMA_ASSERT(ch >= WM_DMA_CH_0 && ch < WM_DMA_CH_MAX);
    wm_ll_dma_set_dma_stop(dma_dev->reg_base, WM_DMA_STOP_EN, ch);

    //TODO, use GPT timer for timeout control
    do {
        ;
    } while (wm_ll_dma_get_dma_start(dma_dev->reg_base, ch));

    WM_HAL_DMA_LOG_I("dma stoped for ch:%d", ch);

    return ret;
}

wm_hal_dma_status_t wm_hal_dma_get_status(wm_hal_dma_dev_t *dma_dev, wm_dma_ch_t ch, wm_dma_sts_info_t *dma_sts)
{
    wm_hal_dma_status_t ret = WM_HAL_DMA_STATUS_SUCCESS;

    WM_HAL_DMA_ASSERT(ch >= WM_DMA_CH_0 && ch < WM_DMA_CH_MAX);

    (wm_ll_dma_get_dma_start(dma_dev->reg_base, ch) == WM_DMA_START_EN) ? (dma_sts->sts = WM_DMA_RUNNIG) :
                                                                          (dma_sts->sts = WM_DMA_IDLE);
    dma_sts->xfer_cnt = wm_ll_dma_get_xfer_cnt(dma_dev->reg_base, ch);

    wm_ll_dma_set_int_sts(dma_dev->reg_base, WM_BIT(ch * WM_DMA_INT_MAX + WM_DMA_INT_BURST_DONE) |
                                                 WM_BIT(ch * WM_DMA_INT_MAX + WM_DMA_INT_XFER_DONE));

    WM_HAL_DMA_LOG_I("dma status:0x%x, cnt:%d", dma_sts->sts, dma_sts->xfer_cnt);

    return ret;
}

wm_hal_dma_status_t wm_hal_dma_request_ch(wm_hal_dma_dev_t *dma_dev, uint8_t *ch)
{
    wm_hal_dma_status_t ret       = WM_HAL_DMA_STATUS_SUCCESS;
    wm_hal_dma_ch_list_t *ch_list = NULL;

    WM_HAL_DMA_ENTER_CRITIAL;
    /* method 1: list */
    /* wm double list not pop operation(signle list have pop), need use first + delete*/
    ch_list = (wm_hal_dma_ch_list_t *)WM_HAL_DMA_LIST_FIRST(&dma_dev->ch_list, struct wm_hal_dma_ch_list_s, list);
    if (ch_list) {
        WM_HAL_DMA_LIST_DEL(&ch_list->list);
        WM_HAL_DMA_ASSERT(ch_list->ch >= WM_DMA_CH_0 && ch_list->ch < WM_DMA_CH_MAX);
        *ch = ch_list->ch;
    } else {
        ret = WM_HAL_DMA_STATUS_RESOURCE_LACK;
    }

    /* method 2: clz */
    /* TBD, efficient may not better than list operation, still need critical protection
    clz = __builtin_clz(ch_free);
    *ch = WM_HAL_DMA_U32_MSB_POS - clz;
    ch_free &= ~WM_BIT(*ch); */

    /* method 3 */
    /*if (dma_dev->dma_ch < WM_DMA_CH_MAX) {
        *ch = dma_dev->dma_ch++;
        if (dma_dev->dma_ch > WM_DMA_CH_7) {
            dma_dev->dma_ch = WM_DMA_CH_7;
        }
        WM_HAL_DMA_ASSERT(*ch < WM_DMA_CH_MAX);
    } else {
        *ch = WM_DMA_CH_MAX;
        ret = WM_HAL_DMA_STATUS_RESOURCE_LACK;
    }*/
    WM_HAL_DMA_EXIT_CRITIAL;

    WM_HAL_DMA_LOG_I("alloc dma ch:%d with ret:%d", *ch, ret);

    return ret;
}

wm_hal_dma_status_t wm_hal_dma_release_ch(wm_hal_dma_dev_t *dma_dev, uint8_t ch)
{
    wm_hal_dma_status_t ret       = WM_HAL_DMA_STATUS_SUCCESS;
    wm_hal_dma_ch_list_t *ch_list = NULL;

    WM_HAL_DMA_ASSERT(ch >= WM_DMA_CH_0 && ch < WM_DMA_CH_MAX);
    ch_list = wm_hal_dma_get_ch_list();

    WM_HAL_DMA_ENTER_CRITIAL;
    /* method 1 */
    WM_HAL_DMA_LIST_PUSH(&dma_dev->ch_list, &ch_list[ch].list);
    /* method 3 */
    /*if (dma_dev->dma_ch > WM_DMA_CH_0) {
        dma_dev->dma_ch--;
    }
    WM_HAL_DMA_ASSERT(dma_dev->dma_ch >= WM_DMA_CH_0 && dma_dev->dma_ch < WM_DMA_CH_MAX);*/
    WM_HAL_DMA_EXIT_CRITIAL;

    WM_HAL_DMA_LOG_I("free dma ch:%d", ch);

    return ret;
}

wm_hal_dma_status_t wm_hal_dma_clr_req(wm_hal_dma_dev_t *dma_dev, wm_dma_req_sel_t req_sel)
{
    wm_hal_dma_status_t ret = WM_HAL_DMA_STATUS_SUCCESS;

    WM_HAL_DMA_ASSERT(req_sel >= WM_DMA_UART_RX_REQ && req_sel < WM_DMA_REQ_MAX);

    wm_ll_dma_set_req_clr(dma_dev->reg_base, wm_ll_dma_get_req_clr(dma_dev->reg_base) | WM_BIT(req_sel));

    WM_HAL_DMA_LOG_I("clr req:%d", req_sel);

    return ret;
}

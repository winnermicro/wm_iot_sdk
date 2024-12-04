/**
 * @file wm_ll_dma.h
 *
 * @brief dma low level operation
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

#ifndef __WM_LL_DMA_H__
#define __WM_LL_DMA_H__

#include "wm_attr.h"
#include "wm_types.h"
#include "wm_dma_reg_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief          get dma channel interrupt mask
 * @param[in]      dma hardware register base address
 * @return         interrupt mask
 */
ATTRIBUTE_INLINE uint32_t wm_ll_dma_get_int_mask(wm_dma_reg_t *reg)
{
    return reg->int_msk.val;
}

/**
 * @brief          set dma channel interrupt mask
 * @param[in]      dma hardware register base address
 * @param[in]      interrupt mask
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_dma_set_int_mask(wm_dma_reg_t *reg, uint32_t mask)
{
    reg->int_msk.val = mask;
}

/**
 * @brief          get dma interrupt status
 * @param[in]      dma hardware register base address
 * @return         int status
 */
ATTRIBUTE_INLINE uint32_t wm_ll_dma_get_int_sts(wm_dma_reg_t *reg)
{
    return reg->int_sts.val;
}

/**
 * @brief          set dma interrupt status
 * @param[in]      dma hardware register base address
 * @param[in]      interrupt status
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_dma_set_int_sts(wm_dma_reg_t *reg, uint32_t sts)
{
    reg->int_sts.val = sts;
}

/**
 * @brief          get uart cmd ch select
 * @param[in]      dma hardware register base address
 * @param[in]      uart dma channel
 * @return         uart dma channel select
 */
ATTRIBUTE_INLINE uint32_t wm_ll_dma_get_uart_ch_sel(wm_dma_reg_t *reg)
{
    return reg->uart_ch.uart_ch_sel;
}

/**
 * @brief          set uart dma ch select
 * @param[in]      dma hardware register base address
 * @param[in]      uart dma channel
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_dma_set_uart_ch_sel(wm_dma_reg_t *reg, uint32_t uart_ch)
{
    reg->uart_ch.uart_ch_sel = uart_ch;
}

/**
 * @brief          get dma req clear
 * @param[in]      dma hardware register base address
 * @return         dma request clear
 */
ATTRIBUTE_INLINE uint32_t wm_ll_dma_get_req_clr(wm_dma_reg_t *reg)
{
    return WM_LL_REG_FILED_COPY_READ(reg->uart_ch, req_clear);
}

/**
 * @brief          set dma req clear
 * @param[in]      dma hardware register base address
 * @param[in]      dma request clear
 */
ATTRIBUTE_INLINE void wm_ll_dma_set_req_clr(wm_dma_reg_t *reg, uint32_t req_clr)
{
    WM_LL_REG_FILED_COPY_WRTIE(reg->uart_ch, req_clear, req_clr);
}

/**
 * @brief          get dma src addr
 * @param[in]      dma hardware register base address
 * @param[in]      dma channel id
 * @return         dma transfer src addr
 */
ATTRIBUTE_INLINE uint32_t wm_ll_dma_get_src_addr(wm_dma_reg_t *reg, uint32_t ch)
{
    return reg->dma_ch[ch].src_addr;
}

/**
 * @brief          set dma src addr
 * @param[in]      dma hardware register base address
 * @param[in]      dma transfer src addr
 * @param[in]      dma channel id
 */
ATTRIBUTE_INLINE void wm_ll_dma_set_src_addr(wm_dma_reg_t *reg, uint32_t src_addr, uint32_t ch)
{
    reg->dma_ch[ch].src_addr = src_addr;
}

/**
 * @brief          get dma dest addr
 * @param[in]      dma hardware register base address
 * @param[in]      dma channel id
 * @return         dma transfer dest addr
 */
ATTRIBUTE_INLINE uint32_t wm_ll_dma_get_dest_addr(wm_dma_reg_t *reg, uint32_t ch)
{
    return reg->dma_ch[ch].dest_addr;
}

/**
 * @brief          set dma dest addr
 * @param[in]      dma hardware register base address
 * @param[in]      dma transfer dest addr
 * @param[in]      dma channel id
 */
ATTRIBUTE_INLINE void wm_ll_dma_set_dest_addr(wm_dma_reg_t *reg, uint32_t dest_addr, uint32_t ch)
{
    reg->dma_ch[ch].dest_addr = dest_addr;
}

/**
 * @brief          get dma src warp addr
 * @param[in]      dma hardware register base address
 * @param[in]      dma channel id
 * @return         dma transfer src warp addr
 */
ATTRIBUTE_INLINE uint32_t wm_ll_dma_get_src_warp_addr(wm_dma_reg_t *reg, uint32_t ch)
{
    return reg->dma_ch[ch].src_warp_addr;
}

/**
 * @brief          set dma src warp addr
 * @param[in]      dma hardware register base address
 * @param[in]      dma transfer src warp addr
 * @param[in]      dma channel id
 */
ATTRIBUTE_INLINE void wm_ll_dma_set_src_warp_addr(wm_dma_reg_t *reg, uint32_t src_warp_addr, uint32_t ch)
{
    reg->dma_ch[ch].src_warp_addr = src_warp_addr;
}

/**
 * @brief          get dma dest warp addr
 * @param[in]      dma hardware register base address
 * @param[in]      dma channel id
 * @return         dma transfer dest warp addr
 */
ATTRIBUTE_INLINE uint32_t wm_ll_dma_get_dest_warp_addr(wm_dma_reg_t *reg, uint32_t ch)
{
    return reg->dma_ch[ch].dest_warp_addr;
}

/**
 * @brief          set dma dest warp addr
 * @param[in]      dma hardware register base address
 * @param[in]      dma transfer dest warp addr
 * @param[in]      dma channel id
 */
ATTRIBUTE_INLINE void wm_ll_dma_set_dest_warp_addr(wm_dma_reg_t *reg, uint32_t dest_warp_addr, uint32_t ch)
{
    reg->dma_ch[ch].dest_warp_addr = dest_warp_addr;
}

/**
 * @brief          get dma src wap len
 * @param[in]      dma hardware register base address
 * @param[in]      dma channel id
 * @return         dma transfer src warp len
 */
ATTRIBUTE_INLINE uint32_t wm_ll_dma_get_src_warp_len(wm_dma_reg_t *reg, uint32_t ch)
{
    return WM_LL_REG_FILED_COPY_READ(reg->dma_ch[ch].warp_size, src_addr_len);
}

/**
 * @brief          set dma src wap len
 * @param[in]      dma hardware register base address
 * @param[in]      dma transfer src warp len
 * @param[in]      dma channel id
 */
ATTRIBUTE_INLINE void wm_ll_dma_set_src_warp_len(wm_dma_reg_t *reg, uint32_t src_warp_len, uint32_t ch)
{
    WM_LL_REG_FILED_COPY_WRTIE(reg->dma_ch[ch].warp_size, src_addr_len, src_warp_len);
}

/**
 * @brief          get dma dest wap len
 * @param[in]      dma hardware register base address
 * @param[in]      dma channel id
 * @return         dma transfer desc warp len
 */
ATTRIBUTE_INLINE uint32_t wm_ll_dma_get_dest_warp_len(wm_dma_reg_t *reg, uint32_t ch)
{
    return WM_LL_REG_FILED_COPY_READ(reg->dma_ch[ch].warp_size, dest_addr_len);
}

/**
 * @brief          set dma dest wap len
 * @param[in]      dma hardware register base address
 * @param[in]      dma transfer desc warp len
 * @param[in]      dma channel id
 */
ATTRIBUTE_INLINE void wm_ll_dma_set_dest_warp_len(wm_dma_reg_t *reg, uint32_t dest_warp_len, uint32_t ch)
{
    WM_LL_REG_FILED_COPY_WRTIE(reg->dma_ch[ch].warp_size, dest_addr_len, dest_warp_len);
}

/**
 * @brief          get dma start
 * @param[in]      dma hardware register base address
 * @param[in]      dma channel id
 * @return         dma start
 */
ATTRIBUTE_INLINE uint32_t wm_ll_dma_get_dma_start(wm_dma_reg_t *reg, uint32_t ch)
{
    return reg->dma_ch[ch].ch_ctrl.dma_start;
}

/**
 * @brief          set dma start
 * @param[in]      dma hardware register base address
 * @param[in]      dma start cfg
 * @param[in]      dma channel id
 */
ATTRIBUTE_INLINE void wm_ll_dma_set_dma_start(wm_dma_reg_t *reg, uint32_t dma_start, uint32_t ch)
{
    reg->dma_ch[ch].ch_ctrl.dma_start = dma_start;
}

/**
 * @brief          get dma stop
 * @param[in]      dma hardware register base address
 * @param[in]      dma channel id
 * @return         dma stop
 */
ATTRIBUTE_INLINE uint32_t wm_ll_dma_get_dma_stop(wm_dma_reg_t *reg, uint32_t ch)
{
    return reg->dma_ch[ch].ch_ctrl.dma_stop;
}

/**
 * @brief          set dma stop
 * @param[in]      dma hardware register base address
 * @param[in]      dma stop cfg
 * @param[in]      dma channel id
 */
ATTRIBUTE_INLINE void wm_ll_dma_set_dma_stop(wm_dma_reg_t *reg, uint32_t dma_stop, uint32_t ch)
{
    reg->dma_ch[ch].ch_ctrl.dma_stop = dma_stop;
}

/**
 * @brief          get dma mode
 * @param[in]      dma hardware register base address
 * @param[in]      dma channel id
 * @return         dma mode
 */
ATTRIBUTE_INLINE uint32_t wm_ll_dma_get_dma_mode(wm_dma_reg_t *reg, uint32_t ch)
{
    return reg->dma_ch[ch].mode.dma_mode;
}

/**
 * @brief          set dma mode
 * @param[in]      dma hardware register base address
 * @param[in]      dma mode
 * @param[in]      dma channel id
 */
ATTRIBUTE_INLINE void wm_ll_dma_set_dma_mode(wm_dma_reg_t *reg, uint32_t mode, uint32_t ch)
{
    reg->dma_ch[ch].mode.dma_mode = mode;
}

/**
 * @brief          get dma chain mode
 * @param[in]      dma hardware register base address
 * @param[in]      dma channel id
 * @return         dma chain mode
 */
ATTRIBUTE_INLINE uint32_t wm_ll_dma_get_dma_chain_mode(wm_dma_reg_t *reg, uint32_t ch)
{
    return reg->dma_ch[ch].mode.chain_mode;
}

/**
 * @brief          set dma chain mode
 * @param[in]      dma hardware register base address
 * @param[in]      dma chain mode
 * @param[in]      dma channel id
 */
ATTRIBUTE_INLINE void wm_ll_dma_set_dma_chain_mode(wm_dma_reg_t *reg, uint32_t chain_mode, uint32_t ch)
{
    reg->dma_ch[ch].mode.chain_mode = chain_mode;
}

/**
 * @brief          get dma sel
 * @param[in]      dma hardware register base address
 * @param[in]      dma channel id
 * @return         dma select id
 */
ATTRIBUTE_INLINE uint32_t wm_ll_dma_get_dma_select(wm_dma_reg_t *reg, uint32_t ch)
{
    return reg->dma_ch[ch].mode.dma_sel;
}

/**
 * @brief          set dma sel
 * @param[in]      dma hardware register base address
 * @param[in]      dma select id
 * @param[in]      dma channel id
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_dma_set_dma_select(wm_dma_reg_t *reg, uint32_t sel_id, uint32_t ch)
{
    reg->dma_ch[ch].mode.dma_sel = sel_id;
}

/**
 * @brief          get dma chain link
 * @param[in]      dma hardware register base address
 * @param[in]      dma channel id
 * @return         dma chain link
 */
ATTRIBUTE_INLINE uint32_t wm_ll_dma_get_chain_link(wm_dma_reg_t *reg, uint32_t ch)
{
    return reg->dma_ch[ch].mode.chain_link_en;
}

/**
 * @brief          set dma chain link
 * @param[in]      dma hardware register base address
 * @param[in]      dma chain link cfg
 * @param[in]      dma channel id
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_dma_set_chain_link(wm_dma_reg_t *reg, uint32_t chain_link, uint32_t ch)
{
    reg->dma_ch[ch].mode.chain_link_en = chain_link;
}

/**
 * @brief          get dma auto reload
 * @param[in]      dma hardware register base address
 * @param[in]      dma channel id
 * @return         dma auto reload
 */
ATTRIBUTE_INLINE uint32_t wm_ll_dma_get_auto_reload(wm_dma_reg_t *reg, uint32_t ch)
{
    return reg->dma_ch[ch].ctrl.auto_reload;
}

/**
 * @brief          set dma auto reload
 * @param[in]      dma hardware register base address
 * @param[in]      dma auto reload cfg
 * @param[in]      dma channel id
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_dma_set_auto_reload(wm_dma_reg_t *reg, uint32_t auto_reload, uint32_t ch)
{
    reg->dma_ch[ch].ctrl.auto_reload = auto_reload;
}

/**
 * @brief          get dma src addr inc mode
 * @param[in]      dma hardware register base address
 * @param[in]      dma channel id
 * @return         dma src addr inc mode
 */
ATTRIBUTE_INLINE uint32_t wm_ll_dma_get_src_addr_inc_mode(wm_dma_reg_t *reg, uint32_t ch)
{
    return reg->dma_ch[ch].ctrl.src_addr_inc;
}

/**
 * @brief          set dma src addr inc mode
 * @param[in]      dma hardware register base address
 * @param[in]      dma src addr inc mode
 * @param[in]      dma channel id
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_dma_set_src_addr_inc_mode(wm_dma_reg_t *reg, uint32_t src_addr_inc, uint32_t ch)
{
    reg->dma_ch[ch].ctrl.src_addr_inc = src_addr_inc;
}

/**
 * @brief          get dma dest addr inc mode
 * @param[in]      dma hardware register base address
 * @param[in]      dma channel id
 * @return         dma dest addr inc mode
 */
ATTRIBUTE_INLINE uint32_t wm_ll_dma_get_dest_addr_inc_mode(wm_dma_reg_t *reg, uint32_t ch)
{
    return reg->dma_ch[ch].ctrl.dest_addr_inc;
}

/**
 * @brief          set dma dest addr inc mode
 * @param[in]      dma hardware register base address
 * @param[in]      dma dest addr inc mode
 * @param[in]      dma channel id
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_dma_set_dest_addr_inc_mode(wm_dma_reg_t *reg, uint32_t dest_addr_inc, uint32_t ch)
{
    reg->dma_ch[ch].ctrl.dest_addr_inc = dest_addr_inc;
}

/**
 * @brief          get dma transfer uint
 * @param[in]      dma hardware register base address
 * @param[in]      dma channel id
 * @return         dma transfer uints
 */
ATTRIBUTE_INLINE uint32_t wm_ll_dma_get_xfer_uint(wm_dma_reg_t *reg, uint32_t ch)
{
    return reg->dma_ch[ch].ctrl.data_uint;
}

/**
 * @brief          set dma transfer uint
 * @param[in]      dma hardware register base address
 * @param[in]      dma transfer uints
 * @param[in]      dma channel id
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_dma_set_xfer_uint(wm_dma_reg_t *reg, uint32_t xfer_uint, uint32_t ch)
{
    reg->dma_ch[ch].ctrl.data_uint = xfer_uint;
}

/**
 * @brief          get dma burst size
 * @param[in]      dma hardware register base address
 * @param[in]      dma channel id
 * @return         dma burst size
 */
ATTRIBUTE_INLINE uint32_t wm_ll_dma_get_burst_uint(wm_dma_reg_t *reg, uint32_t ch)
{
    return reg->dma_ch[ch].ctrl.burst_size;
}

/**
 * @brief          set dma burst size
 * @param[in]      dma hardware register base address
 * @param[in]      dma burst size
 * @param[in]      dma channel id
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_dma_set_burst_uint(wm_dma_reg_t *reg, uint32_t burst_size, uint32_t ch)
{
    reg->dma_ch[ch].ctrl.burst_size = burst_size;
}

/**
 * @brief          get dma transfer total bytes
 * @param[in]      dma hardware register base address
 * @param[in]      dma channel id
 * @return         dma total bytes
 */
ATTRIBUTE_INLINE uint32_t wm_ll_dma_get_xfer_bytes(wm_dma_reg_t *reg, uint32_t ch)
{
    return WM_LL_REG_FILED_COPY_READ(reg->dma_ch[ch].ctrl, total_bytes);
}

/**
 * @brief          set dma transfer total bytes
 * @param[in]      dma hardware register base address
 * @param[in]      dma total bytes
 * @param[in]      dma channel id
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_dma_set_xfer_bytes(wm_dma_reg_t *reg, uint32_t xfer_bytes, uint32_t ch)
{
    WM_LL_REG_FILED_COPY_WRTIE(reg->dma_ch[ch].ctrl, total_bytes, xfer_bytes);
}

/**
 * @brief          get dma transfer total bytes
 * @param[in]      dma hardware register base address
 * @param[in]      dma channel id
 * @return         transfter total cnt
 */
ATTRIBUTE_INLINE uint32_t wm_ll_dma_get_xfer_cnt(wm_dma_reg_t *reg, uint32_t ch)
{
    return WM_LL_REG_FILED_COPY_READ(reg->dma_ch[ch].status, xfer_cnt);
}

/**
 * @brief          get dma desc addr
 * @param[in]      dma hardware register base address
 * @param[in]      dma channel id
 * @return         dma transfer desc addr
 */
ATTRIBUTE_INLINE uint32_t wm_ll_dma_get_desc_addr(wm_dma_reg_t *reg, uint32_t ch)
{
    return reg->dma_ch[ch].desc_addr;
}

/**
 * @brief          set dma desc addr
 * @param[in]      dma hardware register base address
 * @param[in]      dma transfer desc addr
 * @param[in]      dma channel id
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_dma_set_desc_addr(wm_dma_reg_t *reg, uint32_t desc_addr, uint32_t ch)
{
    reg->dma_ch[ch].desc_addr = desc_addr;
}

/**
 * @brief          get dma current dest addr
 * @param[in]      dma hardware register base address
 * @param[in]      dma channel id
 * @return         current destnation address
 */
ATTRIBUTE_INLINE uint32_t wm_ll_dma_get_current_dest_addr(wm_dma_reg_t *reg, uint32_t ch)
{
    return reg->dma_ch[ch].current_dest_addr;
}

#ifdef __cplusplus
}
#endif

#endif /* __WM_LL_DMA_H__ */

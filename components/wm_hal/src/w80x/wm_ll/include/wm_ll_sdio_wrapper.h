/**
 * @file wm_ll_sdio_wrapper.h
 *
 * @brief sdio wrapper low level operation
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

#ifndef __WM_LL_SDIO_WRAPPER_H__
#define __WM_LL_SDIO_WRAPPER_H__

#include "wm_attr.h"
#include "wm_types.h"
#include "wm_sdio_wrapper_reg_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief          get wrapper interrupt status
 * @param[in]      sdio wrapper hardware register base address
 * @return         interrupt status
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdio_wrapper_get_int_sts(wm_sdio_wrapper_reg_t *reg)
{
    return reg->int_sts.val;
}

/**
 * @brief          set wrapper interrupt status
 * @param[in]      sdio wrapper hardware register base address
 * @param[in]      interrupt status
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_sdio_wrapper_set_int_sts(wm_sdio_wrapper_reg_t *reg, uint32_t sts)
{
    reg->int_sts.val = sts;
}

/**
 * @brief          get wrapper interrupt mask
 * @param[in]      sdio wrapper hardware register base address
 * @return         interrupt mask
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdio_wrapper_get_int_mask(wm_sdio_wrapper_reg_t *reg)
{
    return reg->int_msk.val;
}

/**
 * @brief          set wrapper interrupt mask
 * @param[in]      sdio wrapper hardware register base address
 * @param[in]      interrupt mask
 */
ATTRIBUTE_INLINE void wm_ll_sdio_wrapper_set_int_mask(wm_sdio_wrapper_reg_t *reg, uint32_t mask)
{
    reg->int_msk.val = mask;
}

/**
 * @brief          get wrapper up cmd avalibale
 * @param[in]      sdio wrapper hardware register base address
 * @return         wrapper up cmd avalibale
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdio_wrapper_get_up_cmd_aval(wm_sdio_wrapper_reg_t *reg)
{
    return reg->up_cmd_avl.cmd_avl;
}

/**
 * @brief          set wrapper up cmd avalibale
 * @param[in]      sdio wrapper hardware register base address
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_sdio_wrapper_set_up_cmd_aval(wm_sdio_wrapper_reg_t *reg, uint32_t cmd_avl)
{
    reg->up_cmd_avl.cmd_avl = cmd_avl;
}

/**
 * @brief          get wrapper down cmd buf avalibale
 * @param[in]      sdio wrapper hardware register base address
 * @return         wrapper down cmd buf avalibale
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdio_wrapper_get_down_cmd_buf_aval(wm_sdio_wrapper_reg_t *reg)
{
    return reg->dm_cmdb_avl.cmdb_avl;
}

/**
 * @brief          wrapper down cmd buf avalibale
 * @param[in]      sdio wrapper hardware register base address
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_sdio_wrapper_set_down_cmd_buf_aval(wm_sdio_wrapper_reg_t *reg, uint32_t cmdb_avl)
{
    reg->dm_cmdb_avl.cmdb_avl = cmdb_avl;
}

/**
 * @brief          get wrapper sdio tx link enable
 * @param[in]      sdio wrapper hardware register base address
 * @return         wrapper sdio tx link
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdio_wrapper_get_tx_link(wm_sdio_wrapper_reg_t *reg)
{
    return reg->tx_link_en.txl_en;
}

/**
 * @brief          set wrapper sdio tx link enable
 * @param[in]      sdio wrapper hardware register base address
 * @param[in]      sdio wrapper tx link en
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_sdio_wrapper_set_tx_link(wm_sdio_wrapper_reg_t *reg, uint32_t txl_en)
{
    reg->tx_link_en.txl_en = txl_en;
}

/**
 * @brief          get wrapper sdio tx link address
 * @param[in]      sdio wrapper hardware register base address
 * @return         sdio wrapper tx link address
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdio_wrapper_get_tx_link_addr(wm_sdio_wrapper_reg_t *reg)
{
    return reg->tx_link_addr.tx_addr;
}

/**
 * @brief          set wrapper sdio tx link address
 * @param[in]      sdio wrapper hardware register base address
 * @param[in]      sdio wrapper tx link address
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_sdio_wrapper_set_tx_link_addr(wm_sdio_wrapper_reg_t *reg, uint32_t tx_link_addr)
{
    reg->tx_link_addr.tx_addr = tx_link_addr;
}

/**
 * @brief          get wrapper sdio tx en
 * @param[in]      sdio wrapper hardware register base address
 * @return         tx link en
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdio_wrapper_get_tx_en(wm_sdio_wrapper_reg_t *reg)
{
    return reg->tx_start.tx_en;
}
/**
 * @brief          set wrapper sdio tx en
 * @param[in]      sdio wrapper hardware register base address
 * @param[in]      tx link en
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_sdio_wrapper_set_tx_en(wm_sdio_wrapper_reg_t *reg, uint32_t tx_en)
{
    reg->tx_start.tx_en = tx_en;
}

/**
 * @brief          get wrapper sdio tx status
 * @param[in]      sdio wrapper hardware register base address
 * @return         tx status
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdio_wrapper_get_tx_status(wm_sdio_wrapper_reg_t *reg)
{
    return reg->tx_sts.tx_sts;
}

/**
 * @brief          get wrapper sdio rx link
 * @param[in]      sdio wrapper hardware register base address
 * @return         txlink
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdio_wrapper_get_rx_link(wm_sdio_wrapper_reg_t *reg)
{
    return reg->rx_link_en.rxl_en;
}

/**
 * @brief          set wrapper sdio rx link
 * @param[in]      sdio wrapper hardware register base address
 * @param[in]      rx link en
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_sdio_wrapper_set_rx_link(wm_sdio_wrapper_reg_t *reg, uint32_t rxl_en)
{
    reg->rx_link_en.rxl_en = rxl_en;
}

/**
 * @brief          get wrapper sdio rx link address
 * @param[in]      sdio wrapper hardware register base address
 * @return         sdio wrapper rx link address
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdio_wrapper_get_rx_link_addr(wm_sdio_wrapper_reg_t *reg)
{
    return reg->rx_link_addr.rx_addr;
}

/**
 * @brief          set wrapper sdio rx link address
 * @param[in]      sdio wrapper hardware register base address
 * @param[in]      sdio wrapper rx link address
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_sdio_wrapper_set_rx_link_addr(wm_sdio_wrapper_reg_t *reg, uint32_t rx_link_addr)
{
    reg->rx_link_addr.rx_addr = rx_link_addr;
}

/**
 * @brief          get wrapper sdio rx en
 * @param[in]      sdio wrapper hardware register base address
 * @return         rx en
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdio_wrapper_get_rx_en(wm_sdio_wrapper_reg_t *reg)
{
    return reg->rx_start.rx_en;
}

/**
 * @brief          set wrapper sdio rx en
 * @param[in]      sdio wrapper hardware register base address
 * @param[in]      sdio wrapper rx en
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_sdio_wrapper_set_rx_en(wm_sdio_wrapper_reg_t *reg, uint32_t rx_en)
{
    reg->rx_start.rx_en = rx_en;
}

/**
 * @brief          get wrapper sdio rx status
 * @param[in]      sdio wrapper hardware register base address
 * @return         rx status
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdio_wrapper_get_rx_status(wm_sdio_wrapper_reg_t *reg)
{
    return reg->rx_sts.rx_sts;
}

/**
 * @brief          get wrapper cmd buf addr
 * @param[in]      sdio wrapper hardware register base address
 * @return         down link cmd buf addr
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdio_wrapper_get_cmd_buf_addr(wm_sdio_wrapper_reg_t *reg)
{
    return reg->cmd_baddr;
}

/**
 * @brief          set wrapper cmd buf addr
 * @param[in]      sdio wrapper hardware register base address
 * @param[in]      down link cmd buf addr
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_sdio_wrapper_set_cmd_buf_addr(wm_sdio_wrapper_reg_t *reg, uint32_t addr)
{
    reg->cmd_baddr = addr;
}

/**
 * @brief          get wrapper cmd buf size
 * @param[in]      sdio wrapper hardware register base address
 * @return         cmd buf size
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdio_wrapper_get_cmd_buf_size(wm_sdio_wrapper_reg_t *reg)
{
    return reg->cmd_buf_size.buf_size;
}

/**
 * @brief          wrapper cmd buf size
 * @param[in]      sdio wrapper hardware register base address
 * @param[in]      cmd buf size
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_sdio_wrapper_set_cmd_buf_size(wm_sdio_wrapper_reg_t *reg, uint32_t size)
{
    reg->cmd_buf_size.buf_size = size;
}

#ifdef __cplusplus
}
#endif

#endif /* __WM_LL_SDIO_WRAPPER_H__ */

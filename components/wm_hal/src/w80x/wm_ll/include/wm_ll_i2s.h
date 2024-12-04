/**
 * @file wm_ll_i2s.h
 *
 * @brief ll i2s Module
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

#ifndef __WM_LL_I2S_H__
#define __WM_LL_I2S_H__

#include "wm_attr.h"
#include "wm_soc_cfgs.h"
#include "wm_i2s_reg_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/************************************** i2s ctrl start *******************************************/
/**
 * @brief          Reset I2S ctrl reg
 * @param[in]      reg Pointer to I2S register structure.
 */
ATTRIBUTE_INLINE void wm_ll_i2s_reset_ctrl(wm_i2s_reg_t *reg)
{
    reg->control.val = 0;
}

/**
 * @brief          Enable i2s
 * @param[in]      reg Pointer to I2S register structure.
 * @param[in]      en  enable or not,type of  @arg bool
 */
ATTRIBUTE_INLINE void wm_ll_i2s_set_en(wm_i2s_reg_t *reg, bool en)
{
    reg->control.i2sen = en;
}

/**
 * @brief          Get i2s enabled or not
 * @param[in]      reg Pointer to I2S register structure.
 * @retval         Enable or not
 */
ATTRIBUTE_INLINE bool wm_ll_i2s_is_enabled(wm_i2s_reg_t *reg)
{
    return (bool)reg->control.i2sen;
}

/**
 * @brief          Enable i2s tx
 * @param[in]      reg Pointer to I2S register structure.
 * @param[in]      en  enable or not,type of  @arg bool
 */
ATTRIBUTE_INLINE void wm_ll_i2s_set_tx_en(wm_i2s_reg_t *reg, bool en)
{
    reg->control.txen = en;
}

/**
 * @brief          Get i2s tx enabled or not
 * @param[in]      reg Pointer to I2S register structure.
 * @retval         Enable or not
 */
ATTRIBUTE_INLINE bool wm_ll_i2s_is_tx_enabled(wm_i2s_reg_t *reg)
{
    return (bool)reg->control.txen;
}

/**
 * @brief          Enable i2s rx
 * @param[in]      reg Pointer to I2S register structure.
 * @param[in]      en  enable or not,type of  @arg bool
 */
ATTRIBUTE_INLINE void wm_ll_i2s_set_rx_en(wm_i2s_reg_t *reg, bool en)
{
    reg->control.rxen = en;
}

/**
 * @brief          Get i2s rx enabled or not
 * @param[in]      reg Pointer to I2S register structure.
 * @retval         Enable or not
 */
ATTRIBUTE_INLINE bool wm_ll_i2s_is_rx_enabled(wm_i2s_reg_t *reg)
{
    return (bool)reg->control.rxen;
}

/**
 * @brief          Enable i2s mute
 * @param[in]      reg Pointer to I2S register structure.
 * @param[in]      en  enable or not,type of  @arg bool
 */
ATTRIBUTE_INLINE void wm_ll_i2s_set_mute_en(wm_i2s_reg_t *reg, bool en)
{
    reg->control.mute = en;
}

/**
 * @brief          Get i2s mute enabled or not
 * @param[in]      reg Pointer to I2S register structure.
 * @retval         Enable or not
 */
ATTRIBUTE_INLINE bool wm_ll_i2s_is_mute_enabled(wm_i2s_reg_t *reg)
{
    return (bool)reg->control.mute;
}

/**
 * @brief          Set the transmission word length
 * @param[in]      reg Pointer to I2S register structure.
 * @param[in]      fmt i2s word width,type of  @arg enum wm_i2s_fmt
 */
ATTRIBUTE_INLINE void wm_ll_i2s_set_data_fmt(wm_i2s_reg_t *reg, enum wm_i2s_fmt fmt)
{
    reg->control.wdwidth = fmt;
}

/**
 * @brief          Get the transmission word length
 * @param[in]      reg Pointer to I2S register structure.
 * @retval         type of  @ref enum wm_i2s_fmt
 */
ATTRIBUTE_INLINE enum wm_i2s_fmt wm_ll_i2s_get_data_fmt(wm_i2s_reg_t *reg)
{
    return reg->control.wdwidth;
}

/**
 * @brief          Set the tx clock phase
 * @param[in]      reg Pointer to I2S register structure.
 * @param[in]      sel i2s clock phase select, type of  @arg bool
 */
ATTRIBUTE_INLINE void wm_ll_i2s_set_tx_clock_phase(wm_i2s_reg_t *reg, bool sel)
{
    reg->control.tx_clk_phase_sel = sel;
}

/**
 * @brief          Get the tx clock phase
 * @param[in]      reg Pointer to I2S register structure.
 * @retval         type of  @ref bool
 */
ATTRIBUTE_INLINE bool wm_ll_i2s_get_tx_clock_phase(wm_i2s_reg_t *reg)
{
    return reg->control.tx_clk_phase_sel;
}

/**
 * @brief          Set the txfifo threshold
                   When the number of existing words in the TXFIFO is equal to or less than the value of TXTH,    the TXTHIF bit will be set.
                   At this point, based on the configuration, you can choose to trigger either the TXDMA or the I2S interrupt.
 * @param[in]      reg Pointer to I2S register structure.
 * @param[in]      txth txfifo threshold
 */
ATTRIBUTE_INLINE void wm_ll_i2s_set_txfifo_threshold(wm_i2s_reg_t *reg, uint8_t txth)
{
    reg->control.txth = txth;
}

/**
 * @brief          Get the txfifo threshold
 * @param[in]      reg Pointer to I2S register structure.
 * @retval         txth txfifo threshold
 */
ATTRIBUTE_INLINE uint8_t wm_ll_i2s_get_txfifo_threshold(wm_i2s_reg_t *reg)
{
    return reg->control.txth;
}

/**
 * @brief          Set the rxfifo threshold
                   When the number of existing words in the RXFIFO is equal to or greater than the value of RXTH, the RXTHIF bit will be set.
                   At this point, based on the configuration, you can choose to trigger either the RXDMA or the I2S interrupt.
 * @param[in]      reg Pointer to I2S register structure.
 * @param[in]      rxfifo threshold
 */
ATTRIBUTE_INLINE void wm_ll_i2s_set_rxfifo_threshold(wm_i2s_reg_t *reg, uint8_t rxth)
{
    reg->control.rxth = rxth;
}

/**
 * @brief          Get the rxfifo threshold
 * @param[in]      reg Pointer to I2S register structure.
 * @retval         rxfifo threshold
 */
ATTRIBUTE_INLINE uint8_t wm_ll_i2s_get_rxfifo_threshold(wm_i2s_reg_t *reg)
{
    return reg->control.rxth;
}

/**
 * @brief          Set the rx clock phase
 * @param[in]      reg Pointer to I2S register structure.
 * @param[in]      sel i2s clock phase select, type of  @arg bool
 */
ATTRIBUTE_INLINE void wm_ll_i2s_set_rx_clock_phase(wm_i2s_reg_t *reg, bool sel)
{
    reg->control.rx_clk_phase_sel = sel;
}

/**
 * @brief          Get the rx clock phase
 * @param[in]      reg Pointer to I2S register structure.
 * @retval         type of  @ref bool
 */
ATTRIBUTE_INLINE bool wm_ll_i2s_get_rx_clock_phase(wm_i2s_reg_t *reg)
{
    return reg->control.rx_clk_phase_sel;
}

/**
 * @brief          Enable right audio channel zero-crossing detection
 * @param[in]      reg Pointer to I2S register structure.
 * @param[in]      en  enable or not,type of  @arg bool
 */
ATTRIBUTE_INLINE void wm_ll_i2s_rzc_set_en(wm_i2s_reg_t *reg, bool en)
{
    reg->control.rzcen = en;
}

/**
 * @brief          Get i2s rzc enabled or not
 * @param[in]      reg Pointer to I2S register structure.
 * @retval         Enable or not
 */
ATTRIBUTE_INLINE bool wm_ll_i2s_is_rzc_enabled(wm_i2s_reg_t *reg)
{
    return reg->control.rzcen;
}

/**
 * @brief          Enable left audio channel zero-crossing detection
 * @param[in]      reg Pointer to I2S register structure.
 * @param[in]      en  enable or not,type of  @arg bool
 */
ATTRIBUTE_INLINE void wm_ll_i2s_lzc_set_en(wm_i2s_reg_t *reg, bool en)
{
    reg->control.lzcen = en;
}

/**
 * @brief          Get i2s lzc enabled or not
 * @param[in]      reg Pointer to I2S register structure.
 * @retval         Enable or not
 */
ATTRIBUTE_INLINE bool wm_ll_i2s_is_lzc_enabled(wm_i2s_reg_t *reg)
{
    return reg->control.lzcen;
}

/**
 * @brief          Clear tx fifo
 * @param[in]      reg Pointer to I2S register structure.
 */
ATTRIBUTE_INLINE void wm_ll_i2s_clear_tx_fifo(wm_i2s_reg_t *reg)
{
    reg->control.txclr = 1;
}

/**
 * @brief          Clear rx fifo
 * @param[in]      reg Pointer to I2S register structure.
 */
ATTRIBUTE_INLINE void wm_ll_i2s_clear_rx_fifo(wm_i2s_reg_t *reg)
{
    reg->control.rxclr = 1;
}

/**
 * @brief          Enable i2s tx dma
 * @param[in]      reg Pointer to I2S register structure.
 * @param[in]      en  enable or not,type of  @arg bool
 */
ATTRIBUTE_INLINE void wm_ll_i2s_set_tx_dma_en(wm_i2s_reg_t *reg, bool en)
{
    reg->control.txdmaen = en;
}

/**
 * @brief          Get i2s tx dma enabled or not
 * @param[in]      reg Pointer to I2S register structure.
 * @retval         Enable or not
 */
ATTRIBUTE_INLINE bool wm_ll_i2s_is_tx_dma_enabled(wm_i2s_reg_t *reg)
{
    return reg->control.txdmaen;
}

/**
 * @brief          Enable i2s rx dma
 * @param[in]      reg Pointer to I2S register structure.
 * @param[in]      en  enable or not,type of  @arg bool
 */
ATTRIBUTE_INLINE void wm_ll_i2s_set_rx_dma_en(wm_i2s_reg_t *reg, bool en)
{
    reg->control.rxdmaen = en;
}

/**
 * @brief          Get i2s rx dma enabled or not
 * @param[in]      reg Pointer to I2S register structure.
 * @retval         Enable or not
 */
ATTRIBUTE_INLINE bool wm_ll_i2s_is_rx_dma_enabled(wm_i2s_reg_t *reg)
{
    return reg->control.rxdmaen;
}

/**
 * @brief          Select stereo or mono
 * @param[in]      reg Pointer to I2S register structure.
 * @param[in]      en If enable the mono
 */
ATTRIBUTE_INLINE void wm_ll_i2s_set_chan_type(wm_i2s_reg_t *reg, enum wm_i2s_chan_type ctype)
{
    reg->control.mono_stereo = (ctype == WM_I2S_CHAN_TYPE_STEREO ? 0 : 1);
}

/**
 * @brief          Get i2s stereo or mono selection
 * @param[in]      reg Pointer to I2S register structure.
 * @retval         the chan type @ref enum wm_i2s_chan_type
 */
ATTRIBUTE_INLINE enum wm_i2s_chan_type wm_ll_i2s_get_chan_type(wm_i2s_reg_t *reg)
{
    return reg->control.mono_stereo;
}

/**
 * @brief          Select right or left audio channel reception
 * @param[in]      reg Pointer to I2S register structure.
 * @param[in]      sel false if select right else false
 */
ATTRIBUTE_INLINE void wm_ll_i2s_set_mono_left(wm_i2s_reg_t *reg, bool sel)
{
    reg->control.rxlch = !sel;
}

/**
 * @brief          Get if i2s right channel selection in mono mode
 * @param[in]      reg Pointer to I2S register structure.
 * @retval         true if right channel is selected else false
 */
ATTRIBUTE_INLINE bool wm_ll_i2s_get_mono_right(wm_i2s_reg_t *reg)
{
    return reg->control.rxlch;
}

/**
 * @brief          Set data format standard
 * @param[in]      reg Pointer to I2S register structure.
 * @param[in]      sel data format standard, type of @arg enum wm_i2s_std
 */
ATTRIBUTE_INLINE void wm_ll_i2s_set_data_std(wm_i2s_reg_t *reg, enum wm_i2s_std std)
{
    reg->control.format = std;
}

/**
 * @brief          Get data format standard
 * @param[in]      reg Pointer to I2S register structure.
 * @retval         type of  @ref enum wm_i2s_std
 */
ATTRIBUTE_INLINE enum wm_i2s_std wm_ll_i2s_get_data_std(wm_i2s_reg_t *reg)
{
    return reg->control.format;
}

/**
 * @brief          Set 1 to enable timeout counter
 * @param[in]      reg Pointer to I2S register structure.
 * @param[in]      en  enable or not,type of  @arg bool
 */
ATTRIBUTE_INLINE void wm_ll_i2s_set_rx_timeout_en(wm_i2s_reg_t *reg, bool en)
{
    reg->control.timeout_en = en;
}

/**
 * @brief          Get i2s rx timeout counter enabled or not
 * @param[in]      reg Pointer to I2S register structure.
 * @retval         Enable or not
 */
ATTRIBUTE_INLINE bool wm_ll_i2s_is_rx_timeout_enabled(wm_i2s_reg_t *reg)
{
    return reg->control.timeout_en;
}

/**
 * @brief          Enable i2s duplex
 * @param[in]      reg Pointer to I2S register structure.
 * @param[in]      en  enable or not,type of  @arg bool
 */
ATTRIBUTE_INLINE void wm_ll_i2s_set_duplex_en(wm_i2s_reg_t *reg, bool en)
{
    reg->control.duplex_en = en;
}

/**
 * @brief          Get i2s duplex enabled or not
 * @param[in]      reg Pointer to I2S register structure.
 * @retval         Enable or not
 */
ATTRIBUTE_INLINE bool wm_ll_i2s_is_duplex_enabled(wm_i2s_reg_t *reg)
{
    return reg->control.duplex_en;
}

/**
 * @brief          Select master or slave
 * @param[in]      reg Pointer to I2S register structure.
 * @param[in]      sel select master or slave, type of @arg enum wm_i2s_mode
 */
ATTRIBUTE_INLINE void wm_ll_i2s_set_mode(wm_i2s_reg_t *reg, enum wm_i2s_mode sel)
{
    reg->control.master_slave_sel = sel;
}

/**
 * @brief          Get i2s master or slave selection
 * @param[in]      reg Pointer to I2S register structure.
 * @retval         type of  @ref enum wm_i2s_mode
 */
ATTRIBUTE_INLINE enum wm_i2s_mode wm_ll_i2s_get_mode(wm_i2s_reg_t *reg)
{
    return reg->control.master_slave_sel;
}

/************************************** i2s ctrl end *******************************************/

/************************************** interrupt start ****************************************/
/**
 * @brief          Disable specified interrupt
 * @param[in]      reg Pointer to I2S register structure.
 * @param[in]      mask  mask bit, type of @arg wm_i2s_irq_mask_t
 */
ATTRIBUTE_INLINE void wm_ll_i2s_disable_irq(wm_i2s_reg_t *reg, wm_i2s_irq_mask_t mask)
{
    reg->imask.val |= mask;
}

/**
 * @brief          Disable all interrupt
 * @param[in]      reg Pointer to I2S register structure.
 */
ATTRIBUTE_INLINE void wm_ll_i2s_disable_all_irq(wm_i2s_reg_t *reg)
{
    reg->imask.val |= I2S_INT_MASK;
}

/**
 * @brief          Enable specified interrupt
 * @param[in]      reg Pointer to I2S register structure.
 * @param[in]      mask  mask bit, type of @arg wm_i2s_irq_mask_t
 */
ATTRIBUTE_INLINE void wm_ll_i2s_enable_irq(wm_i2s_reg_t *reg, wm_i2s_irq_mask_t mask)
{
    reg->imask.val &= (~mask);
}

/**
 * @brief          Get i2s interrupt mask configuration,1:enable,0: disable
 * @param[in]      reg Pointer to I2S register structure.
 * @retval         interrupt mask register
 */
ATTRIBUTE_INLINE uint32_t wm_ll_i2s_get_irq_mask(wm_i2s_reg_t *reg)
{
    return reg->imask.val;
}

/************************************** interrupt mask end ******************************************/

/************************************** interrupt src start *****************************************/
/**
 * @brief          Get specified interrupt src
 * @param[in]      reg Pointer to I2S register structure.
 * @param[in]      flag  flag bit, type of @arg wm_i2s_irq_flag_t
 * @retval         0     no flag set
 *                 1     flag set
 */
ATTRIBUTE_INLINE bool wm_ll_i2s_get_specified_irq_flag(wm_i2s_reg_t *reg, wm_i2s_irq_flag_t flag)
{
    return !!(reg->int_flag.val & flag);
}

/**
 * @brief          Get i2s interrupt src
 * @param[in]      reg Pointer to I2S register structure.
 * @retval         i2s interrupt src
 */
ATTRIBUTE_INLINE uint32_t wm_ll_i2s_get_irq_flag(wm_i2s_reg_t *reg)
{
    return (reg->int_flag.val & I2S_INT_GET_SRC);
}

/**
 * @brief          Clear specified interrupt src
 * @param[in]      reg Pointer to I2S register structure.
 * @param[in]      clr  clear bit, type of @arg wm_ll_i2s_irq_flag_clear_t
 */
ATTRIBUTE_INLINE void wm_ll_i2s_clear_irq_flag(wm_i2s_reg_t *reg, wm_i2s_irq_flag_clear_t clr)
{
    reg->int_flag.val |= clr;
}

/**
 * @brief          Clear all interrupt src
 * @param[in]      reg Pointer to I2S register structure.
 */
ATTRIBUTE_INLINE void wm_ll_i2s_clear_all_irq_flag(wm_i2s_reg_t *reg)
{
    reg->int_flag.val = I2S_INT_CLS_SRC;
}

/************************************** interrupt src end *******************************************/

/************************************** fifo status start *******************************************/

/**
 * @brief          Get the number of words currently in the RXFIFO
 * @param[in]      reg Pointer to I2S register structure.
 * @retval         rx fifo count
 */
ATTRIBUTE_INLINE uint8_t wm_ll_i2s_get_rxfifo_count(wm_i2s_reg_t *reg)
{
    return (uint8_t)(reg->status.rxcnt);
}

/**
 * @brief          Get the number of words currently in the TXFIFO
 * @param[in]      reg Pointer to I2S register structure.
 * @retval         tx fifo count
 */
ATTRIBUTE_INLINE uint8_t wm_ll_i2s_get_txfifo_count(wm_i2s_reg_t *reg)
{
    return (uint8_t)(reg->status.txcnt);
}

/**
 * @brief          Get the number of valid bytes in the last word of RXFIFO, after reception is completed
 * @param[in]      reg Pointer to I2S register structure.
 * @retval         the number of valid bytes
 */
ATTRIBUTE_INLINE uint8_t wm_ll_i2s_get_valid_bytes_in_last_word(wm_i2s_reg_t *reg)
{
    return (uint8_t)(reg->status.validbyte);
}

/************************************** fifo status end   *******************************************/

/**
 * @brief          Set tx data
 * @param[in]      reg Pointer to I2S register structure.
 * @param[in]      the data to be written into the TXFIFO
 */
ATTRIBUTE_INLINE void wm_ll_i2s_set_tx_data(wm_i2s_reg_t *reg, uint32_t data)
{
    reg->tx.val = data;
}

/**
 * @brief            Read i2s RXFIFO
 * @param[in]        reg Pointer to I2S register structure.
 * @retval           the data read from RXFIFO
 */
ATTRIBUTE_INLINE uint32_t wm_ll_i2s_read_rx_data(wm_i2s_reg_t *reg)
{
    return reg->rx.val;
}

/**
 * @brief          Get TX FIFO address
 * @param[in]      reg Pointer to I2S register structure.
 * @retval         the address of TX FIFO
 */
ATTRIBUTE_INLINE uint32_t wm_ll_i2s_get_tx_fifo_addr(wm_i2s_reg_t *reg)
{
    return (uint32_t)(&reg->tx.val);
}

/**
 * @brief          Get TX FIFO address
 * @param[in]      reg Pointer to I2S register structure.
 * @retval         the address of TX FIFO
 */
ATTRIBUTE_INLINE uint32_t wm_ll_i2s_get_rx_fifo_addr(wm_i2s_reg_t *reg)
{
    return (uint32_t)(&reg->rx.val);
}

#ifdef __cplusplus
}
#endif

#endif

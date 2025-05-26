/**
 * @file wm_ll_spi.h
 *
 * @brief SPI Driver Module
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

#ifndef __WM_LL_SPI_H__
#define __WM_LL_SPI_H__

#include "wm_spi_reg_struct.h"
#include "wm_attr.h"
#include "wm_reg_op.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WM_SPI_MAX_TXRX_LEN     8191  //unit is byte, [3:18] max length is 8191 bytes
#define WM_SPI_MAX_DMA_TXRX_LEN 65535 //unit is byte, [8:23] max length is 65535 bytes

/**
 * @brief Reset the value of the CH_CFG register.
 *
 * @param hw Pointer to the SPI register structure.
 * @param reset_val The value to reset the register to.
 */
ATTRIBUTE_INLINE void wm_ll_spi_reset_ch_cfg(wm_spi_reg_t *hw, uint32_t reset_val)
{
    hw->ch_cfg.val = reset_val;
}

/**
 * @brief Get the start bit value from SPI channel configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @return bool Start bit value.
 */
ATTRIBUTE_INLINE bool wm_ll_spi_get_ch_cfg_start(const wm_spi_reg_t *hw)
{
    return hw->ch_cfg.start;
}

/**
 * @brief Set the start bit value in SPI channel configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val Start bit value.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_ch_cfg_start(wm_spi_reg_t *hw, bool val)
{
    hw->ch_cfg.start = val;
}

/**
 * @brief Get the cs_out bit value from SPI channel configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @return bool cs_out bit value.
 */
ATTRIBUTE_INLINE bool wm_ll_spi_get_ch_cfg_cs_out(const wm_spi_reg_t *hw)
{
    return hw->ch_cfg.cs_out;
}

/**
 * @brief Set the cs_out bit value in SPI channel configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val cs_out bit value.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_ch_cfg_cs_out(wm_spi_reg_t *hw, bool val)
{
    hw->ch_cfg.cs_out = val;
}

/**
 * @brief Get the chip_sel bit value from SPI channel configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @return bool chip_sel bit value.
 */
ATTRIBUTE_INLINE bool wm_ll_spi_get_ch_cfg_chip_sel(const wm_spi_reg_t *hw)
{
    return hw->ch_cfg.chip_sel;
}

/**
 * @brief Set the chip_sel bit value in SPI channel configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val chip_sel bit value.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_ch_cfg_chip_sel(wm_spi_reg_t *hw, bool val)
{
    hw->ch_cfg.chip_sel = val;
}

/**
 * @brief Get the tx_rx_len value from SPI channel configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @return uint16_t tx_rx_len value.
 */
ATTRIBUTE_INLINE uint16_t wm_ll_spi_get_ch_cfg_tx_rx_len(wm_spi_reg_t *hw)
{
    return WM_LL_REG_FILED_COPY_READ(hw->ch_cfg, tx_rx_len);
}

/**
 * @brief Set the tx_rx_len value in SPI channel configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val tx_rx_len value.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_ch_cfg_tx_rx_len(wm_spi_reg_t *hw, uint16_t val)
{
    WM_LL_REG_FILED_COPY_WRTIE(hw->ch_cfg, tx_rx_len, val);
}

/**
 * @brief Get the tx_ch_on bit value from SPI channel configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @return bool tx_ch_on bit value.
 */
ATTRIBUTE_INLINE bool wm_ll_spi_get_ch_cfg_tx_ch_on(const wm_spi_reg_t *hw)
{
    return hw->ch_cfg.tx_ch_on;
}

/**
 * @brief Set the tx_ch_on bit value in SPI channel configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val tx_ch_on bit value.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_ch_cfg_tx_ch_on(wm_spi_reg_t *hw, bool val)
{
    hw->ch_cfg.tx_ch_on = val;
}

/**
 * @brief Get the rx_ch_on bit value from SPI channel configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @return bool rx_ch_on bit value.
 */
ATTRIBUTE_INLINE bool wm_ll_spi_get_ch_cfg_rx_ch_on(const wm_spi_reg_t *hw)
{
    return hw->ch_cfg.rx_ch_on;
}

/**
 * @brief Set the rx_ch_on bit value in SPI channel configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val rx_ch_on bit value.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_ch_cfg_rx_ch_on(wm_spi_reg_t *hw, bool val)
{
    hw->ch_cfg.rx_ch_on = val;
}

/**
 * @brief Get the continue_mode bit value from SPI channel configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @return bool continue_mode bit value.
 */
ATTRIBUTE_INLINE bool wm_ll_spi_get_ch_cfg_continue_mode(const wm_spi_reg_t *hw)
{
    return hw->ch_cfg.continue_mode;
}

/**
 * @brief Set the continue_mode bit value in SPI channel configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val continue_mode bit value.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_ch_cfg_continue_mode(wm_spi_reg_t *hw, bool val)
{
    hw->ch_cfg.continue_mode = val;
}

/**
 * @brief Get the clr_fifo bit value from SPI channel configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @return bool clr_fifo bit value.
 */
ATTRIBUTE_INLINE bool wm_ll_spi_get_ch_cfg_clr_fifo(const wm_spi_reg_t *hw)
{
    return hw->ch_cfg.clr_fifo;
}

/**
 * @brief Set the clr_fifo bit value in SPI channel configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val clr_fifo bit value.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_ch_cfg_clr_fifo(wm_spi_reg_t *hw, bool val)
{
    hw->ch_cfg.clr_fifo = val;
}

/**
 * @brief Get the rx_invalid_bit value from SPI channel configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @return uint8_t rx_invalid_bit value.
 */
ATTRIBUTE_INLINE uint8_t wm_ll_spi_get_ch_cfg_rx_invalid_bit(wm_spi_reg_t *hw)
{
    return WM_LL_REG_FILED_COPY_READ(hw->ch_cfg, rx_invalid_bit);
}

/**
 * @brief Set the rx_invalid_bit value in SPI channel configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val rx_invalid_bit value.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_ch_cfg_rx_invalid_bit(wm_spi_reg_t *hw, uint8_t val)
{
    WM_LL_REG_FILED_COPY_WRTIE(hw->ch_cfg, rx_invalid_bit, val);
}

/**
 * @brief Reset the value of the CFG register.
 *
 * @param hw Pointer to

 the SPI register structure.
 * @param reset_val The value to reset the register to.
 */
ATTRIBUTE_INLINE void wm_ll_spi_reset_cfg(wm_spi_reg_t *hw, uint32_t reset_val)
{
    hw->cfg.val = reset_val;
}

/**
 * @brief Get the cpol bit value from SPI configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @return bool cpol bit value.
 */
ATTRIBUTE_INLINE bool wm_ll_spi_get_cfg_cpol(const wm_spi_reg_t *hw)
{
    return hw->cfg.cpol;
}

/**
 * @brief Set the cpol bit value in SPI configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val cpol bit value.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_cfg_cpol(wm_spi_reg_t *hw, bool val)
{
    hw->cfg.cpol = val;
}

/**
 * @brief Get the cpha bit value from SPI configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @return bool cpha bit value.
 */
ATTRIBUTE_INLINE bool wm_ll_spi_get_cfg_cpha(const wm_spi_reg_t *hw)
{
    return hw->cfg.cpha;
}

/**
 * @brief Set the cpha bit value in SPI configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val cpha bit value.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_cfg_cpha(wm_spi_reg_t *hw, bool val)
{
    hw->cfg.cpha = val;
}

/**
 * @brief Get the role bit value from SPI configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @return bool role bit value.
 */
ATTRIBUTE_INLINE bool wm_ll_spi_get_cfg_role(const wm_spi_reg_t *hw)
{
    return hw->cfg.role;
}

/**
 * @brief Set the role bit value in SPI configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val role bit value.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_cfg_role(wm_spi_reg_t *hw, bool val)
{
    hw->cfg.role = val;
}

/**
 * @brief Get the big_endian bit value from SPI configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @return bool big_endian bit value.
 */
ATTRIBUTE_INLINE bool wm_ll_spi_get_cfg_big_endian(const wm_spi_reg_t *hw)
{
    return hw->cfg.big_edian;
}

/**
 * @brief Set the big_endian bit value in SPI configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val big_endian bit value.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_cfg_big_endian(wm_spi_reg_t *hw, bool val)
{
    hw->cfg.big_edian = val;
}

/**
 * @brief Get the frame_delay value from SPI configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @return uint8_t frame_delay value.
 */
ATTRIBUTE_INLINE uint8_t wm_ll_spi_get_cfg_frame_delay(const wm_spi_reg_t *hw)
{
    return hw->cfg.frame_delay;
}

/**
 * @brief Set the frame_delay value in SPI configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val frame_delay value.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_cfg_frame_delay(wm_spi_reg_t *hw, uint8_t val)
{
    hw->cfg.frame_delay = val;
}

/**
 * @brief Get the spi_out_delay value from SPI configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @return uint8_t spi_out_delay value.
 */
ATTRIBUTE_INLINE uint8_t wm_ll_spi_get_cfg_spi_out_delay(const wm_spi_reg_t *hw)
{
    return hw->cfg.spi_out_delay;
}

/**
 * @brief Set the spi_out_delay value in SPI configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val spi_out_delay value.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_cfg_spi_out_delay(wm_spi_reg_t *hw, uint8_t val)
{
    hw->cfg.spi_out_delay = val;
}

/**
 * @brief Get the cs_setup_time value from SPI configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @return uint8_t cs_setup_time value.
 */
ATTRIBUTE_INLINE uint8_t wm_ll_spi_get_cfg_cs_setup_time(const wm_spi_reg_t *hw)
{
    return hw->cfg.cs_steup_time;
}

/**
 * @brief Set the cs_setup_time value in SPI configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val cs_setup_time value.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_cfg_cs_setup_time(wm_spi_reg_t *hw, uint8_t val)
{
    hw->cfg.cs_steup_time = val;
}

/**
 * @brief Get the hold_time value from SPI configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @return uint8_t hold_time value.
 */
ATTRIBUTE_INLINE uint8_t wm_ll_spi_get_cfg_hold_time(const wm_spi_reg_t *hw)
{
    return hw->cfg.hold_time;
}

/**
 * @brief Set the hold_time value in SPI configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val hold_time value.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_cfg_hold_time(wm_spi_reg_t *hw, uint8_t val)
{
    hw->cfg.hold_time = val;
}

/**
 * @brief Get the tx_driven bit value from SPI configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @return bool tx_driven bit value.
 */
ATTRIBUTE_INLINE bool wm_ll_spi_get_cfg_tx_driven(const wm_spi_reg_t *hw)
{
    return hw->cfg.tx_driven;
}

/**
 * @brief Set the tx_driven bit value in SPI configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val tx_driven bit value.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_cfg_tx_driven(wm_spi_reg_t *hw, bool val)
{
    hw->cfg.tx_driven = val;
}

/**
 * @brief Get the frame_format value from SPI configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @return uint8_t frame_format value.
 */
ATTRIBUTE_INLINE uint8_t wm_ll_spi_get_cfg_frame_format(const wm_spi_reg_t *hw)
{
    return hw->cfg.fram_format;
}

/**
 * @brief Set the frame_format value in SPI configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val frame_format value.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_cfg_frame_format(wm_spi_reg_t *hw, uint8_t val)
{
    hw->cfg.fram_format = val;
}

/**
 * @brief Reset the value of the CLK_CFG register.
 *
 * @param hw Pointer to the SPI register structure.
 * @param reset_val The value to reset the register to.
 */
ATTRIBUTE_INLINE void wm_ll_spi_reset_clk_cfg(wm_spi_reg_t *hw, uint32_t reset_val)
{
    hw->clk_cfg.val = reset_val;
}

/**
 * @brief Get the divider value from SPI clock configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @return uint16_t Divider value.
 */
ATTRIBUTE_INLINE uint16_t wm_ll_spi_get_clk_cfg_divider(wm_spi_reg_t *hw)
{
    return WM_LL_REG_FILED_COPY_READ(hw->clk_cfg, divider);
}

/**
 * @brief Set the divider value in SPI clock configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val Divider value.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_clk_cfg_divider(wm_spi_reg_t *hw, uint16_t val)
{
    WM_LL_REG_FILED_COPY_WRTIE(hw->clk_cfg, divider, val);
}

/**
 * @brief Reset the value of the MODE_CFG register.
 *
 * @param hw Pointer to the SPI register structure.
 * @param reset_val The value to reset the register to.
 */
ATTRIBUTE_INLINE void wm_ll_spi_reset_mode_cfg(wm_spi_reg_t *hw, uint32_t reset_val)
{
    hw->mode_cfg.val = reset_val;
}

/**
 * @brief Get the tx_dma_on bit value from SPI mode configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @return bool tx_dma_on bit value.
 */
ATTRIBUTE_INLINE bool wm_ll_spi_get_mode_cfg_tx_dma_on(const wm_spi_reg_t *hw)
{
    return hw->mode_cfg.tx_dma_on;
}

/**
 * @brief Set the tx_dma_on bit value in SPI mode configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val tx_dma_on bit value.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_mode_cfg_tx_dma_on(wm_spi_reg_t *hw, bool val)
{
    hw->mode_cfg.tx_dma_on = val;
}

/**
 * @brief Get the rx_dma_on bit value from SPI mode configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @return bool rx_dma_on bit value.
 */
ATTRIBUTE_INLINE bool wm_ll_spi_get_mode_cfg_rx_dma_on(const wm_spi_reg_t *hw)
{
    return hw->mode_cfg.rx_dma_on;
}

/**
 * @brief Set the rx_dma_on bit value in SPI mode configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val rx_dma_on bit value.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_mode_cfg_rx_dma_on(wm_spi_reg_t *hw, bool val)
{
    hw->mode_cfg.rx_dma_on = val;
}

/**
 * @brief Get the tx_trig_level value from SPI mode configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @return uint8_t tx_trig_level value.
 */
ATTRIBUTE_INLINE uint8_t wm_ll_spi_get_mode_cfg_tx_trig_level(const wm_spi_reg_t *hw)
{
    return hw->mode_cfg.tx_trig_level;
}

/**
 * @brief Set the tx_trig_level value in SPI mode configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val tx_trig_level value.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_mode_cfg_tx_trig_level(wm_spi_reg_t *hw, uint8_t val)
{
    hw->mode_cfg.tx_trig_level = val;
}

/**
 * @brief Get the rx_trig_level value from SPI mode configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @return uint8_t rx_trig_level value.
 */
ATTRIBUTE_INLINE uint8_t wm_ll_spi_get_mode_cfg_rx_trig_level(const wm_spi_reg_t *hw)
{
    return hw->mode_cfg.rx_trig_level;
}

/**
 * @brief Set the rx_trig_level value in SPI mode configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val rx_trig_level value.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_mode_cfg_rx_trig_level(wm_spi_reg_t *hw, uint8_t val)
{
    hw->mode_cfg.rx_trig_level = val;
}

/**
 * @brief Reset the value of the INT_MASK register.
 *
 * @param hw Pointer to the SPI register structure.
 * @param reset_val The value to reset the register to.
 */
ATTRIBUTE_INLINE void wm_ll_spi_reset_int_mask(wm_spi_reg_t *hw, uint32_t reset_val)
{
    hw->int_mask.val = reset_val;
}

/**
 * @brief Get the value of the INT_MASK register.
 *
 * @param hw Pointer to SPI register structure.
 * @return val value in int_mask.
 */
ATTRIBUTE_INLINE uint32_t wm_ll_spi_int_mask_get_val(const wm_spi_reg_t *hw)
{
    return hw->int_mask.val;
}

/**
 * @brief Get the tx_fifo_rdy_en bit value from SPI interrupt configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @return bool tx_fifo_rdy_en bit value.
 */
ATTRIBUTE_INLINE bool wm_ll_spi_get_int_mask_cfg_tx_fifo_rdy_en(const wm_spi_reg_t *hw)
{
    return hw->int_mask.tx_fifo_rdy_en;
}

/**
 * @brief Set the tx_fifo_rdy_en bit value in SPI interrupt configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val tx_fifo_rdy_en bit value.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_int_mask_cfg_tx_fifo_rdy_en(wm_spi_reg_t *hw, bool val)
{
    hw->int_mask.tx_fifo_rdy_en = val;
}

/**
 * @brief Get the rx_fifo_rdy_en bit value from SPI interrupt configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @return bool rx_fifo_rdy_en bit value.
 */
ATTRIBUTE_INLINE bool wm_ll_spi_get_int_mask_cfg_rx_fifo_rdy_en(const wm_spi_reg_t *hw)
{
    return hw->int_mask.rx_fifo_rdy_en;
}

/**
 * @brief Set the rx_fifo_rdy_en bit value in SPI interrupt configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val rx_fifo_rdy_en bit value.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_int_mask_cfg_rx_fifo_rdy_en(wm_spi_reg_t *hw, bool val)
{
    hw->int_mask.rx_fifo_rdy_en = val;
}

/**
 * @brief Get the tx_underrun_en bit value from SPI interrupt configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @return bool tx_underrun_en bit value.
 */
ATTRIBUTE_INLINE bool wm_ll_spi_get_int_mask_cfg_tx_underrun_en(const wm_spi_reg_t *hw)
{
    return hw->int_mask.tx_underrun_en;
}

/**
 * @brief Set the tx_underrun_en bit value in SPI interrupt configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val tx_underrun_en bit value.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_int_mask_cfg_tx_underrun_en(wm_spi_reg_t *hw, bool val)
{
    hw->int_mask.tx_underrun_en = val;
}

/**
 * @brief Get the tx_overrun_en bit value from SPI interrupt configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @return bool tx_overrun_en bit value.
 */
ATTRIBUTE_INLINE bool wm_ll_spi_get_int_mask_cfg_tx_overrun_en(const wm_spi_reg_t *hw)
{
    return hw->int_mask.tx_overrun_en;
}

/**
 * @brief Set the tx_overrun_en bit value in SPI interrupt configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val tx_overrun_en bit value.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_int_mask_cfg_tx_overrun_en(wm_spi_reg_t *hw, bool val)
{
    hw->int_mask.tx_overrun_en = val;
}

/**
 * @brief Get the rx_underrun_en bit value from SPI interrupt configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @return bool rx_underrun_en bit value.
 */
ATTRIBUTE_INLINE bool wm_ll_spi_get_int_mask_cfg_rx_underrun_en(const wm_spi_reg_t *hw)
{
    return hw->int_mask.rx_underrun_en;
}

/**
 * @brief Set the rx_underrun_en bit value in SPI interrupt configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val rx_underrun_en bit value.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_int_mask_cfg_rx_underrun_en(wm_spi_reg_t *hw, bool val)
{
    hw->int_mask.rx_underrun_en = val;
}

/**
 * @brief Get the rx_overrun_en bit value from SPI interrupt configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @return bool rx_overrun_en bit value.
 */
ATTRIBUTE_INLINE bool wm_ll_spi_get_int_mask_cfg_rx_overrun_en(const wm_spi_reg_t *hw)
{
    return hw->int_mask.rx_overrun_en;
}

/**
 * @brief Set the rx_overrun_en bit value in SPI interrupt configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val rx_overrun_en bit value.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_int_mask_cfg_rx_overrun_en(wm_spi_reg_t *hw, bool val)
{
    hw->int_mask.rx_overrun_en = val;
}

/**
 * @brief Get the spi_done_en bit value from SPI interrupt configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @return bool spi_done_en bit value.
 */
ATTRIBUTE_INLINE bool wm_ll_spi_get_int_mask_cfg_done_en(const wm_spi_reg_t *hw)
{
    return hw->int_mask.spi_done_en;
}

/**
 * @brief Set the spi_done_en bit value in SPI interrupt configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val spi_done_en bit value.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_int_mask_cfg_done_en(wm_spi_reg_t *hw, bool val)
{
    hw->int_mask.spi_done_en = val;
}

/**
 * @brief Get the timeout_en bit value from SPI interrupt configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @return bool timeout_en bit value.
 */
ATTRIBUTE_INLINE bool wm_ll_spi_get_int_mask_cfg_timeout_en(const wm_spi_reg_t *hw)
{
    return hw->int_mask.timeout_en;
}

/**
 * @brief Set the timeout_en bit value in SPI interrupt configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val timeout_en bit value.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_int_mask_cfg_timeout_en(wm_spi_reg_t *hw, bool val)
{
    hw->int_mask.timeout_en = val;
}

/**
 * @brief Reset the value of the INT_STATUS register.
 *
 * @param hw Pointer to the SPI register structure.
 * @param reset_val The value to reset the register to.
 */
ATTRIBUTE_INLINE void wm_ll_spi_reset_int_status(wm_spi_reg_t *hw, uint32_t reset_val)
{
    hw->int_status.val = reset_val;
}

/**
 * @brief get the value of the INT_STATUS register.
 *
 * @param hw Pointer to the SPI register structure.
 * @return interrupt statsus.
 */
ATTRIBUTE_INLINE uint32_t wm_ll_spi_get_int_status(wm_spi_reg_t *hw)
{
    return hw->int_status.val;
}

/**
 * @brief Get the tx_fifo_rdy bit value from SPI interrupt status register.
 *
 * @param hw Pointer to SPI register structure.
 * @return bool tx_fifo_rdy bit value.
 */
ATTRIBUTE_INLINE bool wm_ll_spi_get_int_status_tx_fifo_rdy(const wm_spi_reg_t *hw)
{
    return hw->int_status.tx_fifo_rdy;
}

/**
 * @brief Set the tx_fifo_rdy bit value in SPI interrupt status register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val tx_fifo_rdy bit value.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_int_status_tx_fifo_rdy(wm_spi_reg_t *hw, bool val)
{
    hw->int_status.tx_fifo_rdy = val;
}

/**
 * @brief Get the rx_fifo_rdy bit value from SPI interrupt status register.
 *
 * @param hw Pointer to SPI register structure.
 * @return bool rx_fifo_rdy bit value.
 */
ATTRIBUTE_INLINE bool wm_ll_spi_get_int_status_rx_fifo_rdy(const wm_spi_reg_t *hw)
{
    return hw->int_status.rx_fifo_rdy;
}

/**
 * @brief Set the rx_fifo_rdy bit value in SPI interrupt status register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val rx_fifo_rdy bit value.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_int_status_rx_fifo_rdy(wm_spi_reg_t *hw, bool val)
{
    hw->int_status.rx_fifo_rdy = val;
}

/**
 * @brief Get the tx_underrun bit value from SPI interrupt status register.
 *
 * @param hw Pointer to SPI register structure.
 * @return bool tx_underrun bit value.
 */
ATTRIBUTE_INLINE bool wm_ll_spi_get_int_status_tx_underrun(const wm_spi_reg_t *hw)
{
    return hw->int_status.tx_underrun;
}

/**
 * @brief Set the tx_underrun bit value in SPI interrupt status register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val tx_underrun bit value.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_int_status_tx_underrun(wm_spi_reg_t *hw, bool val)
{
    hw->int_status.tx_underrun = val;
}

/**
 * @brief Get the tx_overrun bit value from SPI interrupt status register.
 *
 * @param hw Pointer to SPI register structure.
 * @return bool tx_overrun bit value.
 */
ATTRIBUTE_INLINE bool wm_ll_spi_get_int_status_tx_overrun(const wm_spi_reg_t *hw)
{
    return hw->int_status.tx_overrun;
}

/**
 * @brief Set the tx_overrun bit value in SPI interrupt status register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val tx_overrun bit value.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_int_status_tx_overrun(wm_spi_reg_t *hw, bool val)
{
    hw->int_status.tx_overrun = val;
}

/**
 * @brief Get the rx_underrun bit value from SPI interrupt status register.
 *
 * @param hw Pointer to SPI register structure.
 * @return bool rx_underrun bit value.
 */
ATTRIBUTE_INLINE bool wm_ll_spi_get_int_status_rx_underrun(const wm_spi_reg_t *hw)
{
    return hw->int_status.rx_underrun;
}

/**
 * @brief Set the rx_underrun bit value in SPI interrupt status register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val rx_underrun bit value.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_int_status_rx_underrun(wm_spi_reg_t *hw, bool val)
{
    hw->int_status.rx_underrun = val;
}

/**
 * @brief Get the rx_overrun bit value from SPI interrupt status register.
 *
 * @param hw Pointer to SPI register structure.
 * @return bool rx_overrun bit value.
 */
ATTRIBUTE_INLINE bool wm_ll_spi_get_int_status_rx_overrun(const wm_spi_reg_t *hw)
{
    return hw->int_status.rx_overrun;
}

/**
 * @brief Set the rx_overrun bit value in SPI interrupt status register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val rx_overrun bit value.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_int_status_rx_overrun(wm_spi_reg_t *hw, bool val)
{
    hw->int_status.rx_overrun = val;
}

/**
 * @brief Get the spi_done bit value from SPI interrupt status register.
 *
 * @param hw Pointer to SPI register structure.
 * @return bool spi_done bit value.
 */
ATTRIBUTE_INLINE bool wm_ll_spi_get_int_status_done(const wm_spi_reg_t *hw)
{
    return hw->int_status.spi_done;
}

/**
 * @brief Set the spi_done bit value in SPI interrupt status register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val spi_done bit value.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_int_status_done(wm_spi_reg_t *hw, bool val)
{
    hw->int_status.spi_done = val;
}

/**
 * @brief Get the timeout bit value from SPI interrupt status register.
 *
 * @param hw Pointer to SPI register structure.
 * @return bool timeout bit value.
 */
ATTRIBUTE_INLINE bool wm_ll_spi_get_int_status_timeout(const wm_spi_reg_t *hw)
{
    return hw->int_status.timeout;
}

/**
 * @brief Set the timeout bit value in SPI interrupt status register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val timeout bit value.

 */
ATTRIBUTE_INLINE void wm_ll_spi_set_int_status_timeout(wm_spi_reg_t *hw, bool val)
{
    hw->int_status.timeout = val;
}

/**
 * @brief Reset the value of the STATUS register.
 *
 * @param hw Pointer to the SPI register structure.
 * @param reset_val The value to reset the register to.
 */
ATTRIBUTE_INLINE void wm_ll_spi_reset_status(wm_spi_reg_t *hw, uint32_t reset_val)
{
    hw->status.val = reset_val;
}

/**
 * @brief Get the tx_fifo_level value from SPI status register.
 *
 * @param hw Pointer to SPI register structure.
 * @return uint8_t tx_fifo_level value.
 */
ATTRIBUTE_INLINE uint8_t wm_ll_spi_get_status_tx_fifo_level(const wm_spi_reg_t *hw)
{
    return (uint8_t)hw->status.tx_fifo_level;
}

/**
 * @brief Get the rx_fifo_level value from SPI status register.
 *
 * @param hw Pointer to SPI register structure.
 * @return uint8_t rx_fifo_level value.
 */
ATTRIBUTE_INLINE uint8_t wm_ll_spi_get_status_rx_fifo_level(const wm_spi_reg_t *hw)
{
    return (uint8_t)hw->status.rx_fifo_level;
}

/**
 * @brief Get the is_busy bit value from SPI status register.
 *
 * @param hw Pointer to SPI register structure.
 * @return bool is_busy bit value.
 */
ATTRIBUTE_INLINE bool wm_ll_spi_get_status_is_busy(const wm_spi_reg_t *hw)
{
    return hw->status.is_busy;
}

/**
 * @brief Reset the value of the TIMEOUT_CFG register.
 *
 * @param hw Pointer to the SPI register structure.
 * @param reset_val The value to reset the register to.
 */
ATTRIBUTE_INLINE void wm_ll_spi_reset_timeout_cfg(wm_spi_reg_t *hw, uint32_t reset_val)
{
    hw->timeout_cfg.val = reset_val;
}

/**
 * @brief Get the time_out_value from SPI timeout configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @return uint32_t time_out_value.
 */
ATTRIBUTE_INLINE uint32_t wm_ll_spi_get_timeout_cfg_timeout_value(const wm_spi_reg_t *hw)
{
    return hw->timeout_cfg.time_out_value;
}

/**
 * @brief Set the time_out_value in SPI timeout configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val time_out_value.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_timeout_cfg_timeout_value(wm_spi_reg_t *hw, uint32_t val)
{
    hw->timeout_cfg.time_out_value = val;
}

/**
 * @brief Get the timer_en bit value from SPI timeout configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @return bool timer_en bit value.
 */
ATTRIBUTE_INLINE bool wm_ll_spi_get_timeout_cfg_timer_en(const wm_spi_reg_t *hw)
{
    return hw->timeout_cfg.timer_en;
}

/**
 * @brief Set the timer_en bit value in SPI timeout configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val timer_en bit value.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_timeout_cfg_timer_en(wm_spi_reg_t *hw, bool val)
{
    hw->timeout_cfg.timer_en = val;
}

/**
 * @brief Reset the value of the TX_DATA register.
 *
 * @param hw Pointer to the SPI register structure.
 * @param reset_val The value to reset the register to.
 */
ATTRIBUTE_INLINE void wm_ll_spi_reset_tx_data(wm_spi_reg_t *hw, uint32_t reset_val)
{
    hw->tx_data.val = reset_val;
}

/**
 * @brief Get the address from SPI transmit data register.
 *
 * @param hw Pointer to SPI register structure.
 * @return uint32_t Address stored in transmit data register.
 */
ATTRIBUTE_INLINE uint32_t wm_ll_spi_get_tx_data_address(const wm_spi_reg_t *hw)
{
    return (uint32_t) & (hw->tx_data.address);
}

/**
 * @brief Set the address in SPI transmit data register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val Address to be stored in transmit data register.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_tx_data_address(wm_spi_reg_t *hw, uint32_t val)
{
    hw->tx_data.address = val;
}

/**
 * @brief Reset the value of the TRANS_MODE register.
 *
 * @param hw Pointer to the SPI register structure.
 * @param reset_val The value to reset the register to.
 */
ATTRIBUTE_INLINE void wm_ll_spi_reset_trans_mode(wm_spi_reg_t *hw, uint32_t reset_val)
{
    hw->trans_mode.val = reset_val;
}

/**
 * @brief Get the micro control length from SPI transmit mode configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @return uint32_t Micro control length value.
 */
ATTRIBUTE_INLINE uint32_t wm_ll_spi_get_micro_ctrl_len(const wm_spi_reg_t *hw)
{
    return hw->trans_mode.micro_ctrl_len;
}

/**
 * @brief Set the micro control length in SPI transmit mode configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val Micro control length value to be set.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_trans_mode_micro_ctrl_len(wm_spi_reg_t *hw, uint32_t val)
{
    hw->trans_mode.micro_ctrl_len = val;
}

/**
 * @brief Get the micro data length from SPI transmit mode configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @return uint32_t Micro data length value.
 */
ATTRIBUTE_INLINE uint32_t wm_ll_spi_get_trans_mode_micro_dat_len(const wm_spi_reg_t *hw)
{
    return hw->trans_mode.micro_dat_len;
}

/**
 * @brief Set the micro data length in SPI transmit mode configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val Micro data length value to be set.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_trans_mode_micro_dat_len(wm_spi_reg_t *hw, uint32_t val)
{
    hw->trans_mode.micro_dat_len = val;
}

/**
 * @brief Get the micro burst mode from SPI transmit mode configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @return bool Micro burst mode value.
 */
ATTRIBUTE_INLINE bool wm_ll_spi_get_trans_mode_micro_burst(const wm_spi_reg_t *hw)
{
    return hw->trans_mode.micro_burst;
}

/**
 * @brief Set the micro burst mode in SPI transmit mode configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val Micro burst mode value to be set.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_trans_mode_micro_burst(wm_spi_reg_t *hw, bool val)
{
    hw->trans_mode.micro_burst = val;
}

/**
 * @brief Get the TI block length from SPI transmit mode configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @return uint32_t TI block length value.
 */
ATTRIBUTE_INLINE uint32_t wm_ll_spi_get_trans_mode_ti_blk_len(const wm_spi_reg_t *hw)
{
    return hw->trans_mode.ti_blk_len;
}

/**
 * @brief Set the TI block length in SPI transmit mode configuration register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val TI block length value to be set.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_trans_mode_ti_blk_len(wm_spi_reg_t *hw, uint32_t val)
{
    hw->trans_mode.ti_blk_len = val;
}

/**
 * @brief Reset the value of the SLV_XMIT_LEN register.
 *
 * @param hw Pointer to the SPI register structure.
 * @param reset_val The value to reset the register to.
 */
ATTRIBUTE_INLINE void wm_ll_spi_reset_slv_xmit_len(wm_spi_reg_t *hw, uint32_t reset_val)
{
    hw->slv_xmit_len.val = reset_val;
}

/**
 * @brief Get the RX valid length from SPI transmit data length register.
 *
 * @param hw Pointer to SPI register structure.
 * @return uint32_t RX valid length value.
 */
ATTRIBUTE_INLINE uint32_t wm_ll_spi_get_slv_xmit_rx_valid_len(wm_spi_reg_t *hw)
{
    return WM_LL_REG_FILED_COPY_READ(hw->slv_xmit_len, rx_valid_len);
}

/**
 * @brief Set the RX valid length in SPI transmit data length register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val RX valid length value to be set.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_slv_xmit_rx_valid_len(wm_spi_reg_t *hw, uint32_t val)
{
    WM_LL_REG_FILED_COPY_WRTIE(hw->slv_xmit_len, rx_valid_len, val);
}

/**
 * @brief Get the TX valid length from SPI transmit data length register.
 *
 * @param hw Pointer to SPI register structure.
 * @return uint32_t TX valid length value.
 */
ATTRIBUTE_INLINE uint32_t wm_ll_spi_get_slv_xmit_tx_valid_len(wm_spi_reg_t *hw)
{
    return WM_LL_REG_FILED_COPY_READ(hw->slv_xmit_len, tx_valid_len);
}

/**
 * @brief Set the TX valid length in SPI transmit data length register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val TX valid length value to be set.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_slv_xmit_tx_valid_len(wm_spi_reg_t *hw, uint32_t val)
{
    WM_LL_REG_FILED_COPY_WRTIE(hw->slv_xmit_len, tx_valid_len, val);
}

/**
 * @brief Reset the value of the RX_DATA register.
 *
 * @param hw Pointer to the SPI register structure.
 * @param reset_val The value to reset the register to.
 */
ATTRIBUTE_INLINE void wm_ll_spi_reset_rx_data(wm_spi_reg_t *hw, uint32_t reset_val)
{
    hw->rx_data.val = reset_val;
}

/**
 * @brief Get the window address from SPI receive data register.
 *
 * @param hw Pointer to SPI register structure.
 * @return uint32_t Window address from RX FIFO.
 */
ATTRIBUTE_INLINE uint32_t wm_ll_spi_get_rx_data(const wm_spi_reg_t *hw)
{
    return hw->rx_data.rx_addr;
}

/**
 * @brief Get the window address from SPI receive data register.
 *
 * @param hw Pointer to SPI register structure.
 * @return uint32_t Window address from RX FIFO.
 */
ATTRIBUTE_INLINE uint32_t wm_ll_spi_get_rx_address(const wm_spi_reg_t *hw)
{
    return (uint32_t) & (hw->rx_data.rx_addr);
}

/**
 * @brief Set the window address in SPI receive data register.
 *
 * @param hw Pointer to SPI register structure.
 * @param val Window address value to be set.
 */
ATTRIBUTE_INLINE void wm_ll_spi_set_rx_data_rx_addr(wm_spi_reg_t *hw, uint32_t val)
{
    hw->rx_data.rx_addr = val;
}
#ifdef __cplusplus
}
#endif
#endif /*__WM_LL_SPI_H__*/

/**
 * @file wm_ll_hspi.h
 *
 * @brief LL_HSPI Module
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

#ifndef __WM_LL_HSPI_H__
#define __WM_LL_HSPI_H__

#include "wm_hspi_reg_struct.h"
#include "wm_soc_cfgs.h"
#include "wm_attr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_LL_HSPI_Functions WM LL_HSPI Functions
 * @brief WinnerMicro LL_HSPI Functions
 */

/**
 * @addtogroup WM_LL_HSPI_Functions
 * @{
 */

/**
 * @brief HSPI clear FIFO
 * @param reg Pointer to hspi register structure
 */
ATTRIBUTE_INLINE void wm_ll_hspi_clear_fifo(wm_hspi_slave_reg_t *reg)
{
    reg->clear_fifo.clear_fifos = 1;
}

/**
 * @brief Set HSPI CPOL
 * @param reg Pointer to hspi register structure
 * @param cpol 0: SCK IDLE low; 1: SCK IDLE high
 */
ATTRIBUTE_INLINE void wm_ll_hspi_set_cpol(wm_hspi_slave_reg_t *reg, uint32_t cpol)
{
    reg->cfg.cpol = cpol;
}

/**
 * @brief Get HSPI CPOL status
 * @param reg Pointer to hspi register structure
 * @return 0: SCK IDLE low; 1: SCK IDLE high
 */
ATTRIBUTE_INLINE uint32_t wm_ll_hspi_get_cpol(wm_hspi_slave_reg_t *reg)
{
    return reg->cfg.cpol;
}

/**
 * @brief Set HSPI CPHA
 * @param reg Pointer to hspi register structure
 * @param cpha 0: Data sampled at first edge; 1: Data sampled at second edge
 */
ATTRIBUTE_INLINE void wm_ll_hspi_set_cpha(wm_hspi_slave_reg_t *reg, uint32_t cpha)
{
    reg->cfg.cpha = cpha;
}

/**
 * @brief Get HSPI CPHA status
 * @param reg Pointer to hspi register structure
 * @return 0: Data sampled at first edge; 1: Data sampled at second edge
 */
ATTRIBUTE_INLINE uint32_t wm_ll_hspi_get_cpha(wm_hspi_slave_reg_t *reg)
{
    return reg->cfg.cpha;
}

/**
 * @brief Set HSPI SPI TX always drive
 * @param reg Pointer to hspi register structure
 * @param spi_tx_always_drive 0: SPI output only active when selected; 1: SPI output always active
 */
ATTRIBUTE_INLINE void wm_ll_hspi_set_spi_tx_always_drive(wm_hspi_slave_reg_t *reg, uint32_t spi_tx_always_drive)
{
    reg->cfg.spi_tx_always_drive = spi_tx_always_drive;
}

/**
 * @brief Get HSPI SPI TX always drive status
 * @param reg Pointer to hspi register structure
 * @return 0: SPI output only active when selected; 1: SPI output always active
 */
ATTRIBUTE_INLINE uint32_t wm_ll_hspi_get_spi_tx_always_drive(wm_hspi_slave_reg_t *reg)
{
    return reg->cfg.spi_tx_always_drive;
}

/**
 * @brief Set HSPI big-endian
 * @param reg Pointer to hspi register structure
 * @param byte_order 0: low byte first; 1: high byte first
 */
ATTRIBUTE_INLINE void wm_ll_hspi_set_byte_order(wm_hspi_slave_reg_t *reg, uint32_t byte_order)
{
    reg->cfg.byte_order = byte_order;
}

/**
 * @brief Get HSPI big-endian status
 * @param reg Pointer to hspi register structure
 * @return 0: low byte first; 1: high byte first
 */
ATTRIBUTE_INLINE uint32_t wm_ll_hspi_get_byte_order(wm_hspi_slave_reg_t *reg)
{
    return reg->cfg.byte_order;
}

/**
 * @brief Set HSPI burst length
 * @param reg Pointer to hspi register structure
 * @param burst_len 0: Burst length 1 word; 1: Burst length 4 words
 */
ATTRIBUTE_INLINE void wm_ll_hspi_set_burst_len(wm_hspi_slave_reg_t *reg, uint32_t burst_len)
{
    reg->mode_cfg.burst_len = burst_len;
}

/**
 * @brief Get HSPI burst length status
 * @param reg Pointer to hspi register structure
 * @return 0: Burst length 1 word; 1: Burst length 4 words
 */
ATTRIBUTE_INLINE uint32_t wm_ll_hspi_get_burst_len(wm_hspi_slave_reg_t *reg)
{
    return reg->mode_cfg.burst_len;
}

/**
 * @brief Set HSPI interrupt enable for TX underrun
 * @param reg Pointer to hspi register structure
 * @param int_en_tx_underrun 0: Enable TX underrun interrupt; 1: Disable TX underrun interrupt
 */
ATTRIBUTE_INLINE void wm_ll_hspi_mask_int_tx_underrun(wm_hspi_slave_reg_t *reg, uint32_t int_en_tx_underrun)
{
    reg->int_mask.int_en_tx_underrun = int_en_tx_underrun;
}

/**
 * @brief Get HSPI interrupt enable status for TX underrun
 * @param reg Pointer to hspi register structure
 * @return 0: Enable TX underrun interrupt; 1: Disable TX underrun interrupt
 */
ATTRIBUTE_INLINE uint32_t wm_ll_hspi_get_mask_int_tx_underrun(wm_hspi_slave_reg_t *reg)
{
    return reg->int_mask.int_en_tx_underrun;
}

/**
 * @brief Set HSPI interrupt enable for RX overrun
 * @param reg Pointer to hspi register structure
 * @param int_en_rx_overrun 0: Enable RX overrun interrupt; 1: Disable RX overrun interrupt
 */
ATTRIBUTE_INLINE void wm_ll_hspi_mask_int_rx_overrun(wm_hspi_slave_reg_t *reg, uint32_t int_en_rx_overrun)
{
    reg->int_mask.int_en_rx_overrun = int_en_rx_overrun;
}

/**
 * @brief Get HSPI interrupt enable status for RX overrun
 * @param reg Pointer to hspi register structure
 * @return 0: Enable RX overrun interrupt; 1: Disable RX overrun interrupt
 */
ATTRIBUTE_INLINE uint32_t wm_ll_hspi_get_mask_int_rx_overrun(wm_hspi_slave_reg_t *reg)
{
    return reg->int_mask.int_en_rx_overrun;
}

/**
 * @brief Get HSPI TX underrun status
 * @param reg Pointer to hspi register structure
 * @return 0: No TX underrun; 1: TX underrun occurred
 */
ATTRIBUTE_INLINE uint32_t wm_ll_hspi_get_tx_underrun_status(wm_hspi_slave_reg_t *reg)
{
    return reg->int_sts.tx_underrun;
}

/**
 * @brief Clear HSPI TX underrun status
 * @param reg Pointer to hspi register structure
 */
ATTRIBUTE_INLINE void wm_ll_hspi_clr_tx_underrun_status(wm_hspi_slave_reg_t *reg)
{
    reg->int_sts.tx_underrun = 1;
}

/**
 * @brief Get HSPI RX overrun status
 * @param reg Pointer to hspi register structure
 * @return 0: No RX overrun; 1: RX overrun occurred
 */
ATTRIBUTE_INLINE uint32_t wm_ll_hspi_get_rx_overrun_status(wm_hspi_slave_reg_t *reg)
{
    return reg->int_sts.rx_overrun;
}

/**
 * @brief Clear HSPI RX overrun status
 * @param reg Pointer to hspi register structure
 */
ATTRIBUTE_INLINE void wm_ll_hspi_clr_rx_overrun_status(wm_hspi_slave_reg_t *reg)
{
    reg->int_sts.rx_overrun = 1;
}

/**
 * @brief Set HSPI RX data length
 * @param reg Pointer to hspi register structure
 * @param rx_dat_len Data length to upload in bytes
 */
ATTRIBUTE_INLINE void wm_ll_hspi_set_rx_data_len(wm_hspi_slave_reg_t *reg, uint32_t rx_dat_len)
{
    reg->rx_dat_len.rx_dat_len = rx_dat_len;
}

/**
 * @brief Get HSPI RX data length
 * @param reg Pointer to hspi register structure
 * @return Data length to upload in bytes
 */
ATTRIBUTE_INLINE uint32_t wm_ll_hspi_get_rx_data_len(wm_hspi_slave_reg_t *reg)
{
    return reg->rx_dat_len.rx_dat_len;
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_LL_HSPI_H__ */
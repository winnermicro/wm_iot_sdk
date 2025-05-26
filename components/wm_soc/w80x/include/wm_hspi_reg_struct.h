/**
 * @file wm_hspi_reg_struct.h
 *
 * @brief hspi register struct
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

#ifndef __WM_HSPI_REG_STRUCT_H__
#define __WM_HSPI_REG_STRUCT_H__

#include <stdint.h>
#include "wm_regs_base.h"
#include "wm_soc_cfgs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief hspi clear fifo register
 */
typedef union {
    struct {
        uint32_t clear_fifos : 1;  /**< [0] Clear Tx and Rx FIFOs */
        uint32_t reserved_1  : 31; /**< [31:1] Reserved */
    };
    uint32_t val;
} wm_hspi_clear_fifo_t;

/**
 * @brief hspi configuration register
 */
typedef union {
    struct {
        uint32_t cpol                : 1;  /**< [0] Clock Polarity Configuration */
        uint32_t cpha                : 1;  /**< [1] Clock Phase Configuration */
        uint32_t spi_tx_always_drive : 1;  /**< [2] SPI Output Drive Configuration */
        uint32_t byte_order          : 1;  /**< [3] byte_order Configuration */
        uint32_t reserved_4          : 28; /**< [31:4] Reserved */
    };
    uint32_t val;
} wm_hspi_cfg_t;

/**
 * @brief hspi mode configuration register
 */
typedef union {
    struct {
        uint32_t burst_len  : 1;  /**< [0] Burst Length Configuration */
        uint32_t reserved_1 : 31; /**< [31:1] Reserved */
    };
    uint32_t val;
} wm_hspi_mode_cfg_t;

/**
 * @brief hspi interrupt configuration register
 */
typedef union {
    struct {
        uint32_t int_en_tx_underrun : 1;  /**< [0] Tx Underrun Interrupt Enable */
        uint32_t int_en_rx_overrun  : 1;  /**< [1] Rx Overrun Interrupt Enable */
        uint32_t reserved_2         : 30; /**< [31:2] Reserved */
    };
    uint32_t val;
} wm_hspi_int_mask_t;

/**
 * @brief hspi interrupt status register
 */
typedef union {
    struct {
        uint32_t tx_underrun : 1;  /**< [0] Tx Underrun Status */
        uint32_t rx_overrun  : 1;  /**< [1] Rx Overrun Status */
        uint32_t reserved_2  : 30; /**< [31:2] Reserved */
    };
    uint32_t val;
} wm_hspi_int_sts_t;

/**
 * @brief hspi data upload length register
 */
typedef union {
    struct {
        uint32_t rx_dat_len  : 16; /**< [15:0] Data Upload Length */
        uint32_t reserved_16 : 16; /**< [31:16] Reserved */
    };
    uint32_t val;
} wm_hspi_rx_dat_len_t;

/**
 * @brief hspi register layout
 */
typedef struct {
    volatile wm_hspi_clear_fifo_t clear_fifo; /**< WM_HSPI_BASE_ADDR + 0x0000 */
    volatile wm_hspi_cfg_t cfg;               /**< WM_HSPI_BASE_ADDR + 0x0004 */
    volatile wm_hspi_mode_cfg_t mode_cfg;     /**< WM_HSPI_BASE_ADDR + 0x0008 */
    volatile wm_hspi_int_mask_t int_mask;     /**< WM_HSPI_BASE_ADDR + 0x000C */
    volatile wm_hspi_int_sts_t int_sts;       /**< WM_HSPI_BASE_ADDR + 0x0010 */
    volatile uint32_t reserved_14;            /**< WM_HSPI_BASE_ADDR + 0x0014 */
    volatile wm_hspi_rx_dat_len_t rx_dat_len; /**< WM_HSPI_BASE_ADDR + 0x0018 */
} wm_hspi_slave_reg_t;

#ifdef __cplusplus
}
#endif

#endif /* __WM_HSPI_REG_STRUCT_H__ */
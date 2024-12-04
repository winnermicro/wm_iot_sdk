/**
 * @file wm_sdio_wrapper_reg_struct.h
 *
 * @brief sdio wrapper register struct
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

#ifndef __WM_SDIO_WRAPPER_REG_STRUCT_H__
#define __WM_SDIO_WRAPPER_REG_STRUCT_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief hspi sdio wrapper register interrput status
 */
typedef union {
    struct {
        uint32_t rx_done    : 1;  /* [0] up steam done */
        uint32_t tx_done    : 1;  /* [1] down steam done */
        uint32_t up_cmd     : 1;  /* [2] up cmd done */
        uint32_t down_cmd   : 1;  /* [3] down steam done */
        uint32_t reserved_4 : 28; /* [31:4] reserved */
    };
    uint32_t val;
} wm_sdiow_int_sts_t;

/**
 * @brief hspi sdio wrapper register interrput mask
 */
typedef union {
    struct {
        uint32_t rx_done_msk  : 1;  /* [0] up steam done msk */
        uint32_t tx_done_msk  : 1;  /* [1] down steam done msk */
        uint32_t up_cmd_msk   : 1;  /* [2] up cmd done msk */
        uint32_t down_cmd_msk : 1;  /* [3] down steam done msk */
        uint32_t reserved_4   : 28; /* [31:4] reserved */
    };
    uint32_t val;
} wm_sdiow_int_msk_t;

/**
 * @brief hspi sdio wrapper register up cmd avaliable
 */
typedef union {
    struct {
        uint32_t cmd_avl    : 1;  /* [0] up cmd avaliable */
        uint32_t reserved_1 : 31; /* [31:1] reserved */
    };
    uint32_t val;
} wm_sdiow_up_cmd_avl_t;

/**
 * @brief hspi sdio wrapper register down cmd buf avaliable
 */
typedef union {
    struct {
        uint32_t cmdb_avl   : 1;  /* [0] down cmd buf avaliable */
        uint32_t reserved_1 : 31; /* [31:1] reserved */
    };
    uint32_t val;
} wm_sdiow_dm_cmdb_avl_t;

/**
 * @brief hspi sdio wrapper register sdio tx link enable
 */
typedef union {
    struct {
        uint32_t txl_en     : 1;  /* [0] tx link enable */
        uint32_t reserved_1 : 31; /* [31:1] reserved */
    };
    uint32_t val;
} wm_sdiow_tx_link_en_t;

/**
 * @brief hspi sdio wrapper register sdio tx link address
 */
typedef union {
    struct {
        uint32_t tx_addr : 32; /* [32:0] tx link address */
    };
    uint32_t val;
} wm_sdiow_tx_addr_t;

/**
 * @brief hspi sdio wrapper register sdio tx enable
 */
typedef union {
    struct {
        uint32_t tx_en      : 1;  /* [0] tx enable */
        uint32_t reserved_1 : 31; /* [31:1] reserved */
    };
    uint32_t val;
} wm_sdiow_tx_en_t;

/**
 * @brief hspi sdio wrapper register sdio tx status
 */
typedef union {
    struct {
        uint32_t tx_sts     : 1;  /* [0] tx status */
        uint32_t reserved_1 : 31; /* [31:1] reserved */
    };
    uint32_t val;
} wm_sdiow_tx_sts_t;

/**
 * @brief hspi sdio wrapper register sdio rx link enable
 */
typedef union {
    struct {
        uint32_t rxl_en     : 1;  /* [0] rx link enable */
        uint32_t reserved_1 : 31; /* [31:1] reserved */
    };
    uint32_t val;
} wm_sdiow_rx_link_en_t;

/**
 * @brief hspi sdio wrapper register sdio rx link address
 */
typedef union {
    struct {
        uint32_t rx_addr : 32; /* [31:0] rx link address */
    };
    uint32_t val;
} wm_sdiow_rx_addr_t;

/**
 * @brief hspi sdio wrapper register sdio rx enable
 */
typedef union {
    struct {
        uint32_t rx_en      : 1;  /* [0] rx enable */
        uint32_t reserved_1 : 31; /* [31:1] reserved */
    };
    uint32_t val;
} wm_sdiow_rx_en_t;

/**
 * @brief hspi sdio wrapper register sdio rx status
 */
typedef union {
    struct {
        uint32_t rx_sts     : 1;  /* [0] rx status */
        uint32_t reserved_1 : 31; /* [31:1] reserved */
    };
    uint32_t val;
} wm_sdiow_rx_sts_t;

/**
 * @brief hspi sdio wrapper register cmd buf size
 */
typedef union {
    struct {
        uint32_t buf_size    : 12; /* [11:0] rx status */
        uint32_t reserved_12 : 20; /* [31:12] reserved */
    };
    uint32_t val;
} wm_sdiow_cmd_buf_size_t;

/**
 * @brief hspi sdio wrapper register layout
 */
typedef struct {
    volatile wm_sdiow_int_sts_t int_sts;           /**< WM_HSPI_SDIO_WRAPPER_BASE_ADDR + 0x00 */
    volatile wm_sdiow_int_msk_t int_msk;           /**< WM_HSPI_SDIO_WRAPPER_BASE_ADDR + 0x04 */
    volatile wm_sdiow_up_cmd_avl_t up_cmd_avl;     /**< WM_HSPI_SDIO_WRAPPER_BASE_ADDR + 0x08 */
    volatile wm_sdiow_dm_cmdb_avl_t dm_cmdb_avl;   /**< WM_HSPI_SDIO_WRAPPER_BASE_ADDR + 0x0C */
    volatile wm_sdiow_tx_link_en_t tx_link_en;     /**< WM_HSPI_SDIO_WRAPPER_BASE_ADDR + 0x10 */
    volatile wm_sdiow_tx_addr_t tx_link_addr;      /**< WM_HSPI_SDIO_WRAPPER_BASE_ADDR + 0x14 */
    volatile wm_sdiow_tx_en_t tx_start;            /**< WM_HSPI_SDIO_WRAPPER_BASE_ADDR + 0x18 */
    volatile wm_sdiow_tx_sts_t tx_sts;             /**< WM_HSPI_SDIO_WRAPPER_BASE_ADDR + 0x1C */
    volatile wm_sdiow_rx_link_en_t rx_link_en;     /**< WM_HSPI_SDIO_WRAPPER_BASE_ADDR + 0x20 */
    volatile wm_sdiow_rx_addr_t rx_link_addr;      /**< WM_HSPI_SDIO_WRAPPER_BASE_ADDR + 0x24 */
    volatile wm_sdiow_rx_en_t rx_start;            /**< WM_HSPI_SDIO_WRAPPER_BASE_ADDR + 0x28 */
    volatile wm_sdiow_rx_sts_t rx_sts;             /**< WM_HSPI_SDIO_WRAPPER_BASE_ADDR + 0x2C */
    volatile uint32_t cmd_baddr;                   /**< WM_HSPI_SDIO_WRAPPER_BASE_ADDR + 0x30 */
    volatile wm_sdiow_cmd_buf_size_t cmd_buf_size; /**< WM_HSPI_SDIO_WRAPPER_BASE_ADDR + 0x34 */
} wm_sdio_wrapper_reg_t;

#ifdef __cplusplus
}
#endif

#endif /* __WM_SDIO_WRAPPER_REG_STRUCT_H__ */

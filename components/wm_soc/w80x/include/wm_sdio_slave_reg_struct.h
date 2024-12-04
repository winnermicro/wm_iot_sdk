/**
 * @file wm_sdio_slave_reg_struct.h
 *
 * @brief sdio slave register struct
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

#ifndef __WM_SDIO_SLAVE_REG_STRUCT_H__
#define __WM_SDIO_SLAVE_REG_STRUCT_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief sdio slave ahb transfer cnt
 */
typedef union {
    struct {
        uint32_t ahb_xcnt    : 21; /* [20:0] ahb transfer cnt */
        uint32_t reserved_21 : 11; /* [31:21] reserved */
    };
    uint32_t val;
} wm_sdio_slave_ahb_xfer_t;

/**
 * @brief sdio slave sdio transfer cnt
 */
typedef union {
    struct {
        uint32_t sdio_xcnt   : 21; /* [20:0] sdio transfer cnt */
        uint32_t reserved_21 : 11; /* [31:21] reserved */
    };
    uint32_t val;
} wm_sdio_slave_sdio_xfer_t;

/**
 * @brief sdio slave cia register
 */
typedef union {
    struct {
        uint32_t cccr_revision : 4; /* [3:0] cccr revsion */
        uint32_t sdio_revision : 4; /* [7:4] sdio spec version support */
        uint32_t sd_revision   : 4; /* [11:8] sd spec version support */
        uint32_t io_code       : 4; /* [15:12] io device code  */
        uint32_t csa_sup       : 1; /* [16] csa support */
        uint32_t ex_io_code    : 8; /* [24:17] extend io device code */
        uint32_t sps           : 1; /* [25] sps supprt */
        uint32_t shs           : 1; /* [26] high speed support */
        uint32_t reserved_27   : 5; /* [31:27] reserved */
    };
    uint32_t val;
} wm_sdio_slave_cia_t;

/**
 * @brief sdio slave program register
 */
typedef union {
    struct {
        uint32_t fn1_ready      : 1;  /* [0] function 1 ready */
        uint32_t fn1_read_ready : 1;  /* [1] function 1 read data ready */
        uint32_t scsi           : 1;  /* [2] cintinues int support */
        uint32_t sdc            : 1;  /* [3] cmd52 work with data transfer  */
        uint32_t smb            : 1;  /* [4] cmd53 block mode */
        uint32_t srw            : 1;  /* [5] read wait */
        uint32_t sbs            : 1;  /* [6] suspend reuse */
        uint32_t s4mi           : 1;  /* [7] int 4bit block mode */
        uint32_t lsc            : 1;  /* [8] low speed device */
        uint32_t bls4           : 1;  /* [9] low speed device that support 4 bit */
        uint32_t card_ready     : 1;  /* [10] card ready afrer power on reset */
        uint32_t reserved_11    : 21; /* [31:11] reserved */
    };
    uint32_t val;
} wm_sdio_slave_pr_t;

/**
 * @brief sdio slave ocr register
 */
typedef union {
    struct {
        uint32_t work_vo     : 24; /* [23:0] work vol */
        uint32_t reserved_24 : 8;  /* [31:24] reserved */
    };
    uint32_t val;
} wm_sdio_slave_ocr_t;

/**
 * @brief sdio slave cd state register
 */
typedef union {
    struct {
        uint32_t sdd3       : 1;  /* [0] data3 state */
        uint32_t reserved_1 : 31; /* [31:1] reserved */
    };
    uint32_t val;
} wm_sdio_slave_cds_t;

/**
 * @brief sdio slave f1 enable register
 */
typedef union {
    struct {
        uint32_t fn1_en     : 1;  /* [0] function1 enable */
        uint32_t reserved_1 : 31; /* [31:1] reserved */
    };
    uint32_t val;
} wm_sdio_slave_f1e_t;

/**
 * @brief sdio slave ahb register layout
 */
typedef struct {
    volatile uint32_t reserved1[2];               /**< WM_SDIO_SLAVE_AHB_BASE_ADDR + 0x0000 */
    volatile uint32_t cis_f0_addr;                /**< WM_SDIO_SLAVE_AHB_BASE_ADDR + 0x0008 */
    volatile uint32_t cis_f1_addr;                /**< WM_SDIO_SLAVE_AHB_BASE_ADDR + 0x000C */
    volatile uint32_t csa_addr;                   /**< WM_SDIO_SLAVE_AHB_BASE_ADDR + 0x0010, not supported in w800 */
    volatile uint32_t read_addr;                  /**< WM_SDIO_SLAVE_AHB_BASE_ADDR + 0x0014, not supported in w800 */
    volatile uint32_t write_addr;                 /**< WM_SDIO_SLAVE_AHB_BASE_ADDR + 0x0018, not supported in w800 */
    volatile wm_sdio_slave_ahb_xfer_t ahb_xfer;   /**< WM_SDIO_SLAVE_AHB_BASE_ADDR + 0x001C */
    volatile wm_sdio_slave_sdio_xfer_t sdio_xfer; /**< WM_SDIO_SLAVE_AHB_BASE_ADDR + 0x0020 */
    volatile wm_sdio_slave_cia_t cia;             /**< WM_SDIO_SLAVE_AHB_BASE_ADDR + 0x0024 */
    volatile wm_sdio_slave_pr_t pr;               /**< WM_SDIO_SLAVE_AHB_BASE_ADDR + 0x0028 */
    volatile uint32_t reserved2;                  /**< WM_SDIO_SLAVE_AHB_BASE_ADDR + 0x0030 */
    volatile wm_sdio_slave_ocr_t ocr;             /**< WM_SDIO_SLAVE_AHB_BASE_ADDR + 0x0034 */
    volatile uint32_t reserved3;                  /**< WM_SDIO_SLAVE_AHB_BASE_ADDR + 0x0038 */
    volatile wm_sdio_slave_cds_t cd_state;        /**< WM_SDIO_SLAVE_AHB_BASE_ADDR + 0x003C */
    volatile wm_sdio_slave_f1e_t f1_en;           /**< WM_SDIO_SLAVE_AHB_BASE_ADDR + 0x0040 */
} wm_sdio_slave_reg_t;

#ifdef __cplusplus
}
#endif

#endif /* __SDIO_SLAVE_REG_STRUCT_H__ */

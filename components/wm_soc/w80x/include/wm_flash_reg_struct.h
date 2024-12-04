/**
 * @file wm_flash_reg_struct.h
 *
 * @brief flash registers struct Module
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

#ifndef __WM_FLASH_REG_STRUCT_H__
#define __WM_FLASH_REG_STRUCT_H__

#include "wm_types.h"
#include "wm_regs_base.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief flash command information register layout
 * This structure is used to show command information
 */
typedef union {
    struct {
        uint32_t cmd         : 8;  /*[7:0] operation command code*/
        uint32_t is_ce       : 1;  /*[8] indicate current write command is CE */
        uint32_t is_hbe      : 1;  /*[9] indicate current write command is hbe*/
        uint32_t is_be       : 1;  /*[10] indicate current write command is BE */
        uint32_t is_se       : 1;  /*[11] indicate current write command is SE */
        uint32_t is_pp       : 1;  /*[12] indicate current write command is PP */
        uint32_t is_wrsr     : 1;  /*[13] indicate current write command is WRSR*/
        uint32_t is_read_cmd : 1;  /*[14] indicate is read cmd*/
        uint32_t has_data    : 1;  /*[15] indicate has data in cmd_info*/
        uint32_t data_size   : 10; /*[25:16] indicate data size in cmd_info*/
        uint32_t dummy_num   : 3;  /*[28:26] indicate dummy number in cmd_info*/
        uint32_t has_dummy   : 1;  /*[29] indicate has dummy in cmd_info*/
        uint32_t has_crm     : 1;  /*[30] indicate has crm in cmd_start*/
        uint32_t has_address : 1;  /*[31] indicate has address in cmd_start*/
    };
    uint32_t val;
} wm_flash_cmd_t_info;

/**
 * @brief flash command start register layout
 * This structure is used to start command
 */
typedef union {
    struct {
        uint32_t crm        : 8;  /* [7:0]store CRM */
        uint32_t start      : 1;  /* [8]start command */
        uint32_t reserved_9 : 23; /* [31:9] Reserved */
    };
    uint32_t val;
} wm_flash_cmd_start_t;

/**
 * @brief flash control register layout
 * This structure is used to contrl operation mode
 */
typedef union {
    struct {
        uint32_t qiom       : 1;  /* [0]  1:QIOM(dual spi, quad spi), 0: stand SPI */
        uint32_t qpim       : 1;  /* [1]  1:QPIM(QPI), 0: qio(spi), Quad Peripheral Interface*/
        uint32_t tshs       : 2;  /* [3:2] tshs value */
        uint32_t mode       : 2;  /* [5:4] mode */
        uint32_t reserved_6 : 26; /* [31:6] Reserved */
    };
    uint32_t val;
} wm_flash_ctrl_t;

/**
 * @brief flash remap register layout
 * This structure is used to control reamp
 */
typedef union {
    struct {
        uint32_t cntrl      : 1;  /* [0]  1: remap address is ROM,  0: is flash */
        uint32_t reserved_1 : 31; /* [31:1] Reserved */
    };
    uint32_t val;
} wm_flash_remap_t;

/**
 * @brief flash ADDR register layout
 * This structure is used to set operation address
 */
typedef union {
    struct {
        uint32_t address; //operation address
    };
    uint32_t val;
} wm_flash_addr_t;

/**
 * @brief decrypt register layout
 * This structure is used to set decrypt mode
 */
typedef union {
    struct {
        uint32_t reserved_0       : 1;  /*[0]   Reserved */
        uint32_t code_ahb_decrypt : 1;  /*[1]   1: decrypt AHB code, 0: donot decrypt AHB code*/
        uint32_t data_decrypt     : 1;  /*[2]   1: decrypt data read by register, 0: donot decrypt data read by register*/
        uint32_t data_ahb_decrypt : 1;  /*[3]   1: decrypt AHB data, 0: donot decrypt AHB data*/
        uint32_t exptsel          : 3;  /*[6:4] 1: group 2 as private key  0: group1 as privte key */
        uint32_t expt_start       : 1;  /*[7]   1: start RSA decrypt*/
        uint32_t reserved_8       : 24; /*[31:8] Reserved */
    };
    uint32_t val;
} wm_flash_decrypt_crtl_t;

/**
 * @brief decrypt status register layout
 * This structure is used to show decrypt status
 */
typedef union {
    struct {
        uint32_t rsa_mode   : 1;  /* [0] 1: encrypyt ongoing,  0: encrypt done*/
        uint32_t key_ready  : 1;  /* [1] key ready */
        uint32_t key_err    : 1;  /* [2] key error */
        uint32_t reserved_3 : 29; /* [31:3] Reserved */
    };
    uint32_t val;
} wm_flash_decrypt_status_t;

/**
 * @brief i2c register layout
 * Notice: must 32bit for each member
 */
typedef struct {
    volatile wm_flash_cmd_t_info cmd_info;             /*WM_FLASH_BASE_ADDR*/
    volatile wm_flash_cmd_start_t cmd_start;           /*WM_FLASH_BASE_ADDR + 0x0004*/
    volatile wm_flash_ctrl_t cntrl;                    /*WM_FLASH_BASE_ADDR + 0x0008*/
    volatile wm_flash_remap_t remap;                   /*WM_FLASH_BASE_ADDR + 0x000C*/
    volatile wm_flash_addr_t addr;                     /*WM_FLASH_BASE_ADDR + 0x0010*/
    volatile wm_flash_decrypt_crtl_t decrypt_crtl;     /*WM_FLASH_BASE_ADDR + 0x0014*/
    volatile wm_flash_decrypt_status_t decrypt_status; /*WM_FLASH_BASE_ADDR + 0x0018*/
} wm_flash_reg_t;

#ifdef __cplusplus
}
#endif

#endif /* __WM_FLASH_REG_STRUCT_H__*/

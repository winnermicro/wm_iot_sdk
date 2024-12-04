/**

@file wm_hal_iflash_chips.h
*
@brief flash Driver Module
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

#ifndef __WM_HAL_IFLASH_CHIPS_H__
#define __WM_HAL_IFLASH_CHIPS_H__

#include "wm_flash_reg_struct.h"
#include "wm_ll_flash.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SUPPORT_STAND_SPI      (1 << 0)
#define SUPPORT_DUAL_SPI       (1 << 1)
#define SUPPORT_QUAD_SPI       (1 << 2)
#define SUPPORT_QPI            (1 << 3)
#define SUPPORT_DTR            (1 << 4)

#define WM_IFLASH_UNSPPORT_CMD 0xFF

typedef struct {
    uint8_t commd_id;
    uint8_t dummy_bit;
} flash_cmd_t;

typedef struct {
    flash_cmd_t wren;      //write enable
    flash_cmd_t pp;        //program page
    flash_cmd_t rdid;      //read identification
    flash_cmd_t rdsr1;     //read status register1
    flash_cmd_t wrsr1;     //read status register2
    flash_cmd_t prscur;    //program secturity register
    flash_cmd_t pe;        //page erase
    flash_cmd_t se;        //sectore erase
    flash_cmd_t ce;        //chip erase
    flash_cmd_t read;      //read data
    flash_cmd_t fread;     //fast read
    flash_cmd_t two_read;  //dual I/O fast read
    flash_cmd_t four_read; //quad I/O fast read
    flash_cmd_t ruid;      //read unique ID
} flash_cmd_set_t;

typedef struct {
    const char *name;
    uint8_t vendor_id;
    uint8_t support_mode;
    uint32_t page_size;
    uint32_t sector_size;
    uint32_t block_size;
    uint32_t unique_id[FLASH_UNIQUE_ID_SIZE / 4];
    flash_cmd_set_t cmd_set;
} wm_flash_t;

static const wm_flash_t wm_iflash_chip_p25q = {
    .name = "flash_p25q16",
    .vendor_id = 0x85,
    .support_mode = (SUPPORT_STAND_SPI | SUPPORT_DUAL_SPI | SUPPORT_QUAD_SPI | SUPPORT_QPI | SUPPORT_DTR),
    .page_size = 256,
    .sector_size = 4 * 1024,
    .block_size = 32 * 1024,
    .cmd_set = {
        .wren = {0x06, 0},
        .pp = {0x2, 0},
        .rdid = {0x9F, 0},
        .rdsr1 = {0x5, 0},
        .wrsr1 = {0x1, 0},
        .prscur = {0x42, 0},
        .pe = {0x81, 0},
        .se = {0x20, 0},
        .ce = {0x60, 0},
        .read = {0x3, 0},
        .fread = {0xB, 1},
        .two_read = {0xBB, 2},
        .four_read = {0xEB, 5},
        .ruid = {0x4B, 4},
    },
};

static const wm_flash_t wm_iflash_chip_th25q = {
    .name = "flash_th25q",
    .vendor_id = 0xEB,
    .support_mode = (SUPPORT_STAND_SPI | SUPPORT_DUAL_SPI | SUPPORT_QUAD_SPI),
    .page_size = 256,
    .sector_size = 4 * 1024,
    .block_size = 32 * 1024,
    .cmd_set = {
        .wren = {0x06, 0},
        .pp = {0x2, 0},
        .rdid = {0x9F, 0},
        .rdsr1 = {0x5, 0},
        .wrsr1 = {0x1, 0},
        .prscur = {0x42, 0},
        .pe = {0x81, 0},
        .se = {0x20, 0},
        .ce = {0x60, 0},
        .read = {0x3, 0},
        .fread = {0xB, 1},
        .two_read = {0xBB, 0},
        .four_read = {0xEB, 4},
        .ruid = {0x4B, 4},
    },
};

static const wm_flash_t wm_iflash_chip_by25q = {
    .name = "flash_by25116bs",
    .vendor_id = 0x68,
    .support_mode = (SUPPORT_STAND_SPI | SUPPORT_DUAL_SPI | SUPPORT_QUAD_SPI | SUPPORT_QPI),
    .page_size = 256,
    .sector_size = 4 * 1024,
    .block_size = 32 * 1024,
    .cmd_set = {
        .wren = {0x06, 0},
        .pp = {0x2, 0},
        .rdid = {0x9F, 0},
        .rdsr1 = {0x5, 0},
        .wrsr1 = {0x1, 0},
        .prscur = {0x42, 0},
        .pe = {WM_IFLASH_UNSPPORT_CMD, 0},
        .se = {0x20, 0},
        .ce = {0x60, 0},
        .read = {0x3, 0},
        .fread = {0xB, 1},
        .two_read = {0xBB, 0},
        .four_read = {0xEB, 2},
        .ruid = {0x4B, 4},
    },
};

static const wm_flash_t wm_iflash_chip_xt25f = {
    .name = "flash_xt25f",
    .vendor_id = 0xB,
    .support_mode = (SUPPORT_STAND_SPI | SUPPORT_DUAL_SPI | SUPPORT_QUAD_SPI),
    .page_size = 256,
    .sector_size = 4 * 1024,
    .block_size = 32 * 1024,
    .cmd_set = {
        .wren = {0x06, 0},
        .pp = {0x2, 0},
        .rdid = {0x9F, 0},
        .rdsr1 = {0x5, 0},
        .wrsr1 = {0x1, 0},
        .prscur = {WM_IFLASH_UNSPPORT_CMD, 0},
        .pe = {WM_IFLASH_UNSPPORT_CMD, 0},
        .se = {0x20, 0},
        .ce = {0x60, 0},
        .read = {0x3, 0},
        .fread = {0xB, 1},
        .two_read = {0xBB, 0},
        .four_read = {0xEB, 2},
        .ruid = {0x4B, 3},
    },
};

//for unknown chip, note: it maybe works abnormally
static const wm_flash_t wm_iflash_chip_unknown = {
    .name = "flash_unknown",
    .vendor_id = 0xFF,
    .support_mode = (SUPPORT_STAND_SPI | SUPPORT_DUAL_SPI | SUPPORT_QUAD_SPI ),
    .page_size = 256,
    .sector_size = 4 * 1024,
    .block_size = 32 * 1024,
    .cmd_set = {
        .wren = {0x06, 0},
        .pp = {0x2, 0},
        .rdid = {0x9F, 0},
        .rdsr1 = {0x5, 0},
        .wrsr1 = {0x1, 0},
        .prscur = {0x42, 0},
        .pe = {WM_IFLASH_UNSPPORT_CMD, 0},
        .se = {0x20, 0},
        .ce = {0x60, 0},
        .read = {0x3, 0},
        .fread = {0xB, 1},
        .two_read = {0xBB, 0},
        .four_read = {0xEB, 2},
        .ruid = {0x4B, 4},
    },
};

static const wm_flash_t *wm_iflash_support_chips[] = {
    &wm_iflash_chip_p25q,
    &wm_iflash_chip_th25q,
    &wm_iflash_chip_by25q,
    &wm_iflash_chip_xt25f,
};

#ifdef __cplusplus
}
#endif

#endif

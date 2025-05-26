/**
 * @file wm_drv_ops_external_flash_support_chips.h
 *
 * @brief extern flash drv module
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
#ifndef __WM_DRV_EFLASH_CHIPS_H__
#define __WM_DRV_EFLASH_CHIPS_H__
#include "wm_drv_ops_external_flash_spi.h"

#ifdef __cplusplus
extern "C" {
#endif

static const wm_eflash_t wm_external_flash_chip_p25q = {
    .name = "flash_w25q",
    .vendor_id = 0x85,
    .support_mode = (SUPPORT_STAND_SPI | SUPPORT_DUAL_SPI | SUPPORT_QUAD_SPI | SUPPORT_QPI | SUPPORT_DTR),
    .addr_width = 3,
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
        .pe = {WM_FLS_UNSUPPORT_CMD, 0},
        .se = {0x20, 0},
        .hbe = {0xD8, 0},
        .ce = {0x60, 0},
        .read = {0x3, 0},
        .fread = {0xB, 1},
        .two_read = {0xBB, 1},
        .four_read = {0xEB, 1},
        .ruid = {0x4B, 4},
    },
};

static const wm_eflash_t wm_external_flash_chip_gd25q = {
    .name = "flash_gd25q",
    .vendor_id = 0xC8,
    .support_mode = (SUPPORT_STAND_SPI | SUPPORT_DUAL_SPI | SUPPORT_QUAD_SPI),
    .addr_width = 3,
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
        .pe = {WM_FLS_UNSUPPORT_CMD, 0},
        .se = {0x20, 0},
        .hbe = {0xD8, 0},
        .ce = {0x60, 0},
        .read = {0x3, 0},
        .fread = {0xB, 1},
        .two_read = {0xBB, 0},
        .four_read = {0xEB, 1},
        .ruid = {0x4B, 4},
    },
};

static const wm_eflash_t wm_external_flash_chip_xt25f = {
    .name = "flash_xt25f",
    .vendor_id = 0xB,
    .support_mode = (SUPPORT_STAND_SPI | SUPPORT_DUAL_SPI),
    .addr_width = 3,
    .page_size = 256,
    .sector_size = 4 * 1024,
    .block_size = 32 * 1024,
    .cmd_set = {
        .wren = {0x06, 0},
        .pp = {0x2, 0},
        .rdid = {0x9F, 0},
        .rdsr1 = {0x5, 0},
        .wrsr1 = {0x1, 0},
        .prscur = {WM_FLS_UNSUPPORT_CMD, 0},
        .pe = {WM_FLS_UNSUPPORT_CMD, 0},
        .se = {0x20, 0},
        .hbe = {0xD8, 0},
        .ce = {0x60, 0},
        .read = {0x3, 0},
        .fread = {0xB, 1},
        .two_read = {0xBB, 0},
        .four_read = {WM_FLS_UNSUPPORT_CMD, 1},
        .ruid = {0x4B, 3},
    },
};

static const wm_eflash_t wm_external_flash_chip_th25q = {
    .name = "flash_th25q",
    .vendor_id = 0xEB,
    .support_mode = (SUPPORT_STAND_SPI | SUPPORT_DUAL_SPI | SUPPORT_QUAD_SPI),
    .addr_width = 3,
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
        .hbe = {0xD8, 0},
        .ce = {0x60, 0},
        .read = {0x3, 0},
        .fread = {0xB, 1},
        .two_read = {0xBB, 0},
        .four_read = {0xEB, 4},
        .ruid = {0x4B, 4},
    },
};

static const wm_eflash_t *wm_external_flash_chips[] = {
    &wm_external_flash_chip_p25q,
    &wm_external_flash_chip_gd25q,
    &wm_external_flash_chip_xt25f,
    &wm_external_flash_chip_th25q,
};

#ifdef __cplusplus
}
#endif

#endif

/**
 * @file wm_psram_reg_struct.h
 *
 * @brief register psram Module
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

#ifndef __WM_PSRAM_REG_STRUCT_H__
#define __WM_PSRAM_REG_STRUCT_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Psram control register
 * This union is used to control psram
 */
typedef union {
    struct {
        uint32_t rst          : 1; /**< [0] psram reset                       */
        uint32_t quad         : 1; /**< [1] enable quad mode, 0: spi, 1 :qspi */
        uint32_t inc_en_burst : 1; /**< [2]  Enable burst on AHB              */
        uint32_t reserved_3   : 1; /**< [3]  reserved                         */
        uint32_t division     : 4; /**< [7:4] SPI frequency division setting
                                            Can only be configured with a value of 2 or higher, and the written value is the
                                            multiple of frequency division*/

        uint32_t tcph         : 3; /**< [10:8] tCPH, CS high-level minimum time setting, measured in AHB clock cycles, must
                                            be greater than 1. The specific time can be set according to the instructions in
                                            different PSRAM manuals. If unclear, the default value can be left unchanged */
        uint32_t hsm          : 1; /**< [11] HSM, Halfsleep mode enabled
                                            1: Enable PSRAM semi sleep mode
                                            0: Clear semi sleep mode  */

        uint32_t reserved_12  : 20; /**< [31:12] Reserved  */
    };
    uint32_t val;
} wm_psram_ctrl_t;

/**
 * @brief psram overtimer
 */
typedef union {
    struct {
        uint32_t cs_timeout  : 12; /**< [11:0] Time out register setting,
                                    set CS to a low maximum time for BURST mode*/
        uint32_t reserved_12 : 20; /**< [31:12] */
    };
    uint32_t val;
} wm_psram_over_timer_t;

/**
 * @brief uart register layout
 */
typedef struct {
    volatile wm_psram_ctrl_t ctrl;             /**< WM_PSRAM_BASE_ADDR          */
    volatile wm_psram_over_timer_t over_timer; /**< WM_PSRAM_BASE_ADDR + 0x004  */
} wm_psram_reg_t;

#ifdef __cplusplus
}
#endif

#endif /**< _WM_PSRAM_REG_STRUCT_H */

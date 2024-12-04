/**
 * @file wm_touch_sensor_reg_struct.h
 *
 * @brief touch sensor registers struct Module
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

#ifndef __WM_TOUCH_SENSOR_REG_STRUCT_H__
#define __WM_TOUCH_SENSOR_REG_STRUCT_H__

#include "wm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Touch sensor control register layout
 * This structure is used to control touch sensor
 */
typedef union {
    struct {
        uint32_t enable        : 1;  /* [0]      Touch sensor controller enable bit ; 1 for enable, 0 for disable */
        uint32_t reserved_1    : 3;  /* [3:1]    Reserved */
        uint32_t key_selection : 16; /* [19:4]   key selection; The touch key status of the corresponding bit is scanned.
                                                    0x0000: Do not scan;
                                                    0x0001: Scan the first key;
                                                    0x0002: Scan the second key;
                                                    0x0003: Scan the first and second keys;
                                                    ...
                                                    0xFFFF: Scan all 16 keys*/
        uint32_t capdet_cnt    : 6;  /* [25:20] The number of CAPDET output pulses is chosen as the counting window. */
        uint32_t scan_period   : 6;  /* [31:26] Scan period (16ms) For example, if scan_period is set to 6'd10,
                                                    keystrokes will be scanned every 160ms State */
    };
    uint32_t val;
} wm_touch_sensor_cr_t;

/**
 * @brief Touch key single-way control register layout
 * This structure is used to control single touch sensor
 */
typedef union {
    struct {
        uint32_t threshold_val : 7;  /* [6:0]   Touch sensor x threshold value */
        uint32_t reserved_7    : 1;  /* [7]     Reserved Read only */
        uint32_t count_val     : 15; /* [22:8]  Touch sensor x count value  Read only */
        uint32_t reserved_23   : 9;  /* [23:31] Reserved */
    };
    uint32_t val;
} wm_touch_sensor_single_t;

/**
 * @brief Touch key interrupt the control register layout
 * This structure is used to control touch sensor interrupt
 */
typedef union {
    struct {
        uint32_t int_source : 16; /* [15:0]  bit 1 indicates corresponding PAD is triggered
                                                   bit 0 indicates corresponding PAD is not triggered
                                                   If you write 1, it will set to 0 */
        uint32_t int_en     : 16; /* [31:16] bit 1 indicates corresponding IO is enable to interrupt
                                                   bit 0 indicates corresponding IO is not disable to interrupt  */
    };
    uint32_t val;
} wm_touch_sensor_int_t;

/**
 * @brief touch register layout
 * must 32bit for each member
 * @note WM_TOUCH_REG_BASE_ADDR = WM_APB_BASE_ADDR + 0x2400
 */
typedef struct {
    volatile wm_touch_sensor_cr_t touch_cr;     /* WM_TOUCH_REG_BASE_ADDR + 0x00 */
    volatile wm_touch_sensor_single_t tc_x[16]; /* WM_TOUCH_REG_BASE_ADDR + 0x04 */
    volatile wm_touch_sensor_int_t touch_int;   /* WM_TOUCH_REG_BASE_ADDR + 0x44 */
} wm_touch_sensor_reg_t;

#ifdef __cplusplus
}
#endif

#endif /* __WM_TOUCH_SENSOR_REG_STRUCT_H__ */

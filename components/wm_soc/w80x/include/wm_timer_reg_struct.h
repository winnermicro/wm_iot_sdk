/**
 * @file wm_timer_reg_struct.h
 *
 * @brief register timer Module
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

#ifndef __WM_TIMER_REG_STRUCT_H__
#define __WM_TIMER_REG_STRUCT_H__

#include <stdint.h>
#include "wm_soc_cfgs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief timer config
 */
typedef union {
    struct {
        uint32_t source_clk  : 7;
        uint32_t reserved_25 : 25;
    };
    uint32_t val;
} wm_timer_config_t;

/**
 * @brief timer0-5 csr
 */
#define WM_TIMER_MS_UNIT(n)     (1UL << (0 + 5 * n))
#define WM_TIMER_AUTO_RELOAD(n) (1UL << (1 + 5 * n))
#define WM_TIMER_EN(n)          (1UL << (2 + 5 * n))
#define WM_TIMER_INT_EN(n)      (1UL << (3 + 5 * n))
#define WM_TIMER_INT_CLR(n)     (1UL << (4 + 5 * n))

/**
 * @brief uart register layout
 */
typedef struct {
    volatile wm_timer_config_t timer_config;             /* WM_TIMER_BASE_ADDR          */
    volatile uint32_t timer_csr;                         /* WM_TIMER_BASE_ADDR + 0x004  */
    volatile uint32_t timer_set_value[WM_TIMER_ID_MAX];  /* WM_TIMER_BASE_ADDR + 0x008  */
    volatile uint32_t timer_curr_value[WM_TIMER_ID_MAX]; /* WM_TIMER_BASE_ADDR + 0x020  */
} wm_timer_reg_t;

#ifdef __cplusplus
}
#endif

#endif /* __WM_TIMER_REG_STRUCT_H__ */

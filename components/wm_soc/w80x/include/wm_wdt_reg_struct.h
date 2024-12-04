/**
 * @file wm_wdt_reg_struct.h
 *
 * @brief Watchdog timer (WDT) Module
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

#ifndef WM_WDT_REG_STRUCT_H
#define WM_WDT_REG_STRUCT_H

#include "wm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief WDT timer load value register
 * This union is used to configure the load value for the watchdog timer,
 * which is used for periodic reload.
 */
typedef union {
    struct {
        uint32_t load_value : 32; /**< [31:0] Load value for watchdog timer */
    };
    uint32_t val; /**< The raw value of the register */
} wm_wdt_timer_load_t;

/**
 * @brief WDT current value register
 * This union is used to get the current value of the watchdog timer.
 */
typedef union {
    struct {
        uint32_t current_value : 32; /**< [31:0] Current value of watchdog timer */
    };
    uint32_t val; /**< The raw value of the register */
} wm_wdt_timer_value_t;

/**
 * @brief WDT control register
 * This union is used to control the watchdog timer behavior.
 */
typedef union {
    struct {
        uint32_t enable     : 1;  /**< [0] Watchdog enable,
                                      1: Timer working, generating periodic interrupts.
                                      0: Timer not working */
        uint32_t rst_enable : 1;  /**< [1] Watchdog reset enable,
                                      1: When the WDT reset condition is generated, a reset signal is generated
                                      0: When the WDT reset condition is generated, no reset signal is generated */
        uint32_t reserved_2 : 30; /**< [31:2] Reserved bits */
    };
    uint32_t val; /**< The raw value of the register */
} wm_wdt_ctrl_t;

/**
 * @brief WDT interrupt clear register
 * This union is used to clear the watchdog interrupt by writing a 1 to the register.
 */
typedef union {
    struct {
        uint32_t int_clear  : 1;  /**< [0] Watchdog interrupt clear */
        uint32_t reserved_1 : 31; /**< [31:1] Reserved bits */
    };
    uint32_t val; /* The raw value of the register */
} wm_wdt_int_clear_t;

/**
 * @brief WDT interrupt source register
 * This union is used to indicate the source of the watchdog interrupt.
 */
typedef union {
    struct {
        uint32_t int_src    : 1;  /**< [0] Watchdog interrupt source */
        uint32_t reserved_1 : 31; /**< [31:1] Reserved bits */
    };
    uint32_t val; /* The raw value of the register */
} wm_wdt_int_src_t;

/**
 * @brief WDT interrupt state register
 * This union is used to reflect the state of the watchdog interrupt.
 */
typedef union {
    struct {
        uint32_t int_state  : 1;  /**< [0] Watchdog interrupt state */
        uint32_t reserved_1 : 31; /**< [31:1] Reserved bits */
    };
    uint32_t val; /**< The raw value of the register */
} wm_wdt_int_state_t;

/**
 * @brief WDT register block
 * This structure is used to encapsulate the watchdog timer's registers.
 */
typedef struct {
    volatile wm_wdt_timer_load_t load;     /**< WM_WDT_BASE_ADDR         */
    volatile wm_wdt_timer_value_t value;   /**< WM_WDT_BASE_ADDR + 0x004 */
    volatile wm_wdt_ctrl_t ctrl;           /**< WM_WDT_BASE_ADDR + 0x008 */
    volatile wm_wdt_int_clear_t int_clr;   /**< WM_WDT_BASE_ADDR + 0x00C */
    volatile wm_wdt_int_src_t int_src;     /**< WM_WDT_BASE_ADDR + 0x010 */
    volatile wm_wdt_int_state_t int_state; /**< WM_WDT_BASE_ADDR + 0x014 */
} wm_wdt_reg_t;

#ifdef __cplusplus
}
#endif

#endif /* WM_WDT_REG_STRUCT_H */

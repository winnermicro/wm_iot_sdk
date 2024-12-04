/**
 * @file wm_ll_wdt.h
 *
 * @brief LL API for Watchdog Timer (WDT)
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

#ifndef __WM_LL_WDT_H__
#define __WM_LL_WDT_H__

#include "wm_reg_op.h"
#include "wm_attr.h"
#include "wm_reg_op.h"
#include "wm_wdt_reg_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set the watchdog timer load value
 * @param[in] reg wdt_reg_t pointer to the WDT hardware registers
 * @param[in] load_value The value to load into the watchdog timer
 */
ATTRIBUTE_INLINE void wm_ll_wdt_set_load_value(wm_wdt_reg_t *reg, uint32_t load_value)
{
    WM_LL_REG_FILED_COPY_WRTIE(reg->load, val, load_value);
}

/**
 * @brief Get the watchdog timer load value
 * @param[in] reg wdt_reg_t pointer to the WDT hardware registers
 * @retval uint32_t The counter value of the watchdog timer
 */
ATTRIBUTE_INLINE uint32_t wm_ll_wdt_get_load_value(wm_wdt_reg_t *reg)
{
    return WM_LL_REG_FILED_COPY_READ(reg->load, val);
}

/**
 * @brief Trigger a watchdog feed (reload)
 * @param[in] reg wdt_reg_t pointer to the WDT hardware registers
 */
ATTRIBUTE_INLINE void wm_ll_wdt_feed(wm_wdt_reg_t *reg)
{
    WM_LL_REG_FILED_COPY_WRTIE(reg->load, val, reg->load.load_value);
}

/**
 * @brief Get the current value of the watchdog timer
 * @param[in] reg wdt_reg_t pointer to the WDT hardware registers
 * @retval uint32_t The current value of the watchdog timer
 */
ATTRIBUTE_INLINE uint32_t wm_ll_wdt_get_current_value(wm_wdt_reg_t *reg)
{
    return WM_LL_REG_FILED_COPY_READ(reg->value, val);
}

/**
 * @brief Enable the watchdog timer
 * @param[in] reg wdt_reg_t pointer to the WDT hardware registers
 */
ATTRIBUTE_INLINE void wm_ll_wdt_enable(wm_wdt_reg_t *reg)
{
    WM_LL_REG_FILED_COPY_WRTIE(reg->ctrl, enable, 1);
}

/**
 * @brief Disable the watchdog timer
 * @param[in] reg wdt_reg_t pointer to the WDT hardware registers
 */
ATTRIBUTE_INLINE void wm_ll_wdt_disable(wm_wdt_reg_t *reg)
{
    WM_LL_REG_FILED_COPY_WRTIE(reg->ctrl, enable, 0);
}

/**
 * @brief Get the watchdog timer status
 * @param[in] reg wdt_reg_t pointer to the WDT hardware registers
 */
ATTRIBUTE_INLINE bool wm_ll_wdt_get_status(wm_wdt_reg_t *reg)
{
    return WM_LL_REG_FILED_COPY_READ(reg->ctrl, enable);
}

/**
 * @brief Enable the watchdog reset functionality
 * @param[in] reg wdt_reg_t pointer to the WDT hardware registers
 */
ATTRIBUTE_INLINE void wm_ll_wdt_enable_reset(wm_wdt_reg_t *reg)
{
    WM_LL_REG_FILED_COPY_WRTIE(reg->ctrl, rst_enable, 1);
}

/**
 * @brief Disable the watchdog reset functionality
 * @param[in] reg wdt_reg_t pointer to the WDT hardware registers
 */
ATTRIBUTE_INLINE void wm_ll_wdt_disable_reset(wm_wdt_reg_t *reg)
{
    WM_LL_REG_FILED_COPY_WRTIE(reg->ctrl, rst_enable, 0);
}

/**
 * @brief Clear the watchdog interrupt
 * @param[in] reg wdt_reg_t pointer to the WDT hardware registers
 */
ATTRIBUTE_INLINE void wm_ll_wdt_clear_interrupt(wm_wdt_reg_t *reg)
{
    WM_LL_REG_FILED_COPY_WRTIE(reg->int_clr, val, 1);
}

/**
 * @brief Get the source of the watchdog interrupt
 * @param[in] reg wdt_reg_t pointer to the WDT hardware registers
 * @retval bool true if an interrupt is pending, false if no interrupt is pending
 */
ATTRIBUTE_INLINE bool wm_ll_wdt_get_interrupt_source(wm_wdt_reg_t *reg)
{
    return reg->int_src.val & WM_BIT(0); // Assuming only one bit is used for the interrupt source
}

/**
 * @brief Get the state of the watchdog interrupt
 * @param[in] reg wdt_reg_t pointer to the WDT hardware registers
 * @retval bool true if the watchdog interrupt is active (triggered), false otherwise
 */
ATTRIBUTE_INLINE bool wm_ll_wdt_get_interrupt_state(wm_wdt_reg_t *reg)
{
    return reg->int_state.val & WM_BIT(0); // Assuming only one bit is used for the interrupt state
}

#ifdef __cplusplus
}
#endif

#endif /* __WM_LL_WDT_H__ */

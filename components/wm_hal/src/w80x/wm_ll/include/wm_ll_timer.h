/**
 * @file wm_ll_timer.h
 *
 * @brief Timer Module
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

#ifndef __WM_LL_TIMER_H__
#define __WM_LL_TIMER_H__

#include "wm_reg_op.h"
#include "wm_timer_reg_struct.h"
#include "wm_attr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief          Set timer source clock
 * @param[in]      hw Timer hardware instance address
 *                 hw is the type of @arg timer_reg_t
 * @param[in]      source clock
 */
ATTRIBUTE_INLINE void wm_ll_timer_set_source_clock(wm_timer_reg_t *hw, uint32_t clock)
{
    hw->timer_config.source_clk = clock;
}

/**
 * @brief          Set timer unit ms
 * @param[in]      hw Timer hardware instance address
 *                 hw is the type of @arg timer_reg_t
 * @param[in]      timer id
 */
ATTRIBUTE_INLINE void wm_ll_timer_set_unit_ms(wm_timer_reg_t *hw, uint32_t timer_id)
{
    WM_REG32_SET_BIT(&hw->timer_csr, WM_TIMER_MS_UNIT(timer_id));
}

/**
 * @brief          Set timer unit us
 * @param[in]      hw Timer hardware instance address
 *                 hw is the type of @arg timer_reg_t
 * @param[in]      timer id
 */
ATTRIBUTE_INLINE void wm_ll_timer_set_unit_us(wm_timer_reg_t *hw, uint32_t timer_id)
{
    WM_REG32_CLR_BIT(&hw->timer_csr, WM_TIMER_MS_UNIT(timer_id));
}

/**
 * @brief          Set timer auto reload
 * @param[in]      hw Timer hardware instance address
 *                 hw is the type of @arg timer_reg_t
 * @param[in]      timer id
 * @param[in]      auto_reload 1 for auto reload, 0 for not
 */
ATTRIBUTE_INLINE void wm_ll_timer_set_auto_reload(wm_timer_reg_t *hw, uint32_t timer_id, bool auto_reload)
{
    if (auto_reload) {
        WM_REG32_CLR_BIT(&hw->timer_csr, WM_TIMER_AUTO_RELOAD(timer_id));
    } else {
        WM_REG32_SET_BIT(&hw->timer_csr, WM_TIMER_AUTO_RELOAD(timer_id));
    }
}

/**
 * @brief          Get timer auto reload
 * @param[in]      hw Timer hardware instance address
 *                 hw is the type of @arg timer_reg_t
 * @param[in]      timer id
 * @param[out]     true auto reload, false for not
 */
ATTRIBUTE_INLINE bool wm_ll_timer_get_auto_reload(wm_timer_reg_t *hw, uint32_t timer_id)
{
    return !WM_REG32_GET_BIT(&hw->timer_csr, WM_TIMER_AUTO_RELOAD(timer_id));
}

/**
 * @brief          Set timer enable
 * @param[in]      hw Timer hardware instance address
 *                 hw is the type of @arg timer_reg_t
 * @param[in]      timer id
 */
ATTRIBUTE_INLINE void wm_ll_timer_set_enable(wm_timer_reg_t *hw, uint32_t timer_id)
{
    WM_REG32_SET_BIT(&hw->timer_csr, WM_TIMER_EN(timer_id));
}

/**
 * @brief          Set timer disable
 * @param[in]      hw Timer hardware instance address
 *                 hw is the type of @arg timer_reg_t
 * @param[in]      timer id
 */
ATTRIBUTE_INLINE void wm_ll_timer_set_disable(wm_timer_reg_t *hw, uint32_t timer_id)
{
    WM_REG32_CLR_BIT(&hw->timer_csr, WM_TIMER_EN(timer_id));
}

/**
 * @brief          Set timer irq enable
 * @param[in]      hw Timer hardware instance address
 *                 hw is the type of @arg timer_reg_t
 * @param[in]      timer id
 */
ATTRIBUTE_INLINE void wm_ll_timer_set_irq_enable(wm_timer_reg_t *hw, uint32_t timer_id)
{
    WM_REG32_SET_BIT(&hw->timer_csr, WM_TIMER_INT_EN(timer_id));
}

/**
 * @brief          Set timer irq disenable
 * @param[in]      hw Timer hardware instance address
 *                 hw is the type of @arg timer_reg_t
 * @param[in]      timer id
 */
ATTRIBUTE_INLINE void wm_ll_timer_set_irq_disable(wm_timer_reg_t *hw, uint32_t timer_id)
{
    WM_REG32_CLR_BIT(&hw->timer_csr, WM_TIMER_INT_EN(timer_id));
}

/**
 * @brief          Get timer irq status
 * @param[in]      hw Timer hardware instance address
 *                 hw is the type of @arg timer_reg_t
 * @param[in]      timer id
 * @param[out]     1 intrrupt occured, 0 for not
 */
ATTRIBUTE_INLINE int wm_ll_timer_get_irq_status(wm_timer_reg_t *hw, uint32_t timer_id)
{
    return WM_REG32_GET_BIT(&hw->timer_csr, WM_TIMER_INT_CLR(timer_id));
}

/**
 * @brief          Get timer csr
 * @param[in]      hw Timer hardware instance address
 * @param[in]      timer id
 * @param[out]     csr reg
 */
ATTRIBUTE_INLINE uint32_t wm_ll_timer_get_csr(wm_timer_reg_t *hw)
{
    return WM_REG32_READ(&hw->timer_csr);
}

/**
 * @brief          Set timer csr
 * @param[in]      hw Timer hardware instance address
 * @param[in]      csr value
 */
ATTRIBUTE_INLINE void wm_ll_timer_set_csr(wm_timer_reg_t *hw, uint32_t csr)
{
    WM_REG32_WRITE(&hw->timer_csr, csr);
}

/**
 * @brief          Clr timer irq status
 * @param[in]      hw Timer hardware instance address
 *                 hw is the type of @arg timer_reg_t
 * @param[in]      timer id
 */
ATTRIBUTE_INLINE void wm_ll_timer_clr_irq_status(wm_timer_reg_t *hw, uint32_t timer_id)
{
    WM_REG32_SET_BIT(&hw->timer_csr, WM_TIMER_INT_CLR(timer_id));
}

/**
 * @brief          Set timer timing value
 * @param[in]      hw Timer hardware instance address
 *                 hw is the type of @arg timer_reg_t
 * @param[in]      timer id
 * @param[in]      timing_value for set timer count value
 */
ATTRIBUTE_INLINE void wm_ll_timer_set_timing_value(wm_timer_reg_t *hw, uint32_t timer_id, uint32_t timing_value)
{
    hw->timer_set_value[timer_id] = timing_value;
}

/**
 * @brief          Get timer timing value
 * @param[in]      hw Timer hardware instance address
 *                 hw is the type of @arg timer_reg_t
 * @param[in]      timer id
 * @param[in]      timing_value for set timer count value
 */
ATTRIBUTE_INLINE uint32_t wm_ll_timer_get_timing_value(wm_timer_reg_t *hw, uint32_t timer_id)
{
    return hw->timer_set_value[timer_id];
}

/**
 * @brief          Get timer current value
 * @param[in]      hw Timer hardware instance address
 *                 hw is the type of @arg timer_reg_t
 * @param[in]      timer id
 * @param[out]     timer current value
 */
ATTRIBUTE_INLINE uint32_t wm_ll_timer_get_current_value(wm_timer_reg_t *hw, uint32_t timer_id)
{
    return hw->timer_curr_value[timer_id];
}

#ifdef __cplusplus
}
#endif

#endif /* __WM_LL_TIMER_H__ */

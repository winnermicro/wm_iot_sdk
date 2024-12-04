/**
 * @file wm_ll_rtc.h
 *
 * @brief rtc Low Layer Module
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

#ifndef __WM_LL_RTC_H__
#define __WM_LL_RTC_H__

#include "wm_regs_base.h"
#include "wm_types.h"
#include "wm_attr.h"
#include "wm_rtc_reg_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief          Set rtc second
 * @param[in]      hw Peripheral RTC hardware instance address
 *                 hw is the type of @arg wm_rtc_reg_t
 * @param[in]      val is second value
 */
ATTRIBUTE_INLINE void wm_ll_rtc_set_second(wm_rtc_reg_t *hw, uint32_t val)
{
    hw->rtc_r1.second = val;
}

/**
 * @brief          Get rtc second
 * @param[in]      hw Peripheral RTC hardware instance address
 *                 hw is the type of @arg wm_rtc_reg_t
 */
ATTRIBUTE_INLINE uint32_t wm_ll_rtc_get_second(wm_rtc_reg_t *hw)
{
    return hw->rtc_r1.second;
}

/**
 * @brief          Set rtc minute
 * @param[in]      hw Peripheral RTC hardware instance address
 *                 hw is the type of @arg wm_rtc_reg_t
 * @param[in]      val is minute value
 */
ATTRIBUTE_INLINE void wm_ll_rtc_set_minute(wm_rtc_reg_t *hw, uint32_t val)
{
    hw->rtc_r1.minute = val;
}

/**
 * @brief          Get rtc minute
 * @param[in]      hw Peripheral RTC hardware instance address
 *                 hw is the type of @arg wm_rtc_reg_t
 */
ATTRIBUTE_INLINE uint32_t wm_ll_rtc_get_minute(wm_rtc_reg_t *hw)
{
    return hw->rtc_r1.minute;
}

/**
 * @brief          Set rtc hour
 * @param[in]      hw Peripheral RTC hardware instance address
 *                 hw is the type of @arg wm_rtc_reg_t
 * @param[in]      val is hour value
 */
ATTRIBUTE_INLINE void wm_ll_rtc_set_hour(wm_rtc_reg_t *hw, uint32_t val)
{
    hw->rtc_r1.hour = val;
}

/**
 * @brief          Get rtc hour
 * @param[in]      hw Peripheral RTC hardware instance address
 *                 hw is the type of @arg wm_rtc_reg_t
 */
ATTRIBUTE_INLINE uint32_t wm_ll_rtc_get_hour(wm_rtc_reg_t *hw)
{
    return hw->rtc_r1.hour;
}

/**
 * @brief          Set rtc day
 * @param[in]      hw Peripheral RTC hardware instance address
 *                 hw is the type of @arg wm_rtc_reg_t
 * @param[in]      val is day value
 */
ATTRIBUTE_INLINE void wm_ll_rtc_set_day(wm_rtc_reg_t *hw, uint32_t val)
{
    hw->rtc_r1.day = val;
}

/**
 * @brief          Get rtc day
 * @param[in]      hw Peripheral RTC hardware instance address
 *                 hw is the type of @arg wm_rtc_reg_t
 */
ATTRIBUTE_INLINE uint32_t wm_ll_rtc_get_day(wm_rtc_reg_t *hw)
{
    return hw->rtc_r1.day;
}

/**
 * @brief          Set rtc month
 *
 * @param[in]      hw Peripheral RTC hardware instance address
 *                 hw is the type of @arg wm_rtc_reg_t
 * @param[in]      val is month value
 */
ATTRIBUTE_INLINE void wm_ll_rtc_set_month(wm_rtc_reg_t *hw, uint32_t val)
{
    hw->rtc_r2.month = val;
}

/**
 * @brief          Get rtc month
 * @param[in]      hw Peripheral RTC hardware instance address
 *                 hw is the type of @arg wm_rtc_reg_t
 */
ATTRIBUTE_INLINE uint32_t wm_ll_rtc_get_month(wm_rtc_reg_t *hw)
{
    return hw->rtc_r2.month;
}

/**
 * @brief          Set rtc year
 * @param[in]      hw Peripheral RTC hardware instance address
 *                 hw is the type of @arg wm_rtc_reg_t
 * @param[in]      val is year value
 */
ATTRIBUTE_INLINE void wm_ll_rtc_set_year(wm_rtc_reg_t *hw, uint32_t val)
{
    hw->rtc_r2.year = val;
}

/**
 * @brief          Get rtc year
 * @param[in]      hw Peripheral RTC hardware instance address
 *                 hw is the type of @arg wm_rtc_reg_t
 */
ATTRIBUTE_INLINE uint32_t wm_ll_rtc_get_year(wm_rtc_reg_t *hw)
{
    return hw->rtc_r2.year;
}

/**
 * @brief          RTC timing interrupt enable
 * @param[in]      hw Peripheral RTC hardware instance address
 *                 hw is the type of @arg rtc_reg_t
 * @param[in]      enable 1: enable interrupt, 0: disable interrupt
 */
ATTRIBUTE_INLINE void wm_ll_rtc_timer_enable(wm_rtc_reg_t *hw, uint32_t enable)
{
    hw->rtc_r1.enable = enable;
}

/**
 * @brief          RTC timer counting mode enable
 * @param[in]      hw Peripheral RTC hardware instance address
 *                 hw is the type of @arg rtc_reg_t
 * @param[in]      enable 1: enable, 0: disable
 */
ATTRIBUTE_INLINE void wm_ll_rtc_timer_counting_enable(wm_rtc_reg_t *hw, uint32_t enable)
{
    hw->rtc_r2.enable = enable;
}

#ifdef __cplusplus
}
#endif

#endif /* __WM_LL_RTC_H__ */

/**
 * @file wm_ll_touch_sensor.h
 *
 * @brief touch sensor Low Layer Module
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

#ifndef __WM_LL_TOUCH_SENSOR_H__
#define __WM_LL_TOUCH_SENSOR_H__

#include "wm_attr.h"
#include "wm_touch_sensor_reg_struct.h"
#include "wm_reg_op.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief          Touch sensor controller enable
 * @param[in]      hw Peripheral TOUCH hardware instance address
 *                 hw is the type of @arg touch_reg_t
 * @param[in]      enable 1 for enable, 0 for disable
 */
ATTRIBUTE_INLINE void wm_ll_touch_sensor_scan_enable(wm_touch_sensor_reg_t *hw, uint8_t en)
{
    hw->touch_cr.enable = en;
}

/**
 * @brief          Get the touch sensor controller enable status
 * @param[in]      hw Peripheral TOUCH hardware instance address
 *                 hw is the type of @arg touch_reg_t
 * @return         The touch sensor controller enable status
 */
ATTRIBUTE_INLINE uint8_t wm_ll_touch_sensor_get_scan_enable(wm_touch_sensor_reg_t *hw)
{
    return hw->touch_cr.enable;
}

/**
 * @brief          Get the touch key status of the corresponding bit is scanned.
 *                 0x0000: Do not scan;
 *                 0x0001: Scan the first key;
 *                 0x0002: Scan the second key;
 *                 0x0003: Scan the first and second keys;
 *                 ...
 *                 0xFFFF: Scan all 16 keys
 * @param[in]      hw Peripheral TOUCH hardware instance address
 *                 hw is the type of @arg touch_reg_t
 */

ATTRIBUTE_INLINE uint16_t wm_ll_touch_sensor_get_key_selection(wm_touch_sensor_reg_t *hw)
{
    return WM_LL_REG_FILED_COPY_READ(hw->touch_cr, key_selection);
}

/**
 * @brief          Set the touch key status of the corresponding bit is scanned.
 * @param[in]      hw Peripheral TOUCH hardware instance address
 *                 hw is the type of @arg touch_reg_t
 * @param[in]      key_selection
 *                 0x0000: Do not scan;
 *                 0x0001: Scan the first key;
 *                 0x0002: Scan the second key;
 *                 0x0003: Scan the first and second keys;
 *                 ...
 *                 0xFFFF: Scan all 16 keys
 */
ATTRIBUTE_INLINE void wm_ll_touch_sensor_set_key_selection(wm_touch_sensor_reg_t *hw, uint16_t key_selection)
{
    WM_LL_REG_FILED_COPY_WRTIE(hw->touch_cr, key_selection, key_selection);
}

/**
 * @brief          Get touch sencor capdet_cnt
 * @param[in]      hw Peripheral TOUCH hardware instance address
 *                 hw is the type of @arg touch_reg_t
 * @return        The number of CAPDET output pulses is chosen as the counting window.
 */
ATTRIBUTE_INLINE uint8_t wm_ll_touch_sensor_get_capdet_cnt(wm_touch_sensor_reg_t *hw)
{
    return hw->touch_cr.capdet_cnt;
}

/**
 * @brief          Set touch sencor capdet_cnt
 * @param[in]      hw Peripheral TOUCH hardware instance address
 *                 hw is the type of @arg touch_reg_t
 * @param[in]      capdet_cnt The number of CAPDET output pulses is chosen as the counting window.
 */
ATTRIBUTE_INLINE void wm_ll_touch_sensor_set_capdet_cnt(wm_touch_sensor_reg_t *hw, uint8_t capdet_cnt)
{
    hw->touch_cr.capdet_cnt = capdet_cnt;
}

/**
 * @brief          Set touch sencor scan_period
 * @param[in]      hw Peripheral TOUCH hardware instance address
 *                 hw is the type of @arg touch_reg_t
 * @param[in]      scan_periods Touch sencor scan period in 16ms
 */
ATTRIBUTE_INLINE void wm_ll_touch_sensor_set_scan_period(wm_touch_sensor_reg_t *hw, uint8_t scan_periods)
{
    hw->touch_cr.scan_period = scan_periods;
}

/**
 * @brief          Get touch sencor scan_period
 * @param[in]      hw Peripheral TOUCH hardware instance address
 *                 hw is the type of @arg touch_reg_t
 * @return        Touch sencor scan period in 16ms
 */
ATTRIBUTE_INLINE uint8_t wm_ll_touch_sensor_get_scan_period(wm_touch_sensor_reg_t *hw)
{
    return hw->touch_cr.scan_period;
}

/**
 * @brief          Set touch sencor threshold_val
 * @param[in]      hw Peripheral TOUCH hardware instance address
 *                 hw is the type of @arg touch_reg_t
 * @param[in]      index Touch sencor tc_x index 0 - WM_TOUCH_NUM_MAX
 * @param[in]      threshold_val Touch sencor threshold value
 */
ATTRIBUTE_INLINE void wm_ll_touch_sensor_set_threshold_val(wm_touch_sensor_reg_t *hw, uint8_t index, uint8_t threshold_val)
{
    hw->tc_x[index].threshold_val = threshold_val;
}

/**
 * @brief          Get touch sencor threshold_val
 * @param[in]      hw Peripheral TOUCH hardware instance address
 *                 hw is the type of @arg touch_reg_t
 * @param[in]      index Touch sencor tc_x index 0 - WM_TOUCH_NUM_MAX
 * @return        Touch sencor threshold value
 */
ATTRIBUTE_INLINE uint8_t wm_ll_touch_sensor_get_threshold_val(wm_touch_sensor_reg_t *hw, uint8_t index)
{
    return hw->tc_x[index].threshold_val;
}

/**
 * @brief          Get touch sencor count_val
 * @param[in]      hw Peripheral TOUCH hardware instance address
 *                 hw is the type of @arg touch_reg_t
 * @param[in]      index Touch sencor tc_x index 0 - WM_TOUCH_NUM_MAX
 * @note           Read only
 */
ATTRIBUTE_INLINE uint16_t wm_ll_touch_sensor_get_count_val(wm_touch_sensor_reg_t *hw, uint8_t index)
{
    return hw->tc_x[index].count_val;
}

/**
 * @brief           Set touch sencor int_source
 * @param[in]       hw Peripheral TOUCH hardware instance address
 *                  hw is the type of @arg touch_reg_t
 * @param[in]       int_source Touch sencor pad status
 * @note            bit 1 indicates corresponding PAD is triggered
 *                  bit 0 indicates corresponding PAD is not triggered
 *                  If write 1, it will set to 0
 */
ATTRIBUTE_INLINE void wm_ll_touch_sensor_set_int_source(wm_touch_sensor_reg_t *hw, uint16_t int_source)
{
    WM_LL_REG_FILED_COPY_WRTIE(hw->touch_int, int_source, int_source);
}

/**
 * @brief           Get touch sencor int_source
 * @param[in]       hw Peripheral TOUCH hardware instance address
 *                  hw is the type of @arg touch_reg_t
 * @return          Touch sencor pad status
 * @note            bit 1 indicates corresponding PAD is triggered
 *                  bit 0 indicates corresponding PAD is not triggered
 *                  If write 1, it will set to 0
 */
ATTRIBUTE_INLINE uint16_t wm_ll_touch_sensor_get_int_source(wm_touch_sensor_reg_t *hw)
{
    return WM_LL_REG_FILED_COPY_READ(hw->touch_int, int_source);
}

/**
 * @brief           Set touch sencor int_en
 * @param[in]       hw Peripheral TOUCH hardware instance address
 *                  hw is the type of @arg touch_reg_t
 * @param[in]       int_en Touch sencor interrupt enable
 * @note            bit 1 indicates corresponding IO is enable to interrupt
 *                  bit 0 indicates corresponding IO is not disable to interrupt
 */
ATTRIBUTE_INLINE void wm_ll_touch_sensor_set_int_enable(wm_touch_sensor_reg_t *hw, uint16_t int_en)
{
    hw->touch_int.int_en = int_en;
}

/**
 * @brief           Get touch sencor int_en
 * @param[in]       hw Peripheral TOUCH hardware instance address
 *                  hw is the type of @arg touch_reg_t
 * @return          Touch sencor interrupt enable
 * @note            bit 1 indicates corresponding IO is enable to interrupt
 *                  bit 0 indicates corresponding IO is not disable to interrupt
 */
ATTRIBUTE_INLINE uint16_t wm_ll_touch_sensor_get_int_enable(wm_touch_sensor_reg_t *hw)
{
    return hw->touch_int.int_en;
}

#ifdef __cplusplus
}
#endif

#endif /* __WM_LL_TOUCH_SENSOR_H__ */

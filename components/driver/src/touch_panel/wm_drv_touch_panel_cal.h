/**
 * @file wm_drv_touch_panel_cal.h
 *
 * @brief Used to achieve coordinate conversion, calibration functions, and confirmation of coordinate origin.
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
#ifndef __WM_DRV_TOUCH_PANEL_CAL_H__
#define __WM_DRV_TOUCH_PANEL_CAL_H__

#include "wm_drv_touch_panel.h"
#include "wm_dt_hw.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Used for mirroring the coordinate axes of the touch panel.
 *
 * @param[in] device_hw is the device table structure variable that needs to be passed.
 * @param[in] x is the incoming x-axis coordinate.alignas.
 * @param[in] y is the incoming x-axis coordinate.alignas.
 */
void wm_drv_tp_adjust_mirror_coord(wm_dt_hw_touch_panel_t *device_hw, uint16_t *x, uint16_t *y);

/**
 * @brief Used to determine the position of the screen coordinate origin.alignas.
 *
 * @param[in] device_hw is the device table structure variable that needs to be passed.
 * @param[in] rotation used to rotate the coordinate axis.
 * @param[in] x is the incoming x-axis coordinate.alignas.
 * @param[in] y is the incoming x-axis coordinate.alignas.
 */
void wm_drv_tp_adjust_rotate_coord(wm_dt_hw_touch_panel_t *device_hw, uint8_t rotation, uint16_t *x, uint16_t *y);

#ifdef __cplusplus
}
#endif

#endif /* __WM_DRV_TOUCH_PANEL_CAL_H__ */

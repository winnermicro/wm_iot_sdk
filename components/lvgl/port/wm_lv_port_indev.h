/**
 * @file lv_port_indev.h
 *
 * @brief Matching LVGL with Touch Drivers
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
#ifndef LV_PORT_INDEV_H
#define LV_PORT_INDEV_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

/**
  * @brief Some parameter configurations
  *
  * @struct lv_touchpad_screen_conv_t
  * @param touch_dev is the device structure pointer of the touch panel.
  * @param x_ratio is the ratio coefficient of the touch panel 
  * and display screen with respect to the x-axis.
  * @param y_ratio is the ratio coefficient of the touch panel 
  * and display screen with respect to the y-axis.
  */

typedef struct {
    wm_device_t *touch_dev;
    float x_ratio;
    float y_ratio;
} lv_touchpad_screen_conv_t;

/**
  * @brief LVGL layer, touchpad and display interface.
  *
  * @param[in] Touchpad_stcreen_cv is a structural variable
  * used to configure the x-axis scaling factor, y-axis scaling
  * factor, and touchpad's structural pointer.
  */
void lv_port_indev_init(lv_touchpad_screen_conv_t touchpad_screen_conv);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
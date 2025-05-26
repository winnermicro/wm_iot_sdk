/**
 * @file wm_drv_ops_touch_calibration.c
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
#include <stdio.h>
#include <string.h>
#include "wmsdk_config.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wm_drv_touch_panel.h"
#include "wm_error.h"
#include "wm_log.h"
#include "wm_osal.h"
#include "wm_drv_touch_panel_cal.h"
#include "wm_dt_hw.h"

void wm_drv_tp_adjust_mirror_coord(wm_dt_hw_touch_panel_t *device_hw, uint16_t *x, uint16_t *y)
{
    uint16_t value_x = *x;
    uint16_t value_y = *y;

    switch (device_hw->mirror_image) {
        case 0:
            *x = value_x;
            *y = value_y;
            break;
        case 1:
            *x = value_y;
            *y = value_x;
            break;
        default:
            break;
    }
}

void wm_drv_tp_adjust_rotate_coord(wm_dt_hw_touch_panel_t *device_hw, uint8_t rotation, uint16_t *x, uint16_t *y)
{
    uint16_t value_x = *x;
    uint16_t value_y = *y;

    switch (rotation) {
        case 0:
            *x = value_x;
            *y = value_y;
            break;
        case 1:
            *x = value_x;
            *y = device_hw->height - value_y;
            break;
        case 2:
            *x = device_hw->width - value_x;
            *y = device_hw->height - value_y;
            break;
        case 3:
            *x = device_hw->width - value_x;
            *y = value_y;
            break;
        default:
            break;
    }
}
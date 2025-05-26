/**
 * @file lv_port_indev.c
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
#include <stdio.h>
#include "wmsdk_config.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wm_drv_gpio.h"
#include "wm_lv_port_indev.h"
#include "wm_drv_touch_panel.h"
#include "wm_error.h"
#include "lvgl.h"
#include "wm_dt_hw.h"

// Task stack size and priority
#define WM_LVGL_TASK_STACK (1024)
#define WM_LVGL_TASK_PRIO  (configMAX_PRIORITIES - 8)

static wm_drv_touch_panel_points_t lv_tp_data;
static lv_touchpad_screen_conv_t lv_touchpad_screen_conv;

static void wm_lv_touch_panel_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data);

void lv_port_indev_init(lv_touchpad_screen_conv_t touchpad_screen_conv)
{
    lv_touchpad_screen_conv.touch_dev = touchpad_screen_conv.touch_dev;
    lv_touchpad_screen_conv.x_ratio   = touchpad_screen_conv.x_ratio;
    lv_touchpad_screen_conv.y_ratio   = touchpad_screen_conv.y_ratio;

    static lv_indev_drv_t indev_drv;
    /*Register a touchpad input device*/
    lv_indev_drv_init(&indev_drv);
    indev_drv.type    = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = wm_lv_touch_panel_read;
    lv_indev_drv_register(&indev_drv);
}

/**
 * @brief Read the state of the touchpad. Called by LVGL.
 * @param indev_drv Pointer to the input device driver.
 * @param data Pointer to the input device data structure.
 */
static void wm_lv_touch_panel_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    static uint16_t index = 0;

    if (index == 0) {
        wm_drv_touch_panel_get_coords(lv_touchpad_screen_conv.touch_dev, &lv_tp_data);
        if (lv_tp_data.cur_points == 0) {
            data->state = LV_INDEV_STATE_REL;
            return;
        }
    }

    index++;

    data->point.x = (lv_coord_t)(lv_touchpad_screen_conv.x_ratio * lv_tp_data.curx[index - 1]);
    data->point.y = (lv_coord_t)(lv_touchpad_screen_conv.y_ratio * lv_tp_data.cury[index - 1]);
    data->state   = LV_INDEV_STATE_PR;

    if (index >= lv_tp_data.cur_points) {
        data->continue_reading = false;
        index                  = 0;
    } else {
        data->continue_reading = true;
    }
}
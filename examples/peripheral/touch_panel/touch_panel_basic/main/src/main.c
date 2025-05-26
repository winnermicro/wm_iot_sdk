/**
 * @file main.c
 *
 * @brief Touch Panel Basic demo main
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

#define LOG_TAG "test"
#include "wm_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wm_drv_touch_panel.h"
#include "wm_drv_touch_panel_cfg.h"

#define TOUCH_PANEL_DEVICE_NAME WM_CFG_TOUCH_PANEL_NAME

static void test_task(void *parameters)
{
    wm_device_t *dev;
    wm_drv_touch_panel_points_t tp_points;
    wm_drv_touch_panel_set_cfg_t tp_cfg;

    tp_cfg.rotation = 0;     // Used to set the origin position of coordinates
    tp_cfg.cal_done = false; // enable calibration

    // Initialize touch panel driver chip
    dev = wm_drv_touch_panel_init(TOUCH_PANEL_DEVICE_NAME);

    // Touch panel Driver Chip Parameter Configuration
    if (wm_drv_touch_panel_set_config(dev, &tp_cfg) != WM_ERR_SUCCESS) {
        wm_log_error("touch panel parameter configuration failed");
    }

    while (1) {
        // Obtain the number of touch points and the coordinates of the actual touch points
        if (wm_drv_touch_panel_get_coords(dev, &tp_points) == WM_ERR_SUCCESS) {
            for (int i = 0; i < tp_points.cur_points; ++i) {
                wm_log_info("Touch coordinate point %d, x-coordinate is %d, y-coordinate is %d.", i + 1, tp_points.curx[i],
                            tp_points.cury[i]);
            }
        }

        // Get once every 0.1 seconds
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    vTaskDelete(NULL);
}

int main(void)
{
    xTaskCreate(test_task, "test", 512, NULL, 5, NULL);

    return 0;
}

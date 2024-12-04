/**
 * @file main.c
 *
 * @brief GDC0689 device driver API display demo main
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
#include "wm_drv_seg_lcd.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "wm_gdc0689.h"

#define LOG_TAG "example"
#include "wm_log.h"

#define WM_GDC0689_REFRESH_DELAY_MS (500)

static void gdc0689_demo(wm_device_t *dev)
{
    int ret = WM_ERR_SUCCESS;

    for (int i = 0; i < 5; i++) {
        wm_log_info("Displaying all segments.");
        ret = wm_gdc0689_display_all(dev);
        if (ret != WM_ERR_SUCCESS) {
            wm_log_error("Failed to display hour, error code: %d.", ret);
            return;
        }
        vTaskDelay(pdMS_TO_TICKS(WM_GDC0689_REFRESH_DELAY_MS));
        wm_log_info("Clearing segment LCD.");
        ret = wm_gdc0689_clear(dev);
        if (ret != WM_ERR_SUCCESS) {
            wm_log_error("Failed to clear segment LCD");
            return;
        }
        vTaskDelay(pdMS_TO_TICKS(WM_GDC0689_REFRESH_DELAY_MS));
    }

    for (int i = 0; i < 24; i++) {
        wm_log_info("Displaying time: hour %d, minute %d.", i, 0);
        ret = wm_gdc0689_display_time(dev, i, 0);
        if (ret != WM_ERR_SUCCESS) {
            wm_log_error("Failed to display hour, error code: %d.", ret);
            return;
        }
        vTaskDelay(pdMS_TO_TICKS(WM_GDC0689_REFRESH_DELAY_MS));
    }

    for (int i = 0; i < 60; i++) {
        wm_log_info("Displaying time: hour %d, minute %d.", 0, i);
        ret = wm_gdc0689_display_time(dev, 0, i);
        if (ret != WM_ERR_SUCCESS) {
            wm_log_error("Failed to display min, error code: %d.", ret);
            return;
        }
        vTaskDelay(pdMS_TO_TICKS(WM_GDC0689_REFRESH_DELAY_MS));
    }

    for (int i = 0; i < WM_GDC0689_SERVICE_ICON_MAX; i++) {
        wm_log_info("Displaying service icon %d.", i);
        ret = wm_gdc0689_display_service_icon(dev, i);
        if (ret != WM_ERR_SUCCESS) {
            wm_log_error("Failed to display service icon, error code: %d.", ret);
            return;
        }
        vTaskDelay(pdMS_TO_TICKS(WM_GDC0689_REFRESH_DELAY_MS));
    }

    for (int i = 0; i < WM_GDC0689_BATTERY_LEVEL_MAX; i++) {
        wm_log_info("Displaying battery level %d.", i);
        ret = wm_gdc0689_display_battery_level(dev, i);
        if (ret != WM_ERR_SUCCESS) {
            wm_log_error("Failed to display battery level, error code: %d.", ret);
            return;
        }
        vTaskDelay(pdMS_TO_TICKS(WM_GDC0689_REFRESH_DELAY_MS));
    }

    for (int i = 0; i < WM_GDC0689_SIGNAL_LEVEL_MAX; i++) {
        wm_log_info("Displaying signal level %d.", i);
        ret = wm_gdc0689_display_signal_level(dev, i);
        if (ret != WM_ERR_SUCCESS) {
            wm_log_error("Failed to display signal level, error code: %d.", ret);
            return;
        }
        vTaskDelay(pdMS_TO_TICKS(WM_GDC0689_REFRESH_DELAY_MS));
    }

    for (int i = 0; i < 10; i++) {
        uint32_t val = i * 111111;

        wm_log_info("Displaying integer value %06d.", val);
        ret = wm_gdc0689_display_integer(dev, val);
        if (ret != WM_ERR_SUCCESS) {
            wm_log_error("Failed to display integer, error code: %d.", ret);
            return;
        }
        vTaskDelay(pdMS_TO_TICKS(WM_GDC0689_REFRESH_DELAY_MS));
    }

    for (int i = 0; i < 10; i++) {
        float val = i * 111111 / 100.00;

        wm_log_info("Displaying decimal value %07.2f.", val);
        ret = wm_gdc0689_display_decimal(dev, val);
        if (ret != WM_ERR_SUCCESS) {
            wm_log_error("Failed to display decimal, error code: %d.", ret);
            return;
        }
        vTaskDelay(pdMS_TO_TICKS(WM_GDC0689_REFRESH_DELAY_MS));
    }

    for (int i = 0; i < WM_GDC0689_UNIT_MAX; i++) {
        wm_log_info("Displaying unit %d.", i);
        ret = wm_gdc0689_display_unit(dev, i);
        if (ret != WM_ERR_SUCCESS) {
            wm_log_error("Failed to display unit, error code: %d.", ret);
            return;
        }
        vTaskDelay(pdMS_TO_TICKS(WM_GDC0689_REFRESH_DELAY_MS));
    }

    wm_log_info("Clearing segment LCD again.");
    ret = wm_gdc0689_clear(dev);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("Failed to clear segment LCD");
        return;
    }
    vTaskDelay(pdMS_TO_TICKS(WM_GDC0689_REFRESH_DELAY_MS));

    wm_log_info("Deinitializing segment LCD device.");
    ret = wm_gdc0689_deinit(dev);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("Failed to deinitialize segment LCD device");
        return;
    }

    wm_log_info("This example succeeds in running.");
}

int main(void)
{
    wm_device_t *dev;

    wm_log_info("GDC0689 display demo start.\n");

    /* Initialize GDC0689 */
    wm_log_info("Initializing segment LCD device...");
    dev = wm_gdc0689_init("gdc0689");

    /* Starting GDC0689 display demo */
    if (dev == NULL) {
        wm_log_error("Failed to initialize segment LCD device");
    } else {
        gdc0689_demo(dev);
    }

    while (1) {
        vTaskDelay(1000);
    }

    return 0;
}

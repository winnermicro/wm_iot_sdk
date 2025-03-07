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
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "wm_drv_seg_lcd.h"
#include "seg_lcd.h"

#define LOG_TAG "example"
#include "wm_log.h"

#define WM_SEG_LCD_REFRESH_DELAY_MS (pdMS_TO_TICKS(1000))
#define WM_SEG_LCD_DISPLAY_COUNT    (5)

int main(void)
{
    int ret;
    wm_device_t *dev;

    wm_log_info("seg_lcd display demo start.");

    wm_log_info("Initializing segment LCD device...");
    dev = wm_drv_seg_lcd_init("seg_lcd");
    if (dev == NULL) {
        wm_log_error("Failed to initialize segment LCD device");
    }

    ret = wm_drv_seg_lcd_register_table(dev, g_seg_lcd_table, sizeof(g_seg_lcd_table) / sizeof(g_seg_lcd_table[0]));
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("Failed to register segment LCD table");
    }

    wm_log_info("Starting segment LCD display/clear cycle...");
    for (int i = 0; i < WM_SEG_LCD_DISPLAY_COUNT; i++) {
        ret = seg_lcd_display_all(dev);
        if (ret != WM_ERR_SUCCESS) {
            wm_log_error("Failed to display segment LCD");
        }
        vTaskDelay(WM_SEG_LCD_REFRESH_DELAY_MS);
        ret = seg_lcd_clear_all(dev);
        if (ret != WM_ERR_SUCCESS) {
            wm_log_error("Failed to clear segment LCD");
        }
        vTaskDelay(WM_SEG_LCD_REFRESH_DELAY_MS);
    }

    wm_log_info("Deinitializing segment LCD device.");
    ret = wm_drv_seg_lcd_deinit(dev);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("Failed to deinitialize segment LCD device");
    }

    wm_log_info("This example succeeds in running.");

    while (1) {
        vTaskDelay(1000);
    }

    return 0;
}

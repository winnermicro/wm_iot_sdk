/**
 * @file main.c
 *
 * @brief Wdt Example
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

#include <stdint.h>
#include <string.h>
#include <time.h>

#include "wm_types.h"
#include "wm_error.h"
#include "wm_event.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wm_drv_wdt.h"

#define LOG_TAG "wdt_test"
#include "wm_log.h"

#define WM_WDT_TASK_STACK_SIZE 1024
#define WM_WDT_TASK_PRIO       6

static void wm_wdt_task(void *arg)
{
    wm_device_t *wdt_dev    = NULL;
    uint32_t remaining_time = 0;
    uint32_t counter_value  = 0;

    wdt_dev = (wm_device_t *)arg;
    while (1) {
        wm_log_info("wdt feed");
        wm_drv_wdt_feed(wdt_dev);
        wm_drv_wdt_get_counter_value(wdt_dev, &counter_value);
        wm_drv_wdt_get_remaining_time(wdt_dev, &remaining_time);
        wm_log_info("wdt counter value %d reamining time %d", counter_value, remaining_time);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

int main(void)
{
    wm_device_t *wdt_dev = NULL;

    wdt_dev = wm_drv_wdt_init("wdt");
    if (wdt_dev == NULL) {
        wm_log_error("wdt init failed");
        return WM_ERR_FAILED;
    }

    xTaskCreate(wm_wdt_task, "wm_wdt", WM_WDT_TASK_STACK_SIZE, wdt_dev, WM_WDT_TASK_PRIO, NULL);

    return WM_ERR_SUCCESS;
}

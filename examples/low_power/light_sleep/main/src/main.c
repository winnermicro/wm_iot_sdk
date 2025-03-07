/**
 * @file main.c
 *
 * @brief PM demo main
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

#include "wmsdk_config.h"
#include "wm_pm.h"
#include "wm_error.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LOG_TAG "demo"
#include "wm_log.h"

static void demo_task(void *parameters)
{
    int ret               = WM_ERR_SUCCESS;
    wm_pm_config_t config = { .mode              = WM_PM_MODE_LIGHT_SLEEP,
                              .sleep_dev_bitmap  = WM_PM_SLEEP_DEV_TYPE_I2C | WM_PM_SLEEP_DEV_TYPE_LSPI,
                              .wakeup_dev_bitmap = WM_PM_SLEEP_DEV_TYPE_LCD | WM_PM_SLEEP_DEV_TYPE_SDIO_MASTER };

    ret = wm_pm_set_config(&config);

    while (WM_ERR_SUCCESS == ret) {
        wm_log_info("hello world.");

        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    vTaskDelete(NULL);
}

int main(void)
{
    xTaskCreate(demo_task, "demo", 512, NULL, 1, NULL);

    return 0;
}

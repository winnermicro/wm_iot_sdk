/**
 * @file main.c
 *
 * @brief Adc Example
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
#include "wm_drv_adc.h"
#include "wm_drv_gpio.h"

#define LOG_TAG "adc_example"
#include "wm_log.h"

#define WM_ADC_CHANNLE_COUNT   1
#define WM_ADC_POLLING_SIZE    20
#define WM_ADC_TASK_STACK_SIZE 4906
#define WM_ADC_TASK_PRIO       6
#define WM_ADC_UNIT            1000

static void wm_adc_task(void *arg)
{
    wm_device_t *adc_dev             = NULL;
    int tmp                          = 0;
    int vdd                          = 0;
    int polling[WM_ADC_POLLING_SIZE] = { 0 };
    char buf[WM_ADC_POLLING_SIZE]    = { 0 };

    adc_dev = (wm_device_t *)arg;
    while (1) {
        wm_drv_thermal_sensor_read_temperature(adc_dev, &tmp);
        snprintf(buf, sizeof(buf), "%d.%03d", tmp / WM_ADC_UNIT, tmp % WM_ADC_UNIT);
        wm_log_info("chip temp %s", buf);

        wm_drv_adc_chip_voltage_read_vdd(adc_dev, &vdd);
        wm_log_info("chip voltage %dmv", vdd);

        wm_drv_adc_oneshot(adc_dev, WM_ADC_CHANNEL_0, &vdd);
        wm_log_info("result oneshot %dmv", vdd);

        wm_drv_adc_polling(adc_dev, WM_ADC_CHANNEL_0, polling, WM_ADC_POLLING_SIZE, 0);
        for (int i = 0; i < WM_ADC_POLLING_SIZE; i++) {
            wm_log_info("result %d %dmv", i, polling[i]);
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

int main(void)
{
    wm_device_t *adc_dev = NULL;

    //Initialize adc dev, and configure adc based on the device tree.
    adc_dev = wm_drv_adc_init("adc");
    if (adc_dev == NULL) {
        wm_log_error("gpio init failed");
        return WM_ERR_FAILED;
    }

    xTaskCreate(wm_adc_task, "adc", WM_ADC_TASK_STACK_SIZE, adc_dev, WM_ADC_TASK_PRIO, NULL);

    return WM_ERR_SUCCESS;
}

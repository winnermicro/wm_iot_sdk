/**
 * @file main.c
 *
 * @brief rcc example
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
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "wm_types.h"
#include "wm_error.h"
#include "wm_drv_rcc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#define LOG_TAG "exam_rcc"
#include "wm_log.h"

int main(void)
{
    wm_device_t *dev;
    int ret = 0;

    wm_log_info("rcc set cpu clock start.\n");

    /*initialize rcc*/
    dev = wm_drv_clock_init("rcc");
    if (dev != NULL) {
        /*start set cpu clock example*/
        ret = wm_drv_rcc_config_clock(dev, WM_RCC_TYPE_CPU, 120);
        if (ret == WM_ERR_SUCCESS) {
            wm_log_info("set cpu clock as 120Mhz pass\n");
        } else {
            wm_log_info("set cpu clock fail: %d\n", ret);
        }
    } else {
        wm_log_info("get rcc device error\n");
    }

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    return 0;
}

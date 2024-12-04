/**
 * @file main.c
 *
 * @brief Timer Example
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

#include "wm_types.h"
#include "wm_error.h"
#include "wm_event.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wm_drv_timer.h"

#define LOG_TAG "timer_test"
#include "wm_log.h"

#define WM_TIMER_DELAY 10000

void wm_timer_test_callback(void *arg)
{
    wm_device_t *dev = (wm_device_t *)arg;

    if (dev) {
        //this just a demon, please do not perform time-consuming operations in Timer's callback, link printf...
        wm_log_info("%s TRIGGERED", dev->name);
    }
}

int main(void)
{
    int err                      = 0;
    wm_device_t *timer0_dev      = NULL;
    wm_device_t *timer1_dev      = NULL;
    wm_device_t *timer2_dev      = NULL;
    wm_drv_timer_cfg_t timer_cfg = { 0 };

    if (NULL == (timer0_dev = wm_drv_timer_init("timer0")) || NULL == (timer1_dev = wm_drv_timer_init("timer1")) ||
        NULL == (timer2_dev = wm_drv_timer_init("timer2"))) {
        wm_log_error("timer init failed");
        return WM_ERR_FAILED;
    }

    if (WM_ERR_SUCCESS != (err = wm_drv_timer_register_callback(timer0_dev, wm_timer_test_callback, timer0_dev)) ||
        WM_ERR_SUCCESS != (err = wm_drv_timer_register_callback(timer1_dev, wm_timer_test_callback, timer1_dev)) ||
        WM_ERR_SUCCESS != (err = wm_drv_timer_register_callback(timer2_dev, wm_timer_test_callback, timer2_dev))) {
        wm_log_error("timer register err");
        return WM_ERR_FAILED;
    }

    timer_cfg.unit        = WM_HAL_TIMER_UNIT_MS;
    timer_cfg.auto_reload = false;
    timer_cfg.period      = 10000;
    wm_drv_timer_start(timer0_dev, timer_cfg);

    timer_cfg.unit        = WM_HAL_TIMER_UNIT_MS;
    timer_cfg.auto_reload = true;
    timer_cfg.period      = 2000;
    wm_drv_timer_start(timer1_dev, timer_cfg);

    timer_cfg.unit        = WM_HAL_TIMER_UNIT_MS;
    timer_cfg.auto_reload = true;
    timer_cfg.period      = 3000;
    wm_drv_timer_start(timer2_dev, timer_cfg);

    // WAIT
    vTaskDelay(pdMS_TO_TICKS(WM_TIMER_DELAY));

    wm_log_info("all timer stop");

    wm_drv_timer_stop(timer0_dev);
    wm_drv_timer_stop(timer1_dev);
    wm_drv_timer_stop(timer2_dev);

    wm_drv_timer_deinit(timer0_dev);
    wm_drv_timer_deinit(timer1_dev);
    wm_drv_timer_deinit(timer2_dev);

    return WM_ERR_SUCCESS;
}

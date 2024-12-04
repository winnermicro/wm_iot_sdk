/**
 * @file main.c
 *
 * @brief Gpio Drv Module
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

#include "wmsdk_config.h"
#include "wm_types.h"
#include "wm_error.h"
#include "wm_event.h"
#include "wm_drv_gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LOG_TAG "gpio"
#include "wm_log.h"

// defined in device tree
#if CONFIG_BUILD_TYPE_W802 && CONFIG_BUILD_TYPE_W806 && CONFIG_BUILD_TYPE_W801S
#define WM_GPIO_DEFINE0 WM_GPIO_NUM_37
#define WM_GPIO_DEFINE1 WM_GPIO_NUM_38
#endif

// not defined in device tree
#define WM_GPIO_DEFINE2   WM_GPIO_NUM_4

// not defined in device tree
#define WM_GPIO_DEFINE3   WM_GPIO_NUM_27

#define WM_GPIO_PULL_HIGH 1

static void wm_drv_gpio_isr_pin_handler(void *arg)
{
    wm_log_info("GPIO[%d] intr, val: %d\n", (int)arg, wm_drv_gpio_data_get((wm_gpio_num_t)arg));
}

int main(void)
{
    int err = 0;

    //Initialize gpio dev, and configure gpio based on the device tree.
    wm_drv_gpio_init("gpio");

#if CONFIG_BUILD_TYPE_W802 && CONFIG_BUILD_TYPE_W806 && CONFIG_BUILD_TYPE_W801S
    // add gpio pin isr callback
    if (WM_ERR_SUCCESS != (err = wm_drv_gpio_add_isr_callback(WM_GPIO_DEFINE0, (wm_drv_gpio_isr_t)wm_drv_gpio_isr_pin_handler,
                                                              (void *)WM_GPIO_DEFINE0))) {
        wm_log_error("WM_GPIO_DEFINE0 add isr fail. %d", err);
        return err;
    }

    if (WM_ERR_SUCCESS != (err = wm_drv_gpio_add_isr_callback(WM_GPIO_DEFINE1, (wm_drv_gpio_isr_t)wm_drv_gpio_isr_pin_handler,
                                                              (void *)WM_GPIO_DEFINE1))) {
        wm_log_error("WM_GPIO_DEFINE1 add isr fail. %d", err);
        return err;
    }
#endif

    // Initialize WM_GPIO_NUM_2, it is not defined in the device tree
    if (WM_ERR_SUCCESS != (err = wm_drv_gpio_iomux_func_sel(WM_GPIO_DEFINE2, WM_GPIO_IOMUX_FUN5)) ||
        WM_ERR_SUCCESS != (err = wm_drv_gpio_set_dir(WM_GPIO_DEFINE2, WM_GPIO_DIR_INPUT)) ||
        WM_ERR_SUCCESS != (err = wm_drv_gpio_set_pullmode(WM_GPIO_DEFINE2, WM_GPIO_FLOAT)) ||
        WM_ERR_SUCCESS != (err = wm_drv_gpio_set_intr_mode(WM_GPIO_DEFINE2, WM_GPIO_IRQ_TRIG_DOUBLE_EDGE)) ||
        WM_ERR_SUCCESS != (err = wm_drv_gpio_add_isr_callback(WM_GPIO_DEFINE2, (wm_drv_gpio_isr_t)wm_drv_gpio_isr_pin_handler,
                                                              (void *)WM_GPIO_DEFINE2)) ||
        WM_ERR_SUCCESS != (err = wm_drv_gpio_enable_isr(WM_GPIO_DEFINE2))) {
        wm_log_error("WM_GPIO_DEFINE2 cfg err %d", err);
        return err;
    }

    // Initialize WM_GPIO_NUM_3, it is not defined in the device tree
    if (WM_ERR_SUCCESS != (err = wm_drv_gpio_iomux_func_sel(WM_GPIO_DEFINE3, WM_GPIO_IOMUX_FUN5)) ||
        WM_ERR_SUCCESS != (err = wm_drv_gpio_set_dir(WM_GPIO_DEFINE3, WM_GPIO_DIR_OUTPUT)) ||
        WM_ERR_SUCCESS != (err = wm_drv_gpio_data_set(WM_GPIO_DEFINE3))) {
        wm_log_error("WM_GPIO_DEFINE3 cfg err %d", err);
        return err;
    }

    wm_log_info("WM_GPIO_DEFINE3 data value high");
    if (WM_GPIO_PULL_HIGH != wm_drv_gpio_data_get(WM_GPIO_DEFINE3)) {
        wm_log_error("WM_GPIO_DEFINE3 data high err");
    }

    // time delay if need
    vTaskDelay(pdMS_TO_TICKS(1000));

    wm_drv_gpio_data_reset(WM_GPIO_DEFINE3);
    if (wm_drv_gpio_data_get(WM_GPIO_DEFINE3)) {
        wm_log_error("get data err");
    }
    wm_log_info("WM_GPIO_DEFINE3 data value low");

    // time delay if need
    vTaskDelay(pdMS_TO_TICKS(1000));

    wm_log_info("gpio init ok");

    return WM_ERR_SUCCESS;
}

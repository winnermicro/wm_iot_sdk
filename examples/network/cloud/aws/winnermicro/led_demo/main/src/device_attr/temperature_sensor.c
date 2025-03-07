/**
 * @file pwm_led.c
 *
 * @brief Control PWM LED
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
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

#include "wmsdk_config.h"
#include "wm_soc_cfgs.h"
#include "wm_error.h"
#include "wm_types.h"
#include "wm_drv_gpio.h"

#include "aws_device_attr.h"
#include "temperature_sensor.h"

#define LOG_TAG "temp_sensor"
#include "wm_log.h"

#define ANALOG_TEMP_SENSOR 1
#if ANALOG_TEMP_SENSOR
static void init_key(void **ptr);
#endif

typedef struct {
    int temperature; /* Unit: 0.01 degrees Celsius */
} temperature_sensor_param;

static temperature_sensor_param temperature_sensor;

const char *get_temperature_sensor_name(void)
{
    return "temperature_sensor";
}

void init_temperature_sensor(void **ptr)
{
#if ANALOG_TEMP_SENSOR
    init_key(ptr);
#endif
    *ptr = &temperature_sensor;
}

int get_temperature_sensor(void *ptr, char *data, size_t max_len)
{
    if (ptr == NULL || data == NULL || !max_len) {
        return WM_ERR_FAILED;
    }

    temperature_sensor_param *tmp_sensor = (temperature_sensor_param *)ptr;
    snprintf(data, max_len, "%d.%d", tmp_sensor->temperature / 100, tmp_sensor->temperature % 100);
    return WM_ERR_SUCCESS;
}

#if ANALOG_TEMP_SENSOR

typedef struct {
    char *gpio_name;

    wm_device_t *gpio_device;
    TimerHandle_t timer;
    uint16_t key_value_ms;
    uint16_t key_check_ms;
} key_param;

/* key process cfg */
#define WM_GPIO_USER_KEY     WM_GPIO_NUM_21 //PB5, active low
#define WM_TIMER_DELAY       20             // 20 micro second
#define WM_KEY_VALID_TIME_MS 100            // The button is effective for 500ms
#define WM_KEY_CHECK_TIME_MS 200            // The button is ckeck time for 500ms

static void timer_callback(TimerHandle_t xTimer)
{
    char buff[32];
    key_param *param = (key_param *)pvTimerGetTimerID(xTimer);

    assert(param && param->timer);

    param->key_check_ms += WM_TIMER_DELAY;
    if (!wm_drv_gpio_data_get(WM_GPIO_USER_KEY)) {
        param->key_value_ms += WM_TIMER_DELAY;
    }

    if (param->key_value_ms >= WM_KEY_VALID_TIME_MS) {
        /* post key event */
        wm_log_info("user_key pressed\n");

        /* increase temperature */
        temperature_sensor.temperature += 500; // 5 Celsius
        snprintf(buff, sizeof(buff), "%d.%d", temperature_sensor.temperature / 100, temperature_sensor.temperature % 100);
        aws_device_attr_change(get_temperature_sensor_name(), buff);
        return;
    }

    if (param->key_check_ms < WM_KEY_CHECK_TIME_MS) {
        /* Continue checking */
        xTimerStart(param->timer, 0);
    }
}

static void wm_drv_gpio_isr_pin_handler(void *arg)
{
    key_param *param = (key_param *)arg;

    assert(param && param->timer);
    if (!wm_drv_gpio_data_get(WM_GPIO_USER_KEY) && !xTimerIsTimerActive(param->timer)) {
        /* The button is valid and timer is not running*/

        param->key_value_ms = 0;
        param->key_check_ms = 0;
        xTimerStart(param->timer, 0);
    }

    wm_log_debug("GPIO intr\n");
}

static void init_key(void **ptr)
{
    key_param *param = calloc(sizeof(key_param), 1);

    if (param == NULL) {
        wm_log_error("OOM, pwm initialized fail!");
        return;
    }

    param->gpio_name = "gpio";

    param->gpio_device = wm_dt_get_device_by_name(param->gpio_name);
    if (!(param->gpio_device && param->gpio_device->state == WM_DEV_ST_INITED)) {
        param->gpio_device = wm_drv_gpio_init(param->gpio_name);
        if (!(param->gpio_device && param->gpio_device->state == WM_DEV_ST_INITED)) {
            wm_log_error("gpio initialized fail!");
            free(param);
            return;
        }
    }

    if (WM_ERR_SUCCESS != wm_drv_gpio_iomux_func_sel(WM_GPIO_USER_KEY, WM_GPIO_IOMUX_FUN5) ||
        WM_ERR_SUCCESS != wm_drv_gpio_set_dir(WM_GPIO_USER_KEY, WM_GPIO_DIR_INPUT) ||
        WM_ERR_SUCCESS != wm_drv_gpio_set_pullmode(WM_GPIO_USER_KEY, WM_GPIO_FLOAT) ||
        WM_ERR_SUCCESS != wm_drv_gpio_set_intr_mode(WM_GPIO_USER_KEY, WM_GPIO_IRQ_TRIG_FALLING_EDGE) ||
        WM_ERR_SUCCESS !=
            wm_drv_gpio_add_isr_callback(WM_GPIO_USER_KEY, (wm_drv_gpio_isr_t)wm_drv_gpio_isr_pin_handler, (void *)param) ||
        WM_ERR_SUCCESS != wm_drv_gpio_enable_isr(WM_GPIO_USER_KEY)) {
        wm_log_error("WM_GPIO_USER_KEY cfg err");
        free(param);
        return;
    }

    param->timer = xTimerCreate("key", pdMS_TO_TICKS(20), false, param, timer_callback);

    if (param->timer == NULL) {
        wm_log_error("timer creat err");
        free(param);
        return;
    }

    wm_log_debug("key init success");

    *ptr = param;
}

#endif

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

#include "wm_dt.h"
#include "wm_dt_hw.h"
#include "wm_soc_cfgs.h"
#include "wmsdk_config.h"
#include "wm_error.h"
#include "wm_drv_pwm.h"
#include "wm_drv_gpio.h"

#define LOG_TAG "pwm_led"
#include "wm_log.h"

#define PWM_INVALID_RATIO        (-1)
#define PWM_INIT_CHECK()         (param->pwm_device && param->pwm_device->state == WM_DEV_ST_INITED)
#define PWM_DUTY_CYCLE_LEVEL_MAX (255)
#define PWM_RATIO_CHECK(v)                              \
    if ((unsigned int)(v) > PWM_DUTY_CYCLE_LEVEL_MAX) { \
        cmd_pwm_usage(argv[0]);                         \
        return WM_ERR_FAILED;                           \
    }

enum {
    PWM_LED_GREEN, /**< map to pwm channel 0 */
    PWM_LED_RED,   /**< map to pwm channel 1 */
    PWM_LED_BLUE,  /**< map to pwm channel 2 */

    PWM_LED_MAX
};

typedef struct {
    int ratio[PWM_LED_MAX];
    char *pwm_name;
    wm_device_t *pwm_device;
    wm_drv_pwm_channel_cfg_t cfg[PWM_LED_MAX];
} pwm_led_param;

static int cmd_pwm_channel_operation(wm_device_t *pwm_device, wm_drv_pwm_channel_cfg_t *cfg, uint8_t duty_cycle)
{
    enum wm_pwm_channels channel = cfg->channel;
    wm_dt_hw_pwm_t *hw           = pwm_device->hw;
    wm_dt_hw_pin_cfg_t *pin_cfg  = hw->pin_cfg;

    /* this channel maybe not initialized if failed */
    if (WM_ERR_SUCCESS == wm_drv_pwm_set_channel_duty(pwm_device, channel, duty_cycle)) {
        return WM_ERR_SUCCESS;
    }

    if (pin_cfg == NULL) {
        wm_cli_printf("err: invalid pin cfg!\r\n");
        return WM_ERR_INVALID_PARAM;
    }

    if (WM_ERR_SUCCESS == wm_drv_pwm_channel_init(pwm_device, cfg)) {
        if (WM_ERR_SUCCESS != wm_drv_pwm_channel_start(pwm_device, channel)) {
            return WM_ERR_FAILED;
        }
    } else {
        return WM_ERR_FAILED;
    }

    if (WM_ERR_SUCCESS != wm_drv_gpio_iomux_func_sel(pin_cfg[channel].pin_num, pin_cfg[channel].pin_mux)) {
        wm_cli_printf("err: fail to set gpio[%d]!\r\n", pin_cfg[channel].pin_num);
        wm_drv_pwm_channel_deinit(pwm_device, channel);
        return WM_ERR_FAILED;
    }

    return WM_ERR_SUCCESS;
}

static int cmd_pwm_channel_foreach(pwm_led_param *param)
{
    uint8_t duty_cycle;
    for (int i = 0; i < PWM_LED_MAX; i++) {
        if (param->ratio[i] != PWM_INVALID_RATIO) {
            param->cfg[i].channel = i;
            param->cfg[i].mode    = WM_PWM_OUT_INDPT;
            param->cfg[i].clkdiv  = WM_PWM_CLKDIV_DEFAULT;

            /* period_cycle means how many cycle is included in one period,
             * The cycle is the divided freqency of pwm clock source which
             * configurated by rcc module and it needs to be notified that
             * the cycle mentioned here is not duty cycle. ex. pwm clock
             * source is 40MHz (set by rcc module) the divided freqency is
             * 2MHz if clock divider is 20, and the cycle's unit is 0.5us.
             * the period cycle 200 indicates the period time is 100us
             * the pwm frequency is 1/period_cycle = 10KHz
             * */
            param->cfg[i].period_cycle = WM_PWM_MAX_PERIOD;

            /* singal reverse as W800_ARDUINO_Vx.x Board’s PWM RGB LED is turn on when IO output low */
            duty_cycle = PWM_DUTY_CYCLE_LEVEL_MAX - param->ratio[i];

            param->cfg[i].duty_cycle = duty_cycle;
            param->cfg[i].autoload   = true;
            if (WM_ERR_SUCCESS != cmd_pwm_channel_operation(param->pwm_device, &param->cfg[i], duty_cycle)) {
                wm_log_error("err: fail to set the channel[%d]!", i);
                return WM_ERR_FAILED;
            }
        } else {
            (void)wm_drv_pwm_channel_stop(param->pwm_device, i);
            (void)wm_drv_pwm_channel_deinit(param->pwm_device, i);
        }
    }

    return WM_ERR_SUCCESS;
}

const char *get_pwm_led_name(void)
{
    return "led";
}

void init_pwm_led(void **ptr)
{
    pwm_led_param *param = calloc(sizeof(pwm_led_param), 1);

    if (param == NULL) {
        wm_log_error("OOM, pwm initialized fail!");
        return;
    }

    memset(param->ratio, PWM_INVALID_RATIO, sizeof(param->ratio));
    memset(param->cfg, 0, sizeof(param->cfg));

    param->pwm_name   = "pwm";
    param->pwm_device = NULL;

    param->pwm_device = wm_dt_get_device_by_name(param->pwm_name);
    if (!PWM_INIT_CHECK()) {
        param->pwm_device = wm_drv_pwm_init(param->pwm_name);
        if (!PWM_INIT_CHECK()) {
            wm_log_error("pwm initialized fail!");
            free(param);
            return;
        }
    }
    wm_log_debug("pwm led init success");

    *ptr = param;
}

int set_pwm_led(void *ptr, const char *data, size_t len)
{
    pwm_led_param *param = (pwm_led_param *)ptr;

    if (param == NULL || data == NULL || !len || strlen(data) != len) {
        return WM_ERR_FAILED;
    }

    /* support red, green, blue, off*/
    if (!strcmp(data, "red")) {
        wm_log_debug("set led red");
        param->ratio[PWM_LED_RED]   = 255;
        param->ratio[PWM_LED_GREEN] = 0;
        param->ratio[PWM_LED_BLUE]  = 0;
    } else if (!strcmp(data, "green")) {
        wm_log_debug("set led green");
        param->ratio[PWM_LED_GREEN] = 255;
        param->ratio[PWM_LED_RED]   = 0;
        param->ratio[PWM_LED_BLUE]  = 0;
    } else if (!strcmp(data, "blue")) {
        wm_log_debug("set led blue");
        param->ratio[PWM_LED_BLUE]  = 255;
        param->ratio[PWM_LED_RED]   = 0;
        param->ratio[PWM_LED_GREEN] = 0;
    } else if (!strcmp(data, "off")) {
        wm_log_debug("set led off");
        param->ratio[PWM_LED_RED]   = 0;
        param->ratio[PWM_LED_GREEN] = 0;
        param->ratio[PWM_LED_BLUE]  = 0;
    } else {
        wm_log_error("set led param error(%s)", data);
        return WM_ERR_FAILED;
    }

    return cmd_pwm_channel_foreach(param);
}

int get_pwm_led(void *ptr, char *data, size_t max_len)
{
    int ret;
    pwm_led_param *param = (pwm_led_param *)ptr;

    if (param == NULL || data == NULL || !max_len) {
        return WM_ERR_FAILED;
    }

    if (param->ratio[PWM_LED_RED] != 0) {
        ret = snprintf(data, max_len, "red");
    } else if (param->ratio[PWM_LED_GREEN] != 0) {
        ret = snprintf(data, max_len, "green");
    } else if (param->ratio[PWM_LED_BLUE] != 0) {
        ret = snprintf(data, max_len, "blue");
    } else {
        ret = snprintf(data, max_len, "off");
    }

    if (ret < 0 || ret == max_len) {
        wm_log_error("get led fail (buff is small)");
        return WM_ERR_FAILED;
    }
    return WM_ERR_SUCCESS;
}

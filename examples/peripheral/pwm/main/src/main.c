/**
 * @file main.c
 *
 * @brief PWM DRV demo Module
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
#include "wm_list.h"
#include "wmsdk_config.h"
#include "wm_error.h"
#include "wm_types.h"
#include "wm_cli.h"
#include "wm_drv_gpio.h"
#include "wm_drv_pwm.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#define WM_PWM_DEMO_DEBUG

#include "wm_log.h"
#undef LOG_TAG
#define LOG_TAG "PWM"

int main(void)
{
    int ret;
    int step                     = 1;
    uint8_t duty                 = 0;
    uint8_t period               = 0;
    uint16_t clkdiv              = 0;
    wm_drv_pwm_channel_cfg_t cfg = { 0 };

    wm_device_t *pwm_device = wm_drv_pwm_init("pwm");
    if (pwm_device == NULL) {
        wm_log_error("PWM driver init Failed!");

        return WM_ERR_FAILED;
    }

    cfg.channel      = WM_PWM_CHANNEL_0;
    cfg.mode         = WM_PWM_OUT_ALLSYNC;
    cfg.clkdiv       = WM_PWM_CLKDIV_DEFAULT;
    cfg.period_cycle = WM_PWM_PERIOD_DEFAULT;
    cfg.duty_cycle   = WM_PWM_DUTY_CYCLE_DEFAULT;
    cfg.autoload     = true;

    ret = wm_drv_pwm_channel_init(pwm_device, &cfg);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("PWM driver channel init failed!");
        goto exit;
    }

    ret = wm_drv_pwm_channels_start(pwm_device);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("PWM driver channel start failed!");
        goto exit;
    }

    /* demo for API: change frequency to 10KHz */
    ret = wm_drv_pwm_set_channel_freq(pwm_device, cfg.channel, 10000);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("failed to change frequency to 10KHz!");
        goto exit;
    }

    ret = wm_drv_pwm_get_channel_period_clkdiv(pwm_device, cfg.channel, &period, &clkdiv);
    if (WM_ERR_SUCCESS != ret) {
        wm_log_error("failed to get current period and clkdiv value!");
        return WM_ERR_FAILED;
    }

    wm_log_info("PWM will continuously output a PWM waveform with a periodically varying duty cycle.");
    /* demo for API: change duty */
    for (; /* Iterate through all duty values with a 50ms interval */;) {
        ret = wm_drv_pwm_set_channel_duty(pwm_device, cfg.channel, duty);
        if (ret != WM_ERR_SUCCESS) {
            wm_log_error("fail to set duty to %d!", duty);
            goto exit;
        }

        duty += step;

        if (duty == period || duty == 0) {
            step = -step;
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }

exit:
    wm_drv_pwm_deinit(pwm_device);

    return ret;
}

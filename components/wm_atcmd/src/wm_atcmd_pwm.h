/**
 * @file wm_atcmd_pwm.h
 *
 * @brief Default PWM AT Command
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

#ifndef __WM_ATCMD_PWM_H__
#define __WM_ATCMD_PWM_H__

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_WM_ATCMD_PWM_ENABLE

#define WM_ATCMD_PWM_INIT_USAGE                             \
    "AT+DRVPWMINIT=<freq>,<ch0_gpio>,[,...,<ch4_gpio>]\r\n" \
    "example:AT+DRVPWMINIT=10000,16\r\n"

#define WM_ATCMD_PWM_DUTY_USAGE                 \
    "AT+DRVPWMDUTY=<ch0 duty>[,<ch1 duty>]\r\n" \
    "example:AT+DRVPWMDUTY=100\r\n"

#define WM_ATCMD_PWM_FADE_USAGE                                                                   \
    "AT+DRVPWMFADE=<ch0_target_duty>,<ch0_fade_time>[,...,<ch4_target_duty>,<ch4_fade_time>]\r\n" \
    "example:AT+DRVPWMFADE=100,10000\r\n"

int wm_atcmd_pwm_init(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_pwm_duty(wm_at_type_e type, int argc, char **argv);
int wm_atcmd_pwm_fade(wm_at_type_e type, int argc, char **argv);

WM_ATCMD_DEFINE("AT+DRVPWMINIT", wm_atcmd_pwm_init, WM_ATCMD_PWM_INIT_USAGE);
WM_ATCMD_DEFINE("AT+DRVPWMDUTY", wm_atcmd_pwm_duty, WM_ATCMD_PWM_DUTY_USAGE);
WM_ATCMD_DEFINE("AT+DRVPWMFADE", wm_atcmd_pwm_fade, WM_ATCMD_PWM_FADE_USAGE);

#endif //CONFIG_WM_ATCMD_PWM_ENABLE

#ifdef __cplusplus
}
#endif

#endif /* __WM_ATCMD_PWM_H__ */

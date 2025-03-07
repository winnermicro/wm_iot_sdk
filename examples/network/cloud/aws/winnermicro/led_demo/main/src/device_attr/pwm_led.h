/**
 * @file pwm_led.h
 *
 * @brief AWS user define file
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

#ifndef __PWM_LED_H__
#define __PWM_LED_H__

#include "wm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* led process function */

const char *get_pwm_led_name(void);

void init_pwm_led(void **ptr);

int set_pwm_led(void *ptr, const char *data, size_t len);

int get_pwm_led(void *ptr, char *data, size_t max_len);

#ifdef __cplusplus
}
#endif

#endif /* __PWM_LED_H__ */

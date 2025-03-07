/**
 * @file aws_device_attr.h
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

#ifndef __AWS_DEVICE_ATTR_H__
#define __AWS_DEVICE_ATTR_H__

#include "wm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void aws_device_init(void);

void aws_device_update(void);

void aws_device_attr_set(const char *data, size_t len);

void aws_device_attr_change(const char *item, const char *value);

#ifdef __cplusplus
}
#endif

#endif /* __AWS_DEVICE_ATTR_H__ */

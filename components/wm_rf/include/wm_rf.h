/**
 * @file wm_rf.h
 *
 * @brief RF Module
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

#ifndef __WM_RF_H__
#define __WM_RF_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_RF_Functions WM RF Functions
 * @brief WinnerMicro RF Functions
 */

/**
 * @addtogroup WM_RF_Functions
 * @{
 */

uint32_t wm_rf_read(uint32_t data);

void wm_rf_write(uint32_t data);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_RF_H__ */

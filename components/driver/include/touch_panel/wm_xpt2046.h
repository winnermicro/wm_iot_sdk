/**
 * @file wm_xpt2046.h
 *
 * @brief Touch Panel Chip Module
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

#ifndef __WM_XPT2046_H__
#define __WM_XPT2046_H__

// The command definitions for XPT2046 must strictly match those in the XPT2046 Datasheet.
#define XPT2046_TEMP0_CMD    0x86 // Get temperatur of temp0
#define XPT2046_TEMP1_CMD    0xF6 // Get temperatur of temp1
#define XPT2046_VBAT_CMD     0xA6 // Get spannung
#define XPT2046_AUXIN_CMD    0xE6 // Get hilfseingang
#define XPT_CMD_X_COORDINATE 0XD0 // Get x coordinate
#define XPT_CMD_Y_COORDINATE 0X90 // Get y coordinate
#define XPT_CMD_PRESSURE_Z1  0XB0 // Get the pressure of z1
#define XPT_CMD_PRESSURE_Z2  0XC0 // Get the pressure of z2

#define XPT_CMD_SWAP_NUM     3   // the number of bytes swap at once
#define XPT2046_THRESHOLD_Z1 150 // pressure threshold of z1 in xpt2046, used to judge is pressed
// the max value of the raw data for the touch points detected by XPT2046, which matches with XPT2046 datasheet
#define MAX_XPT2046_RAW      4095
/**
 * @brief Check condition and return WM_ERR_FAILED
 */
#define WM_DRV_XPT2046_CHECK_FAIL(p) \
    if (!(p)) {                      \
        return WM_ERR_FAILED;        \
    }

/**
 * @brief  mutex lock for api
 */
#define WM_DRV_XPT2046_LOCK(mutex)                                                                                     \
    do {                                                                                                               \
        if (wm_os_internal_recursive_mutex_acquire((wm_os_mutex_t *)(mutex), WM_OS_WAIT_TIME_MAX) != WM_ERR_SUCCESS) { \
            return WM_ERR_FAILED;                                                                                      \
        }                                                                                                              \
    } while (0)

/**
 * @brief  mutex unlock for api
 */
#define WM_DRV_XPT2046_UNLOCK(mutex) wm_os_internal_recursive_mutex_release((wm_os_mutex_t *)(mutex))

#endif
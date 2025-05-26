/**
 * @file wm_ft6336.h
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

#ifndef __WM_FT6336_H__
#define __WM_FT6336_H__
#define FT_TRUE                       1
#define FT_FALSE                      0
// I2C address
#define I2C_ADDRESS                   0x38
#define I2C_ADDRESS_WIDTH             8
// max write time ms
#define MAX_WRITE_TIME_MS             10
// The command definitions for FT6336 must strictly match those in the FT6336 Datasheet.
#define FT_DEVIDE_MODE                0x00 // FT6336 mode control register
#define FT_REG_NUM_FINGER             0x02 // Touch status register
#define FT_TP1_REG                    0X03 // First touch point data address
#define FT_TP2_REG                    0X09 // Second touch point data address
#define FT_ID_G_CIPHER_MID            0x9F // Chip code (medium byte) default value 0x26
#define FT_ID_G_CIPHER_LOW            0xA0 // Chip code (low byte) 0x01: Ft6336G  0x02: Ft6336U
#define FT_ID_G_LIB_VERSION           0xA1 // edition
#define FT_ID_G_CIPHER_HIGH           0xA3 // Chip code (high byte) default 0x64
#define FT_ID_G_MODE                  0xA4 // FT6636 Interrupt Mode Control Register
#define FT_ID_G_FOCALTECH_ID          0xA8 // The default value for VENDOR ID is 0x11
#define FT_ID_G_THGROUP               0x80 // Touch the effective value setting register
#define FT_ID_G_PERIODACTIVE          0x88 // Activate state cycle setting register

// MATCH VALUE LIST
#define PANNEL_ID                     0x11

// ft6336 chip read speed
#define FT_CHIP_SPEED                 WM_I2C_SPEED_STANDARD

// The max number of touch points the FT6336 supports for simultaneous user presses
#define FT6336_TOUCH_MAX_POINT_NUMBER 2

/**
 * @brief Check condition and return WM_ERR_FAILED
 */
#define WM_DRV_FT6336_CHECK_FAIL(p) \
    if (!(p)) {                     \
        return WM_ERR_FAILED;       \
    }

/**
 * @brief  mutex lock for api
 */
#define WM_DRV_FT6336_LOCK(mutex)                                                                                      \
    do {                                                                                                               \
        if (wm_os_internal_recursive_mutex_acquire((wm_os_mutex_t *)(mutex), WM_OS_WAIT_TIME_MAX) != WM_ERR_SUCCESS) { \
            return WM_ERR_FAILED;                                                                                      \
        }                                                                                                              \
    } while (0)

/**
 * @brief  mutex unlock for api
 */
#define WM_DRV_FT6336_UNLOCK(mutex)      wm_os_internal_recursive_mutex_release((wm_os_mutex_t *)(mutex))

/**
 * @brief  Use for address round up
 */
#define WM_DRV_FT6336_DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))

#endif
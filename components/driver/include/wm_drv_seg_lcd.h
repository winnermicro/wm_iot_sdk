/**
 * @file wm_drv_seg_lcd.h
 *
 * @brief DRV_SEG_LCD Module
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

#ifndef __WM_DRV_SEG_LCD_H__
#define __WM_DRV_SEG_LCD_H__

#include "wm_hal_seg_lcd.h"
#include "wm_dt.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_DRV_SEG_LCD_Functions WM DRV_SEG_LCD Functions
 * @brief WinnerMicro DRV_SEG_LCD Functions
 */

/**
 * @addtogroup WM_DRV_SEG_LCD_Functions
 * @{
 */

/**
 * @brief Initializes the segment LCD device.
 *
 * This function looks up the device structure by name in the device tree and initializes
 * the segment LCD device using the operations defined for it. It is the first step to
 * prepare the device for use.
 *
 * @param[in] dev_name The name of the device to initialize.
 *
 * @return
 *    - A pointer to the device structure if the initialization succeeds.
 *    - NULL if the initialization fails or the device name is not recognized, or the device has already been initialized.
 */
wm_device_t *wm_drv_seg_lcd_init(char *dev_name);

/**
 * @brief Deinitializes the segment LCD device.
 *
 * This function is used to release the resources and停止 any ongoing operations of the segment LCD device.
 * It should be called when the device is no longer needed to ensure clean resource management.
 *
 * @param[in] dev Pointer to the device structure that needs to be deinitialized.
 *
 * @return
 *    - WM_ERR_SUCCESS if the deinitialization succeeds.
 *    - WM_ERR_INVALID_PARAM if the provided device pointer is NULL or invalid.
 *    - Other error codes may be returned based on the specific failure.
 */
int wm_drv_seg_lcd_deinit(wm_device_t *dev);

/**
 * @brief Clears the content displayed on the segment LCD.
 *
 * This function is used to clear all the segments on the LCD screen, effectively
 * resetting the display to a blank state.
 *
 * @param[in] dev Pointer to the device structure of the segment LCD.
 *
 * @return
 *    - WM_ERR_SUCCESS if the display is successfully cleared.
 *    - WM_ERR_INVALID_PARAM if the provided device pointer is NULL or invalid.
 *    - Other error codes may be returned based on the specific failure.
 */
int wm_drv_seg_lcd_clear(wm_device_t *dev);

/**
 * @brief Displays content on the segment LCD.
 *
 * This function is used to control the display of individual segments on the segment LCD device.
 * It allows for the selective illumination of segments based on the provided segment mask and data,
 * enabling the creation of digits, characters, or custom patterns on the LCD.
 *
 * @param[in] dev Pointer to the device structure of the segment LCD.
 * @param[in] com_id The common line identifier to which the segments are connected.
 * @param[in] seg_mask A bitmask indicating which segments should be affected by this operation.
 * @param[in] seg_data The data that determines the state (on/off) of the segments specified by the mask.
 *
 * @return
 *    - WM_ERR_SUCCESS if the display update is successfully applied.
 *    - WM_ERR_INVALID_PARAM if the provided device pointer is NULL, the common line identifier is out of range,
 *      or the segment mask is zero, indicating no segments to update.
 *    - Other error codes may be returned based on the specific failure.
 */
int wm_drv_seg_lcd_display(wm_device_t *dev, uint8_t com_id, uint32_t seg_mask, uint32_t seg_data);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_DRV_SEG_LCD_H__ */

/**
 * @file seg_lcd.h
 *
 * @brief SEG_LCD Module
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

#ifndef __SEG_LCD_H__
#define __SEG_LCD_H__

#include "wm_drv_seg_lcd.h"

/**
 * @brief The size of the segment LCD table array
 */
#define SEG_LCD_SEG_LCD_TABLE_SIZE (124)

/**
 * @brief Enumeration of weekdays for segment LCD display
 *
 * This enum defines the days of the week that can be displayed on the segment LCD.
 */
typedef enum {
    SEG_LCD_WEEKDAY_MON = 0, /**< Monday */
    SEG_LCD_WEEKDAY_TUE,     /**< Tuesday */
    SEG_LCD_WEEKDAY_WED,     /**< Wednesday */
    SEG_LCD_WEEKDAY_THU,     /**< Thursday */
    SEG_LCD_WEEKDAY_FRI,     /**< Friday */
    SEG_LCD_WEEKDAY_SAT,     /**< Saturday */
    SEG_LCD_WEEKDAY_SUN,     /**< Sunday */
    SEG_LCD_WEEKDAY_MAX      /**< Maximum value for weekday enum */
} seg_lcd_weekday_t;

/**
 * @brief Global segment LCD table array
 *
 * This array contains the mapping between segment names and their hardware configurations.
 */
extern const seg_lcd_table_t g_seg_lcd_table[SEG_LCD_SEG_LCD_TABLE_SIZE];

/**
 * @brief Displays all segments on the LCD
 *
 * This function turns on all segments on the segment LCD display.
 *
 * @param[in] dev Pointer to the device structure of the segment LCD.
 *
 * @return
 *    - WM_ERR_SUCCESS if all segments are successfully displayed.
 *    - WM_ERR_INVALID_PARAM if the provided device pointer is NULL or invalid.
 */
int seg_lcd_display_all(wm_device_t *dev);

/**
 * @brief Clears all segments on the LCD
 *
 * This function turns off all segments on the segment LCD display.
 *
 * @param[in] dev Pointer to the device structure of the segment LCD.
 *
 * @return
 *    - WM_ERR_SUCCESS if all segments are successfully cleared.
 *    - WM_ERR_INVALID_PARAM if the provided device pointer is NULL or invalid.
 */
int seg_lcd_clear_all(wm_device_t *dev);

#endif /* __SEG_LCD_H__ */

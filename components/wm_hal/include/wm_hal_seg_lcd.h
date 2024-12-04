/**
 * @file wm_hal_seg_lcd.h
 *
 * @brief Segment LCD HAL Driver Module
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

#ifndef __WM_HAL_SEG_LCD_H__
#define __WM_HAL_SEG_LCD_H__

#include <string.h>
#include "wm_osal.h"
#include "wm_types.h"
#include "wm_soc_cfgs.h"
#include "wm_ll_seg_lcd.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_HAL_SEG_LCD_Structures WM HAL_SEG_LCD Structures
 * @brief WinnerMicro HAL_SEG_LCD Structures
 */

/**
 * @addtogroup WM_HAL_SEG_LCD_Structures
 * @{
 */

/**
 * @brief Segment LCD Configuration structure.
 *
 * The Segment LCD Configuration structure contains various configuration
 * parameters required for setting up and controlling the segment LCD display.
 * It includes duty selection, output supply voltage, internal bias configuration,
 * output drive strength, and frame frequency settings.
 */
typedef struct {
    wm_seg_lcd_duty_sel_t duty_sel; /**< LCD duty selection */
    wm_seg_lcd_vlcd_cc_t vlcd_cc;   /**< LCD output supply voltage selection */
    wm_seg_lcd_bias_t bias;         /**< LCD internal bias configuration */
    wm_seg_lcd_hd_t hd;             /**< LCD output drive strength control */
    uint32_t frame_freq;            /**< The frame frequency setting for the LCD is calculated as follows:
                                         `frm_freq = 32000 / (frame_cnt * com_num)`, where `frame_cnt` can range from 1 to 2048,
                                         and `com_num` can range from 1 to 8, with the specific values to be chosen based on the actual SEG_LCD screen. */
    uint8_t com_num; /**< The number of COM lines should be selected from 1 to 8 based on the actual SEG_LCD screen. */
} wm_hal_seg_lcd_cfg_t;

/**
 * @brief Segment LCD Device structure.
 *
 * The Segment LCD Device structure contains the base address of the LCD registers
 * and a configuration structure for the segment LCD. It is used to manage
 * and interact with the segment LCD hardware.
 */
typedef struct {
    wm_seg_lcd_reg_t *seg_lcd_reg;    /**< Pointer to the base address of LCD registers */
    wm_hal_seg_lcd_cfg_t seg_lcd_cfg; /**< Configuration for the segment LCD */
} wm_hal_seg_lcd_dev_t;

/**
 * @}
 */

/**
 * @defgroup WM_HAL_SEG_LCD_Functions WM HAL_SEG_LCD Functions
 * @brief WinnerMicro HAL_SEG_LCD Functions
 */

/**
 * @addtogroup WM_HAL_SEG_LCD_Functions
 * @{
 */

/**
 * @brief Initialize the segment LCD device.
 *
 * This function sets up the segment LCD device with the provided configuration.
 * It prepares the hardware for displaying content on the LCD.
 *
 * @param dev Pointer to the segment LCD device structure.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the initialization process
 */
int wm_hal_seg_lcd_init(wm_hal_seg_lcd_dev_t *dev);

/**
 * @brief Deinitialize the segment LCD device.
 *
 * This function disables the segment LCD device and releases any resources
 * associated with it. It is used to cleanly shut down the LCD hardware.
 *
 * @param dev Pointer to the segment LCD device structure.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the deinitialization process
 */
int wm_hal_seg_lcd_deinit(wm_hal_seg_lcd_dev_t *dev);

/**
 * @brief Write data to a specific COM line of the segment LCD.
 *
 * This function writes data to the specified COM line of the segment LCD,
 * allowing for the display of individual segments or rows on the LCD.
 *
 * @param dev Pointer to the segment LCD device structure.
 * @param com_id Identifier for the COM line to write data to.
 * @param seg_mask A bitmask that determines which segments to update.
 *                 Bit positions set to 1 in this mask will be modified according to `seg_data`.
 * @param seg_data Segment data to be written to the COM line.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the write operation
 */
int wm_hal_seg_lcd_write(wm_hal_seg_lcd_dev_t *dev, wm_seg_lcd_com_id_t com_id, uint32_t seg_mask, uint32_t seg_data);

/**
 * @brief Clear the segment LCD display.
 *
 * This function clears all the data on the segment LCD, effectively turning off
 * the display. It is useful for resetting the display to a blank state.
 *
 * @param dev Pointer to the segment LCD device structure.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the clear operation
 */
int wm_hal_seg_lcd_clear(wm_hal_seg_lcd_dev_t *dev);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_HAL_SEG_LCD_H__ */

/**
 * @file wm_drv_tft_lcd.h
 *
 * @brief Driver LCD(TFT) COMMON Module
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

#ifndef __WM_DRV_TFT_LCD_H__
#define __WM_DRV_TFT_LCD_H__

#include "wm_types.h"
#include "wm_error.h"
#include "wm_attr.h"
#include "wm_dt.h"
#include "wm_reg_op.h"
#include "wm_drv_tft_lcd_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_DRV_TFT_LCD_Enumerations WM TFT_LCD Enumerations
 * @brief WinnerMicro TFT_LCD Enumerations
 */

/**
 * @addtogroup WM_DRV_TFT_LCD_Enumerations
 * @{
 */

/**
 * @brief Enumeration with tft lcd display rotation
 */
typedef enum {
    LCD_ROTATION_NONE,       /**< NOMAL without display rotation   */
    LCD_ROTATION_90_DEGREE,  /**< 90 degree rotation for the display */
    LCD_ROTATION_180_DEGREE, /**< 180 degree rotation for the display */
    LCD_ROTATION_270_DEGREE, /**< 270 degree rotation for the display */
    LCD_ROTATION_MAX,        /**< The max number of rotation enum */
} wm_lcd_rotate_t;

/**
 * @brief Enumeration with command type for TFT LCD
 */
typedef enum {
    LCD_CMD_TYPE_8BIT  = 0, /**< 8-bit Command Length */
    LCD_CMD_TYPE_16BIT = 1, /**< 16-bit Command Length  */
    LCD_CMD_TYPE_MAX,
} wm_lcd_cmd_type_t;

/**
 * @}
 */

/**
 * @defgroup WM_DRV_TFT_LCD_Macros WM TFT_LCD Macros
 * @brief WinnerMicro TFT_LCD Macros
 */

/**
 * @addtogroup WM_DRV_TFT_LCD_Macros
 * @{
 */

/**
 * @brief Enumeration with display window and data descriptor
 */
typedef struct {
    uint16_t x_start;  /**< Start pixel in the X direction for the display window
                         *  if rotation is 0 or 180 degree, the range is from 0 to (WM_CFG_TFT_LCD_X_RESOLUTION - 1)
                         *  if rotation is 90 or 270 degree, the range is from 0 to (WM_CFG_TFT_LCD_Y_RESOLUTION - 1) */
    uint16_t y_start;  /**< Start pixel in the Y direction for the display window
                         *  if rotation is 0 or 180 degree, the range is from 0 to (WM_CFG_TFT_LCD_Y_RESOLUTION - 1)
                         *  if rotation is 90 or 270 degree, the range is from 0 to (WM_CFG_TFT_LCD_X_RESOLUTION - 1) */
    uint16_t x_end;    /**< End pixel in the X direction for the display window
                         *  the valid range is same as x_start's */
    uint16_t y_end;    /**< End pixel in the Y direction for the display window
                         *  the valid range is same as y_start's */
    uint8_t *buf;      /**< Pointer to the display data buffer for this window, 
                         *  The buffer must be located in RAM and can be allocated using 
                         *  malloc/calloc or be a global variable. 
                         *  Const value or pointers to FLASH physical addresses are not supported */
    uint32_t buf_size; /**< Size of data buffer for the window in bytes
                         *  The size must be equals to the product of  
                         *  (x_end-x_start+1)*(y_end-y_start+1)*Pixel_Width
                         *  Currently, 'Pixel_Width' is set to 2, indicating 16-bit color depth*/
} wm_lcd_data_desc_t;

/**
 * @brief Structure holding lcd capabilities.
 */
typedef struct {
    uint16_t x_resolution;    /**< Current Horizontal display resolution, indicating the total number of pixels in a row.
                                *  the value is dependent with rotation setting,
                                *  if rotation is 0 or 180 degree, x_resolution is equal to WM_CFG_TFT_LCD_X_RESOLUTION,
                                *  if rotation is 90 or 270 degree, x_resolution is equal to WM_CFG_TFT_LCD_Y_RESOLUTION */
    uint16_t y_resolution;    /**< current Vertical display resolution, indicating the total number of pixels in a column.
                                *  the value is dependent with rotation setting,
                                *  if rotation is 0 or 180 degree, y_resolution is equal to WM_CFG_TFT_LCD_Y_RESOLUTION,
                                *  if rotation is 90 or 270 degree, y_resolution is equal to WM_CFG_TFT_LCD_X_RESOLUTION */
    wm_lcd_rotate_t rotation; /**< Current display orientation */
} wm_lcd_capabilitys_t;

/**
 * @}
 */

/**
 * @defgroup WM_DRV_TFT_LCD_Type_Definitions WM TFT_LCD Type Definitions
 * @brief WinnerMicro TFT_LCD Type Definitions
 */

/**
 * @addtogroup WM_DRV_TFT_LCD_Type_Definitions
 * @{
 */

/**
 * @brief  callback function of lcd module once the async trasmissioin done
 * 
 * @param[in]  result  return value from driver layer once transmission done
 * WM_ERR_SUCCESS: succeed
 * others: this transmission is failed in driver layer
 * @param[in]  data   User-defined data to pass to the callback function
 * 
 * @return  None
 * 
 * @note Avoid handling any blocking or time-consuming processes in this callback,
 * since it is directly called by the ISR handler. Failure to do so may result in
 * other tasks or ISRs not being scheduled on time.
 */
typedef void (*wm_lcd_tx_callback_t)(int result, void *data);

/**
 * @}
 */

/**
 * @defgroup WM_DRV_TFT_LCD_Functions WM TFT_LCD Functions
 * @brief WinnerMicro TFT_LCD Functions
 */

/**
 * @addtogroup WM_DRV_TFT_LCD_Functions
 * @{
 */

/**
 * @brief  initialize tft lcd driver
 *
 * @param[in]  dev_name  LCD TFT device name must be identical to the name specified in the device table
 * @return
 *         - !NULL : Initialize success, return the pointer to the TFT LCD driver device
 *         - NULL :  Param error or initialize failed or already be initialized
 */
wm_device_t *wm_drv_tft_lcd_init(const char *dev_name);

/**
 * @brief Deinitializes the TFT LCD driver and releases all allocated resources.
 *
 * @param[in] dev The pointer to the TFT LCD driver device.
 *
 * @return WM_ERR_SUCCESS on success, others error code on failure.
 *
 */
int wm_drv_tft_lcd_deinit(wm_device_t *dev);

/**
 * @brief Starts the bitmap content transmission to TFT LCD device.
 * This function is responsible for selecting a display window on the TFT LCD device and subsequently 
 * transmitting the image content to the device's GRAM (Graphics Random Access Memory) for rendering.
 * 
 * @param[in] dev       The pointer to the TFT LCD driver device.
 * @param[in] data_desc Display window and data descriptor, type of @ref wm_lcd_data_desc_t
 *
 * @return WM_ERR_SUCCESS on success, others error code on failure.
 *
 * @note This funciton using cpu polling for the transmission by default.  it will utilize DMA
 * if the transmission callback is reigstered via the API @ref wm_drv_tft_lcd_register_tx_callback
 * and the buf size is exceeds 32 bytes.
 */
int wm_drv_tft_lcd_draw_bitmap(wm_device_t *dev, wm_lcd_data_desc_t data_desc);

/**
 * @brief This function adjusts the display orientation by sending a specific configuration command 
 * to the device. It defines the order of transformation for the x and y coordinates and whether the 
 * rows and columns should be exchanged. This allows for flexible display configurations, accommodating 
 * various user interface requirements.
 * 
 * @param[in] dev    The pointer to the TFT LCD driver device.
 * @param[in] degree The rotation direction, type of @ref wm_lcd_rotate_t
 *
 * @return WM_ERR_SUCCESS on success, others error code on failure.
 *
 * @note This function solely modifies the TFT LCD rotation attribute. The application must also 
 * adjust the image content accordingly. Failure to do so may result in the image being partially 
 * obscured when a 90-degree or 270-degree rotation is applied.
 */
int wm_drv_tft_lcd_set_rotation(wm_device_t *dev, wm_lcd_rotate_t degree);

/**
 * @brief This function is designed to manage the I/O control for the interface connected to the 
 * backlight pin of the TFT LCD device, to control it's backlight state.
 * 
 * @param[in] dev    The pointer to the TFT LCD driver device.
 * @param[in] on_off The backlight on or off setting, TRUE:backlight on, FALSE: backlight off
 *
 * @return WM_ERR_SUCCESS on success, others error code on failure.
 */
int wm_drv_tft_lcd_set_backlight(wm_device_t *dev, bool on_off);

/**
 * @brief This function obtains the functional parameters currently in effect and in use for the LCD device.
 * 
 * @param[in] dev  The pointer to the TFT LCD driver device.
 * @param[in] cap  The pointer to the capabilities currently being utilized by the TFT LCD driver device.
 *                 type of @ref wm_lcd_capabilitys_t
 *
 * @return WM_ERR_SUCCESS on success, others error code on failure.
 */
int wm_drv_tft_lcd_get_capability(wm_device_t *dev, wm_lcd_capabilitys_t *cap);

/**
 * @brief This function is purposefully engineered to dispatch commands to the TFT LCD device, 
 * with the capacity to include pertinent data payloads where necessary.
 * Upon receipt of these commands, the TFT LCD device is capable of altering its operational 
 * attributes accordingly, such as adjusting the pixel format or other configurable parameters.
 * 
 * @param[in] dev  The pointer to the TFT LCD driver device.
 * @param[in] cmd  The command which sending to TFT LCD device. 
 * @param[in] cmd_type  The command length which sending to TFT LCD device. 
 * @param[in] data The pointer indicates the data which sending to TFT LCD device. 
 * @param[in] data_len The data len which is sending to TFT LCD device. 
 *
 * @return WM_ERR_SUCCESS on success, others error code on failure.
 * @note It is imperative that the commands and data issued to the TFT LCD device strictly adhere 
 * to the specifications outlined in the device's datasheet. The compatibility and functionality of 
 * the device are contingent upon the accurate implementation of these parameters. Developers should 
 * refer to the LCD device's datasheet for detailed command sequences and data formats. Additionally, 
 * the header files associated with this TFT LCD may provide a predefined command list that encapsulates 
 * the necessary instructions for proper device operation. 
 */
int wm_drv_tft_lcd_send_cmd_data(wm_device_t *dev, uint16_t cmd, wm_lcd_cmd_type_t cmd_type, uint8_t *data, uint16_t data_len);

/**
 * @brief This function is register a callback to TFT LCD driver device for the async transmission.
 * once the transmission done, this callback would be invoked in the ISR handler.
 * 
 * @param[in] dev  The pointer to the TFT LCD driver device.
 * @param[in] func The pointer of callback function which need to be reigsterd to TFT LCD driver. 
 *                 type of @ref wm_lcd_tx_callback_t
 * @param[in] arg  The pointer of user data which ISR handler will input to callback function.
 * 
 * @return WM_ERR_SUCCESS on success, others error code on failure.
 * 
 * @note Avoid handling any blocking or time-consuming processes in this callback, since it is directly 
 * called by the ISR handler. Failure to do so may result in other tasks or ISRs not being scheduled 
 * on time.
 */
int wm_drv_tft_lcd_register_tx_callback(wm_device_t *dev, wm_lcd_tx_callback_t func, void *arg);

/**
 * @brief This function is unregister the callback from TFT LCD driver device
 * 
 * @param[in] dev  The pointer to the TFT LCD driver device.

 * @return WM_ERR_SUCCESS on success, others error code on failure.
 */
int wm_drv_tft_lcd_unregister_tx_callback(wm_device_t *dev);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_DRV_TFT_LCD_H__ */
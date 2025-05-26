/**
 * @file wm_drv_tft_lcd_cfg.h
 *
 * @brief Driver LCD(TFT) CONFIGURATION FILE
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

#ifndef __WM_DRV_TFT_LCD_CFG_H__
#define __WM_DRV_TFT_LCD_CFG_H__

#ifdef __cplusplus
extern "C" {
#endif

/** This file provides a comprehensive summary of basic information related to LCD devices.
 *  Special notice:
 *  - The device names listed herein must precisely match the definitions in the device table.
 *  - When a new LCD device is added, it is essential to add new options to the COMPONENT_DRIVER_TFT_LCD_OPTIONS 
 *    managed by menuconfig, which is located in components/driver/kconfig.
*/

/** List all lcd device's device name */
#define DEV_NAME_NV3041A_SPI "nv3041a_spi"
#define DEV_NAME_ST7735_SPI  "st7735_spi"
#define DEV_NAME_GZ035_SPI   "gz035_spi"
#define DEV_NAME_GC9A01_SPI  "gc9a01_spi"

/** List all lcd device's resoultion and rotation info */
/*  Special notice:
 *  - For WM_CFG_TFT_LCD_PIXEL_WIDTH，it must match with the color mode and rgb order setting in lcd device driver
 *  - Example： COLMOD and MADCTRL in wm_nv3041a.h / wm_st7735.h, only RGB565(16bit) be support in current lcd driver
 */
#if defined(CONFIG_COMPONENT_DRIVER_LCD_NV3041A_SPI)
#define WM_CFG_TFT_LCD_DEVICE_NAME  DEV_NAME_NV3041A_SPI
#define WM_CFG_TFT_LCD_X_RESOLUTION 480 //Horizontal display resolution in LCD spec,independent of WM_CFG_TFT_LCD_ROTATION.
#define WM_CFG_TFT_LCD_Y_RESOLUTION 272 //Vertical display resolution in LCD spec, independent of WM_CFG_TFT_LCD_ROTATION.
#define WM_CFG_TFT_LCD_ROTATION     0   //The rotation to be used.
#define WM_CFG_TFT_LCD_PIXEL_WIDTH  2   //The pixel width to be used, RGB565(2 Bytes).

#elif defined(CONFIG_COMPONENT_DRIVER_LCD_ST7735_SPI)
/* User need to check the ST7735 LCD Model's GM mode pin selection which descripted on ST7735's Datasheet.
 * GM[2:0]=000 , 132RGB x 162 resolution 
 * GM[2:0]=011 , 128RGB x 160 resolution 
*/
#define WM_CFG_TFT_LCD_DEVICE_NAME  DEV_NAME_ST7735_SPI
#define WM_CFG_TFT_LCD_X_RESOLUTION 132
#define WM_CFG_TFT_LCD_Y_RESOLUTION 162
#define WM_CFG_TFT_LCD_ROTATION     0
#define WM_CFG_TFT_LCD_PIXEL_WIDTH  2 //RGB565(2 Bytes)

#elif defined(CONFIG_COMPONENT_DRIVER_LCD_GZ035_SPI)
#define WM_CFG_TFT_LCD_DEVICE_NAME  DEV_NAME_GZ035_SPI
#define WM_CFG_TFT_LCD_X_RESOLUTION 212
#define WM_CFG_TFT_LCD_Y_RESOLUTION 318
#define WM_CFG_TFT_LCD_ROTATION     1
#define WM_CFG_TFT_LCD_PIXEL_WIDTH  2 //RGB565(2 Bytes)

#elif defined(CONFIG_COMPONENT_DRIVER_LCD_GC9A01_SPI)
#define WM_CFG_TFT_LCD_DEVICE_NAME  DEV_NAME_GC9A01_SPI
#define WM_CFG_TFT_LCD_X_RESOLUTION 240
#define WM_CFG_TFT_LCD_Y_RESOLUTION 240
#define WM_CFG_TFT_LCD_ROTATION     0
#define WM_CFG_TFT_LCD_PIXEL_WIDTH  2 //RGB565(2 Bytes)

#endif

#ifdef __cplusplus
}
#endif

#endif /* __WM_DRV_TFT_LCD_CFG_H__ */
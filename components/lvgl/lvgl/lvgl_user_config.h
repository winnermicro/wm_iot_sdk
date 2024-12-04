/**
 * @file lvgl_user_config.h
 * Include all user config setting for LVGL related headers
 */

#ifndef LVGL_USER_CONFIG_H
#define LVGL_USER_CONFIG_H

#include "wm_drv_tft_lcd_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      DEFINES
 *********************/

/*Indicate the exact LCD device name as defined in the device table and wm_drv_tft_lcd_cfg.h*/
#define WM_LVGL_LCD_MODULE_NAME WM_CFG_TFT_LCD_DEVICE_NAME

/* actual screen width, it must equals to the x-resolution in the wm_drv_tft_lcd_cfg.h if no rotation */
#define MY_DISP_HOR_RES         ((WM_CFG_TFT_LCD_ROTATION % 2) ? WM_CFG_TFT_LCD_Y_RESOLUTION : WM_CFG_TFT_LCD_X_RESOLUTION)

/* actual screen height, it must equals to the y-resolution in the wm_drv_tft_lcd_cfg.h if no rotation */
#define MY_DISP_VER_RES         ((WM_CFG_TFT_LCD_ROTATION % 2) ? WM_CFG_TFT_LCD_X_RESOLUTION : WM_CFG_TFT_LCD_Y_RESOLUTION)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_USER_CONFIG_H*/

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

#ifndef __WM_DRV_OPS_TFT_LCD_H__
#define __WM_DRV_OPS_TFT_LCD_H__

#include "wm_types.h"
#include "wm_error.h"
#include "wm_attr.h"
#include "wm_dt.h"
#include "wm_drv_tft_lcd.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief driver api abstraction layer for lcd module
 */
typedef struct {
    int (*lcd_init)(wm_device_t *dev);                                      /**< Initialization */
    int (*lcd_deinit)(wm_device_t *dev);                                    /**< De-initialization  */
    int (*draw_bitmap)(wm_device_t *dev, wm_lcd_data_desc_t desc);          /**< Draw bitmap  */
    int (*lcd_set_backlight)(wm_device_t *dev, bool on_off);                /**< Control lcd backlight */
    int (*lcd_set_rotation)(wm_device_t *dev, wm_lcd_rotate_t degree);      /**< Control lcd x ,y axis */
    int (*lcd_get_capability)(wm_device_t *dev, wm_lcd_capabilitys_t *cap); /**< Get lcd capability */
    int (*lcd_cmd_data)(wm_device_t *dev, uint16_t cmd, wm_lcd_cmd_type_t cmd_type, uint8_t *data,
                        uint16_t data_len); /**< Send command with data payload to lcd */
    int (*lcd_add_tx_callback)(wm_device_t *dev, wm_lcd_tx_callback_t callback,
                               void *usr_data);      /**< Register tx done callback */
    int (*lcd_remove_tx_callback)(wm_device_t *dev); /**< Unregister tx done callback */
} wm_drv_tft_lcd_ops_t;

#ifdef __cplusplus
}
#endif

#endif /* __WM_DRV_OPS_TFT_LCD_H__ */
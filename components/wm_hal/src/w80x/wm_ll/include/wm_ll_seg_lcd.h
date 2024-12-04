/**
 * @file wm_ll_seg_lcd.h
 *
 * @brief Segment LCD LL Driver Module
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

#ifndef __WM_LL_SEG_LCD_H__
#define __WM_LL_SEG_LCD_H__

#include "wm_reg_op.h"
#include "wm_attr.h"
#include "wm_seg_lcd_reg_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable the LCD module
 *
 * @param reg Pointer to the lcd_ctrl_t structure
 *
 * @return None
 */
ATTRIBUTE_INLINE void wm_ll_seg_lcd_enable(wm_seg_lcd_reg_t *reg)
{
    reg->ctrl.val |= WM_BIT(8); // Set the bit to enable the LCD module
}

/**
 * @brief Disable the LCD module
 *
 * @param reg Pointer to the lcd_ctrl_t structure
 *
 * @return None
 */
ATTRIBUTE_INLINE void wm_ll_seg_lcd_disable(wm_seg_lcd_reg_t *reg)
{
    reg->ctrl.val &= ~WM_BIT(8); // Clear the bit to disable the LCD module
}

/**
 * @brief Power up the LCD module
 *
 * @param reg Pointer to the lcd_ctrl_t structure
 *
 * @return None
 */
ATTRIBUTE_INLINE void wm_ll_seg_lcd_power_up(wm_seg_lcd_reg_t *reg)
{
    reg->ctrl.lcd_pd = 1; // Clear the bit to power up the LCD module
}

/**
 * @brief Power down the LCD module
 *
 * @param reg Pointer to the lcd_ctrl_t structure
 *
 * @return None
 */
ATTRIBUTE_INLINE void wm_ll_seg_lcd_power_down(wm_seg_lcd_reg_t *reg)
{
    reg->ctrl.lcd_pd = 0; // Set the bit to power down the LCD module
}

/**
 * @brief Set the LCD duty selection
 *
 * @param reg Pointer to the lcd_ctrl_t structure
 * @param duty_sel Desired LCD duty selection
 *
 * @return None
 */
ATTRIBUTE_INLINE void wm_ll_seg_lcd_set_duty_sel(wm_seg_lcd_reg_t *reg, uint32_t duty_sel)
{
    reg->ctrl.duty_sel = duty_sel;
}

/**
 * @brief Set the LCD output supply voltage selection
 *
 * @param reg Pointer to the lcd_ctrl_t structure
 * @param vlcd_cc Desired LCD output supply voltage selection
 *
 * @return None
 */
ATTRIBUTE_INLINE void wm_ll_seg_lcd_set_vlcd_cc(wm_seg_lcd_reg_t *reg, uint32_t vlcd_cc)
{
    reg->ctrl.vlcd_cc = vlcd_cc;
}

/**
 * @brief Set the internal bias configuration
 *
 * @param reg Pointer to the lcd_ctrl_t structure
 * @param bias Desired LCD internal bias configuration
 *
 * @return None
 */
ATTRIBUTE_INLINE void wm_ll_seg_lcd_set_bias(wm_seg_lcd_reg_t *reg, uint32_t bias)
{
    reg->ctrl.bias = bias;
}

/**
 * @brief Set the LCD output drive strength control
 *
 * @param reg Pointer to the lcd_ctrl_t structure
 * @param hd Desired LCD output drive strength control
 *
 * @return None
 */
ATTRIBUTE_INLINE void wm_ll_seg_lcd_set_hd(wm_seg_lcd_reg_t *reg, uint32_t hd)
{
    reg->ctrl.hd = hd;
}

/**
 * @brief Set the frame frequency for the LCD
 *
 * @param reg Pointer to the lcd_frame_cnt_t structure
 * @param frame_cnt Desired frame frequency setting
 *
 * @return None
 */
ATTRIBUTE_INLINE void wm_ll_seg_lcd_set_frame_cnt(wm_seg_lcd_reg_t *reg, uint32_t frame_cnt)
{
    reg->frame_cnt.val = frame_cnt;
}

/**
 * @brief Get the display control at the COMx and SEG intersection
 *
 * @param reg Pointer to the lcd_reg_t structure
 * @param com_id COM line to be configured, from LCD_COM0 to LCD_COM7
 *
 * @return Desired display control value for the specified COM line
 */
ATTRIBUTE_INLINE uint32_t wm_ll_seg_lcd_get_comx_seg_ctl(wm_seg_lcd_reg_t *reg, uint32_t com_id)
{
    return reg->com[com_id].val;
}

/**
 * @brief Set the display control at the COMx and SEG intersection
 *
 * @param reg Pointer to the lcd_reg_t structure
 * @param com_id COM line to be configured, from LCD_COM0 to LCD_COM7
 * @param seg_ctl Desired display control value for the specified COM line
 *
 * @return None
 */
ATTRIBUTE_INLINE void wm_ll_seg_lcd_set_comx_seg_ctl(wm_seg_lcd_reg_t *reg, uint32_t com_id, uint32_t seg_ctl)
{
    reg->com[com_id].val = seg_ctl;
}

/**
 * @brief Enable the COM lines for the LCD
 *
 * @param reg Pointer to the lcd_com_io_en_t structure
 * @param com_io_en Bitmask for COM line IO enable
 *
 * @return None
 */
ATTRIBUTE_INLINE void wm_ll_seg_lcd_set_com_lines(wm_seg_lcd_reg_t *reg, uint32_t com_io_en)
{
    WM_LL_REG_FILED_COPY_WRTIE(reg->com_io_en, lcd_io_en, com_io_en);
}

/**
 * @brief Get the COM lines for the LCD
 *
 * @param reg Pointer to the lcd_com_io_en_t structure
 * @param com_io_en Bitmask for COM line IO enable
 *
 * @return the COM lines for the LCD
 */
ATTRIBUTE_INLINE uint8_t wm_ll_seg_lcd_get_com_lines(wm_seg_lcd_reg_t *reg)
{
    return WM_LL_REG_FILED_COPY_READ(reg->com_io_en, lcd_io_en);
}

/**
 * @brief Enable the SEG lines for the LCD
 *
 * @param reg Pointer to the lcd_seg_io_en_t structure
 * @param seg_io_en Bitmask for SEG line IO enable
 *
 * @return None
 */
ATTRIBUTE_INLINE void wm_ll_seg_lcd_set_seg_lines(wm_seg_lcd_reg_t *reg, uint32_t seg_io_en)
{
    reg->seg_io_en.val = seg_io_en;
}

/**
 * @brief Get the SEG lines for the LCD
 *
 * @param reg Pointer to the lcd_seg_io_en_t structure
 * @param seg_io_en Bitmask for SEG line IO enable
 * 
 * @return the SEG lines for the LCD
 */
ATTRIBUTE_INLINE uint32_t wm_ll_seg_lcd_get_seg_lines(wm_seg_lcd_reg_t *reg)
{
    return reg->seg_io_en.val;
}

#ifdef __cplusplus
}
#endif

#endif /* __WM_LL_SEG_LCD_H__ */
/**
 * @file wm_hal_seg_lcd.c
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
#include <stdio.h>
#include <string.h>
#include "wm_soc.h"
#include "wm_reg_op.h"
#include "wm_ll_seg_lcd.h"
#include "wm_hal_seg_lcd.h"

#if WM_SEG_LCD_DEBUG
#undef LOG_TAG
#define LOG_TAG "HAL_SEG_LCD"
#include "wm_log.h"
#define WM_SEG_LCD_LOG_D(...) wm_log_debug(__VA_ARGS__)
#define WM_SEG_LCD_LOG_I(...) wm_log_info(__VA_ARGS__)
#define WM_SEG_LCD_LOG_E(...) wm_log_error(__VA_ARGS__)
#else
#endif
#define WM_SEG_LCD_ASSERT       wm_log_assert

//TODO: Select based on MPU unit
#define WM_SEG_LCD_INTERNAL_32K 32000 /**< Define the clock frequency of the segment LCD to be internal 32 kHz. */

int wm_hal_seg_lcd_init(wm_hal_seg_lcd_dev_t *dev)
{
    uint32_t frame_cnt        = 0;
    uint32_t frame_freq_total = 0;
    uint32_t remainder        = 0;

    /*
     * Set the frame counter value frame_cnt to ensure the LCD works at the desired refresh rate.
     * The specific conversion formula is:
     * frame_cnt = (clk_rtc / frm_freq) / com_num
     *
     * clk_rtc is the RTC clock signal frequency, with a frequency of 32.768KHz;
     * frm_freq is the desired frame refresh rate;
     * com_num is the number of COM pins;
     *
     * Note, if the frame_cnt value is not an integer, please round to the nearest integer value for configuration in the register.
     */
    if (dev->seg_lcd_cfg.frame_freq != 0 && dev->seg_lcd_cfg.com_num != 0) {
        frame_freq_total = dev->seg_lcd_cfg.frame_freq * dev->seg_lcd_cfg.com_num;
        frame_cnt        = WM_SEG_LCD_INTERNAL_32K / frame_freq_total;
        remainder        = WM_SEG_LCD_INTERNAL_32K % frame_freq_total;
        if ((remainder << 1) >= frame_freq_total) {
            frame_cnt += 1;
        }
        if (frame_cnt >= WM_BIT(11)) {
            return WM_ERR_INVALID_PARAM;
        }
    }

    wm_ll_seg_lcd_power_up(dev->seg_lcd_reg);
    wm_ll_seg_lcd_set_hd(dev->seg_lcd_reg, dev->seg_lcd_cfg.hd);
    wm_ll_seg_lcd_set_bias(dev->seg_lcd_reg, dev->seg_lcd_cfg.bias);
    wm_ll_seg_lcd_set_vlcd_cc(dev->seg_lcd_reg, dev->seg_lcd_cfg.vlcd_cc);
    wm_ll_seg_lcd_set_duty_sel(dev->seg_lcd_reg, dev->seg_lcd_cfg.duty_sel);
    wm_ll_seg_lcd_set_frame_cnt(dev->seg_lcd_reg, frame_cnt);
    wm_ll_seg_lcd_enable(dev->seg_lcd_reg);

    return WM_ERR_SUCCESS;
}

int wm_hal_seg_lcd_deinit(wm_hal_seg_lcd_dev_t *dev)
{
    wm_ll_seg_lcd_disable(dev->seg_lcd_reg);
    wm_ll_seg_lcd_power_down(dev->seg_lcd_reg);

    return WM_ERR_SUCCESS;
}

int wm_hal_seg_lcd_write(wm_hal_seg_lcd_dev_t *dev, wm_seg_lcd_com_id_t com_id, uint32_t seg_mask, uint32_t seg_data)
{
    uint8_t com_io_enable  = 0;
    uint32_t seg_io_enable = 0;
    uint32_t seg_ctl_val   = 0;

    if (com_id >= WM_SEG_LCD_COM_MAX) {
        return WM_ERR_INVALID_PARAM;
    }

    seg_ctl_val = wm_ll_seg_lcd_get_comx_seg_ctl(dev->seg_lcd_reg, com_id);
    wm_ll_seg_lcd_set_comx_seg_ctl(dev->seg_lcd_reg, com_id, (seg_ctl_val & ~seg_mask) | seg_data);

    com_io_enable = wm_ll_seg_lcd_get_com_lines(dev->seg_lcd_reg);
    if ((com_io_enable & WM_BIT(com_id)) != WM_BIT(com_id)) {
        com_io_enable |= WM_BIT(com_id);
        wm_ll_seg_lcd_set_com_lines(dev->seg_lcd_reg, com_io_enable);
    }

    seg_io_enable = wm_ll_seg_lcd_get_seg_lines(dev->seg_lcd_reg);
    if ((seg_io_enable & seg_data) != seg_data) {
        seg_io_enable |= seg_data;
        wm_ll_seg_lcd_set_seg_lines(dev->seg_lcd_reg, seg_io_enable);
    }

    return WM_ERR_SUCCESS;
}

int wm_hal_seg_lcd_clear(wm_hal_seg_lcd_dev_t *dev)
{
    uint8_t com_io_enable = wm_ll_seg_lcd_get_com_lines(dev->seg_lcd_reg);

    for (wm_seg_lcd_com_id_t i = 0; i < WM_SEG_LCD_COM_MAX; i++) {
        if ((com_io_enable >> i) & 0x01) {
            wm_ll_seg_lcd_set_comx_seg_ctl(dev->seg_lcd_reg, i, 0);
        }
    }

    wm_ll_seg_lcd_set_com_lines(dev->seg_lcd_reg, 0);
    wm_ll_seg_lcd_set_seg_lines(dev->seg_lcd_reg, 0);

    return WM_ERR_SUCCESS;
}

#if WM_SEG_LCD_DEBUG

void wm_hal_seg_lcd_regs_dump(wm_hal_seg_lcd_dev_t *dev)
{
    if (dev == NULL) {
        WM_SEG_LCD_LOG_E("Invalid register pointer");
        return;
    }

    WM_SEG_LCD_LOG_I("SEG LCD Registers:");
    WM_SEG_LCD_LOG_I("ctrl.duty_sel      : %u", dev->seg_lcd_reg->ctrl.duty_sel);
    WM_SEG_LCD_LOG_I("ctrl.vlcd_cc       : %u", dev->seg_lcd_reg->ctrl.vlcd_cc);
    WM_SEG_LCD_LOG_I("ctrl.bias          : %u", dev->seg_lcd_reg->ctrl.bias);
    WM_SEG_LCD_LOG_I("ctrl.lcd_en        : %u", dev->seg_lcd_reg->ctrl.lcd_en);
    WM_SEG_LCD_LOG_I("ctrl.lcd_pd        : %u", dev->seg_lcd_reg->ctrl.lcd_pd);
    WM_SEG_LCD_LOG_I("ctrl.hd            : %u", dev->seg_lcd_reg->ctrl.hd);

    WM_SEG_LCD_LOG_I("frame_cnt.frame_cnt : %u", dev->seg_lcd_reg->frame_cnt.frame_cnt);

    for (wm_seg_lcd_com_id_t i = 0; i < WM_SEG_LCD_COM_MAX; i++) {
        WM_SEG_LCD_LOG_I("com[%d].comx_seg_ctl: 0x%08x", i, dev->seg_lcd_reg->com[i].comx_seg_ctl);
    }

    WM_SEG_LCD_LOG_I("com_io_en.lcd_io_en : 0x%08x", dev->seg_lcd_reg->com_io_en.lcd_io_en);
    WM_SEG_LCD_LOG_I("seg_io_en.lcd_io_en : 0x%08x", dev->seg_lcd_reg->seg_io_en.lcd_io_en);
}

#endif

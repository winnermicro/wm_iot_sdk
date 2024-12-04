/**
 * @file wm_seg_lcd_reg_struct.h
 *
 * @brief Segment LCD Driver Module
 *
 */

/**
 *  Copyright 2022-2024 Beijing WinnerMictrloelectronics Co.,Ltd.
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

#ifndef __WM_SEG_LCD_REG_STRUCT_H__
#define __WM_SEG_LCD_REG_STRUCT_H__

#include "wm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Segment LCD Control Register
 * This union is used to configure the LCD controller settings.
 */
typedef union {
    struct {
        uint32_t duty_sel    : 3;  /**< [2:0] Duty selection */
        uint32_t vlcd_cc     : 3;  /**< [5:3] Output supply voltage selection */
        uint32_t bias        : 2;  /**< [7:6] Internal bias configuration */
        uint32_t lcd_en      : 1;  /**< [8] LCD module enable signal */
        uint32_t lcd_pd      : 1;  /**< [9] LCD module power-down signal */
        uint32_t reserved_10 : 2;  /**< [11:10] Reserved bits */
        uint32_t hd          : 1;  /**< [12] LCD output drive strength control */
        uint32_t reserved_13 : 19; /**< [31:13] Reserved bits */
    };
    uint32_t val; /**< The raw value of the register */
} wm_seg_lcd_ctrl_t;

/**
 * @brief Segment LCD Frame Frequency Setting Register
 * This union is used to set the frame frequency for the LCD.
 */
typedef union {
    struct {
        uint32_t frame_cnt   : 11; /**< [10:0] Frame frequency setting */
        uint32_t reserved_11 : 21; /**< [31:11] Reserved bits */
    };
    uint32_t val; /* The raw value of the register */
} wm_seg_lcd_frame_cnt_t;

/**
 * @brief Segment LCD COMx Display Setting Registers
 * These unions are used to control the display at the intersection of COMx and SEG lines.
 */
typedef union {
    struct {
        uint32_t comx_seg_ctl : 32; /**< [31:0] Display control at the COMx and SEG intersection */
    };
    uint32_t val; /**< The raw value of the register */
} wm_seg_lcd_comx_t;

/**
 * @brief Segment LCD COMx Enable Registers
 * These unions are used to enable the COMx lines for the LCD.
 */
typedef union {
    struct {
        uint32_t lcd_io_en  : 8;  /**< [7:0] COMx IO enable */
        uint32_t reserved_8 : 24; /**< [31:8] Reserved bits */
    };
    uint32_t val; /* The raw value of the register */
} wm_seg_lcd_com_io_en_t;

/**
 * @brief Segment LCD SEGx Enable Registers
 * These unions are used to enable the SEGx lines for the LCD.
 */
typedef union {
    struct {
        uint32_t lcd_io_en : 32; /**< [31:0] SEGx IO enable */
    };
    uint32_t val; /* The raw value of the register */
} wm_seg_lcd_seg_io_en_t;

/**
 * @brief Segment LCD register block
 * This structure is used to encapsulate the LCD driver's registers.
 */
typedef struct {
    volatile wm_seg_lcd_ctrl_t ctrl;           /**< SEG_LCD_BASE_ADDR        */
    volatile wm_seg_lcd_frame_cnt_t frame_cnt; /**< SEG_LCD_BASE_ADDR + 0x04 */
    volatile wm_seg_lcd_comx_t com[8];         /**< SEG_LCD_BASE_ADDR + 0x08 */
    volatile wm_seg_lcd_com_io_en_t com_io_en; /**< SEG_LCD_BASE_ADDR + 0x28 */
    volatile wm_seg_lcd_seg_io_en_t seg_io_en; /**< SEG_LCD_BASE_ADDR + 0x2C */
} wm_seg_lcd_reg_t;

#ifdef __cplusplus
}
#endif

#endif /* __WM_SEG_LCD_REG_STRUCT_H__ */

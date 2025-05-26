/**
 * @file wm_gc9a01.h
 *
 * @brief LCD TFT GC9A01 Module
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

#ifndef __WM_DRV_GC9A01_H__
#define __WM_DRV_GC9A01_H__

#include "wm_types.h"
#include "wm_error.h"
#include "wm_attr.h"

#ifdef __cplusplus
extern "C" {
#endif

/* LCD command list which is strictly matchs with LCD's datasheet */
// Display size
#define GC9A01_WIDTH             240
#define GC9A01_HEIGHT            240
#define GC9A01_CHARS_COLS_LEN    5 // number of columns for chars
#define GC9A01_CHARS_ROWS_LEN    8 // number of rows for chars

// Command
#define GC9A01_CASET             0x2A /* Window columes set */
#define GC9A01_RASET             0x2B /* Window rows set    */
#define GC9A01_RAMWR             0x2C /* Ram write read     */
#define GC9A01_MADCTL            0x36 /* Rotation control   */

// Memory Access Control
#define GC9A01_CFG_MADCTL_MY     (1 << 7) /* Row Address Order */
#define GC9A01_CFG_MADCTL_MX     (1 << 6) /* Column Address Order */
#define GC9A01_CFG_MADCTL_MV     (1 << 5) /* Row / Column Exchange */
#define GC9A01_CFG_MADCTL_ML     (1 << 4) /* Vertical Refresh Order */
#define GC9A01_CFG_MADCTL_BGR    (1 << 3) /* RGB or BGR Order */
#define GC9A01_CFG_MADCTL_MH     (1 << 2) /* Horizontal Refresh Order */
#define GC9A01_CFG_MADCTL_NORMAL 0        /* Default state */

// Colors Content for one pixel with RGB565 format
#define GC9A01_BLACK             0x0000
#define GC9A01_BLUE              0x001F
#define GC9A01_RED               0xF800
#define GC9A01_GREEN             0x07E0
#define GC9A01_CYAN              0x07FF
#define GC9A01_MAGENTA           0xF81F
#define GC9A01_YELLOW            0xFFE0
#define GC9A01_WHITE             0xFFFF
#define GC9A01_COLOR565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3))

/* The command table definiation, which using TLV format
   the command header is defined by LCD_CMD_SEQ_HEADER_LEN
   len = cmd content + data content */
const uint8_t gc9a01_init_seq[] = {
    /*  len , delay(ms), cmd_type,  cmd, data ... */

    /* GC9A01 Chip Manual.pdf page 91 */
    0x01, 0x00, LCD_CMD_TYPE_8BIT, 0xFE,       // Inner register enable 1
    0x01, 0x00, LCD_CMD_TYPE_8BIT, 0xEF,       // Inner register enable 2
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xEB, 0x14, //
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x84, 0x40, //
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x85, 0xFF, //
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x86, 0xFF, //
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x87, 0xFF, //
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x8E, 0xFF, //
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x8F, 0xFF, //

    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x88, 0x0A, //
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x89, 0x21, //
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x8A, 0x00, //
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x8B, 0x80, //

    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x8C, 0x01, //
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x8D, 0x01, //

    0x03, 0x00, LCD_CMD_TYPE_8BIT, 0xB6, 0x00, 0x02, //Display Function Control
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x36, 0x48,       //Memory Access Control
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x3A, 0x05,       // Mcu interface Format 16bits/pixel

    0x05, 0x00, LCD_CMD_TYPE_8BIT, 0x90, 0x08, 0x08, 0x08, 0x08, //

    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xBD, 0x06,             //
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xBC, 0x00,             //
    0x04, 0x00, LCD_CMD_TYPE_8BIT, 0xFF, 0x60, 0x01, 0x04, //

    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xC3, 0x17, // fliker
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xC4, 0x17, // fliker

    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xC9, 0x22,             //Vreg2a voltage Control
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xBE, 0x11,             //
    0x03, 0x00, LCD_CMD_TYPE_8BIT, 0xE1, 0x10, 0x0E,       //
    0x04, 0x00, LCD_CMD_TYPE_8BIT, 0xDF, 0x21, 0x0C, 0x02, //

    0x07, 0x00, LCD_CMD_TYPE_8BIT, 0xF0, 0x45, 0x09, 0x08, 0x08, 0x26, 0x2A, //
    0x07, 0x00, LCD_CMD_TYPE_8BIT, 0xF1, 0x43, 0x70, 0x72, 0x36, 0x37, 0x6F, //
    0x07, 0x00, LCD_CMD_TYPE_8BIT, 0xF2, 0x45, 0x09, 0x08, 0x08, 0x26, 0x2A, //

    0x07, 0x00, LCD_CMD_TYPE_8BIT, 0xF3, 0x43, 0x70, 0x72, 0x36, 0x37, 0x6F, //
    0x03, 0x00, LCD_CMD_TYPE_8BIT, 0xED, 0x1B, 0x0B,                         //
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xAE, 0x77,                               //
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xCD, 0x63,                               //

    0x0A, 0x00, LCD_CMD_TYPE_8BIT, 0x70, 0x07, 0x07, 0x04, 0x0E, 0x10, 0x09, 0x07, 0x08, 0x03, //
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xE8, 0x34,                                                 //

    0x0D, 0x00, LCD_CMD_TYPE_8BIT, 0x62, 0x18, 0x0D, 0x71, 0xED, 0x70, 0x70, 0x18, 0x0F, 0x71, 0xEF, 0x70, 0x70, //
    0x0D, 0x00, LCD_CMD_TYPE_8BIT, 0x63, 0x18, 0x11, 0x71, 0xF1, 0x70, 0x70, 0x18, 0x13, 0x71, 0xF3, 0x70, 0x70, //

    0x08, 0x00, LCD_CMD_TYPE_8BIT, 0x64, 0x28, 0x29, 0xF1, 0x01, 0xF1, 0x00, 0x07,                   //
    0x0B, 0x00, LCD_CMD_TYPE_8BIT, 0x66, 0x3C, 0x00, 0xCD, 0x67, 0x45, 0x45, 0x10, 0x00, 0x00, 0x00, //
    0x0B, 0x00, LCD_CMD_TYPE_8BIT, 0x67, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x01, 0x54, 0x10, 0x32, 0x98, //

    //frame rate, 10 60HZ  10  62HZ   10 65HZ   10   67HZ     10  70HZ
    0x08, 0x00, LCD_CMD_TYPE_8BIT, 0x74, 0x10, 0x85, 0x80, 0x00, 0x00, 0x4E, 0x00, //

    0x03, 0x00, LCD_CMD_TYPE_8BIT, 0x98, 0x3e, 0x07, //
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x35, 0x00,       //Tearing Effect Line ON
    0x01, 120, LCD_CMD_TYPE_8BIT, 0x21,              //Display Inversion ON
    0x01, 255, LCD_CMD_TYPE_8BIT, 0x11,              //Sleep out, need sleep 320 ms
    0x01, 120, LCD_CMD_TYPE_8BIT, 0x29,              //Display ON
    0x01, 0x00, LCD_CMD_TYPE_8BIT, 0x2C,             //Memory Write

    0, // END bytes
};

#ifdef __cplusplus
}
#endif

#endif /* __WM_DRV_GC9A01_H__ */

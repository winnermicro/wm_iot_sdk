/**
 * @file wm_st7735.h
 *
 * @brief LCD TFT ST7735 Module
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

#ifndef __WM_DRV_ST7735_H__
#define __WM_DRV_ST7735_H__

#include "wm_types.h"
#include "wm_error.h"
#include "wm_attr.h"

#ifdef __cplusplus
extern "C" {
#endif

/* LCD command list which is strictly matchs with LCD's datasheet */
// Display size
#define ST7735_WIDTH             128
#define ST7735_HEIGHT            160
#define ST7735_CHARS_COLS_LEN    5 // number of columns for chars
#define ST7735_CHARS_ROWS_LEN    8 // number of rows for chars

// Commands
#define ST7735_NOP               0x00
#define ST7735_SWRESET           0x01
#define ST7735_RDDID             0x04
#define ST7735_RDDST             0x09
#define ST7735_SLPIN             0x10
#define ST7735_SLPOUT            0x11
#define ST7735_PTLON             0x12
#define ST7735_NORON             0x13
#define ST7735_INVOFF            0x20
#define ST7735_INVON             0x21
#define ST7735_GAMSET            0x26
#define ST7735_DISPOFF           0x28
#define ST7735_DISPON            0x29
#define ST7735_CASET             0x2A
#define ST7735_RASET             0x2B
#define ST7735_RAMWR             0x2C
#define ST7735_RAMRD             0x2E
#define ST7735_PTLAR             0x30
#define ST7735_TEOFF             0x34
#define ST7735_TEON              0x35
#define ST7735_MADCTL            0x36
#define ST7735_COLMOD            0x3A
#define ST7735_FRMCTR1           0xB1
#define ST7735_FRMCTR2           0xB2
#define ST7735_FRMCTR3           0xB3
#define ST7735_INVCTR            0xB4
#define ST7735_DISSET5           0xB6
#define ST7735_PWCTR1            0xC0
#define ST7735_PWCTR2            0xC1
#define ST7735_PWCTR3            0xC2
#define ST7735_PWCTR4            0xC3
#define ST7735_PWCTR5            0xC4
#define ST7735_VMCTR1            0xC5
#define ST7735_RDID1             0xDA
#define ST7735_RDID2             0xDB
#define ST7735_RDID3             0xDC
#define ST7735_RDID4             0xDD
#define ST7735_PWCTR6            0xFC
#define ST7735_GMCTRP1           0xE0
#define ST7735_GMCTRN1           0xE1

// Memory Access Control
#define ST7735_CFG_MADCTL_MY     (1 << 7) /* Row Address Order */
#define ST7735_CFG_MADCTL_MX     (1 << 6) /* Column Address Order */
#define ST7735_CFG_MADCTL_MV     (1 << 5) /* Row / Column Exchange */
#define ST7735_CFG_MADCTL_ML     (1 << 4) /* Vertical Refresh Order */
#define ST7735_CFG_MADCTL_BGR    (1 << 3) /* RGB or BGR Order */
#define ST7735_CFG_MADCTL_MH     (1 << 2) /* Horizontal Refresh Order */
#define ST7735_CFG_MADCTL_NORMAL 0        /* Default state */

// Colors Content for one pixel with RGB565 format
#define ST7735_BLACK             0x0000
#define ST7735_BLUE              0x001F
#define ST7735_RED               0xF800
#define ST7735_GREEN             0x07E0
#define ST7735_CYAN              0x07FF
#define ST7735_MAGENTA           0xF81F
#define ST7735_YELLOW            0xFFE0
#define ST7735_WHITE             0xFFFF
#define ST7735_COLOR565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3))

/* The command table definiation, which using TLV format 
   the command header is defined by LCD_CMD_SEQ_HEADER_LEN
   len = cmd content + data content */
const uint8_t st7735_init_seq[] = {
    /*  len , delay(ms), cmd_type,  cmd, data ... */
    //0x03, 0x00, LCD_CMD_TYPE_16BIT, 0x12, 0x34, 0xBE, // Example:16bit command "0x1234" be used
    0x01, 0x00, LCD_CMD_TYPE_8BIT, ST7735_SWRESET,      // Software reset
    0x01, 0x00, LCD_CMD_TYPE_8BIT, ST7735_SLPOUT,       // Out of sleep mode
    0x02, 0x00, LCD_CMD_TYPE_8BIT, ST7735_COLMOD, 0x05, // set color mode - 16-bit color

    /* frame rate ctrl */
    0x04, 0x00, LCD_CMD_TYPE_8BIT, ST7735_FRMCTR1, 0x01, 0x2C, 0x2D,                   // normal mode
    0x04, 0x10, LCD_CMD_TYPE_8BIT, ST7735_FRMCTR2, 0x01, 0x2C, 0x2D,                   // idle mode
    0x07, 0x00, LCD_CMD_TYPE_8BIT, ST7735_FRMCTR3, 0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D, // dot/line inver mode

    /* display inversion control*/
    0x02, 0x00, LCD_CMD_TYPE_8BIT, ST7735_INVCTR, 0x07, // no inversion

    /* power control */
    0x04, 0x00, LCD_CMD_TYPE_8BIT, ST7735_PWCTR1, 0xA2, 0x02, 0x84, // -4.6V, Auto mode
    0x02, 0x00, LCD_CMD_TYPE_8BIT, ST7735_PWCTR2, 0xC5,             // VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
    0x03, 0x00, LCD_CMD_TYPE_8BIT, ST7735_PWCTR3, 0x0A, 0x00,       // Opamp current small,-Boost frequency
    0x03, 0x00, LCD_CMD_TYPE_8BIT, ST7735_PWCTR4, 0x8A, 0x2A,       // BCLK/2, Opamp current small & Medium low
    0x03, 0x00, LCD_CMD_TYPE_8BIT, ST7735_PWCTR5, 0x8A, 0xEE,       // , ,
    0x02, 0x00, LCD_CMD_TYPE_8BIT, ST7735_VMCTR1, 0x0E,             // ,

    0x02, 0x00, LCD_CMD_TYPE_8BIT, ST7735_MADCTL, 0xC0, // R and B byte are swapped
    0x01, 0x00, LCD_CMD_TYPE_8BIT, ST7735_NORON,        // Normal display on, no args
    0x01, 0x00, LCD_CMD_TYPE_8BIT, ST7735_DISPON,       // Main screen turn on, no args

    0, // END bytes
};

#ifdef __cplusplus
}
#endif

#endif /* __WM_DRV_ST7735_H__ */

/**
 * @file wm_gz035.h
 *
 * @brief LCD TFT GZ035 Module
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

#ifndef __WM_DRV_GZ035_H__
#define __WM_DRV_GZ035_H__

#include "wm_types.h"
#include "wm_error.h"
#include "wm_attr.h"

#ifdef __cplusplus
extern "C" {
#endif

/* LCD command list which is strictly matchs with LCD's datasheet */
// Display size
#define GZ035_WIDTH             212
#define GZ035_HEIGHT            318

// Commands
#define GZ035_CASET             0x2A
#define GZ035_RASET             0x2B
#define GZ035_RAMWR             0x2C
#define GZ035_MADCTL            0x36
#define GZ035_COLMOD            0x3A

// Memory Access Control
#define GZ035_CFG_MADCTL_MY     (1 << 7) /* Row Address Order */
#define GZ035_CFG_MADCTL_MX     (1 << 6) /* Column Address Order */
#define GZ035_CFG_MADCTL_MV     (1 << 5) /* Row / Column Exchange */
#define GZ035_CFG_MADCTL_ML     (1 << 4) /* Vertical Refresh Order */
#define GZ035_CFG_MADCTL_BGR    (1 << 3) /* RGB or BGR Order */
#define GZ035_CFG_MADCTL_MH     (1 << 2) /* Horizontal Refresh Order */
#define GZ035_CFG_MADCTL_NORMAL 0        /* Default state */

// Colors
#define GZ035_BLACK             0x0000
#define GZ035_BLUE              0x001F
#define GZ035_RED               0xF800
#define GZ035_GREEN             0x07E0
#define GZ035_CYAN              0x07FF
#define GZ035_MAGENTA           0xF81F
#define GZ035_YELLOW            0xFFE0
#define GZ035_WHITE             0xFFFF
#define GZ035_COLOR565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3))

#define GZ035_GAMMA_2_2_DATA                                                                                                \
    0x3F, 0x38, 0x33, 0x30, 0x35, 0x3B, 0x38, 0x3A, 0x39, 0x3B, 0x3C, 0x39, 0x38, 0x34, 0x01, 0x01, 0x3F, 0x38, 0x33, 0x30, \
        0x35, 0x3B, 0x38, 0x3A, 0x39, 0x3B, 0x3C, 0x39, 0x38, 0x34, 0x01, 0x01

/* The command table definiation, which using TLV format 
   the command header is defined by LCD_CMD_SEQ_HEADER_LEN
   len = cmd content + data content */
const uint8_t gz035_init_seq[] = {
    /*  len , delay(ms), cmd_type,  cmd, data ... */
    //0x03, 0x00, LCD_CMD_TYPE_16BIT, 0x12, 0x34, 0xBE, // Example:16bit command "0x1234" be used
    0x03, 0x00, LCD_CMD_TYPE_8BIT, 0xDF, 0x98, 0x53,                                                             //
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xDE, 0x00,                                                                   //
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xB2, 0x40,                                                                   //
    0x05, 0x00, LCD_CMD_TYPE_8BIT, 0xB7, 0x00, 0x33, 0x0, 0x5B,                                                  //
    0x07, 0x00, LCD_CMD_TYPE_8BIT, 0xBB, 0x4C, 0x2F, 0x55, 0x73, 0x6F, 0xF0,                                     //
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xBC, 0x77,                                                                   //
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xC1, 0x12,                                                                   //
    0x09, 0x00, LCD_CMD_TYPE_8BIT, 0xC3, 0x7D, 0x08, 0x14, 0x06, 0xC9, 0x72, 0x6C, 0x77,                         //
    0x0D, 0x00, LCD_CMD_TYPE_8BIT, 0xC4, 0x00, 0x00, 0xA0, 0x79, 0x0A, 0x0E, 0x16, 0x79, 0x0A, 0x0E, 0x16, 0x82, //60Hz,320 Line
    /*GAMMA*/
    0x21, 0x00, LCD_CMD_TYPE_8BIT, 0xC8, GZ035_GAMMA_2_2_DATA,         //G2.5
    0x06, 0x00, LCD_CMD_TYPE_8BIT, 0xD0, 0x04, 0x06, 0x6A, 0x0F, 0x00, //
    0x03, 0x00, LCD_CMD_TYPE_8BIT, 0xD7, 0x00, 0x30,                   //2DL_OPT
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xE6, 0x10,                         //
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xDE, 0x01,                         //
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xBB, 0x04,                         //
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xD7, 0x12,                         //
    0x06, 0x00, LCD_CMD_TYPE_8BIT, 0xB7, 0x03, 0x13, 0xEF, 0x3B, 0x3B, //
    0x04, 0x00, LCD_CMD_TYPE_8BIT, 0xC1, 0x14, 0x15, 0xC0,             //
    0x03, 0x00, LCD_CMD_TYPE_8BIT, 0xC2, 0x06, 0x3A,                   //
    0x03, 0x00, LCD_CMD_TYPE_8BIT, 0xC4, 0x72, 0x12,                   //
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xBE, 0x00,                         //
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xDE, 0x02,                         //
    0x04, 0x00, LCD_CMD_TYPE_8BIT, 0xE5, 0x00, 0x02, 0x00,             //
    0x04, 0x00, LCD_CMD_TYPE_8BIT, 0xE5, 0x01, 0x02, 0x00,             //
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xDE, 0x00,                         //
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x35, 0x00,                         //
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x36, 0x60, //MADCTL，00=FW正扫 ; 03=BW反扫（上下镜像）；0x40 左右镜像
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x3A, 0x05, //06=RGB666；05=RGB565
    0x05, 0x00, LCD_CMD_TYPE_8BIT, 0x2A, 0x00, 0x0E, 0x00, 0xE1, //Start_X=14, End_X=225
    0x05, 0x00, LCD_CMD_TYPE_8BIT, 0x2B, 0x00, 0x00, 0x01, 0x3D, //Start_Y=0,  End_Y=317
    0x01, 0x8C, LCD_CMD_TYPE_8BIT, 0x11,                         //
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xDE, 0x02,                   //
    0x04, 0x00, LCD_CMD_TYPE_8BIT, 0xE5, 0x00, 0x02, 0x00,       //
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xDE, 0x00,                   //
    0x01, 0x0A, LCD_CMD_TYPE_8BIT, 0x29,                         //

    0x00, //End Bytes
};

#ifdef __cplusplus
}
#endif

#endif /* __WM_DRV_GZ035_H__ */

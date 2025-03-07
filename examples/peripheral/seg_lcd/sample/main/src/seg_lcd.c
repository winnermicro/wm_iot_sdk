/**
 * @file seg_lcd.c
 *
 * @brief SEG_LCD Module
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

#include "seg_lcd.h"

/*
+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+
| PIN     | 1       | 2       | 3       | 4       | 5       | 6       | 7       | 8       | 9       | 10      | 11      | 12      | 13      | 14      | 15      | 16      | 17      | 18      | 19      | 20      |
|         | COM1    | COM2    | COM3    | COM4    | SEG1    | SEG2    | SEG3    | SEG4    | SEG5    | SEG6    | SEG7    | SEG8    | SEG9    | SEG10   | SEG11   | SEG12   | SEG13   | SEG14   | SEG15   | SEG16   |
+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+
| COM1    | COM1    |         | --      | --      | 下午    | 上午     | 7A      | 7B      | 8A      | 8B      | 9A      | 9B      | H3      | 13A     | 13B     | 14B     | 15A     | 15B     | 11B     | 11A    |
| COM2    | --      | COM2    | --      | --      | 6AGD    | 6B      | 7F      | 7G      | 8F      | 8G      | 9F      | 9G      | 12BC    | 13F     | 13G     | 14C     | 15F     | 15G     | 11G     | 11F    |
| COM3    | --      | --      | COM3    | --      | 6E      | 6C      | 7E      | 7C      | 8E      | 8C      | 9E      | 9C      | 星期六   | 13E     | 13C     | 14AGD   | 15E     | 15C     | 11C     | 11E    |
| COM4    | --      | --      | --      | COM4    | 星期一   | 星期二   | 7D      | COL     | 8D      | 星期三  | 9D      | 星期四   | 星期五  | 13D     | H5      | 14E     | 15D     | 星期日   | 室外     | 11D    |
+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+

+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+
| PIN     | 21      | 22      | 23      | 24      | 25      | 26      | 27      | 28      | 29      | 30      | 31      | 32      | 33      | 34      | 35      | 36      | 37      | 38      | 39      | 40      |
|         | SEG17   | SEG18   | SEG19   | SEG20   | SEG21   | SEG22   | SEG23   | SEG24   | SEG25   | SEG26   | SEG27   | SEG28   | SEG29   | SEG30   | SEG31   | SEG32   | COM1    | COM2    | COM3    | COM4   |
+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+
| COM1    | 10B     | 10A     | L9      | H1      | 5B      | 5A      | 4B      | 4A      | 3B      | 3A      | H2      | 2B      | 2A      | 1B      | 1A      | L1      | COM1    | --      | --      | --      |
| COM2    | 10G     | 10F     | L11     | L6      | 5G      | 5F      | 4G      | 4F      | 3G      | 3F      | 炎热    | 2G      | 2F      | 1G      | 1F      | L2      | --      | COM2    | --      | --      |
| COM3    | 10C     | 10E     | L10     | L7      | 5C      | 5E      | 4C      | 4E      | 3C      | 3E      | 舒适2   | 2C      | 2E      | 1C      | 1E      | L3      | --      | --      | COM3    | --      |
| COM4    | H4      | 10D     | L12     | L8      | 潮湿    | 5D      | 舒适1    | 4D      | 干燥    | 3D      | 寒冷    | L5      | 2D      | L4      | 1D      | --      | --      | --      | --      | COM4    |
+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+
*/

/**
 * @brief Defines the mapping of segment names to their respective COM and bit mask.
 *
 * This table is used to associate each segment with a specific COM line and bit
 * mask for easy manipulation of the segments on the LCD.
 * 
 * @note
 * - The logical name of the segment should not exceed 7 characters in length
 *   and should only contain English characters.
 * - Please complete the encapsulation of other display content by yourself
 */
const seg_lcd_table_t g_seg_lcd_table[124] = {
    { "PM",    WM_SEG_LCD_COM0, WM_SEG_LCD_SEG1  },
    { "AM",    WM_SEG_LCD_COM0, WM_SEG_LCD_SEG2  },
    { "7A",    WM_SEG_LCD_COM0, WM_SEG_LCD_SEG3  },
    { "7B",    WM_SEG_LCD_COM0, WM_SEG_LCD_SEG4  },
    { "8A",    WM_SEG_LCD_COM0, WM_SEG_LCD_SEG5  },
    { "8B",    WM_SEG_LCD_COM0, WM_SEG_LCD_SEG6  },
    { "9A",    WM_SEG_LCD_COM0, WM_SEG_LCD_SEG7  },
    { "9B",    WM_SEG_LCD_COM0, WM_SEG_LCD_SEG8  },
    { "H3",    WM_SEG_LCD_COM0, WM_SEG_LCD_SEG9  },
    { "13A",   WM_SEG_LCD_COM0, WM_SEG_LCD_SEG10 },
    { "13B",   WM_SEG_LCD_COM0, WM_SEG_LCD_SEG11 },
    { "14B",   WM_SEG_LCD_COM0, WM_SEG_LCD_SEG12 },
    { "15A",   WM_SEG_LCD_COM0, WM_SEG_LCD_SEG13 },
    { "15B",   WM_SEG_LCD_COM0, WM_SEG_LCD_SEG14 },
    { "11B",   WM_SEG_LCD_COM0, WM_SEG_LCD_SEG15 },
    { "11A",   WM_SEG_LCD_COM0, WM_SEG_LCD_SEG16 },
    { "10B",   WM_SEG_LCD_COM0, WM_SEG_LCD_SEG17 },
    { "10A",   WM_SEG_LCD_COM0, WM_SEG_LCD_SEG18 },
    { "L9",    WM_SEG_LCD_COM0, WM_SEG_LCD_SEG19 },
    { "H1",    WM_SEG_LCD_COM0, WM_SEG_LCD_SEG20 },
    { "5B",    WM_SEG_LCD_COM0, WM_SEG_LCD_SEG21 },
    { "5A",    WM_SEG_LCD_COM0, WM_SEG_LCD_SEG22 },
    { "4B",    WM_SEG_LCD_COM0, WM_SEG_LCD_SEG23 },
    { "4A",    WM_SEG_LCD_COM0, WM_SEG_LCD_SEG24 },
    { "3B",    WM_SEG_LCD_COM0, WM_SEG_LCD_SEG25 },
    { "3A",    WM_SEG_LCD_COM0, WM_SEG_LCD_SEG26 },
    { "H2",    WM_SEG_LCD_COM0, WM_SEG_LCD_SEG27 },
    { "2B",    WM_SEG_LCD_COM0, WM_SEG_LCD_SEG28 },
    { "2A",    WM_SEG_LCD_COM0, WM_SEG_LCD_SEG29 },
    { "1B",    WM_SEG_LCD_COM0, WM_SEG_LCD_SEG30 },
    { "1A",    WM_SEG_LCD_COM0, WM_SEG_LCD_SEG31 },
    // { "L1",   WM_SEG_LCD_COM0, WM_SEG_LCD_SEG32 },

    { "6AGD",  WM_SEG_LCD_COM1, WM_SEG_LCD_SEG1  },
    { "6B",    WM_SEG_LCD_COM1, WM_SEG_LCD_SEG2  },
    { "7F",    WM_SEG_LCD_COM1, WM_SEG_LCD_SEG3  },
    { "7G",    WM_SEG_LCD_COM1, WM_SEG_LCD_SEG4  },
    { "8F",    WM_SEG_LCD_COM1, WM_SEG_LCD_SEG5  },
    { "8G",    WM_SEG_LCD_COM1, WM_SEG_LCD_SEG6  },
    { "9F",    WM_SEG_LCD_COM1, WM_SEG_LCD_SEG7  },
    { "9G",    WM_SEG_LCD_COM1, WM_SEG_LCD_SEG8  },
    { "12BC",  WM_SEG_LCD_COM1, WM_SEG_LCD_SEG9  },
    { "13F",   WM_SEG_LCD_COM1, WM_SEG_LCD_SEG10 },
    { "13G",   WM_SEG_LCD_COM1, WM_SEG_LCD_SEG11 },
    { "14C",   WM_SEG_LCD_COM1, WM_SEG_LCD_SEG12 },
    { "15F",   WM_SEG_LCD_COM1, WM_SEG_LCD_SEG13 },
    { "15G",   WM_SEG_LCD_COM1, WM_SEG_LCD_SEG14 },
    { "11G",   WM_SEG_LCD_COM1, WM_SEG_LCD_SEG15 },
    { "11F",   WM_SEG_LCD_COM1, WM_SEG_LCD_SEG16 },
    { "10G",   WM_SEG_LCD_COM1, WM_SEG_LCD_SEG17 },
    { "10F",   WM_SEG_LCD_COM1, WM_SEG_LCD_SEG18 },
    { "L11",   WM_SEG_LCD_COM1, WM_SEG_LCD_SEG19 },
    { "L6",    WM_SEG_LCD_COM1, WM_SEG_LCD_SEG20 },
    { "5G",    WM_SEG_LCD_COM1, WM_SEG_LCD_SEG21 },
    { "5F",    WM_SEG_LCD_COM1, WM_SEG_LCD_SEG22 },
    { "4G",    WM_SEG_LCD_COM1, WM_SEG_LCD_SEG23 },
    { "4F",    WM_SEG_LCD_COM1, WM_SEG_LCD_SEG24 },
    { "3G",    WM_SEG_LCD_COM1, WM_SEG_LCD_SEG25 },
    { "3F",    WM_SEG_LCD_COM1, WM_SEG_LCD_SEG26 },
    { "HOT",   WM_SEG_LCD_COM1, WM_SEG_LCD_SEG27 },
    { "2G",    WM_SEG_LCD_COM1, WM_SEG_LCD_SEG28 },
    { "2F",    WM_SEG_LCD_COM1, WM_SEG_LCD_SEG29 },
    { "1G",    WM_SEG_LCD_COM1, WM_SEG_LCD_SEG30 },
    { "1F",    WM_SEG_LCD_COM1, WM_SEG_LCD_SEG31 },
    // { "L2",   WM_SEG_LCD_COM1, WM_SEG_LCD_SEG32 },

    { "6E",    WM_SEG_LCD_COM2, WM_SEG_LCD_SEG1  },
    { "6C",    WM_SEG_LCD_COM2, WM_SEG_LCD_SEG2  },
    { "7E",    WM_SEG_LCD_COM2, WM_SEG_LCD_SEG3  },
    { "7C",    WM_SEG_LCD_COM2, WM_SEG_LCD_SEG4  },
    { "8E",    WM_SEG_LCD_COM2, WM_SEG_LCD_SEG5  },
    { "8C",    WM_SEG_LCD_COM2, WM_SEG_LCD_SEG6  },
    { "9E",    WM_SEG_LCD_COM2, WM_SEG_LCD_SEG7  },
    { "9C",    WM_SEG_LCD_COM2, WM_SEG_LCD_SEG8  },
    { "SAT",   WM_SEG_LCD_COM2, WM_SEG_LCD_SEG9  },
    { "13E",   WM_SEG_LCD_COM2, WM_SEG_LCD_SEG10 },
    { "13C",   WM_SEG_LCD_COM2, WM_SEG_LCD_SEG11 },
    { "14AGD", WM_SEG_LCD_COM2, WM_SEG_LCD_SEG12 },
    { "15E",   WM_SEG_LCD_COM2, WM_SEG_LCD_SEG13 },
    { "15C",   WM_SEG_LCD_COM2, WM_SEG_LCD_SEG14 },
    { "11C",   WM_SEG_LCD_COM2, WM_SEG_LCD_SEG15 },
    { "11E",   WM_SEG_LCD_COM2, WM_SEG_LCD_SEG16 },
    { "10C",   WM_SEG_LCD_COM2, WM_SEG_LCD_SEG17 },
    { "10E",   WM_SEG_LCD_COM2, WM_SEG_LCD_SEG18 },
    { "L10",   WM_SEG_LCD_COM2, WM_SEG_LCD_SEG19 },
    { "L7",    WM_SEG_LCD_COM2, WM_SEG_LCD_SEG20 },
    { "5C",    WM_SEG_LCD_COM2, WM_SEG_LCD_SEG21 },
    { "5E",    WM_SEG_LCD_COM2, WM_SEG_LCD_SEG22 },
    { "4C",    WM_SEG_LCD_COM2, WM_SEG_LCD_SEG23 },
    { "4E",    WM_SEG_LCD_COM2, WM_SEG_LCD_SEG24 },
    { "3C",    WM_SEG_LCD_COM2, WM_SEG_LCD_SEG25 },
    { "3E",    WM_SEG_LCD_COM2, WM_SEG_LCD_SEG26 },
    { "COMF2", WM_SEG_LCD_COM2, WM_SEG_LCD_SEG27 },
    { "2C",    WM_SEG_LCD_COM2, WM_SEG_LCD_SEG28 },
    { "2E",    WM_SEG_LCD_COM2, WM_SEG_LCD_SEG29 },
    { "1C",    WM_SEG_LCD_COM2, WM_SEG_LCD_SEG30 },
    { "1E",    WM_SEG_LCD_COM2, WM_SEG_LCD_SEG31 },
    // { "L3",   WM_SEG_LCD_COM2, WM_SEG_LCD_SEG32 },

    { "MON",   WM_SEG_LCD_COM3, WM_SEG_LCD_SEG1  },
    { "TUE",   WM_SEG_LCD_COM3, WM_SEG_LCD_SEG2  },
    { "7D",    WM_SEG_LCD_COM3, WM_SEG_LCD_SEG3  },
    { "COL",   WM_SEG_LCD_COM3, WM_SEG_LCD_SEG4  },
    { "8D",    WM_SEG_LCD_COM3, WM_SEG_LCD_SEG5  },
    { "WED",   WM_SEG_LCD_COM3, WM_SEG_LCD_SEG6  },
    { "9D",    WM_SEG_LCD_COM3, WM_SEG_LCD_SEG7  },
    { "THU",   WM_SEG_LCD_COM3, WM_SEG_LCD_SEG8  },
    { "FRI",   WM_SEG_LCD_COM3, WM_SEG_LCD_SEG9  },
    { "13D",   WM_SEG_LCD_COM3, WM_SEG_LCD_SEG10 },
    { "H5",    WM_SEG_LCD_COM3, WM_SEG_LCD_SEG11 },
    { "14E",   WM_SEG_LCD_COM3, WM_SEG_LCD_SEG12 },
    { "15D",   WM_SEG_LCD_COM3, WM_SEG_LCD_SEG13 },
    { "SUN",   WM_SEG_LCD_COM3, WM_SEG_LCD_SEG14 },
    { "OUT",   WM_SEG_LCD_COM3, WM_SEG_LCD_SEG15 },
    { "11D",   WM_SEG_LCD_COM3, WM_SEG_LCD_SEG16 },
    { "H4",    WM_SEG_LCD_COM3, WM_SEG_LCD_SEG17 },
    { "10D",   WM_SEG_LCD_COM3, WM_SEG_LCD_SEG18 },
    { "L12",   WM_SEG_LCD_COM3, WM_SEG_LCD_SEG19 },
    { "L8",    WM_SEG_LCD_COM3, WM_SEG_LCD_SEG20 },
    { "HUMID", WM_SEG_LCD_COM3, WM_SEG_LCD_SEG21 },
    { "5D",    WM_SEG_LCD_COM3, WM_SEG_LCD_SEG22 },
    { "COMF1", WM_SEG_LCD_COM3, WM_SEG_LCD_SEG23 },
    { "4D",    WM_SEG_LCD_COM3, WM_SEG_LCD_SEG24 },
    { "DRY",   WM_SEG_LCD_COM3, WM_SEG_LCD_SEG25 },
    { "3D",    WM_SEG_LCD_COM3, WM_SEG_LCD_SEG26 },
    { "COLD",  WM_SEG_LCD_COM3, WM_SEG_LCD_SEG27 },
    { "L5",    WM_SEG_LCD_COM3, WM_SEG_LCD_SEG28 },
    { "2D",    WM_SEG_LCD_COM3, WM_SEG_LCD_SEG29 },
    { "L4",    WM_SEG_LCD_COM3, WM_SEG_LCD_SEG30 },
    { "1D",    WM_SEG_LCD_COM3, WM_SEG_LCD_SEG31 }
};

int seg_lcd_display_all(wm_device_t *dev)
{
    int ret = WM_ERR_SUCCESS;

    seg_lcd_display_t seg_lcd_display[sizeof(g_seg_lcd_table) / sizeof(g_seg_lcd_table[0])] = { 0 };

    for (uint32_t i = 0; i < (sizeof(g_seg_lcd_table) / sizeof(g_seg_lcd_table[0])); i++) {
        strcpy(seg_lcd_display[i].seg_name, g_seg_lcd_table[i].seg_name);
        seg_lcd_display[i].is_on = true;
    }

    ret = wm_drv_seg_lcd_display_seg(dev, seg_lcd_display, sizeof(seg_lcd_display) / sizeof(seg_lcd_display[0]));

    return ret;
}

int seg_lcd_clear_all(wm_device_t *dev)
{
    return wm_drv_seg_lcd_clear(dev);
}

/**
 * Please complete the encapsulation of other display content by yourself.
 * You can refer to wm_gdc0689.c for implementation details.
 */

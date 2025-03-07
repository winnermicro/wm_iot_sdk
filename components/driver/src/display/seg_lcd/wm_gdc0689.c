/**
 * @file wm_gdc0689.c
 *
 * @brief WM_GDC0689 Module
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
#include "wm_gdc0689.h"
#include "wm_drv_seg_lcd.h"
#include "wm_dt_hw.h"

#define LOG_TAG "example"
#include "wm_log.h"

/*
  +---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+
  | PIN | 1    | 2    | 3    | 4    | 5    | 6    | 7    | 8    | 9    | 10   | 11   | 12   | 13   | 14   | 15   |
  | LCD | SEG1 | SEG2 | SEG3 | SEG4 | SEG5 | SEG6 | SEG7 | SEG8 | SEG9 | SEG10| SEG11| COM0 | COM1 | COM2 | COM3 |
  +---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+
  +---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+
  | PIN | 16   | 17   | 18   | 19   | 20   | 21   | 22   | 23   | 24   | 25   | 26   | 27   | 28   | 29   |
  | LCD | SEG12| SEG13| SEG14| SEG15| SEG16| SEG17| SEG18| SEG19| SEG20| SEG21| SEG22| SEG23| SEG24| SEG25|
  +---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+

  +---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+
  | LCD  | SEG1 | SEG2 | SEG3 | SEG4 | SEG5 | SEG6 | SEG7 | SEG8 | SEG9 | SEG10| SEG11|
  | COM0 |  1D  |  DP1 |  2D  |  DP2 |  3D  |  DP3 |  4D  | COL1 | COL2 |  W5  |  L1  |
  | COM1 |  1E  |  1C  |  2E  |  2C  |  3E  |  3C  |  4E  |  4C  | COL3 |  W4  |  L2  |
  | COM2 |  1G  |  1B  |  2G  |  2B  |  3G  |  3B  |  4G  |  4B  |  T1  |  W3  |  L3  |
  | COM3 |  1F  |  1A  |  2F  |  2A  |  3F  |  3A  |  4F  |  4A  |  W1  |  W2  |  L4  |
  +---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+
  +---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+
  | LCD  | SEG12| SEG13| SEG14| SEG15| SEG16| SEG17| SEG18| SEG19| SEG20| SEG21| SEG22| SEG23| SEG24| SEG25|
  | COM0 |  5F  |  5A  |  6F  |  6A  |  7F  |  7A  |  S4  |  S5  |  8F  |  8A  |  9F  |  9A  |  10F |  10A |
  | COM1 |  5G  |  5B  |  6G  |  6B  |  7G  |  7B  |  S3  |  S6  |  8G  |  8B  |  9G  |  9B  |  10G |  10B |
  | COM2 |  5E  |  5C  |  6E  |  6C  |  7E  |  7C  |  S2  |  S7  |  8E  |  8C  |  9E  |  9C  |  10E |  10C |
  | COM3 |  5D  |  DP5 |  6D  |  DP6 |  7D  |  DP7 |  S1  |  S8  |  8D  |  DP8 |  9D  |  DP9 |  10D |  S9  |
  +---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+---------+
*/

/**
 * @brief Defines the device context structure related to the GDC0680 driver.
 *
 * This structure contains all the context information required by the driver,
 * such as the pointer to the segment LCD device.
 */
typedef struct {
    wm_device_t *seg_lcd_dev; /**< Pointer to the device structure controlling the segment LCD */
} wm_drv_gdc0680_dev_ctx_t;

/**
 * @brief Defines the specific data structure for the GDC0689 driver.
 *
 * This structure inherits from wm_drv_gdc0680_dev_ctx_t and provides the specific context information required by the GDC0689 driver.
 */
typedef struct {
    wm_drv_gdc0680_dev_ctx_t drv_ctx; /**< Nested structure containing the driver context */
} wm_drv_gdc0689_drv_t;

/**
 * @brief Defines the mapping of segment names to their respective COM and bit mask.
 *
 * This table is used to associate each segment with a specific COM line and bit
 * mask for easy manipulation of the segments on the LCD.
 */
static const seg_lcd_table_t gdc0689_seg_lcd_table[100] = {
    { "1D",   WM_SEG_LCD_COM0, WM_SEG_LCD_SEG1  },
    { "DP1",  WM_SEG_LCD_COM0, WM_SEG_LCD_SEG2  },
    { "2D",   WM_SEG_LCD_COM0, WM_SEG_LCD_SEG3  },
    { "DP2",  WM_SEG_LCD_COM0, WM_SEG_LCD_SEG4  },
    { "3D",   WM_SEG_LCD_COM0, WM_SEG_LCD_SEG5  },
    { "DP3",  WM_SEG_LCD_COM0, WM_SEG_LCD_SEG6  },
    { "4D",   WM_SEG_LCD_COM0, WM_SEG_LCD_SEG7  },
    { "COL1", WM_SEG_LCD_COM0, WM_SEG_LCD_SEG8  },
    { "COL2", WM_SEG_LCD_COM0, WM_SEG_LCD_SEG9  },
    { "W5",   WM_SEG_LCD_COM0, WM_SEG_LCD_SEG10 },
    { "L1",   WM_SEG_LCD_COM0, WM_SEG_LCD_SEG11 },
    { "5F",   WM_SEG_LCD_COM0, WM_SEG_LCD_SEG12 },
    { "5A",   WM_SEG_LCD_COM0, WM_SEG_LCD_SEG13 },
    { "6F",   WM_SEG_LCD_COM0, WM_SEG_LCD_SEG14 },
    { "6A",   WM_SEG_LCD_COM0, WM_SEG_LCD_SEG15 },
    { "7F",   WM_SEG_LCD_COM0, WM_SEG_LCD_SEG16 },
    { "7A",   WM_SEG_LCD_COM0, WM_SEG_LCD_SEG17 },
    { "S4",   WM_SEG_LCD_COM0, WM_SEG_LCD_SEG18 },
    { "S5",   WM_SEG_LCD_COM0, WM_SEG_LCD_SEG19 },
    { "8F",   WM_SEG_LCD_COM0, WM_SEG_LCD_SEG20 },
    { "8A",   WM_SEG_LCD_COM0, WM_SEG_LCD_SEG21 },
    { "9F",   WM_SEG_LCD_COM0, WM_SEG_LCD_SEG22 },
    { "9A",   WM_SEG_LCD_COM0, WM_SEG_LCD_SEG23 },
    { "10F",  WM_SEG_LCD_COM0, WM_SEG_LCD_SEG24 },
    { "10A",  WM_SEG_LCD_COM0, WM_SEG_LCD_SEG25 },

    { "1E",   WM_SEG_LCD_COM1, WM_SEG_LCD_SEG1  },
    { "1C",   WM_SEG_LCD_COM1, WM_SEG_LCD_SEG2  },
    { "2E",   WM_SEG_LCD_COM1, WM_SEG_LCD_SEG3  },
    { "2C",   WM_SEG_LCD_COM1, WM_SEG_LCD_SEG4  },
    { "3E",   WM_SEG_LCD_COM1, WM_SEG_LCD_SEG5  },
    { "3C",   WM_SEG_LCD_COM1, WM_SEG_LCD_SEG6  },
    { "4E",   WM_SEG_LCD_COM1, WM_SEG_LCD_SEG7  },
    { "4C",   WM_SEG_LCD_COM1, WM_SEG_LCD_SEG8  },
    { "COL3", WM_SEG_LCD_COM1, WM_SEG_LCD_SEG9  },
    { "W4",   WM_SEG_LCD_COM1, WM_SEG_LCD_SEG10 },
    { "L2",   WM_SEG_LCD_COM1, WM_SEG_LCD_SEG11 },
    { "5G",   WM_SEG_LCD_COM1, WM_SEG_LCD_SEG12 },
    { "5B",   WM_SEG_LCD_COM1, WM_SEG_LCD_SEG13 },
    { "6G",   WM_SEG_LCD_COM1, WM_SEG_LCD_SEG14 },
    { "6B",   WM_SEG_LCD_COM1, WM_SEG_LCD_SEG15 },
    { "7G",   WM_SEG_LCD_COM1, WM_SEG_LCD_SEG16 },
    { "7B",   WM_SEG_LCD_COM1, WM_SEG_LCD_SEG17 },
    { "S3",   WM_SEG_LCD_COM1, WM_SEG_LCD_SEG18 },
    { "S6",   WM_SEG_LCD_COM1, WM_SEG_LCD_SEG19 },
    { "8G",   WM_SEG_LCD_COM1, WM_SEG_LCD_SEG20 },
    { "8B",   WM_SEG_LCD_COM1, WM_SEG_LCD_SEG21 },
    { "9G",   WM_SEG_LCD_COM1, WM_SEG_LCD_SEG22 },
    { "9B",   WM_SEG_LCD_COM1, WM_SEG_LCD_SEG23 },
    { "10G",  WM_SEG_LCD_COM1, WM_SEG_LCD_SEG24 },
    { "10B",  WM_SEG_LCD_COM1, WM_SEG_LCD_SEG25 },

    { "1G",   WM_SEG_LCD_COM2, WM_SEG_LCD_SEG1  },
    { "1B",   WM_SEG_LCD_COM2, WM_SEG_LCD_SEG2  },
    { "2G",   WM_SEG_LCD_COM2, WM_SEG_LCD_SEG3  },
    { "2B",   WM_SEG_LCD_COM2, WM_SEG_LCD_SEG4  },
    { "3G",   WM_SEG_LCD_COM2, WM_SEG_LCD_SEG5  },
    { "3B",   WM_SEG_LCD_COM2, WM_SEG_LCD_SEG6  },
    { "4G",   WM_SEG_LCD_COM2, WM_SEG_LCD_SEG7  },
    { "4B",   WM_SEG_LCD_COM2, WM_SEG_LCD_SEG8  },
    { "T1",   WM_SEG_LCD_COM2, WM_SEG_LCD_SEG9  },
    { "W3",   WM_SEG_LCD_COM2, WM_SEG_LCD_SEG10 },
    { "L3",   WM_SEG_LCD_COM2, WM_SEG_LCD_SEG11 },
    { "5E",   WM_SEG_LCD_COM2, WM_SEG_LCD_SEG12 },
    { "5C",   WM_SEG_LCD_COM2, WM_SEG_LCD_SEG13 },
    { "6E",   WM_SEG_LCD_COM2, WM_SEG_LCD_SEG14 },
    { "6C",   WM_SEG_LCD_COM2, WM_SEG_LCD_SEG15 },
    { "7E",   WM_SEG_LCD_COM2, WM_SEG_LCD_SEG16 },
    { "7C",   WM_SEG_LCD_COM2, WM_SEG_LCD_SEG17 },
    { "S2",   WM_SEG_LCD_COM2, WM_SEG_LCD_SEG18 },
    { "S7",   WM_SEG_LCD_COM2, WM_SEG_LCD_SEG19 },
    { "8E",   WM_SEG_LCD_COM2, WM_SEG_LCD_SEG20 },
    { "8C",   WM_SEG_LCD_COM2, WM_SEG_LCD_SEG21 },
    { "9E",   WM_SEG_LCD_COM2, WM_SEG_LCD_SEG22 },
    { "9C",   WM_SEG_LCD_COM2, WM_SEG_LCD_SEG23 },
    { "10E",  WM_SEG_LCD_COM2, WM_SEG_LCD_SEG24 },
    { "10C",  WM_SEG_LCD_COM2, WM_SEG_LCD_SEG25 },

    { "1F",   WM_SEG_LCD_COM3, WM_SEG_LCD_SEG1  },
    { "1A",   WM_SEG_LCD_COM3, WM_SEG_LCD_SEG2  },
    { "2F",   WM_SEG_LCD_COM3, WM_SEG_LCD_SEG3  },
    { "2A",   WM_SEG_LCD_COM3, WM_SEG_LCD_SEG4  },
    { "3F",   WM_SEG_LCD_COM3, WM_SEG_LCD_SEG5  },
    { "3A",   WM_SEG_LCD_COM3, WM_SEG_LCD_SEG6  },
    { "4F",   WM_SEG_LCD_COM3, WM_SEG_LCD_SEG7  },
    { "4A",   WM_SEG_LCD_COM3, WM_SEG_LCD_SEG8  },
    { "W1",   WM_SEG_LCD_COM3, WM_SEG_LCD_SEG9  },
    { "W2",   WM_SEG_LCD_COM3, WM_SEG_LCD_SEG10 },
    { "L4",   WM_SEG_LCD_COM3, WM_SEG_LCD_SEG11 },
    { "5D",   WM_SEG_LCD_COM3, WM_SEG_LCD_SEG12 },
    { "DP5",  WM_SEG_LCD_COM3, WM_SEG_LCD_SEG13 },
    { "6D",   WM_SEG_LCD_COM3, WM_SEG_LCD_SEG14 },
    { "DP6",  WM_SEG_LCD_COM3, WM_SEG_LCD_SEG15 },
    { "7D",   WM_SEG_LCD_COM3, WM_SEG_LCD_SEG16 },
    { "DP7",  WM_SEG_LCD_COM3, WM_SEG_LCD_SEG17 },
    { "S1",   WM_SEG_LCD_COM3, WM_SEG_LCD_SEG18 },
    { "S8",   WM_SEG_LCD_COM3, WM_SEG_LCD_SEG19 },
    { "8D",   WM_SEG_LCD_COM3, WM_SEG_LCD_SEG20 },
    { "DP8",  WM_SEG_LCD_COM3, WM_SEG_LCD_SEG21 },
    { "9D",   WM_SEG_LCD_COM3, WM_SEG_LCD_SEG22 },
    { "DP9",  WM_SEG_LCD_COM3, WM_SEG_LCD_SEG23 },
    { "10D",  WM_SEG_LCD_COM3, WM_SEG_LCD_SEG24 },
    { "S9",   WM_SEG_LCD_COM3, WM_SEG_LCD_SEG25 }
};

/**
 * @brief Numeric representation for the digits 0 through 9 on the LCD.
 *
 * Each value corresponds to a specific pattern of segment activation that
 * represents a digit when displayed on an LCD.
 */
static const uint8_t g_seg_lcd_num[10] = {
    0x3F, /**< Numeric pattern for digit 0 */
    0x06, /**< Numeric pattern for digit 1 */
    0x5B, /**< Numeric pattern for digit 2 */
    0x4F, /**< Numeric pattern for digit 3 */
    0x66, /**< Numeric pattern for digit 4 */
    0x6D, /**< Numeric pattern for digit 5 */
    0x7D, /**< Numeric pattern for digit 6 */
    0x07, /**< Numeric pattern for digit 7 */
    0x7F, /**< Numeric pattern for digit 8 */
    0x6F  /**< Numeric pattern for digit 9 */
};

/**
 * @brief Names of the segments used for numeric display on a 7-segment display.
 *
 * This array provides the names for the segments typically used in a 7-segment
 * display, allowing for easy reference in the code when manipulating the display.
 */
static const char g_number_seg_name[7][2] = { "A", "B", "C", "D", "E", "F", "G" };

wm_device_t *wm_gdc0689_init(char *dev_name)
{
    int ret                  = WM_ERR_SUCCESS;
    wm_device_t *seg_lcd_dev = NULL;

    if (dev_name == NULL) {
        return NULL;
    }

    seg_lcd_dev = wm_drv_seg_lcd_init(dev_name);
    if (seg_lcd_dev == NULL) {
        return NULL;
    }
    ret = wm_drv_seg_lcd_register_table(seg_lcd_dev, gdc0689_seg_lcd_table,
                                        sizeof(gdc0689_seg_lcd_table) / sizeof(gdc0689_seg_lcd_table[0]));
    if (ret != WM_ERR_SUCCESS) {
        return NULL;
    }

    return seg_lcd_dev;
}

int wm_gdc0689_deinit(wm_device_t *dev)
{
    int ret = WM_ERR_SUCCESS;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    ret = wm_drv_seg_lcd_deinit(dev);
    if (ret != WM_ERR_SUCCESS) {
        return ret;
    }

    return ret;
}

int wm_gdc0689_clear(wm_device_t *dev)
{
    int ret = WM_ERR_SUCCESS;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    ret = wm_drv_seg_lcd_clear(dev);

    return ret;
}

int wm_gdc0689_display_service_icon(wm_device_t *dev, wm_gdc0689_service_icon_t service_icon)
{
    int ret                             = WM_ERR_SUCCESS;
    seg_lcd_display_t seg_lcd_display[] = {
        {
         .seg_name = "T1",
         .is_on    = false,
         }
    };

    if (dev == NULL || service_icon >= WM_GDC0689_SERVICE_ICON_MAX) {
        return WM_ERR_INVALID_PARAM;
    }

    switch (service_icon) {
        case WM_GDC0689_SERVICE_ICON_OFF:
            seg_lcd_display[0].is_on = false;

            break;
        case WM_GDC0689_SERVICE_ICON_ON:
            seg_lcd_display[0].is_on = true;
            break;
        default:
            return WM_ERR_INVALID_PARAM;
            break;
    }

    ret = wm_drv_seg_lcd_display_seg(dev, seg_lcd_display, sizeof(seg_lcd_display) / sizeof(seg_lcd_display[0]));

    return ret;
}

int wm_gdc0689_display_battery_level(wm_device_t *dev, wm_gdc0689_battery_level_t battery_level)
{
    int ret = WM_ERR_SUCCESS;

    seg_lcd_display_t seg_lcd_display[] = {
        {
         .seg_name = "W1",
         .is_on    = false,
         },
        {
         .seg_name = "W2",
         .is_on    = false,
         },
        {
         .seg_name = "W3",
         .is_on    = false,
         },
        {
         .seg_name = "W4",
         .is_on    = false,
         },
        {
         .seg_name = "W5",
         .is_on    = false,
         }
    };

    if (dev == NULL || battery_level >= WM_GDC0689_BATTERY_LEVEL_MAX) {
        return WM_ERR_INVALID_PARAM;
    }

    switch (battery_level) {
        case WM_GDC0689_BATTERY_LEVEL_OFF:
            seg_lcd_display[0].is_on = false;
            seg_lcd_display[1].is_on = false;
            seg_lcd_display[2].is_on = false;
            seg_lcd_display[3].is_on = false;
            seg_lcd_display[4].is_on = false;
            break;
        case WM_GDC0689_BATTERY_LEVEL_PERCENT_0:
            seg_lcd_display[0].is_on = true;
            seg_lcd_display[1].is_on = false;
            seg_lcd_display[2].is_on = false;
            seg_lcd_display[3].is_on = false;
            seg_lcd_display[4].is_on = false;
            break;
        case WM_GDC0689_BATTERY_LEVEL_PERCENT_25:
            seg_lcd_display[0].is_on = true;
            seg_lcd_display[1].is_on = false;
            seg_lcd_display[2].is_on = false;
            seg_lcd_display[3].is_on = false;
            seg_lcd_display[4].is_on = true;
            break;
        case WM_GDC0689_BATTERY_LEVEL_PERCENT_50:
            seg_lcd_display[0].is_on = true;
            seg_lcd_display[1].is_on = false;
            seg_lcd_display[2].is_on = false;
            seg_lcd_display[3].is_on = true;
            seg_lcd_display[4].is_on = true;
            break;
        case WM_GDC0689_BATTERY_LEVEL_PERCENT_75:
            seg_lcd_display[0].is_on = true;
            seg_lcd_display[1].is_on = false;
            seg_lcd_display[2].is_on = true;
            seg_lcd_display[3].is_on = true;
            seg_lcd_display[4].is_on = true;
            break;
        case WM_GDC0689_BATTERY_LEVEL_PERCENT_100:
            seg_lcd_display[0].is_on = true;
            seg_lcd_display[1].is_on = true;
            seg_lcd_display[2].is_on = true;
            seg_lcd_display[3].is_on = true;
            seg_lcd_display[4].is_on = true;
            break;
        default:
            return WM_ERR_INVALID_PARAM;
            break;
    }

    ret = wm_drv_seg_lcd_display_seg(dev, seg_lcd_display, sizeof(seg_lcd_display) / sizeof(seg_lcd_display[0]));

    return ret;
}

int wm_gdc0689_display_signal_level(wm_device_t *dev, wm_gdc0689_signal_level_t signal_level)
{
    int ret = WM_ERR_SUCCESS;

    seg_lcd_display_t seg_lcd_display[] = {
        {
         .seg_name = "L1",
         .is_on    = false,
         },
        {
         .seg_name = "L2",
         .is_on    = false,
         },
        {
         .seg_name = "L3",
         .is_on    = false,
         },
        {
         .seg_name = "L4",
         .is_on    = false,
         }
    };

    if (dev == NULL || signal_level >= WM_GDC0689_SIGNAL_LEVEL_MAX) {
        return WM_ERR_INVALID_PARAM;
    }

    switch (signal_level) {
        case WM_GDC0689_SIGNAL_LEVEL_OFF:
            seg_lcd_display[0].is_on = false;
            seg_lcd_display[1].is_on = false;
            seg_lcd_display[2].is_on = false;
            seg_lcd_display[3].is_on = false;
            break;
        case WM_GDC0689_SIGNAL_LEVEL_PERCENT_25:
            seg_lcd_display[0].is_on = true;
            seg_lcd_display[1].is_on = false;
            seg_lcd_display[2].is_on = false;
            seg_lcd_display[3].is_on = false;
            break;
        case WM_GDC0689_SIGNAL_LEVEL_PERCENT_50:
            seg_lcd_display[0].is_on = true;
            seg_lcd_display[1].is_on = true;
            seg_lcd_display[2].is_on = false;
            seg_lcd_display[3].is_on = false;
            break;
        case WM_GDC0689_SIGNAL_LEVEL_PERCENT_75:
            seg_lcd_display[0].is_on = true;
            seg_lcd_display[1].is_on = true;
            seg_lcd_display[2].is_on = true;
            seg_lcd_display[3].is_on = false;
            break;
        case WM_GDC0689_SIGNAL_LEVEL_PERCENT_100:
            seg_lcd_display[0].is_on = true;
            seg_lcd_display[1].is_on = true;
            seg_lcd_display[2].is_on = true;
            seg_lcd_display[3].is_on = true;
            break;
        default:
            return WM_ERR_INVALID_PARAM;
            break;
    }

    ret = wm_drv_seg_lcd_display_seg(dev, seg_lcd_display, sizeof(seg_lcd_display) / sizeof(seg_lcd_display[0]));

    return ret;
}

int wm_gdc0689_display_unit(wm_device_t *dev, wm_gdc0689_unit_t unit)
{
    int ret = WM_ERR_SUCCESS;

    seg_lcd_display_t seg_lcd_display[] = {
        {
         .seg_name = "S1",
         .is_on    = false,
         },
        {
         .seg_name = "S2",
         .is_on    = false,
         },
        {
         .seg_name = "S3",
         .is_on    = false,
         },
        {
         .seg_name = "S4",
         .is_on    = false,
         },
        {
         .seg_name = "S5",
         .is_on    = false,
         },
        {
         .seg_name = "S6",
         .is_on    = false,
         },
        {
         .seg_name = "S7",
         .is_on    = false,
         },
        {
         .seg_name = "S8",
         .is_on    = false,
         },
        {
         .seg_name = "S9",
         .is_on    = false,
         }
    };

    if (dev == NULL || unit >= WM_GDC0689_UNIT_MAX) {
        return WM_ERR_INVALID_PARAM;
    }

    if (unit > 0) {
        seg_lcd_display[unit - 1].is_on = true;
    }

    ret = wm_drv_seg_lcd_display_seg(dev, seg_lcd_display, sizeof(seg_lcd_display) / sizeof(seg_lcd_display[0]));

    return ret;
}

int wm_gdc0689_display_time(wm_device_t *dev, int tm_hour, int tm_min)
{
    int ret                      = WM_ERR_SUCCESS;
    uint8_t number[4]            = { 0 };
    const char number_name[4][2] = { "1", "2", "3", "4" };

    if (dev == NULL || tm_hour > 23 || tm_hour < 0 || tm_min > 59 || tm_min < 0) {
        return WM_ERR_INVALID_PARAM;
    }

    number[0] = tm_hour / 10;
    number[1] = tm_hour % 10;
    number[2] = tm_min / 10;
    number[3] = tm_min % 10;

    for (uint8_t j = 0; j < (sizeof(number) / sizeof(number[0])); j++) {
        seg_lcd_display_t seg_lcd_display[7] = { 0 };

        for (uint8_t i = 0; i < (sizeof(seg_lcd_display) / sizeof(seg_lcd_display[0])); i++) {
            strcat(seg_lcd_display[i].seg_name, number_name[j]);
            strcat(seg_lcd_display[i].seg_name, g_number_seg_name[i]);
            seg_lcd_display[i].is_on = g_seg_lcd_num[number[j]] & (1 << i);
            // wm_log_info("seg_name: %s, is_on: %d", seg_lcd_display[i].seg_name, seg_lcd_display[i].is_on);
        }
        ret = wm_drv_seg_lcd_display_seg(dev, seg_lcd_display, sizeof(seg_lcd_display) / sizeof(seg_lcd_display[0]));
        if (ret != WM_ERR_SUCCESS) {
            return ret;
        }
    }

    do {
        seg_lcd_display_t seg_lcd_display[] = {
            { "COL1", true  },
            { "DP1",  false },
            { "DP2",  false },
            { "DP3",  false }
        };

        ret = wm_drv_seg_lcd_display_seg(dev, seg_lcd_display, sizeof(seg_lcd_display) / sizeof(seg_lcd_display[0]));
    } while (0);

    return ret;
}

int wm_gdc0689_display_integer(wm_device_t *dev, uint32_t value)
{
    int ret                      = WM_ERR_SUCCESS;
    uint8_t number[6]            = { 0 };
    const char number_name[6][3] = { "5", "6", "7", "8", "9", "10" };

    if (dev == NULL || value > 999999) {
        return WM_ERR_INVALID_PARAM;
    }

    number[0] = (value / 100000) % 10;
    number[1] = (value / 10000) % 10;
    number[2] = (value / 1000) % 10;
    number[3] = (value / 100) % 10;
    number[4] = (value / 10) % 10;
    number[5] = (value / 1) % 10;

    for (uint8_t j = 0; j < (sizeof(number) / sizeof(number[0])); j++) {
        seg_lcd_display_t seg_lcd_display[7] = { 0 };

        for (uint8_t i = 0; i < (sizeof(seg_lcd_display) / sizeof(seg_lcd_display[0])); i++) {
            strcat(seg_lcd_display[i].seg_name, number_name[j]);
            strcat(seg_lcd_display[i].seg_name, g_number_seg_name[i]);
            seg_lcd_display[i].is_on = g_seg_lcd_num[number[j]] & (1 << i);
            // wm_log_info("seg_name: %s, is_on: %d", seg_lcd_display[i].seg_name, seg_lcd_display[i].is_on);
        }
        ret = wm_drv_seg_lcd_display_seg(dev, seg_lcd_display, sizeof(seg_lcd_display) / sizeof(seg_lcd_display[0]));
        if (ret != WM_ERR_SUCCESS) {
            return ret;
        }
    }

    do {
        seg_lcd_display_t seg_lcd_display[] = {
            { "COL2", false },
            { "COL3", false },
            { "DP5",  false },
            { "DP6",  false },
            { "DP7",  false },
            { "DP8",  false },
            { "DP9",  false }
        };

        ret = wm_drv_seg_lcd_display_seg(dev, seg_lcd_display, sizeof(seg_lcd_display) / sizeof(seg_lcd_display[0]));
    } while (0);

    return ret;
}

int wm_gdc0689_display_decimal(wm_device_t *dev, float value)
{
    int ret                      = WM_ERR_SUCCESS;
    uint8_t number[6]            = { 0 };
    const char number_name[6][3] = { "5", "6", "7", "8", "9", "10" };
    uint32_t display_value       = (uint32_t)(value * 100);

    if (dev == NULL || display_value > 999999 || value < 0) {
        return WM_ERR_INVALID_PARAM;
    }

    number[0] = (display_value / 100000) % 10;
    number[1] = (display_value / 10000) % 10;
    number[2] = (display_value / 1000) % 10;
    number[3] = (display_value / 100) % 10;
    number[4] = (display_value / 10) % 10;
    number[5] = (display_value / 1) % 10;

    for (uint8_t j = 0; j < (sizeof(number) / sizeof(number[0])); j++) {
        seg_lcd_display_t seg_lcd_display[7] = { 0 };

        for (uint8_t i = 0; i < (sizeof(seg_lcd_display) / sizeof(seg_lcd_display[0])); i++) {
            strcat(seg_lcd_display[i].seg_name, number_name[j]);
            strcat(seg_lcd_display[i].seg_name, g_number_seg_name[i]);
            seg_lcd_display[i].is_on = g_seg_lcd_num[number[j]] & (1 << i);
            // wm_log_info("seg_name: %s, is_on: %d", seg_lcd_display[i].seg_name, seg_lcd_display[i].is_on);
        }
        ret = wm_drv_seg_lcd_display_seg(dev, seg_lcd_display, sizeof(seg_lcd_display) / sizeof(seg_lcd_display[0]));
        if (ret != WM_ERR_SUCCESS) {
            return ret;
        }
    }

    do {
        seg_lcd_display_t seg_lcd_display[] = {
            { "COL2", false },
            { "COL3", false },
            { "DP5",  false },
            { "DP6",  false },
            { "DP7",  false },
            { "DP8",  true  },
            { "DP9",  false }
        };

        ret = wm_drv_seg_lcd_display_seg(dev, seg_lcd_display, sizeof(seg_lcd_display) / sizeof(seg_lcd_display[0]));
    } while (0);

    return ret;
}

int wm_gdc0689_display_all(wm_device_t *dev)
{
    int ret = WM_ERR_SUCCESS;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    seg_lcd_display_t seg_lcd_display[sizeof(gdc0689_seg_lcd_table) / sizeof(gdc0689_seg_lcd_table[0])] = { 0 };

    for (uint32_t i = 0; i < (sizeof(gdc0689_seg_lcd_table) / sizeof(gdc0689_seg_lcd_table[0])); i++) {
        strcpy(seg_lcd_display[i].seg_name, gdc0689_seg_lcd_table[i].seg_name);
        seg_lcd_display[i].is_on = true;
    }

    ret = wm_drv_seg_lcd_display_seg(dev, seg_lcd_display, sizeof(seg_lcd_display) / sizeof(seg_lcd_display[0]));

    return ret;
}

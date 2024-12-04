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
#include "wm_drv_gpio.h"
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
 * @brief Defines a mapping between segment names and their corresponding hardware identifiers.
 *
 * This structure is used to relate the logical names of segments to their
 * actual hardware configuration identifiers, including the COM address and the
 * segment bit representation.
 */
typedef struct {
    char seg_name[5];        /**< The logical name of the segment */
    wm_seg_lcd_com_id_t com; /**< The hardware COM identifier for the segment */
    uint32_t seg;            /**< The hardware segment bit representation */
} seg_lcd_table_t;

/**
 * @brief Represents the display state of a segment on the LCD.
 *
 * This structure is used to define whether a particular segment should be turned on
 * or off for display purposes.
 */
typedef struct {
    char seg_name[5]; /**< The logical name of the segment */
    bool is_on;       /**< Boolean indicating whether the segment is on or off */
} seg_lcd_display_t;

/**
 * @brief Defines the mapping of segment names to their respective COM and bit mask.
 *
 * This table is used to associate each segment with a specific COM line and bit
 * mask for easy manipulation of the segments on the LCD.
 */
static const seg_lcd_table_t g_seg_lcd_table[] = {
    // COM0
    { "1D",   0, (1 << 1)  },
    { "DP1",  0, (1 << 2)  },
    { "2D",   0, (1 << 3)  },
    { "DP2",  0, (1 << 4)  },
    { "3D",   0, (1 << 5)  },
    { "DP3",  0, (1 << 6)  },
    { "4D",   0, (1 << 7)  },
    { "COL1", 0, (1 << 8)  },
    { "COL2", 0, (1 << 9)  },
    { "W5",   0, (1 << 10) },
    { "L1",   0, (1 << 11) },
    { "5F",   0, (1 << 12) },
    { "5A",   0, (1 << 13) },
    { "6F",   0, (1 << 14) },
    { "6A",   0, (1 << 15) },
    { "7F",   0, (1 << 16) },
    { "7A",   0, (1 << 17) },
    { "S4",   0, (1 << 18) },
    { "S5",   0, (1 << 19) },
    { "8F",   0, (1 << 20) },
    { "8A",   0, (1 << 21) },
    { "9F",   0, (1 << 22) },
    { "9A",   0, (1 << 23) },
    { "10F",  0, (1 << 24) },
    { "10A",  0, (1 << 25) },
    // COM1
    { "1E",   1, (1 << 1)  },
    { "1C",   1, (1 << 2)  },
    { "2E",   1, (1 << 3)  },
    { "2C",   1, (1 << 4)  },
    { "3E",   1, (1 << 5)  },
    { "3C",   1, (1 << 6)  },
    { "4E",   1, (1 << 7)  },
    { "4C",   1, (1 << 8)  },
    { "COL3", 1, (1 << 9)  },
    { "W4",   1, (1 << 10) },
    { "L2",   1, (1 << 11) },
    { "5G",   1, (1 << 12) },
    { "5B",   1, (1 << 13) },
    { "6G",   1, (1 << 14) },
    { "6B",   1, (1 << 15) },
    { "7G",   1, (1 << 16) },
    { "7B",   1, (1 << 17) },
    { "S3",   1, (1 << 18) },
    { "S6",   1, (1 << 19) },
    { "8G",   1, (1 << 20) },
    { "8B",   1, (1 << 21) },
    { "9G",   1, (1 << 22) },
    { "9B",   1, (1 << 23) },
    { "10G",  1, (1 << 24) },
    { "10B",  1, (1 << 25) },
    // COM2
    { "1G",   2, (1 << 1)  },
    { "1B",   2, (1 << 2)  },
    { "2G",   2, (1 << 3)  },
    { "2B",   2, (1 << 4)  },
    { "3G",   2, (1 << 5)  },
    { "3B",   2, (1 << 6)  },
    { "4G",   2, (1 << 7)  },
    { "4B",   2, (1 << 8)  },
    { "T1",   2, (1 << 9)  },
    { "W3",   2, (1 << 10) },
    { "L3",   2, (1 << 11) },
    { "5E",   2, (1 << 12) },
    { "5C",   2, (1 << 13) },
    { "6E",   2, (1 << 14) },
    { "6C",   2, (1 << 15) },
    { "7E",   2, (1 << 16) },
    { "7C",   2, (1 << 17) },
    { "S2",   2, (1 << 18) },
    { "S7",   2, (1 << 19) },
    { "8E",   2, (1 << 20) },
    { "8C",   2, (1 << 21) },
    { "9E",   2, (1 << 22) },
    { "9C",   2, (1 << 23) },
    { "10E",  2, (1 << 24) },
    { "10C",  2, (1 << 25) },
    // COM3
    { "1F",   3, (1 << 1)  },
    { "1A",   3, (1 << 2)  },
    { "2F",   3, (1 << 3)  },
    { "2A",   3, (1 << 4)  },
    { "3F",   3, (1 << 5)  },
    { "3A",   3, (1 << 6)  },
    { "4F",   3, (1 << 7)  },
    { "4A",   3, (1 << 8)  },
    { "W1",   3, (1 << 9)  },
    { "W2",   3, (1 << 10) },
    { "L4",   3, (1 << 11) },
    { "5D",   3, (1 << 12) },
    { "DP5",  3, (1 << 13) },
    { "6D",   3, (1 << 14) },
    { "DP6",  3, (1 << 15) },
    { "7D",   3, (1 << 16) },
    { "DP7",  3, (1 << 17) },
    { "S1",   3, (1 << 18) },
    { "S8",   3, (1 << 19) },
    { "8D",   3, (1 << 20) },
    { "DP8",  3, (1 << 21) },
    { "9D",   3, (1 << 22) },
    { "DP9",  3, (1 << 23) },
    { "10D",  3, (1 << 24) },
    { "S9",   3, (1 << 25) }
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

static int wm_gdc0689_display(wm_device_t *dev, seg_lcd_display_t *seg_lcd_display, uint32_t seg_num)
{
    int ret                        = WM_ERR_SUCCESS;
    wm_drv_gdc0689_drv_t *drv_data = NULL;
    uint32_t seg_val               = 0;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    drv_data = (wm_drv_gdc0689_drv_t *)dev->drv;
    if (drv_data == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    for (uint32_t i = 0; i < seg_num; i++) {
        for (uint32_t j = 0; j < (sizeof(g_seg_lcd_table) / sizeof(g_seg_lcd_table[0])); j++) {
            if (!(strcmp(seg_lcd_display[i].seg_name, g_seg_lcd_table[j].seg_name))) {
                seg_val = (seg_lcd_display[i].is_on) ? g_seg_lcd_table[j].seg : 0;
                ret     = wm_drv_seg_lcd_display(drv_data->drv_ctx.seg_lcd_dev, g_seg_lcd_table[j].com, g_seg_lcd_table[j].seg,
                                                 seg_val);
                if (ret != WM_ERR_SUCCESS) {
                    return ret;
                }
            }
        }
    }

    return ret;
}

wm_device_t *wm_gdc0689_init(char *dev_name)
{
    int ret                        = WM_ERR_SUCCESS;
    wm_device_t *gdc0689_dev       = NULL;
    wm_device_t *seg_lcd_dev       = NULL;
    wm_dt_hw_gdc0689_t *hw_gdc0689 = NULL;
    wm_drv_gdc0689_drv_t *drv_data = NULL;

    gdc0689_dev = wm_dt_get_device_by_name(dev_name);
    if (!gdc0689_dev) {
        return NULL;
    }

    hw_gdc0689 = (wm_dt_hw_gdc0689_t *)gdc0689_dev->hw;
    for (uint8_t i = 0; i < hw_gdc0689->pin_cfg_count; i++) {
        ret = wm_drv_gpio_iomux_func_sel(hw_gdc0689->pin_cfg[i].pin_num, hw_gdc0689->pin_cfg[i].pin_mux);
        if (ret != WM_ERR_SUCCESS) {
            return NULL;
        }
    }
    seg_lcd_dev = wm_drv_seg_lcd_init(hw_gdc0689->seg_lcd_device_name);

    if (seg_lcd_dev) {
        drv_data = (wm_drv_gdc0689_drv_t *)wm_os_internal_malloc(sizeof(wm_drv_gdc0689_drv_t));
        if (!drv_data) {
            return NULL;
        }
        memset(drv_data, 0x00, sizeof(wm_drv_gdc0689_drv_t));
        drv_data->drv_ctx.seg_lcd_dev = seg_lcd_dev;
        gdc0689_dev->drv              = drv_data;
    }
    gdc0689_dev->state = WM_DEV_ST_INITED;

    return gdc0689_dev;
}

int wm_gdc0689_deinit(wm_device_t *dev)
{
    int ret                        = WM_ERR_SUCCESS;
    wm_drv_gdc0689_drv_t *drv_data = NULL;
    wm_dt_hw_gdc0689_t *hw_gdc0689 = NULL;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    drv_data   = (wm_drv_gdc0689_drv_t *)dev->drv;
    hw_gdc0689 = (wm_dt_hw_gdc0689_t *)dev->hw;
    if (drv_data == NULL || hw_gdc0689 == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    ret = wm_drv_seg_lcd_deinit(drv_data->drv_ctx.seg_lcd_dev);
    if (ret != WM_ERR_SUCCESS) {
        return ret;
    }

    for (uint8_t i = 0; i < hw_gdc0689->pin_cfg_count; i++) {
        ret = wm_drv_gpio_iomux_func_sel(hw_gdc0689->pin_cfg[i].pin_num, WM_GPIO_IOMUX_FUN5);
        if (ret != WM_ERR_SUCCESS) {
            return ret;
        }
    }

    if (dev->drv != NULL) {
        wm_os_internal_free(dev->drv);
        dev->drv = NULL;
    }
    dev->state = WM_DEV_ST_UNINIT;

    return ret;
}

int wm_gdc0689_clear(wm_device_t *dev)
{
    int ret                        = WM_ERR_SUCCESS;
    wm_drv_gdc0689_drv_t *drv_data = NULL;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    drv_data = (wm_drv_gdc0689_drv_t *)dev->drv;
    if (drv_data == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    ret = wm_drv_seg_lcd_clear(drv_data->drv_ctx.seg_lcd_dev);

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

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
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

    ret = wm_gdc0689_display(dev, seg_lcd_display, sizeof(seg_lcd_display) / sizeof(seg_lcd_display[0]));

    return ret;
}

int wm_gdc0689_display_battery_level(wm_device_t *dev, wm_gdc0689_battery_level_t battery_level)
{
    int ret                             = WM_ERR_SUCCESS;
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

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
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

    ret = wm_gdc0689_display(dev, seg_lcd_display, sizeof(seg_lcd_display) / sizeof(seg_lcd_display[0]));

    return ret;
}

int wm_gdc0689_display_signal_level(wm_device_t *dev, wm_gdc0689_signal_level_t signal_level)
{
    int ret                             = WM_ERR_SUCCESS;
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

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
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

    ret = wm_gdc0689_display(dev, seg_lcd_display, sizeof(seg_lcd_display) / sizeof(seg_lcd_display[0]));

    return ret;
}

int wm_gdc0689_display_unit(wm_device_t *dev, wm_gdc0689_unit_t unit)
{
    int ret                             = WM_ERR_SUCCESS;
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

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    if (unit > 0) {
        seg_lcd_display[unit - 1].is_on = true;
    }

    ret = wm_gdc0689_display(dev, seg_lcd_display, sizeof(seg_lcd_display) / sizeof(seg_lcd_display[0]));

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

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
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
        ret = wm_gdc0689_display(dev, seg_lcd_display, sizeof(seg_lcd_display) / sizeof(seg_lcd_display[0]));
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

        ret = wm_gdc0689_display(dev, seg_lcd_display, sizeof(seg_lcd_display) / sizeof(seg_lcd_display[0]));
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

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
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
        ret = wm_gdc0689_display(dev, seg_lcd_display, sizeof(seg_lcd_display) / sizeof(seg_lcd_display[0]));
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

        ret = wm_gdc0689_display(dev, seg_lcd_display, sizeof(seg_lcd_display) / sizeof(seg_lcd_display[0]));
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

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
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
        ret = wm_gdc0689_display(dev, seg_lcd_display, sizeof(seg_lcd_display) / sizeof(seg_lcd_display[0]));
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

        ret = wm_gdc0689_display(dev, seg_lcd_display, sizeof(seg_lcd_display) / sizeof(seg_lcd_display[0]));
    } while (0);

    return ret;
}

int wm_gdc0689_display_all(wm_device_t *dev)
{
    int ret = WM_ERR_SUCCESS;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    seg_lcd_display_t seg_lcd_display[sizeof(g_seg_lcd_table) / sizeof(g_seg_lcd_table[0])] = { 0 };

    for (uint32_t i = 0; i < (sizeof(g_seg_lcd_table) / sizeof(g_seg_lcd_table[0])); i++) {
        strcpy(seg_lcd_display[i].seg_name, g_seg_lcd_table[i].seg_name);
        seg_lcd_display[i].is_on = true;
    }

    ret = wm_gdc0689_display(dev, seg_lcd_display, sizeof(seg_lcd_display) / sizeof(seg_lcd_display[0]));

    return ret;
}

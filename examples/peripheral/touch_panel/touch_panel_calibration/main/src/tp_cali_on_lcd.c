/**
 * @file tp_cali_on_lcd.c
 *
 * @brief LCD screen display function
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
#include "tp_cali_on_lcd.h"

#include "wm_log.h"

static wm_device_t *lcd_dev = NULL;
static wm_device_t *spi_dev = NULL;

int example_lcd_init(void)
{
    spi_dev = wm_drv_sdh_spi_init(LCD_SPI_CONTROLLER_DEVICE_NAME);
    if (!spi_dev) {
        wm_log_error("SPI init failed");
        return WM_ERR_FAILED;
    }

    lcd_dev = wm_drv_tft_lcd_init(LCD_DEVICE_NAME);
    if (!lcd_dev) {
        wm_log_error("LCD init failed");
        return WM_ERR_FAILED;
    }

    if (wm_drv_tft_lcd_set_backlight(lcd_dev, true) != WM_ERR_SUCCESS) {
        wm_log_error("Failed to set backlight");
        return WM_ERR_FAILED;
    }

    return WM_ERR_SUCCESS;
}

void example_lcd_deinit(void)
{
    if (lcd_dev) {
        wm_drv_tft_lcd_set_backlight(lcd_dev, false);
        wm_drv_tft_lcd_deinit(lcd_dev);
        lcd_dev = NULL;
    }
    if (spi_dev) {
        wm_drv_sdh_spi_deinit(spi_dev);
        spi_dev = NULL;
    }
}

void example_lcd_clear(uint16_t color)
{
    if (!lcd_dev) {
        wm_log_error("LCD not initialized");
        return;
    }

    wm_lcd_data_desc_t data_desc = { 0 };
    wm_lcd_capabilitys_t cap     = { 0 };
    wm_drv_tft_lcd_get_capability(lcd_dev, &cap);

    uint32_t buf_size = LCD_DATA_DRAW_LINE_UNIT * cap.x_resolution * 2;
    uint8_t *buf      = malloc(buf_size);
    if (!buf) {
        wm_log_error("Failed to allocate buffer");
        return;
    }

    for (int i = 0; i < buf_size; i += 2) {
        buf[i]     = (uint8_t)(color >> 8);
        buf[i + 1] = (uint8_t)(color & 0xFF);
    }

    for (int y = 0; y < cap.y_resolution;) {
        data_desc.x_start = 0;
        data_desc.x_end   = cap.x_resolution - 1;
        data_desc.y_start = y;
        data_desc.y_end =
            (y + LCD_DATA_DRAW_LINE_UNIT > cap.y_resolution) ? cap.y_resolution - 1 : y + LCD_DATA_DRAW_LINE_UNIT - 1;

        data_desc.buf      = buf;
        data_desc.buf_size = (data_desc.y_end - y + 1) * cap.x_resolution * 2;

        if (wm_drv_tft_lcd_draw_bitmap(lcd_dev, data_desc) != WM_ERR_SUCCESS) {
            wm_log_error("Draw bitmap failed at y=%d", y);
        }
        y = data_desc.y_end + 1;
    }
    free(buf);
}

void example_lcd_draw_cross(uint16_t x, uint16_t y, uint16_t size, uint16_t color)
{
    if (!lcd_dev) {
        wm_log_error("LCD not initialized");
        return;
    }

    wm_lcd_data_desc_t data_desc = { 0 };
    wm_lcd_capabilitys_t cap     = { 0 };
    if (wm_drv_tft_lcd_get_capability(lcd_dev, &cap) != WM_ERR_SUCCESS) {
        wm_log_error("Failed to get LCD capability");
        return;
    }

    uint16_t max_length = (2 * size + 1) > cap.x_resolution ? cap.x_resolution : (2 * size + 1);
    uint8_t *buf        = malloc(max_length * 2);
    if (!buf) {
        wm_log_error("Failed to allocate buffer");
        return;
    }

    for (int i = 0; i < max_length * 2; i += 2) {
        buf[i]     = (uint8_t)(color >> 8);
        buf[i + 1] = (uint8_t)(color & 0xFF);
    }

    // draw a horizontal line
    data_desc.x_start  = (x >= size) ? (x - size) : 0;
    data_desc.x_end    = (x + size < cap.x_resolution) ? (x + size) : (cap.x_resolution - 1);
    data_desc.y_start  = y;
    data_desc.y_end    = y;
    data_desc.buf      = buf;
    data_desc.buf_size = (data_desc.x_end - data_desc.x_start + 1) * 2;
    wm_drv_tft_lcd_draw_bitmap(lcd_dev, data_desc);

    // draw a vertical line
    data_desc.x_start  = x;
    data_desc.x_end    = x;
    data_desc.y_start  = (y >= size) ? (y - size) : 0;
    data_desc.y_end    = (y + size < cap.y_resolution) ? (y + size) : (cap.y_resolution - 1);
    data_desc.buf      = buf;
    data_desc.buf_size = (data_desc.y_end - data_desc.y_start + 1) * 2;
    wm_drv_tft_lcd_draw_bitmap(lcd_dev, data_desc);

    free(buf);
}
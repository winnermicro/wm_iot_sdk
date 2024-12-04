/**
 * @file main.c
 *
 * @brief TFT LCD demo main
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
#include "wmsdk_config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "wm_drv_tft_lcd.h"
#include "wm_drv_sdh_spi.h"
#include "picture.h"
#include "wm_utils.h"

#define LOG_TAG "lcd_example"
#include "wm_log.h"

#define WM_LCD_TFT_DEMO_TASK_STACK     (2048)
#define WM_LCD_TFT_DEMO_TASK_PRIO      (configMAX_PRIORITIES - 8)
#define WM_LCD_TFT_REFRESH_DELAY_MS    (500)

#define LCD_RGB565_BLACK               0x0000
#define LCD_RGB565_BLUE                0x001F
#define LCD_RGB565_RED                 0xF800
#define LCD_RGB565_GREEN               0x07E0
#define LCD_RGB565_CYAN                0x07FF
#define LCD_RGB565_MAGENTA             0xF81F
#define LCD_RGB565_YELLOW              0xFFE0
#define LCD_RGB565_WHITE               0xFFFF

/* the controller device name which used by LCD, it must same as the device name defined in device table*/
#define LCD_SPI_CONTROLLER_DEVICE_NAME "sdspi"

/* lcd device name which must same as the device name defined in device table*/
#define LCD_DEVICE_NAME                WM_CFG_TFT_LCD_DEVICE_NAME

/* divide the image as many blocks, allocate one application buffer to send them one by one
 * this is in order to use less memory for the screen refresh*/
#define LCD_DATA_DRAW_LINE_UNIT        (40)

/* demo semaphore that used to control the timing of next buffer filling*/
static SemaphoreHandle_t lcd_demo_sem = NULL;

typedef struct {
    const uint8_t *image_buf;
    uint16_t image_width;
    uint16_t image_high;
    uint16_t image_size;
} image_attr_t;

void lcd_demo_tx_cb(int result, void *data)
{
    if (result != WM_ERR_SUCCESS) {
        wm_log_info("lcd_demo_tx_cb fail");
    }
    if (lcd_demo_sem != NULL) {
        xSemaphoreGive(lcd_demo_sem);
    }
}

static int lcd_demo_clean_screen_with_dma(wm_device_t *dev, uint8_t *buf, uint32_t buf_len, uint16_t bk_color)
{
    wm_lcd_data_desc_t data_desc = { 0 };
    uint16_t width               = 0;
    uint16_t high                = 0;
    int ret                      = WM_ERR_FAILED;
    wm_lcd_capabilitys_t cap     = { 0 };

    wm_drv_tft_lcd_get_capability(dev, &cap);

    width = cap.x_resolution;
    high  = cap.y_resolution;

    for (int x = 0; x < buf_len; x += 2) {
        buf[x]     = (uint8_t)(bk_color >> 8);
        buf[x + 1] = (uint8_t)(bk_color & 0x00FF);
    }

    /* The maximum number of lines for each drawing is LCD_DATA_DRAW_LINE_UNIT */
    for (int i = 0; i < high;) {
        data_desc.x_start  = 0;
        data_desc.x_end    = width - 1;
        data_desc.y_start  = i;
        data_desc.y_end    = (i + LCD_DATA_DRAW_LINE_UNIT > high) ? (high - 1) : (i + LCD_DATA_DRAW_LINE_UNIT - 1);
        data_desc.buf      = buf;
        data_desc.buf_size = (data_desc.y_end - i + 1) * width * WM_CFG_TFT_LCD_PIXEL_WIDTH;

        xSemaphoreTake(lcd_demo_sem, pdMS_TO_TICKS(1000));

        ret = wm_drv_tft_lcd_draw_bitmap(dev, data_desc);
        if (ret != WM_ERR_SUCCESS) {
            wm_log_info("wm_lcd_demo_clean_screen_with_dma ret=%d", ret);
        }

        i = data_desc.y_end + 1;
    }

    return ret;
}

static int lcd_demo_show_image_with_dma(wm_device_t *dev, uint8_t *buf, uint32_t buf_len, image_attr_t img)
{
    wm_lcd_data_desc_t data_desc = { 0 };
    uint16_t width               = 0;
    uint16_t high                = 0;
    int ret                      = WM_ERR_FAILED;

    width = img.image_width;
    high  = img.image_high;

    /* The maximum number of lines for each drawing is LCD_DATA_DRAW_LINE_UNIT */
    for (int i = 0; i < high;) {
        data_desc.x_start  = 0;
        data_desc.x_end    = width - 1;
        data_desc.y_start  = i;
        data_desc.y_end    = (i + LCD_DATA_DRAW_LINE_UNIT > high) ? (high - 1) : (i + LCD_DATA_DRAW_LINE_UNIT - 1);
        data_desc.buf      = buf;
        data_desc.buf_size = (data_desc.y_end - i + 1) * width * WM_CFG_TFT_LCD_PIXEL_WIDTH;

        xSemaphoreTake(lcd_demo_sem, pdMS_TO_TICKS(1000));

        /*in case DMA cannot access the flash, move the image content to SRAM before sending.*/
        memcpy(data_desc.buf, img.image_buf + (i * width * WM_CFG_TFT_LCD_PIXEL_WIDTH), data_desc.buf_size);
        wm_log_info("data_desc.buf=%p, size=%d", data_desc.buf, data_desc.buf_size);

        ret = wm_drv_tft_lcd_draw_bitmap(dev, data_desc);
        if (ret != WM_ERR_SUCCESS) {
            wm_log_info("wm_lcd_demo_show_image ret=%d", ret);
        }

        i = data_desc.y_end + 1;
    }

    return ret;
}

static void wm_lcd_demo(void *arg)
{
    int ret = WM_ERR_FAILED;
    int id  = 0;

    wm_device_t *dev    = NULL;
    image_attr_t img    = { 0 };
    uint8_t *app_buf    = NULL;
    uint32_t block_size = 0;
    uint16_t width = 0, high = 0;
    wm_lcd_capabilitys_t cap = { 0 };

    lcd_demo_sem = xSemaphoreCreateCounting(1, 1);

    /*TODO: add more initial methods for other interfaces(like RGB/MIPI...) in future on new chip*/
    /*use sdio(spi mode) for lcd demo as it's support high speed */
    dev = wm_drv_sdh_spi_init(LCD_SPI_CONTROLLER_DEVICE_NAME);
    if (dev == NULL) {
        wm_log_error("init sdspi fail.");
    }

    /* initial the lcd device, and use the same device name which defined in device table*/
    dev = wm_drv_tft_lcd_init(LCD_DEVICE_NAME);
    if (dev == NULL) {
        wm_log_info("init lcd fail.");
    }

    /* turn on the backlight*/
    ret = wm_drv_tft_lcd_set_backlight(dev, true);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_info("lcd bl set fail.");
    }

    /* show LCD capability */
    wm_drv_tft_lcd_get_capability(dev, &cap);
    wm_log_info("LCD x_resolution = %d", cap.x_resolution);
    wm_log_info("LCD y_resolution = %d", cap.y_resolution);
    wm_log_info("LCD rotation = %d\n", cap.rotation);

    //NOTE: when color mode change , the byte width could be adjusted too.
    /* malloc an application buffer to refresh the screen*/
    width = cap.x_resolution;
    high  = cap.y_resolution;

    block_size = (LCD_DATA_DRAW_LINE_UNIT * width * WM_CFG_TFT_LCD_PIXEL_WIDTH);
    wm_log_info("DEMO:block_size=%d", block_size);

    app_buf = malloc(block_size);
    if (app_buf == NULL) {
        wm_log_error("mem err\n");
    }

    /* set image and the image width, height depend on selected LCD device */
#if CONFIG_COMPONENT_DRIVER_LCD_NV3041A_SPI
    img.image_buf   = image_bluesky_480x272;
    img.image_width = 480;
    img.image_high  = 272;
#else
    img.image_buf   = gImage_pic_winner_micro_logo_93x93;
    img.image_width = 93;
    img.image_high  = 93;
#endif

    if (img.image_width > width || img.image_high > high) {
        wm_log_error("image unmatch");
    }

    /* Registers a callback function that gets triggered after the transmission of a bitmap is complete */
    ret = wm_drv_tft_lcd_register_tx_callback(dev, lcd_demo_tx_cb, NULL);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_info("register tx callback error(%d)", ret);
    }

    while (1) {
        /* demo scenario 1 - show blue screen by dma*/
        wm_log_info("wm_lcd_demo show blue background");

        ret = lcd_demo_clean_screen_with_dma(dev, app_buf, block_size, LCD_RGB565_BLUE);
        if (ret != WM_ERR_SUCCESS) {
            wm_log_info("wm_lcd_demo_show_image ret=%d", ret);
        }
        vTaskDelay(pdMS_TO_TICKS(2000));

        /* demo scenario 2 - rotation the image once lcd_demo_show_image() be invoked*/
        if (id++ % 2) {
            ret = wm_drv_tft_lcd_set_rotation(dev, LCD_ROTATION_NONE);
        } else {
            ret = wm_drv_tft_lcd_set_rotation(dev, LCD_ROTATION_180_DEGREE);
        }

        /* demo scenario 3 - show image by dma*/
        wm_log_info("wm_lcd_demo show image(w=%d, h=%d)", img.image_width, img.image_high);

        ret = lcd_demo_show_image_with_dma(dev, app_buf, block_size, img);
        if (ret != WM_ERR_SUCCESS) {
            wm_log_info("wm_lcd_demo_show_image ret=%d", ret);
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
    }

    free(app_buf);

    vSemaphoreDelete((QueueHandle_t)lcd_demo_sem);

    vTaskDelete(NULL);
}

int main(void)
{
    xTaskCreate(wm_lcd_demo, "wm_lcd_demo_task", WM_LCD_TFT_DEMO_TASK_STACK, NULL, WM_LCD_TFT_DEMO_TASK_PRIO, NULL);

    return 0;
}

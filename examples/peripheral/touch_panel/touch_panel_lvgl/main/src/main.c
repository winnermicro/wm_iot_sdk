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

#include "wm_drv_gpio.h"
#include "wm_drv_tft_lcd.h"
#include "wm_drv_sdh_spi.h"
#include "wm_utils.h"

#include "wm_drv_timer.h"
#include "lvgl.h"
#include "wm_lv_port_disp.h"

#include "wm_lv_port_indev.h"

#include "wm_drv_touch_panel.h"
#include "wm_drv_touch_panel_cfg.h"

#include "lvgl_user_config.h"
#include "wm_drv_i2c.h"

#define LOG_TAG "touch_panel_example"
#include "wm_log.h"

#define WM_LVGL_TASK_STACK             (1024)
#define WM_LVGL_TASK_PRIO              (configMAX_PRIORITIES - 8)

/* the controller device name which used by LCD, it must same as the device name defined in device table*/
#define LCD_SPI_CONTROLLER_DEVICE_NAME "sdspi"

/* LVGL tick period uint */
#define LV_TICK_PERIOD_MS              (1)

#define TOUCH_PANEL_DEVICE_NAME        WM_CFG_TOUCH_PANEL_NAME

#define LABLE_X_DISPLAY_POS            20
#define LABLE_Y_DISPLAY_POS            50
#define COORD_DISPLAY_OFF_LABLE        40

/* Hardware Peripheral Driver Inititalization */
void app_lcd_init(void)
{
    int ret                  = WM_ERR_FAILED;
    wm_device_t *dev         = NULL;
    wm_lcd_capabilitys_t cap = { 0 };

    /* init gpio driver */
    dev = wm_dt_get_device_by_name("gpio");
    if ((!dev) || (dev->state != WM_DEV_ST_INITED)) {
        dev = wm_drv_gpio_init("gpio");
    }
    if (!dev || dev->state != WM_DEV_ST_INITED) {
        wm_log_error("init gpio fail.");
    }

    /*TODO: add more initial methods for other interfaces(like RGB/MIPI...) in future on new chip*/
    /*use sdio(spi mode) for lcd demo as it's support high speed */
    dev = wm_drv_sdh_spi_init(LCD_SPI_CONTROLLER_DEVICE_NAME);
    if (dev == NULL) {
        wm_log_error("init sdspi fail.");
    }

    /* initial the lcd device, and use the same device name which defined in device table*/
    dev = wm_drv_tft_lcd_init(WM_LVGL_LCD_MODULE_NAME);
    if (dev == NULL) {
        wm_log_error("init lcd fail.");
    }

    /* turn on the backlight*/
    ret = wm_drv_tft_lcd_set_backlight(dev, true);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("lcd bl set fail.");
    }

    /* show LCD capability */
    wm_drv_tft_lcd_get_capability(dev, &cap);
    wm_log_info("LCD x_resolution = %d", cap.x_resolution);
    wm_log_info("LCD y_resolution = %d", cap.y_resolution);
    wm_log_info("LCD rotation = %d\n", cap.rotation);
}

/* LVGL Timer Porting */
static void wm_lv_tick_timer_irq(void *arg)
{
    lv_tick_inc(LV_TICK_PERIOD_MS);
}

void app_lv_create_tick(void)
{
    int err                      = 0;
    wm_device_t *timer0_dev      = NULL;
    wm_drv_timer_cfg_t timer_cfg = { 0 };

    if (NULL == (timer0_dev = wm_drv_timer_init("timer0"))) {
        wm_log_error("timer init failed");
    }

    if (WM_ERR_SUCCESS != (err = wm_drv_timer_register_callback(timer0_dev, wm_lv_tick_timer_irq, timer0_dev))) {
        wm_log_error("timer register err");
    }

    timer_cfg.unit        = WM_HAL_TIMER_UNIT_MS;
    timer_cfg.auto_reload = true;
    timer_cfg.period      = LV_TICK_PERIOD_MS;
    wm_drv_timer_start(timer0_dev, timer_cfg);
}

#if LV_USE_LOG
void wm_lv_log_cb(const char *buf)
{
    wm_log_info("%s", buf);
}
#endif

/* Create minimal UI with LVGL native APIs */
static void create_ui(lv_obj_t **label_x_coord, lv_obj_t **label_y_coord)
{
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_white(), 0);

    lv_obj_t *x_label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(x_label, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(x_label, lv_color_black(), 0);
    lv_obj_align(x_label, LV_ALIGN_TOP_LEFT, LABLE_X_DISPLAY_POS, LABLE_X_DISPLAY_POS);
    lv_label_set_text(x_label, "X:");
    lv_obj_t *y_label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(y_label, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(y_label, lv_color_black(), 0);
    lv_obj_align(y_label, LV_ALIGN_TOP_LEFT, LABLE_X_DISPLAY_POS, LABLE_Y_DISPLAY_POS);
    lv_label_set_text(y_label, "Y:");

    *label_x_coord = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(*label_x_coord, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(*label_x_coord, lv_color_black(), 0);
    lv_obj_align(*label_x_coord, LV_ALIGN_TOP_LEFT, LABLE_X_DISPLAY_POS + COORD_DISPLAY_OFF_LABLE, LABLE_X_DISPLAY_POS);
    lv_label_set_text(*label_x_coord, "0");

    *label_y_coord = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(*label_y_coord, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(*label_y_coord, lv_color_black(), 0);
    lv_obj_align(*label_y_coord, LV_ALIGN_TOP_LEFT, LABLE_X_DISPLAY_POS + COORD_DISPLAY_OFF_LABLE, LABLE_Y_DISPLAY_POS);
    lv_label_set_text(*label_y_coord, "0");
}

static void wm_lv_task_entry(void *arg)
{
    char str[6];
    lv_indev_data_t last_data;
    static int point_x = 0;
    static int point_y = 0;
    lv_obj_t *label_x_coord, *label_y_coord;

    wm_device_t *dev;
    wm_drv_touch_panel_set_cfg_t tp_scfg;
    wm_drv_touch_panel_get_cfg_t tp_gcfg;
    lv_touchpad_screen_conv_t touchpad_screen_conv;

    app_lcd_init();

    //TODO: for Touch panel calibration without LVGL module.

#if LV_USE_LOG
    lv_log_register_print_cb(wm_lv_log_cb);
#endif

    lv_init();

    app_lv_create_tick();

    lv_port_disp_init();

    // initialize touch panel
    tp_scfg.rotation = 0;     // Used to set the origin position of coordinates
    tp_scfg.cal_done = false; // enable calibration

    dev = wm_drv_touch_panel_init(TOUCH_PANEL_DEVICE_NAME);

    // touch panel Touch Driver Chip Parameter Configuration
    if (wm_drv_touch_panel_set_config(dev, &tp_scfg) != WM_ERR_SUCCESS) {
        wm_log_error("touch panel parameter configuration failed");
    }

    // Retrieve width and height from the device table
    if (wm_drv_touch_panel_get_config(dev, &tp_gcfg) != WM_ERR_SUCCESS) {
        wm_log_error("touch panel parameter configuration failed");
    }

    touchpad_screen_conv.touch_dev = dev;
    touchpad_screen_conv.x_ratio   = (MY_DISP_HOR_RES - 1) / (float)tp_gcfg.width;
    touchpad_screen_conv.y_ratio   = (MY_DISP_VER_RES - 1) / (float)tp_gcfg.height;

    lv_port_indev_init(touchpad_screen_conv);

    create_ui(&label_x_coord, &label_y_coord);

    lv_indev_t *touch_indev = lv_indev_get_next(NULL);
    while (touch_indev && lv_indev_get_type(touch_indev) != LV_INDEV_TYPE_POINTER) {
        touch_indev = lv_indev_get_next(touch_indev);
    }

    while (1) {
        // Loop printing real-time coordinate points
        lv_indev_get_point(touch_indev, &last_data.point);
        if (touch_indev && (point_x != last_data.point.x || point_y != last_data.point.y)) {
            point_x = last_data.point.x;
            point_y = last_data.point.y;

            snprintf(str, sizeof(str), "%d", last_data.point.x);
            lv_label_set_text(label_x_coord, str);
            snprintf(str, sizeof(str), "%d", last_data.point.y);
            lv_label_set_text(label_y_coord, str);
            wm_log_info("Last touch: x=%d, y=%d", last_data.point.x, last_data.point.y);
        }

        lv_task_handler();
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    /* deinit once loop break */
    vTaskDelete(NULL);
}

int main(void)
{
    xTaskCreate(wm_lv_task_entry, "wm_lv_task", WM_LVGL_TASK_STACK, NULL, WM_LVGL_TASK_PRIO, NULL);

    return 0;
}

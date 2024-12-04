/**
 * @file wm_lv_port_disp.c
 *
 * @brief LVGL Porting Layer of Display Module
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

/*********************
 *      INCLUDES
 *********************/
#include "wm_lv_port_disp.h"
#include <stdbool.h>

#include "wm_error.h"
#include "wm_drv_tft_lcd.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#define LOG_TAG "lvgl_port"
#include "wm_log.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    wm_device_t *dev;
    SemaphoreHandle_t lvgl_sem; /*semaphore to control the timing of next buffer filling*/
} wm_lvgl_ctx_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);

/**********************
 *  STATIC VARIABLES
 **********************/
static wm_lvgl_ctx_t wm_lvgl_ctx = { 0 };

#define LVGL_PORT_BUFF_SIZE (MY_DISP_HOR_RES * MY_DISP_VER_RES / 8) // 1/8 screen resolution
static lv_color_t lvgl_draw_buff1[LVGL_PORT_BUFF_SIZE];
static lv_color_t lvgl_draw_buff2[LVGL_PORT_BUFF_SIZE];

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void wm_lvgl_tx_cb(int result, void *data)
{
    if (result != WM_ERR_SUCCESS) {
        wm_log_info("lcd_demo_tx_cb fail");
    }
    if (wm_lvgl_ctx.lvgl_sem != NULL) {
        //wm_log_info("give lvgl_sem");
        xSemaphoreGive(wm_lvgl_ctx.lvgl_sem);
    }
}

void lv_port_disp_init(void)
{
    int ret = WM_ERR_FAILED;

    /*-------------------------
     * Initialize your display
     * -----------------------*/
    disp_init();

    /*-----------------------------
     * Create a buffer for drawing
     *----------------------------*/
#if 0
    /**
     * LVGL requires a buffer where it internally draws the widgets.
     * Later this buffer will passed to your display driver's `flush_cb` to copy its content to your display.
     * The buffer has to be greater than 1 display row
     *
     * There are 3 buffering configurations:
     * 1. Create ONE buffer:
     *      LVGL will draw the display's content here and writes it to your display
     *
     * 2. Create TWO buffer:
     *      LVGL will draw the display's content to a buffer and writes it your display.
     *      You should use DMA to write the buffer's content to the display.
     *      It will enable LVGL to draw the next part of the screen to the other buffer while
     *      the data is being sent form the first buffer. It makes rendering and flushing parallel.
     *
     * 3. Double buffering
     *      Set 2 screens sized buffers and set disp_drv.full_refresh = 1.
     *      This way LVGL will always provide the whole rendered screen in `flush_cb`
     *      and you only need to change the frame buffer's address.
     */

    /* Example for 1) */
    static lv_disp_draw_buf_t draw_buf_dsc_1;
    static lv_color_t buf_1[MY_DISP_HOR_RES * 10];                             /*A buffer for 10 rows*/
    lv_disp_draw_buf_init(&draw_buf_dsc_1, buf_1, NULL, MY_DISP_HOR_RES * 10); /*Initialize the display buffer*/

    /* Example for 2) */
    static lv_disp_draw_buf_t draw_buf_dsc_2;
    static lv_color_t buf_2_1[MY_DISP_HOR_RES * 10];                                /*A buffer for 10 rows*/
    static lv_color_t buf_2_2[MY_DISP_HOR_RES * 10];                                /*An other buffer for 10 rows*/
    lv_disp_draw_buf_init(&draw_buf_dsc_2, buf_2_1, buf_2_2, MY_DISP_HOR_RES * 10); /*Initialize the display buffer*/

    /* Example for 3) also set disp_drv.full_refresh = 1 below*/
    static lv_disp_draw_buf_t draw_buf_dsc_3;
    static lv_color_t buf_3_1[MY_DISP_HOR_RES * MY_DISP_VER_RES]; /*A screen sized buffer*/
    static lv_color_t buf_3_2[MY_DISP_HOR_RES * MY_DISP_VER_RES]; /*Another screen sized buffer*/
    lv_disp_draw_buf_init(&draw_buf_dsc_3, buf_3_1, buf_3_2,
                          MY_DISP_VER_RES * LV_VER_RES_MAX); /*Initialize the display buffer*/
#endif

    static lv_disp_draw_buf_t draw_buf_dsc;
    lv_disp_draw_buf_init(&draw_buf_dsc, lvgl_draw_buff1, lvgl_draw_buff2,
                          LVGL_PORT_BUFF_SIZE); /*Initialize the display buffer*/

    /*-----------------------------------
     * Register the display in LVGL
     *----------------------------------*/

    static lv_disp_drv_t disp_drv; /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv);   /*Basic initialization*/

    /*Set up the functions to access to your display*/

    /*Set the resolution of the display*/
    disp_drv.hor_res = MY_DISP_HOR_RES;
    disp_drv.ver_res = MY_DISP_VER_RES;

    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = disp_flush;

    /*Set a display buffer*/
    disp_drv.draw_buf = &draw_buf_dsc;

    disp_drv.full_refresh = 0;
    /*Required for Example 3)*/
    //disp_drv.full_refresh = 1;

    /* Registers a callback function that gets triggered after the transmission of a bitmap is completed */
    ret = wm_drv_tft_lcd_register_tx_callback(wm_lvgl_ctx.dev, wm_lvgl_tx_cb, NULL);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("register tx cb error(%d)", ret);
    }

    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
    /*You code here*/
    wm_lvgl_ctx.dev = wm_dt_get_device_by_name(WM_LVGL_LCD_MODULE_NAME);
    if (wm_lvgl_ctx.dev == NULL) {
        wm_log_error("lvgl disp init fail");
    }

    wm_lvgl_ctx.lvgl_sem = xSemaphoreCreateCounting(1, 0);
}

volatile bool disp_flush_enabled = true;

/* Enable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_enable_update(void)
{
    disp_flush_enabled = true;
}

/* Disable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_disable_update(void)
{
    disp_flush_enabled = false;
}

/*Flush the content of the internal buffer the specific area on the display
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_disp_flush_ready()' has to be called when finished.*/
static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    int ret                      = WM_ERR_SUCCESS;
    wm_device_t *dev             = wm_lvgl_ctx.dev;
    wm_lcd_data_desc_t data_desc = { 0 };

    if (disp_flush_enabled) {
        data_desc.x_start  = area->x1;
        data_desc.y_start  = area->y1;
        data_desc.x_end    = area->x2;
        data_desc.y_end    = area->y2;
        data_desc.buf      = (uint8_t *)color_p;
        data_desc.buf_size = (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1) * (sizeof(lv_color_t));

        ret = wm_drv_tft_lcd_draw_bitmap(dev, data_desc);
        if (ret != WM_ERR_SUCCESS) {
            wm_log_info("draw bitmap ret(%d)", ret);
        }
    }

    xSemaphoreTake(wm_lvgl_ctx.lvgl_sem, pdMS_TO_TICKS(2000));

    /*IMPORTANT!!!
     *Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(disp_drv);
}
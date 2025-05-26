/**
 * @file main.c
 *
 * @brief Touch Panel Calibration demo main
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

#include "wm_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wm_nvs.h"
#include "wm_drv_tft_lcd.h"
#include "wm_drv_sdh_spi.h"

#include "tp_cali_on_lcd.h"
#include "wm_drv_touch_panel.h"
#include "wm_drv_touch_panel_cfg.h"

#undef LOG_TAG
#define LOG_TAG                 "example"
#define EXAMPLE_NVS_GROUP       "user"
#define TOUCH_PANEL_DEVICE_NAME WM_CFG_TOUCH_PANEL_NAME
#define CALIBRATION_POINT_NUM   3
// The display offset for character '+' on LCD screen
#define LCD_CAL_DISPLAY_OFFSET  20
/**
 * (0,0)|----------------------|
 *      |  +                +  |
 *      |                      |
 *      |                      |
 *      |                      |
 *      |  +                   |
 *      |----------------------|
 * The distance between each coordinate and
 *  the nearest two sides is 'LCD_CAL_DISPLAY_OFFSET'
 */

/**
* Calibration parameters:
*    ax : X-axis scaling. bx : X-axis slope.   cx : X-axis offset.
*    ay : Y-axis slope.   by : Y-axis scaling. cy : Y-axis offset.
*/
typedef struct {
    double ax;
    double bx;
    double cx;
    double ay;
    double by;
    double cy;
} cal_t;
static cal_t g_caldata;

/** Resistance screen calibration: affine transformation method
* @param cross Theoretical coordinates ： calibration point position displayed on the screen
* @param points Actual touch coordinates ： original coordinates returned by the resistive screen
*   cross(X,Y)   point(x,y)
*       X = ax*x + bx*y + cx
*       Y = ay*x + by*y + cy
*   Enter 3 points and solve six equations to obtain six unknowns
*/
static void example_tp_cal_comp(const uint16_t cross[3][2], const uint16_t points[3][2])
{
    uint16_t c0, c1, c2;

    // Work on x values
    c0 = cross[0][0];
    c1 = cross[1][0];
    c2 = cross[2][0];

    /* Compute all the required determinants */
    double dx;

    dx = (double)(points[0][0] - points[2][0]) * (double)(points[1][1] - points[2][1]) -
         (double)(points[1][0] - points[2][0]) * (double)(points[0][1] - points[2][1]);

    g_caldata.ax = ((double)(c0 - c2) * (double)(points[1][1] - points[2][1]) -
                    (double)(c1 - c2) * (double)(points[0][1] - points[2][1])) /
                   dx;
    g_caldata.bx = ((double)(c1 - c2) * (double)(points[0][0] - points[2][0]) -
                    (double)(c0 - c2) * (double)(points[1][0] - points[2][0])) /
                   dx;
    g_caldata.cx = (c0 * ((double)points[1][0] * (double)points[2][1] - (double)points[2][0] * (double)points[1][1]) -
                    c1 * ((double)points[0][0] * (double)points[2][1] - (double)points[2][0] * (double)points[0][1]) +
                    c2 * ((double)points[0][0] * (double)points[1][1] - (double)points[1][0] * (double)points[0][1])) /
                   dx;

    // Work on y values
    c0 = cross[0][1];
    c1 = cross[1][1];
    c2 = cross[2][1];

    g_caldata.ay = ((double)(c0 - c2) * (double)(points[1][1] - points[2][1]) -
                    (double)(c1 - c2) * (double)(points[0][1] - points[2][1])) /
                   dx;
    g_caldata.by = ((double)(c1 - c2) * (double)(points[0][0] - points[2][0]) -
                    (double)(c0 - c2) * (double)(points[1][0] - points[2][0])) /
                   dx;
    g_caldata.cy = (c0 * ((double)points[1][0] * (double)points[2][1] - (double)points[2][0] * (double)points[1][1]) -
                    c1 * ((double)points[0][0] * (double)points[2][1] - (double)points[2][0] * (double)points[0][1]) +
                    c2 * ((double)points[0][0] * (double)points[1][1] - (double)points[1][0] * (double)points[0][1])) /
                   dx;
}

static void example_erase_cal_param(void)
{
    wm_nvs_handle_t handle;
    wm_nvs_open(WM_NVS_DEF_PARTITION, EXAMPLE_NVS_GROUP, WM_NVS_OP_READ_WRITE, &handle);
    wm_nvs_del_key(handle, "tp_cal_ax");
    wm_nvs_del_key(handle, "tp_cal_bx");
    wm_nvs_del_key(handle, "tp_cal_cx");
    wm_nvs_del_key(handle, "tp_cal_ay");
    wm_nvs_del_key(handle, "tp_cal_by");
    wm_nvs_del_key(handle, "tp_cal_cy");
    wm_nvs_close(handle);
}
static int example_set_cal_param(void)
{
    int ret;
    wm_nvs_handle_t handle;
    ret = wm_nvs_open(WM_NVS_DEF_PARTITION, EXAMPLE_NVS_GROUP, WM_NVS_OP_READ_WRITE, &handle);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("open %s, group=%s ret=%d", WM_NVS_DEF_PARTITION, EXAMPLE_NVS_GROUP, ret);
    }

    ret = wm_nvs_set_float(handle, "tp_cal_ax", g_caldata.ax);
    ret |= wm_nvs_set_float(handle, "tp_cal_bx", g_caldata.bx);
    ret |= wm_nvs_set_float(handle, "tp_cal_cx", g_caldata.cx);
    ret |= wm_nvs_set_float(handle, "tp_cal_ay", g_caldata.ay);
    ret |= wm_nvs_set_float(handle, "tp_cal_by", g_caldata.by);
    ret |= wm_nvs_set_float(handle, "tp_cal_cy", g_caldata.cy);
    if (ret != WM_NVS_ERR_OK) {
        wm_log_error("!REEROR! Failed to set calibration params to NVS! ret=%d", ret);
    }
    wm_nvs_close(handle);
    return ret;
}
static int example_get_cal_param(void)
{
    int ret;
    wm_nvs_handle_t handle;
    ret = wm_nvs_open(WM_NVS_DEF_PARTITION, EXAMPLE_NVS_GROUP, WM_NVS_OP_READ_WRITE, &handle);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("open %s, group=%s ret=%d", WM_NVS_DEF_PARTITION, EXAMPLE_NVS_GROUP, ret);
    }
    ret = wm_nvs_get_float(handle, "tp_cal_ax", &g_caldata.ax);
    ret |= wm_nvs_get_float(handle, "tp_cal_bx", &g_caldata.bx);
    ret |= wm_nvs_get_float(handle, "tp_cal_cx", &g_caldata.cx);
    ret |= wm_nvs_get_float(handle, "tp_cal_ay", &g_caldata.ay);
    ret |= wm_nvs_get_float(handle, "tp_cal_by", &g_caldata.by);
    ret |= wm_nvs_get_float(handle, "tp_cal_cy", &g_caldata.cy);
    if (ret != WM_NVS_ERR_OK) {
        wm_log_error("!REEROR! Failed to set calibration params to NVS! ret=%d", ret);
    }
    wm_nvs_close(handle);
    return ret;
}

static int example_tp_cal(wm_device_t *dev, wm_drv_touch_panel_points_t tp_points, uint16_t (*cross)[2])
{
    wm_log_info("start calibration");
    uint16_t point[CALIBRATION_POINT_NUM][2] = { 0 };

    for (int i = 0; i < CALIBRATION_POINT_NUM; i++) {
        example_lcd_clear(LCD_RGB565_WHITE);
        example_lcd_draw_cross(cross[i][0], cross[i][1], 10, LCD_RGB565_RED);

        uint8_t state = 0;
        while (state < 2) {
            int ret = wm_drv_touch_panel_get_coords(dev, &tp_points);

            switch (state) {
                case 0: // Waiting for touch release：
                    if (ret != WM_ERR_SUCCESS) {
                        state = 1;
                    }
                    break;
                case 1: // Waiting for touch press
                    if (ret == WM_ERR_SUCCESS) {
                        point[i][0] = tp_points.curx[0];
                        point[i][1] = tp_points.cury[0];
                        state       = 2;
                    }
                    break;
            }
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
    example_lcd_clear(LCD_RGB565_WHITE);
    example_tp_cal_comp(cross, point);
    return WM_ERR_SUCCESS;
}

static void tp_cal_demo_task(void *parameters)
{
    wm_device_t *dev;
    wm_drv_touch_panel_points_t tp_points;
    wm_drv_touch_panel_set_cfg_t tp_set_cfg;
    wm_drv_touch_panel_get_cfg_t tp_get_cfg;

    tp_set_cfg.rotation = 0;    // used to set the origin position of coordinates
    tp_set_cfg.cal_done = true; // enable calibration

    if (example_lcd_init() != 0) {
        wm_log_error("LCD initialization failed");
        vTaskDelete(NULL);
        return;
    }

    dev = wm_drv_touch_panel_init(TOUCH_PANEL_DEVICE_NAME);
    if (dev == NULL) {
        wm_log_error("Touch panel initialization failed");
        return;
    }

    if (wm_drv_touch_panel_get_config(dev, &tp_get_cfg) != WM_ERR_SUCCESS) {
        wm_log_error("get LCD width and height failed");
        return;
    }

    uint16_t cross[CALIBRATION_POINT_NUM][2] = {
        { LCD_CAL_DISPLAY_OFFSET,                    LCD_CAL_DISPLAY_OFFSET                     },
        { tp_get_cfg.width - LCD_CAL_DISPLAY_OFFSET, LCD_CAL_DISPLAY_OFFSET                     },
        { LCD_CAL_DISPLAY_OFFSET,                    tp_get_cfg.height - LCD_CAL_DISPLAY_OFFSET }
    };
    example_tp_cal(dev, tp_points, cross);

    //Notice: user can customize the touch panel calirbation result storage strategy.
    example_set_cal_param();   // Store calibration parameters to NVS
    example_get_cal_param();   // Get calibration parameters from NVS
    example_erase_cal_param(); // Erasing calibration parameters in NVS

    memcpy(&tp_set_cfg.cal_data, &g_caldata, sizeof(tp_set_cfg.cal_data));

    if (wm_drv_touch_panel_set_config(dev, &tp_set_cfg) != WM_ERR_SUCCESS) {
        wm_log_error("touch panel parameter configuration failed");
    }

    if (wm_drv_touch_panel_get_config(dev, &tp_get_cfg) != WM_ERR_SUCCESS) {
        wm_log_error("get calibration parameters failed");
        return;
    }
    wm_log_info("Calibration parameters: ax = %.4f, bx = %.4f, cx = %.4f, ay = %.4f, by = %.4f, cy = %.4f",
                tp_get_cfg.cal_data.ax, tp_get_cfg.cal_data.bx, tp_get_cfg.cal_data.cx, tp_get_cfg.cal_data.ay,
                tp_get_cfg.cal_data.by, tp_get_cfg.cal_data.cy);

    while (1) {
        if (wm_drv_touch_panel_get_coords(dev, &tp_points) == WM_ERR_SUCCESS) {
            for (int i = 0; i < tp_points.cur_points; ++i) {
                wm_log_info("LCD coordinates(x=%d,y=%d) after calibration.", tp_points.curx[i], tp_points.cury[i]);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
    vTaskDelete(NULL);
}

int main(void)
{
    xTaskCreate(tp_cal_demo_task, "test", 512, NULL, 5, NULL);

    return 0;
}

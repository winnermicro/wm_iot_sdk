/**
  * @file wm_drv_touch_panel.h
  *
  * @brief touch panel header
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
#ifndef __WM_DRV_TOUCH_PANEL_H__
#define __WM_DRV_TOUCH_PANEL_H__

#include "wm_types.h"
#include "wm_error.h"
#include "wm_attr.h"
#include "wm_soc_cfgs.h"
#include "wm_dt.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_TOUCH_PANEL_Structs WM TOUCH PANEL Structs
 * @brief WinnerMicro TOUCH PANEL Structs
 */

/**
 * @addtogroup WM_TOUCH_PANEL_Structs
 * @{
 */

/**
  * @brief Structure to hold touch panel points information.
  *
  * This structure is used to store the maximum number of points,
  * the current number of points, and the x and y coordinates of
  * the points detected by the touch panel.
  *
  * @struct wm_drv_touch_panel_points_t
  * @param cur_points Current number of touch points detected.
  * @param curx Array of x coordinates for the current touch points.
  * @param cury Array of y coordinates for the current touch points.
  */

typedef struct {
    uint8_t cur_points;
    uint16_t curx[CONFIG_CONPONENT_DRIVER_TOUCH_PANEL_MAX_POINT];
    uint16_t cury[CONFIG_CONPONENT_DRIVER_TOUCH_PANEL_MAX_POINT];
} wm_drv_touch_panel_points_t;

/**
  * @brief Configuration structure for the touch panel.
  *
  * @struct wm_drv_touch_panel_set_cfg_t
  * @param rotation Used to rotate the coordinate axis.
  *    The value can be [0, 3]:
  *    - 0: No rotation.
  *    - 1: Rotate 90 degrees counterclockwise.
  *    - 2: Rotate 180 degrees counterclockwise.
  *    - 3: Rotate 270 degrees counterclockwise.
  * @param cal_done Use to enable or disable calibration
  * @param cal_data Calibration parameters used for setting
  *    Actual point(X,Y) , Touch point(x,y) . Satisfy the following formula.
  *       X = ax*x + bx*y + cx
  *       Y = ay*x + by*y + cy
  *    ax : X-axis scaling. bx : X-axis slope.   cx : X-axis offset.
  *    ay : Y-axis slope.   by : Y-axis scaling. cy : Y-axis offset.
  */

typedef struct {
    /**
     * @note The calibration data and other variables need to be expanded.
     */
    uint8_t rotation;
    bool cal_done;
    struct {
        double ax;
        double bx;
        double cx;
        double ay;
        double by;
        double cy;
    } cal_data;
} wm_drv_touch_panel_set_cfg_t;

/**
  * @brief Used to obtain the touchpad attribute structure.
  *
  * @struct wm_drv_touch_panel_get_cfg_t
  * @param rotation is a rotation configuration parameter.
  * @param width is used to store the maximum value of 
  * touchpad width obtained from the get function.
  * @param height is used to store the maximum value of
  * touchpad height obtained from the get function
  * @param cal_data Calibration parameters used for getting
  *    Actual point(X,Y), Touch point(x,y). Satisfy the following formula.
  *       X = ax*x + bx*y + cx
  *       Y = ay*x + by*y + cy
  *    ax : X-axis scaling. bx : X-axis slope.   cx : X-axis offset.
  *    ay : Y-axis slope.   by : Y-axis scaling. cy : Y-axis offset.
  */

typedef struct {
    /**
     * @note The calibration data and other variables need to be expanded.
     */
    uint8_t rotation;
    uint16_t width;
    uint16_t height;
    struct {
        double ax;
        double bx;
        double cx;
        double ay;
        double by;
        double cy;
    } cal_data;
} wm_drv_touch_panel_get_cfg_t;

/**
  * @brief instance api types
  */
typedef struct {
    int (*init)(wm_device_t *device);   /**< initialize   */
    int (*deinit)(wm_device_t *device); /**< deinitialize */

    int (*set_cfg)(wm_device_t *device, wm_drv_touch_panel_set_cfg_t *dev_scfg);    /**< set value */
    int (*get_cfg)(wm_device_t *device, wm_drv_touch_panel_get_cfg_t *dev_gcfg);    /**< get value */
    int (*get_coords)(wm_device_t *device, wm_drv_touch_panel_points_t *tp_points); /**< gitcoords    */
} wm_drv_touch_panel_ops_t;

/**
 * @}
 */

/**
 * @defgroup WM_DRV_TOUCH_PANEL_APIs WM DRV TOUCH PANEL APIs
 * @brief WinnerMicro DRV TOUCH PANEL APIs
 */

/**
 * @addtogroup WM_DRV_TOUCH_PANEL_APIs
 * @{
 */

/**
  * @brief Initialize touch panel chip.
  *
  * @param[in] device_name is a pointer to the touch panel configuration structure.
  * @return
  *    - !0 : controler handle
  *    - NULL: fail
  */
wm_device_t *wm_drv_touch_panel_init(const char *device_name);

/**
  * @brief Deinitializes touch panel chip.
  *
  * @param[in] device is a pointer to the touch panel configuration structure.
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_drv_touch_panel_deinit(wm_device_t *device);

/**
  * @brief Used to transfer configuration parameters to the internal system of the program. 
  *
  * @param[in] device is a pointer to the touch panel configuration structure.
  * @param[in] dev_scfg is a structure used to configure some parameters.
  * The parameters passed include rotation in the wm_drv_touch_panel_set_cfg_t structure.
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_drv_touch_panel_set_config(wm_device_t *device, wm_drv_touch_panel_set_cfg_t *dev_scfg);

/**
  * @brief Used to retrieve data on the width and height of the touch panel from the device table.
  *
  * @param[in] device is a pointer to the touch panel configuration structure.
  * @param[in] dev_gcfg Used to retrieve some data from the device table.
  * The parameters passed include the width and height of the structure 'wm_drv_touch_panel_get_cfg_t'.
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_drv_touch_panel_get_config(wm_device_t *device, wm_drv_touch_panel_get_cfg_t *dev_gcfg);

/**
  * @brief Obtain the number of coordinate points and
  * the actual x and y positions of the coordinate points.
  *
  * @param[in] device is a pointer to the touch panel configuration structure.
  * @param[in] tp_points Tppoints is a structural pointer used to obtain
  * the number of touch coordinate points and the x-axis and y-axis of the
  * coordinates, while also passing the maximum number of touch points
  * supported by the driver.
  * 
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_drv_touch_panel_get_coords(wm_device_t *device, wm_drv_touch_panel_points_t *tp_points);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_DRV_TOUCH_PANEL_H__ */

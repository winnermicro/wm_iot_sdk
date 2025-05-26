/**
 * @file wm_drv_touch_panel.c
 *
 * @brief Universal touch driver interface functions
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

#include "wmsdk_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wm_log.h"
#include "wm_types.h"
#include "wm_error.h"
#include "wm_attr.h"
#include "wm_osal.h"
#include "wm_drv_touch_panel.h"
// Change the current path of wm-d t_hw. h to the path in your own project
#include "wm_dt_hw.h"

/**
 * @brief Check param and return WM_ERR_INVALID_PARAM
 */
#define WM_DRV_TOUCH_CHECK_PARAM(p)  \
    if (!(p)) {                      \
        return WM_ERR_INVALID_PARAM; \
    }

wm_device_t *wm_drv_touch_panel_init(const char *device_name)
{
    wm_device_t *dev = (device_name ? wm_dt_get_device_by_name(device_name) : NULL);
    return (dev && (((wm_drv_touch_panel_ops_t *)dev->ops)->init(dev) == WM_ERR_SUCCESS)) ? dev : NULL;
}

int wm_drv_touch_panel_deinit(wm_device_t *device)
{
    WM_DRV_TOUCH_CHECK_PARAM(device);
    return ((wm_drv_touch_panel_ops_t *)device->ops)->deinit(device);
}

int wm_drv_touch_panel_set_config(wm_device_t *device, wm_drv_touch_panel_set_cfg_t *dev_scfg)
{
    WM_DRV_TOUCH_CHECK_PARAM(device && dev_scfg);
    return ((wm_drv_touch_panel_ops_t *)device->ops)->set_cfg(device, dev_scfg);
}

int wm_drv_touch_panel_get_config(wm_device_t *device, wm_drv_touch_panel_get_cfg_t *dev_scfg)
{
    WM_DRV_TOUCH_CHECK_PARAM(device && dev_scfg);
    return ((wm_drv_touch_panel_ops_t *)device->ops)->get_cfg(device, dev_scfg);
}

int wm_drv_touch_panel_get_coords(wm_device_t *device, wm_drv_touch_panel_points_t *tp_points)
{
    WM_DRV_TOUCH_CHECK_PARAM(device && tp_points);
    return ((wm_drv_touch_panel_ops_t *)device->ops)->get_coords(device, tp_points);
}
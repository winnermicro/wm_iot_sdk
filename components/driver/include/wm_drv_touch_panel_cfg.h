/**
  * @file wm_drv_touch_panel_cfg.h
  *
  * @brief touch panel config header
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
#ifndef __WM_DRV_TOUCH_PANEL_CFG_H__
#define __WM_DRV_TOUCH_PANEL_CFG_H__

/** List all the device names of touch panel devices that
 *  exactly match the device names defined in the device table. */
#define DEV_NAME_FT6336_I2C        "ft6336"
#define DEV_NAME_XPT2046_SPIM_SOFT "xpt2046"

#if defined(CONFIG_COMPONENT_XPT2046_DRIVER)
#define WM_CFG_TOUCH_PANEL_NAME DEV_NAME_XPT2046_SPIM_SOFT

#elif defined(CONFIG_COMPONENT_FT6336_DRIVER)
#define WM_CFG_TOUCH_PANEL_NAME DEV_NAME_FT6336_I2C

#endif

#endif
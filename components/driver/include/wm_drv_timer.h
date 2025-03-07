/**
 * @file wm_drv_timer.h
 *
 * @brief Timer Module
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

#ifndef __WM_DRV_TIMER_H__
#define __WM_DRV_TIMER_H__

#include "wm_types.h"
#include "wm_soc_cfgs.h"
#include "wm_dt.h"
#include "wm_hal_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_TIMER_TYPEs WM DRV Uart TYPEs
 * @brief WinnerMicro DRV Uart TYPEs
 */

/**
 * @addtogroup WM_TIMER_TYPEs
 * @{
 */

/**
 * @typedef wm_drv_timer_unit
 * @brief TIMER unit type
 */

typedef wm_hal_timer_unit_t wm_drv_timer_unit;

/**
 * @typedef wm_drv_timer_callback_t
 * @brief TIMER callback type
 */

typedef void (*wm_drv_timer_callback_t)(void *arg);

/**
 * @}
 */

/**
 * @defgroup WM_TIMER_Structs WM TIMER Structs
 * @brief WinnerMicro TIMER Structs
 */

/**
 * @addtogroup WM_TIMER_Structs
 * @{
 */

/**
 * @struct wm_drv_timer_cfg_t
 * @brief TIMER configuration
 */
typedef struct {
    wm_drv_timer_unit unit; /**< timer accuracy */
    uint32_t period;        /**< timeout period */
    bool auto_reload;       /**< auto reload */
} wm_drv_timer_cfg_t;

/**
 * @}
 */

/**
 * @defgroup WM_DRV_TIMER_APIs WM DRV TIMER APIs
 * @brief WinnerMicro DRV TIMER APIs
 */

/**
 * @addtogroup WM_DRV_TIMER_APIs
 * @{
 */

/**
  * @brief Init timer
  *
  * @param [in] dev_name: device name, like "timer0","timer1"...
  * @return
  *    - device handle, use @arg wm_device_t
  *    - NULL: failed
  */
wm_device_t *wm_drv_timer_init(char *dev_name);

/**
  * @brief Deinit timer.
  *
  * @param [in] dev: device handle, use @arg wm_device_t
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_drv_timer_deinit(wm_device_t *dev);

/**
  * @brief Start timer.
  *
  * @param [in] dev: device handle, use @arg wm_device_t  
  * @param [in] timer_cfg: timer cfg, use @arg wm_drv_timer_cfg_t
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_drv_timer_start(wm_device_t *dev, wm_drv_timer_cfg_t timer_cfg);

/**
  * @brief Stop timer.
  *
  * @param [in] dev: device handle, use @arg wm_device_t  
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_drv_timer_stop(wm_device_t *dev);

/**
  * @brief Set timer period. Timer will restart for this
  *
  * @param [in] dev: device handle, use @arg wm_device_t    
  * @param [in] period: timer period, unit decided by @arg wm_drv_timer_cfg_t
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_drv_timer_set_period(wm_device_t *dev, uint32_t period);

/**
  * @brief Set timer autoreload. Timer will restart for this
  *
  * @param [in] dev: device handle, use @arg wm_device_t    
  * @param [in] auto_reload: timer autoreload
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_drv_timer_set_auto_reload(wm_device_t *dev, bool auto_reload);

/**
  * @brief Get timer counter.
  *
  * @param [in] dev: device handle, use @arg wm_device_t
  * @param [out] counter: timer counter, unit decided by @arg wm_drv_timer_cfg_t
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_drv_timer_get_counter(wm_device_t *dev, uint32_t *counter);

/**
  * @brief Register timer isr.
  *
  * @param [in] dev: device handle, use @arg wm_device_t
  * @param [in] timer_callback: timer isr handle, use @arg wm_drv_timer_callback_t  
  * @param [in] arg: private data
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_drv_timer_register_callback(wm_device_t *dev, wm_drv_timer_callback_t timer_callback, void *arg);

/**
  * @brief Unregister timer isr.
  *
  * @param [in] dev: device handle, use @arg wm_device_t
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_drv_timer_unregister_callback(wm_device_t *dev);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* WM_DRV_TIMER_H */

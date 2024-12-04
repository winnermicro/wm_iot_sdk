/**
 * @file wm_drv_wdt.h
 *
 * @brief wdt drv api
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

#ifndef __WM_DRV_WDT_H__
#define __WM_DRV_WDT_H__

#include "wm_types.h"
#include "wm_osal.h"
#include "wm_soc_cfgs.h"
#include "wm_dt.h"
#include "wm_drv_irq.h"
#include "wm_hal_wdt.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_WDT_APIs WM WDT APIs
 * @brief WM WDT APIs
 */

/**
 * @addtogroup WM_WDT_APIs
 * @{
 */

/**
 * @brief Initialize WDT dev, and configure wdt based on the device tree.
 *
 * @param [in] dev_name like "wdt", if device tree set
 * @return
 *    - device handle: device handle, used @ref wm_device_t*
 *    - NULL: failed
 */
wm_device_t *wm_drv_wdt_init(char *dev_name);

/**
 * @brief Deinit wdt.
 *
 * @param [in] dev use @arg wm_device_t
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_drv_wdt_deinit(wm_device_t *dev);

/**
 * @brief Start wdt.
 *
 * @param [in] dev use @arg wm_device_t
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_drv_wdt_start(wm_device_t *dev);

/**
 * @brief Stop wdt.
 * @param [in] dev use @arg wm_device_t
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_drv_wdt_stop(wm_device_t *dev);

/**
 * @brief Set wdt counter value.
 *
 * @param [in] dev use @arg wm_device_t
 * @param [in] counter_value wdt counter value, unit:microseconds
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_drv_wdt_set_counter_value(wm_device_t *dev, uint32_t counter_value);

/**
 * @brief Get wdt counter value.
 *
 * @param [in] dev use @arg wm_device_t
 * @param [out] counter_value wdt counter value. the unit is microseconds, which needs to be divided by apb clock
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_drv_wdt_get_counter_value(wm_device_t *dev, uint32_t *counter_value);

/**
 * @brief Get wdt remaining time.
 *
 * @param [in] dev use @arg wm_device_t
 * @param [out] remaining_time wdt remaining value
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_drv_wdt_get_remaining_time(wm_device_t *dev, uint32_t *remaining_time);

/**
 * @brief Wdt feed.
 *
 * @param [in] dev use @arg wm_device_t
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_drv_wdt_feed(wm_device_t *dev);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_DRV_WDT_H__ */

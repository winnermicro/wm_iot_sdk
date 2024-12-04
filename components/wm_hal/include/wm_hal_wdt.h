/**
 * @file wm_hal_wdt.h
 *
 * @brief Watchdog Timer Module
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
#ifndef __WM_HAL_WDT_H__
#define __WM_HAL_WDT_H__

#include "wm_types.h"
#include "wm_wdt_reg_struct.h"
#include "wm_ll_wdt.h"
#include "wm_soc_cfgs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief wdt device structure
 * 
 * @note
 *      - register_base: wdt register base 
 *      - irq_num: irq number
 *      - period: wdt period
 */
typedef struct {
    uint32_t register_base;
    uint32_t irq_num;
    uint32_t period; //wdt counter value , unit:microseconds
} wm_hal_wdt_dev_t;

/**
 * @brief wdt state
 * 
 * @note
 *      - WM_WDT_STATE_ON: wdt is on
 *      - WM_WDT_STATE_OFF: wdt is off
 */
typedef enum {
    WM_WDT_STATE_ON,
    WM_WDT_STATE_OFF,
} wm_hal_wdt_state_t;

/**
 * @brief wdt init
 * @param[in] dev: wdt device
 * 
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_wdt_init(wm_hal_wdt_dev_t *dev);

/**
 * @brief wdt deinit
 * @param[in] dev: wdt device
 * 
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_wdt_deinit(wm_hal_wdt_dev_t *dev);

/**
 * @brief wdt start
 * @param[in] dev: wdt device
 * 
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_wdt_start(wm_hal_wdt_dev_t *dev);

/**
 * @brief wdt stop
 * @param[in] dev: wdt device
 * 
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_wdt_stop(wm_hal_wdt_dev_t *dev);

/**
 * @brief wdt feed
 * @param[in] dev: wdt device
 * 
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_wdt_feed(wm_hal_wdt_dev_t *dev);

/**
 * @brief wdt get status
 * @param[in] dev: wdt device
 * 
 * @return
 *    - WM_WDT_STATE_OFF: wdt off
 *    - WM_WDT_STATE_ON: wdt on
 */
int wm_hal_wdt_get_status(wm_hal_wdt_dev_t *dev);

/**
 * @brief set wdt counter value and restart wdt
 * @param[in] dev: wdt device 
 * @param[in] conuter_value: wdt counter value
 * 
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_wdt_set_counter_value(wm_hal_wdt_dev_t *dev, uint32_t conuter_value);

/**
 * @brief get wdt counter value
 * @param[in] dev: wdt device 
 * @param[out] conuter_value: wdt counter value
 * 
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_wdt_get_counter_value(wm_hal_wdt_dev_t *dev, uint32_t *conuter_value);

/**
 * @brief get wdt current value
 * @param[in] dev: wdt device 
 * @param[out] current_value: wdt current value
 * 
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_wdt_get_current_value(wm_hal_wdt_dev_t *dev, uint32_t *current_value);

#ifdef __cplusplus
}
#endif

#endif /* __WM_HAL_WDT_H__ */

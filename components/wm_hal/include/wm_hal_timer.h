/**
 * @file wm_hal_timer.h
 *
 * @brief timer hal api
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

#ifndef __WM_HAL_TIMER_H__
#define __WM_HAL_TIMER_H__

#include "wm_ll_timer.h"
#include "wm_soc_cfgs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*wm_hal_timer_callback_t)(void *arg);

typedef enum {
    WM_HAL_TIMER_UNIT_US = 0, /**< microsecond level(us) */
    WM_HAL_TIMER_UNIT_MS      /**< millisecond level(ms) */
} wm_hal_timer_unit_t;

typedef struct {
    wm_hal_timer_unit_t unit; /**< timer accuracy */
    uint32_t period;          /**< timeout period */
    bool auto_reload;         /**< auto reload */
} wm_hal_timer_cfg_t;

typedef struct {
    uint32_t clock; /**< dev clock, unit: MHZ,it must be an APB clock*/
} wm_hal_timer_dev_cfg_t;

typedef struct {
    wm_timer_reg_t *dev;
    wm_hal_timer_callback_t callback;
    void *arg;
} wm_hal_timer_dev_t;

#define WM_HAL_TIMER_IS_TIMERID(timer_id) (timer_id >= WM_TIMER_ID_0 && timer_id < WM_TIMER_ID_MAX)

/**
 * @brief Initialize timer.
 *
 * @param [in] hal_timer use @arg wm_hal_timer_dev_t
 * @param [in] dev_cfg use @arg wm_hal_timer_dev_cfg_t
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_timer_init(wm_hal_timer_dev_t *hal_timer, wm_hal_timer_dev_cfg_t dev_cfg);

/**
 * @brief Start timer.
 *
 * @param [in] hal_timer use @arg wm_hal_timer_dev_t
 * @param [in] timer_id use @arg wm_timer_id_t
 * @param [in] timer_cfg use @arg wm_hal_timer_cfg_t
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_timer_start(wm_hal_timer_dev_t *hal_timer, wm_timer_id_t timer_id, wm_hal_timer_cfg_t timer_cfg);

/**
 * @brief Stop timer.
 *
 * @param [in] hal_timer use @arg wm_hal_timer_dev_t
 * @param [in] timer_id use @arg wm_timer_id_t
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_timer_stop(wm_hal_timer_dev_t *hal_timer, wm_timer_id_t timer_id);

/**
 * @brief Restart timer.
 *
 * @param [in] hal_timer use @arg wm_hal_timer_dev_t
 * @param [in] timer_id  use @arg wm_timer_id_t
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_timer_restart(wm_hal_timer_dev_t *hal_timer, wm_timer_id_t timer_id);

/**
 * @brief Set timer period.
 *
 * @param [in] hal_timer use @arg wm_hal_timer_dev_t
 * @param [in] timer_id use @arg wm_timer_id_t
 * @param [in] period
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_timer_set_period(wm_hal_timer_dev_t *hal_timer, wm_timer_id_t timer_id, uint32_t period);

/**
 * @brief Get timer period.
 *
 * @param [in] hal_timer use @arg wm_hal_timer_dev_t
 * @param [in] timer_id use @arg wm_timer_id_t
 * @param [out] period
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_timer_get_period(wm_hal_timer_dev_t *hal_timer, wm_timer_id_t timer_id, uint32_t *period);

/**
 * @brief Set timer autoreload and start timer.
 *
 * @param [in] hal_timer use @arg wm_hal_timer_dev_t
 * @param [in] timer_id use @arg wm_timer_id_t
 * @param [in] auto_reload  auto_reload 1 for auto reload, 0 for not
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_timer_set_auto_reload(wm_hal_timer_dev_t *hal_timer, wm_timer_id_t timer_id, bool auto_reload);

/**
 * @brief Set timer autoreload.
 *
 * @param [in] hal_timer use @arg wm_hal_timer_dev_t
 * @param [in] timer_id use @arg wm_timer_id_t
 * @param [out] auto_reload auto reload
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_timer_get_auto_reload(wm_hal_timer_dev_t *hal_timer, wm_timer_id_t timer_id, bool *auto_reload);

/**
 * @brief Get timer counter.
 *
 * @param [in] hal_timer use @arg wm_hal_timer_dev_t
 * @param [in] timer_id use @arg wm_timer_id_t
 * @param [out] counter counter
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_timer_get_counter(wm_hal_timer_dev_t *hal_timer, wm_timer_id_t timer_id, uint32_t *counter);

/**
 * @brief Get timer irq status.
 *
 * @param [in] hal_timer use @arg wm_hal_timer_dev_t
 * @param [in] timer_id use @arg wm_timer_id_t
 * @param [out] status irq status, 1 intrrupt occured, 0 for not
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_timer_get_irq_status(wm_hal_timer_dev_t *hal_timer, wm_timer_id_t timer_id, uint32_t *status);

/**
 * @brief Clean timer irq status.
 *
 * @param [in] hal_timer use @arg wm_hal_timer_dev_t
 * @param [in] timer_id use @arg wm_timer_id_t
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_timer_clear_irq_status(wm_hal_timer_dev_t *hal_timer, wm_timer_id_t timer_id);

#ifdef __cplusplus
}
#endif

#endif /* __WM_HAL_TIMER_H__ */

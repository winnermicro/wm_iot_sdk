/**
 * @file wm_hal_touch_sensor.h
 *
 * @brief touch sensor Module
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
#ifndef __WM_HAL_TOUCH_SENSOR_H__
#define __WM_HAL_TOUCH_SENSOR_H__

#include "wm_types.h"
#include "wm_irq.h"
#include "wm_soc_cfgs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief touch sensor device struct
 *
 * @param register_base     touch sensor register base address
 * @param irq_num           touch sensor interrupt number
 * @param hal_touch_config  touch sensor config struct
 * @param user_data         Pointer to user data to be passed to the callback function.
 * @param g_hal_touch_sensor_callback     Pointer to the touch_num callback function.
 * @param pins              The corresponding io pin of the sensor
 */
typedef struct wm_hal_touch_sensor_dev {
    uint32_t register_base;
    wm_irq_no_t irq_num;

    void *callback[WM_TOUCH_NUM_MAX + 1]; /**< ref wm_hal_touch_sensor_callback_t */
    void *callback_priv[WM_TOUCH_NUM_MAX + 1];
} wm_hal_touch_sensor_dev_t;

/**
 * @brief Prototype touch sensor callback function
*/
typedef void (*wm_hal_touch_sensor_callback_t)(wm_hal_touch_sensor_dev_t *dev, wm_touch_sensor_num_t touch_num, void *priv);

/**
 * @brief Initializes the touch sensor.
 * @param[in] dev Pointer to the touch sensor device structure.
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_touch_sensor_init(wm_hal_touch_sensor_dev_t *dev);

/**
 * @brief Deinitializes the touch sensor.
 * @param[in] dev Pointer to the touch sensor device structure.
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_touch_sensor_deinit(wm_hal_touch_sensor_dev_t *dev);

/**
 * @brief Sets the threshold value for touch sensing.
 * @param[in] dev Pointer to the touch sensor device structure.
 * @param[in] touch_num Touch sensor number, range [0, WM_TOUCH_NUM_MAX].
 * @param[in] threshold Threshold value to set, max value is WM_TOUCH_THRESHOLD_MAX.
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_touch_sensor_set_threshold(wm_hal_touch_sensor_dev_t *dev, wm_touch_sensor_num_t touch_num, uint32_t threshold);

/**
 * @brief Gets the threshold value for touch sensing.
 * @param[in]  dev Pointer to the touch sensor device structure.
 * @param[in]  touch_num Touch sensor number, range [0, WM_TOUCH_NUM_MAX].
 * @param[out] threshold Threshold value to get.
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_touch_sensor_get_threshold(wm_hal_touch_sensor_dev_t *dev, wm_touch_sensor_num_t touch_num, uint32_t *threshold);

/**
 * @brief Gets the interrupt enable status for touch sensing.
 * @param[in]  dev Pointer to the touch sensor device structure.
 * @param[out] enable the interrupt enable status.
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_touch_sensor_get_int_enable(wm_hal_touch_sensor_dev_t *dev, uint16_t *enable);

/**
 * @brief Sets the interrupt enable status for touch sensing.
 * @param[in]  dev Pointer to the touch sensor device structure.
 * @param[in]  enable the interrupt enable status.
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_touch_sensor_set_int_enable(wm_hal_touch_sensor_dev_t *dev, uint16_t enable);

/**
 * @brief Registers a callback function for touch sensor.
 * @param[in] dev Pointer to the touch sensor device structure.
 * @param[in] touch_num Touch sensor number, range [0, WM_TOUCH_NUM_MAX].
 * @param[in] callback Pointer to the callback function .
 * @param[in] priv Pointer to user data to be passed to the callback function.
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_hal_touch_sensor_register_callback(wm_hal_touch_sensor_dev_t *dev, wm_touch_sensor_num_t touch_num,
                                          wm_hal_touch_sensor_callback_t callback, void *priv);

/**
 * @brief Reads the count number from the touch sensor.
 * @param[in] dev Pointer to the touch sensor device structure.
 * @param[in] touch_num Touch sensor number [0, WM_TOUCH_NUM_MAX].
 * @param[out] countnum Countnum value to get.
 * @return The count number read from the touch_num touch sensor
 */
int wm_hal_touch_sensor_read_countnum(wm_hal_touch_sensor_dev_t *dev, wm_touch_sensor_num_t touch_num, uint32_t *countnum);

/**
 * @brief Starts the touch sensor scan.
 * @param[in] dev Pointer to the touch sensor device structure.
 * @param[in] touch_num Touch sensor number, range [0, WM_TOUCH_NUM_MAX].
 */
void wm_hal_touch_sensor_start(wm_hal_touch_sensor_dev_t *dev, wm_touch_sensor_num_t touch_num);

/**
 * @brief Stops the touch sensor scan.
 * @param[in] dev Pointer to the touch sensor device structure.
 * @param[in] touch_num Touch sensor number, range [0, WM_TOUCH_NUM_MAX].
 */
void wm_hal_touch_sensor_stop(wm_hal_touch_sensor_dev_t *dev, wm_touch_sensor_num_t touch_num);

#ifdef __cplusplus
}
#endif

#endif /* __WM_HAL_TOUCH_SENSOR_H__ */

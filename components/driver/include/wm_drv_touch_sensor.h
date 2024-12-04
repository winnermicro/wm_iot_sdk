/**
 * @file wm_drv_touch_sensor.h
 *
 * @brief touch sensor Driver Module
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

#ifndef __WM_DRV_TOUCH_SENSOR_H__
#define __WM_DRV_TOUCH_SENSOR_H__

#include "wm_types.h"
#include "wm_dt.h"
#include "wm_soc_cfgs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_DRV_TOUCH_SENSOR_TYPEs WM DRV TOUCH SENSOR TYPEs
 * @brief WinnerMicro DRV TOUCH SENSOR TYPEs
 */

/**
 * @addtogroup WM_DRV_TOUCH_SENSOR_TYPEs
 * @{
 */

/**
 * @brief Prototype touch sensor driver callback function
*/
typedef void (*wm_drv_touch_sensor_callback_t)(wm_touch_sensor_num_t touch_num, void *priv);

/**
 * @}
 */

/**
 * @defgroup WM_DRV_TOUCH_SENSOR_APIs WM DRV TOUCH SENSOR APIs
 * @brief WinnerMicro DRV TOUCH SENSOR APIs
 */

/**
 * @addtogroup WM_DRV_TOUCH_SENSOR_APIs
 * @{
 */

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
int wm_drv_touch_sensor_register_callback(wm_device_t *dev, wm_touch_sensor_num_t touch_num,
                                          wm_drv_touch_sensor_callback_t callback, void *priv);

/**
 * @brief Sets the threshold value for touch sensing.
 * @param[in] dev Pointer to the touch sensor device.
 * @param[in] touch_num Touch sensor number, range [0, WM_TOUCH_NUM_MAX].
 * @param[in] threshold Threshold value to set, max value is WM_TOUCH_THRESHOLD_MAX.
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_drv_touch_sensor_set_threshold(wm_device_t *dev, wm_touch_sensor_num_t touch_num, uint32_t threshold);

/**
 * @brief Gets the threshold value for touch sensing.
 * @param[in]  dev Pointer to the touch sensor device.
 * @param[in]  touch_num Touch sensor number, range [0, WM_TOUCH_NUM_MAX].
 * @param[out] threshold Threshold value to get.
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_drv_touch_sensor_get_threshold(wm_device_t *dev, wm_touch_sensor_num_t touch_num, uint32_t *threshold);

/**
 * @brief Reads the count number from the touch sensor.
 * @param[in]  dev Pointer to the touch sensor device structure.
 * @param[in]  touch_num Touch sensor number [0, WM_TOUCH_NUM_MAX].
 * @param[out] countnum Countnum value to get.
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_drv_touch_sensor_read_countnum(wm_device_t *dev, wm_touch_sensor_num_t touch_num, uint32_t *countnum);

/**
  * @brief     start touch sensor driver
  *
  * @param[in] dev Pointer to the touch sensor device structure.
  * @param[in] touch_num Touch sensor number [0, WM_TOUCH_NUM_MAX].
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_drv_touch_sensor_start(wm_device_t *dev, wm_touch_sensor_num_t touch_num);

/**
  * @brief     stop touch sensor driver
  *
  * @param[in] dev Pointer to the touch sensor device structure.
  * @param[in] touch_num Touch sensor number [0, WM_TOUCH_NUM_MAX].
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_drv_touch_sensor_stop(wm_device_t *dev, wm_touch_sensor_num_t touch_num);

/**
  * @brief     Initialize touch sensor driver
  *
  * @param[in] dev_name  touch sensor device name
  *
  * @return
  *    - NULL: failed
  *    - others: succeed
  */
wm_device_t *wm_drv_touch_sensor_init(const char *dev_name);

/**
  * @brief     deinit touch sensor driver
  *
  * @param[in] dev  touch sensor driver device
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_drv_touch_sensor_deinit(wm_device_t *dev);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_DRV_TOUCH_SENSOR_H__ */

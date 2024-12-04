/**
 * @file wm_drv_touch_button.h
 *
 * @brief TOUCH BUTTON Module
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

#ifndef __WM_DRV_TOUCH_BUTTON_H__
#define __WM_DRV_TOUCH_BUTTON_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "wm_types.h"
#include "wm_dt.h"
#include "wm_soc_cfgs.h"

/**
 * @defgroup WM_DRV_TOUCH_BUTTON_TYPEs WM DRV TOUCH BUTTON TYPEs
 * @brief WinnerMicro DRV TOUCH BUTTON TYPEs
 */

/**
 * @addtogroup WM_DRV_TOUCH_BUTTON_TYPEs
 * @{
 */

typedef enum {
    WM_DRV_TOUCH_BUTTON_PRESS = 0, /**< touch key press event */
    WM_DRV_TOUCH_BUTTON_RELEASE,   /**< touch key release event */

    WM_DRV_TOUCH_BUTTON_PRESSED,     /**< touch key short pressed event */
    WM_DRV_TOUCH_BUTTON_LONG_PRESSED /**< touch key long pressed event */
} wm_drv_touch_button_state_t;

/**
 * @brief Prototype touch button callback function
*/
typedef void (*wm_drv_touch_button_callback_t)(wm_touch_sensor_num_t touch_num, wm_drv_touch_button_state_t state, void *priv);

/**
 * @}
 */

/**
 * @defgroup WM_DRV_TOUCH_BUTTON_APIs WM DRV TOUCH BUTTON APIs
 * @brief WinnerMicro DRV TOUCH BUTTON APIs
 */

/**
 * @addtogroup WM_DRV_TOUCH_BUTTON_APIs
 * @{
 */

/**
 * @brief   Touch button register callback
 *
 * This function sets a callback routine into touch button,
 * when the subscribed events occur, the callback routine will be called.
 *
 * @param[in] dev           Touch button device
 * @param[in] callback      Event callback
 * @param[in] priv          User private pointer
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 *
 * @warning     Must be used before start api,
 *              since this event callback routine runs on driver core,
 *              it should not do something that attempts to Block, such as calling vTaskDelay().
 *
 */
int wm_drv_touch_button_register_callback(wm_device_t *dev, wm_drv_touch_button_callback_t callback, void *priv);

/**
 * @brief   Touch button set long press trigger time
 *
 * This function sets the threshold time (ms) for a long press event. If a button is pressed
 * and held for a period of time that exceeds the threshold time, a long press event is triggered.
 * Default long press time 1000 ms.
 *
 * @param[in] dev               Touch button device
 * @param[in] threshold_time    Threshold time (ms) of long press event occur, Min 1000.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 */
int wm_drv_touch_button_set_longpress(wm_device_t *dev, uint32_t threshold_time);

/**
  * @brief     start touch button
  *
  * @param[in] dev Pointer to the touch button device structure.
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_drv_touch_button_start(wm_device_t *dev);

/**
  * @brief     stop touch button
  *
  * @param[in] dev Pointer to the touch button device structure.
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_drv_touch_button_stop(wm_device_t *dev);

/**
  * @brief     Initialize touch button
  *
  * @param[in] dev_name  touch button device name
  *
  * @return
  *    - NULL: failed
  *    - others: succeed
  */
wm_device_t *wm_drv_touch_button_init(const char *dev_name);

/**
  * @brief     Deinitialize touch button
  *
  * @param[in] dev  touch button device
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_drv_touch_button_deinit(wm_device_t *dev);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_TOUCH_BUTTON_H__ */

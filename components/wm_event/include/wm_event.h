/**
 * @file wm_event.h
 *
 * @brief event Module
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

#ifndef __WM_EVENT_H__
#define __WM_EVENT_H__

#include "wm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_EVENT_MACROs WM EVENT MACROs
 * @brief WinnerMicro EVENT MACROs
 */

/**
 * @defgroup WM_EVENT_TYPEs WM EVENT TYPEs
 * @brief WinnerMicro EVENT TYPEs
 */

/**
 * @defgroup WM_EVENT_APIs WM EVENT APIs
 * @brief WinnerMicro EVENT APIs
 */

/**
 * @addtogroup WM_EVENT_MACROs
 * @{
 */

/**
  * @brief  declare event group
  *
  * Used in moudle's api header file, Users can only use the name declared by WM_EVENTDECLARE_GROUP
  */
#define WM_EVENT_DECLARE_GROUP(group) extern wm_event_group_t const group

/**
  * @brief  define event group
  *
  * Used in moudle's source file,only define one time
  */
#define WM_EVENT_DEFINE_GROUP(group)  wm_event_group_t const group = #group

#define WM_EVENT_ANY_GROUP            NULL /**< any event group */
#define WM_EVENT_ANY_TYPE             -1   /**< any event type  */

/**
 * @}
 */

/**
 * @addtogroup WM_EVENT_TYPEs
 * @{
 */

typedef const char *wm_event_group_t; /**< group type, Must use the names defined by WM_EVENT_DEFINE_GROUP
                                        and declared by WM_EVENT_DECLARE_GROUP */

/**
  * @brief  event callback
  *
  * @param[in] group  event group,module define by WM_EVENT_DEFINE_GROUP
  * @param[in] event  event type
  * @param[in] data  pointer to module event data
  * @param[in] priv  user private data,come from wm_event_add_callback
  *
  */
typedef void (*wm_event_callback)(wm_event_group_t group, int event, void *data, void *priv);

/**
 * @}
 */

/**
 * @addtogroup WM_EVENT_APIs
 * @{
 */

/**
  * @brief  init event
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_FAILED: failed
  */
int wm_event_init(void);

/**
  * @brief  deinit event
  *     remove all callback before deinit
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_FAILED: failed
  */
int wm_event_deinit(void);

/**
  * @brief  add callback
  *
  * @param[in] group  event group,module define by WM_EVENT_DEFINE_GROUP,
  *                    WM_EVENT_ANY_GROUP for filter all group
  * @param[in] event  event type,define by modules
  *                    WM_EVENT_ANY_EVENT for all event in group
  * @param[in] callback  user callback function
  * @param[in] priv  user private data,it will transfer from callback to user
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_FAILED: failed
  *    - WM_ERR_INVALID_PARAM： invalid param
  * @note This function will send the add action to the event task, the callback will be added at the task loop.
  */

int wm_event_add_callback(wm_event_group_t group, int event, wm_event_callback callback, void *priv);

/**
  * @brief  remove callback,all params need same to wm_event_add_callback
  *
  * @param[in] group  event group,module define by WM_EVENT_DEFINE_GROUP,
  *                    WM_EVENT_ANY_GROUP for filter all group
  * @param[in] event  event type,define by modules
  *                    WM_EVENT_ANY_EVENT for all event in group
  * @param[in] callback  user callback function
  * @param[in] priv  user private data
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_FAILED: failed
  *    - WM_ERR_INVALID_PARAM： invalid param
  * @note This function will send the remove action to the event task, The remove operation will
  * be executed in the task loop, so the function will return WM_SUCCESS even if the callback does not exist.
  */
int wm_event_remove_callback(wm_event_group_t group, int event, wm_event_callback callback, void *priv);

/**
  * @brief  send event to event queue, it will be process by event task
  *
  * @param[in] group  event group, defined by WM_EVENT_DEFINE_GROUP,Do not directly use the char * variable
  * @param[in] event  event type
  * @param[in] data  pointer to event data will be send
  * @param[in] size  data size
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_FAILED: failed
  *    - WM_ERR_INVALID_PARAM： invalid param
  */
int wm_event_send(wm_event_group_t group, int event, void *data, size_t size);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_EVENT_H__ */

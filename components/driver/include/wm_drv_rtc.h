/**
 * @file wm_drv_rtc.h
 *
 * @brief DRV_RTC Module
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

#ifndef __WM_DRV_RTC_H__
#define __WM_DRV_RTC_H__

#include <time.h>

#include "wm_soc_cfgs.h"
#include "wm_types.h"
#include "wm_dt.h"

#include "wm_hal_rtc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_DRV_RTC_Enumerations WM DRV_RTC Enumerations
 * @brief WinnerMicro DRV_RTC Enumerations
 */

/**
 * @addtogroup WM_DRV_RTC_Enumerations
 * @{
 */

/**
 * @brief RTC alarm mask enum
 */

typedef enum {
    WM_RTC_ALARM_TIME_MASK_SECOND   = (1 << 0), /**< alarm trigger ignore seconds */
    WM_RTC_ALARM_TIME_MASK_MINUTE   = (1 << 1), /**< alarm trigger ignore minutes */
    WM_RTC_ALARM_TIME_MASK_HOUR     = (1 << 2), /**< alarm trigger ignore hours */
    WM_RTC_ALARM_TIME_MASK_MONTHDAY = (1 << 3), /**< alarm trigger ignore day of the month*/
    WM_RTC_ALARM_TIME_MASK_MONTH    = (1 << 4), /**< alarm trigger ignore month */
    WM_RTC_ALARM_TIME_MASK_YEAR     = (1 << 5), /**< alarm trigger ignore year */
    WM_RTC_ALARM_TIME_MASK_WEEKDAY  = (1 << 6), /**< alarm trigger ignore weekday */
    WM_RTC_ALARM_TIME_MASK_YEARDAY  = (1 << 7), /**< alarm trigger ignore day of the year */
    WM_RTC_ALARM_TIME_MASK_NSEC     = (1 << 8), /**< alarm trigger ignore nanosecond */
} wm_rtc_alarm_mask_e;

/**
 * @}
 */

/**
 * @defgroup WM_DRV_RTC_Type_Definitions WM DRV_RTC Type Definitions
 * @brief WinnerMicro DRV_RTC Type Definitions
 */

/**
 * @addtogroup WM_DRV_RTC_Type_Definitions
 * @{
 */

/**
 * @brief RTC driver time structure
 */
typedef wm_hal_rtc_time_t wm_drv_rtc_time_t;

/**
 * @brief Prototype of RTC driver callback function
 */
typedef void (*wm_rtc_callback_t)(void *user_data);

/**
 * @}
 */

/**
 * @defgroup WM_DRV_RTC_Functions WM DRV_RTC Functions
 * @brief WinnerMicro DRV_RTC Functions
 */

/**
 * @addtogroup WM_DRV_RTC_Functions
 * @{
 */

/**
 * @brief  Initialize RTC driver
 *
 * @param     dev_name  RTC device name
 *
 * @return
  *    - NULL: failed
  *    - others: succeed
 *
 * @note This API must be called before all other RTC API can be called
 *
 */
wm_device_t *wm_drv_rtc_init(const char *dev_name);

/**
 * @brief     Deinitialization RTC driver
 *
 * @param     dev  RTC driver device
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 */
int wm_drv_rtc_deinit(wm_device_t *dev);

/**
 * @brief     Set RTC calendar
 *
 * @param     dev  RTC driver device
 * @param     tm   Calendar time
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 *
 * @note W800 calendar time input Range 2000.01.01 0:0:0 ~ 2127:12.31 23:59:59
 * @note Set RTC calendar will cancel RTC alarm
 *
 */
int wm_drv_rtc_set_time(wm_device_t *dev, struct tm *tm);

/**
 * @brief      Get RTC calendar
 *
 * @param      dev  RTC driver device
 * @param[out] tm   Calendar time write back address
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 */
int wm_drv_rtc_get_time(wm_device_t *dev, struct tm *tm);

/**
 * @brief      Get RTC alarm supported mask
 *
 * @param      dev  RTC driver device
 * @param      id   RTC alarm index number, range is 0 ~ WM_DRV_RTC_ALARM_NUMBER
 * @param[out] mask RTC alarm supported mask @ wm_rtc_alarm_mask_e
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 */
int wm_drv_rtc_get_alarm_supported_mask(wm_device_t *dev, int id, int *mask);

/**
 * @brief      Get available RTC alarm ID, mean this alarm is in idle state
 *
 * @param      dev  RTC driver device
 * @param[out] id   available RTC alarm ID
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 */
int wm_drv_rtc_get_alarm_available_id(wm_device_t *dev, int *id);

/**
 * @brief      Set RTC alarm time
 *
 * @param      dev  RTC driver device
 * @param      id   RTC alarm index number, range is 0 ~ WM_DRV_RTC_ALARM_NUMBER
 * @param      mask RTC alarm mask @ wm_rtc_alarm_mask_e
 * @param      tm   RTC alarm time
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 *
 * @note Set calendar time will clear alarm
 * @note The alarm time must be later than the current time
 * @note W800 alarm time input Range 2000.01.01 0:0:0 ~ 2127:12.31 23:59:59
 *
 */
int wm_drv_rtc_set_alarm(wm_device_t *dev, int id, int mask, struct tm *tm);

/**
 * @brief      Get RTC alarm time
 *
 * @param      dev  RTC driver device
 * @param      id   RTC alarm index number, range is 0 ~ WM_DRV_RTC_ALARM_NUMBER
 * @param[out] mask RTC alarm mask write back address
 * @param[out] tm   RTC alarm time write back address
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 */
int wm_drv_rtc_get_alarm(wm_device_t *dev, int id, int *mask, struct tm *tm);

/**
 * @brief      Abort RTC alarm
 *
 * @param      dev  RTC driver device
 * @param      id   RTC alarm index number, range is 0 ~ WM_DRV_RTC_ALARM_NUMBER
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 */
int wm_drv_rtc_abort_alarm(wm_device_t *dev, int id);

/**
 * @brief      Register RTC alarm callback function
 *
 * @param      dev  RTC driver device
 * @param      id   RTC alarm index number, range is 0 ~ WM_DRV_RTC_ALARM_NUMBER
 * @param      callback RTC alarm callback function
 * @param      user_data User data for RTC alarm callback function
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 *
 * @attention  As the callback is invoked in the ISR handler, it is not suitable to use any blocking operation
 *      or logic that will take more time in the callback, to avoid other ISRs from not being triggered in time.
 *      It is suggested to send an event to the user application task in this callback if there is any blocking operation.
 *
 */
int wm_drv_rtc_register_alarm_callback(wm_device_t *dev, int id, wm_rtc_callback_t callback, void *user_data);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_DRV_RTC_H__ */

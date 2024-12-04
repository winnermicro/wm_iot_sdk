/**
 * @file wm_hal_rtc.h
 *
 * @brief HAL_RTC Module
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

#ifndef __WM_HAL_RTC_H__
#define __WM_HAL_RTC_H__

#include "wm_soc_cfgs.h"
#include "wm_types.h"

#include "wm_hal_irq.h"
#include "wm_ll_rtc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_HAL_RTC_Type_Definitions WM HAL_RTC Type Definitions
 * @brief WinnerMicro HAL_RTC Type Definitions
 */

/**
 * @addtogroup WM_HAL_RTC_Type_Definitions
 * @{
 */

/**
 * @brief Prototype of RTC HAL callback function
 */
typedef void (*hal_rtc_callback_t)(void *);

/**
 * @}
 */

/**
 * @defgroup WM_HAL_RTC_Structures WM HAL_RTC Structures
 * @brief WinnerMicro HAL_RTC Structures
 */

/**
 * @addtogroup WM_HAL_RTC_Structures
 * @{
 */

/**
 * @brief HAL RTC time structure
 */

typedef struct {
    int tm_sec;   /**< Seconds, normal range from 0 to 59 */
    int tm_min;   /**< Minute, normal range from 0 to 59 */
    int tm_hour;  /**< Hours, range from 0 to 23 */
    int tm_mday;  /**< On the day of the month, ranging from 1 to 31 */
    int tm_mon;   /**< Month, from 0 (January) to 11 (December) */
    int tm_year;  /**< Years Since 1900 */
    int tm_wday;  /**< Day of the week [0, 6] (Sunday = 0) (Unknown = -1) */
    int tm_yday;  /**< Day of the year [0, 365] (Unknown = -1) */
    int tm_isdst; /**< Daylight saving time flag [-1] (Unknown = -1) */
} wm_hal_rtc_time_t;

/**
 * @brief HAL RTC HAL device structure
 */

typedef struct {
    bool is_init;           /**< rtc is already initialized? */
    wm_irq_no_t irq_num;    /**< interrupt number for RTC module */
    wm_rtc_reg_t *reg_base; /**< RTC controller reg base addr info from device table */
    hal_rtc_callback_t cb;  /**< interrupt callback function */
    void *user_data;        /**< used for interrupt callback function */
} wm_hal_rtc_dev_t;

/**
 * @}
 */

/**
 * @defgroup WM_HAL_RTC_Functions WM HAL_RTC Functions
 * @brief WinnerMicro HAL_RTC Functions
 */

/**
 * @addtogroup WM_HAL_RTC_Functions
 * @{
 */

/**
 * @brief  Initialize RTC hardware
 *
 * @param     dev  RTC HAL device
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 */
int wm_hal_rtc_init(wm_hal_rtc_dev_t *dev);

/**
 * @brief  Deinitialization RTC hardware
 *
 * @param     dev  RTC HAL device
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 */
int wm_hal_rtc_deinit(wm_hal_rtc_dev_t *dev);

/**
 * @brief  Set RTC hardware calendar time
 *
 * @param     dev   RTC HAL device
 * @param     time  Calendar time @ wm_hal_rtc_time_t
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 */
int wm_hal_rtc_set_time(wm_hal_rtc_dev_t *dev, wm_hal_rtc_time_t *time);

/**
 * @brief  Get RTC hardware calendar time
 *
 * @param       dev   RTC HAL device
 * @param[out]  time  Calendar time write back address
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 */
int wm_hal_rtc_get_time(wm_hal_rtc_dev_t *dev, wm_hal_rtc_time_t *time);

/**
 * @brief  Set RTC hardware alarm time
 *
 * @param    dev   RTC HAL device
 * @param    time  Calendar time
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 */
int wm_hal_rtc_set_alarm(wm_hal_rtc_dev_t *dev, wm_hal_rtc_time_t *time);

/**
 * @brief  Abort RTC hardware alarm time
 *
 * @param    dev   RTC HAL device
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 */
int wm_hal_rtc_abort_alarm(wm_hal_rtc_dev_t *dev);

/**
 * @brief  Register RTC hardware alarm callback function
 *
 * @param    dev   RTC HAL device
 * @param    call_back  RTC HAL alarm callback function
 * @param    user_data  RTC HAL alarm callback function parameters
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 */
int wm_hal_rtc_register_callback(wm_hal_rtc_dev_t *dev, hal_rtc_callback_t call_back, void *user_data);

/**
 * @brief  Get RTC hardware alarm supported mask
 *
 * @param       dev   RTC HAL device
 * @param[out]  mask  RTC HAL alarm mask write back address
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - others: failed
 */
int wm_hal_rtc_get_supported_mask(wm_hal_rtc_dev_t *dev, int *mask);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_HAL_RTC_H__ */

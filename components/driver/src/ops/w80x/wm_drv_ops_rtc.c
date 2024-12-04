/**
 * @file wm_drv_ops_rtc.c
 *
 * @brief DRV_RTC Module
 *
 */

/**
 *  Copyright 2022-2024 Beijing WinnerMicroelectronics Co.,Ltd.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"){}
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
#include "wmsdk_config.h"
#include <string.h>
#include "wm_types.h"
#include "wm_error.h"
#include "wm_osal.h"

#include "wm_hal_rtc.h"
#include "wm_drv_pmu.h"

#define WM_DRV_RTC_ALARM_NUMBER 1
#define WM_DRV_RTC_DEBUG

#define STD_TIME_START_YEAR     (1900)
#define WM_RTC_TIME_START_YEAR  (2000) // HW limitations, since 2000.
#define WM_RTC_TIME_MAX_YEAR    (2127) // 7 bits for RTC year

#define STD_TIME_START_MONTH    (0)
#define WM_RTC_TIME_START_MONTH (1) // HW limitations, month is since from 1.

#ifdef WM_DRV_RTC_DEBUG

#include <assert.h>
#define LOG_TAG "RTC_DRV"
#include "wm_log.h"
#define WM_DRV_RTC_LOG_D(...)   wm_log_debug(__VA_ARGS__)
#define WM_DRV_RTC_LOG_I(...)   wm_log_info(__VA_ARGS__)
#define WM_DRV_RTC_LOG_E(...)   wm_log_error(__VA_ARGS__)
#define WM_DRV_RTC_LOG_MARKER() wm_log_error("%s@%d\n", __func__, __LINE__)
#define WM_DRV_RTC_ASSERT(v)    assert(v)

#else

#define WM_DRV_RTC_LOG_D(...)
#define WM_DRV_RTC_LOG_I(...)
#define WM_DRV_RTC_LOG_E(...)
#define WM_DRV_RTC_ASSERT(...)
#define WM_DRV_RTC_LOG_MARKER()

#endif

typedef struct {
    /* Basic */
    int (*init)(wm_device_t *dev);
    int (*deinit)(wm_device_t *dev);
    int (*set_time)(wm_device_t *dev, wm_drv_rtc_time_t *time);
    int (*get_time)(wm_device_t *dev, wm_drv_rtc_time_t *time);
    /* Alarm */
    int (*get_alarm_supported_mask)(wm_device_t *dev, int id, int *mask);
    int (*get_alarm_available_id)(wm_device_t *dev, int *id);
    int (*register_alarm_callback)(wm_device_t *dev, int id, wm_rtc_callback_t callback, void *user_data);
    int (*set_alarm)(wm_device_t *dev, int id, int mask, wm_drv_rtc_time_t *time);
    int (*get_alarm)(wm_device_t *dev, int id, int *mask, wm_drv_rtc_time_t *time);
    int (*abort_alarm)(wm_device_t *dev, int id);
} wm_drv_rtc_ops_t;

typedef struct {
    wm_os_mutex_t *mutex;
    struct {
        bool active;
        wm_drv_rtc_time_t time;
        wm_rtc_callback_t cb;
        void *user_data;
    } alarm[WM_DRV_RTC_ALARM_NUMBER];
} wm_drv_rtc_ctx_t;

typedef struct {
    wm_hal_rtc_dev_t hal_rtc;
    wm_drv_rtc_ctx_t drv_ctx;
} wm_drv_rtc_data_t;

static bool wm_drv_time_is_available(wm_drv_rtc_time_t *tm)
{
    if (tm == NULL) {
        return false;
    }

    if (tm->tm_year < (WM_RTC_TIME_START_YEAR - STD_TIME_START_YEAR) ||
        tm->tm_year > (WM_RTC_TIME_MAX_YEAR - STD_TIME_START_YEAR)) {
        return false;
    }

    if (tm->tm_mon < 0 || tm->tm_mon > 11) {
        return false;
    }

    if (tm->tm_mday <= 0 || tm->tm_mday > 31) {
        return false;
    }

    switch (tm->tm_mon) {
        case 1: //February
        {
            int year = tm->tm_year + STD_TIME_START_YEAR;
            if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) {
                if (tm->tm_mday > 29) {
                    return false;
                }
            } else if (tm->tm_mday > 28) {
                return false;
            }
            break;
        }
        case 3:  //March
        case 5:  //June
        case 8:  //September
        case 10: //November
        {
            if (tm->tm_mday > 30) {
                return false;
            }
            break;
        }
        default:
        {
            break;
        }
    }

    if (tm->tm_hour < 0 || tm->tm_hour > 23) {
        return false;
    }

    if (tm->tm_min < 0 || tm->tm_min > 59) {
        return false;
    }

    if (tm->tm_sec < 0 || tm->tm_sec > 59) {
        return false;
    }

    return true;
}

// 0: same, -1: time1 before time2, 1: time1 after time2
static int wm_drv_rtc_time_compare(wm_drv_rtc_time_t *t1, wm_drv_rtc_time_t *t2)
{
    if (t1->tm_year != t2->tm_year) {
        return (t1->tm_year < t2->tm_year) ? -1 : 1;
    }
    if (t1->tm_mon != t2->tm_mon) {
        return (t1->tm_mon < t2->tm_mon) ? -1 : 1;
    }
    if (t1->tm_mday != t2->tm_mday) {
        return (t1->tm_mday < t2->tm_mday) ? -1 : 1;
    }
    if (t1->tm_hour != t2->tm_hour) {
        return (t1->tm_hour < t2->tm_hour) ? -1 : 1;
    }
    if (t1->tm_min != t2->tm_min) {
        return (t1->tm_min < t2->tm_min) ? -1 : 1;
    }
    if (t1->tm_sec != t2->tm_sec) {
        return (t1->tm_sec < t2->tm_sec) ? -1 : 1;
    }
    return 0;
}

/* Reason: W800 RTC hardware does not match with <time.h> */
static void wm_800_rtc_std_time_conversion(bool dir, wm_drv_rtc_time_t *time)
{
    if (dir) {
        //std time -> w800 time
        time->tm_year -= (WM_RTC_TIME_START_YEAR - STD_TIME_START_YEAR);
        time->tm_mon -= (STD_TIME_START_MONTH - WM_RTC_TIME_START_MONTH);
    } else {
        //w800 time -> std time
        time->tm_year += (WM_RTC_TIME_START_YEAR - STD_TIME_START_YEAR);
        time->tm_mon += (STD_TIME_START_MONTH - WM_RTC_TIME_START_MONTH);
    }
}

static void wm_800_rtc_callback(void *user_data)
{
    wm_drv_rtc_time_t time     = { 0 };
    wm_drv_rtc_data_t *dev_rtc = (wm_drv_rtc_data_t *)user_data;

    WM_DRV_RTC_ASSERT(dev_rtc != NULL);

    if (wm_hal_rtc_get_time(&dev_rtc->hal_rtc, (wm_hal_rtc_time_t *)&time) != WM_ERR_SUCCESS) {
        WM_DRV_RTC_LOG_MARKER();
        return;
    }

    for (int i = 0; i < WM_DRV_RTC_ALARM_NUMBER; i++) {
        if (dev_rtc->drv_ctx.alarm[i].active) {
            if (wm_drv_rtc_time_compare(&dev_rtc->drv_ctx.alarm[i].time, &time) > 0) {
                continue; // try next alarm
            }
            dev_rtc->drv_ctx.alarm[i].active = false;

            // this alarm is triggered
            if (dev_rtc->drv_ctx.alarm[i].cb != NULL) {
                dev_rtc->drv_ctx.alarm[i].cb(dev_rtc->drv_ctx.alarm[i].user_data);
            }
        }
    }
}

/// Todo: temp method, later modified to interrupt registration with IRQ_HAL
static int wm_w800_drv_rtc_regist_temp_function(wm_drv_rtc_data_t *dev_rtc)
{
    wm_device_t *dev_pmu = wm_drv_pmu_init("pmu");

    if (dev_pmu == NULL) {
        dev_pmu = wm_dt_get_device_by_name("pmu");
    }

    if (dev_pmu == NULL) {
        wm_log_error("get pmu device failed");
        return WM_ERR_FAILED;
    }

    if (wm_drv_pmu_register_irq_callback(dev_pmu, WM_PMU_IRQ_RTC, wm_800_rtc_callback, dev_rtc) != WM_ERR_SUCCESS) {
        wm_log_error("reg RTC callback failed");
        return WM_ERR_FAILED;
    }

    return WM_ERR_SUCCESS;
}

static int wm_w800_drv_rtc_init(wm_device_t *dev)
{
    wm_drv_rtc_data_t *dev_rtc = NULL;
    wm_dt_hw_rtc_t *hw         = (wm_dt_hw_rtc_t *)dev->hw;

    dev_rtc = (wm_drv_rtc_data_t *)dev->drv;

    if (dev->state == WM_DEV_ST_INITED) {
        return WM_ERR_ALREADY_INITED; //already initialized
    }

    dev_rtc = wm_os_internal_calloc(sizeof(wm_drv_rtc_data_t), 1);
    if (dev_rtc == NULL) {
        return WM_ERR_NO_MEM;
    }

    //create mutex
    if (wm_os_internal_mutex_create(&dev_rtc->drv_ctx.mutex) != WM_OS_STATUS_SUCCESS) {
        WM_DRV_RTC_LOG_MARKER();
        goto error;
    }

    if (wm_os_internal_mutex_acquire(dev_rtc->drv_ctx.mutex, WM_OS_WAIT_TIME_MAX) != WM_OS_STATUS_SUCCESS) {
        WM_DRV_RTC_LOG_MARKER();
        goto error;
    }

    dev_rtc->hal_rtc.reg_base = (wm_rtc_reg_t *)hw->reg_base;
    dev_rtc->hal_rtc.irq_num  = hw->irq_cfg.irq_num;

    if (wm_hal_rtc_init(&dev_rtc->hal_rtc) != WM_ERR_SUCCESS) {
        wm_os_internal_mutex_release(dev_rtc->drv_ctx.mutex);
        WM_DRV_RTC_LOG_MARKER();
        goto error;
    }

    /// Todo: temp method, will be rewritten
    if (wm_w800_drv_rtc_regist_temp_function(dev_rtc) != WM_ERR_SUCCESS) {
        wm_os_internal_mutex_release(dev_rtc->drv_ctx.mutex);
        WM_DRV_RTC_LOG_MARKER();
        goto error;
    }

    dev->drv   = dev_rtc;
    dev->state = WM_DEV_ST_INITED;
    wm_os_internal_mutex_release(dev_rtc->drv_ctx.mutex);

    return WM_ERR_SUCCESS;

error:

    if (dev_rtc->drv_ctx.mutex != NULL) {
        wm_os_internal_mutex_delete(dev_rtc->drv_ctx.mutex);
    }
    if (dev_rtc != NULL) {
        wm_os_internal_free(dev_rtc);
    }
    dev->drv = NULL;
    WM_DRV_RTC_LOG_E("RTC init fail");
    return WM_ERR_FAILED;
}

static int wm_w800_drv_rtc_deinit(wm_device_t *dev)
{
    WM_DRV_RTC_ASSERT(dev != NULL);

    wm_drv_rtc_data_t *dev_rtc = (wm_drv_rtc_data_t *)dev->drv;

    if (dev->state != WM_DEV_ST_INITED) {
        WM_DRV_RTC_LOG_MARKER();
        return WM_ERR_NO_INITED;
    }

    if (wm_os_internal_mutex_acquire(dev_rtc->drv_ctx.mutex, WM_OS_WAIT_TIME_MAX) != WM_OS_STATUS_SUCCESS) {
        WM_DRV_RTC_LOG_MARKER();
        return WM_ERR_NO_INITED;
    }

    if (wm_hal_rtc_deinit(&dev_rtc->hal_rtc) != WM_ERR_SUCCESS) {
        wm_os_internal_mutex_release(dev_rtc->drv_ctx.mutex);
        WM_DRV_RTC_LOG_MARKER();
        return WM_ERR_FAILED;
    }

    dev->drv = NULL;

    dev->state = WM_DEV_ST_UNINIT;
    wm_os_internal_mutex_release(dev_rtc->drv_ctx.mutex);
    wm_os_internal_mutex_delete(dev_rtc->drv_ctx.mutex);

    memset(dev_rtc, 0, sizeof(wm_drv_rtc_data_t));
    wm_os_internal_free(dev_rtc);

    return WM_ERR_SUCCESS;
}

static int wm_w800_drv_rtc_set_time(wm_device_t *dev, wm_drv_rtc_time_t *time)
{
    WM_DRV_RTC_ASSERT(dev != NULL);

    wm_drv_rtc_data_t *dev_rtc = (wm_drv_rtc_data_t *)dev->drv;

    if (dev_rtc == NULL || !wm_drv_time_is_available(time)) {
        WM_DRV_RTC_LOG_MARKER();
        return WM_ERR_INVALID_PARAM;
    }

    wm_800_rtc_std_time_conversion(true, time);

    if (wm_os_internal_mutex_acquire(dev_rtc->drv_ctx.mutex, WM_OS_WAIT_TIME_MAX) != WM_OS_STATUS_SUCCESS) {
        WM_DRV_RTC_LOG_MARKER();
        return WM_ERR_FAILED;
    }

    if (wm_hal_rtc_set_time(&dev_rtc->hal_rtc, (wm_hal_rtc_time_t *)time) != WM_ERR_SUCCESS) {
        wm_os_internal_mutex_release(dev_rtc->drv_ctx.mutex);
        WM_DRV_RTC_LOG_MARKER();
        return WM_ERR_FAILED;
    }

    /** Set RTC time will cancel RTC alarm by HW */
    for (int i = 0; i < WM_DRV_RTC_ALARM_NUMBER; i++) {
        dev_rtc->drv_ctx.alarm[i].active = false;
    }

    wm_os_internal_mutex_release(dev_rtc->drv_ctx.mutex);

    return WM_ERR_SUCCESS;
}

static int wm_w800_drv_rtc_get_time(wm_device_t *dev, wm_drv_rtc_time_t *time)
{
    WM_DRV_RTC_ASSERT(dev != NULL);

    wm_drv_rtc_data_t *dev_rtc = (wm_drv_rtc_data_t *)dev->drv;

    if (dev_rtc == NULL) {
        WM_DRV_RTC_LOG_MARKER();
        return WM_ERR_INVALID_PARAM;
    }

    if (wm_os_internal_mutex_acquire(dev_rtc->drv_ctx.mutex, WM_OS_WAIT_TIME_MAX) != WM_OS_STATUS_SUCCESS) {
        WM_DRV_RTC_LOG_MARKER();
        return WM_ERR_FAILED;
    }

    if (wm_hal_rtc_get_time(&dev_rtc->hal_rtc, (wm_hal_rtc_time_t *)time) != WM_ERR_SUCCESS) {
        wm_os_internal_mutex_release(dev_rtc->drv_ctx.mutex);
        WM_DRV_RTC_LOG_MARKER();
        return WM_ERR_FAILED;
    }

    wm_os_internal_mutex_release(dev_rtc->drv_ctx.mutex);

    wm_800_rtc_std_time_conversion(false, time);

    time->tm_wday  = -1;
    time->tm_yday  = -1;
    time->tm_isdst = -1;

    return WM_ERR_SUCCESS;
}

static int wm_w800_drv_rtc_get_supported_mask(wm_device_t *dev, int id, int *mask)
{
    WM_DRV_RTC_ASSERT(dev != NULL);

    wm_drv_rtc_data_t *dev_rtc = (wm_drv_rtc_data_t *)dev->drv;

    if (dev_rtc == NULL || id >= WM_DRV_RTC_ALARM_NUMBER || id < 0 || mask == NULL) {
        WM_DRV_RTC_LOG_MARKER();
        return WM_ERR_INVALID_PARAM;
    }

    if (wm_os_internal_mutex_acquire(dev_rtc->drv_ctx.mutex, WM_OS_WAIT_TIME_MAX) != WM_OS_STATUS_SUCCESS) {
        WM_DRV_RTC_LOG_MARKER();
        return WM_ERR_FAILED;
    }

    if (wm_hal_rtc_get_supported_mask(&dev_rtc->hal_rtc, mask) != WM_ERR_SUCCESS) {
        wm_os_internal_mutex_release(dev_rtc->drv_ctx.mutex);
        WM_DRV_RTC_LOG_MARKER();
        return WM_ERR_FAILED;
    }

    wm_os_internal_mutex_release(dev_rtc->drv_ctx.mutex);

    return WM_ERR_SUCCESS;
}

static int wm_w800_drv_rtc_get_alarm_available_id(wm_device_t *dev, int *id)
{
    int available_id = -1;

    WM_DRV_RTC_ASSERT(dev != NULL);

    wm_drv_rtc_data_t *dev_rtc = (wm_drv_rtc_data_t *)dev->drv;

    if (dev_rtc == NULL || id == NULL) {
        WM_DRV_RTC_LOG_MARKER();
        return WM_ERR_INVALID_PARAM;
    }

    if (wm_os_internal_mutex_acquire(dev_rtc->drv_ctx.mutex, WM_OS_WAIT_TIME_MAX) != WM_OS_STATUS_SUCCESS) {
        WM_DRV_RTC_LOG_MARKER();
        return WM_ERR_FAILED;
    }

    for (int i = 0; i < WM_DRV_RTC_ALARM_NUMBER; i++) {
        if (!dev_rtc->drv_ctx.alarm[i].active) {
            available_id = i;
            break;
        }
    }

    wm_os_internal_mutex_release(dev_rtc->drv_ctx.mutex);

    if (available_id >= 0) {
        *id = available_id;
        return WM_ERR_SUCCESS;
    }
    return WM_ERR_FAILED;
}

static int wm_w800_drv_rtc_register_alarm_callback(wm_device_t *dev, int id, wm_rtc_callback_t callback, void *user_data)
{
    WM_DRV_RTC_ASSERT(dev != NULL);

    wm_drv_rtc_data_t *dev_rtc = (wm_drv_rtc_data_t *)dev->drv;

    if (dev_rtc == NULL || id >= WM_DRV_RTC_ALARM_NUMBER || id < 0 || callback == NULL) {
        WM_DRV_RTC_LOG_MARKER();
        return WM_ERR_INVALID_PARAM;
    }

    if (wm_os_internal_mutex_acquire(dev_rtc->drv_ctx.mutex, WM_OS_WAIT_TIME_MAX) != WM_OS_STATUS_SUCCESS) {
        WM_DRV_RTC_LOG_MARKER();
        return WM_ERR_FAILED;
    }
    dev_rtc->drv_ctx.alarm[id].cb        = callback;
    dev_rtc->drv_ctx.alarm[id].user_data = user_data;
    wm_os_internal_mutex_release(dev_rtc->drv_ctx.mutex);

    return WM_ERR_SUCCESS;
}

static int wm_w800_drv_rtc_set_alarm(wm_device_t *dev, int id, int mask, wm_drv_rtc_time_t *time)
{
    int supported_mask         = 0;
    wm_drv_rtc_time_t now_time = { 0 };

    WM_DRV_RTC_ASSERT(dev != NULL);

    wm_drv_rtc_data_t *dev_rtc = (wm_drv_rtc_data_t *)dev->drv;

    if (dev_rtc == NULL || id >= WM_DRV_RTC_ALARM_NUMBER || id < 0 || !wm_drv_time_is_available(time)) {
        WM_DRV_RTC_LOG_MARKER();
        return WM_ERR_INVALID_PARAM;
    }

    wm_800_rtc_std_time_conversion(true, time);

    if (wm_os_internal_mutex_acquire(dev_rtc->drv_ctx.mutex, WM_OS_WAIT_TIME_MAX) != WM_OS_STATUS_SUCCESS) {
        WM_DRV_RTC_LOG_MARKER();
        return WM_ERR_FAILED;
    }

    if (wm_hal_rtc_get_supported_mask(&dev_rtc->hal_rtc, &supported_mask) != WM_ERR_SUCCESS) {
        wm_os_internal_mutex_release(dev_rtc->drv_ctx.mutex);
        WM_DRV_RTC_LOG_MARKER();
        return WM_ERR_FAILED;
    }

    if (mask & (~supported_mask)) {
        wm_os_internal_mutex_release(dev_rtc->drv_ctx.mutex);
        WM_DRV_RTC_LOG_MARKER();
        return WM_ERR_FAILED;
    }

    if (wm_hal_rtc_get_time(&dev_rtc->hal_rtc, (wm_hal_rtc_time_t *)&now_time) != WM_ERR_SUCCESS) {
        wm_os_internal_mutex_release(dev_rtc->drv_ctx.mutex);
        WM_DRV_RTC_LOG_MARKER();
        return WM_ERR_FAILED;
    }

    // The alarm time must be later than the current time
    if (wm_drv_rtc_time_compare(time, &now_time) <= 0) {
        wm_os_internal_mutex_release(dev_rtc->drv_ctx.mutex);
        WM_DRV_RTC_LOG_MARKER();
        return WM_ERR_FAILED;
    }

    memcpy(&dev_rtc->drv_ctx.alarm[id].time, time, sizeof(*time));
    if (wm_hal_rtc_set_alarm(&dev_rtc->hal_rtc, &dev_rtc->drv_ctx.alarm[id].time) != WM_ERR_SUCCESS) {
        wm_os_internal_mutex_release(dev_rtc->drv_ctx.mutex);
        WM_DRV_RTC_LOG_MARKER();
        return WM_ERR_FAILED;
    }

    dev_rtc->drv_ctx.alarm[id].active = true;
    wm_os_internal_mutex_release(dev_rtc->drv_ctx.mutex);

    return WM_ERR_SUCCESS;
}

static int wm_w800_drv_rtc_get_alarm(wm_device_t *dev, int id, int *mask, wm_drv_rtc_time_t *time)
{
    WM_DRV_RTC_ASSERT(dev != NULL);

    wm_drv_rtc_data_t *dev_rtc = (wm_drv_rtc_data_t *)dev->drv;

    if (dev_rtc == NULL || id >= WM_DRV_RTC_ALARM_NUMBER || id < 0 || time == NULL) {
        WM_DRV_RTC_LOG_MARKER();
        return WM_ERR_INVALID_PARAM;
    }

    if (wm_os_internal_mutex_acquire(dev_rtc->drv_ctx.mutex, WM_OS_WAIT_TIME_MAX) != WM_OS_STATUS_SUCCESS) {
        WM_DRV_RTC_LOG_MARKER();
        return WM_ERR_FAILED;
    }
    if (!dev_rtc->drv_ctx.alarm[id].active) {
        wm_os_internal_mutex_release(dev_rtc->drv_ctx.mutex); //RTC alarm is not active
        return WM_ERR_FAILED;
    }

    memcpy(time, &dev_rtc->drv_ctx.alarm[id].time, sizeof(*time));

    if (wm_hal_rtc_get_supported_mask(&dev_rtc->hal_rtc, mask) != WM_ERR_SUCCESS) {
        wm_os_internal_mutex_release(dev_rtc->drv_ctx.mutex);
        WM_DRV_RTC_LOG_MARKER();
        return WM_ERR_FAILED;
    }

    wm_800_rtc_std_time_conversion(false, time);

    wm_os_internal_mutex_release(dev_rtc->drv_ctx.mutex);

    return WM_ERR_SUCCESS;
}

static int wm_w800_drv_rtc_abort_alarm(wm_device_t *dev, int id)
{
    WM_DRV_RTC_ASSERT(dev != NULL);

    wm_drv_rtc_data_t *dev_rtc = (wm_drv_rtc_data_t *)dev->drv;

    if (dev_rtc == NULL || id >= WM_DRV_RTC_ALARM_NUMBER || id < 0) {
        WM_DRV_RTC_LOG_MARKER();
        return WM_ERR_INVALID_PARAM;
    }

    if (wm_os_internal_mutex_acquire(dev_rtc->drv_ctx.mutex, WM_OS_WAIT_TIME_MAX) != WM_OS_STATUS_SUCCESS) {
        WM_DRV_RTC_LOG_MARKER();
        return WM_ERR_FAILED;
    }

    dev_rtc->drv_ctx.alarm[id].active = false;
    (void)wm_hal_rtc_abort_alarm(&dev_rtc->hal_rtc);

    wm_os_internal_mutex_release(dev_rtc->drv_ctx.mutex);

    return WM_ERR_SUCCESS;
}

/* RTC driver operations */
const wm_drv_rtc_ops_t wm_drv_rtc_ops = {
    /* Basic */
    .init     = wm_w800_drv_rtc_init,
    .deinit   = wm_w800_drv_rtc_deinit,
    .set_time = wm_w800_drv_rtc_set_time,
    .get_time = wm_w800_drv_rtc_get_time,
    /* Alarm */
    .get_alarm_supported_mask = wm_w800_drv_rtc_get_supported_mask,
    .get_alarm_available_id   = wm_w800_drv_rtc_get_alarm_available_id,
    .register_alarm_callback  = wm_w800_drv_rtc_register_alarm_callback,
    .set_alarm                = wm_w800_drv_rtc_set_alarm,
    .get_alarm                = wm_w800_drv_rtc_get_alarm,
    .abort_alarm              = wm_w800_drv_rtc_abort_alarm,
};

/**
 * @file pwm_drv_demo.c
 *
 * @brief PWM DRV demo Module
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

#include <stdint.h>
#include <string.h>
#include <time.h>

#include "wm_types.h"
#include "wm_error.h"
#include "wm_event.h"
#include "wm_drv_rtc.h"

#define LOG_TAG "rtc_example"
#include "wm_log.h"

#define WM_RTC_NEXT_ALARM 5            // The next alarm will be in 5 seconds
WM_EVENT_DEFINE_GROUP(WM_RTC_EXAMPLE); /**< Define rtc example event group */

typedef enum {
    WM_DRV_RTC_ALARM = 0,
} wm_drv_rtc_event_e;

static void wm_rtc_time_example(void)
{
    char buffer[32];
    struct tm tm         = { 0 };
    wm_device_t *rtc_dev = wm_dt_get_device_by_name("rtc");

    tm.tm_year = 124; // 2024-07-01 12:00:00
    tm.tm_mon  = 6;
    tm.tm_mday = 1;
    tm.tm_hour = 12;
    tm.tm_min  = 0;
    tm.tm_sec  = 0;

    if (wm_drv_rtc_set_time(rtc_dev, &tm) != WM_ERR_SUCCESS) {
        wm_log_info("set time failed");
    }

    if (wm_drv_rtc_get_time(rtc_dev, &tm) == WM_ERR_SUCCESS) {
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);
        wm_log_info("now is: %s", buffer);
    } else {
        wm_log_info("get time failed");
    }
}

static void wm_rtc_alarm_callback(void *arg)
{
    (void)arg;

    // send rtc alarm event
    if (wm_event_send(WM_RTC_EXAMPLE, WM_DRV_RTC_ALARM, NULL, 0) != WM_ERR_SUCCESS) {
        wm_log_error("send rtc alarm event fail");
        return;
    }
}

static void wm_rtc_alarm_event_handle(wm_event_group_t group, int event, void *data, void *priv)
{
    wm_device_t *rtc_dev;
    struct tm tm = { 0 };
    struct tm *next_tm;
    int alarm_id;
    time_t timestamp;
    char buffer[32];

    rtc_dev = (wm_device_t *)priv;

    if (rtc_dev == NULL) {
        wm_log_error("error in alarm event handle");
        return;
    }

    if (wm_drv_rtc_get_time(rtc_dev, &tm) != WM_ERR_SUCCESS) {
        wm_log_error("get rtc time error");
        return;
    }

    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);
    wm_log_info("alarm arrived: %s", buffer);

    timestamp = mktime(&tm);
    timestamp += WM_RTC_NEXT_ALARM;
    next_tm = localtime(&timestamp);

    if (wm_drv_rtc_get_alarm_available_id(rtc_dev, &alarm_id) != WM_ERR_SUCCESS) {
        wm_log_info("no available alarms");
        return;
    }

    if (wm_drv_rtc_set_alarm(rtc_dev, alarm_id, 0, next_tm) != WM_ERR_SUCCESS) {
        wm_log_info("set alarm failed");
        return;
    }
}

static void wm_rtc_alarm_example(void)
{
    time_t timestamp;
    struct tm *next_tm;
    struct tm tm = { 0 };
    wm_device_t *rtc_dev;
    int alarm_id;

    rtc_dev = wm_dt_get_device_by_name("rtc");

    if (wm_drv_rtc_get_alarm_available_id(rtc_dev, &alarm_id) != WM_ERR_SUCCESS) {
        wm_log_info("no available alarms");
        return;
    }

    if (wm_drv_rtc_register_alarm_callback(rtc_dev, alarm_id, wm_rtc_alarm_callback, rtc_dev) != WM_ERR_SUCCESS) {
        wm_log_info("register alarm callback failed");
        return;
    }

    if (wm_drv_rtc_get_time(rtc_dev, &tm) != WM_ERR_SUCCESS) {
        wm_log_info("get time failed");
        return;
    }

    timestamp = mktime(&tm);
    timestamp += WM_RTC_NEXT_ALARM;
    next_tm = localtime(&timestamp);

    if (wm_drv_rtc_set_alarm(rtc_dev, alarm_id, 0, next_tm) != WM_ERR_SUCCESS) {
        wm_log_info("set alarm failed");
        return;
    }

    // register alarm event callback
    if (wm_event_add_callback(WM_RTC_EXAMPLE, WM_DRV_RTC_ALARM, wm_rtc_alarm_event_handle, rtc_dev) != WM_ERR_SUCCESS) {
        wm_log_info("register alarm event callback failed");
        return;
    }
}

int main(void)
{
    wm_device_t *rtc_dev;

    rtc_dev = wm_drv_rtc_init("rtc"); //init rtc device
    if (rtc_dev == NULL) {
        wm_log_error("rtc init failed");
        return WM_ERR_FAILED;
    }

    wm_rtc_time_example();
    wm_rtc_alarm_example();

    return WM_ERR_SUCCESS;
}

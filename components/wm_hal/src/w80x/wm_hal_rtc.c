/**
 * @file wm_hal_rtc.c
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
#include "wmsdk_config.h"
#include "wm_types.h"
#include "wm_error.h"

#include "wm_hal_irq.h"
#include "wm_hal_rtc.h"
#include "wm_ll_rtc.h"

#define WM_HAL_RTC_DEBUG

#ifdef WM_HAL_RTC_DEBUG

#include <assert.h>
#define LOG_TAG "HAL_RTC"
#include "wm_log.h"
#define WM_HAL_RTC_LOG_D(...)   wm_log_debug(__VA_ARGS__)
#define WM_HAL_RTC_LOG_I(...)   wm_log_info(__VA_ARGS__)
#define WM_HAL_RTC_LOG_E(...)   wm_log_error(__VA_ARGS__)
#define WM_HAL_RTC_LOG_MARKER() wm_log_error("%s@%d\n", __func__, __LINE__)
#define WM_HAL_RTC_ASSERT(v)    assert(v)

#else

#define WM_HAL_RTC_LOG_D(...)
#define WM_HAL_RTC_LOG_I(...)
#define WM_HAL_RTC_LOG_E(...)
#define WM_HAL_RTC_ASSERT(...)
#define WM_HAL_RTC_LOG_MARKER()

#endif

int wm_hal_rtc_init(wm_hal_rtc_dev_t *dev)
{
    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->is_init) {
        return WM_ERR_SUCCESS;
    }

    wm_ll_rtc_timer_counting_enable(dev->reg_base, true);
    wm_ll_rtc_timer_enable(dev->reg_base, false);
    dev->is_init = true;
    return WM_ERR_SUCCESS;
}

int wm_hal_rtc_deinit(wm_hal_rtc_dev_t *dev)
{
    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (!dev->is_init) {
        return WM_ERR_SUCCESS;
    }

    dev->is_init = false;

    wm_ll_rtc_timer_enable(dev->reg_base, false);
    wm_ll_rtc_timer_counting_enable(dev->reg_base, false);
    return WM_ERR_SUCCESS;
}

int wm_hal_rtc_set_time(wm_hal_rtc_dev_t *dev, wm_hal_rtc_time_t *time)
{
    if (dev == NULL || time == NULL || !dev->is_init) {
        return WM_ERR_INVALID_PARAM;
    }

    wm_ll_rtc_timer_counting_enable(dev->reg_base, false);

    wm_ll_rtc_set_year(dev->reg_base, (uint32_t)time->tm_year);
    wm_ll_rtc_set_month(dev->reg_base, (uint32_t)time->tm_mon);
    wm_ll_rtc_set_day(dev->reg_base, (uint32_t)time->tm_mday);
    wm_ll_rtc_set_hour(dev->reg_base, (uint32_t)time->tm_hour);
    wm_ll_rtc_set_minute(dev->reg_base, (uint32_t)time->tm_min);
    wm_ll_rtc_set_second(dev->reg_base, (uint32_t)time->tm_sec);

    wm_ll_rtc_timer_counting_enable(dev->reg_base, true);

    return WM_ERR_SUCCESS;
}

int wm_hal_rtc_get_time(wm_hal_rtc_dev_t *dev, wm_hal_rtc_time_t *time)
{
    if (dev == NULL || time == NULL || !dev->is_init) {
        return WM_ERR_INVALID_PARAM;
    }

    time->tm_year = wm_ll_rtc_get_year(dev->reg_base);
    time->tm_mon  = wm_ll_rtc_get_month(dev->reg_base);
    time->tm_mday = wm_ll_rtc_get_day(dev->reg_base);
    time->tm_hour = wm_ll_rtc_get_hour(dev->reg_base);
    time->tm_min  = wm_ll_rtc_get_minute(dev->reg_base);
    time->tm_sec  = wm_ll_rtc_get_second(dev->reg_base);
    return WM_ERR_SUCCESS;
}

int wm_hal_rtc_set_alarm(wm_hal_rtc_dev_t *dev, wm_hal_rtc_time_t *time)
{
    if (dev == NULL || time == NULL || !dev->is_init) {
        return WM_ERR_INVALID_PARAM;
    }

    // HW limitation: When using alarm, must write word to register
    uint32_t tmp[2]             = { 0 };
    wm_rtc_reg_t *fake_reg_base = (wm_rtc_reg_t *)&tmp;

    wm_ll_rtc_set_year(fake_reg_base, (uint32_t)time->tm_year);
    wm_ll_rtc_set_month(fake_reg_base, (uint32_t)time->tm_mon);
    wm_ll_rtc_set_day(fake_reg_base, (uint32_t)time->tm_mday);
    wm_ll_rtc_set_hour(fake_reg_base, (uint32_t)time->tm_hour);
    wm_ll_rtc_set_minute(fake_reg_base, (uint32_t)time->tm_min);
    wm_ll_rtc_set_second(fake_reg_base, (uint32_t)time->tm_sec);

    wm_ll_rtc_timer_counting_enable(fake_reg_base, true);
    wm_ll_rtc_timer_enable(fake_reg_base, true);

    dev->reg_base->rtc_r1.val = fake_reg_base->rtc_r1.val;
    dev->reg_base->rtc_r2.val = fake_reg_base->rtc_r2.val;

    return WM_ERR_SUCCESS;
}

int wm_hal_rtc_abort_alarm(wm_hal_rtc_dev_t *dev)
{
    if (dev == NULL || !dev->is_init) {
        return WM_ERR_INVALID_PARAM;
    }

    wm_ll_rtc_timer_enable(dev->reg_base, false);

    return WM_ERR_SUCCESS;
}

int wm_hal_rtc_register_callback(wm_hal_rtc_dev_t *dev, hal_rtc_callback_t call_back, void *user_data)
{
    if (dev == NULL || call_back == NULL || !dev->is_init) {
        return WM_ERR_INVALID_PARAM;
    }

    dev->cb        = call_back;
    dev->user_data = user_data;
    return WM_ERR_SUCCESS;
}

int wm_hal_rtc_get_supported_mask(wm_hal_rtc_dev_t *dev, int *mask)
{
    if (dev == NULL || mask == NULL || !dev->is_init) {
        return WM_ERR_INVALID_PARAM;
    }

    (void)dev;
    *mask = 0; //W800 not supported mask
    return WM_ERR_SUCCESS;
}

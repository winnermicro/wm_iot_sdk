/**
 * @file wm_drv_rtc.c
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
#include "wm_types.h"
#include "wm_error.h"
#include "wm_osal.h"

#include "wm_dt_hw.h"
#include "wm_dt.h"
#include "wm_drv_rtc.h"
#include "wm_hal_rtc.h"

#include "wm_drv_ops_rtc.c"

wm_device_t *wm_drv_rtc_init(const char *dev_name)
{
    wm_device_t *dev      = NULL;
    wm_drv_rtc_ops_t *ops = NULL;
    int ret               = WM_ERR_FAILED;

    dev = wm_dt_get_device_by_name(dev_name);

    if (dev == NULL) {
        return NULL;
    }

    if (dev->ops) {
        ops = (wm_drv_rtc_ops_t *)dev->ops;
        if (ops->init) {
            ret = ops->init(dev);
        }
    }

    return ret == WM_ERR_SUCCESS ? dev : NULL;
}

int wm_drv_rtc_deinit(wm_device_t *dev)
{
    wm_drv_rtc_ops_t *ops = NULL;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    ops = (wm_drv_rtc_ops_t *)dev->ops;
    if (ops && ops->deinit) {
        return ops->deinit(dev);
    }

    return WM_ERR_FAILED;
}

int wm_drv_rtc_set_time(wm_device_t *dev, struct tm *tm)
{
    wm_drv_rtc_ops_t *ops = NULL;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    ops = (wm_drv_rtc_ops_t *)dev->ops;
    if (ops && ops->set_time) {
        wm_drv_rtc_time_t rtc_time = { 0 };
        memcpy(&rtc_time, tm, sizeof(rtc_time));
        return ops->set_time(dev, &rtc_time);
    }

    return WM_ERR_FAILED;
}

int wm_drv_rtc_get_time(wm_device_t *dev, struct tm *tm)
{
    wm_drv_rtc_ops_t *ops = NULL;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    ops = (wm_drv_rtc_ops_t *)dev->ops;
    if (ops && ops->get_time) {
        return ops->get_time(dev, (wm_drv_rtc_time_t *)tm);
    }

    return WM_ERR_FAILED;
}

int wm_drv_rtc_get_alarm_supported_mask(wm_device_t *dev, int id, int *mask)
{
    wm_drv_rtc_ops_t *ops = NULL;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    ops = (wm_drv_rtc_ops_t *)dev->ops;
    if (ops && ops->get_alarm_supported_mask) {
        return ops->get_alarm_supported_mask(dev, id, mask);
    }

    return WM_ERR_FAILED;
}

int wm_drv_rtc_get_alarm_available_id(wm_device_t *dev, int *id)
{
    wm_drv_rtc_ops_t *ops = NULL;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    ops = (wm_drv_rtc_ops_t *)dev->ops;
    if (ops && ops->get_alarm_available_id) {
        return ops->get_alarm_available_id(dev, id);
    }

    return WM_ERR_FAILED;
}

int wm_drv_rtc_set_alarm(wm_device_t *dev, int id, int mask, struct tm *tm)
{
    wm_drv_rtc_ops_t *ops = NULL;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    ops = (wm_drv_rtc_ops_t *)dev->ops;
    if (ops && ops->set_alarm) {
        wm_drv_rtc_time_t rtc_time = { 0 };
        memcpy(&rtc_time, tm, sizeof(rtc_time));
        return ops->set_alarm(dev, id, mask, &rtc_time);
    }

    return WM_ERR_FAILED;
}

int wm_drv_rtc_get_alarm(wm_device_t *dev, int id, int *mask, struct tm *tm)
{
    wm_drv_rtc_ops_t *ops = NULL;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    ops = (wm_drv_rtc_ops_t *)dev->ops;
    if (ops && ops->get_alarm) {
        return ops->get_alarm(dev, id, mask, (wm_drv_rtc_time_t *)tm);
    }

    return WM_ERR_FAILED;
}

int wm_drv_rtc_abort_alarm(wm_device_t *dev, int id)
{
    wm_drv_rtc_ops_t *ops = NULL;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    ops = (wm_drv_rtc_ops_t *)dev->ops;
    if (ops && ops->abort_alarm) {
        return ops->abort_alarm(dev, id);
    }

    return WM_ERR_FAILED;
}

int wm_drv_rtc_register_alarm_callback(wm_device_t *dev, int id, wm_rtc_callback_t callback, void *user_data)
{
    wm_drv_rtc_ops_t *ops = NULL;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    ops = (wm_drv_rtc_ops_t *)dev->ops;
    if (ops && ops->register_alarm_callback) {
        return ops->register_alarm_callback(dev, id, callback, user_data);
    }

    return WM_ERR_FAILED;
}

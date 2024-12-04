/**
  * @file wm_drv_ops_wdt.c
  *
  * @brief wdt drv api
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

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "wm_error.h"
#include "wm_soc_cfgs.h"
#include "wm_drv_wdt.h"
#include "wm_drv_rcc.h"
#include "wm_dt_hw.h"
#include "wm_dt.h"

#define LOG_TAG "wdt"
#include "wm_log.h"

static int wm_w800_drv_wdt_init(wm_device_t *dev);
static int wm_w800_drv_wdt_deinit(wm_device_t *dev);
static int wm_w800_drv_wdt_start(wm_device_t *dev);
static int wm_w800_drv_wdt_stop(wm_device_t *dev);
static int wm_w800_drv_wdt_set_counter_value(wm_device_t *dev, uint32_t counter_value);
static int wm_w800_drv_wdt_get_counter_value(wm_device_t *dev, uint32_t *counter_value);
static int wm_w800_drv_wdt_get_remaining_time(wm_device_t *dev, uint32_t *remaining_time);
static int wm_w800_drv_wdt_feed(wm_device_t *dev);

#define WM_DRV_WDT_MAX_TIME (0xFFFFFFFF)
#define WM_DRV_WDT_LOCK(lock)                                                            \
    do {                                                                                 \
        if (!lock) {                                                                     \
            wm_log_error("mutex create err!");                                           \
            return WM_ERR_FAILED;                                                        \
        }                                                                                \
        if (WM_ERR_SUCCESS != wm_os_internal_mutex_acquire(lock, WM_OS_WAIT_TIME_MAX)) { \
            wm_log_error("mutex acquire err!");                                          \
            return WM_ERR_FAILED;                                                        \
        }                                                                                \
    } while (0);

#define WM_DRV_WDT_UNLOCK(lock)             \
    do {                                    \
        if (!lock) {                        \
            return WM_ERR_FAILED;           \
        }                                   \
        wm_os_internal_mutex_release(lock); \
    } while (0);

typedef struct {
    wm_os_mutex_t *mutex;
    wm_device_t *clock_dev;
} wm_drv_wdt_ctx_t;

typedef struct {
    wm_hal_wdt_dev_t wdt_hal;
    wm_drv_wdt_ctx_t wdt_drv;
} wm_drv_wdt_data_t;

typedef struct {
    int (*init)(wm_device_t *dev);
    int (*deinit)(wm_device_t *dev);
    int (*start)(wm_device_t *dev);
    int (*stop)(wm_device_t *dev);
    int (*set_counter_value)(wm_device_t *dev, uint32_t counter_value);
    int (*get_counter_value)(wm_device_t *dev, uint32_t *counter_value);
    int (*get_remaining_time)(wm_device_t *dev, uint32_t *remaining_time);
    int (*feed)(wm_device_t *dev);
} wm_drv_wdt_ops_t;

int wm_w800_drv_wdt_init(wm_device_t *dev)
{
    int err                     = WM_ERR_NO_INITED;
    wm_drv_wdt_data_t *drv_data = NULL;
    wm_dt_hw_wdt_t *basic       = NULL;
    int apb_clk                 = 0;

    if (!dev->drv) {
        //alloc drv data
        drv_data = (wm_drv_wdt_data_t *)wm_os_internal_malloc(sizeof(wm_drv_wdt_data_t));
        if (!drv_data) {
            return WM_ERR_NO_MEM;
        }
        memset(drv_data, 0x00, sizeof(wm_drv_wdt_data_t));
        basic = (wm_dt_hw_wdt_t *)dev->hw;

        // hal/drv ctx init
        drv_data->wdt_hal.register_base = basic->reg_base;
        drv_data->wdt_hal.period        = basic->wdt_cfg.counter_value;
        drv_data->wdt_hal.irq_num       = basic->irq_cfg.irq_num;

        drv_data->wdt_drv.clock_dev = wm_dt_get_device_by_name("rcc");
        if (!drv_data->wdt_drv.clock_dev) {
            wm_log_error("wdt get rcc err");
            wm_os_internal_free(drv_data);
            return WM_ERR_FAILED;
        }

        apb_clk = wm_drv_rcc_get_config_clock(drv_data->wdt_drv.clock_dev, WM_RCC_TYPE_APB);
        if (WM_ERR_SUCCESS != wm_os_internal_mutex_create(&drv_data->wdt_drv.mutex)) {
            wm_log_error("mutex create err");
            wm_os_internal_free(drv_data);
            return WM_ERR_FAILED;
        }

        if (WM_ERR_SUCCESS != wm_os_internal_mutex_acquire(drv_data->wdt_drv.mutex, WM_OS_WAIT_TIME_MAX)) {
            wm_log_error("mutex acquire err!");
            wm_os_internal_mutex_delete(drv_data->wdt_drv.mutex);
            wm_os_internal_free(drv_data);
            return WM_ERR_FAILED;
        }

        drv_data->wdt_hal.period = drv_data->wdt_hal.period * apb_clk;

        if (WM_ERR_SUCCESS != (err = wm_hal_wdt_init(&drv_data->wdt_hal))) {
            wm_os_internal_mutex_delete(drv_data->wdt_drv.mutex);
            wm_os_internal_free(drv_data);
            return err;
        }

        dev->drv   = drv_data;
        dev->state = WM_DEV_ST_INITED;
        WM_DRV_WDT_UNLOCK(drv_data->wdt_drv.mutex);

        return WM_ERR_SUCCESS;
    }

    return WM_ERR_NOT_ALLOWED;
}

int wm_w800_drv_wdt_deinit(wm_device_t *dev)
{
    int err                     = WM_ERR_NO_INITED;
    wm_drv_wdt_data_t *drv_data = NULL;

    if (dev->drv) {
        drv_data = (wm_drv_wdt_data_t *)dev->drv;

        WM_DRV_WDT_LOCK(drv_data->wdt_drv.mutex);
        if (WM_ERR_SUCCESS != (err = wm_hal_wdt_deinit(&drv_data->wdt_hal))) {
            WM_DRV_WDT_UNLOCK(drv_data->wdt_drv.mutex);
            return err;
        }
        WM_DRV_WDT_UNLOCK(drv_data->wdt_drv.mutex);
        wm_os_internal_mutex_delete(drv_data->wdt_drv.mutex);
        wm_os_internal_free(drv_data);
        dev->drv   = NULL;
        dev->state = WM_DEV_ST_UNINIT;

        return WM_ERR_SUCCESS;
    }

    return WM_ERR_NO_INITED;
}

int wm_w800_drv_wdt_start(wm_device_t *dev)
{
    int err                     = WM_ERR_NO_INITED;
    wm_drv_wdt_data_t *drv_data = NULL;

    if (dev->drv) {
        drv_data = (wm_drv_wdt_data_t *)dev->drv;

        WM_DRV_WDT_LOCK(drv_data->wdt_drv.mutex);
        if (WM_ERR_SUCCESS != (err = wm_hal_wdt_start(&drv_data->wdt_hal))) {
            WM_DRV_WDT_UNLOCK(drv_data->wdt_drv.mutex);
            return err;
        }
        WM_DRV_WDT_UNLOCK(drv_data->wdt_drv.mutex);

        return WM_ERR_SUCCESS;
    }

    return WM_ERR_NO_INITED;
}

int wm_w800_drv_wdt_stop(wm_device_t *dev)
{
    int err                     = WM_ERR_NO_INITED;
    wm_drv_wdt_data_t *drv_data = NULL;

    if (dev->drv) {
        drv_data = (wm_drv_wdt_data_t *)dev->drv;

        WM_DRV_WDT_LOCK(drv_data->wdt_drv.mutex);
        if (WM_ERR_SUCCESS != (err = wm_hal_wdt_stop(&drv_data->wdt_hal))) {
            WM_DRV_WDT_UNLOCK(drv_data->wdt_drv.mutex);
            return err;
        }
        WM_DRV_WDT_UNLOCK(drv_data->wdt_drv.mutex);

        return WM_ERR_SUCCESS;
    }

    return WM_ERR_NO_INITED;
}

int wm_w800_drv_wdt_set_counter_value(wm_device_t *dev, uint32_t counter_value)
{
    int err                     = WM_ERR_NO_INITED;
    wm_drv_wdt_data_t *drv_data = NULL;
    int apb_clk                 = 0;

    if (dev->drv) {
        drv_data = (wm_drv_wdt_data_t *)dev->drv;
        apb_clk  = wm_drv_rcc_get_config_clock(drv_data->wdt_drv.clock_dev, WM_RCC_TYPE_APB);

        if (!counter_value || counter_value > (WM_DRV_WDT_MAX_TIME / apb_clk)) {
            wm_log_info("counter_value err");
            return WM_ERR_INVALID_PARAM;
        }

        WM_DRV_WDT_LOCK(drv_data->wdt_drv.mutex);
        if (WM_ERR_SUCCESS != (err = wm_hal_wdt_set_counter_value(&drv_data->wdt_hal, counter_value * apb_clk))) {
            WM_DRV_WDT_UNLOCK(drv_data->wdt_drv.mutex);
            return err;
        }
        WM_DRV_WDT_UNLOCK(drv_data->wdt_drv.mutex);

        return WM_ERR_SUCCESS;
    }

    return WM_ERR_NO_INITED;
}

int wm_w800_drv_wdt_get_counter_value(wm_device_t *dev, uint32_t *counter_value)
{
    int err                     = WM_ERR_NO_INITED;
    wm_drv_wdt_data_t *drv_data = NULL;

    if (dev->drv && counter_value) {
        drv_data = (wm_drv_wdt_data_t *)dev->drv;

        WM_DRV_WDT_LOCK(drv_data->wdt_drv.mutex);
        if (WM_ERR_SUCCESS != (err = wm_hal_wdt_get_counter_value(&drv_data->wdt_hal, counter_value))) {
            WM_DRV_WDT_UNLOCK(drv_data->wdt_drv.mutex);
            return err;
        }
        WM_DRV_WDT_UNLOCK(drv_data->wdt_drv.mutex);

        return WM_ERR_SUCCESS;
    }

    return WM_ERR_NO_INITED;
}

int wm_w800_drv_wdt_get_remaining_time(wm_device_t *dev, uint32_t *remaining_time)
{
    int err                     = WM_ERR_NO_INITED;
    wm_drv_wdt_data_t *drv_data = NULL;
    uint32_t current_value      = 0;

    if (dev->drv && remaining_time) {
        drv_data = (wm_drv_wdt_data_t *)dev->drv;

        WM_DRV_WDT_LOCK(drv_data->wdt_drv.mutex);
        if (WM_ERR_SUCCESS != (err = wm_hal_wdt_get_current_value(&drv_data->wdt_hal, &current_value))) {
            WM_DRV_WDT_UNLOCK(drv_data->wdt_drv.mutex);
            return err;
        }

        *remaining_time = current_value;
        WM_DRV_WDT_UNLOCK(drv_data->wdt_drv.mutex);

        return WM_ERR_SUCCESS;
    }

    return WM_ERR_NO_INITED;
}

int wm_w800_drv_wdt_feed(wm_device_t *dev)
{
    int err                     = WM_ERR_NO_INITED;
    wm_drv_wdt_data_t *drv_data = NULL;

    if (dev->drv) {
        drv_data = (wm_drv_wdt_data_t *)dev->drv;

        WM_DRV_WDT_LOCK(drv_data->wdt_drv.mutex);
        if (WM_ERR_SUCCESS != (err = wm_hal_wdt_feed(&drv_data->wdt_hal))) {
            WM_DRV_WDT_UNLOCK(drv_data->wdt_drv.mutex);
            return err;
        }
        WM_DRV_WDT_UNLOCK(drv_data->wdt_drv.mutex);

        return WM_ERR_SUCCESS;
    }

    return WM_ERR_NO_INITED;
}

const wm_drv_wdt_ops_t wm_drv_wdt_ops = {
    .init               = wm_w800_drv_wdt_init,
    .deinit             = wm_w800_drv_wdt_deinit,
    .start              = wm_w800_drv_wdt_start,
    .stop               = wm_w800_drv_wdt_stop,
    .set_counter_value  = wm_w800_drv_wdt_set_counter_value,
    .get_counter_value  = wm_w800_drv_wdt_get_counter_value,
    .get_remaining_time = wm_w800_drv_wdt_get_remaining_time,
    .feed               = wm_w800_drv_wdt_feed,
};
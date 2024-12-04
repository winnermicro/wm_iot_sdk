/**
 * @file wm_drv_ops_timer.c
 *
 * @brief Timer ops Driver Module
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

#include <string.h>
#include "wm_types.h"
#include "wm_soc_cfgs.h"
#include "wm_error.h"
/*TODO: dependent device management*/
#include "wm_utils.h"
/*end*/
#include "wm_osal.h"

#include "wm_hal_timer.h"
#include "wm_drv_timer.h"
#include "wm_drv_rcc.h"
#include "wm_drv_irq.h"
#include "wm_drv_rcc.h"
#include "wm_dt_hw.h"
#include "wm_dt.h"
#define LOG_TAG "timer"
#include "wm_log.h"

static int wm_w800_drv_timer_init(wm_device_t *dev);
static int wm_w800_drv_timer_deinit(wm_device_t *dev);
static int wm_w800_drv_timer_start(wm_device_t *dev, wm_drv_timer_cfg_t cfg);
static int wm_w800_drv_timer_stop(wm_device_t *dev);
static int wm_w800_drv_timer_set_period(wm_device_t *dev, uint32_t period);
static int wm_w800_drv_timer_set_auto_reload(wm_device_t *dev, bool auto_reload);
static int wm_w800_drv_timer_get_counter(wm_device_t *dev, uint32_t *counter);
static int wm_w800_drv_timer_add_callback(wm_device_t *dev, wm_drv_timer_callback_t timer_callback, void *arg);
static int wm_w800_drv_timer_remove_callback(wm_device_t *dev);

#define WM_TIMER_ERR_IDX            -1
#define WM_TIMER_GET_IDX(name, idx) sscanf(name, "timer%d", &idx)

#define WM_TIMER_ENABLE_CLK(clock_dev) \
    if (g_timer_drv_ctx.timer_bitmap)  \
        wm_drv_clock_enable(clock_dev, WM_RCC_TIMER_GATE_EN);
#define WM_TIMER_DISABLE_CLK(clock_dev) \
    if (!g_timer_drv_ctx.timer_bitmap)  \
        wm_drv_clock_disable(clock_dev, WM_RCC_TIMER_GATE_EN);

#define WM_DRV_TIMER_LOCK(lock)                                                          \
    do {                                                                                 \
        if (!lock) {                                                                     \
            if (WM_ERR_SUCCESS != wm_os_internal_mutex_create(&lock)) {                  \
                wm_log_error("mutex create err!");                                       \
                return WM_ERR_FAILED;                                                    \
            }                                                                            \
        }                                                                                \
        if (WM_ERR_SUCCESS != wm_os_internal_mutex_acquire(lock, WM_OS_WAIT_TIME_MAX)) { \
            wm_log_error("mutex acquire err!");                                          \
            return WM_ERR_FAILED;                                                        \
        }                                                                                \
    } while (0);

#define WM_DRV_TIMER_UNLOCK(lock)           \
    do {                                    \
        if (!lock) {                        \
            return WM_ERR_NO_INITED;        \
        }                                   \
        wm_os_internal_mutex_release(lock); \
    } while (0);

typedef struct {
    wm_os_mutex_t *mutex;
    uint32_t timer_bitmap;
    wm_device_t *clock_dev;
} wm_drv_timer_ctx_t;

typedef struct {
    wm_hal_timer_dev_t *timer_hal;
    wm_drv_timer_ctx_t *timer_drv;
} wm_drv_timer_data_t;

typedef struct {
    int (*init)(wm_device_t *dev);
    int (*deinit)(wm_device_t *dev);
    int (*start)(wm_device_t *dev, wm_drv_timer_cfg_t cfg);
    int (*stop)(wm_device_t *dev);
    int (*set_period)(wm_device_t *dev, uint32_t period);
    int (*get_counter)(wm_device_t *dev, uint32_t *counter);
    int (*set_auto_reload)(wm_device_t *dev, bool auto_reload);
    int (*add_callback)(wm_device_t *dev, wm_drv_timer_callback_t timer_callback, void *arg);
    int (*remove_callback)(wm_device_t *dev);
} wm_drv_timer_ops_t;

static wm_drv_timer_ctx_t g_timer_drv_ctx = { 0 };
extern wm_hal_timer_dev_t g_timer_hal_dev[WM_TIMER_ID_MAX];

static int wm_w800_drv_timer_init(wm_device_t *dev)
{
    int idx                        = WM_TIMER_ERR_IDX;
    wm_drv_timer_data_t *drv_data  = NULL;
    wm_dt_hw_timer_t *basic        = NULL;
    wm_hal_timer_dev_cfg_t dev_cfg = { 0 };
    int err                        = WM_ERR_NO_INITED;

    WM_TIMER_GET_IDX(dev->name, idx);
    if (WM_TIMER_ERR_IDX != idx && WM_HAL_TIMER_IS_TIMERID(idx)) {
        if (!dev->drv) {
            WM_DRV_TIMER_LOCK(g_timer_drv_ctx.mutex);

            //alloc drv data
            drv_data = (wm_drv_timer_data_t *)wm_os_internal_malloc(sizeof(wm_drv_timer_data_t));
            if (!drv_data) {
                return WM_ERR_NO_MEM;
            }
            memset(drv_data, 0x00, sizeof(wm_drv_timer_data_t));

            basic = (wm_dt_hw_timer_t *)dev->hw;

            // hal/drv ctx init
            drv_data->timer_hal            = &g_timer_hal_dev[idx];
            drv_data->timer_drv            = &g_timer_drv_ctx;
            drv_data->timer_hal->dev       = (wm_timer_reg_t *)basic->reg_base;
            drv_data->timer_drv->clock_dev = wm_dt_get_device_by_name("rcc");

            dev_cfg.clock = wm_drv_rcc_get_config_clock(drv_data->timer_drv->clock_dev, WM_RCC_TYPE_APB);
            if (WM_ERR_SUCCESS != (err = wm_hal_timer_init(drv_data->timer_hal, dev_cfg))) {
                // no need to destory mutex
                WM_DRV_TIMER_UNLOCK(g_timer_drv_ctx.mutex);
                wm_os_internal_free(drv_data);
                return err;
            }

            drv_data->timer_drv->timer_bitmap |= WM_BIT(idx);
            WM_TIMER_ENABLE_CLK(drv_data->timer_drv->clock_dev);

            dev->drv   = drv_data;
            dev->state = WM_DEV_ST_INITED;

            WM_DRV_TIMER_UNLOCK(g_timer_drv_ctx.mutex);

            return WM_ERR_SUCCESS;
        } else {
            wm_log_info("timer already inited");
            return WM_ERR_SUCCESS;
        }
    }

    return WM_ERR_INVALID_PARAM;
}

static int wm_w800_drv_timer_deinit(wm_device_t *dev)
{
    wm_drv_timer_data_t *drv_data = NULL;
    int idx                       = WM_TIMER_ERR_IDX;
    int err                       = WM_ERR_SUCCESS;

    WM_TIMER_GET_IDX(dev->name, idx);
    if (WM_TIMER_ERR_IDX != idx && dev->drv) {
        drv_data = (wm_drv_timer_data_t *)dev->drv;

        // clr timer cfg
        WM_DRV_TIMER_LOCK(g_timer_drv_ctx.mutex);
        if (WM_ERR_SUCCESS != (err = wm_hal_timer_stop(drv_data->timer_hal, idx))) {
            WM_DRV_TIMER_UNLOCK(g_timer_drv_ctx.mutex);
            return err;
        }

        // clr isr
        drv_data->timer_hal->callback = NULL;
        drv_data->timer_hal->arg      = NULL;

        // clr bitmap
        drv_data->timer_drv->timer_bitmap &= ~WM_BIT(idx);
        WM_TIMER_DISABLE_CLK(drv_data->timer_drv->clock_dev);

        // clr drv data
        wm_os_internal_free(drv_data);
        dev->drv = NULL;

        dev->state = WM_DEV_ST_UNINIT;
        WM_DRV_TIMER_UNLOCK(g_timer_drv_ctx.mutex);

        return WM_ERR_SUCCESS;
    }

    return WM_ERR_INVALID_PARAM;
}

static int wm_w800_drv_timer_start(wm_device_t *dev, wm_drv_timer_cfg_t cfg)
{
    wm_drv_timer_data_t *drv_data = NULL;
    int idx                       = WM_TIMER_ERR_IDX;
    int err                       = WM_ERR_NO_INITED;

    WM_TIMER_GET_IDX(dev->name, idx);
    if (WM_TIMER_ERR_IDX != idx && dev->drv) {
        wm_hal_timer_cfg_t timer_cfg = { 0 };

        timer_cfg.auto_reload = cfg.auto_reload;
        timer_cfg.period      = cfg.period;
        timer_cfg.unit        = cfg.unit;

        drv_data = (wm_drv_timer_data_t *)dev->drv;
        WM_DRV_TIMER_LOCK(drv_data->timer_drv->mutex);

        WM_TIMER_ENABLE_CLK(drv_data->timer_drv->clock_dev);
        if (WM_ERR_SUCCESS != (err = wm_hal_timer_start(drv_data->timer_hal, idx, timer_cfg))) {
            // start err , if no timer enable ,close clk
            WM_DRV_TIMER_UNLOCK(drv_data->timer_drv->mutex);
            return err;
        }
        WM_DRV_TIMER_UNLOCK(drv_data->timer_drv->mutex);

        return err;
    }

    return err;
}

static int wm_w800_drv_timer_stop(wm_device_t *dev)
{
    wm_drv_timer_data_t *drv_data = NULL;
    int idx                       = WM_TIMER_ERR_IDX;
    int err                       = WM_ERR_NO_INITED;

    WM_TIMER_GET_IDX(dev->name, idx);
    if (WM_TIMER_ERR_IDX != idx && dev->drv) {
        drv_data = (wm_drv_timer_data_t *)dev->drv;

        WM_DRV_TIMER_LOCK(drv_data->timer_drv->mutex);
        if (WM_ERR_SUCCESS != (err = wm_hal_timer_stop(drv_data->timer_hal, idx))) {
            WM_DRV_TIMER_UNLOCK(drv_data->timer_drv->mutex);
            return err;
        }

        WM_DRV_TIMER_UNLOCK(drv_data->timer_drv->mutex);

        return err;
    }

    return err;
}

static int wm_w800_drv_timer_set_period(wm_device_t *dev, uint32_t period)
{
    wm_drv_timer_data_t *drv_data = NULL;
    int idx                       = WM_TIMER_ERR_IDX;
    int err                       = WM_ERR_NO_INITED;

    WM_TIMER_GET_IDX(dev->name, idx);
    if (WM_TIMER_ERR_IDX != idx && dev->drv) {
        drv_data = (wm_drv_timer_data_t *)dev->drv;

        WM_DRV_TIMER_LOCK(drv_data->timer_drv->mutex);
        if (WM_ERR_SUCCESS != (err = wm_hal_timer_set_period(drv_data->timer_hal, idx, period))) {
            WM_DRV_TIMER_UNLOCK(drv_data->timer_drv->mutex);
            return err;
        }
        WM_DRV_TIMER_UNLOCK(drv_data->timer_drv->mutex);
    }

    return err;
}

static int wm_w800_drv_timer_set_auto_reload(wm_device_t *dev, bool auto_reload)
{
    wm_drv_timer_data_t *drv_data = NULL;
    int idx                       = WM_TIMER_ERR_IDX;
    int err                       = WM_ERR_NO_INITED;

    WM_TIMER_GET_IDX(dev->name, idx);
    if (WM_TIMER_ERR_IDX != idx && dev->drv) {
        drv_data = (wm_drv_timer_data_t *)dev->drv;

        WM_DRV_TIMER_LOCK(drv_data->timer_drv->mutex);
        if (WM_ERR_SUCCESS != (err = wm_hal_timer_set_auto_reload(drv_data->timer_hal, idx, auto_reload))) {
            WM_DRV_TIMER_UNLOCK(drv_data->timer_drv->mutex);
            return err;
        }
        WM_DRV_TIMER_UNLOCK(drv_data->timer_drv->mutex);
    }

    return err;
}

static int wm_w800_drv_timer_get_counter(wm_device_t *dev, uint32_t *counter)
{
    wm_drv_timer_data_t *drv_data = NULL;
    int idx                       = WM_TIMER_ERR_IDX;
    int err                       = WM_ERR_NO_INITED;

    WM_TIMER_GET_IDX(dev->name, idx);
    if (WM_TIMER_ERR_IDX != idx && dev->drv && counter) {
        drv_data = (wm_drv_timer_data_t *)dev->drv;

        WM_DRV_TIMER_LOCK(drv_data->timer_drv->mutex);
        if (WM_ERR_SUCCESS != (err = wm_hal_timer_get_counter(drv_data->timer_hal, idx, counter))) {
            WM_DRV_TIMER_UNLOCK(drv_data->timer_drv->mutex);
            return err;
        }
        WM_DRV_TIMER_UNLOCK(drv_data->timer_drv->mutex);
    }

    return err;
}

static int wm_w800_drv_timer_add_callback(wm_device_t *dev, wm_drv_timer_callback_t timer_callback, void *arg)
{
    wm_drv_timer_data_t *drv_data = NULL;
    int idx                       = WM_TIMER_ERR_IDX;
    int err                       = WM_ERR_SUCCESS;

    WM_TIMER_GET_IDX(dev->name, idx);
    if (WM_TIMER_ERR_IDX != idx && dev->drv) {
        drv_data = (wm_drv_timer_data_t *)dev->drv;

        drv_data->timer_hal->callback = timer_callback;
        drv_data->timer_hal->arg      = arg;
    }

    return err;
}

static int wm_w800_drv_timer_remove_callback(wm_device_t *dev)
{
    wm_drv_timer_data_t *drv_data = NULL;
    int idx                       = WM_TIMER_ERR_IDX;
    int err                       = WM_ERR_SUCCESS;

    WM_TIMER_GET_IDX(dev->name, idx);
    if (WM_TIMER_ERR_IDX != idx && dev->drv) {
        drv_data = (wm_drv_timer_data_t *)dev->drv;

        drv_data->timer_hal->callback = NULL;
        drv_data->timer_hal->arg      = NULL;
    }

    return err;
}

const wm_drv_timer_ops_t wm_drv_timer_ops = {
    .init            = wm_w800_drv_timer_init,
    .deinit          = wm_w800_drv_timer_deinit,
    .start           = wm_w800_drv_timer_start,
    .stop            = wm_w800_drv_timer_stop,
    .set_period      = wm_w800_drv_timer_set_period,
    .set_auto_reload = wm_w800_drv_timer_set_auto_reload,
    .get_counter     = wm_w800_drv_timer_get_counter,
    .add_callback    = wm_w800_drv_timer_add_callback,
    .remove_callback = wm_w800_drv_timer_remove_callback,
};

/**
 * @file wm_drv_ops_touch_sensor.c
 *
 * @brief touch sensor ops Driver Module
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
#include <string.h>
#include <assert.h>
#include "wm_types.h"
#include "wm_osal.h"
#include "wm_drv_gpio.h"
#include "wm_drv_rcc.h"
#include "wm_hal_touch_sensor.h"
#include "wm_drv_touch_sensor.h"
#include "wm_dt_hw.h"

#define LOG_TAG "drv_touch"
#include "wm_log.h"

#define WM_DRV_TOUCH_SENSOR_ERR wm_log_error

#define WM_DRV_TOUCH_SENSOR_LOCK(lock)                                               \
    if (WM_ERR_SUCCESS != wm_os_internal_mutex_acquire(lock, WM_OS_WAIT_TIME_MAX)) { \
        WM_DRV_TOUCH_SENSOR_ERR("lock failed: %s:%d", __func__, __LINE__);           \
        return WM_ERR_FAILED;                                                        \
    }

#define WM_DRV_TOUCH_SENSOR_UNLOCK(lock) wm_os_internal_mutex_release(lock)

typedef struct {
    wm_drv_touch_sensor_callback_t callback;
    void *callback_priv;
} wm_drv_touch_sensor_callback_ctx_t;

typedef struct {
    wm_hal_touch_sensor_dev_t hal_dev;

    wm_os_mutex_t *mutex;

    wm_drv_touch_sensor_callback_ctx_t callbacks[WM_TOUCH_NUM_MAX + 1];
} wm_drv_touch_sensor_ctx_t;

typedef struct {
    int (*register_callback)(wm_device_t *dev, wm_touch_sensor_num_t touch_num, wm_drv_touch_sensor_callback_t callback,
                             void *priv);
    int (*set_threshold)(wm_device_t *dev, wm_touch_sensor_num_t touch_num, uint32_t threshold);
    int (*get_threshold)(wm_device_t *dev, wm_touch_sensor_num_t touch_num, uint32_t *threshold);
    int (*read_countnum)(wm_device_t *dev, wm_touch_sensor_num_t touch_num, uint32_t *countnum);
    int (*start)(wm_device_t *dev, wm_touch_sensor_num_t touch_num);
    int (*stop)(wm_device_t *dev, wm_touch_sensor_num_t touch_num);
    int (*init)(wm_device_t *dev);
    int (*deinit)(wm_device_t *dev);
} wm_drv_touch_sensor_ops_t;

static void wm_drv_touch_sensor_callback(wm_hal_touch_sensor_dev_t *dev, wm_touch_sensor_num_t touch_num, void *priv)
{
    wm_drv_touch_sensor_ctx_t *drv_ctx = (wm_drv_touch_sensor_ctx_t *)priv;

    if (WM_ERR_SUCCESS == wm_os_internal_mutex_acquire(drv_ctx->mutex, WM_OS_WAIT_TIME_MAX)) {
        if (drv_ctx->callbacks[touch_num].callback)
            drv_ctx->callbacks[touch_num].callback(touch_num, drv_ctx->callbacks[touch_num].callback_priv);
        wm_os_internal_mutex_release(drv_ctx->mutex);
    }
}

static int wm_drv_ops_touch_sensor_reg_callback(wm_device_t *dev, wm_touch_sensor_num_t touch_num,
                                                wm_drv_touch_sensor_callback_t callback, void *priv)
{
    wm_drv_touch_sensor_ctx_t *drv_ctx = (wm_drv_touch_sensor_ctx_t *)dev->drv;
    int ret                            = WM_ERR_NO_INITED;
    uint16_t enable                    = 0;

    if (drv_ctx) {
        WM_DRV_TOUCH_SENSOR_LOCK(drv_ctx->mutex);

        drv_ctx->callbacks[touch_num].callback      = callback;
        drv_ctx->callbacks[touch_num].callback_priv = priv;

        ret = wm_hal_touch_sensor_get_int_enable(&drv_ctx->hal_dev, &enable);

        if (!ret) {
            ret = wm_hal_touch_sensor_register_callback(&drv_ctx->hal_dev, touch_num, wm_drv_touch_sensor_callback, drv_ctx);
            if (!ret) {
                if (callback)
                    enable |= WM_BIT(touch_num);
                else
                    enable &= ~WM_BIT(touch_num);
                ret = wm_hal_touch_sensor_set_int_enable(&drv_ctx->hal_dev, enable);
            }
        }

        WM_DRV_TOUCH_SENSOR_UNLOCK(drv_ctx->mutex);
    }

    return ret;
}

static int wm_drv_ops_touch_sensor_set_threshold(wm_device_t *dev, wm_touch_sensor_num_t touch_num, uint32_t threshold)
{
    wm_drv_touch_sensor_ctx_t *drv_ctx = (wm_drv_touch_sensor_ctx_t *)dev->drv;
    int ret                            = WM_ERR_NO_INITED;

    if (drv_ctx) {
        WM_DRV_TOUCH_SENSOR_LOCK(drv_ctx->mutex);

        ret = wm_hal_touch_sensor_set_threshold(&drv_ctx->hal_dev, touch_num, threshold);

        WM_DRV_TOUCH_SENSOR_UNLOCK(drv_ctx->mutex);
    }

    return ret;
}

static int wm_drv_ops_touch_sensor_get_threshold(wm_device_t *dev, wm_touch_sensor_num_t touch_num, uint32_t *threshold)
{
    wm_drv_touch_sensor_ctx_t *drv_ctx = (wm_drv_touch_sensor_ctx_t *)dev->drv;
    int ret                            = WM_ERR_NO_INITED;

    if (drv_ctx) {
        //WM_DRV_TOUCH_SENSOR_LOCK(drv_ctx->mutex);

        ret = wm_hal_touch_sensor_get_threshold(&drv_ctx->hal_dev, touch_num, threshold);

        //WM_DRV_TOUCH_SENSOR_UNLOCK(drv_ctx->mutex);
    }

    return ret;
}

static int wm_drv_ops_touch_sensor_read_countnum(wm_device_t *dev, wm_touch_sensor_num_t touch_num, uint32_t *countnum)
{
    wm_drv_touch_sensor_ctx_t *drv_ctx = (wm_drv_touch_sensor_ctx_t *)dev->drv;
    int ret                            = WM_ERR_NO_INITED;

    if (drv_ctx) {
        //WM_DRV_TOUCH_SENSOR_LOCK(drv_ctx->mutex);

        ret = wm_hal_touch_sensor_read_countnum(&drv_ctx->hal_dev, touch_num, countnum);

        //WM_DRV_TOUCH_SENSOR_UNLOCK(drv_ctx->mutex);
    }

    return ret;
}

static int wm_drv_ops_touch_sensor_start(wm_device_t *dev, wm_touch_sensor_num_t touch_num)
{
    wm_drv_touch_sensor_ctx_t *drv_ctx = (wm_drv_touch_sensor_ctx_t *)dev->drv;
    int ret                            = WM_ERR_NO_INITED;

    if (touch_num > WM_TOUCH_NUM_MAX)
        return WM_ERR_INVALID_PARAM;

    if (drv_ctx) {
        WM_DRV_TOUCH_SENSOR_LOCK(drv_ctx->mutex);

        wm_hal_touch_sensor_start(&drv_ctx->hal_dev, touch_num);

        WM_DRV_TOUCH_SENSOR_UNLOCK(drv_ctx->mutex);

        ret = WM_ERR_SUCCESS;
    }

    return ret;
}

static int wm_drv_ops_touch_sensor_stop(wm_device_t *dev, wm_touch_sensor_num_t touch_num)
{
    wm_drv_touch_sensor_ctx_t *drv_ctx = (wm_drv_touch_sensor_ctx_t *)dev->drv;
    int ret                            = WM_ERR_NO_INITED;

    if (touch_num > WM_TOUCH_NUM_MAX)
        return WM_ERR_INVALID_PARAM;

    if (drv_ctx) {
        WM_DRV_TOUCH_SENSOR_LOCK(drv_ctx->mutex);

        wm_hal_touch_sensor_stop(&drv_ctx->hal_dev, touch_num);

        WM_DRV_TOUCH_SENSOR_UNLOCK(drv_ctx->mutex);

        ret = WM_ERR_SUCCESS;
    }

    return ret;
}

static int wm_drv_ops_touch_sensor_init(wm_device_t *dev)
{
    wm_drv_touch_sensor_ctx_t *drv_ctx = NULL;
    wm_dt_hw_touch_sensor_t *hw        = (wm_dt_hw_touch_sensor_t *)dev->hw;
    int ret                            = WM_ERR_ALREADY_INITED;
    int i;

    if (dev->drv == NULL) {
        drv_ctx = wm_os_internal_malloc(sizeof(wm_drv_touch_sensor_ctx_t));
        if (!drv_ctx) {
            return WM_ERR_NO_MEM;
        }

        memset(drv_ctx, 0, sizeof(wm_drv_touch_sensor_ctx_t));

        //create mutex
        ret = wm_os_internal_mutex_create(&drv_ctx->mutex);
        if (ret) {
            wm_os_internal_free(drv_ctx);
            return WM_ERR_NO_MEM;
        }

        ret = wm_os_internal_mutex_acquire(drv_ctx->mutex, WM_OS_WAIT_TIME_MAX);
        if (ret) {
            wm_os_internal_mutex_delete(drv_ctx->mutex);
            wm_os_internal_free(drv_ctx);
            return WM_ERR_FAILED;
        }

        dev->drv = drv_ctx;

        // hal dev init
        drv_ctx->hal_dev.register_base = hw->reg_base;
        drv_ctx->hal_dev.irq_num       = hw->irq_cfg.irq_num;

        // pinmux
        for (i = 0; i < hw->pin_cfg_count; i++) {
            if (!ret)
                ret = wm_drv_gpio_iomux_func_sel(hw->pin_cfg[i].pin_num, hw->pin_cfg[i].pin_mux);
            if (!ret)
                ret = wm_drv_gpio_set_dir(hw->pin_cfg[i].pin_num, hw->pin_cfg[i].pin_dir);
            if (!ret)
                ret = wm_drv_gpio_set_pullmode(hw->pin_cfg[i].pin_num, hw->pin_cfg[i].pin_pupd);
            if (ret)
                break;
        }

        if (!ret)
            ret = wm_hal_touch_sensor_init(&drv_ctx->hal_dev);

        if (ret) {
            wm_os_internal_mutex_delete(drv_ctx->mutex);
            wm_os_internal_free(drv_ctx);
            dev->drv = NULL;
        } else {
            wm_drv_clock_enable(wm_dt_get_device_by_name(hw->rcc_device_name), WM_RCC_TOUCH_GATE_EN);
            dev->state = WM_DEV_ST_INITED;
            wm_os_internal_mutex_release(drv_ctx->mutex);
        }
    }

    return ret;
}

static int wm_drv_ops_touch_sensor_deinit(wm_device_t *dev)
{
    wm_drv_touch_sensor_ctx_t *drv_ctx = (wm_drv_touch_sensor_ctx_t *)dev->drv;
    int ret                            = WM_ERR_NO_INITED;
    wm_dt_hw_touch_sensor_t *hw        = (wm_dt_hw_touch_sensor_t *)dev->hw;

    if (drv_ctx) {
        WM_DRV_TOUCH_SENSOR_LOCK(drv_ctx->mutex);

        wm_hal_touch_sensor_deinit(&drv_ctx->hal_dev);

        wm_os_internal_mutex_delete(drv_ctx->mutex);

        wm_os_internal_free(drv_ctx);
        wm_drv_clock_disable(wm_dt_get_device_by_name(hw->rcc_device_name), WM_RCC_TOUCH_GATE_EN);

        dev->drv   = NULL;
        dev->state = WM_DEV_ST_UNINIT;

        ret = WM_ERR_SUCCESS;
    }

    return ret;
}

const wm_drv_touch_sensor_ops_t wm_drv_touch_sensor_ops = {
    .register_callback = wm_drv_ops_touch_sensor_reg_callback,
    .set_threshold     = wm_drv_ops_touch_sensor_set_threshold,
    .get_threshold     = wm_drv_ops_touch_sensor_get_threshold,
    .read_countnum     = wm_drv_ops_touch_sensor_read_countnum,
    .start             = wm_drv_ops_touch_sensor_start,
    .stop              = wm_drv_ops_touch_sensor_stop,
    .init              = wm_drv_ops_touch_sensor_init,
    .deinit            = wm_drv_ops_touch_sensor_deinit,
};

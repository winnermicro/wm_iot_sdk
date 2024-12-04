/**
 * @file wm_drv_pmu.c
 *
 * @brief DRV_PMU Module
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

#include "wm_drv_pmu.h"

#define LOG_TAG "PMU_DRV"
#include "wm_log.h"

typedef struct {
    /* Init */
    int (*init)(wm_device_t *dev);
    int (*deinit)(wm_device_t *dev);
    /* Clock */
    int (*set_clock_source)(wm_device_t *dev, wm_pmu_clock_source_t clk_src);
    /* Low power mode */
    int (*set_power_mode)(wm_device_t *dev, wm_pmu_power_mode_t mode);
    /* Query status before wakeup */
    int (*get_status_before_wakeup)(wm_device_t *dev, wm_pmu_wakeup_pre_status_t *status);
    /* Timer */
    int (*set_timer)(wm_device_t *dev, uint32_t time_ms);
    int (*abort_timer)(wm_device_t *dev);
    /* Register irq callback */
    int (*register_irq_cb)(wm_device_t *dev, wm_pmu_irq_type_t type, wm_pmu_callback_t callback, void *user_data);
    int (*unregister_irq_cb)(wm_device_t *dev, wm_pmu_irq_type_t type);
} wm_drv_pmu_ops_t;

#include "wm_drv_ops_pmu.c"

wm_device_t *wm_drv_pmu_init(const char *dev_name)
{
    wm_device_t *dev      = NULL;
    wm_drv_pmu_ops_t *ops = NULL;
    int ret               = WM_ERR_FAILED;

    dev = wm_dt_get_device_by_name(dev_name);

    if (dev && dev->ops) {
        ops = (wm_drv_pmu_ops_t *)dev->ops;
        if (ops->init) {
            ret = ops->init(dev);
        }
    }

    return ret == WM_ERR_SUCCESS ? dev : NULL;
}

int wm_drv_pmu_deinit(wm_device_t *dev)
{
    wm_drv_pmu_ops_t *ops = NULL;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    if (dev->ops) {
        ops = (wm_drv_pmu_ops_t *)dev->ops;
        if (ops->deinit) {
            return ops->deinit(dev);
        }
    }

    return WM_ERR_FAILED;
}

int wm_drv_pmu_set_clock_source(wm_device_t *dev, wm_pmu_clock_source_t clk_src)
{
    wm_drv_pmu_ops_t *ops = NULL;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    if (dev->ops) {
        ops = (wm_drv_pmu_ops_t *)dev->ops;
        if (ops->set_clock_source) {
            return ops->set_clock_source(dev, clk_src);
        }
    }

    return WM_ERR_FAILED;
}

int wm_drv_pmu_set_power_mode(wm_device_t *dev, wm_pmu_power_mode_t mode)
{
    wm_drv_pmu_ops_t *ops = NULL;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    if (dev->ops) {
        ops = (wm_drv_pmu_ops_t *)dev->ops;
        if (ops->set_power_mode) {
            return ops->set_power_mode(dev, mode);
        }
    }

    return WM_ERR_FAILED;
}

int wm_drv_pmu_register_irq_callback(wm_device_t *dev, wm_pmu_irq_type_t type, wm_pmu_callback_t callback, void *user_data)
{
    wm_drv_pmu_ops_t *ops = NULL;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    if (dev->ops) {
        ops = (wm_drv_pmu_ops_t *)dev->ops;
        if (ops->register_irq_cb) {
            return ops->register_irq_cb(dev, type, callback, user_data);
        }
    }

    return WM_ERR_FAILED;
}

int wm_drv_pmu_unregister_irq_callback(wm_device_t *dev, wm_pmu_irq_type_t type)
{
    wm_drv_pmu_ops_t *ops = NULL;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    if (dev->ops) {
        ops = (wm_drv_pmu_ops_t *)dev->ops;
        if (ops->unregister_irq_cb) {
            return ops->unregister_irq_cb(dev, type);
        }
    }

    return WM_ERR_FAILED;
}

int wm_drv_pmu_get_status_before_wakeup(wm_device_t *dev, wm_pmu_wakeup_pre_status_t *status)
{
    wm_drv_pmu_ops_t *ops = NULL;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    if (dev->ops) {
        ops = (wm_drv_pmu_ops_t *)dev->ops;
        if (ops->get_status_before_wakeup) {
            return ops->get_status_before_wakeup(dev, status);
        }
    }

    return WM_ERR_FAILED;
}

int wm_drv_pmu_set_timer(wm_device_t *dev, uint32_t time_ms)
{
    wm_drv_pmu_ops_t *ops = NULL;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    if (dev->ops) {
        ops = (wm_drv_pmu_ops_t *)dev->ops;
        if (ops->set_timer) {
            return ops->set_timer(dev, time_ms);
        }
    }

    return WM_ERR_FAILED;
}

int wm_drv_pmu_abort_timer(wm_device_t *dev)
{
    wm_drv_pmu_ops_t *ops = NULL;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    if (dev->ops) {
        ops = (wm_drv_pmu_ops_t *)dev->ops;
        if (ops->abort_timer) {
            return ops->abort_timer(dev);
        }
    }

    return WM_ERR_FAILED;
}

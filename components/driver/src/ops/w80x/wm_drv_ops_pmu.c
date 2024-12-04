/**
 * @file wm_drv_ops_pmu.c
 *
 * @brief DRV_OPS_PMU Module
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

#include <assert.h>
#include "wmsdk_config.h"
#include "wm_types.h"
#include "wm_error.h"
#include "wm_osal.h"
#include "wm_dt_hw.h"

#include "wm_drv_pmu.h"

#define WM_DRV_PMU_LOG_MARKER()          wm_log_debug("%s@%d\n", __func__, __LINE__)

#define WM_800_PMU_TIME0_MIN_TIME        (1 * 1000)     /**< W800 PMU timer0 min sleep time (millisecond) */
#define WM_800_PMU_TIME0_MAX_TIME        (65535 * 1000) /**< W800 PMU timer0 max sleep time (millisecond) */
#define WM_800_PMU_TIME1_MIN_TIME        (1)            /**< W800 PMU timer1 min sleep time (millisecond) */
#define WM_800_PMU_TIME1_MAX_TIME        (65535)        /**< W800 PMU timer1 max sleep time (millisecond) */
#define WM_800_PMU_WAKEUP_IO_THRES_LEVEL (4)            /**< Wakeup duration time is 4 * 128 ms */

typedef struct {
    wm_os_mutex_t *mutex;
    struct {
        wm_pmu_callback_t cb;
        void *user_data;
    } pmu_irq[WM_PMU_IRQ_MAX];
} wm_drv_pmu_ctx_t;

typedef struct {
    wm_hal_pmu_dev_t hal_pmu;
    wm_drv_pmu_ctx_t drv_ctx;
} wm_drv_pmu_data_t;

static void wm_drv_ops_pmu_callback(wm_pmu_irq_type_t type, void *ptr)
{
    if (type >= WM_PMU_IRQ_MAX || ptr == NULL) {
        return;
    }

    wm_drv_pmu_data_t *drv_pmu = (wm_drv_pmu_data_t *)ptr;

    if (drv_pmu->drv_ctx.pmu_irq[type].cb != NULL) {
        drv_pmu->drv_ctx.pmu_irq[type].cb(drv_pmu->drv_ctx.pmu_irq[type].user_data);
    }
}

static int wm_drv_ops_pmu_init(wm_device_t *dev)
{
    wm_drv_pmu_data_t *dev_pmu       = NULL;
    wm_dt_hw_pmu_t *hw               = (wm_dt_hw_pmu_t *)dev->hw;
    wm_pmu_wakeup_io_config_t config = { .wake_thres_level = WM_800_PMU_WAKEUP_IO_THRES_LEVEL };

    if (dev->drv != NULL) {
        return WM_ERR_ALREADY_INITED;
    }

    dev_pmu = (wm_drv_pmu_data_t *)dev->drv;
    if (dev_pmu == NULL) {
        dev_pmu = wm_os_internal_calloc(sizeof(wm_drv_pmu_data_t), 1);
        if (dev_pmu == NULL) {
            return WM_ERR_NO_MEM;
        }
    }

    /** create mutex */
    if (wm_os_internal_mutex_create(&dev_pmu->drv_ctx.mutex) != WM_ERR_SUCCESS) {
        WM_DRV_PMU_LOG_MARKER();
        goto error;
    }

    if (wm_os_internal_mutex_acquire(dev_pmu->drv_ctx.mutex, WM_OS_WAIT_TIME_MAX) != WM_ERR_SUCCESS) {
        WM_DRV_PMU_LOG_MARKER();
        goto error;
    }

    dev_pmu->hal_pmu.reg_base = (wm_pmu_reg_t *)hw->reg_base;
    dev_pmu->hal_pmu.irq_num  = hw->irq_cfg.irq_num;

    if (wm_hal_pmu_init(&dev_pmu->hal_pmu) != WM_ERR_SUCCESS) {
        wm_os_internal_mutex_release(dev_pmu->drv_ctx.mutex);
        WM_DRV_PMU_LOG_MARKER();
        goto error;
    }

    if (wm_hal_pmu_register_callback(&dev_pmu->hal_pmu, wm_drv_ops_pmu_callback, dev_pmu) != WM_ERR_SUCCESS) {
        wm_os_internal_mutex_release(dev_pmu->drv_ctx.mutex);
        WM_DRV_PMU_LOG_MARKER();
        goto error;
    }

    if (wm_hal_pmu_set_clock_source(&dev_pmu->hal_pmu, hw->clk_src) != WM_ERR_SUCCESS) {
        wm_os_internal_mutex_release(dev_pmu->drv_ctx.mutex);
        WM_DRV_PMU_LOG_MARKER();
        goto error;
    }

    if (wm_hal_pmu_calibration_internal_rc(&dev_pmu->hal_pmu) != WM_ERR_SUCCESS) {
        wm_os_internal_mutex_release(dev_pmu->drv_ctx.mutex);
        WM_DRV_PMU_LOG_MARKER();
        goto error;
    }

    if (wm_hal_pmu_set_wakeup_io(&dev_pmu->hal_pmu, &config) != WM_ERR_SUCCESS) {
        wm_os_internal_mutex_release(dev_pmu->drv_ctx.mutex);
        WM_DRV_PMU_LOG_MARKER();
        goto error;
    }

    dev->drv   = dev_pmu;
    dev->state = WM_DEV_ST_INITED;

    wm_os_internal_mutex_release(dev_pmu->drv_ctx.mutex);

    return WM_ERR_SUCCESS;

error:

    if (dev_pmu->drv_ctx.mutex != NULL) {
        wm_os_internal_mutex_delete(dev_pmu->drv_ctx.mutex);
    }

    if (dev_pmu != NULL) {
        wm_os_internal_free(dev_pmu);
    }

    dev->drv = NULL;
    wm_log_error("PMU init fail");
    return WM_ERR_FAILED;
}

static int wm_drv_ops_pmu_deinit(wm_device_t *dev)
{
    if (dev == NULL || dev->drv == NULL) {
        return WM_ERR_FAILED;
    }

    /** This is dummy function, it will not uninstall PMU. */
    return WM_ERR_SUCCESS;
}

static int wm_drv_ops_pmu_set_clock_source(wm_device_t *dev, wm_pmu_clock_source_t clk_src)
{
    if (dev == NULL || dev->drv == NULL) {
        return WM_ERR_FAILED;
    }

    wm_drv_pmu_data_t *dev_pmu = (wm_drv_pmu_data_t *)dev->drv;

    if (wm_os_internal_mutex_acquire(dev_pmu->drv_ctx.mutex, WM_OS_WAIT_TIME_MAX) != WM_ERR_SUCCESS) {
        WM_DRV_PMU_LOG_MARKER();
        return WM_ERR_FAILED;
    }

    if (wm_hal_pmu_set_clock_source(&dev_pmu->hal_pmu, clk_src) != WM_ERR_SUCCESS) {
        wm_os_internal_mutex_release(dev_pmu->drv_ctx.mutex);
        WM_DRV_PMU_LOG_MARKER();
        return WM_ERR_FAILED;
    }

    if (wm_hal_pmu_calibration_internal_rc(&dev_pmu->hal_pmu) != WM_ERR_SUCCESS) {
        wm_os_internal_mutex_release(dev_pmu->drv_ctx.mutex);
        WM_DRV_PMU_LOG_MARKER();
        return WM_ERR_FAILED;
    }

    wm_os_internal_mutex_release(dev_pmu->drv_ctx.mutex);

    return WM_ERR_SUCCESS;
}

int wm_drv_ops_pmu_set_power_mode(wm_device_t *dev, wm_pmu_power_mode_t mode)
{
    if (dev == NULL || dev->drv == NULL) {
        return WM_ERR_FAILED;
    }

    wm_drv_pmu_data_t *dev_pmu = (wm_drv_pmu_data_t *)dev->drv;

    if (wm_os_internal_mutex_acquire(dev_pmu->drv_ctx.mutex, WM_OS_WAIT_TIME_MAX) != WM_ERR_SUCCESS) {
        WM_DRV_PMU_LOG_MARKER();
        return WM_ERR_FAILED;
    }

    if (wm_hal_pmu_set_power_mode(&dev_pmu->hal_pmu, mode) != WM_ERR_SUCCESS) {
        wm_os_internal_mutex_release(dev_pmu->drv_ctx.mutex);
        WM_DRV_PMU_LOG_MARKER();
        return WM_ERR_FAILED;
    }

    wm_os_internal_mutex_release(dev_pmu->drv_ctx.mutex);

    return WM_ERR_SUCCESS;
}

int wm_drv_ops_pmu_get_status_before_wakeup(wm_device_t *dev, wm_pmu_wakeup_pre_status_t *status)
{
    if (dev == NULL || dev->drv == NULL) {
        return WM_ERR_FAILED;
    }

    wm_drv_pmu_data_t *dev_pmu = (wm_drv_pmu_data_t *)dev->drv;

    if (status == NULL) {
        WM_DRV_PMU_LOG_MARKER();
        return WM_ERR_FAILED;
    }

    if (wm_os_internal_mutex_acquire(dev_pmu->drv_ctx.mutex, WM_OS_WAIT_TIME_MAX) != WM_ERR_SUCCESS) {
        WM_DRV_PMU_LOG_MARKER();
        return WM_ERR_FAILED;
    }

    if (wm_hal_pmu_get_status_before_wakeup(&dev_pmu->hal_pmu, status) != WM_ERR_SUCCESS) {
        wm_os_internal_mutex_release(dev_pmu->drv_ctx.mutex);
        WM_DRV_PMU_LOG_MARKER();
        return WM_ERR_FAILED;
    }

    wm_os_internal_mutex_release(dev_pmu->drv_ctx.mutex);

    return WM_ERR_SUCCESS;
}

int wm_drv_ops_pmu_set_timer(wm_device_t *dev, uint32_t time_ms)
{
    if (dev == NULL || dev->drv == NULL) {
        return WM_ERR_FAILED;
    }

    wm_drv_pmu_data_t *dev_pmu = (wm_drv_pmu_data_t *)dev->drv;

    if (!(time_ms >= WM_800_PMU_TIME1_MIN_TIME && time_ms <= WM_800_PMU_TIME1_MAX_TIME) &&
        !(time_ms >= WM_800_PMU_TIME0_MIN_TIME && time_ms <= WM_800_PMU_TIME0_MAX_TIME)) {
        WM_DRV_PMU_LOG_MARKER();
        return WM_ERR_FAILED;
    }

    if (wm_os_internal_mutex_acquire(dev_pmu->drv_ctx.mutex, WM_OS_WAIT_TIME_MAX) != WM_ERR_SUCCESS) {
        WM_DRV_PMU_LOG_MARKER();
        return WM_ERR_FAILED;
    }

    /** prioritize using timer1, timer1 has higher accuracy */
    if (time_ms >= WM_800_PMU_TIME1_MIN_TIME && time_ms <= WM_800_PMU_TIME1_MAX_TIME) {
        /** abort timer0, avoidance timer0 is running */
        wm_hal_pmu_abort_timer0(&dev_pmu->hal_pmu);

        if (wm_hal_pmu_set_timer1(&dev_pmu->hal_pmu, time_ms) != WM_ERR_SUCCESS) {
            wm_os_internal_mutex_release(dev_pmu->drv_ctx.mutex);
            WM_DRV_PMU_LOG_MARKER();
            return WM_ERR_FAILED;
        }
    } else if (time_ms >= WM_800_PMU_TIME0_MIN_TIME && time_ms <= WM_800_PMU_TIME0_MAX_TIME) {
        /** abort timer1, avoidance timer1 is running */
        wm_hal_pmu_abort_timer1(&dev_pmu->hal_pmu);

        if (wm_hal_pmu_set_timer0(&dev_pmu->hal_pmu, time_ms / 1000) != WM_ERR_SUCCESS) {
            wm_os_internal_mutex_release(dev_pmu->drv_ctx.mutex);
            WM_DRV_PMU_LOG_MARKER();
            return WM_ERR_FAILED;
        }
    } else {
        assert(0); /** it should not be executed */
    }

    wm_os_internal_mutex_release(dev_pmu->drv_ctx.mutex);

    return WM_ERR_SUCCESS;
}

int wm_drv_ops_pmu_abort_timer(wm_device_t *dev)
{
    if (dev == NULL || dev->drv == NULL) {
        return WM_ERR_FAILED;
    }

    wm_drv_pmu_data_t *dev_pmu = (wm_drv_pmu_data_t *)dev->drv;

    if (wm_os_internal_mutex_acquire(dev_pmu->drv_ctx.mutex, WM_OS_WAIT_TIME_MAX) != WM_ERR_SUCCESS) {
        WM_DRV_PMU_LOG_MARKER();
        return WM_ERR_FAILED;
    }

    if (wm_hal_pmu_abort_timer0(&dev_pmu->hal_pmu) != WM_ERR_SUCCESS) {
        wm_os_internal_mutex_release(dev_pmu->drv_ctx.mutex);
        WM_DRV_PMU_LOG_MARKER();
        return WM_ERR_FAILED;
    }

    if (wm_hal_pmu_abort_timer1(&dev_pmu->hal_pmu) != WM_ERR_SUCCESS) {
        wm_os_internal_mutex_release(dev_pmu->drv_ctx.mutex);
        WM_DRV_PMU_LOG_MARKER();
        return WM_ERR_FAILED;
    }

    wm_os_internal_mutex_release(dev_pmu->drv_ctx.mutex);

    return WM_ERR_SUCCESS;
}

int wm_drv_ops_pmu_register_irq_callback(wm_device_t *dev, wm_pmu_irq_type_t type, wm_pmu_callback_t callback, void *user_data)
{
    if (dev == NULL || dev->drv == NULL) {
        return WM_ERR_FAILED;
    }

    wm_drv_pmu_data_t *dev_pmu = (wm_drv_pmu_data_t *)dev->drv;

    if (type >= WM_PMU_IRQ_MAX || callback == NULL) {
        WM_DRV_PMU_LOG_MARKER();
        return WM_ERR_FAILED;
    }

    if (wm_os_internal_mutex_acquire(dev_pmu->drv_ctx.mutex, WM_OS_WAIT_TIME_MAX) != WM_ERR_SUCCESS) {
        WM_DRV_PMU_LOG_MARKER();
        return WM_ERR_FAILED;
    }

    dev_pmu->drv_ctx.pmu_irq[type].cb        = callback;
    dev_pmu->drv_ctx.pmu_irq[type].user_data = user_data;

    wm_os_internal_mutex_release(dev_pmu->drv_ctx.mutex);

    return WM_ERR_SUCCESS;
}

int wm_drv_ops_pmu_unregister_irq_callback(wm_device_t *dev, wm_pmu_irq_type_t type)
{
    if (dev == NULL || dev->drv == NULL) {
        return WM_ERR_FAILED;
    }

    if (type >= WM_PMU_IRQ_MAX) {
        WM_DRV_PMU_LOG_MARKER();
        return WM_ERR_FAILED;
    }

    wm_drv_pmu_data_t *dev_pmu = (wm_drv_pmu_data_t *)dev->drv;

    if (wm_os_internal_mutex_acquire(dev_pmu->drv_ctx.mutex, WM_OS_WAIT_TIME_MAX) != WM_ERR_SUCCESS) {
        WM_DRV_PMU_LOG_MARKER();
        return WM_ERR_FAILED;
    }

    dev_pmu->drv_ctx.pmu_irq[type].cb        = NULL;
    dev_pmu->drv_ctx.pmu_irq[type].user_data = NULL;

    wm_os_internal_mutex_release(dev_pmu->drv_ctx.mutex);

    return WM_ERR_SUCCESS;
}

const wm_drv_pmu_ops_t wm_drv_pmu_ops = {
    /* Init */
    .init   = wm_drv_ops_pmu_init,
    .deinit = wm_drv_ops_pmu_deinit,
    /* Clock */
    .set_clock_source = wm_drv_ops_pmu_set_clock_source,
    /* Low power mode */
    .set_power_mode = wm_drv_ops_pmu_set_power_mode,
    /* Query status before wakeup */
    .get_status_before_wakeup = wm_drv_ops_pmu_get_status_before_wakeup,
    /* Timer */
    .set_timer   = wm_drv_ops_pmu_set_timer,
    .abort_timer = wm_drv_ops_pmu_abort_timer,
    /* Register irq callback */
    .register_irq_cb   = wm_drv_ops_pmu_register_irq_callback,
    .unregister_irq_cb = wm_drv_ops_pmu_unregister_irq_callback,
};

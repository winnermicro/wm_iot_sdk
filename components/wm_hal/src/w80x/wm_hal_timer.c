/**
 * @file wm_hal_timer.c
 *
 * @brief Timer Module
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

#include "wm_types.h"
#include "wm_error.h"
#include "wm_irq.h"
#include "wm_regs_base.h"
#include "wm_hal_irq.h"
#include "wm_hal_timer.h"

wm_hal_timer_dev_t g_timer_hal_dev[WM_TIMER_ID_MAX] = { 0 };

static void wm_timer_irq_handle(wm_irq_no_t irq, void *arg)
{
    int i                         = 0;
    uint32_t timer_csr            = 0;
    wm_hal_timer_dev_t *hal_timer = NULL;

    if (arg) {
        hal_timer = (wm_hal_timer_dev_t *)arg;

        timer_csr = wm_ll_timer_get_csr((wm_timer_reg_t *)hal_timer->dev);
        wm_ll_timer_set_csr((wm_timer_reg_t *)hal_timer->dev, timer_csr);

        for (i = 0; i < WM_TIMER_ID_MAX; i++) {
            if (timer_csr & WM_TIMER_INT_CLR(i)) {
                if (g_timer_hal_dev[i].callback) {
                    g_timer_hal_dev[i].callback(g_timer_hal_dev[i].arg);
                }
            }
        }
    }
}

int wm_hal_timer_init(wm_hal_timer_dev_t *hal_timer, wm_hal_timer_dev_cfg_t dev_cfg)
{
    static uint8_t init = 0;

    if (!init) {
        init = 1;
        wm_hal_irq_enable(WM_IRQ_TIMER);
        wm_hal_irq_attach_sw_vector(WM_IRQ_TIMER, wm_timer_irq_handle, hal_timer);
    }

    if (dev_cfg.clock && hal_timer) {
        wm_ll_timer_set_source_clock(hal_timer->dev, dev_cfg.clock - 1);
        return WM_ERR_SUCCESS;
    }

    return WM_ERR_INVALID_PARAM;
}

int wm_hal_timer_start(wm_hal_timer_dev_t *hal_timer, wm_timer_id_t timer_id, wm_hal_timer_cfg_t timer_cfg)
{
    if (!WM_HAL_TIMER_IS_TIMERID(timer_id) || !hal_timer) {
        return WM_ERR_INVALID_PARAM;
    }

    // stop
    wm_ll_timer_clr_irq_status(hal_timer->dev, timer_id);
    wm_ll_timer_set_disable(hal_timer->dev, timer_id);

    if (WM_HAL_TIMER_UNIT_MS == timer_cfg.unit) {
        wm_ll_timer_set_unit_ms(hal_timer->dev, timer_id);
    } else if (WM_HAL_TIMER_UNIT_US == timer_cfg.unit) {
        wm_ll_timer_set_unit_us(hal_timer->dev, timer_id);
    } else {
        return WM_ERR_INVALID_PARAM;
    }

    wm_ll_timer_set_auto_reload(hal_timer->dev, timer_id, timer_cfg.auto_reload);

    if (timer_cfg.period) {
        wm_ll_timer_set_timing_value(hal_timer->dev, timer_id, timer_cfg.period);
    } else {
        return WM_ERR_INVALID_PARAM;
    }

    wm_ll_timer_set_irq_enable(hal_timer->dev, timer_id);
    wm_ll_timer_set_enable(hal_timer->dev, timer_id);

    return WM_ERR_SUCCESS;
}

int wm_hal_timer_stop(wm_hal_timer_dev_t *hal_timer, wm_timer_id_t timer_id)
{
    if (!WM_HAL_TIMER_IS_TIMERID(timer_id) || !hal_timer) {
        return WM_ERR_INVALID_PARAM;
    }

    wm_ll_timer_clr_irq_status(hal_timer->dev, timer_id);
    wm_ll_timer_set_disable(hal_timer->dev, timer_id);

    return WM_ERR_SUCCESS;
}

int wm_hal_timer_restart(wm_hal_timer_dev_t *hal_timer, wm_timer_id_t timer_id)
{
    if (!WM_HAL_TIMER_IS_TIMERID(timer_id) || !hal_timer) {
        return WM_ERR_INVALID_PARAM;
    }

    // stop
    wm_ll_timer_clr_irq_status(hal_timer->dev, timer_id);
    wm_ll_timer_set_disable(hal_timer->dev, timer_id);

    // start
    wm_ll_timer_set_irq_enable(hal_timer->dev, timer_id);
    wm_ll_timer_set_enable(hal_timer->dev, timer_id);
    return WM_ERR_SUCCESS;
}

int wm_hal_timer_set_period(wm_hal_timer_dev_t *hal_timer, wm_timer_id_t timer_id, uint32_t period)
{
    if (!WM_HAL_TIMER_IS_TIMERID(timer_id) || !period || !hal_timer) {
        return WM_ERR_INVALID_PARAM;
    }

    // stop
    wm_ll_timer_clr_irq_status(hal_timer->dev, timer_id);
    wm_ll_timer_set_disable(hal_timer->dev, timer_id);

    // set
    wm_ll_timer_set_timing_value(hal_timer->dev, timer_id, period);

    // start
    wm_ll_timer_set_irq_enable(hal_timer->dev, timer_id);
    wm_ll_timer_set_enable(hal_timer->dev, timer_id);
    return WM_ERR_SUCCESS;

    return WM_ERR_SUCCESS;
}

int wm_hal_timer_get_period(wm_hal_timer_dev_t *hal_timer, wm_timer_id_t timer_id, uint32_t *period)
{
    if (!WM_HAL_TIMER_IS_TIMERID(timer_id) || !hal_timer) {
        return WM_ERR_INVALID_PARAM;
    }

    *period = wm_ll_timer_get_timing_value(hal_timer->dev, timer_id);
    return WM_ERR_SUCCESS;
}

int wm_hal_timer_set_auto_reload(wm_hal_timer_dev_t *hal_timer, wm_timer_id_t timer_id, bool auto_reload)
{
    if (!WM_HAL_TIMER_IS_TIMERID(timer_id) || !hal_timer) {
        return WM_ERR_INVALID_PARAM;
    }

    // stop
    wm_ll_timer_clr_irq_status(hal_timer->dev, timer_id);
    wm_ll_timer_set_disable(hal_timer->dev, timer_id);

    // set
    wm_ll_timer_set_auto_reload(hal_timer->dev, timer_id, auto_reload);

    // start
    wm_ll_timer_set_irq_enable(hal_timer->dev, timer_id);
    wm_ll_timer_set_enable(hal_timer->dev, timer_id);

    return WM_ERR_SUCCESS;
}

int wm_hal_timer_get_autoreload(wm_hal_timer_dev_t *hal_timer, wm_timer_id_t timer_id, bool *auto_reload)
{
    if (!WM_HAL_TIMER_IS_TIMERID(timer_id) || !hal_timer) {
        return WM_ERR_INVALID_PARAM;
    }

    *auto_reload = wm_ll_timer_get_auto_reload(hal_timer->dev, timer_id);
    return WM_ERR_SUCCESS;
}

int wm_hal_timer_get_counter(wm_hal_timer_dev_t *hal_timer, wm_timer_id_t timer_id, uint32_t *counter)
{
    if (!WM_HAL_TIMER_IS_TIMERID(timer_id) || !hal_timer || !counter) {
        return WM_ERR_INVALID_PARAM;
    }

    *counter = wm_ll_timer_get_current_value(hal_timer->dev, timer_id);
    return WM_ERR_SUCCESS;
}

int wm_hal_timer_get_irq_status(wm_hal_timer_dev_t *hal_timer, wm_timer_id_t timer_id, uint32_t *status)
{
    if (!WM_HAL_TIMER_IS_TIMERID(timer_id) || !hal_timer) {
        return WM_ERR_INVALID_PARAM;
    }

    *status = wm_ll_timer_get_irq_status(hal_timer->dev, timer_id);
    return WM_ERR_SUCCESS;
}

int wm_hal_timer_clear_irq_status(wm_hal_timer_dev_t *hal_timer, wm_timer_id_t timer_id)
{
    if (!WM_HAL_TIMER_IS_TIMERID(timer_id) || !hal_timer) {
        return WM_ERR_INVALID_PARAM;
    }

    wm_ll_timer_clr_irq_status(hal_timer->dev, timer_id);
    return WM_ERR_SUCCESS;
}

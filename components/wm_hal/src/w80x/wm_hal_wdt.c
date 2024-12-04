/**
 * @file wm_hal_wdt.c
 *
 * @brief Watchdog Timer Module
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

#include "wm_error.h"
#include "wm_soc.h"
#include "wm_hal_wdt.h"
#include "wm_hal_irq.h"
#include "wm_irq.h"
#include "wm_debug.h"

#define LOG_TAG "WDT_HAL"
#include "wm_log.h"

static void wm_wdt_irq_handle(wm_irq_no_t irq, void *arg)
{
    if (arg) {
        wm_hal_wdt_dev_t *dev = (wm_hal_wdt_dev_t *)arg;

        wm_hal_wdt_set_counter_value(dev, 1);
    }
    wm_set_reboot_reason(WM_REBOOT_REASON_WDG_TIMEOUT);
}

int wm_hal_wdt_init(wm_hal_wdt_dev_t *dev)
{
    if (WM_ERR_SUCCESS == wm_hal_irq_attach_sw_vector(dev->irq_num, wm_wdt_irq_handle, dev)) {
        wm_hal_irq_enable(dev->irq_num);
        wm_ll_wdt_clear_interrupt((wm_wdt_reg_t *)dev->register_base);
        wm_ll_wdt_set_load_value((wm_wdt_reg_t *)dev->register_base, dev->period);
        wm_ll_wdt_enable_reset((wm_wdt_reg_t *)dev->register_base);
        wm_ll_wdt_enable((wm_wdt_reg_t *)dev->register_base);
        return WM_ERR_SUCCESS;
    }

    return WM_ERR_FAILED;
}

int wm_hal_wdt_deinit(wm_hal_wdt_dev_t *dev)
{
    wm_ll_wdt_disable((wm_wdt_reg_t *)dev->register_base);
    wm_ll_wdt_disable_reset((wm_wdt_reg_t *)dev->register_base);
    wm_ll_wdt_clear_interrupt((wm_wdt_reg_t *)dev->register_base);
    wm_hal_irq_disable(dev->irq_num);

    return WM_ERR_SUCCESS;
}

int wm_hal_wdt_start(wm_hal_wdt_dev_t *dev)
{
    wm_ll_wdt_enable((wm_wdt_reg_t *)dev->register_base);

    return WM_ERR_SUCCESS;
}

int wm_hal_wdt_stop(wm_hal_wdt_dev_t *dev)
{
    wm_ll_wdt_disable((wm_wdt_reg_t *)dev->register_base);

    return WM_ERR_SUCCESS;
}

int wm_hal_wdt_feed(wm_hal_wdt_dev_t *dev)
{
    wm_ll_wdt_feed((wm_wdt_reg_t *)dev->register_base);

    return WM_ERR_SUCCESS;
}

int wm_hal_wdt_get_status(wm_hal_wdt_dev_t *dev)
{
    if (wm_ll_wdt_get_status((wm_wdt_reg_t *)dev->register_base) == 0) {
        return WM_WDT_STATE_OFF;
    } else {
        return WM_WDT_STATE_ON;
    }
}

int wm_hal_wdt_set_counter_value(wm_hal_wdt_dev_t *dev, uint32_t conuter_value)
{
    // stop wdt
    wm_ll_wdt_disable((wm_wdt_reg_t *)dev->register_base);
    wm_ll_wdt_disable_reset((wm_wdt_reg_t *)dev->register_base);
    wm_ll_wdt_clear_interrupt((wm_wdt_reg_t *)dev->register_base);

    // set counter value
    wm_ll_wdt_set_load_value((wm_wdt_reg_t *)dev->register_base, conuter_value);

    // start wdt
    wm_ll_wdt_clear_interrupt((wm_wdt_reg_t *)dev->register_base);
    wm_ll_wdt_enable_reset((wm_wdt_reg_t *)dev->register_base);
    wm_ll_wdt_enable((wm_wdt_reg_t *)dev->register_base);

    return WM_ERR_SUCCESS;
}

int wm_hal_wdt_get_counter_value(wm_hal_wdt_dev_t *dev, uint32_t *conuter_value)
{
    // set counter value
    *conuter_value = wm_ll_wdt_get_load_value((wm_wdt_reg_t *)dev->register_base);

    return WM_ERR_SUCCESS;
}

int wm_hal_wdt_get_current_value(wm_hal_wdt_dev_t *dev, uint32_t *current_value)
{
    // get current value
    *current_value = wm_ll_wdt_get_current_value((wm_wdt_reg_t *)dev->register_base);

    return WM_ERR_SUCCESS;
}

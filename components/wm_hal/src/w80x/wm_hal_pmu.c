/**
 * @file wm_hal_pmu.c
 *
 * @brief HAL_PMU Module
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
#include "wm_hal_pmu.h"
#include "wm_ll_pmu.h"

#include <assert.h>
#define LOG_TAG "HAL_PMU"
#include "wm_log.h"

#define WM_HAL_PMU_LOG_MARKER() wm_log_debug("%s@%d\n", __func__, __LINE__)

static void wm_hal_pmu_irq_handle(wm_irq_no_t irq, void *arg)
{
    wm_hal_pmu_dev_t *dev = (wm_hal_pmu_dev_t *)arg;

    assert(dev != NULL);

    /** PMU timer0 interrupt */
    if (wm_ll_pmu_get_timer0_isr_flag(dev->reg_base)) {
        wm_ll_pmu_clr_timer0_isr_flag(dev->reg_base);
        /** must stop timer0, otherwise, it will run periodically */
        wm_ll_pmu_set_timer0_enable(dev->reg_base, false);

        if (dev->call_back != NULL) {
            dev->call_back(WM_PMU_IRQ_TIMER, dev->user_data);
        }
    }

    /** PMU timer1 interrupt */
    if (wm_ll_pmu_get_timer1_isr_flag(dev->reg_base)) {
        wm_ll_pmu_clr_timer1_isr_flag(dev->reg_base);
        /** must stop timer1, otherwise, it will run periodically */
        wm_ll_pmu_set_timer1_ctrl(dev->reg_base, 0, false, 0, 0, 0);

        if (dev->call_back != NULL) {
            dev->call_back(WM_PMU_IRQ_TIMER, dev->user_data);
        }
    }

    /** PMU wakeup IO interrupt */
    if (wm_ll_pmu_get_io_wake_isr_flag(dev->reg_base)) {
        if (dev->call_back != NULL) {
            dev->call_back(WM_PMU_IRQ_IO_WAKE, dev->user_data);
        }
        wm_ll_pmu_clr_io_wake_isr_flag(dev->reg_base);
    }

    ///todo: will refactor this
    /** RTC interrupt */
    if (wm_ll_pmu_get_rtc_isr_flag(dev->reg_base)) {
        if (dev->call_back != NULL) {
            dev->call_back(WM_PMU_IRQ_RTC, dev->user_data);
        }
        wm_ll_pmu_clr_rtc_isr_flag(dev->reg_base);
    }
    wm_hal_irq_enable(dev->irq_num);
}

int wm_hal_pmu_init(wm_hal_pmu_dev_t *dev)
{
    int ret = WM_ERR_SUCCESS;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->is_init) {
        return WM_ERR_ALREADY_INITED;
    }

    /* 32K switch to use RC circuit & calibration */
    wm_ll_pmu_set_32k_bypass(dev->reg_base, false);
    wm_ll_pmu_set_32k_calibration_enable(dev->reg_base, true);

    /** Switch to DBG */
    wm_ll_pmu_set_xtal_buf_bg_mux(dev->reg_base, false);
    wm_ll_pmu_set_dldo_core_sel(dev->reg_base, false);

    /** attach irq handle */
    ret = wm_hal_irq_attach_sw_vector(dev->irq_num, wm_hal_pmu_irq_handle, dev);
    if (ret != WM_ERR_SUCCESS) {
        WM_HAL_PMU_LOG_MARKER();
        return WM_ERR_FAILED;
    }

    /** enable pmu irq */
    ret = wm_hal_irq_enable(dev->irq_num);
    if (ret != WM_ERR_SUCCESS) {
        WM_HAL_PMU_LOG_MARKER();
        return WM_ERR_FAILED;
    }
    dev->is_init = true;

    return WM_ERR_SUCCESS;
}

int wm_hal_pmu_deinit(wm_hal_pmu_dev_t *dev)
{
    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    /** This is dummy function, it will not uninstall PMU. */
    return WM_ERR_SUCCESS;
}

int wm_hal_pmu_set_clock_source(wm_hal_pmu_dev_t *dev, wm_pmu_clock_source_t clk_src)
{
    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    switch (clk_src) {
        case WM_PMU_CLOCK_SRC_32K:
        {
            wm_ll_pmu_set_32k_bypass(dev->reg_base, false);
            break;
        }
        case WM_PMU_CLOCK_SRC_40M_DIV:
        {
            wm_ll_pmu_set_32k_bypass(dev->reg_base, true);
            break;
        }
        default:
        {
            WM_HAL_PMU_LOG_MARKER();
            return WM_ERR_INVALID_PARAM;
        }
    }

    return WM_ERR_SUCCESS;
}

int wm_hal_pmu_calibration_internal_rc(wm_hal_pmu_dev_t *dev)
{
    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    wm_ll_pmu_set_32k_calibration_enable(dev->reg_base, true);

    return WM_ERR_SUCCESS;
}

int wm_hal_pmu_set_power_mode(wm_hal_pmu_dev_t *dev, wm_pmu_power_mode_t mode)
{
    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    switch (mode) {
        case WM_PMU_POWER_MODE_SLEEP:
        case WM_PMU_POWER_MODE_STANDBY:
        {
            if (wm_ll_pmu_get_32k_bypass(dev->reg_base)) {
                wm_ll_pmu_set_wlan_xtal_40m_enable(dev->reg_base, true);
            }

            /** Writing a PMU register requires 250us,
            *   To improve efficiency, clear all three states at once here.
            */
            // wm_ll_pmu_clr_io_wake_isr_flag(dev->reg_base);
            // wm_ll_pmu_clr_sleep_status(dev->reg_base);
            // wm_ll_pmu_clr_standby_status(dev->reg_base);

            wm_pmu_isr_t fake_pmu_isr = { .io_wake_isr_flag = 1, .last_sleep_state = 1, .standby_status = 1 };

            dev->reg_base->pmu_isr.val = fake_pmu_isr.val;

            if (WM_PMU_POWER_MODE_SLEEP == mode) {
                wm_ll_pmu_set_sleep_enable(dev->reg_base, true);
            } else {
                /** HW limitation: must switch to 32K clock, otherwise standby may not wake up */
                wm_ll_pmu_set_32k_bypass(dev->reg_base, false);

                wm_ll_pmu_set_standby_enable(dev->reg_base, true);
            }
            break;
        }
        default:
        {
            return WM_ERR_INVALID_PARAM;
        }
    }
    return WM_ERR_SUCCESS;
}

int wm_hal_pmu_set_wakeup_io(wm_hal_pmu_dev_t *dev, wm_pmu_wakeup_io_config_t *config)
{
    if (dev == NULL || config == NULL || config->wake_thres_level > 15) {
        return WM_ERR_INVALID_PARAM;
    }

    wm_ll_pmu_set_wake_thres_time(dev->reg_base, config->wake_thres_level);

    return WM_ERR_SUCCESS;
}

int wm_hal_pmu_register_callback(wm_hal_pmu_dev_t *dev, hal_pmu_callback_t call_back, void *user_data)
{
    if (dev == NULL || call_back == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    dev->call_back = call_back;
    dev->user_data = user_data;

    return WM_ERR_SUCCESS;
}

int wm_hal_pmu_get_status_before_wakeup(wm_hal_pmu_dev_t *dev, wm_pmu_wakeup_pre_status_t *status)
{
    if (dev == NULL || status == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (wm_ll_pmu_get_sleep_status(dev->reg_base)) {
        *status = WM_PMU_WAKEUP_PRE_STATUS_SLEEP;
    } else if (wm_ll_pmu_get_standby_status(dev->reg_base)) {
        *status = WM_PMU_WAKEUP_PRE_STATUS_STANDBY;
    } else {
        *status = WM_PMU_WAKEUP_PRE_STATUS_NONE;
    }

    return WM_ERR_SUCCESS;
}

int wm_hal_pmu_set_timer0(wm_hal_pmu_dev_t *dev, uint32_t time_s)
{
    if (dev == NULL || !time_s || time_s > 0xFFFF) {
        return WM_ERR_INVALID_PARAM;
    }

    wm_ll_pmu_set_32k_calibration_enable(dev->reg_base, true);
    wm_ll_pmu_set_timer0_value(dev->reg_base, time_s);
    wm_ll_pmu_set_timer0_enable(dev->reg_base, true);

    return WM_ERR_SUCCESS;
}

int wm_hal_pmu_abort_timer0(wm_hal_pmu_dev_t *dev)
{
    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    wm_ll_pmu_set_timer0_enable(dev->reg_base, false);

    return WM_ERR_SUCCESS;
}

int wm_hal_pmu_set_timer1(wm_hal_pmu_dev_t *dev, uint32_t time_ms)
{
    if (dev == NULL || !time_ms || time_ms > 0xFFFF) {
        return WM_ERR_INVALID_PARAM;
    }

    uint16_t listen_interval = time_ms;

    wm_ll_pmu_set_32k_calibration_enable(dev->reg_base, true);
    if (wm_ll_pmu_get_32k_bypass(dev->reg_base)) {
        /** 40M clk frequency division */
        /** Less than 2ms, call wm_drv_pmu_set_timer & wm_drv_pmu_set_power_mode
         * unable to wake up device, there is 1ms margin reserved here.
        */
        if (time_ms < 3) {
            listen_interval = 2;
        } else {
            listen_interval = time_ms - 1;
        }
    } else {
        /** 32K RC, HW limitation: compensate clock */
        if (time_ms < 5) {
            listen_interval = 0;
        } else {
            listen_interval = time_ms - 5;
        }
    }

    wm_ll_pmu_set_timer1_ctrl(dev->reg_base, listen_interval, true, 0, 0, 0);

    return WM_ERR_SUCCESS;
}

int wm_hal_pmu_abort_timer1(wm_hal_pmu_dev_t *dev)
{
    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    wm_ll_pmu_set_timer1_ctrl(dev->reg_base, 0, false, 0, 0, 0);

    return WM_ERR_SUCCESS;
}

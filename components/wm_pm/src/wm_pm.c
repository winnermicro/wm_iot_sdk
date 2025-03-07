/**
 * @file wm_pm.c
 *
 * @brief PM Module
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
#include "wm_error.h"
#include "wm_pm.h"
#include "wm_irq.h"
#include "wm_soc.h"
#include "wm_drv_rcc.h"
#include "wm_drv_pmu.h"
#include "wm_drv_irq.h"
#include "wm_drv_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

struct wm_pm_context {
    wm_pm_config_t config;

    wm_device_t *timer;
    wm_device_t *rcc;
    wm_device_t *pmu;

    wm_os_sem_t *lock;

    bool is_timer_wakeup;
};

struct wm_pm_timer_ctx {
    wm_hal_timer_dev_t *hal;
    void *drv;
};

static struct wm_pm_context g_wm_pm_ctx                  = { 0 };
static wm_irq_no_t g_wm_pm_irq_list[WM_IRQ_SYS_INTR_NUM] = { WM_IRQ_MAX };

static void wm_pm_timer_callback(void *arg)
{
    g_wm_pm_ctx.is_timer_wakeup = true;
}

int wm_pm_get_config(wm_pm_config_t *config)
{
    if (!config) {
        return WM_ERR_INVALID_PARAM;
    }

    memcpy(config, &g_wm_pm_ctx.config, sizeof(wm_pm_config_t));

    return WM_ERR_SUCCESS;
}

int wm_pm_set_config(wm_pm_config_t *config)
{
    uint8_t i;

    if (!config) {
        return WM_ERR_INVALID_PARAM;
    }

    if (config->mode == g_wm_pm_ctx.config.mode)
        return WM_ERR_SUCCESS;

    if ((!config->mode && g_wm_pm_ctx.config.mode) || (config->mode != g_wm_pm_ctx.config.mode)) {
        if (g_wm_pm_ctx.lock) {
            wm_os_internal_sem_delete(g_wm_pm_ctx.lock);
            g_wm_pm_ctx.lock = NULL;
        }
        if (g_wm_pm_ctx.timer) {
            wm_drv_timer_deinit(g_wm_pm_ctx.timer);
            g_wm_pm_ctx.timer = NULL;
        }
        if (g_wm_pm_ctx.pmu) {
            wm_drv_pmu_deinit(g_wm_pm_ctx.pmu);
            g_wm_pm_ctx.pmu = NULL;
        }
        //rcc cannot be de-initialized

        for (i = 0; i < WM_IRQ_SYS_INTR_NUM; i++) {
            if (WM_IRQ_MAX != g_wm_pm_irq_list[i]) {
                wm_drv_irq_clear_wakeup(g_wm_pm_irq_list[i]);
                g_wm_pm_irq_list[i] = WM_IRQ_MAX;
            }
        }

        wm_drv_irq_clear_wakeup(WM_IRQ_TIMER);
    }

    if (config->mode) {
        wm_os_internal_sem_create(&g_wm_pm_ctx.lock, 1);
        if (!g_wm_pm_ctx.lock) {
            return WM_ERR_NO_MEM;
        }

        g_wm_pm_ctx.rcc = wm_drv_clock_init("rcc");
        if (!g_wm_pm_ctx.rcc) {
            wm_os_internal_sem_delete(g_wm_pm_ctx.lock);
            g_wm_pm_ctx.lock = NULL;
            return WM_ERR_FAILED;
        }

        if (WM_PM_MODE_LIGHT_SLEEP == config->mode) {
            g_wm_pm_ctx.timer = wm_drv_timer_init(CONFIG_PM_WAKEUP_TIMER_DEVICE);
            if (!g_wm_pm_ctx.timer) {
                wm_os_internal_sem_delete(g_wm_pm_ctx.lock);
                g_wm_pm_ctx.lock = NULL;
                wm_drv_clock_deinit(g_wm_pm_ctx.rcc);
                g_wm_pm_ctx.rcc = NULL;
                return WM_ERR_FAILED;
            }

            wm_drv_timer_register_callback(g_wm_pm_ctx.timer, wm_pm_timer_callback, NULL);

            for (i = 0; i < WM_IRQ_SYS_INTR_NUM; i++) {
                if (config->wakeup_dev_bitmap & WM_BIT(i)) {
                    g_wm_pm_irq_list[i] = i;
                    wm_drv_irq_set_wakeup(g_wm_pm_irq_list[i]);
                }
            }

            wm_drv_irq_set_wakeup(WM_IRQ_TIMER);
        } else if ((WM_PM_MODE_DEEP_SLEEP == config->mode) || (WM_PM_MODE_STANDBY == config->mode)) {
            g_wm_pm_ctx.pmu = wm_dt_get_device_by_name("pmu");
            if (!g_wm_pm_ctx.pmu) {
                wm_os_internal_sem_delete(g_wm_pm_ctx.lock);
                g_wm_pm_ctx.lock = NULL;
                wm_drv_clock_deinit(g_wm_pm_ctx.rcc);
                g_wm_pm_ctx.rcc = NULL;
                return WM_ERR_FAILED;
            }

            wm_drv_pmu_set_clock_source(g_wm_pm_ctx.pmu, WM_PMU_CLOCK_SRC_32K);
            wm_drv_pmu_register_irq_callback(g_wm_pm_ctx.pmu, WM_PMU_IRQ_TIMER, wm_pm_timer_callback, NULL);
        }
    }

    memcpy(&g_wm_pm_ctx.config, config, sizeof(wm_pm_config_t));

    return WM_ERR_SUCCESS;
}

int wm_pm_lock_acquire(void)
{
    wm_os_status_t err;

    if (!g_wm_pm_ctx.lock)
        return WM_ERR_NOT_ALLOWED;

    err = wm_os_internal_sem_acquire(g_wm_pm_ctx.lock, WM_OS_WAIT_TIME_MAX);

    return err;
}

int wm_pm_lock_release(void)
{
    if (!g_wm_pm_ctx.lock)
        return WM_ERR_NOT_ALLOWED;

    if (wm_os_internal_sem_get_count(g_wm_pm_ctx.lock))
        return WM_ERR_SUCCESS;

    return wm_os_internal_sem_release(g_wm_pm_ctx.lock);
}

int wm_pm_enter_sleep(uint32_t ticks)
{
    int ret = WM_ERR_FAILED;
    wm_os_status_t err;
    uint32_t slee_time_ms             = 0;
    uint32_t slept_time_ms            = 0;
    wm_hal_timer_cfg_t timer_cfg      = { 0 };
    wm_drv_clock_ctx_t *rcc_drv_ctx   = NULL;
    wm_hal_clock_dev_t *rcc_hal_dev   = NULL;
    wm_hal_timer_dev_t *timer_hal_dev = NULL;
    wm_hal_pmu_dev_t *pmu_hal_dev     = NULL;

    //check sleep mode
    if (!g_wm_pm_ctx.config.mode || !g_wm_pm_ctx.lock) {
        return WM_ERR_NOT_ALLOWED;
    }

    err = wm_os_internal_sem_acquire(g_wm_pm_ctx.lock, 0);
    if (err) {
        return WM_ERR_NOT_ALLOWED;
    }

    //close perihs clock
    if (g_wm_pm_ctx.config.sleep_dev_bitmap) {
        rcc_drv_ctx = (wm_drv_clock_ctx_t *)g_wm_pm_ctx.rcc->drv;
        rcc_hal_dev = &(rcc_drv_ctx->hal_dev);
        wm_hal_clock_disable(rcc_hal_dev, g_wm_pm_ctx.config.sleep_dev_bitmap & (~WM_PM_SLEEP_DEV_TYPE_TIMER));
    }

    //set wakeup source
    //set wakeup timer
    //wfi or pmu
    slee_time_ms = ticks * portTICK_PERIOD_MS;
    if (WM_PM_MODE_LIGHT_SLEEP == g_wm_pm_ctx.config.mode) {
        timer_cfg.unit        = WM_HAL_TIMER_UNIT_MS;
        timer_cfg.auto_reload = false;
        timer_cfg.period      = slee_time_ms;
        timer_hal_dev         = ((struct wm_pm_timer_ctx *)(g_wm_pm_ctx.timer->drv))->hal;
        ret                   = wm_hal_timer_start(timer_hal_dev, CONFIG_PM_WAKEUP_TIMER_INDEX, timer_cfg);
        if (WM_ERR_SUCCESS != ret) {
            return ret;
        }

        g_wm_pm_ctx.is_timer_wakeup = false;

        __WAIT();
    } else if ((WM_PM_MODE_DEEP_SLEEP == g_wm_pm_ctx.config.mode) || (WM_PM_MODE_STANDBY == g_wm_pm_ctx.config.mode)) {
#if CONFIG_CHIP_W80X
        /* the timer will run immediately after setting the pmu timer,
           and if the time is too short it will not have time to sleep before it is already triggered */
        if (slee_time_ms <= 8)
            goto end;

        pmu_hal_dev = (wm_hal_pmu_dev_t *)g_wm_pm_ctx.pmu->drv;
        if (slee_time_ms >= WM_PMU_TIME1_MIN_TIME && slee_time_ms <= WM_PMU_TIME1_MAX_TIME) {
            wm_hal_pmu_abort_timer0(pmu_hal_dev);
            ret = wm_hal_pmu_set_timer1(pmu_hal_dev, slee_time_ms);
        } else if (slee_time_ms >= WM_PMU_TIME0_MIN_TIME && slee_time_ms <= WM_PMU_TIME0_MAX_TIME) {
            wm_hal_pmu_abort_timer1(pmu_hal_dev);
            ret = wm_hal_pmu_set_timer0(pmu_hal_dev, slee_time_ms / 1000);
        } else if (slee_time_ms > WM_PMU_TIME0_MAX_TIME) {
            wm_hal_pmu_abort_timer1(pmu_hal_dev);
            ticks = pdMS_TO_TICKS(WM_PMU_TIME0_MAX_TIME);
            ret   = wm_hal_pmu_set_timer0(pmu_hal_dev, WM_PMU_TIME0_MAX_TIME / 1000);
        }

        if (WM_ERR_SUCCESS == ret) {
            g_wm_pm_ctx.is_timer_wakeup = false;
            ret = wm_hal_pmu_set_power_mode(pmu_hal_dev, WM_PM_MODE_DEEP_SLEEP == g_wm_pm_ctx.config.mode ?
                                                             WM_PMU_POWER_MODE_SLEEP :
                                                             WM_PMU_POWER_MODE_STANDBY);
            //after pmu sleep starts, it needs to wait for about 6 32k clock cycles
            udelay(185); //for (int j = 0; j < 1700; j++) __NOP();
        }
#endif

        if (WM_ERR_SUCCESS != ret) {
            goto end;
        }
    }

    //get timer remain time
    if (WM_PM_MODE_LIGHT_SLEEP == g_wm_pm_ctx.config.mode) {
        ret = wm_hal_timer_stop(timer_hal_dev, CONFIG_PM_WAKEUP_TIMER_INDEX);
        if (g_wm_pm_ctx.is_timer_wakeup) {
            vTaskStepTick(ticks);
        } else {
            ret |= wm_hal_timer_get_counter(timer_hal_dev, CONFIG_PM_WAKEUP_TIMER_INDEX, &slept_time_ms);
            if (WM_ERR_SUCCESS == ret) {
                vTaskStepTick(pdMS_TO_TICKS(slept_time_ms));
            }
        }
    } else {
        if (g_wm_pm_ctx.is_timer_wakeup) {
            vTaskStepTick(ticks);
        } else {
#if CONFIG_CHIP_W80X
            //w80x can't get how long it's actually been asleep, so someone else needs to tell it.
            if (g_wm_pm_ctx.config.get_slept) {
                vTaskStepTick(pdMS_TO_TICKS(g_wm_pm_ctx.config.get_slept()));
            }
#endif
        }
    }

end:
    //open perihs clock
    if (g_wm_pm_ctx.config.sleep_dev_bitmap) {
        wm_hal_clock_enable(rcc_hal_dev, g_wm_pm_ctx.config.sleep_dev_bitmap);
    }

    wm_os_internal_sem_reset(g_wm_pm_ctx.lock);
    wm_os_internal_sem_release(g_wm_pm_ctx.lock);

    return ret;
}

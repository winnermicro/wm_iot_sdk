/**
 * @file wm_drv_timer.c
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

#include "wm_drv_ops_timer.c"
#define LOG_TAG "timer"
#include "wm_log.h"

wm_device_t *wm_drv_timer_init(char *dev_name)
{
    wm_device_t *timer_dev  = NULL;
    wm_drv_timer_ops_t *ops = NULL;
    int err                 = WM_ERR_INVALID_PARAM;

    timer_dev = wm_dt_get_device_by_name(dev_name);
    if (timer_dev && timer_dev->ops) {
        ops = timer_dev->ops;
        if (ops->init) {
            err = ops->init(timer_dev);
        }

        return err != WM_ERR_SUCCESS ? NULL : timer_dev;
    }

    return NULL;
}

int wm_drv_timer_deinit(wm_device_t *dev)
{
    wm_drv_timer_ops_t *ops = NULL;
    int err                 = WM_ERR_INVALID_PARAM;

    if (dev) {
        ops = dev->ops;
        if (ops && ops->deinit) {
            err = ops->deinit(dev);
        }
    }

    return err;
}

int wm_drv_timer_start(wm_device_t *dev, wm_drv_timer_cfg_t timer_cfg)
{
    wm_drv_timer_ops_t *ops = NULL;
    int err                 = WM_ERR_INVALID_PARAM;

    if (dev) {
        ops = dev->ops;
        if (ops && ops->start) {
            err = ops->start(dev, timer_cfg);
        }
    }

    return err;
}

int wm_drv_timer_stop(wm_device_t *dev)
{
    wm_drv_timer_ops_t *ops = NULL;
    int err                 = WM_ERR_INVALID_PARAM;

    if (dev) {
        ops = dev->ops;
        if (ops && ops->stop) {
            err = ops->stop(dev);
        }
    }

    return err;
}

int wm_drv_timer_set_period(wm_device_t *dev, uint32_t period)
{
    wm_drv_timer_ops_t *ops = NULL;
    int err                 = WM_ERR_INVALID_PARAM;

    if (dev) {
        ops = dev->ops;
        if (period && ops && ops->set_period) {
            err = ops->set_period(dev, period);
        }
    }

    return err;
}

int wm_drv_timer_set_auto_reload(wm_device_t *dev, bool auto_reload)
{
    wm_drv_timer_ops_t *ops = NULL;
    int err                 = WM_ERR_INVALID_PARAM;

    if (dev) {
        ops = dev->ops;
        if (ops && ops->set_auto_reload) {
            err = ops->set_auto_reload(dev, auto_reload);
        }
    }

    return err;
}

int wm_drv_timer_get_counter(wm_device_t *dev, uint32_t *counter)
{
    wm_drv_timer_ops_t *ops = NULL;
    int err                 = WM_ERR_INVALID_PARAM;

    if (dev) {
        ops = dev->ops;
        if (ops && ops->get_counter) {
            err = ops->get_counter(dev, counter);
        }
    }

    return err;
}

int wm_drv_timer_register_callback(wm_device_t *dev, wm_drv_timer_callback_t timer_callback, void *arg)
{
    wm_drv_timer_ops_t *ops = NULL;
    int err                 = WM_ERR_INVALID_PARAM;

    if (dev) {
        ops = dev->ops;
        if (ops && ops->add_callback) {
            err = ops->add_callback(dev, timer_callback, arg);
        }
    }

    return err;
}

int wm_drv_timer_unregister_callback(wm_device_t *dev)
{
    wm_drv_timer_ops_t *ops = NULL;
    int err                 = WM_ERR_INVALID_PARAM;

    if (dev) {
        ops = dev->ops;
        if (ops && ops->remove_callback) {
            err = ops->remove_callback(dev);
        }
    }

    return err;
}

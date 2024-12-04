/**
  * @file wm_drv_wdt.c
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
#include "wm_dt_hw.h"
#include "wm_dt.h"
#include "wm_drv_ops_wdt.c"

#define LOG_TAG "wdt"
#include "wm_log.h"

wm_device_t *wm_drv_wdt_init(char *dev_name)
{
    wm_device_t *wdt_dev  = NULL;
    wm_drv_wdt_ops_t *ops = NULL;
    int err               = WM_ERR_INVALID_PARAM;

    if (!strcmp(dev_name, "wdt")) {
        wdt_dev = wm_dt_get_device_by_name(dev_name);
        if (wdt_dev && wdt_dev->ops) {
            ops = wdt_dev->ops;
            if (ops->init) {
                err = ops->init(wdt_dev);
            }

            return err != WM_ERR_SUCCESS ? NULL : wdt_dev;
        }
    }

    return NULL;
}

int wm_drv_wdt_deinit(wm_device_t *dev)
{
    int err               = WM_ERR_INVALID_PARAM;
    wm_drv_wdt_ops_t *ops = NULL;

    if (dev && dev->ops) {
        ops = (wm_drv_wdt_ops_t *)dev->ops;
        if (ops->deinit) {
            err = ops->deinit(dev);
        }

        return err;
    }

    return err;
}

int wm_drv_wdt_start(wm_device_t *dev)
{
    int err               = WM_ERR_INVALID_PARAM;
    wm_drv_wdt_ops_t *ops = NULL;

    if (dev && dev->ops) {
        ops = (wm_drv_wdt_ops_t *)dev->ops;
        if (ops->start) {
            err = ops->start(dev);
        }

        return err;
    }

    return err;
}

int wm_drv_wdt_stop(wm_device_t *dev)
{
    int err               = WM_ERR_INVALID_PARAM;
    wm_drv_wdt_ops_t *ops = NULL;

    if (dev && dev->ops) {
        ops = (wm_drv_wdt_ops_t *)dev->ops;
        if (ops->stop) {
            err = ops->stop(dev);
        }

        return err;
    }

    return err;
}

int wm_drv_wdt_set_counter_value(wm_device_t *dev, uint32_t counter_value)
{
    int err               = WM_ERR_INVALID_PARAM;
    wm_drv_wdt_ops_t *ops = NULL;

    if (dev && dev->ops) {
        ops = (wm_drv_wdt_ops_t *)dev->ops;
        if (ops->set_counter_value) {
            err = ops->set_counter_value(dev, counter_value);
        }

        return err;
    }

    return err;
}

int wm_drv_wdt_get_counter_value(wm_device_t *dev, uint32_t *counter_value)
{
    int err               = WM_ERR_INVALID_PARAM;
    wm_drv_wdt_ops_t *ops = NULL;

    if (dev && dev->ops && counter_value) {
        ops = (wm_drv_wdt_ops_t *)dev->ops;
        if (ops->get_counter_value) {
            err = ops->get_counter_value(dev, counter_value);
        }

        return err;
    }

    return err;
}

int wm_drv_wdt_get_remaining_time(wm_device_t *dev, uint32_t *remaining_time)
{
    int err               = WM_ERR_INVALID_PARAM;
    wm_drv_wdt_ops_t *ops = NULL;

    if (dev && dev->ops && remaining_time) {
        ops = (wm_drv_wdt_ops_t *)dev->ops;
        if (ops->get_remaining_time) {
            err = ops->get_remaining_time(dev, remaining_time);
        }

        return err;
    }

    return err;
}

int wm_drv_wdt_feed(wm_device_t *dev)
{
    int err               = WM_ERR_INVALID_PARAM;
    wm_drv_wdt_ops_t *ops = NULL;

    if (dev && dev->ops) {
        ops = (wm_drv_wdt_ops_t *)dev->ops;
        if (ops->feed) {
            err = ops->feed(dev);
        }

        return err;
    }

    return err;
}

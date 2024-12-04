/**
  * @file wm_drv_gpio.c
  *
  * @brief gpio drv api
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
#include "wm_drv_gpio.h"
#include "wm_dt_hw.h"
#include "wm_dt.h"
#include "wm_drv_ops_gpio.c"

#define LOG_TAG "gpio"
#include "wm_log.h"

static wm_device_t *gpio_dev = NULL;

wm_device_t *wm_drv_gpio_init(char *dev_name)
{
    wm_drv_gpio_ops_t *ops = NULL;
    int err                = WM_ERR_INVALID_PARAM;

    if (!strcmp(dev_name, "gpio")) {
        gpio_dev = wm_dt_get_device_by_name(dev_name);
        if (gpio_dev && gpio_dev->ops) {
            ops = gpio_dev->ops;
            if (ops->init) {
                err = ops->init(gpio_dev);
            }

            return err != WM_ERR_SUCCESS ? NULL : gpio_dev;
        }
    }

    return NULL;
}

int wm_drv_gpio_deinit(wm_device_t *dev)
{
    int err                = WM_ERR_INVALID_PARAM;
    wm_drv_gpio_ops_t *ops = NULL;

    if (dev && dev->ops) {
        ops = (wm_drv_gpio_ops_t *)dev->ops;
        if (ops->deinit) {
            err = ops->deinit(dev);
        }

        return err;
    }

    return err;
}

int wm_drv_gpio_set_dir(wm_gpio_num_t pin, wm_gpio_dir_t dir)
{
    int err                = WM_ERR_INVALID_PARAM;
    wm_drv_gpio_ops_t *ops = NULL;
    wm_device_t *gpio_dev  = NULL;

    gpio_dev = wm_dt_get_device_by_name("gpio");
    if (gpio_dev && gpio_dev->ops) {
        ops = (wm_drv_gpio_ops_t *)gpio_dev->ops;
        if (ops->set_dir) {
            err = ops->set_dir(pin, dir);
        }

        return err;
    }

    return err;
}

int wm_drv_gpio_set_pullmode(wm_gpio_num_t pin, wm_gpio_pupd_t pullmode)
{
    int err                = WM_ERR_INVALID_PARAM;
    wm_drv_gpio_ops_t *ops = NULL;
    wm_device_t *gpio_dev  = NULL;

    gpio_dev = wm_dt_get_device_by_name("gpio");
    if (gpio_dev && gpio_dev->ops) {
        ops = (wm_drv_gpio_ops_t *)gpio_dev->ops;
        if (ops->set_pullmode) {
            err = ops->set_pullmode(pin, pullmode);
        }

        return err;
    }

    return err;
}

int wm_drv_gpio_data_set(wm_gpio_num_t pin)
{
    int err = WM_ERR_NO_INITED;

    if (gpio_dev && gpio_dev->ops) {
        if (((wm_drv_gpio_ops_t *)(gpio_dev->ops))->set_data) {
            err = ((wm_drv_gpio_ops_t *)(gpio_dev->ops))->set_data(pin);
        }

        return err;
    }

    return err;
}

int wm_drv_gpio_data_reset(wm_gpio_num_t pin)
{
    int err = WM_ERR_NO_INITED;

    if (gpio_dev && gpio_dev->ops) {
        if (((wm_drv_gpio_ops_t *)(gpio_dev->ops))->reset_data) {
            err = ((wm_drv_gpio_ops_t *)(gpio_dev->ops))->reset_data(pin);
        }

        return err;
    }

    return err;
}

int wm_drv_gpio_data_get(wm_gpio_num_t pin)
{
    int err = WM_ERR_NO_INITED;

    if (gpio_dev && gpio_dev->ops) {
        if (((wm_drv_gpio_ops_t *)(gpio_dev->ops))->get_data) {
            err = ((wm_drv_gpio_ops_t *)(gpio_dev->ops))->get_data(pin);
        }

        return err;
    }

    return err;
}

int wm_drv_gpio_iomux_func_sel(wm_gpio_num_t pin, wm_gpio_pin_mux_t func)
{
    int err                = WM_ERR_INVALID_PARAM;
    wm_drv_gpio_ops_t *ops = NULL;
    wm_device_t *gpio_dev  = NULL;

    gpio_dev = wm_dt_get_device_by_name("gpio");
    if (gpio_dev && gpio_dev->ops) {
        ops = (wm_drv_gpio_ops_t *)gpio_dev->ops;
        if (ops->iomux_func_sel) {
            err = ops->iomux_func_sel(pin, func);
        }

        return err;
    }

    return err;
}

int wm_drv_gpio_enable_isr(wm_gpio_num_t pin)
{
    int err                = WM_ERR_INVALID_PARAM;
    wm_drv_gpio_ops_t *ops = NULL;
    wm_device_t *gpio_dev  = NULL;

    gpio_dev = wm_dt_get_device_by_name("gpio");
    if (gpio_dev && gpio_dev->ops) {
        ops = (wm_drv_gpio_ops_t *)gpio_dev->ops;
        if (ops->enable_isr) {
            err = ops->enable_isr(pin);
        }

        return err;
    }

    return err;
}

int wm_drv_gpio_disable_isr(wm_gpio_num_t pin)
{
    int err                = WM_ERR_INVALID_PARAM;
    wm_drv_gpio_ops_t *ops = NULL;
    wm_device_t *gpio_dev  = NULL;

    gpio_dev = wm_dt_get_device_by_name("gpio");
    if (gpio_dev && gpio_dev->ops) {
        ops = (wm_drv_gpio_ops_t *)gpio_dev->ops;
        if (ops->disable_isr) {
            err = ops->disable_isr(pin);
        }

        return err;
    }

    return err;
}

int wm_drv_gpio_add_isr_callback(wm_gpio_num_t pin, wm_drv_gpio_isr_t isr_handler, void *args)
{
    int err                = WM_ERR_INVALID_PARAM;
    wm_drv_gpio_ops_t *ops = NULL;
    wm_device_t *gpio_dev  = NULL;

    gpio_dev = wm_dt_get_device_by_name("gpio");
    if (gpio_dev && gpio_dev->ops) {
        ops = (wm_drv_gpio_ops_t *)gpio_dev->ops;
        if (ops->add_isr_handler) {
            err = ops->add_isr_handler(gpio_dev, pin, isr_handler, args);
        }

        return err;
    }

    return err;
}

int wm_drv_gpio_remove_isr_callback(wm_gpio_num_t pin)
{
    int err                = WM_ERR_INVALID_PARAM;
    wm_drv_gpio_ops_t *ops = NULL;
    wm_device_t *gpio_dev  = NULL;

    gpio_dev = wm_dt_get_device_by_name("gpio");
    if (gpio_dev && gpio_dev->ops) {
        ops = (wm_drv_gpio_ops_t *)gpio_dev->ops;
        if (ops->remove_isr_handler) {
            err = ops->remove_isr_handler(gpio_dev, pin);
        }

        return err;
    }

    return err;
}

int wm_drv_gpio_set_intr_mode(wm_gpio_num_t pin, wm_gpio_intr_mode_t mode)
{
    int err                = WM_ERR_INVALID_PARAM;
    wm_drv_gpio_ops_t *ops = NULL;
    wm_device_t *gpio_dev  = NULL;

    gpio_dev = wm_dt_get_device_by_name("gpio");
    if (gpio_dev && gpio_dev->ops) {
        ops = (wm_drv_gpio_ops_t *)gpio_dev->ops;
        if (ops->set_intr_mode) {
            err = ops->set_intr_mode(pin, mode);
        }

        return err;
    }

    return err;
}

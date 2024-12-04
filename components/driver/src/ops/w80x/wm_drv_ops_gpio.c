/**
 * @file wm_drv_ops_gpio.c
 *
 * @brief Gpio ops Driver Module
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

#define LOG_TAG "gpio"
#include "wm_log.h"

#define WM_DRV_GPIO_LOCK(lock)                                                                     \
    do {                                                                                           \
        if (unlikely(!lock)) {                                                                     \
            if (WM_ERR_SUCCESS != wm_os_internal_mutex_create(&lock)) {                            \
                wm_log_error("mutex create err!");                                                 \
                return WM_ERR_FAILED;                                                              \
            }                                                                                      \
        }                                                                                          \
        if (unlikely(WM_ERR_SUCCESS != wm_os_internal_mutex_acquire(lock, WM_OS_WAIT_TIME_MAX))) { \
            wm_log_error("mutex acquire err!");                                                    \
            return WM_ERR_FAILED;                                                                  \
        }                                                                                          \
    } while (0);

#define WM_DRV_GPIO_UNLOCK(lock)            \
    do {                                    \
        wm_os_internal_mutex_release(lock); \
    } while (0);

typedef struct {
    wm_hal_gpio_dev_t gpio_hal;
} wm_drv_gpio_data_t;

static wm_os_mutex_t *gpio_mutex;

typedef struct {
    int (*init)(wm_device_t *dev);
    int (*deinit)(wm_device_t *dev);
    int (*set_dir)(wm_gpio_num_t pin, wm_gpio_dir_t dir);
    int (*set_pullmode)(wm_gpio_num_t pin, wm_gpio_pupd_t pullmode);
    int (*set_data)(wm_gpio_num_t pin);
    int (*reset_data)(wm_gpio_num_t pin);
    int (*get_data)(wm_gpio_num_t pin);
    int (*iomux_func_sel)(wm_gpio_num_t pin, wm_gpio_pin_mux_t func);
    int (*enable_isr)(wm_gpio_num_t pin);
    int (*disable_isr)(wm_gpio_num_t pin);
    int (*add_isr_handler)(wm_device_t *dev, wm_gpio_num_t pin, wm_drv_gpio_isr_t isr_handler, void *args);
    int (*remove_isr_handler)(wm_device_t *dev, wm_gpio_num_t pin);
    int (*set_intr_mode)(uint32_t pin, wm_gpio_intr_mode_t mode);
} wm_drv_gpio_ops_t;

static int wm_w800_drv_gpio_init(wm_device_t *dev)
{
    wm_drv_gpio_data_t *drv_data = NULL;
    int err                      = WM_ERR_NO_INITED;
    wm_dt_hw_gpio_t *hw          = NULL;
    wm_hal_gpio_config_t cfg     = { 0 };

    if (dev->hw && !dev->drv) {
        WM_DRV_GPIO_LOCK(gpio_mutex);

        //alloc drv data
        drv_data = (wm_drv_gpio_data_t *)wm_os_internal_malloc(sizeof(wm_drv_gpio_data_t));
        if (!drv_data) {
            WM_DRV_GPIO_UNLOCK(gpio_mutex);
            return WM_ERR_NO_MEM;
        }
        memset(drv_data, 0x00, sizeof(wm_drv_gpio_data_t));

        if (WM_ERR_SUCCESS != (err = wm_hal_gpio_install_isr_service(&drv_data->gpio_hal))) {
            wm_log_error("gpio install isr err %d", err);
            WM_DRV_GPIO_UNLOCK(gpio_mutex);
            wm_os_internal_free(drv_data);
            return err;
        }

        hw = (wm_dt_hw_gpio_t *)dev->hw;

        for (int i = 0; i < hw->gpio_cfg_count; i++) {
            cfg.dir       = hw->gpio_cfg[i].pin_dir;
            cfg.pull_mode = hw->gpio_cfg[i].pin_pupd;
            cfg.intr_mode = hw->gpio_cfg[i].int_mode;
            if (WM_ERR_SUCCESS != (err = wm_hal_gpio_init(hw->gpio_cfg[i].pin_num, &cfg))) {
                wm_log_error("gpio init err %d", err);
                wm_os_internal_free(drv_data);
                WM_DRV_GPIO_UNLOCK(gpio_mutex);
                return err;
            }
        }

        dev->state = WM_DEV_ST_INITED;
        dev->drv   = drv_data;

        WM_DRV_GPIO_UNLOCK(gpio_mutex);
        return WM_ERR_SUCCESS;
    } else {
        return WM_ERR_ALREADY_INITED;
    }

    return WM_ERR_NOT_ALLOWED;
}

static int wm_w800_drv_gpio_deinit(wm_device_t *dev)
{
    int err                      = WM_ERR_NO_INITED;
    wm_dt_hw_gpio_t *hw          = NULL;
    wm_drv_gpio_data_t *drv_data = NULL;

    if (dev->drv && dev->hw) {
        WM_DRV_GPIO_LOCK(gpio_mutex);
        hw       = (wm_dt_hw_gpio_t *)dev->hw;
        drv_data = (wm_drv_gpio_data_t *)dev->drv;

        for (int i = 0; i < hw->gpio_cfg_count; i++) {
            if (WM_ERR_SUCCESS != (err = wm_hal_gpio_deinit(hw->gpio_cfg[i].pin_num))) {
                wm_log_error("gpio deinit err %d", err);
                WM_DRV_GPIO_UNLOCK(gpio_mutex);
                return err;
            }
        }

        if (WM_ERR_SUCCESS != (err = wm_hal_gpio_uninstall_isr_service(&drv_data->gpio_hal))) {
            wm_log_error("gpio uninstall isr err %d", err);
            WM_DRV_GPIO_UNLOCK(gpio_mutex);
            return err;
        }

        wm_os_internal_free(dev->drv);
        dev->drv   = NULL;
        dev->state = WM_DEV_ST_UNINIT;
        WM_DRV_GPIO_UNLOCK(gpio_mutex);

        return WM_ERR_SUCCESS;
    } else {
        return WM_ERR_NO_INITED;
    }

    return WM_ERR_NO_INITED;
}

static int wm_w800_drv_gpio_set_dir(wm_gpio_num_t pin, wm_gpio_dir_t dir)
{
    int err = WM_ERR_SUCCESS;

    WM_DRV_GPIO_LOCK(gpio_mutex);
    err = wm_hal_gpio_set_dir(pin, dir);
    WM_DRV_GPIO_UNLOCK(gpio_mutex);

    return err;
}

static int wm_w800_drv_gpio_set_pullmode(wm_gpio_num_t pin, wm_gpio_pupd_t pullmode)
{
    int err = WM_ERR_SUCCESS;

    WM_DRV_GPIO_LOCK(gpio_mutex);
    err = wm_hal_gpio_set_pullmode(pin, pullmode);
    WM_DRV_GPIO_UNLOCK(gpio_mutex);

    return err;
}

static int wm_w800_drv_gpio_data_set(wm_gpio_num_t pin)
{
    return wm_hal_gpio_data_set(pin);
}

static int wm_w800_drv_gpio_data_reset(wm_gpio_num_t pin)
{
    return wm_hal_gpio_data_reset(pin);
}

static int wm_w800_drv_gpio_data_get(wm_gpio_num_t pin)
{
    return wm_hal_gpio_data_get(pin);
}

static int wm_w800_drv_gpio_iomux_func_sel(wm_gpio_num_t pin, wm_gpio_pin_mux_t func)
{
    int err = WM_ERR_SUCCESS;

    WM_DRV_GPIO_LOCK(gpio_mutex);
    err = wm_hal_gpio_iomux_func_sel(pin, func);
    if (WM_ERR_SUCCESS == err) {
        for (int i = 0; i < WM_IOMUX_ATTR_NUM; i++) {
            if (pin == g_iomux_attr_table[i].pin_num && func == g_iomux_attr_table[i].pin_mux) {
                if (WM_ERR_SUCCESS == (err = wm_hal_gpio_set_dir(pin, g_iomux_attr_table[i].dir)) &&
                    WM_ERR_SUCCESS == (err = wm_hal_gpio_set_pullmode(pin, g_iomux_attr_table[i].pullmode))) {
                    ;
                }
                break;
            }
        }
    }
    WM_DRV_GPIO_UNLOCK(gpio_mutex);

    return err;
}

static int wm_w800_drv_gpio_enable_irq(wm_gpio_num_t pin)
{
    int err = WM_ERR_SUCCESS;

    WM_DRV_GPIO_LOCK(gpio_mutex);
    err = wm_hal_gpio_enable_irq(pin);
    WM_DRV_GPIO_UNLOCK(gpio_mutex);

    return err;
}

static int wm_w800_drv_gpio_disable_irq(wm_gpio_num_t pin)
{
    int err = WM_ERR_SUCCESS;

    WM_DRV_GPIO_LOCK(gpio_mutex);
    err = wm_hal_gpio_disable_irq(pin);
    WM_DRV_GPIO_UNLOCK(gpio_mutex);

    return err;
}

static int wm_w800_drv_gpio_add_isr_handler(wm_device_t *dev, wm_gpio_num_t gpio_num, wm_drv_gpio_isr_t isr_handler, void *args)
{
    int err                      = WM_ERR_NO_INITED;
    wm_drv_gpio_data_t *drv_data = NULL;

    if (dev && dev->drv) {
        WM_DRV_GPIO_LOCK(gpio_mutex);
        drv_data = (wm_drv_gpio_data_t *)dev->drv;
        err      = wm_hal_gpio_add_isr_handler(&drv_data->gpio_hal, gpio_num, isr_handler, args);
        WM_DRV_GPIO_UNLOCK(gpio_mutex);
    }

    return err;
}

static int wm_w800_drv_gpio_remove_isr_handler(wm_device_t *dev, wm_gpio_num_t gpio_num)
{
    int err                      = WM_ERR_NO_INITED;
    wm_drv_gpio_data_t *drv_data = NULL;

    if (dev && dev->drv) {
        WM_DRV_GPIO_LOCK(gpio_mutex);
        drv_data = (wm_drv_gpio_data_t *)dev->drv;
        err      = wm_hal_gpio_remove_isr_handler(&drv_data->gpio_hal, gpio_num);
        WM_DRV_GPIO_UNLOCK(gpio_mutex);
    }
    return err;
}

static int wm_w800_drv_gpio_set_intr_mode(uint32_t pin, wm_gpio_intr_mode_t mode)
{
    int err = WM_ERR_SUCCESS;

    WM_DRV_GPIO_LOCK(gpio_mutex);
    err = wm_hal_gpio_set_intr_mode(pin, mode);
    WM_DRV_GPIO_UNLOCK(gpio_mutex);

    return err;
}

const wm_drv_gpio_ops_t wm_drv_gpio_ops = {
    .init               = wm_w800_drv_gpio_init,
    .deinit             = wm_w800_drv_gpio_deinit,
    .set_dir            = wm_w800_drv_gpio_set_dir,
    .set_pullmode       = wm_w800_drv_gpio_set_pullmode,
    .set_data           = wm_w800_drv_gpio_data_set,
    .reset_data         = wm_w800_drv_gpio_data_reset,
    .get_data           = wm_w800_drv_gpio_data_get,
    .iomux_func_sel     = wm_w800_drv_gpio_iomux_func_sel,
    .enable_isr         = wm_w800_drv_gpio_enable_irq,
    .disable_isr        = wm_w800_drv_gpio_disable_irq,
    .add_isr_handler    = wm_w800_drv_gpio_add_isr_handler,
    .remove_isr_handler = wm_w800_drv_gpio_remove_isr_handler,
    .set_intr_mode      = wm_w800_drv_gpio_set_intr_mode,
};
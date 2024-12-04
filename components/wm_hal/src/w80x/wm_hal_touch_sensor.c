/**
 * @file wm_hal_touch_sensor.c
 *
 * @brief touch sensor Module
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
#include "wm_soc_cfgs.h"
#include "wm_touch_sensor_reg_struct.h"
#include "wm_ll_touch_sensor.h"
#include "wm_hal_touch_sensor.h"
#include "wm_hal_irq.h"
//#include "wm_hal_rcc.h"

#define LOG_TAG "TOUCH_HAL"
#include "wm_log.h"

#define DEFAULT_SCAN_PERIOD 1
#define DEFAULT_SCAN_WINDOW 1

#define WM_HAL_TOUCH_DEBUG  0

#if WM_HAL_TOUCH_DEBUG
#define WM_HAL_TOUCH_PRINTF wm_log_debug
#else
#define WM_HAL_TOUCH_PRINTF(...)
#endif
#define WM_HAL_TOUCH_ERR wm_log_error

static void touch_sensor_handle(wm_irq_no_t irq, void *arg)
{
    wm_hal_touch_sensor_dev_t *dev = arg;
    uint16_t int_source;
    uint8_t touch_num;

    int_source = wm_ll_touch_sensor_get_int_source((wm_touch_sensor_reg_t *)dev->register_base);
    wm_ll_touch_sensor_set_int_source((wm_touch_sensor_reg_t *)dev->register_base, int_source);

    for (touch_num = 0; touch_num < WM_TOUCH_NUM_MAX; touch_num++) {
        if (int_source & WM_BIT(touch_num)) {
            if (dev->callback[touch_num])
                ((wm_hal_touch_sensor_callback_t)dev->callback[touch_num])(dev, touch_num, dev->callback_priv[touch_num]);
        }
    }
}

int wm_hal_touch_sensor_init(wm_hal_touch_sensor_dev_t *dev)
{
    int err;

    if (!dev) {
        return WM_ERR_INVALID_PARAM;
    }

    if (!dev->register_base) {
        return WM_ERR_INVALID_PARAM;
    }

    if (wm_hal_irq_attach_sw_vector(dev->irq_num, touch_sensor_handle, dev) != WM_ERR_SUCCESS) {
        return WM_ERR_FAILED;
    }

    //wm_hal_clock_enable(WM_RCC_TOUCH_GATE_EN);

    wm_ll_touch_sensor_set_capdet_cnt((wm_touch_sensor_reg_t *)dev->register_base, DEFAULT_SCAN_WINDOW);
    wm_ll_touch_sensor_set_scan_period((wm_touch_sensor_reg_t *)dev->register_base, DEFAULT_SCAN_PERIOD);

    wm_ll_touch_sensor_set_key_selection((wm_touch_sensor_reg_t *)dev->register_base, 0);
    wm_ll_touch_sensor_set_int_enable((wm_touch_sensor_reg_t *)dev->register_base, 0);

    err = wm_hal_irq_enable(dev->irq_num);
    if (err != WM_ERR_SUCCESS) {
        WM_HAL_TOUCH_ERR("wm_hal_irq_enable touch is failed");
        return WM_ERR_FAILED;
    }

    return WM_ERR_SUCCESS;
}

int wm_hal_touch_sensor_deinit(wm_hal_touch_sensor_dev_t *dev)
{
    if (!dev) {
        return WM_ERR_INVALID_PARAM;
    }

    if (wm_hal_irq_disable(dev->irq_num) != WM_ERR_SUCCESS) {
        return WM_ERR_FAILED;
    }

    if (wm_hal_irq_detach_sw_vector(dev->irq_num) != WM_ERR_SUCCESS) {
        return WM_ERR_FAILED;
    }

    wm_ll_touch_sensor_set_key_selection((wm_touch_sensor_reg_t *)dev->register_base, 0);
    wm_ll_touch_sensor_set_int_source((wm_touch_sensor_reg_t *)dev->register_base, 0xffff);
    wm_ll_touch_sensor_set_int_enable((wm_touch_sensor_reg_t *)dev->register_base, 0);
    wm_ll_touch_sensor_scan_enable((wm_touch_sensor_reg_t *)dev->register_base, 0);

    //wm_hal_clock_disable(WM_RCC_TOUCH_GATE_EN);

    return WM_ERR_SUCCESS;
}

int wm_hal_touch_sensor_set_threshold(wm_hal_touch_sensor_dev_t *dev, wm_touch_sensor_num_t touch_num, uint32_t threshold)
{
    if ((dev == NULL) || !touch_num || (touch_num > WM_TOUCH_NUM_MAX) || (threshold > WM_TOUCH_THRESHOLD_MAX)) {
        return WM_ERR_INVALID_PARAM;
    }

    wm_ll_touch_sensor_set_threshold_val((wm_touch_sensor_reg_t *)dev->register_base, touch_num, threshold);

    return WM_ERR_SUCCESS;
}

int wm_hal_touch_sensor_get_threshold(wm_hal_touch_sensor_dev_t *dev, wm_touch_sensor_num_t touch_num, uint32_t *threshold)
{
    if ((dev == NULL) || !touch_num || (touch_num > WM_TOUCH_NUM_MAX) || (threshold == NULL)) {
        return WM_ERR_INVALID_PARAM;
    }

    *threshold = wm_ll_touch_sensor_get_threshold_val((wm_touch_sensor_reg_t *)dev->register_base, touch_num);

    return WM_ERR_SUCCESS;
}

int wm_hal_touch_sensor_get_int_enable(wm_hal_touch_sensor_dev_t *dev, uint16_t *enable)
{
    if ((dev == NULL) || (enable == NULL)) {
        return WM_ERR_INVALID_PARAM;
    }

    *enable = wm_ll_touch_sensor_get_int_enable((wm_touch_sensor_reg_t *)dev->register_base);

    return WM_ERR_SUCCESS;
}

int wm_hal_touch_sensor_set_int_enable(wm_hal_touch_sensor_dev_t *dev, uint16_t enable)
{
    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    wm_ll_touch_sensor_set_int_enable((wm_touch_sensor_reg_t *)dev->register_base, enable);

    return WM_ERR_SUCCESS;
}

int wm_hal_touch_sensor_register_callback(wm_hal_touch_sensor_dev_t *dev, wm_touch_sensor_num_t touch_num,
                                          wm_hal_touch_sensor_callback_t callback, void *priv)
{
    if ((dev == NULL) || !touch_num || (touch_num > WM_TOUCH_NUM_MAX)) {
        return WM_ERR_INVALID_PARAM;
    }

    dev->callback[touch_num]      = callback;
    dev->callback_priv[touch_num] = priv;

    return WM_ERR_SUCCESS;
}

int wm_hal_touch_sensor_read_countnum(wm_hal_touch_sensor_dev_t *dev, wm_touch_sensor_num_t touch_num, uint32_t *countnum)
{
    if ((dev == NULL) || !touch_num || (touch_num > WM_TOUCH_NUM_MAX) || (countnum == NULL)) {
        return WM_ERR_INVALID_PARAM;
    }

    *countnum = wm_ll_touch_sensor_get_count_val((wm_touch_sensor_reg_t *)dev->register_base, touch_num);

    return WM_ERR_SUCCESS;
}

void wm_hal_touch_sensor_start(wm_hal_touch_sensor_dev_t *dev, wm_touch_sensor_num_t touch_num)
{
    uint16_t selection;

    if ((dev == NULL) || !touch_num || (touch_num > WM_TOUCH_NUM_MAX)) {
        return;
    }

    selection = wm_ll_touch_sensor_get_key_selection((wm_touch_sensor_reg_t *)dev->register_base);
    if (!(selection & WM_BIT(touch_num))) {
        selection |= WM_BIT(touch_num);
        wm_ll_touch_sensor_set_key_selection((wm_touch_sensor_reg_t *)dev->register_base, selection);
    }

    if (!wm_ll_touch_sensor_get_scan_enable((wm_touch_sensor_reg_t *)dev->register_base))
        wm_ll_touch_sensor_scan_enable((wm_touch_sensor_reg_t *)dev->register_base, 1);
}

void wm_hal_touch_sensor_stop(wm_hal_touch_sensor_dev_t *dev, wm_touch_sensor_num_t touch_num)
{
    uint16_t selection;

    if ((dev == NULL) || !touch_num || (touch_num > WM_TOUCH_NUM_MAX)) {
        return;
    }

    selection = wm_ll_touch_sensor_get_key_selection((wm_touch_sensor_reg_t *)dev->register_base);
    if (selection & WM_BIT(touch_num)) {
        selection &= ~WM_BIT(touch_num);
        wm_ll_touch_sensor_set_key_selection((wm_touch_sensor_reg_t *)dev->register_base, selection);
    }

    if (!selection && wm_ll_touch_sensor_get_scan_enable((wm_touch_sensor_reg_t *)dev->register_base))
        wm_ll_touch_sensor_scan_enable((wm_touch_sensor_reg_t *)dev->register_base, 0);
}

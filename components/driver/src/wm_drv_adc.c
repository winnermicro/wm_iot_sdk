/**
 * @file wm_drv_adc.c
 *
 * @brief Adc Module
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
#include "wm_osal.h"

#include "wm_drv_rcc.h"
#include "wm_drv_gpio.h"
#include "wm_dt_hw.h"
#include "wm_drv_adc.h"
#include "wm_drv_ops_adc.c"

#define LOG_TAG "adc"
#include "wm_log.h"

wm_device_t *wm_drv_adc_init(char *dev_name)
{
    wm_device_t *adc_dev  = NULL;
    wm_drv_adc_ops_t *ops = NULL;
    int err               = WM_ERR_INVALID_PARAM;

    if (!strcmp(dev_name, "adc")) {
        adc_dev = wm_dt_get_device_by_name(dev_name);
        if (adc_dev && adc_dev->ops) {
            ops = adc_dev->ops;
            if (ops->init) {
                err = ops->init(adc_dev);
            }

            return err != WM_ERR_SUCCESS ? NULL : adc_dev;
        }
    }

    return NULL;
}

int wm_drv_adc_deinit(wm_device_t *dev)
{
    if (!dev) {
        return WM_ERR_INVALID_PARAM;
    }

    wm_drv_adc_ops_t *ops = NULL;
    int err               = WM_ERR_INVALID_PARAM;

    ops = dev->ops;
    if (ops && ops->deinit) {
        err = ops->deinit(dev);
    }

    return err;
}

int wm_drv_adc_cfg(wm_device_t *dev, const wm_drv_adc_cfg_t *cfg)
{
    if (!dev || !cfg) {
        return WM_ERR_INVALID_PARAM;
    }

    wm_drv_adc_ops_t *ops = NULL;
    int err               = WM_ERR_INVALID_PARAM;

    ops = dev->ops;
    if (ops && ops->set_cfg) {
        err = ops->set_cfg(dev, cfg);
    }

    return err;
}

int wm_drv_adc_register_callback(wm_device_t *dev, wm_adc_intr_type_t intr_type, wm_drv_adc_callback_t adc_callback,
                                 void *user_data)
{
    if (!dev || !adc_callback) {
        return WM_ERR_INVALID_PARAM;
    }

    wm_drv_adc_ops_t *ops = NULL;
    int err               = WM_ERR_INVALID_PARAM;

    ops = dev->ops;
    if (ops && ops->register_handle) {
        err = ops->register_handle(dev, intr_type, adc_callback, user_data);
    }

    return err;
}

int wm_drv_adc_oneshot(wm_device_t *dev, wm_adc_channel_t adc_channel, int *result)
{
    if (!dev || !WM_ADC_CHANNEL_IS_VAILD(adc_channel) || !result) {
        return WM_ERR_INVALID_PARAM;
    }

    wm_drv_adc_ops_t *ops = NULL;
    int err               = WM_ERR_INVALID_PARAM;

    ops = dev->ops;
    if (ops && ops->oneshot) {
        err = ops->oneshot(dev, adc_channel, result);
    }

    return err;
}

int wm_drv_adc_polling(wm_device_t *dev, wm_adc_channel_t adc_channel, int *buf, uint16_t count, uint32_t timeout)
{
    if (!dev || !WM_ADC_CHANNEL_IS_VAILD(adc_channel) || !buf) {
        return WM_ERR_INVALID_PARAM;
    }

    wm_drv_adc_ops_t *ops = NULL;
    int err               = WM_ERR_INVALID_PARAM;

    ops = dev->ops;
    if (ops && ops->polling) {
        err = ops->polling(dev, adc_channel, buf, count, timeout);
    }

    return err;
}

int wm_drv_adc_start_it(wm_device_t *dev, wm_adc_channel_t adc_channel)
{
    if (!dev || !WM_ADC_CHANNEL_IS_VAILD(adc_channel)) {
        return WM_ERR_INVALID_PARAM;
    }

    wm_drv_adc_ops_t *ops = NULL;
    int err               = WM_ERR_INVALID_PARAM;

    ops = dev->ops;
    if (ops && ops->start_intr) {
        err = ops->start_intr(dev, adc_channel);
    }

    return err;
}

int wm_drv_adc_stop_it(wm_device_t *dev, wm_adc_channel_t adc_channel)
{
    if (!dev || !WM_ADC_CHANNEL_IS_VAILD(adc_channel)) {
        return WM_ERR_INVALID_PARAM;
    }

    wm_drv_adc_ops_t *ops = NULL;
    int err               = WM_ERR_INVALID_PARAM;

    ops = dev->ops;
    if (ops && ops->stop_intr) {
        err = ops->stop_intr(dev, adc_channel);
    }

    return err;
}

int wm_drv_thermal_sensor_read_temperature(wm_device_t *dev, int *temperature_val)
{
    if (!dev || !temperature_val) {
        return WM_ERR_INVALID_PARAM;
    }

    wm_drv_adc_ops_t *ops = NULL;
    int err               = WM_ERR_INVALID_PARAM;

    ops = dev->ops;
    if (ops && ops->read_temp) {
        err = ops->read_temp(dev, temperature_val);
    }

    return err;
}

int wm_drv_adc_chip_voltage_read_vdd(wm_device_t *dev, int *voltage)
{
    if (!dev || !voltage) {
        return WM_ERR_INVALID_PARAM;
    }

    wm_drv_adc_ops_t *ops = NULL;
    int err               = WM_ERR_INVALID_PARAM;

    ops = dev->ops;
    if (ops && ops->read_voltage) {
        err = ops->read_voltage(dev, voltage);
    }

    return err;
}

int wm_drv_adc_cal_voltage(wm_device_t *dev, int vol)
{
    if (!dev) {
        return WM_ERR_INVALID_PARAM;
    }

    wm_drv_adc_ops_t *ops = NULL;
    int err               = WM_ERR_INVALID_PARAM;

    ops = dev->ops;
    if (ops && ops->cal_voltage) {
        err = ops->cal_voltage(dev, vol);
    }

    return err;
}

/**
 * @file wm_drv_touch_sensor.c
 *
 * @brief touch sensor Driver Module
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
#include "wm_drv_touch_sensor.h"
#include "wm_drv_ops_touch_sensor.c"

int wm_drv_touch_sensor_register_callback(wm_device_t *dev, wm_touch_sensor_num_t touch_num,
                                          wm_drv_touch_sensor_callback_t callback, void *priv)
{
    wm_drv_touch_sensor_ops_t *ops = NULL;
    int ret                        = WM_ERR_NO_INITED;

    if (dev == NULL || !touch_num || touch_num > WM_TOUCH_NUM_MAX) {
        return WM_ERR_INVALID_PARAM;
    }

    ops = dev->ops;
    if (ops && ops->register_callback) {
        ret = ops->register_callback(dev, touch_num, callback, priv);
    }

    return ret;
}

int wm_drv_touch_sensor_set_threshold(wm_device_t *dev, wm_touch_sensor_num_t touch_num, uint32_t threshold)
{
    wm_drv_touch_sensor_ops_t *ops = NULL;
    int ret                        = WM_ERR_NO_INITED;

    if (dev == NULL || !touch_num || touch_num > WM_TOUCH_NUM_MAX || threshold > WM_TOUCH_THRESHOLD_MAX) {
        return WM_ERR_INVALID_PARAM;
    }

    ops = dev->ops;
    if (ops && ops->set_threshold) {
        ret = ops->set_threshold(dev, touch_num, threshold);
    }

    return ret;
}

int wm_drv_touch_sensor_get_threshold(wm_device_t *dev, wm_touch_sensor_num_t touch_num, uint32_t *threshold)
{
    wm_drv_touch_sensor_ops_t *ops = NULL;
    int ret                        = WM_ERR_NO_INITED;

    if (dev == NULL || !touch_num || touch_num > WM_TOUCH_NUM_MAX || threshold == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    ops = dev->ops;
    if (ops && ops->get_threshold) {
        ret = ops->get_threshold(dev, touch_num, threshold);
    }

    return ret;
}

int wm_drv_touch_sensor_read_countnum(wm_device_t *dev, wm_touch_sensor_num_t touch_num, uint32_t *countnum)
{
    wm_drv_touch_sensor_ops_t *ops = NULL;
    int ret                        = WM_ERR_NO_INITED;

    if (dev == NULL || !touch_num || touch_num > WM_TOUCH_NUM_MAX || countnum == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    ops = dev->ops;
    if (ops && ops->read_countnum) {
        ret = ops->read_countnum(dev, touch_num, countnum);
    }

    return ret;
}

int wm_drv_touch_sensor_start(wm_device_t *dev, wm_touch_sensor_num_t touch_num)
{
    wm_drv_touch_sensor_ops_t *ops = NULL;
    int ret                        = WM_ERR_NO_INITED;

    if (dev == NULL || !touch_num || touch_num > WM_TOUCH_NUM_MAX) {
        return WM_ERR_INVALID_PARAM;
    }

    ops = dev->ops;
    if (ops && ops->start) {
        ret = ops->start(dev, touch_num);
    }

    return ret;
}

int wm_drv_touch_sensor_stop(wm_device_t *dev, wm_touch_sensor_num_t touch_num)
{
    wm_drv_touch_sensor_ops_t *ops = NULL;
    int ret                        = WM_ERR_NO_INITED;

    if (dev == NULL || !touch_num || touch_num > WM_TOUCH_NUM_MAX) {
        return WM_ERR_INVALID_PARAM;
    }

    ops = dev->ops;
    if (ops && ops->stop) {
        ret = ops->stop(dev, touch_num);
    }

    return ret;
}

int wm_drv_touch_sensor_deinit(wm_device_t *dev)
{
    wm_drv_touch_sensor_ops_t *ops = NULL;
    int ret                        = WM_ERR_NO_INITED;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    ops = dev->ops;
    if (ops && ops->deinit) {
        ret = ops->deinit(dev);
    }

    return ret;
}

wm_device_t *wm_drv_touch_sensor_init(const char *dev_name)
{
    wm_device_t *touch_dev         = NULL;
    wm_drv_touch_sensor_ops_t *ops = NULL;
    int ret                        = WM_ERR_FAILED;

    if (dev_name == NULL) {
        return NULL;
    }

    touch_dev = wm_dt_get_device_by_name(dev_name);
    if (touch_dev && touch_dev->ops) {
        ops = touch_dev->ops;
        if (ops->init) {
            ret = ops->init(touch_dev);
        }
    }

    return ret == WM_ERR_SUCCESS ? touch_dev : NULL;
}

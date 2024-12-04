/**
 * @file wm_touch_button.c
 *
 * @brief TOUCH BUTTON Module
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
#include "wm_error.h"
#include "wm_drv_touch_button.h"
#include "wm_drv_ops_touch_button.c"

int wm_drv_touch_button_register_callback(wm_device_t *dev, wm_drv_touch_button_callback_t callback, void *priv)
{
    wm_drv_touch_button_ops_t *ops = NULL;
    int ret                        = WM_ERR_NO_INITED;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    ops = dev->ops;
    if (ops && ops->register_callback) {
        ret = ops->register_callback(dev, callback, priv);
    }

    return ret;
}

int wm_drv_touch_button_set_longpress(wm_device_t *dev, uint32_t threshold_time)
{
    wm_drv_touch_button_ops_t *ops = NULL;
    int ret                        = WM_ERR_NO_INITED;

    if (dev == NULL || threshold_time < 1000) {
        return WM_ERR_INVALID_PARAM;
    }

    ops = dev->ops;
    if (ops && ops->set_longpress) {
        ret = ops->set_longpress(dev, threshold_time);
    }

    return ret;
}

int wm_drv_touch_button_start(wm_device_t *dev)
{
    wm_drv_touch_button_ops_t *ops = NULL;
    int ret                        = WM_ERR_NO_INITED;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    ops = dev->ops;
    if (ops && ops->start) {
        ret = ops->start(dev);
    }

    return ret;
}

int wm_drv_touch_button_stop(wm_device_t *dev)
{
    wm_drv_touch_button_ops_t *ops = NULL;
    int ret                        = WM_ERR_NO_INITED;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    ops = dev->ops;
    if (ops && ops->stop) {
        ret = ops->stop(dev);
    }

    return ret;
}

wm_device_t *wm_drv_touch_button_init(const char *dev_name)
{
    wm_device_t *touch_dev         = NULL;
    wm_drv_touch_button_ops_t *ops = NULL;
    int ret                        = WM_ERR_FAILED;

    touch_dev = wm_dt_get_device_by_name(dev_name);
    if (touch_dev && touch_dev->ops) {
        ops = touch_dev->ops;
        if (ops->init) {
            ret = ops->init(touch_dev);
        }
    }

    return ret == WM_ERR_SUCCESS ? touch_dev : NULL;
}

int wm_drv_touch_button_deinit(wm_device_t *dev)
{
    wm_drv_touch_button_ops_t *ops = NULL;
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
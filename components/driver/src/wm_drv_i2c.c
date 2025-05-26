/**
 * @file wm_drv_i2c.c
 *
 * @brief driver i2c master Module
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

#include "wm_drv_i2c.h"
#undef LOG_TAG
#define LOG_TAG "drv_i2c"
#include "wm_log.h"

#define WM_DRV_I2C_LOGE         wm_log_error
#define WM_DRV_I2C_TRACE_FAIL() WM_DRV_I2C_LOGE("err line %d", __LINE__)

/**
 * @brief Check param p and return r
 */
#define WM_DRV_I2C_CHECK_PARAM_RET(p, r) \
    if (!(p)) {                          \
        WM_DRV_I2C_TRACE_FAIL();         \
        return (r);                      \
    }

/**
 * @brief Check param and return WM_ERR_INVALID_PARAM
 */
#define WM_DRV_I2C_CHECK_PARAM(p)    \
    if (!(p)) {                      \
        WM_DRV_I2C_TRACE_FAIL();     \
        return WM_ERR_INVALID_PARAM; \
    }

wm_device_t *wm_drv_i2c_init(const char *device_name)
{
    wm_device_t *dev = (device_name ? wm_dt_get_device_by_name(device_name) : NULL);
    WM_DRV_I2C_CHECK_PARAM_RET(dev, NULL);
    return ((wm_drv_i2c_ops_t *)dev->ops)->init(dev) == WM_ERR_SUCCESS ? dev : NULL;
}

int wm_drv_i2c_deinit(wm_device_t *device)
{
    WM_DRV_I2C_CHECK_PARAM(device);
    return ((wm_drv_i2c_ops_t *)device->ops)->deinit(device);
}

int wm_drv_i2c_read(wm_device_t *device, wm_drv_i2c_config_t *config, const void *sub_addr, uint32_t sub_addr_size,
                    void *read_buf, uint32_t read_size)
{
    WM_DRV_I2C_CHECK_PARAM(device && config && read_buf && read_size > 0);
    return ((wm_drv_i2c_ops_t *)device->ops)->read(device, config, sub_addr, sub_addr_size, read_buf, read_size);
}

int wm_drv_i2c_write(wm_device_t *device, wm_drv_i2c_config_t *config, const void *sub_addr, uint32_t sub_addr_size,
                     const void *data, uint32_t data_size)
{
    WM_DRV_I2C_CHECK_PARAM(device && config && sub_addr && sub_addr_size > 0);
    return ((wm_drv_i2c_ops_t *)device->ops)->write(device, config, sub_addr, sub_addr_size, data, data_size);
}

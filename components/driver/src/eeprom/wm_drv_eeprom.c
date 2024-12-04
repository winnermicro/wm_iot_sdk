/**
 * @file wm_drv_eeprom.c
 *
 * @brief driver eeprom common Module
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wm_types.h"
#include "wm_error.h"
#include "wm_attr.h"
#include "wm_osal.h"
#include "wm_drv_gpio.h"
#include "wm_drv_i2c.h"
#include "wm_drv_eeprom.h"
#include "wm_dt_hw.h"

/**
 * @brief Check param and return WM_ERR_INVALID_PARAM
 */
#define WM_DRV_EEPROM_CHECK_PARAM(p) \
    if (!(p)) {                      \
        return WM_ERR_INVALID_PARAM; \
    }

wm_device_t *wm_drv_eeprom_init(const char *eeprom_dev_name)
{
    wm_device_t *dev = (eeprom_dev_name ? wm_dt_get_device_by_name(eeprom_dev_name) : NULL);
    return (dev && (((wm_drv_eeprom_ops_t *)dev->ops)->init(dev) == WM_ERR_SUCCESS)) ? dev : NULL;
}

int wm_drv_eeprom_deinit(wm_device_t *eeprom_device)
{
    WM_DRV_EEPROM_CHECK_PARAM(eeprom_device);
    return ((wm_drv_eeprom_ops_t *)eeprom_device->ops)->deinit(eeprom_device);
}

int wm_drv_eeprom_read(wm_device_t *eeprom_device, uint32_t offset, void *buffer, uint32_t read_size)
{
    WM_DRV_EEPROM_CHECK_PARAM(eeprom_device && buffer && read_size > 0);
    return ((wm_drv_eeprom_ops_t *)eeprom_device->ops)->read(eeprom_device, offset, buffer, read_size);
}

int wm_drv_eeprom_write(wm_device_t *eeprom_device, uint32_t offset, const void *data, uint32_t data_size)
{
    WM_DRV_EEPROM_CHECK_PARAM(eeprom_device && data && data_size > 0);
    return ((wm_drv_eeprom_ops_t *)eeprom_device->ops)->write(eeprom_device, offset, data, data_size);
}

int wm_drv_eeprom_get_size(wm_device_t *eeprom_device)
{
    WM_DRV_EEPROM_CHECK_PARAM(eeprom_device);
    return ((wm_drv_eeprom_ops_t *)eeprom_device->ops)->get_size(eeprom_device);
}

int wm_drv_eeprom_set_wp(wm_device_t *eeprom_device, bool on)
{
    WM_DRV_EEPROM_CHECK_PARAM(eeprom_device);
    return ((wm_drv_eeprom_ops_t *)eeprom_device->ops)->set_wp(eeprom_device, on);
}

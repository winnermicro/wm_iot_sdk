/**
 * @file wm_drv_eeprom_nv24c0x.c
 *
 * @brief driver eeprom nv240x Module
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
 * @brief NV24C0X slave address
 *
 * 1 0 1 0 a10 a9 a8 R/W NV24C16
 * 1 0 1 0 A2  a9 a8 R/W NV24C08
 * 1 0 1 0 A2  A1 a8 R/W NV24C04
 * 1 0 1 0 A2  A1 A0 R/W NV24C02
 */

/**
 * @brief Check param and return WM_ERR_INVALID_PARAM
 */
#define WM_DRV_EEPROM_CHECK_PARAM(p) \
    if (!(p)) {                      \
        return WM_ERR_INVALID_PARAM; \
    }

/**
 * @brief Check condition and return WM_ERR_FAILED
 */
#define WM_DRV_EEPROM_CHECK_FAIL(p) \
    if (!(p)) {                     \
        return WM_ERR_FAILED;       \
    }

/**
  * @brief  mutex lock for api
  */
#define WM_DRV_EEPROM_LOCK(mutex)                                                                                      \
    do {                                                                                                               \
        if (wm_os_internal_recursive_mutex_acquire((wm_os_mutex_t *)(mutex), WM_OS_WAIT_TIME_MAX) != WM_ERR_SUCCESS) { \
            return WM_ERR_FAILED;                                                                                      \
        }                                                                                                              \
    } while (0)

/**
  * @brief  mutex unlock for api
  */
#define WM_DRV_EEPROM_UNLOCK(mutex)      wm_os_internal_recursive_mutex_release((wm_os_mutex_t *)(mutex))

/**
  * @brief  Use for address round up
  */
#define WM_DRV_EEPROM_DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))

/**
  * @brief  eeprom nv24c0x device struct
  */
typedef struct {
    char *name;               /**< device name                      */
    wm_dt_hw_eeprom_t *hw;    /**< hardware info                    */
    wm_drv_eeprom_ops_t *ops; /**< device operation interface       */
    void *drv;                /**< driver context data              */
    wm_device_status_t state; /**< device state                     */
    void *priv;               /**< user private data                */
} wm_drv_eeprom_dev_t;

/**
  * @brief  config write protect pin by the config from device table
  */
static int wm_drv_eeprom_pin_config(wm_dt_hw_pin_cfg_t *pin_cfg, int count)
{
    wm_gpio_num_t pin;

    for (int i = 0; i < count; i++) {
        pin = pin_cfg[i].pin_num;
        wm_drv_gpio_iomux_func_sel(pin, pin_cfg[i].pin_mux);
        wm_drv_gpio_set_pullmode(pin, pin_cfg[i].pin_pupd);
        wm_drv_gpio_set_dir(pin, pin_cfg[i].pin_dir);
    }

    return WM_ERR_SUCCESS;
}

/**
  * @brief  get the remaining len int current page by offset
  */
static uint32_t eeprom_nv24c0x_get_remaining_in_page(wm_drv_eeprom_dev_t *dev, uint32_t offset, uint32_t len)
{
    uint32_t page_size = dev->hw->page_size;
    uint32_t remain    = page_size - offset % page_size;

    if (remain > len) {
        remain = len;
    }

    return remain;
}

/**
  * @brief  translate the i2c slave address and the sub address
  */
static uint16_t eeprom_nv24c0x_convert_address(wm_drv_eeprom_dev_t *dev, uint32_t offset, uint8_t *addr)
{
    wm_dt_hw_eeprom_t *hw = dev->hw;

    if (hw->addr_width > 8) {
        addr[0] = (uint8_t)((offset >> 8) & WM_BIT_MASK(hw->addr_width - 8));
        addr[1] = (uint8_t)(offset & 0xff);
    } else {
        addr[0] = offset & WM_BIT_MASK(hw->addr_width);
    }

    return hw->i2c_addr + (offset >> hw->addr_width);
}

static int eeprom_nv24c0x_get_size(wm_device_t *eeprom_device)
{
    wm_drv_eeprom_dev_t *dev = (wm_drv_eeprom_dev_t *)eeprom_device;
    return (int)(dev->hw->size);
}

static int eeprom_nv24c0x_set_wp(wm_device_t *eeprom_device, bool on)
{
    wm_drv_eeprom_dev_t *dev = (wm_drv_eeprom_dev_t *)eeprom_device;
    if (dev->hw->pin_cfg_count > 0) {
        if (on) {
            /*set gpio to high*/
            wm_drv_gpio_data_set(dev->hw->pin_cfg->pin_num);
        } else {
            /*set gpio to low*/
            wm_drv_gpio_data_reset(dev->hw->pin_cfg->pin_num);
        }
        return WM_ERR_SUCCESS;
    } else {
        return WM_ERR_NO_SUPPORT;
    }
}

static int eeprom_nv24c0x_init(wm_device_t *eeprom_device)
{
    wm_drv_eeprom_dev_t *dev = (wm_drv_eeprom_dev_t *)eeprom_device;
    wm_os_mutex_t *mutex;

    if (dev->state == WM_DEV_ST_INITED) {
        /*initialized before*/
        return WM_ERR_ALREADY_INITED;
    }

    /* create mutex for api calling*/
    if (wm_os_internal_recursive_mutex_create(&mutex) != WM_OS_STATUS_SUCCESS) {
        return WM_ERR_FAILED;
    }

    /* config write protect pin*/
    wm_drv_eeprom_pin_config(dev->hw->pin_cfg, dev->hw->pin_cfg_count);

    dev->drv   = (void *)mutex;
    dev->state = WM_DEV_ST_INITED;

    return WM_ERR_SUCCESS;
}

static int eeprom_nv24c0x_deinit(wm_device_t *eeprom_device)
{
    wm_drv_eeprom_dev_t *dev = (wm_drv_eeprom_dev_t *)eeprom_device;
    if (dev->state == WM_DEV_ST_INITED && dev->drv) {
        wm_os_internal_recursive_mutex_delete((wm_os_mutex_t *)dev->drv);
        dev->drv   = NULL;
        dev->state = WM_DEV_ST_UNINIT;
        return WM_ERR_SUCCESS;
    } else {
        return WM_ERR_FAILED;
    }
}

static int eeprom_nv24c0x_read(wm_device_t *eeprom_device, uint32_t offset, void *buf, uint32_t len)
{
    wm_drv_eeprom_dev_t *dev = (wm_drv_eeprom_dev_t *)eeprom_device;
    wm_dt_hw_eeprom_t *hw    = dev->hw;

    uint8_t addr[2];
    uint16_t i2c_addr;
    uint32_t len_in_page;
    int ret = WM_ERR_SUCCESS;

    wm_drv_i2c_config_t config = {
        .addr     = dev->hw->i2c_addr,
        .speed_hz = dev->hw->speed_hz,
    };
    wm_device_t *i2c_controler = wm_dt_get_device_by_name(dev->hw->i2c_device_name);

    WM_DRV_EEPROM_CHECK_PARAM(offset + len <= dev->hw->size);
    WM_DRV_EEPROM_CHECK_FAIL(dev->drv);
    WM_DRV_EEPROM_CHECK_FAIL(i2c_controler);

    WM_DRV_EEPROM_LOCK(dev->drv);

    /*read data by page */
    while (len) {
        /*get current i2c slave address and sub address*/
        i2c_addr = eeprom_nv24c0x_convert_address(dev, offset, addr);

        /*get read len in page*/
        len_in_page = eeprom_nv24c0x_get_remaining_in_page(dev, offset, len);

        config.addr = i2c_addr;

        /*read current page*/
        ret = wm_drv_i2c_read(i2c_controler, &config, addr, WM_DRV_EEPROM_DIV_ROUND_UP(hw->addr_width, 8), buf, len_in_page);
        if (ret < 0) {
            break;
        }

        /*move to next page*/
        len -= len_in_page;
        buf = ((char *)buf) + len_in_page;
        offset += len_in_page;
    }

    WM_DRV_EEPROM_UNLOCK(dev->drv);

    return ret;
}

static int eeprom_nv24c0x_write(wm_device_t *eeprom_device, uint32_t offset, const void *buf, uint32_t len)
{
    wm_drv_eeprom_dev_t *dev = (wm_drv_eeprom_dev_t *)eeprom_device;
    wm_dt_hw_eeprom_t *hw    = dev->hw;

    uint8_t addr[2];
    uint16_t i2c_addr;
    uint32_t len_in_page;
    int ret = WM_ERR_SUCCESS;

    wm_drv_i2c_config_t config = {
        .addr     = dev->hw->i2c_addr,
        .speed_hz = dev->hw->speed_hz,
    };
    wm_device_t *i2c_controler = wm_dt_get_device_by_name(dev->hw->i2c_device_name);

    WM_DRV_EEPROM_CHECK_PARAM(offset + len <= dev->hw->size);
    WM_DRV_EEPROM_CHECK_FAIL(dev->drv);
    WM_DRV_EEPROM_CHECK_FAIL(i2c_controler);
    WM_DRV_EEPROM_CHECK_FAIL(!dev->hw->read_only);

    WM_DRV_EEPROM_LOCK(dev->drv);

    /*turn off write protection*/
    eeprom_nv24c0x_set_wp(eeprom_device, 0);

    /*write data by page*/
    while (len) {
        /*get current i2c slave address and sub address*/
        i2c_addr = eeprom_nv24c0x_convert_address(dev, offset, addr);

        /*get write len in page*/
        len_in_page = eeprom_nv24c0x_get_remaining_in_page(dev, offset, len);

        config.addr = i2c_addr;

        /*write current page*/
        ret = wm_drv_i2c_write(i2c_controler, &config, addr, WM_DRV_EEPROM_DIV_ROUND_UP(hw->addr_width, 8), buf, len_in_page);
        if (ret < 0) {
            break;
        }

        /*move to next page*/
        len -= len_in_page;
        buf = ((char *)buf) + len_in_page;
        offset += len_in_page;

        /*wait eeprom write end*/
        wm_os_internal_time_delay_ms(hw->max_write_time_ms);
    }

    /*turn on write protection*/
    eeprom_nv24c0x_set_wp(eeprom_device, 1);

    WM_DRV_EEPROM_UNLOCK(dev->drv);

    return ret;
}

/*
NV24C0X api ops
*/
const wm_drv_eeprom_ops_t wm_drv_eeprom_nv24c0x_ops = {
    .init     = eeprom_nv24c0x_init,
    .deinit   = eeprom_nv24c0x_deinit,
    .read     = eeprom_nv24c0x_read,
    .write    = eeprom_nv24c0x_write,
    .get_size = eeprom_nv24c0x_get_size,
    .set_wp   = eeprom_nv24c0x_set_wp,
};

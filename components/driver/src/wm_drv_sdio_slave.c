/**
 * @file wm_drv_sdio_slave.c
 *
 * @brief SDIO Slave Driver Module
 *
 * This module implements the SDIO slave driver functionality for WinnerMicro devices.
 * It provides APIs for initializing, deinitializing, data transfer and command handling.
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
#include "wm_drv_sdio_slave.h"
#include "wm_dt_hw.h"
#include "wm_drv_ops_sdio_slave.c"

wm_device_t *wm_drv_sdio_slave_init(char *dev_name)
{
    int ret                      = WM_ERR_SUCCESS;
    wm_device_t *dev             = NULL;
    wm_drv_sdio_slave_ops_t *ops = NULL;

    dev = wm_dt_get_device_by_name(dev_name);
    if (dev && dev->ops) {
        if (dev->state != WM_DEV_ST_UNINIT) {
            return NULL;
        }
        ops = dev->ops;
        if (ops->init) {
            ret = ops->init(dev);
            if (ret == WM_ERR_SUCCESS) {
                return dev;
            }
        }
    }

    return NULL;
}

int wm_drv_sdio_slave_deinit(wm_device_t *dev)
{
    int ret                      = WM_ERR_SUCCESS;
    wm_drv_sdio_slave_ops_t *ops = NULL;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    ops = dev->ops;
    if (ops && ops->deinit) {
        ret = ops->deinit(dev);
    } else {
        return WM_ERR_INVALID_PARAM;
    }

    return ret;
}

int wm_drv_sdio_slave_register_callback(wm_device_t *dev, wm_drv_sdio_slave_callback_t cb, void *priv)
{
    int ret                      = WM_ERR_SUCCESS;
    wm_drv_sdio_slave_ops_t *ops = NULL;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    ops = dev->ops;
    if (ops && ops->register_callback) {
        ret = ops->register_callback(dev, cb, priv);
    } else {
        return WM_ERR_INVALID_PARAM;
    }

    return ret;
}

int wm_drv_sdio_slave_unregister_callback(wm_device_t *dev)
{
    int ret                      = WM_ERR_SUCCESS;
    wm_drv_sdio_slave_ops_t *ops = NULL;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    ops = dev->ops;
    if (ops && ops->unregister_callback) {
        ret = ops->unregister_callback(dev);
    } else {
        return WM_ERR_INVALID_PARAM;
    }

    return ret;
}

int wm_drv_sdio_slave_read_cis_reg(wm_device_t *dev, wm_drv_sdio_slave_func_t func, uint32_t pos, uint32_t *reg_val)
{
    int ret                      = WM_ERR_SUCCESS;
    wm_drv_sdio_slave_ops_t *ops = NULL;

    if (dev == NULL || reg_val == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    ops = dev->ops;
    if (ops && ops->read_cis_reg) {
        ret = ops->read_cis_reg(dev, func, pos, reg_val);
    } else {
        return WM_ERR_INVALID_PARAM;
    }

    return ret;
}

int wm_drv_sdio_slave_write_cis_reg(wm_device_t *dev, wm_drv_sdio_slave_func_t func, uint32_t pos, uint32_t reg_val)
{
    int ret                      = WM_ERR_SUCCESS;
    wm_drv_sdio_slave_ops_t *ops = NULL;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    ops = dev->ops;
    if (ops && ops->write_cis_reg) {
        ret = ops->write_cis_reg(dev, func, pos, reg_val);
    } else {
        return WM_ERR_INVALID_PARAM;
    }

    return ret;
}

int wm_drv_sdio_slave_tx_data_sync(wm_device_t *dev, void *addr, uint32_t len, uint32_t timeout)
{
    int ret                      = WM_ERR_SUCCESS;
    wm_drv_sdio_slave_ops_t *ops = NULL;

    if (dev == NULL || addr == NULL || len == 0) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    ops = dev->ops;
    if (ops && ops->tx_data_sync) {
        ret = ops->tx_data_sync(dev, addr, len, timeout);
    } else {
        return WM_ERR_INVALID_PARAM;
    }

    return ret;
}

int wm_drv_sdio_slave_tx_data_async(wm_device_t *dev, void *addr, uint32_t len)
{
    int ret                      = WM_ERR_SUCCESS;
    wm_drv_sdio_slave_ops_t *ops = NULL;

    if (dev == NULL || addr == NULL || len == 0) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    ops = dev->ops;
    if (ops && ops->tx_data_async) {
        ret = ops->tx_data_async(dev, addr, len);
    } else {
        return WM_ERR_INVALID_PARAM;
    }

    return ret;
}

int wm_drv_sdio_slave_rx_data(wm_device_t *dev, void **addr, uint32_t timeout)
{
    int ret                      = WM_ERR_SUCCESS;
    wm_drv_sdio_slave_ops_t *ops = NULL;

    if (dev == NULL || addr == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    ops = dev->ops;
    if (ops && ops->rx_data) {
        ret = ops->rx_data(dev, addr, timeout);
    } else {
        return WM_ERR_INVALID_PARAM;
    }

    return ret;
}

int wm_drv_sdio_slave_rx_cmd(wm_device_t *dev, void **addr, uint32_t timeout)
{
    int ret                      = WM_ERR_SUCCESS;
    wm_drv_sdio_slave_ops_t *ops = NULL;

    if (dev == NULL || addr == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    ops = dev->ops;
    if (ops && ops->rx_cmd) {
        ret = ops->rx_cmd(dev, addr, timeout);
    } else {
        return WM_ERR_INVALID_PARAM;
    }

    return ret;
}

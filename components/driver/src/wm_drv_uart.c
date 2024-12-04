/**
 * @file wm_drv_uart.c
 *
 * @brief hal uart Module
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

#include "wm_drv_uart.h"

/**
 * @brief Check param and return r
 */
#define WM_DRV_UART_CHECK_PARAM_RET(p, r) \
    if (!(p)) {                           \
        return (r);                       \
    }

/**
 * @brief Check param and return WM_ERR_INVALID_PARAM
 */
#define WM_DRV_UART_CHECK_PARAM(p)   \
    if (!(p)) {                      \
        return WM_ERR_INVALID_PARAM; \
    }

#include "wm_drv_ops_uart.c"

wm_device_t *wm_drv_uart_init(const char *dev_name, uint32_t rx_buf_size, uint32_t tx_buf_size)
{
    wm_device_t *dev = (dev_name ? wm_dt_get_device_by_name(dev_name) : NULL);
    WM_DRV_UART_CHECK_PARAM_RET(dev, NULL);
    return ((wm_drv_uart_ops_t *)dev->ops)->init(dev, rx_buf_size, tx_buf_size) == WM_ERR_SUCCESS ? dev : NULL;
}

int wm_drv_uart_deinit(wm_device_t *dev)
{
    WM_DRV_UART_CHECK_PARAM(dev);
    return ((wm_drv_uart_ops_t *)dev->ops)->deinit(dev);
}

int wm_drv_uart_ioctrl(wm_device_t *dev, int cmd, uint32_t param, void *arg)
{
    WM_DRV_UART_CHECK_PARAM(dev);
    return ((wm_drv_uart_ops_t *)dev->ops)->ioctrl(dev, cmd, param, arg);
}

int wm_drv_uart_register_callback(wm_device_t *dev, wm_drv_uart_callback_t cb, void *user_data)
{
    WM_DRV_UART_CHECK_PARAM(dev);
    return ((wm_drv_uart_ops_t *)dev->ops)->register_callback(dev, cb, user_data);
}

int wm_drv_uart_write(wm_device_t *dev, const uint8_t *data, uint32_t size)
{
    WM_DRV_UART_CHECK_PARAM(dev);
    return ((wm_drv_uart_ops_t *)dev->ops)->write(dev, data, size);
}

int wm_drv_uart_read(wm_device_t *dev, uint8_t *buf, uint32_t size, uint32_t timeout_ms)
{
    WM_DRV_UART_CHECK_PARAM(dev);
    return ((wm_drv_uart_ops_t *)dev->ops)->read(dev, buf, size, timeout_ms);
}

int wm_drv_uart_receive_data(wm_device_t *dev, uint8_t **buf, uint32_t *size)
{
    WM_DRV_UART_CHECK_PARAM(dev);
    return ((wm_drv_uart_ops_t *)dev->ops)->receive_data(dev, buf, size);
}

int wm_drv_uart_release_data(wm_device_t *dev, uint8_t *buf, uint32_t size)
{
    WM_DRV_UART_CHECK_PARAM(dev);
    return ((wm_drv_uart_ops_t *)dev->ops)->release_data(dev, buf, size);
}

int wm_drv_uart_set_config(wm_device_t *dev, wm_drv_uart_config_t *config)
{
    WM_DRV_UART_CHECK_PARAM(dev);
    return ((wm_drv_uart_ops_t *)dev->ops)->set_config(dev, config);
}

int wm_drv_uart_get_config(wm_device_t *dev, wm_drv_uart_config_t *config)
{
    WM_DRV_UART_CHECK_PARAM(dev);
    return ((wm_drv_uart_ops_t *)dev->ops)->get_config(dev, config);
}

int wm_drv_uart_set_baudrate(wm_device_t *dev, uint32_t baudrate)
{
    WM_DRV_UART_CHECK_PARAM(dev);
    return ((wm_drv_uart_ops_t *)dev->ops)->set_baudrate(dev, baudrate);
}

int wm_drv_uart_get_baudrate(wm_device_t *dev, uint32_t *baudrate)
{
    WM_DRV_UART_CHECK_PARAM(dev);
    return ((wm_drv_uart_ops_t *)dev->ops)->get_baudrate(dev, baudrate);
}

int wm_drv_uart_set_data_bits(wm_device_t *dev, wm_uart_data_bits_t data_bits)
{
    WM_DRV_UART_CHECK_PARAM(dev);
    return ((wm_drv_uart_ops_t *)dev->ops)->set_data_bits(dev, data_bits);
}

int wm_drv_uart_get_data_bits(wm_device_t *dev, wm_uart_data_bits_t *data_bits)
{
    WM_DRV_UART_CHECK_PARAM(dev);
    return ((wm_drv_uart_ops_t *)dev->ops)->get_data_bits(dev, data_bits);
}

int wm_drv_uart_set_stop_bits(wm_device_t *dev, wm_uart_stop_bits_t stop_bits)
{
    WM_DRV_UART_CHECK_PARAM(dev);
    return ((wm_drv_uart_ops_t *)dev->ops)->set_stop_bits(dev, stop_bits);
}

int wm_drv_uart_get_stop_bits(wm_device_t *dev, wm_uart_stop_bits_t *stop_bits)
{
    WM_DRV_UART_CHECK_PARAM(dev);
    return ((wm_drv_uart_ops_t *)dev->ops)->get_stop_bits(dev, stop_bits);
}

int wm_drv_uart_set_parity(wm_device_t *dev, wm_uart_parity_t parity)
{
    WM_DRV_UART_CHECK_PARAM(dev);
    return ((wm_drv_uart_ops_t *)dev->ops)->set_parity(dev, parity);
}

int wm_drv_uart_get_parity(wm_device_t *dev, wm_uart_parity_t *parity)
{
    WM_DRV_UART_CHECK_PARAM(dev);
    return ((wm_drv_uart_ops_t *)dev->ops)->get_parity(dev, parity);
}

int wm_drv_uart_set_flow_ctrl(wm_device_t *dev, wm_uart_flowctrl_t flow_ctrl)
{
    WM_DRV_UART_CHECK_PARAM(dev);
    return ((wm_drv_uart_ops_t *)dev->ops)->set_flow_ctrl(dev, flow_ctrl);
}

int wm_drv_uart_get_flow_ctrl(wm_device_t *dev, wm_uart_flowctrl_t *flow_ctrl)
{
    WM_DRV_UART_CHECK_PARAM(dev);
    return ((wm_drv_uart_ops_t *)dev->ops)->get_flow_ctrl(dev, flow_ctrl);
}

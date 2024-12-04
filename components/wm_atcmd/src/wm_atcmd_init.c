/**
 * @file wm_atcmd_init.c
 *
 * @brief ATCMD_INIT Module
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
#include "wm_atcmd.h"
#include "wm_drv_uart.h"

#define LOG_TAG "atcmd"
#include "wm_log.h"

#define WM_ATCMD_UART_DEV_NAME    CONFIG_WM_ATCMD_UART_DEVICE
#define WM_ATCMD_UART_RX_BUF_SIZE 1024
#define WM_ATCMD_UART_TX_BUF_SIZE 0

static wm_device_t *g_atcmd_uart = NULL;

extern int wm_atcmd_init_internal(void);

static int atcmd_send(const void *buffer, int size)
{
    if (g_atcmd_uart) {
        wm_drv_uart_write(g_atcmd_uart, (uint8_t *)buffer, size);
    }
    return size;
}

static int atcmd_recv(void *buffer, int *size, int timeout)
{
    int ret = wm_drv_uart_read(g_atcmd_uart, buffer, (uint32_t)(*size), timeout);
    *size   = (ret > 0 ? ret : 0);
    return *size;
}

int wm_atcmd_init(void)
{
    wm_dev_ops_t ops;
    int err;

    wm_device_t *dev = wm_dt_get_device_by_name(WM_ATCMD_UART_DEV_NAME);
    if (!dev || (dev && (WM_DEV_ST_INITED != dev->state))) {
        g_atcmd_uart = wm_drv_uart_init(WM_ATCMD_UART_DEV_NAME, WM_ATCMD_UART_RX_BUF_SIZE, WM_ATCMD_UART_TX_BUF_SIZE);
        if (!g_atcmd_uart) {
            wm_log_error("atcmd %s init failed.", WM_ATCMD_UART_DEV_NAME);
            return WM_ERR_FAILED;
        }
    } else if (dev && (WM_DEV_ST_INITED == dev->state)) {
        g_atcmd_uart = dev;
    }

    err = wm_atcmd_init_internal();
    if (err) {
        wm_log_error("atcmd init failed.");
        return WM_ERR_FAILED;
    }

    ops.atcmd_recv = atcmd_recv;
    ops.atcmd_send = atcmd_send;
    err            = wm_atcmd_dev_register(WM_ATCMD_DEV_TYPE_UART, ops);
    if (err) {
        wm_log_error("atcmd register dev failed.");
        return WM_ERR_FAILED;
    }

    wm_atcmd_print_result("ready\r\n");

    return WM_ERR_SUCCESS;
}

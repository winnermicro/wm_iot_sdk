/**
 * @file main.c
 *
 * @brief HSPI slave example demonstrating data transfer between host and slave
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

#include <stdio.h>
#include "wmsdk_config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "wm_drv_hspi_slave.h"

#define LOG_TAG "example"
#include "wm_log.h"

#define WM_HSPI_HOST_CMD_LEN 5
uint8_t wm_hspi_host_cmd[8] = { 0x5A, 0x00, 0x05, 0x01, 0x60 };

#define WM_HSPI_SLAVE_TX_DATA_SIZE (1024)
#define WM_HSPI_SLAVE_RX_DATA_SIZE (1024)

/* Event bits for HSPI events */
#define EVENT_RX_CMD_BIT           (1 << 0) /* Bit for command receive event */
#define EVENT_RX_BIT               (1 << 1) /* Bit for data receive event */
#define EVENT_TX_BIT               (1 << 2) /* Bit for data transmit event */

/* Event group handle for asynchronizing HSPI events */
static EventGroupHandle_t event_group;

static void hspi_slave_callback(wm_device_t *dev, wm_drv_hspi_slave_event_t event, void *addr, void *priv)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* Set corresponding event bits based on event type */
    if (event == WM_DRV_HSPI_SLAVE_EVENT_RX_CMD) {
        xEventGroupSetBitsFromISR(event_group, EVENT_RX_CMD_BIT, &xHigherPriorityTaskWoken);
    } else if (event == WM_DRV_HSPI_SLAVE_EVENT_RX) {
        xEventGroupSetBitsFromISR(event_group, EVENT_RX_BIT, &xHigherPriorityTaskWoken);
    } else if (event == WM_DRV_HSPI_SLAVE_EVENT_TX) {
        xEventGroupSetBitsFromISR(event_group, EVENT_TX_BIT, &xHigherPriorityTaskWoken);
    }

    /* Yield if higher priority task is woken */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static int hspi_slave_tx_data_sync_demo(wm_device_t *dev)
{
    int ret = WM_ERR_SUCCESS;

    /* Allocate buffer for test data */
    void *tx_buf = wm_heap_caps_alloc(WM_HSPI_SLAVE_TX_DATA_SIZE, WM_HEAP_CAP_SHARED);
    if (tx_buf == NULL) {
        wm_log_error("Failed to allocate memory for tx buffer");
        return WM_ERR_NO_MEM;
    }

    /* Fill buffer with test pattern */
    for (int i = 0; i < WM_HSPI_SLAVE_TX_DATA_SIZE; i++) {
        ((char *)tx_buf)[i] = (i + 1) % 255;
    }

    /* Send data synchronously with 1s timeout */
    ret = wm_drv_hspi_slave_tx_data_sync(dev, tx_buf, WM_HSPI_SLAVE_TX_DATA_SIZE, 1000);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("Failed to send sync data with error code: %d", ret);
    }
    wm_log_info("SYNC TX: %d bytes", WM_HSPI_SLAVE_TX_DATA_SIZE);

    wm_heap_caps_free(tx_buf);
    tx_buf = NULL;

    return ret;
}

static int hspi_slave_tx_data_async_demo(wm_device_t *dev)
{
    int ret = WM_ERR_SUCCESS;
    EventBits_t bits;

    /* Allocate buffer for test data */
    void *tx_buf = wm_heap_caps_alloc(WM_HSPI_SLAVE_TX_DATA_SIZE, WM_HEAP_CAP_SHARED);
    if (tx_buf == NULL) {
        wm_log_error("Failed to allocate memory for tx buffer");
        return WM_ERR_NO_MEM;
    }

    /* Fill buffer with test pattern */
    for (int i = 0; i < WM_HSPI_SLAVE_TX_DATA_SIZE; i++) {
        ((char *)tx_buf)[i] = (i + 1) % 255;
    }

    /* Send data asynchronously */
    ret = wm_drv_hspi_slave_tx_data_async(dev, tx_buf, WM_HSPI_SLAVE_TX_DATA_SIZE);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("Failed to send async data with error code: %d", ret);
    }

    /* Wait for transmission complete event */
    bits = xEventGroupWaitBits(event_group, EVENT_TX_BIT, pdTRUE, pdTRUE, portMAX_DELAY);
    if (bits & EVENT_TX_BIT) {
        wm_log_info("ASYNC TX: %d bytes", WM_HSPI_SLAVE_TX_DATA_SIZE);
        wm_heap_caps_free(tx_buf);
        tx_buf = NULL;
    }

    return ret;
}

static void rx_data_parse(char *buf)
{
    int i = 0, err_num = 0;

    for (i = 0; i < WM_HSPI_SLAVE_RX_DATA_SIZE; i++) {
        if (buf[i] != ((i + 1) % 255))
            err_num++;
    }

    if (err_num != 0) {
        wm_log_info("HSPI slave Rx ERR");
        wm_log_dump(WM_LOG_LEVEL_INFO, "[RX DATA:]", 16, buf, WM_HSPI_SLAVE_RX_DATA_SIZE);
    } else {
        wm_log_info("HSPI slave RX OK");
    }
}

int main(void)
{
    int ret          = WM_ERR_SUCCESS;
    wm_device_t *dev = NULL;
    EventBits_t bits;
    int sync_async_sel = 0;
    wm_drv_hspi_slave_cfg_t drv_cfg;

    wm_log_info("HSPI slave demo start");

    /* Create event group for synchronization */
    event_group = xEventGroupCreate();
    if (event_group == NULL) {
        wm_log_error("Failed to create event group");
        return -1;
    }

    /* Initialize HSPI slave device */
    wm_log_info("Initializing hspi slave device...");
    dev = wm_drv_hspi_slave_init("hspi_slave");

    /* config HSPI interface */
    memset(&drv_cfg, 0, sizeof(drv_cfg));
    drv_cfg.cpol = WM_HSPI_CPOL_LOW;
    drv_cfg.cpha = WM_HSPI_CPHA_FIRST_EDGE;
    wm_drv_hspi_slave_config(dev, &drv_cfg);

    /* Register callback if initialization successful */
    if (dev != NULL) {
        wm_log_info("Successfully initialized hspi slave device");
        ret = wm_drv_hspi_slave_register_callback(dev, (wm_drv_hspi_slave_callback_t)hspi_slave_callback, NULL);
        if (ret != WM_ERR_SUCCESS) {
            wm_log_error("Failed to register callback for hspi slave device with error code: %d", ret);
        }
    } else {
        wm_log_error("Failed to initialize hspi slave device");
    }

    /* Main event loop */
    wm_log_info("Waiting for rx events...");
    //while (count++ < 3) {
    while (1) {
        /* Wait for RX events */
        bits = xEventGroupWaitBits(event_group, EVENT_RX_CMD_BIT | EVENT_RX_BIT, pdTRUE, /* Clear bits before returning */
                                   pdFALSE,                                              /* Wait for any bit, not all */
                                   portMAX_DELAY);

        /* Handle command receive event */
        if (bits & EVENT_RX_CMD_BIT) {
            void *cmd_buf = NULL;
            wm_drv_hspi_slave_rx_cmd(dev, &cmd_buf, 0);
            wm_log_info("RX cmd complete");
            wm_log_dump(WM_LOG_LEVEL_INFO, "[RX CMD:]", 16, cmd_buf, sizeof(wm_hspi_host_cmd));

            if (memcmp(cmd_buf, wm_hspi_host_cmd, WM_HSPI_HOST_CMD_LEN) == 0) {
                /* Send alternately using synchronous or asynchronous modes. */
                if (sync_async_sel & 0x01) {
                    /* Send response synchronously */
                    ret = hspi_slave_tx_data_sync_demo(dev);
                    if (ret != WM_ERR_SUCCESS) {
                        wm_log_error("Failed to send sync data with error code: %d", ret);
                    }
                } else {
                    /* Send response asynchronously */
                    ret = hspi_slave_tx_data_async_demo(dev);
                    if (ret != WM_ERR_SUCCESS) {
                        wm_log_error("Failed to send async data with error code: %d", ret);
                    }
                }
                sync_async_sel++;
            }
        }

        /* Handle data receive event */
        if (bits & EVENT_RX_BIT) {
            void *rx_buf = NULL;

            ret = wm_drv_hspi_slave_rx_data(dev, &rx_buf, 0);
            if (ret == WM_ERR_SUCCESS) {
                wm_log_info("RX data complete");
                rx_data_parse((char *)rx_buf);
            } else {
                wm_log_error("Failed to receive data with error code: %d", ret);
            }
        }
    }

    ret = wm_drv_hspi_slave_deinit(dev);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("Failed to deinitialize hspi slave device with error code: %d", ret);
    } else {
        wm_log_info("This example succeeds in running.");
    }

    return 0;
}

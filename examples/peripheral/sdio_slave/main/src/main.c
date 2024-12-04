/**
 * @file main.c
 *
 * @brief SDIO slave example demonstrating data transfer between host and slave
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
#include "wm_drv_sdio_slave.h"
#include "cis.h"

#define LOG_TAG "example"
#include "wm_log.h"

#define WM_SDIO_SLAVE_TX_DATA_SIZE (2048)

/* Event bits for SDIO events */
#define EVENT_RX_CMD_BIT           (1 << 0) /* Bit for command receive event */
#define EVENT_RX_BIT               (1 << 1) /* Bit for data receive event */
#define EVENT_TX_BIT               (1 << 2) /* Bit for data transmit event */

/* Event group handle for synchronizing SDIO events */
static EventGroupHandle_t event_group;

static void sdio_slave_callback(wm_device_t *dev, wm_drv_sdio_slave_event_t event, void *addr, void *priv)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* Set corresponding event bits based on event type */
    if (event == WM_DRV_SDIO_SLAVE_EVENT_RX_CMD) {
        xEventGroupSetBitsFromISR(event_group, EVENT_RX_CMD_BIT, &xHigherPriorityTaskWoken);
    } else if (event == WM_DRV_SDIO_SLAVE_EVENT_RX) {
        xEventGroupSetBitsFromISR(event_group, EVENT_RX_BIT, &xHigherPriorityTaskWoken);
    } else if (event == WM_DRV_SDIO_SLAVE_EVENT_TX) {
        xEventGroupSetBitsFromISR(event_group, EVENT_TX_BIT, &xHigherPriorityTaskWoken);
    }

    /* Yield if higher priority task is woken */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static int sdio_slave_tx_data_sync_demo(wm_device_t *dev)
{
    int ret = WM_ERR_SUCCESS;

    /* Allocate buffer for test data */
    void *tx_buf = wm_heap_caps_alloc(WM_SDIO_SLAVE_TX_DATA_SIZE, WM_HEAP_CAP_SHARED);
    if (tx_buf == NULL) {
        wm_log_error("Failed to allocate memory for tx buffer");
        return WM_ERR_NO_MEM;
    }

    /* Fill buffer with test pattern */
    for (int i = 0; i < WM_SDIO_SLAVE_TX_DATA_SIZE; i++) {
        ((char *)tx_buf)[i] = 'a' + (i % 26);
    }
    ((char *)tx_buf)[WM_SDIO_SLAVE_TX_DATA_SIZE - 1] = '\0';

    /* Send data synchronously with 1s timeout */
    ret = wm_drv_sdio_slave_tx_data_sync(dev, tx_buf, WM_SDIO_SLAVE_TX_DATA_SIZE, 1000);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("Failed to send sync data with error code: %d", ret);
    }
    wm_log_info("SYNC TX: %s", (char *)tx_buf);

    wm_heap_caps_free(tx_buf);
    tx_buf = NULL;

    return ret;
}

static int sdio_slave_tx_data_async_demo(wm_device_t *dev)
{
    int ret = WM_ERR_SUCCESS;
    EventBits_t bits;

    /* Allocate buffer for test data */
    void *tx_buf = wm_heap_caps_alloc(WM_SDIO_SLAVE_TX_DATA_SIZE, WM_HEAP_CAP_SHARED);
    if (tx_buf == NULL) {
        wm_log_error("Failed to allocate memory for tx buffer");
        return WM_ERR_NO_MEM;
    }

    /* Fill buffer with test pattern */
    for (int i = 0; i < WM_SDIO_SLAVE_TX_DATA_SIZE; i++) {
        ((char *)tx_buf)[i] = 'A' + (i % 26);
    }
    ((char *)tx_buf)[WM_SDIO_SLAVE_TX_DATA_SIZE - 1] = '\0';

    /* Send data asynchronously */
    ret = wm_drv_sdio_slave_tx_data_async(dev, tx_buf, WM_SDIO_SLAVE_TX_DATA_SIZE);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("Failed to send async data with error code: %d", ret);
    }

    /* Wait for transmission complete event */
    bits = xEventGroupWaitBits(event_group, EVENT_TX_BIT, pdTRUE, pdTRUE, portMAX_DELAY);
    if (bits & EVENT_TX_BIT) {
        wm_log_info("ASYNC TX: %s", (char *)tx_buf);
        wm_heap_caps_free(tx_buf);
        tx_buf = NULL;
    }

    return ret;
}

int main(void)
{
    int ret          = WM_ERR_SUCCESS;
    wm_device_t *dev = NULL;
    EventBits_t bits;
    int count = 0;

    wm_log_info("SDIO slave demo start");

    /* Create event group for synchronization */
    event_group = xEventGroupCreate();
    if (event_group == NULL) {
        wm_log_error("Failed to create event group");
        return -1;
    }

    /* Initialize SDIO slave device */
    wm_log_info("Initializing sdio slave device...");
    dev = wm_drv_sdio_slave_init("sdio_slave");

    /* Initialize CIS registers */
    ret = wm_sdio_slave_cis_init(dev);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("Failed to initialize CIS registers with error code: %d", ret);
    }

    /* Register callback if initialization successful */
    if (dev != NULL) {
        wm_log_info("Successfully initialized sdio slave device");
        ret = wm_drv_sdio_slave_register_callback(dev, (wm_drv_sdio_slave_callback_t)sdio_slave_callback, NULL);
        if (ret != WM_ERR_SUCCESS) {
            wm_log_error("Failed to register callback for sdio slave device with error code: %d", ret);
        }
    } else {
        wm_log_error("Failed to initialize sdio slave device");
    }

    /* Main event loop */
    wm_log_info("Waiting for rx events...");
    while (count++ < 3) {
        /* Wait for RX events */
        bits = xEventGroupWaitBits(event_group, EVENT_RX_CMD_BIT | EVENT_RX_BIT, pdTRUE, /* Clear bits before returning */
                                   pdFALSE,                                              /* Wait for any bit, not all */
                                   portMAX_DELAY);

        /* Handle command receive event */
        if (bits & EVENT_RX_CMD_BIT) {
            void *cmd_buf = NULL;
            wm_drv_sdio_slave_rx_cmd(dev, &cmd_buf, 0);
            wm_log_info("RX CMD: %s", (char *)cmd_buf);

            /* Send response synchronously */
            ret = sdio_slave_tx_data_sync_demo(dev);
            if (ret != WM_ERR_SUCCESS) {
                wm_log_error("Failed to send sync data with error code: %d", ret);
            }
        }

        /* Handle data receive event */
        if (bits & EVENT_RX_BIT) {
            void *rx_buf = NULL;

            ret = wm_drv_sdio_slave_rx_data(dev, &rx_buf, 0);
            if (ret == WM_ERR_SUCCESS) {
                wm_log_info("RX: %s", (char *)rx_buf);
            } else {
                wm_log_error("Failed to receive data with error code: %d", ret);
            }

            /* Send response asynchronously */
            ret = sdio_slave_tx_data_async_demo(dev);
            if (ret != WM_ERR_SUCCESS) {
                wm_log_error("Failed to send async data with error code: %d", ret);
            }
        }
    }

    ret = wm_drv_sdio_slave_deinit(dev);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("Failed to deinitialize sdio slave device with error code: %d", ret);
    } else {
        wm_log_info("This example succeeds in running.");
    }

    return 0;
}

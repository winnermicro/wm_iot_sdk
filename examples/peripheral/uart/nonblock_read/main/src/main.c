/**
 * @file main.c
 *
 * @brief uart example
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
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "wm_types.h"
#include "wm_error.h"
#include "wm_drv_uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#define LOG_TAG "exam_uart"
#include "wm_log.h"

#define UART_DEV_NAME "uart1"
#define RX_BUF_SIZE   4096
#define READ_BUF_SIZE 2048

static xQueueHandle receive_evt_queue = NULL;

void example_uart_evt_callback(wm_device_t *dev, wm_drv_uart_evt_t *evt, void *user_data)
{
    if (evt->type == WM_DRV_UART_RX_READY) {
        /*The callback is called from interrupt service routine
        Do not process data at callback, notify the task to process the data.*/
        uint32_t type = evt->type;
        xQueueSendToBackFromISR(receive_evt_queue, &type, NULL);
    }
}

void example_uart_nonblock_read_with_event(wm_device_t *dev)
{
    uint8_t *buf;
    int len;
    uint32_t type;

    buf = malloc(READ_BUF_SIZE);
    if (!buf) {
        return;
    }

    while (1) {
        /*receive uart event from callback*/
        if (xQueueReceive(receive_evt_queue, &type, 1000) == pdPASS && type == WM_DRV_UART_RX_READY) {
            while (1) {
                /*read data from uart.*/
                len = wm_drv_uart_read(dev, buf, READ_BUF_SIZE, 1000);
                if (len > 0) {
                    /* If uart wants to use high speed baudrate, the following logs may affect reception, you can removed it */
                    wm_log_info("main recv [%d]:%.*s\n", len, len, buf);

                    /*write data back to uart.*/
                    wm_drv_uart_write(dev, buf, len);
                } else {
                    break;
                }
            }
        }
    }

    free(buf);
}

static void example_uart_task(void *parameters)
{
    wm_device_t *dev;

    /*initialize uart*/
    dev = wm_drv_uart_init(UART_DEV_NAME, RX_BUF_SIZE, 0);

    /*create a freertos queue to receive uart event from callback*/
    receive_evt_queue = xQueueCreate(8, sizeof(uint32_t));
    if (!receive_evt_queue) {
        wm_log_error("create queue fail");
        return;
    }

    /*register uart event*/
    wm_drv_uart_register_callback(dev, example_uart_evt_callback, dev);

    /*start uart receive example*/
    example_uart_nonblock_read_with_event(dev);
}

int main(void)
{
    wm_log_info("uart noblock read demo start.\n");

    xTaskCreate(example_uart_task, "uart_example", 1024, NULL, 5, NULL);

    return 0;
}

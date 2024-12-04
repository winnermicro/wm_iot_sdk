/**
 * @file main.c
 *
 * @brief psram example
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
#include "wm_utils.h"

#include "wm_drv_psram.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wm_heap.h"

#define LOG_TAG "example_psram"
#include "wm_log.h"

#define PSRAM_DEVICE_NAME "psram"
#define PSRAM_COPY_SIZE   20
#define PSRAM_DMA_SIZE    1024

volatile static uint8_t g_psram_dma_done = 0;

/**
  * @brief  integer read/write
  */
void example_psram_integer(wm_device_t *dev)
{
    uint32_t *p;

    p = wm_heap_caps_alloc(sizeof(*p), WM_HEAP_CAP_SPIRAM);
    if (p) {
        *p = 0x11223344;
        wm_log_info("write 32bits integer 0x11223344 to address=%p", p);
        wm_log_info("read 32bits integer from address=%p,value=0x%x", p, *p);

        wm_heap_caps_free(p);
    } else {
        wm_log_error("alloc from psram fail.");
    }
}

/**
  * @brief  memory copy by cpu
  */
void example_psram_memcpy(wm_device_t *dev)
{
    char *p;
    uint8_t str[] = "0123456789abcdefghij";
    uint8_t value[PSRAM_COPY_SIZE];

    p = wm_heap_caps_alloc(PSRAM_COPY_SIZE, WM_HEAP_CAP_SPIRAM);
    if (p) {
        memcpy(p, str, PSRAM_COPY_SIZE);
        memcpy(value, p, PSRAM_COPY_SIZE);
        wm_log_info("CPU copy %s to address=%p", str, p);
        wm_log_info("CPU read string from address=%p,value=%.*s", p, PSRAM_COPY_SIZE, value);
        wm_heap_caps_free(p);
    } else {
        wm_log_error("alloc from psram fail.");
    }
}

static void example_psram_dma_done_callback(wm_device_t *device, void *user_data)
{
    g_psram_dma_done = 1;
}

/**
  * @brief  memory copy by cpu
  */
void example_psram_dma(wm_device_t *dev)
{
    char *p;
    const char *str = "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
                      "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
                      "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
                      "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
                      "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
                      "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
                      "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
                      "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
                      "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
                      "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
                      "abcdefghijklmnopqrstuvwx";
    char *str_bak;

    /*DMA not support flash data, str is in flash, copy it to sram*/
    str_bak = strdup(str);

    if (!str_bak) {
        wm_log_error("no memory");
        return;
    }

    p = wm_heap_caps_alloc(PSRAM_DMA_SIZE, WM_HEAP_CAP_SPIRAM);
    if (p) {
        wm_drv_psram_register_callback(dev, example_psram_dma_done_callback, dev);

        g_psram_dma_done = 0;

        wm_drv_psram_memcpy_dma(dev, p, str_bak, PSRAM_DMA_SIZE);
        wm_log_info("DMA copy string to address=%p,string is:\n%s", p, str);

        while (!g_psram_dma_done) {
            vTaskDelay(pdMS_TO_TICKS(1));
        }

        wm_log_info("DMA read string from address=%p,value is\n=%.*s", p, PSRAM_DMA_SIZE, p);

        wm_drv_psram_register_callback(dev, NULL, NULL);

        wm_heap_caps_free(p);
    } else {
        wm_log_error("alloc from psram fail.");
    }

    free(str_bak);
}

int main(void)
{
    wm_device_t *dev;

    wm_log_info("psram write/read example.\n");

    dev = wm_dt_get_device_by_name(PSRAM_DEVICE_NAME);
    if (!dev) {
        wm_log_error("Can't find %s.", PSRAM_DEVICE_NAME);
    }

    example_psram_integer(dev);
    example_psram_memcpy(dev);
    example_psram_dma(dev);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    return 0;
}

/**
 * @file main.c
 *
 * @brief spi master sync transceive example
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

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "wm_types.h"
#include "wm_error.h"
#include "wm_soc_cfgs.h"
#include "wm_drv_spi_slave.h"
#include "wm_drv_gpio.h"

#define LOG_TAG "exam_spis"
#include "wm_log.h"

void example_spis_transceive_sync(wm_device_t *dev)
{
    int ret                        = 0;
    spis_transceive_t desc         = { 0 };
    uint32_t tx_len                = 256;
    uint8_t *tx_buf                = NULL;
    uint32_t rx_len                = 256;
    uint8_t *rx_buf                = NULL;
    wm_dt_hw_spis_dev_cfg_t config = {
        .mode = 0,
    };

    tx_buf = malloc(tx_len);
    assert(tx_buf != NULL);
    for (int i = 0; i < tx_len; i++) {
        tx_buf[i] = i;
    }

    rx_buf = malloc(rx_len);
    assert(rx_buf != NULL);

    desc.rx_buf = rx_buf;
    desc.rx_len = rx_len;
    desc.tx_buf = tx_buf;
    desc.tx_len = tx_len;
    ret         = wm_drv_spis_transceive_sync(dev, &config, &desc, 2000);
    if (ret == WM_ERR_SUCCESS) {
        wm_log_info("rx data as below");
        wm_log_dump_info("rx data", rx_buf, rx_len);
        wm_log_info("This example succeeds in running.");
    } else {
        wm_log_error("Sync transceive failed with error code: %d", ret);
    }

    free(tx_buf);
    free(rx_buf);
}

int main(void)
{
    wm_device_t *dev = NULL;

    wm_log_info("spi master sync transecie start.");

    /*initialize spi master*/
    dev = wm_drv_spis_init("spis");
    assert(dev != NULL);

    /*start spi master sync transceive example*/
    example_spis_transceive_sync(dev);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    return 0;
}

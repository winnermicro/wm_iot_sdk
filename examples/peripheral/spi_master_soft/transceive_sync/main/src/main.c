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
#include "wm_drv_spi_master.h"
#include "wm_drv_gpio.h"

#define LOG_TAG "exam_spim_soft"
#include "wm_log.h"

void example_spim_transceive_sync(wm_device_t *dev)
{
    spim_transceive_t desc = { 0 };
    uint32_t tx_len        = 256;
    uint8_t *tx_buf        = NULL;
    uint32_t rx_len        = 256;
    uint8_t *rx_buf        = NULL;
    wm_dt_hw_spim_dev_cfg_t config = {
        .freq = 2 * 1000000, //2M clock
        .mode = 0,
        .pin_cs = {
            .pin_num = WM_GPIO_NUM_27,
            .pin_mux = WM_GPIO_IOMUX_FUN5,
        },
    };

    wm_drv_gpio_iomux_func_sel(config.pin_cs.pin_num, WM_GPIO_IOMUX_FUN5);
    wm_drv_gpio_set_pullmode(config.pin_cs.pin_num, WM_GPIO_FLOAT);
    wm_drv_gpio_set_dir(config.pin_cs.pin_num, WM_GPIO_DIR_OUTPUT);
    //default CS is invalid
    wm_drv_gpio_data_set(config.pin_cs.pin_num);

    tx_buf = malloc(tx_len);
    rx_buf = malloc(rx_len);

    if (tx_buf != NULL && rx_buf != NULL) {
        for (int i = 0; i < tx_len; i++) {
            tx_buf[i] = 0xAA;
        }

        for (int i = 0; i < rx_len; i++) {
            rx_buf[i] = 0xAA;
        }

        desc.rx_buf = rx_buf;
        desc.rx_len = rx_len;
        desc.tx_buf = tx_buf;
        desc.tx_len = tx_len;
        wm_drv_spim_transceive_sync(dev, &config, &desc, 10000);

        wm_log_info("rx data as below");
        wm_log_dump_info("rx data", rx_buf, rx_len);
    } else {
        wm_log_error("buffer create fail");
    }

    if (tx_buf) {
        free(tx_buf);
    }

    if (rx_buf) {
        free(rx_buf);
    }
}

int main(void)
{
    wm_device_t *dev = NULL;

    wm_log_info("spi master sync transecie start.");

    /*initialize spi master*/
    dev = wm_drv_spim_init("spim_soft");
    if (dev == NULL) {
        wm_log_info("spi soft master init fail.");
        return WM_ERR_FAILED;
    }

    /*start spi master sync transceive example*/
    example_spim_transceive_sync(dev);

    return 0;
}

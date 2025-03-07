/**
 * @file main.c
 *
 * @brief Use spi master tx comand and get result example
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

#define LOG_TAG "exam_spim"
#include "wm_log.h"

//contol spi-flash, fast read data from flash address 0x1fb000
int example_spim_read_data_from_flash(wm_device_t *dev)
{
    int ret                      = 0;
    uint32_t rx_len              = 256;
    uint8_t *rx_buf              = NULL;
    spim_transceive_ex_t desc_ex = { 0 };
    wm_dt_hw_spim_dev_cfg_t config = {
        .freq = 2 * 1000000, //2M clock
        .mode = 0,
        .pin_cs = {
            .pin_num = WM_GPIO_NUM_26,
            .pin_mux = WM_GPIO_IOMUX_FUN5,
        },
    };

    rx_buf = wm_os_internal_malloc(rx_len);
    if (!rx_buf) {
        wm_log_error("alloc rx buf failed\n");
        return -1;
    }

    wm_drv_gpio_iomux_func_sel(config.pin_cs.pin_num, WM_GPIO_IOMUX_FUN5);
    wm_drv_gpio_set_pullmode(config.pin_cs.pin_num, WM_GPIO_FLOAT);
    wm_drv_gpio_set_dir(config.pin_cs.pin_num, WM_GPIO_DIR_OUTPUT);
    //default CS is invalid
    wm_drv_gpio_data_set(config.pin_cs.pin_num);

    desc_ex.cmd         = 0xB; //commd id
    desc_ex.cmd_len     = 1;   //1 byte command
    desc_ex.basic.flags = SPI_TRANS_VARIABLE_CMD;
    desc_ex.addr        = 0x1fb000; //flash begin address will be read
    desc_ex.addr_len    = 3;        //3 byte addr
    desc_ex.basic.flags |= SPI_TRANS_VARIABLE_ADDR;
    desc_ex.dummy_bits = 1 * 8; //dummy: 8 bit
    desc_ex.basic.flags |= SPI_TRANS_DUMMY_BITS;
    desc_ex.basic.tx_buf = NULL;
    desc_ex.basic.tx_len = 0;
    desc_ex.basic.rx_buf = rx_buf;
    desc_ex.basic.rx_len = rx_len;

    ret = wm_drv_spim_transceive_sync(dev, &config, &desc_ex.basic, 1000);

    if (ret == WM_ERR_SUCCESS) {
        wm_log_info("read flash data succ.\n");
        wm_log_dump(WM_LOG_LEVEL_INFO, "flash_data", 20, rx_buf, 256);
    } else {
        wm_log_error("read flash data error: %d\n", ret);
    }

    free(rx_buf);

    return ret;
}

int main(void)
{
    wm_device_t *dev = NULL;

    wm_log_info("spi master get flash data examle.\n");

    /*initialize spi master*/
    dev = wm_drv_spim_init("spim");

    if (dev) {
        /*start spi master sync transceive example*/
        example_spim_read_data_from_flash(dev);
    } else {
        wm_log_error("example failed in spim init\n");
    }

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    return 0;
}

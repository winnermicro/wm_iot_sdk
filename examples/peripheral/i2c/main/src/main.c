/**
 * @file main.c
 *
 * @brief i2c example
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
#include "wm_drv_i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#define LOG_TAG "example_i2c"
#include "wm_log.h"

/**
  * @brief  i2c example, write/read eeprom by i2c api directly
  */
#define I2C_CONTROLER_NAME             "i2c"
#define EEPROM_NV24C0X_ADDRESS         0x52
#define EEPROM_NV24C0X_SUB_ADDRESS_LEN 1
#define EEPROM_NV24C0X_SPEED           400000
#define EEPROM_PAGE_SIZE               16

void example_i2c_write_read(wm_device_t *dev)
{
    int ret;
    uint8_t sub_addr = 0; /*read page 0*/

    wm_drv_i2c_config_t config = {
        .addr     = EEPROM_NV24C0X_ADDRESS,
        .speed_hz = EEPROM_NV24C0X_SPEED,
    };

    char str[EEPROM_PAGE_SIZE] = "0123456789abcdef";
    char buf[EEPROM_PAGE_SIZE];

    /*write str to page 0*/
    ret = wm_drv_i2c_write(dev, &config, &sub_addr, EEPROM_NV24C0X_SUB_ADDRESS_LEN, str, EEPROM_PAGE_SIZE);
    wm_log_info("write end,ret=%d", ret);

    /*wait eeprom write end.*/
    vTaskDelay(10);

    /*read a page from page 0*/
    ret = wm_drv_i2c_read(dev, &config, &sub_addr, EEPROM_NV24C0X_SUB_ADDRESS_LEN, buf, sizeof(buf));
    wm_log_info("read end,ret=%d", ret);

    if (ret == WM_ERR_SUCCESS) {
        wm_log_info("read OK:%.*s", sizeof(buf), buf);
    } else {
        wm_log_error("read fail,ret=%d", ret);
    }
}

int main(void)
{
    wm_device_t *dev;

    wm_log_info("i2c write/read example.\n");

    /*initialize i2c*/
    dev = wm_drv_i2c_init(I2C_CONTROLER_NAME);
    if (dev) {
        /*start eeprom read write*/
        example_i2c_write_read(dev);
    } else {
        wm_log_error("init fail.");
    }

    while (1) {
        vTaskDelay(1000);
    }

    return 0;
}

/**
 * @file main.c
 *
 * @brief eeprom example
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
#include "wm_drv_eeprom.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#define LOG_TAG "example_eeprom"
#include "wm_log.h"

#define I2C_CONTROLER_NAME "i2c"
#define EEPROM_DEVICE_NAME "eeprom0"

/**
  * @brief  eeprom example, write/read eeprom by eeprom device driver api.
  */

void example_eeprom_write_read(wm_device_t *dev)
{
    int ret;
    uint8_t offset = 5;

    char *str = "0123456789abcdefghijklmn";
    int len   = strlen(str);
    char buf[len];

    /*write string to eeprom*/
    ret = wm_drv_eeprom_write(dev, offset, str, len);
    wm_log_info("write end,len=%d,ret=%d", len, ret);

    /*read string from eeprom*/
    ret = wm_drv_eeprom_read(dev, offset, buf, len);
    wm_log_info("read end,ret=%d", ret);

    if (ret == WM_ERR_SUCCESS) {
        wm_log_info("read OK:%.*s", len, buf);
    } else {
        wm_log_error("read fail,ret=%d", ret);
    }
}

int main(void)
{
    wm_device_t *dev;

    wm_log_info("eeprom write/read example.\n");

    /*initialize i2c*/
    if (!wm_drv_i2c_init(I2C_CONTROLER_NAME)) {
        wm_log_error("init i2c fail.");
        return -1;
    }

    /*initialize eeprom*/
    dev = wm_drv_eeprom_init(EEPROM_DEVICE_NAME);
    if (dev) {
        /*start eeprom read write*/
        example_eeprom_write_read(dev);
    } else {
        wm_log_error("init fail.");
        return -1;
    }

    while (1) {
        vTaskDelay(1000);
    }

    return 0;
}

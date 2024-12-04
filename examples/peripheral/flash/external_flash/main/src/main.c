/**
 * @file main.c
 *
 * @brief flash example
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
#include "wm_drv_flash.h"

#define LOG_TAG "flash"
#include "wm_log.h"

#define TEST_FLASH_BEGING_ADDR (2 * 1024 * 1024) - 0x5000
#define TEST_LEN               20

int example_wr_rd_external_flash(void)
{
    wm_device_t *dev                = NULL;
    uint8_t write_buf[TEST_LEN + 1] = { 0 };
    uint8_t read_buf[TEST_LEN + 1]  = { 0 };
    int ret                         = 0;

    wm_log_info("start example write and read external flash");

    for (int i = 0; i < TEST_LEN; i++) {
        write_buf[i] = 'a' + i;
    }

    /*initialize spi master*/
    dev = wm_drv_flash_init("flash_w25q");
    if (dev == NULL) {
        wm_log_error("init external flash error");
        return -1;
    }

    //case1: call write without erase API step
    //step1: erase region
    //case1: call write without erase API step
    //step1: erase region
    wm_log_info("\n\n********start write without erase example*********");
    ret = wm_drv_flash_erase_region(dev, TEST_FLASH_BEGING_ADDR, TEST_LEN);

    //step2: erase before write, otherwise the write value maybe incorrect
    if (ret == WM_ERR_SUCCESS) {
        ret = wm_drv_flash_write(dev, TEST_FLASH_BEGING_ADDR, write_buf, TEST_LEN);
    }

    //step3:  read flash and compare with write value
    if (ret == WM_ERR_SUCCESS) {
        ret = wm_drv_flash_read(dev, TEST_FLASH_BEGING_ADDR, read_buf, TEST_LEN);
    }

    if (ret == WM_ERR_SUCCESS) {
        wm_log_info("write data is: %s", write_buf);
        wm_log_info("read data is: %s", read_buf);
        ret = strcmp((const char *)write_buf, (const char *)read_buf);
    }

    if (ret == WM_ERR_SUCCESS) {
        wm_log_info("without erase example pass");
    } else {
        wm_log_info("without erase example failed");
        return ret;
    }

    //case2: call write with erase API step
    wm_log_info("\n\n*************start write with erase example*************");
    for (int i = 0; i < TEST_LEN; i++) {
        write_buf[i] = 'z' - i;
    }
    //step1: call write erase erase API
    if (ret == WM_ERR_SUCCESS) {
        ret = wm_drv_flash_write_with_erase(dev, TEST_FLASH_BEGING_ADDR, write_buf, TEST_LEN);
    }

    //step3:  read flash and compare with write value
    if (ret == WM_ERR_SUCCESS) {
        ret = wm_drv_flash_read(dev, TEST_FLASH_BEGING_ADDR, read_buf, TEST_LEN);
    }

    if (ret == WM_ERR_SUCCESS) {
        wm_log_info("write data is: %s", write_buf);
        wm_log_info("read data is: %s", read_buf);
        ret = strcmp((const char *)write_buf, (const char *)read_buf);
    }

    if (ret == WM_ERR_SUCCESS) {
        wm_log_info("with erase example pass");
    } else {
        wm_log_info("with erase example failed");
        return ret;
    }

    return ret;
}

int main(void)
{
    example_wr_rd_external_flash();

    while (1) {
        vTaskDelay(1000);
    }

    return 0;
}

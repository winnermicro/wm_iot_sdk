/**
 * @file main.c
 *
 * @brief sdh_sdmmc driver API read and write SD card demo main
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
#include "wm_task_config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "wm_drv_sdh_sdmmc.h"

#define LOG_TAG "example"
#include "wm_log.h"

#define WM_SDMMC_BLOCK_NUMS  (8)
#define WM_SDMMC_START_BLOCK (0)

static int sdh_sdmmc_single_block_read_write_demo(wm_device_t *dev)
{
    int ret                = WM_ERR_SUCCESS;
    uint8_t *sdmmc_in_buf  = NULL;
    uint8_t *sdmmc_out_buf = NULL;
    uint32_t sdmmc_buf_len = WM_SDH_BLOCK_SIZE;

    sdmmc_in_buf = malloc(sdmmc_buf_len);
    if (sdmmc_in_buf == NULL) {
        goto exit;
    }

    sdmmc_out_buf = malloc(sdmmc_buf_len);
    if (sdmmc_out_buf == NULL) {
        goto exit;
    }

    for (uint32_t i = 0; i < sdmmc_buf_len; i++) {
        sdmmc_in_buf[i] = i;
    }

    ret = wm_drv_sdh_sdmmc_write_blocks(dev, sdmmc_in_buf, WM_SDMMC_START_BLOCK, 1);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("Failed to write blocks from SDMMC device with error code: %d.", ret);
        goto exit;
    }

    memset(sdmmc_out_buf, 0, sdmmc_buf_len);
    ret = wm_drv_sdh_sdmmc_read_blocks(dev, sdmmc_out_buf, WM_SDMMC_START_BLOCK, 1);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("Failed to read blocks from SDMMC device with error code: %d.", ret);
        goto exit;
    }

    ret = memcmp(sdmmc_in_buf, sdmmc_out_buf, sdmmc_buf_len);
    if (ret != 0) {
        wm_log_error("Single block data read/write failure.");
        goto exit;
    } else {
        wm_log_info("Single block data read/write successful");
    }
    wm_log_dump(WM_LOG_LEVEL_INFO, "write block", 32, sdmmc_in_buf, sdmmc_buf_len);
    wm_log_dump(WM_LOG_LEVEL_INFO, "read block", 32, sdmmc_out_buf, sdmmc_buf_len);

exit:
    free(sdmmc_in_buf);
    free(sdmmc_out_buf);

    return ret;
}

static int sdh_sdmmc_multi_block_read_write_demo(wm_device_t *dev)
{
    int ret                = WM_ERR_SUCCESS;
    uint8_t *sdmmc_in_buf  = NULL;
    uint8_t *sdmmc_out_buf = NULL;
    uint32_t sdmmc_buf_len = WM_SDH_BLOCK_SIZE * WM_SDMMC_BLOCK_NUMS;

    sdmmc_in_buf = malloc(sdmmc_buf_len);
    if (sdmmc_in_buf == NULL) {
        goto exit;
    }

    sdmmc_out_buf = malloc(sdmmc_buf_len);
    if (sdmmc_out_buf == NULL) {
        goto exit;
    }

    for (uint32_t i = 0; i < sdmmc_buf_len; i++) {
        sdmmc_in_buf[i] = i;
    }

    ret = wm_drv_sdh_sdmmc_write_blocks(dev, sdmmc_in_buf, WM_SDMMC_START_BLOCK, WM_SDMMC_BLOCK_NUMS);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("Failed to write blocks from SDMMC device with error code: %d.", ret);
        goto exit;
    }

    memset(sdmmc_out_buf, 0, sdmmc_buf_len);
    ret = wm_drv_sdh_sdmmc_read_blocks(dev, sdmmc_out_buf, WM_SDMMC_START_BLOCK, WM_SDMMC_BLOCK_NUMS);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("Failed to read blocks from SDMMC device with error code: %d.", ret);
        goto exit;
    }

    ret = memcmp(sdmmc_in_buf, sdmmc_out_buf, sdmmc_buf_len);
    if (ret != 0) {
        wm_log_error("Multi block data read/write failure.");
        goto exit;
    } else {
        wm_log_info("Multi block data read/write successful");
    }
    wm_log_dump(WM_LOG_LEVEL_INFO, "write block", 32, sdmmc_in_buf, sdmmc_buf_len);
    wm_log_dump(WM_LOG_LEVEL_INFO, "read block", 32, sdmmc_out_buf, sdmmc_buf_len);

exit:
    free(sdmmc_in_buf);
    free(sdmmc_out_buf);

    return ret;
}

int main(void)
{
    int ret          = WM_ERR_SUCCESS;
    wm_device_t *dev = NULL;

    wm_log_info("SDMMC demo start.\n");

    /* Initialize sdmmc */
    wm_log_info("Initializing sdmmc device...");
    dev = wm_drv_sdh_sdmmc_init("sdmmc");

    /* Starting sdmmc demo */
    if (dev != NULL) {
        ret = sdh_sdmmc_single_block_read_write_demo(dev);
        if (ret == WM_ERR_SUCCESS) {
            ret = sdh_sdmmc_multi_block_read_write_demo(dev);
            if (ret == WM_ERR_SUCCESS) {
                wm_log_info("This example succeeds in running.");
            } else {
                wm_log_error("SDH sdmmc multi block read write failed: %d", ret);
            }
        } else {
            wm_log_error("SDH sdmmc single block read write failed: %d", ret);
        }

        ret = wm_drv_sdh_sdmmc_deinit(dev);
        if (ret != WM_ERR_SUCCESS) {
            wm_log_error("Failed to deinitialize sdmmc device with error code: %d", ret);
        }
    } else {
        wm_log_error("Failed to initialize sdmmc device");
    }

    while (1) {
        vTaskDelay(1000);
    }

    return 0;
}

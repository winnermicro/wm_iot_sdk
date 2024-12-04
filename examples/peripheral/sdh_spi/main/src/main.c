/**
 * @file main.c
 *
 * @brief sdh_spi driver API read and write SD card demo main
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
#include "freertos/semphr.h"
#include "wm_drv_sdh_spi.h"

#define LOG_TAG "example"
#include "wm_log.h"

#define WM_SPI_BUF_LEN            (1024)
#define WM_SPI_TRANSCEIVE_TIMEOUT (100)

static SemaphoreHandle_t sdspi_sem = NULL;

static int sdh_spi_transceive_sync_demo(wm_device_t *dev)
{
    int ret                = WM_ERR_SUCCESS;
    spim_transceive_t desc = { 0 };
    uint8_t *tx_buf        = NULL;
    wm_dt_hw_spim_dev_cfg_t config = {
        .freq = 40000000, //40M clock
        .mode = 0,
        .pin_cs = {
            .pin_num = WM_GPIO_NUM_27,
            .pin_mux = WM_GPIO_IOMUX_FUN5,
            .pin_dir = WM_GPIO_DIR_OUTPUT,
            .pin_pupd = WM_GPIO_PULL_UP,
        },
    };

    tx_buf = malloc(WM_SPI_BUF_LEN);
    if (tx_buf == NULL) {
        goto exit;
    }
    for (int i = 0; i < WM_SPI_BUF_LEN; i++) {
        tx_buf[i] = i;
    }

    desc.tx_buf = tx_buf;
    desc.tx_len = WM_SPI_BUF_LEN;

    ret = wm_drv_sdh_spi_transceive_sync(dev, &config, &desc, WM_SPI_TRANSCEIVE_TIMEOUT);
    if (ret != WM_ERR_SUCCESS) {
        goto exit;
    }
    wm_log_dump(WM_LOG_LEVEL_INFO, "sync tx data", 32, tx_buf, WM_SPI_BUF_LEN);

exit:
    free(tx_buf);

    return ret;
}

static void sdh_spi_transceive_async_callback(int result, void *data)
{
    if (result != WM_ERR_SUCCESS) {
        wm_log_error("SPI async transceive failed with result: %d", result);
    }
    if (sdspi_sem != NULL) {
        xSemaphoreGive(sdspi_sem);
    }
}

static int sdh_spi_transceive_async_demo(wm_device_t *dev)
{
    int ret                = WM_ERR_SUCCESS;
    spim_transceive_t desc = { 0 };
    uint8_t *tx_buf        = NULL;
    wm_dt_hw_spim_dev_cfg_t config = {
        .freq = 40000000, //40M clock
        .mode = 0,
        .pin_cs = {
            .pin_num = WM_GPIO_NUM_27,
            .pin_mux = WM_GPIO_IOMUX_FUN5,
            .pin_dir = WM_GPIO_DIR_OUTPUT,
            .pin_pupd = WM_GPIO_PULL_UP,
        },
    };

    sdspi_sem = xSemaphoreCreateCounting(configSEMAPHORE_MAX_VALUE, 0);

    tx_buf = malloc(WM_SPI_BUF_LEN);
    if (tx_buf == NULL) {
        goto exit;
    }
    for (int i = 0; i < WM_SPI_BUF_LEN; i++) {
        tx_buf[i] = i;
    }

    desc.tx_buf = tx_buf;
    desc.tx_len = WM_SPI_BUF_LEN;

    /** The data transmitted through this interface must be aligned with 4 bytes */
    ret = wm_drv_sdh_spi_transceive_async(dev, &config, &desc, sdh_spi_transceive_async_callback, NULL);
    if (ret != WM_ERR_SUCCESS) {
        goto exit;
    }

    if (xSemaphoreTake(sdspi_sem, HZ * 5) != pdTRUE) {
        wm_log_error("SPI async transceive failed, tiemout");
        goto exit;
    }
    wm_log_dump(WM_LOG_LEVEL_INFO, "async tx data", 32, tx_buf, WM_SPI_BUF_LEN);

exit:
    free(tx_buf);
    vSemaphoreDelete(sdspi_sem);
    sdspi_sem = NULL;

    return ret;
}

int main(void)
{
    int ret          = WM_ERR_SUCCESS;
    wm_device_t *dev = NULL;

    wm_log_info("SPI demo start.");

    /* Initialize sdspi */
    wm_log_info("Initializing sdspi device...");
    dev = wm_drv_sdh_spi_init("sdspi");

    /* Starting sdspi demo */
    if (dev != NULL) {
        ret = sdh_spi_transceive_sync_demo(dev);
        if (ret == WM_ERR_SUCCESS) {
            wm_log_info("SPI sync transmission successful");
            ret = sdh_spi_transceive_async_demo(dev);
            if (ret == WM_ERR_SUCCESS) {
                wm_log_info("SPI async transmission successful");
                wm_log_info("This example succeeds in running.");
            } else {
                wm_log_error("SPI async transmission failed: %d", ret);
            }
        } else {
            wm_log_error("SPI sync transmission failed: %d", ret);
        }
    } else {
        wm_log_error("Failed to initialize sdspi device");
    }

    ret = wm_drv_sdh_spi_deinit(dev);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("Failed to deinitialize sdspi device with error code: %d", ret);
    }

    while (1) {
        vTaskDelay(1000);
    }

    return 0;
}

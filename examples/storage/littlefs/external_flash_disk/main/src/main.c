/**
 * @file main.c
 *
 * @brief LittleFS external flash disk demo for basic file operations
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
#include <string.h>
#include "wmsdk_config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wm_lfs_flash.h"
#include "wm_log.h"
#include "wm_error.h"

#define TEST_FILENAME "hello.txt"
#define TEST_MESSAGE  "Hello LittleFS!"

static int littlefs_demo(void)
{
    struct lfs_config *cfg;
    lfs_t lfs;
    lfs_file_t file;
    uint8_t buffer[64];
    int ret = 0;

    // Initialize LittleFS with external flash
    cfg = lfs_flash_init(WM_LFS_FLASH_TYPE_EXTERNAL);
    if (cfg == NULL) {
        wm_log_error("Failed to initialize LittleFS configuration.");
        return WM_ERR_NO_MEM;
    }

    // Format the filesystem
    ret = lfs_format(&lfs, cfg);
    if (ret != 0) {
        wm_log_error("Failed to format filesystem.");
        ret = WM_ERR_FAILED;
        goto cleanup;
    }

    // Mount the filesystem
    ret = lfs_mount(&lfs, cfg);
    if (ret != 0) {
        wm_log_error("Failed to mount filesystem.");
        ret = WM_ERR_FAILED;
        goto cleanup;
    }

    // Write operation
    wm_log_info("Writing to file: %s", TEST_FILENAME);
    ret = lfs_file_open(&lfs, &file, TEST_FILENAME, LFS_O_WRONLY | LFS_O_CREAT);
    if (ret != 0) {
        wm_log_error("Failed to open file for writing.");
        ret = WM_ERR_FAILED;
        goto unmount;
    }

    ret = lfs_file_write(&lfs, &file, TEST_MESSAGE, strlen(TEST_MESSAGE));
    if (ret < 0) {
        wm_log_error("Failed to write to file.");
        lfs_file_close(&lfs, &file);
        ret = WM_ERR_FAILED;
        goto unmount;
    }
    lfs_file_close(&lfs, &file);

    // Read operation
    wm_log_info("Reading from file: %s", TEST_FILENAME);
    ret = lfs_file_open(&lfs, &file, TEST_FILENAME, LFS_O_RDONLY);
    if (ret != 0) {
        wm_log_error("Failed to open file for reading.");
        ret = WM_ERR_FAILED;
        goto unmount;
    }

    memset(buffer, 0, sizeof(buffer));
    ret = lfs_file_read(&lfs, &file, buffer, sizeof(buffer));
    if (ret < 0) {
        wm_log_error("Failed to read from file.");
        lfs_file_close(&lfs, &file);
        ret = WM_ERR_FAILED;
        goto unmount;
    }
    lfs_file_close(&lfs, &file);

    wm_log_info("Read content: %s", buffer);
    wm_log_info("File operations completed successfully.");
    ret = WM_ERR_SUCCESS;

unmount:
    // Unmount the filesystem
    if (lfs_unmount(&lfs) != 0) {
        wm_log_error("Failed to unmount filesystem.");
        ret = WM_ERR_FAILED;
    }

cleanup:
    // Deinitialize LittleFS
    if (lfs_flash_deinit(cfg) != 0) {
        wm_log_error("Failed to deinitialize LittleFS.");
        ret = WM_ERR_FAILED;
    }

    return ret;
}

int main(void)
{
    wm_log_info("LittleFS demo start.");

    int ret = littlefs_demo();
    if (ret == WM_ERR_SUCCESS) {
        wm_log_info("This example succeeds in running.");
    } else {
        wm_log_error("This example failed with error code: %d", ret);
    }

    return ret;
}

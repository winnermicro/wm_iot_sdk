/**
 * @file main.c
 *
 * @brief FatFs external flash disk demo for reading and writing files
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
#include "freertos/queue.h"
#include "wm_diskio.h"
#include "ff.h"

#define LOG_TAG "example"
#include "wm_log.h"

#define FATFS_FORMAT_WORK_BUFFER_SIZE (4096) /**< Size of the work buffer used for formatting the file system */

static FRESULT format_filesystem(const char *path)
{
    FRESULT res;
    MKFS_PARM opt;
    void *work;

    UINT len    = FATFS_FORMAT_WORK_BUFFER_SIZE; // Size of work buffer
    opt.fmt     = FM_ANY;                        // Format type, FM_ANY means auto-select
    opt.n_fat   = 1;                             // Number of FAT tables
    opt.align   = 0;                             // Data area alignment, 0 means no alignment
    opt.n_root  = 0;                             // Number of root directory entries for FAT12/16, 0 means use maximum
    opt.au_size = 0;                             // Cluster size, 0 means use default size

    work = malloc(len);
    if (work == NULL) {
        wm_log_error("Failed to allocate work buffer.\n");
        return FR_INT_ERR;
    }

    res = f_mkfs(path, &opt, work, len);
    free(work);
    work = NULL;

    if (res != FR_OK) {
        wm_log_error("File system formatting failed with error: %d", res);
    }

    return res;
}

static FRESULT mount_filesystem(FATFS *fs, char *path)
{
    FRESULT res = f_mount(fs, path, 1);
    if (res == FR_NO_FILESYSTEM) {
        wm_log_info("File system not found. Formatting...");
        res = format_filesystem(path);
        if (res == FR_OK) {
            res = f_mount(fs, path, 1);
        }
    }
    if (res != FR_OK) {
        wm_log_error("Failed to mount file system.");
    } else {
        wm_log_info("File system mount success, fs_type: %d", fs->fs_type);
    }

    return res;
}

static FRESULT write_to_file(FIL *fil, const char *full_path, const char *write_buf)
{
    FRESULT res;
    UINT bw;

    res = f_open(fil, full_path, FA_CREATE_ALWAYS | FA_WRITE);
    if (res != FR_OK) {
        wm_log_error("Failed to open file for writing: %d", res);
        goto exit;
    }

    res = f_write(fil, write_buf, strlen(write_buf), &bw);
    if (res != FR_OK) {
        wm_log_error("Failed to write to file: %d", res);
    }

exit:
    f_close(fil);

    return res;
}

static FRESULT read_from_file(FIL *fil, const char *full_path, char *read_buf, UINT buf_size)
{
    FRESULT res;
    UINT br;

    res = f_open(fil, full_path, FA_READ);
    if (res != FR_OK) {
        wm_log_error("Failed to open file for reading: %d", res);
        goto exit;
    }

    res = f_read(fil, read_buf, buf_size, &br);
    if (res != FR_OK) {
        wm_log_error("Failed to read from file: %d", res);
    }

exit:
    f_close(fil);

    return res;
}

static FRESULT unmount_filesystem(FATFS *fs, const char *path)
{
    FRESULT res = f_mount(fs, path, 0);
    if (res != FR_OK) {
        wm_log_error("Failed to unmount file system.");
    }

    return res;
}

static int fatfs_write_read_demo(void)
{
    FRESULT res;
    FATFS *fs          = NULL;
    FIL fil            = { 0 };
    char write_buf[16] = "Hello, FatFs!";
    char read_buf[16]  = "";
    char path[8]       = { 0 };
    char full_path[32] = { 0 };

    /* Allocate memory for FATFS structure that needs to be valid for the entire filesystem lifetime */
    fs = (FATFS *)malloc(sizeof(FATFS));
    memset(fs, 0, sizeof(FATFS));

    /**
     * @note If CONFIG_FATFS_FF_STR_VOLUME_ID_ENABLE=y is configured in menuconfig,
     *       use the following volume ID format to support Windows-like drive letters (C:/D:/E:):
     *       sprintf(path, "%s:", CONFIG_FATFS_EXTERNAL_FLASH_DISK_FF_VOLUME_STRS);
     */
    sprintf(path, "%d:", WM_DISKIO_DRIVER_NUM_EXTERNAL_FLASH);
    res = mount_filesystem(fs, path);
    if (res != FR_OK) {
        goto exit1;
    }

    sprintf(full_path, "%s%s", path, "test.txt");
    res = write_to_file(&fil, full_path, write_buf);
    if (res != FR_OK) {
        goto exit2;
    }

    res = read_from_file(&fil, full_path, read_buf, sizeof(read_buf));
    if (res != FR_OK) {
        goto exit2;
    }

    if (strcmp(write_buf, read_buf)) {
        wm_log_error("Data mismatch between write and read buffers.");
    }
    wm_log_info("write txt: %s", write_buf);
    wm_log_info("read txt: %s", read_buf);

exit2:
    res = unmount_filesystem(fs, path);
    if (res != FR_OK) {
        wm_log_error("Failed to unmount file system.");
    }
exit1:
    free(fs);
    fs = NULL;

    return res;
}

int main(void)
{
    int ret = 0;
    wm_log_info("FATFS demo start.");

    ret = fatfs_write_read_demo();
    if (ret == RES_OK) {
        wm_log_info("This example succeeds in running.");
    } else {
        wm_log_error("File read and write demo failed.");
    }

    return 0;
}

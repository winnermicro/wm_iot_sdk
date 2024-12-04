/**
 * @file app_fs.c
 *
 * @brief Application FileSystem 
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
#include "wm_error.h"
#include "wm_utils.h"
#include "wm_diskio.h"

#define LOG_TAG "app_fs"
#include "wm_log.h"

#if CONFIG_COMPONENT_FATFS_ENABLED

/* filesystem object */
static FATFS app_fs = { 0 };

int app_check_logo_in_fs(const char *logo_path)
{
    FRESULT res = FR_DISK_ERR;
    FILINFO fno = { 0 };

    res = f_mount(&app_fs, logo_path, 1);
    if (res != FR_OK) {
        wm_log_error("failed(%d) to mount fs: %s", res, logo_path);
        return WM_ERR_FAILED;
    }

    res = f_stat(logo_path, &fno);
    if (res != FR_OK) {
        wm_log_error("failed(%d) to get file info: %s", res, logo_path);
        return WM_ERR_FAILED;
    } else {
        wm_log_info("found file in the file system, size: %u bytes", fno.fsize);
    }

    return WM_ERR_SUCCESS;
}

#endif

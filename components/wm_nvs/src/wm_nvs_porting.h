/**
 * @file wm_nvs_port.h
 *
 * @brief nvs port
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

#ifndef __WM_NVS_PORTING_H__
#define __WM_NVS_PORTING_H__

#include <stdint.h>

#include "wmsdk_config.h"
#include "wm_types.h"
#include "wm_error.h"
#include "wm_utils.h"
#include "wm_partition_table.h"
#include "wm_list.h"
#include "wm_log.h"
#include "wm_osal.h"

#ifdef __cplusplus
extern "C" {
#endif
#define WM_NVS_DEBUG               1

/* partition name size*/
#define WM_NVS_PARTITION_NAME_SIZE WM_PARTITION_NAME_SIZE

#if WM_NVS_DEBUG == 2
#define WM_NVS_LOGE(...) wm_log_error(__VA_ARGS__)
#define WM_NVS_LOGW(...) wm_log_warn(__VA_ARGS__)
#define WM_NVS_LOGI(...) wm_log_info(__VA_ARGS__)
#define WM_NVS_LOGD(...) wm_log_debug(__VA_ARGS__)
#define WM_NVS_LOGV(...) ((void)0)
#elif WM_NVS_DEBUG == 1
#define WM_NVS_LOGE(...) wm_log_error(__VA_ARGS__)
#define WM_NVS_LOGW(...) ((void)0)
#define WM_NVS_LOGI(...) ((void)0)
#define WM_NVS_LOGD(...) ((void)0)
#define WM_NVS_LOGV(...) ((void)0)
#else
#define WM_NVS_LOGE(...) ((void)0)
#define WM_NVS_LOGW(...) ((void)0)
#define WM_NVS_LOGI(...) ((void)0)
#define WM_NVS_LOGD(...) ((void)0)
#define WM_NVS_LOGV(...) ((void)0)
#endif

#define WM_NVS_RAWE(...)              wm_log_raw(WM_LOG_LEVEL_ERROR, __VA_ARGS__)
#define WM_NVS_DUMPE(name, buf, size) wm_log_dump(WM_LOG_LEVEL_ERROR, name, 16, buf, size)

#define WM_NVS_MALLOC(size)           wm_os_internal_malloc(size)
#define WM_NVS_REALLOC(ptr, size)     wm_os_internal_realloc(ptr, size)
#define WM_NVS_CALLOC(nelem, elsize)  wm_os_internal_calloc(nelem, elsize)
#define WM_NVS_FREE(ptr)              wm_os_internal_free(ptr)

int wm_nvs_port_init(void);
int wm_nvs_port_deinit(void);
int wm_nvs_port_is_init(void);
int wm_nvs_port_mutex_lock(void);
int wm_nvs_port_mutex_unlock(void);

uint32_t wm_nvs_port_crc32(uint32_t crc, const void *buf, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif

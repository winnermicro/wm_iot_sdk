
/**
 * @file wm_nvs.c
 *
 * @brief nvs module
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

#include <string.h>
#include <stdlib.h>

#include "wm_types.h"
#include "wm_error.h"
#include "wmsdk_config.h"
#include "wm_utils.h"
#include "wm_list.h"

#define LOG_TAG "nvs_ptm"
#include "wm_nvs_porting.h"
#include "wm_nvs.h"
#include "wm_nvs_partition_manager.h"
#include "wm_nvs_handler.h"

/**
 * @brief   storage list
 */
static struct dl_list g_storage_list = DL_LIST_HEAD_INIT(g_storage_list);

/**
 * @brief   handle list
 */
static struct dl_list g_handle_list = DL_LIST_HEAD_INIT(g_handle_list);

int wm_nvs_ptm_is_in_using(void)
{
    return !dl_list_empty(&g_storage_list);
}

struct dl_list *wm_nvs_get_storage_list(void)
{
    return &g_storage_list;
}

struct dl_list *wm_nvs_get_handle_list(void)
{
    return &g_handle_list;
}

wm_nvs_storage_t *wm_nvs_ptm_find_storage(const char *partition_name)
{
    wm_nvs_storage_t *entry = NULL;

    /*look up storage from storage list*/
    dl_list_for_each(entry, &g_storage_list, wm_nvs_storage_t, list)
    {
        if (!strcmp(entry->pt.name, partition_name)) {
            return entry;
        }
    }
    return NULL;
}

int wm_nvs_ptm_init(const char *partition_name)
{
    int err;
    wm_nvs_storage_t *storage = NULL;
    wm_nvs_partition_t pt;

    /*check init before*/
    storage = wm_nvs_ptm_find_storage(partition_name);
    if (storage) {
        WM_NVS_LOGW("Aready init");
        return WM_NVS_ERR_AREADY_INIT;
    }

    /*init partition*/
    err = wm_nvs_pt_init(partition_name, &pt);
    if (err != WM_NVS_ERR_OK) {
        return err;
    }

    /*init storage*/
    err = wm_nvs_storage_init(&pt, &storage);
    if (err == WM_NVS_ERR_OK) {
        dl_list_add_tail(&g_storage_list, &storage->list);
    }

    WM_NVS_LOGI("ptm init, err=%d", err);

    return err;
}

int wm_nvs_ptm_deinit(const char *partition_name)
{
    int err;

    wm_nvs_storage_t *storage   = NULL;
    wm_nvs_handle_info_t *entry = NULL;
    wm_nvs_handle_info_t *next;

    if (!g_storage_list.next) {
        WM_NVS_LOGE("Not Init");
        return WM_NVS_ERR_FAIL;
    }

    /*remove storage from list*/
    storage = wm_nvs_ptm_find_storage(partition_name);
    if (!storage) {
        WM_NVS_LOGE("Not found");
        return WM_NVS_ERR_NOT_FOUND;
    }

    /*delete all handle*/
    dl_list_for_each_safe(entry, next, &g_handle_list, wm_nvs_handle_info_t, list)
    {
        if (entry->storage == storage) {
            WM_NVS_LOGD("Destroy handle: %p, group=%d", entry, entry->group_id);

            dl_list_del(&entry->list);
            wm_nvs_handler_close(entry);
        }
    }

    /*deteach from list*/
    dl_list_del(&storage->list);

    /* deinit storage*/
    err = wm_nvs_storage_deinit(storage);

    /*free storage*/
    WM_NVS_FREE(storage);

    WM_NVS_LOGI("ptm deinit, err=%d", err);

    return err;
}

int wm_nvs_ptm_open_handle(const char *partition_name, const char *group, wm_nvs_open_mode_t mode,
                           wm_nvs_handle_info_t **handle)
{
    int err;
    wm_nvs_storage_t *storage = NULL;
    uint8_t group_id;

    /*check init*/
    if (dl_list_empty(&g_storage_list)) {
        WM_NVS_LOGE("not init");
        return WM_NVS_ERR_NOT_INIT;
    }

    if (dl_list_len(&g_handle_list) >= WM_NVS_MAX_HANDLE_NUM) {
        return WM_NVS_ERR_FAIL;
    }

    /* find partition*/
    storage = wm_nvs_ptm_find_storage(partition_name);
    if (!storage) {
        WM_NVS_LOGW("%s not init", partition_name);
        return WM_NVS_ERR_NOT_FOUND;
    }

    /* check write allowed*/
    if (mode == WM_NVS_OP_READ_WRITE && storage->pt.readonly) {
        WM_NVS_LOGW("%s can't be written", partition_name);
        return WM_NVS_ERR_READ_ONLY;
    }

    /* get group id*/
    err = wm_nvs_storage_open_group(storage, group, true, &group_id);
    if (err != WM_NVS_ERR_OK) {
        return err;
    }

    /* create handler*/
    err = wm_nvs_handler_open(storage, group_id, mode, handle);
    if (err != WM_NVS_ERR_OK) {
        return err;
    }

    /* add to handler list */
    dl_list_add_tail(&g_handle_list, &(*handle)->list);

    WM_NVS_LOGD("open %p", handle);

    return WM_NVS_ERR_OK;
}

int wm_nvs_ptm_close_handle(wm_nvs_handle_info_t *handle)
{
    wm_nvs_handle_info_t *entry = NULL;
    wm_nvs_handle_info_t *next;

    WM_NVS_LOGD("close %p", handle);

    /*look up storage from storage list*/
    dl_list_for_each_safe(entry, next, &g_handle_list, wm_nvs_handle_info_t, list)
    {
        if (entry == handle) {
            dl_list_del(&entry->list);
            return wm_nvs_handler_close(handle);
        }
    }
    return WM_NVS_ERR_INVALID_HANDLE;
}

bool wm_nvs_ptm_is_handle_valid(wm_nvs_handle_info_t *handle)
{
    bool found = false;

    wm_nvs_handle_info_t *entry = NULL;

    /*look up storage from storage list*/
    dl_list_for_each(entry, &g_handle_list, wm_nvs_handle_info_t, list)
    {
        if (entry == handle) {
            found = (entry->valid ? true : false);
            break;
        }
    }

    return found;
}

int wm_nvs_ptm_get_status(const char *partition_name, wm_nvs_status_t *status)
{
    wm_nvs_handle_info_t *entry = NULL;
    wm_nvs_storage_t *storage   = wm_nvs_ptm_find_storage(partition_name);

    if (!storage) {
        return WM_NVS_ERR_NOT_INIT;
    }

    memset(status, 0, sizeof(*status));

    status->group_num = dl_list_len(&storage->group_list);

    /*look up handler list*/
    dl_list_for_each(entry, &g_handle_list, wm_nvs_handle_info_t, list)
    {
        if (entry->storage == storage) {
            status->handle_num++;
        }
    }

    return wm_nvs_sm_get_status(&storage->sm, status);
}

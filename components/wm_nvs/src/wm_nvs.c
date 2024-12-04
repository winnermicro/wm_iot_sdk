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

#define LOG_TAG "nvs_api"
#include "wm_nvs_porting.h"
#include "wm_nvs.h"
#include "wm_nvs_partition_manager.h"

#define WM_NVS_LOCK()                      \
    do {                                   \
        int _e = wm_nvs_port_mutex_lock(); \
        if (_e != WM_ERR_SUCCESS) {        \
            return _e;                     \
        }                                  \
    } while (0)

#define WM_NVS_UNLOCK() wm_nvs_port_mutex_unlock()

int wm_nvs_init(const char *partition_name)
{
    int err;

    if (!partition_name) {
        return WM_NVS_ERR_INVALID_PARAM;
    }

    WM_NVS_LOGD("init %s", partition_name);

    /*init port at first partition initializing*/
    err = wm_nvs_port_init();
    if (err != WM_NVS_ERR_OK) {
        return err;
    }

    WM_NVS_LOCK();

    /*init partition*/
    err = wm_nvs_ptm_init(partition_name);

    WM_NVS_UNLOCK();

    WM_NVS_LOGD("init end,err=%d", err);

    return err;
}

int wm_nvs_deinit(const char *partition_name)
{
    int err;

    if (!partition_name) {
        return WM_NVS_ERR_INVALID_PARAM;
    }

    /*init port at first partition initializing*/
    if (!wm_nvs_ptm_is_in_using() && (err = wm_nvs_port_init()) != WM_NVS_ERR_OK) {
        return err;
    }
    WM_NVS_LOCK();

    err = wm_nvs_ptm_deinit(partition_name);

    WM_NVS_UNLOCK();

    /*deinit port if no partiton in using*/
    if (!wm_nvs_ptm_is_in_using()) {
        wm_nvs_port_deinit();
    }

    return err;
}

int wm_nvs_erase(const char *partition_name)
{
    wm_nvs_storage_t *storage = NULL;

    if (!partition_name) {
        return WM_NVS_ERR_INVALID_PARAM;
    }

    if (wm_nvs_ptm_is_in_using()) {
        WM_NVS_LOCK();

        storage = wm_nvs_ptm_find_storage(partition_name);

        WM_NVS_UNLOCK();

        if (storage) {
            /*Do not erase the using partition*/
            WM_NVS_LOGE("partition busy,not erase");
            return WM_NVS_ERR_FAIL;
        }
    }

    return wm_nvs_pt_erase_all(partition_name);
}

int wm_nvs_open(const char *partition_name, const char *group_name, wm_nvs_open_mode_t mode, wm_nvs_handle_t *handle)
{
    int err = WM_NVS_ERR_FAIL;

    if (!(group_name && handle && mode >= 0 && mode < WM_NVS_OP_MAX)) {
        return WM_NVS_ERR_INVALID_PARAM;
    }

    if (!partition_name) {
        /* use default nvs partition*/
        partition_name = WM_NVS_DEF_PARTITION;
    }

    if (strlen(group_name) > WM_NVS_MAX_KEY_LEN) {
        return WM_NVS_ERR_INVALID_PARAM;
    }

    WM_NVS_LOCK();

    err = wm_nvs_ptm_open_handle(partition_name, group_name, mode, (wm_nvs_handle_info_t **)handle);

    WM_NVS_UNLOCK();

    return err;
}

int wm_nvs_close(wm_nvs_handle_t handle)
{
    int err = WM_NVS_ERR_FAIL;

    if (!handle) {
        return WM_NVS_ERR_INVALID_PARAM;
    }

    WM_NVS_LOCK();

    if (wm_nvs_ptm_is_handle_valid(handle)) {
        err = wm_nvs_ptm_close_handle(handle);
    } else {
        err = WM_NVS_ERR_INVALID_HANDLE;
    }

    WM_NVS_UNLOCK();

    return err;
}

static int get_item(wm_nvs_handle_t *handle, wm_nvs_type_t type, const char *key, void *out_value, size_t *length)
{
    int err;
    wm_nvs_handle_info_t *h = (wm_nvs_handle_info_t *)handle;

    if (!(handle && key && out_value && length && *length > 0 && type > WM_NVS_TYPE_ANY && type < WM_NVS_TYPE_MAX)) {
        return WM_NVS_ERR_INVALID_PARAM;
    }

    WM_NVS_LOCK();

    if (wm_nvs_ptm_is_handle_valid(h)) {
        err = wm_nvs_storage_read_item(h->storage, h->group_id, type, key, out_value, length);
    } else {
        err = WM_NVS_ERR_INVALID_HANDLE;
    }

    WM_NVS_UNLOCK();

    WM_NVS_LOGD("get item %s, size=%u, err=%d", key, (err == WM_NVS_ERR_OK ? *length : 0), err);

    return err;
}

static int set_item(wm_nvs_handle_t *handle, wm_nvs_type_t type, const char *key, const void *value, uint32_t length)
{
    int err;
    wm_nvs_handle_info_t *h = (wm_nvs_handle_info_t *)handle;
    int key_len;

    if (!(handle && key && value && length >= 0 && type > WM_NVS_TYPE_ANY && type < WM_NVS_TYPE_MAX)) {
        return WM_NVS_ERR_INVALID_PARAM;
    }

    key_len = strlen(key);
    if (!(key_len > 0 && key_len <= WM_NVS_MAX_KEY_LEN)) {
        return WM_NVS_ERR_INVALID_PARAM;
    }

    WM_NVS_LOCK();

    if (!wm_nvs_ptm_is_handle_valid(h)) {
        err = WM_NVS_ERR_INVALID_HANDLE;
    } else if (h->mode == WM_NVS_OP_READ_ONLY) {
        err = WM_NVS_ERR_READ_ONLY;
    } else {
        err = wm_nvs_storage_write_item(h->storage, h->group_id, type, key, value, length);
    }

    WM_NVS_UNLOCK();

    WM_NVS_LOGD("write %s, size=%u, err=%d", key, length, err);

    return err;
}

int wm_nvs_get_str(wm_nvs_handle_t handle, const char *key, char *out_value, size_t *length)
{
    return get_item(handle, WM_NVS_TYPE_STRING, key, out_value, length);
}

int wm_nvs_set_str(wm_nvs_handle_t handle, const char *key, const char *value)
{
    if (!value) {
        return WM_NVS_ERR_INVALID_PARAM;
    }

    return set_item(handle, WM_NVS_TYPE_STRING, key, value, strlen(value) + 1);
}

int wm_nvs_get_binary(wm_nvs_handle_t handle, const char *key, void *data, size_t *data_len)
{
    return get_item(handle, WM_NVS_TYPE_BINARY, key, data, data_len);
}

int wm_nvs_set_binary(wm_nvs_handle_t handle, const char *key, const void *data, size_t data_len)
{
    return set_item(handle, WM_NVS_TYPE_BINARY, key, data, data_len);
}

int wm_nvs_get_blob(wm_nvs_handle_t handle, const char *key, void *blob, size_t *blob_len)
{
    return get_item(handle, WM_NVS_TYPE_BLOB, key, blob, blob_len);
}

int wm_nvs_set_blob(wm_nvs_handle_t handle, const char *key, const void *blob, size_t blob_len)
{
    return set_item(handle, WM_NVS_TYPE_BLOB, key, blob, blob_len);
}

int wm_nvs_set_i8(wm_nvs_handle_t handle, const char *key, int8_t value)
{
    return set_item(handle, WM_NVS_TYPE_INT8, key, &value, sizeof(value));
}

int wm_nvs_set_u8(wm_nvs_handle_t handle, const char *key, uint8_t value)
{
    return set_item(handle, WM_NVS_TYPE_UINT8, key, &value, sizeof(value));
}

int wm_nvs_set_i16(wm_nvs_handle_t handle, const char *key, int16_t value)
{
    return set_item(handle, WM_NVS_TYPE_INT16, key, &value, sizeof(value));
}

int wm_nvs_set_u16(wm_nvs_handle_t handle, const char *key, uint16_t value)
{
    return set_item(handle, WM_NVS_TYPE_UINT16, key, &value, sizeof(value));
}

int wm_nvs_set_i32(wm_nvs_handle_t handle, const char *key, int32_t value)
{
    return set_item(handle, WM_NVS_TYPE_INT32, key, &value, sizeof(value));
}

int wm_nvs_set_u32(wm_nvs_handle_t handle, const char *key, uint32_t value)
{
    return set_item(handle, WM_NVS_TYPE_UINT32, key, &value, sizeof(value));
}

int wm_nvs_set_i64(wm_nvs_handle_t handle, const char *key, int64_t value)
{
    return set_item(handle, WM_NVS_TYPE_INT64, key, &value, sizeof(value));
}

int wm_nvs_set_u64(wm_nvs_handle_t handle, const char *key, uint64_t value)
{
    return set_item(handle, WM_NVS_TYPE_UINT64, key, &value, sizeof(value));
}

int wm_nvs_set_float(wm_nvs_handle_t handle, const char *key, double value)
{
    return set_item(handle, WM_NVS_TYPE_DOUBLE, key, &value, sizeof(value));
}

int wm_nvs_get_i8(wm_nvs_handle_t handle, const char *key, int8_t *out_value)
{
    size_t length = sizeof(*out_value);
    return get_item(handle, WM_NVS_TYPE_INT8, key, out_value, &length);
}

int wm_nvs_get_u8(wm_nvs_handle_t handle, const char *key, uint8_t *out_value)
{
    size_t length = sizeof(*out_value);
    return get_item(handle, WM_NVS_TYPE_UINT8, key, out_value, &length);
}

int wm_nvs_get_i16(wm_nvs_handle_t handle, const char *key, int16_t *out_value)
{
    size_t length = sizeof(*out_value);
    return get_item(handle, WM_NVS_TYPE_INT16, key, out_value, &length);
}

int wm_nvs_get_u16(wm_nvs_handle_t handle, const char *key, uint16_t *out_value)
{
    size_t length = sizeof(*out_value);
    return get_item(handle, WM_NVS_TYPE_UINT16, key, out_value, &length);
}

int wm_nvs_get_i32(wm_nvs_handle_t handle, const char *key, int32_t *out_value)
{
    size_t length = sizeof(*out_value);
    return get_item(handle, WM_NVS_TYPE_INT32, key, out_value, &length);
}

int wm_nvs_get_u32(wm_nvs_handle_t handle, const char *key, uint32_t *out_value)
{
    size_t length = sizeof(*out_value);
    return get_item(handle, WM_NVS_TYPE_UINT32, key, out_value, &length);
}

int wm_nvs_get_i64(wm_nvs_handle_t handle, const char *key, int64_t *out_value)
{
    size_t length = sizeof(*out_value);
    return get_item(handle, WM_NVS_TYPE_INT64, key, out_value, &length);
}

int wm_nvs_get_u64(wm_nvs_handle_t handle, const char *key, uint64_t *out_value)
{
    size_t length = sizeof(*out_value);
    return get_item(handle, WM_NVS_TYPE_UINT64, key, out_value, &length);
}

int wm_nvs_get_float(wm_nvs_handle_t handle, const char *key, double *out_value)
{
    size_t length = sizeof(*out_value);
    return get_item(handle, WM_NVS_TYPE_DOUBLE, key, out_value, &length);
}

int wm_nvs_del_key(wm_nvs_handle_t handle, const char *key)
{
    int err;
    wm_nvs_handle_info_t *h = (wm_nvs_handle_info_t *)handle;

    if (!(handle && key)) {
        return WM_NVS_ERR_INVALID_PARAM;
    }

    WM_NVS_LOCK();

    if (!wm_nvs_ptm_is_handle_valid(h)) {
        err = WM_NVS_ERR_INVALID_HANDLE;
    } else if (h->mode == WM_NVS_OP_READ_ONLY) {
        err = WM_NVS_ERR_READ_ONLY;
    } else {
        err = wm_nvs_storage_del_item(h->storage, h->group_id, WM_NVS_TYPE_ANY_WITHOUT_SEG, key);
    }

    WM_NVS_UNLOCK();

    WM_NVS_LOGD("del %s,err=%d", key, err);

    return err;
}

int wm_nvs_del_group(wm_nvs_handle_t handle)
{
    int err;
    wm_nvs_handle_info_t *h = (wm_nvs_handle_info_t *)handle;

    if (!(handle && h->valid)) {
        return WM_NVS_ERR_INVALID_PARAM;
    }

    WM_NVS_LOCK();

    if (!wm_nvs_ptm_is_handle_valid(h)) {
        err = WM_NVS_ERR_INVALID_HANDLE;
    } else if (h->mode == WM_NVS_OP_READ_ONLY) {
        err = WM_NVS_ERR_READ_ONLY;
    } else {
        err = wm_nvs_storage_del_group(h->storage, h->group_id);
    }

    WM_NVS_UNLOCK();

    WM_NVS_LOGD("del all group, id=%d", h->group_id);

    return err;
}

int wm_nvs_entry_find(const char *partition_name, const char *group, wm_nvs_type_t type, wm_nvs_iterator_t *output_iterator)
{
    int err;

    if (!(partition_name && group && type >= WM_NVS_TYPE_ANY && type < WM_NVS_TYPE_MAX && output_iterator)) {
        return WM_NVS_ERR_INVALID_PARAM;
    }

    WM_NVS_LOCK();

    if (type == WM_NVS_TYPE_ANY) {
        type = WM_NVS_TYPE_ANY_WITHOUT_SEG;
    }

    err = wm_nvs_iterator_find(partition_name, group, type, output_iterator);

    WM_NVS_UNLOCK();

    return err;
}

int wm_nvs_entry_find_by_handle(wm_nvs_handle_t handle, wm_nvs_type_t type, wm_nvs_iterator_t *output_iterator)
{
    int err;

    if (!(handle && type >= WM_NVS_TYPE_ANY && type < WM_NVS_TYPE_MAX && output_iterator)) {
        return WM_NVS_ERR_INVALID_PARAM;
    }

    WM_NVS_LOCK();

    if (wm_nvs_ptm_is_handle_valid((wm_nvs_handle_info_t *)handle)) {
        if (type == WM_NVS_TYPE_ANY) {
            type = WM_NVS_TYPE_ANY_WITHOUT_SEG;
        }
        err = wm_nvs_iterator_find_by_handle(handle, type, output_iterator);
    } else {
        err = WM_NVS_ERR_INVALID_HANDLE;
    }

    WM_NVS_UNLOCK();

    return err;
}

int wm_nvs_entry_next(wm_nvs_iterator_t *iterator)
{
    int err;

    if (!(iterator)) {
        return WM_NVS_ERR_INVALID_PARAM;
    }

    WM_NVS_LOCK();

    err = wm_nvs_iterator_next(iterator);

    WM_NVS_UNLOCK();

    return err;
}

int wm_nvs_entry_info(wm_nvs_iterator_t iterator, wm_nvs_entry_t *info)
{
    int err;

    if (!(iterator && info)) {
        return WM_NVS_ERR_INVALID_PARAM;
    }

    WM_NVS_LOCK();

    err = wm_nvs_iterator_info(iterator, info);

    WM_NVS_UNLOCK();

    return err;
}

int wm_nvs_entry_data(wm_nvs_iterator_t iterator, void *data, size_t *data_len)
{
    int err;

    if (!(iterator && data && data_len && *data_len > 0)) {
        return WM_NVS_ERR_INVALID_PARAM;
    }

    WM_NVS_LOCK();

    err = wm_nvs_iterator_data(iterator, data, data_len);

    WM_NVS_UNLOCK();

    return err;
}

int wm_nvs_release_iterator(wm_nvs_iterator_t iterator)
{
    int err;

    if (!(iterator)) {
        return WM_NVS_ERR_OK;
    }

    WM_NVS_LOCK();

    err = wm_nvs_iterator_release(iterator);

    WM_NVS_UNLOCK();

    return err;
}

int wm_nvs_print(const char *partition_name, const char *group_name)
{
    int err;

    if (!partition_name) {
        return WM_NVS_ERR_INVALID_PARAM;
    }

    WM_NVS_LOCK();

    err = wm_nvs_iterator_print(partition_name, group_name);

    WM_NVS_UNLOCK();

    return err;
}

int wm_nvs_get_info(wm_nvs_handle_t handle, const char *key, wm_nvs_type_t *type, size_t *size)
{
    int err;
    wm_nvs_handle_info_t *h = (wm_nvs_handle_info_t *)handle;
    wm_nvs_item_t item;

    if (!(handle && key && type && size)) {
        return WM_NVS_ERR_INVALID_PARAM;
    }

    WM_NVS_LOCK();

    err = wm_nvs_storage_find_key(h->storage, h->group_id, key, &item);
    if (err == WM_NVS_ERR_OK) {
        *type = item.type;

        if (item.type == WM_NVS_TYPE_BLOB) {
            *size = item.all_size;
        } else {
            *size = item.length;
        }
    }

    WM_NVS_UNLOCK();

    WM_NVS_LOGD("get info %s, size=%u, err=%d", key, ((!err) ? *size : 0), err);

    return err;
}

int wm_nvs_get_status(const char *partition_name, wm_nvs_status_t *status)
{
    int err;

    if (!(partition_name && status)) {
        return WM_NVS_ERR_INVALID_PARAM;
    }

    WM_NVS_LOCK();

    err = wm_nvs_ptm_get_status(partition_name, status);

    WM_NVS_UNLOCK();

    return err;
}


/**
 * @file wm_nvs_port.c
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
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include "wm_dt.h"
#include "wm_drv_flash.h"

#define LOG_TAG "nvs_debug"
#include "wm_nvs_porting.h"
#include "wm_nvs_partition.h"
#include "wm_nvs_storage.h"
#include "wm_nvs_partition_manager.h"
#include "wm_nvs.h"
#include "wm_nvs_debug.h"

#define WM_NVS_DEBUG_HANDLER_NUM (WM_NVS_MAX_HANDLE_NUM + 2)

static wm_nvs_handle_t g_cmd_handle[WM_NVS_DEBUG_HANDLER_NUM];

int wm_nvs_debug_print_sector(const char *partition_name, int index, int size)
{
    wm_partition_item_t ptable;
    wm_device_t *dev = NULL;
    uint8_t *sec     = NULL;
    uint32_t offset;
    int err;

    err = wm_partition_table_find(partition_name, &ptable);
    if (err != WM_ERR_SUCCESS) {
        wm_log_error("read pt %s fail", partition_name);
        return err;
    }

    dev = wm_dt_get_device_by_name(WM_NVS_FLASH_DEVICE);
    if (!dev) {
        wm_log_error("get flash dev %s fail", WM_NVS_FLASH_DEVICE);
        return WM_NVS_ERR_FAIL;
    }

    sec = WM_NVS_MALLOC(WM_NVS_SECTION_SIZE);
    if (!sec) {
        return WM_NVS_ERR_NO_MEM;
    }

    offset = ptable.offset + index * WM_NVS_SECTION_SIZE;

    err = wm_drv_flash_read(dev, offset, sec, WM_NVS_SECTION_SIZE);

    wm_log_info("read off[%d]=0x%x, err=%d", index, offset, err);

    if (err == WM_ERR_SUCCESS) {
        wm_log_dump(WM_LOG_LEVEL_INFO, "sec", 32, sec, size);
    }

    WM_NVS_FREE(sec);

    return err;
}

int wm_nvs_debug_print_status(const char *partition_name)
{
    wm_nvs_status_t status;
    int err;

    err = wm_nvs_get_status(partition_name, &status);
    if (err == WM_NVS_ERR_OK) {
        wm_log_info("total_size=0x%x, %u", status.total_size, status.total_size);
        wm_log_info("using_size=0x%x, %u", status.using_size, status.using_size);
        wm_log_info("droped_size=0x%x, %u", status.droped_size, status.droped_size);
        wm_log_info("free_size=0x%x, %u", status.free_size, status.free_size);
        wm_log_info("group_num=%d", status.group_num);
        wm_log_info("handle_num=%d", status.handle_num);
    } else {
        wm_log_error("get status err=%d", err);
    }

    return err;
}

static int wm_nvs_debug_nvs_get_user_id(wm_nvs_handle_t handle)
{
    int ret = -1;

    for (int i = 0; i < WM_NVS_DEBUG_HANDLER_NUM; i++) {
        if (g_cmd_handle[i] == handle) {
            ret = i;
            break;
        }
    }

    return ret;
}

int wm_nvs_debug_print_storage(int storage_detail, int sec_detail)
{
    wm_nvs_storage_t *it;
    wm_nvs_sector_t *sec;
    wm_nvs_group_t *group;
    wm_nvs_handle_info_t *hinfo = NULL;

    struct dl_list *storage     = wm_nvs_get_storage_list();
    struct dl_list *handle_list = wm_nvs_get_handle_list();

    if (dl_list_empty(storage)) {
        wm_log_info("Not init!");
        return WM_NVS_ERR_NOT_INIT;
    }

    /*look up storage from storage list*/
    dl_list_for_each(it, storage, wm_nvs_storage_t, list)
    {
        wm_log_info("%s: offset=[%d,0x%x], sec_num=%u, readonly=%d", it->pt.name, it->pt.offset / WM_NVS_SECTION_SIZE,
                    it->pt.offset, it->pt.sec_num, it->pt.readonly);

        if (storage_detail) {
            wm_log_info("active : %d", dl_list_len(&it->sm.active));
            wm_log_info("idle : %d\r\n", dl_list_len(&it->sm.idle));
            wm_log_info("serial_number %u", it->sm.serial_number);

            wm_nvs_debug_print_status(it->pt.name);

            /*look up sgroup list*/
            wm_log_raw_info("group list:\r\n");

            dl_list_for_each(group, &it->group_list, wm_nvs_group_t, list)
            {
                wm_log_raw_info("group id=%02d : %s\r\n", group->id, group->name);

                dl_list_for_each(hinfo, handle_list, wm_nvs_handle_info_t, list)
                {
                    int use_id = wm_nvs_debug_nvs_get_user_id((wm_nvs_handle_t)hinfo);

                    if (hinfo->group_id == group->id) {
                        wm_log_raw_info("   handle %p: valid=%d,mode=%d, user_id=%d\r\n", hinfo, hinfo->valid, hinfo->mode,
                                        use_id);
                    }
                }
            }

            wm_log_raw_info("\r\n");

            if (sec_detail) {
                wm_log_raw_info("active sec list:\r\n");

                /*look up active sector list*/

                dl_list_for_each(sec, &it->sm.active, wm_nvs_sector_t, list)
                {
                    int free_slice = WM_NVS_ENTRY_COUNT - sec->used_slice;
                    wm_log_raw_info("state=%x, sec index=[%u], sn=%d, next=%d, used=%d, droped=%d, left=[%u,%u]\r\n",
                                    sec->state, sec->address / WM_NVS_SECTION_SIZE, sec->serial_number, sec->next_free_slice,
                                    sec->used_slice, sec->droped_slice, free_slice, free_slice * WM_NVS_SLICE_SIZE);

                    if (sec_detail > 2) {
                        wm_nvs_hash_t *h = &sec->hash;

                        wm_log_raw_info("   hash list:\r\n");
                        for (int i = 0; i < h->count; i++) {
                            if (h->hash_table[i].index != WM_NVS_HASH_INVALID) {
                                wm_log_raw_info("       %d: %d-0x%06x", i, h->hash_table[i].index, h->hash_table[i].hash);
                                if ((i + 1) % 5 == 0) {
                                    wm_log_raw_info("\r\n");
                                }
                            }
                        }
                        wm_log_raw_info("\r\n");
                    }
                }
            }

            wm_log_raw_info("\r\n");
        }
    }

    return WM_NVS_ERR_OK;
}

int wm_nvs_debug_print_handle(void)
{
    wm_nvs_handle_info_t *it;
    int cnt = 0;
    struct dl_list *handle_list;

    handle_list = wm_nvs_get_handle_list();

    wm_log_raw_info("handler list:\r\n");

    dl_list_for_each(it, handle_list, wm_nvs_handle_info_t, list)
    {
        wm_log_raw_info("handler %d,%p: pt=%s, group_id=%d,valid=%d,mode=%d\r\n", cnt++, it, it->storage->pt.name, it->group_id,
                        it->valid, it->mode);
    }

    return WM_NVS_ERR_OK;
}

int wm_nvs_debug_nvs_open(const char *partition, const char *group, wm_nvs_open_mode_t mode)
{
    wm_nvs_handle_t handle = NULL;

    int err = wm_nvs_open(partition, group, mode, &handle);
    if (err != WM_NVS_ERR_OK) {
        wm_log_error("open nvs %s %s %d,err=%d", partition, group, mode, err);
        return err;
    }

    for (int i = 0; i < WM_NVS_DEBUG_HANDLER_NUM; i++) {
        if (!g_cmd_handle[i]) {
            g_cmd_handle[i] = handle;
            wm_log_info("open nvs %s %s %d OK, handle_index=%d,handle=%p", partition, group, mode, i, handle);
            return WM_NVS_ERR_OK;
        }
    }

    return WM_NVS_ERR_FAIL;
}

int wm_nvs_debug_nvs_close(int id)
{
    if (id >= 0 && id < WM_NVS_DEBUG_HANDLER_NUM && g_cmd_handle[id]) {
        int err = wm_nvs_close(g_cmd_handle[id]);
        if (err == WM_NVS_ERR_OK) {
            g_cmd_handle[id] = NULL;
        }
        wm_log_info("close %d, %p, err=%d", id, g_cmd_handle[id], err);
        return err;
    } else {
        wm_log_error("not open %d", id);
        return WM_NVS_ERR_INVALID_HANDLE;
    }
}

wm_nvs_handle_t wm_nvs_debug_nvs_get_handle(int id)
{
    wm_nvs_handle_t handle = NULL;

    if (id >= 0 && id < WM_NVS_DEBUG_HANDLER_NUM && g_cmd_handle[id]) {
        handle = g_cmd_handle[id];
    } else {
        wm_log_error("the handler index=%d not opend", id);
    }

    return handle;
}

bool wm_nvs_debug_nvs_check_handle_id(int id)
{
    if (!(id >= 0 && id < WM_NVS_DEBUG_HANDLER_NUM && g_cmd_handle[id])) {
        wm_log_error("the handler index=%d not opend", id);
        return false;
    }
    return true;
}

int wm_nvs_debug_clear_partiton_handle(const char *partition)
{
    wm_nvs_handle_info_t *info;

    for (int i = 0; i < WM_NVS_DEBUG_HANDLER_NUM; i++) {
        info = (wm_nvs_handle_info_t *)g_cmd_handle[i];
        if (info) {
            if (!strcmp(info->storage->pt.name, partition)) {
                WM_NVS_LOGD("clear %d", i);
                g_cmd_handle[i] = NULL;
            }
        }
    }

    return WM_NVS_ERR_OK;
}

void wm_nvs_debug_print_value(const char *key, wm_nvs_type_t type, const void *p, size_t length)
{
    if (!(key && p)) {
        return;
    }

    switch (type) {
        case WM_NVS_TYPE_STRING:
        {
            wm_log_info("%s=%s", key, (length > 0 ? (char *)p : ""));
            break;
        }
        case WM_NVS_TYPE_INT32:
        {
            wm_log_info("%s=%d", key, *((int *)p));
            break;
        }
        case WM_NVS_TYPE_UINT32:
        {
            wm_log_info("%s=%u", key, *((unsigned int *)p));
            break;
        }
        case WM_NVS_TYPE_INT8:
        {
            wm_log_info("%s=%d", key, *((signed char *)p));
            break;
        }
        case WM_NVS_TYPE_INT16:
        {
            wm_log_info("%s=%d", key, *((short *)p));
            break;
        }
        case WM_NVS_TYPE_UINT8:
        {
            wm_log_info("%s=%u", key, *((unsigned char *)p));
            break;
        }
        case WM_NVS_TYPE_UINT16:
        {
            wm_log_info("%s=%u", key, *((unsigned short *)p));
            break;
        }
        case WM_NVS_TYPE_INT64:
        {
            wm_log_info("%s=%lld", key, (int64_t)(*((int64_t *)p)));
            break;
        }
        case WM_NVS_TYPE_UINT64:
        {
            wm_log_info("%s=%llu", key, (uint64_t)(*((uint64_t *)p)));
            break;
        }
        case WM_NVS_TYPE_DOUBLE:
        {
            wm_log_info("%s=%lf", key, *((double *)p));
            break;
        }
        case WM_NVS_TYPE_BINARY:
        case WM_NVS_TYPE_BLOB:
        {
            wm_log_dump_info(key, p, length);
            break;
        }
        default:
        {
            wm_log_error("error type");
            break;
        }
    }
}

int wm_nvs_debug_cmd_get_item(int id, const char *key)
{
    wm_nvs_type_t type;
    size_t size;
    int err;
    unsigned char *buf = NULL;

    if (!(id >= 0 && id < WM_NVS_DEBUG_HANDLER_NUM && g_cmd_handle[id])) {
        wm_log_error("the handler index=%d not opend", id);
        return WM_NVS_ERR_INVALID_HANDLE;
    }

    err = wm_nvs_get_info(g_cmd_handle[id], key, &type, &size);
    if (err == WM_ERR_SUCCESS) {
        if (type == WM_NVS_TYPE_STRING || type == WM_NVS_TYPE_BLOB || WM_NVS_TYPE_BINARY) {
            buf = WM_NVS_MALLOC(size);

            if (!buf) {
                buf = wm_heap_caps_alloc(size, WM_HEAP_CAP_SPIRAM);
            }

            if (!buf) {
                wm_log_error("malloc fail,size=%d\n", (int)size);
                return WM_NVS_ERR_NO_MEM;
            }
        }

        wm_log_info("get %s ,type=0x%x,len=%u", key, type, size);

        switch (type) {
            case WM_NVS_TYPE_STRING:
            {
                err = wm_nvs_get_str(g_cmd_handle[id], key, (char *)buf, &size);
                if (!err) {
                    wm_log_info("%s=%s", key, (char *)buf);
                }
                break;
            }
            case WM_NVS_TYPE_INT32:
            {
                int32_t v;
                err = wm_nvs_get_i32(g_cmd_handle[id], key, &v);
                if (!err) {
                    wm_log_info("%s=%d", key, v);
                }
                break;
            }
            case WM_NVS_TYPE_UINT32:
            {
                uint32_t v;
                err = wm_nvs_get_u32(g_cmd_handle[id], key, &v);
                if (!err) {
                    wm_log_info("%s=%u", key, v);
                }
                break;
            }
            case WM_NVS_TYPE_INT8:
            {
                int8_t v;
                err = wm_nvs_get_i8(g_cmd_handle[id], key, &v);
                if (!err) {
                    wm_log_info("%s=%d", key, v);
                }
                break;
            }
            case WM_NVS_TYPE_INT16:
            {
                int16_t v;
                err = wm_nvs_get_i16(g_cmd_handle[id], key, &v);
                if (!err) {
                    wm_log_info("%s=%d", key, v);
                }
                break;
            }
            case WM_NVS_TYPE_UINT8:
            {
                uint8_t v;
                err = wm_nvs_get_u8(g_cmd_handle[id], key, &v);
                if (!err) {
                    wm_log_info("%s=%u", key, v);
                }
                break;
            }
            case WM_NVS_TYPE_UINT16:
            {
                uint16_t v;
                err = wm_nvs_get_u16(g_cmd_handle[id], key, &v);
                if (!err) {
                    wm_log_info("%s=%u", key, v);
                }
                break;
            }
            case WM_NVS_TYPE_INT64:
            {
                int64_t v;
                err = wm_nvs_get_i64(g_cmd_handle[id], key, &v);
                if (!err) {
                    wm_log_info("%s=%lld", key, v);
                }
                break;
            }
            case WM_NVS_TYPE_UINT64:
            {
                uint64_t v;
                err = wm_nvs_get_u64(g_cmd_handle[id], key, &v);
                if (!err) {
                    wm_log_info("%s=%llu", key, v);
                }
                break;
            }
            case WM_NVS_TYPE_DOUBLE:
            {
                double v;
                err = wm_nvs_get_float(g_cmd_handle[id], key, &v);
                if (!err) {
                    wm_log_info("%s=%lf", key, v);
                }
                break;
            }
            case WM_NVS_TYPE_BINARY:
            {
                err = wm_nvs_get_binary(g_cmd_handle[id], key, buf, &size);
                if (!err) {
                    wm_log_dump_info(key, buf, size);
                }
                break;
            }
            case WM_NVS_TYPE_BLOB:
            {
                wm_log_info("start get blob %s", key);

                err = wm_nvs_get_blob(g_cmd_handle[id], key, buf, &size);
                if (!err) {
                    wm_log_dump_info(key, buf, size);
                }
                break;
            }
            default:
            {
                wm_log_info("error type");
                break;
            }
        }

    } else if (err == WM_NVS_ERR_NOT_FOUND) {
        wm_log_info("%s not exist.", key);
    }

    if (buf) {
        WM_NVS_FREE(buf);
    }

    return err;
}

int wm_nvs_debug_set_item_with_size(int id, const char *key, const char *str_size, wm_nvs_type_t type)
{
    int len;
    int err = WM_NVS_ERR_INVALID_PARAM;
    int i;
    uint8_t *p = NULL;

    if (wm_nvs_debug_nvs_check_handle_id(id)) {
        len = strtoul(str_size, NULL, 0);

        if (len > 0) {
            p = WM_NVS_MALLOC(len);

            if (!p) {
                p = wm_heap_caps_alloc(len, WM_HEAP_CAP_SPIRAM);
            }

            if (p) {
                for (i = 0; i < len; i++) {
                    p[i] = (uint8_t)(0x30 + (i % 10));
                }

                p[len - 1] = 0;

                if (type == WM_NVS_TYPE_STRING) {
                    err = wm_nvs_set_str(g_cmd_handle[id], key, (char *)p);
                } else if (type == WM_NVS_TYPE_BINARY) {
                    err = wm_nvs_set_binary(g_cmd_handle[id], key, (char *)p, len);
                } else if (type == WM_NVS_TYPE_BLOB) {
                    err = wm_nvs_set_blob(g_cmd_handle[id], key, (char *)p, len);
                } else {
                    wm_log_error("bad type,type=%d", type);
                }

                WM_NVS_FREE(p);
            } else {
                err = WM_NVS_ERR_NO_MEM;
                wm_log_error("no memory");
            }
        }
    }

    return err;
}

static int wm_nvs_debug_check_int_float(const char *type, const char *value)
{
    int err = WM_NVS_ERR_OK;
    int32_t v32;

    errno = 0;

    if (!strcmp(type, "U8") || !strcmp(type, "u8")) {
        v32 = strtoul(value, NULL, 0);
        if (v32 < 0 || v32 > 0xff || errno != 0) {
            err = WM_NVS_ERR_INVALID_PARAM;
        }

    } else if (!strcmp(type, "I8") || !strcmp(type, "i8")) {
        v32 = strtoul(value, NULL, 0);
        if (v32 < -0x80 || v32 > 0x7f || errno != 0) {
            err = WM_NVS_ERR_INVALID_PARAM;
        }

    } else if (!strcmp(type, "U16") || !strcmp(type, "u16")) {
        v32 = strtoul(value, NULL, 0);
        if (v32 < 0 || v32 > 0xffff || errno != 0) {
            err = WM_NVS_ERR_INVALID_PARAM;
        }

    } else if (!strcmp(type, "I16") || !strcmp(type, "i16")) {
        v32 = strtoul(value, NULL, 0);
        if (v32 < -0x8000 || v32 > 0x7fff || errno != 0) {
            err = WM_NVS_ERR_INVALID_PARAM;
        }

    } else if (!strcmp(type, "I32") || !strcmp(type, "i32")) {
        strtol(value, NULL, 0);
        if (!(errno == 0 || errno == ERANGE || INT32_MIN)) {
            err = WM_NVS_ERR_INVALID_PARAM;
        }

    } else if (!strcmp(type, "U32") || !strcmp(type, "u32")) {
        strtoul(value, NULL, 0);
        if (!(errno == 0 || errno == ERANGE || UINT32_MAX)) {
            err = WM_NVS_ERR_INVALID_PARAM;
        }
    } else if (!strcmp(type, "I64") || !strcmp(type, "i64")) {
        strtoll(value, NULL, 0);
        if (!(errno == 0 || errno == ERANGE || INT64_MIN)) {
            err = WM_NVS_ERR_INVALID_PARAM;
        }

    } else if (!strcmp(type, "U64") || !strcmp(type, "u64")) {
        strtoull(value, NULL, 0);
        if (!(errno == 0 || errno == ERANGE || UINT64_MAX)) {
            err = WM_NVS_ERR_INVALID_PARAM;
        }
    } else if (!strcmp(type, "FLOAT") || !strcmp(type, "float")) {
        strtof(value, NULL);
        if (errno != 0) {
            err = WM_NVS_ERR_INVALID_PARAM;
        }
    } else {
        wm_log_error("bad type");
        err = WM_NVS_ERR_INVALID_PARAM;
    }

    return err;
}

int wm_nvs_debug_set_base_item(int id, const char *key, const char *type, const char *value)
{
    int err = 0;

    wm_nvs_handle_t handle;

    handle = wm_nvs_debug_nvs_get_handle(id);
    if (!handle) {
        return WM_NVS_ERR_INVALID_HANDLE;
    }

    err = wm_nvs_debug_check_int_float(type, value);
    if (err != WM_NVS_ERR_OK) {
        wm_log_error("bad param value");
        return err;
    }

    if (!strcmp(type, "U8") || !strcmp(type, "u8")) {
        err = wm_nvs_set_u8(handle, key, (uint8_t)strtoul(value, NULL, 0));

    } else if (!strcmp(type, "I8") || !strcmp(type, "i8")) {
        err = wm_nvs_set_i8(handle, key, (uint8_t)strtol(value, NULL, 0));

    } else if (!strcmp(type, "U16") || !strcmp(type, "u16")) {
        err = wm_nvs_set_u16(handle, key, (uint16_t)strtoul(value, NULL, 0));

    } else if (!strcmp(type, "I16") || !strcmp(type, "i16")) {
        err = wm_nvs_set_i16(handle, key, (uint16_t)strtol(value, NULL, 0));

    } else if (!strcmp(type, "U32") || !strcmp(type, "u32")) {
        err = wm_nvs_set_u32(handle, key, (uint32_t)strtoul(value, NULL, 0));

    } else if (!strcmp(type, "I32") || !strcmp(type, "i32")) {
        err = wm_nvs_set_i32(handle, key, (uint32_t)strtol(value, NULL, 0));

    } else if (!strcmp(type, "U64") || !strcmp(type, "u64")) {
        err = wm_nvs_set_u64(handle, key, (uint64_t)strtoull(value, NULL, 0));

    } else if (!strcmp(type, "I64") || !strcmp(type, "i64")) {
        err = wm_nvs_set_i64(handle, key, (uint64_t)strtoll(value, NULL, 0));

    } else if (!strcmp(type, "FLOAT") || !strcmp(type, "float")) {
        err = wm_nvs_set_float(handle, key, strtof(value, NULL));
    } else {
        wm_log_error("bad type");
        err = WM_NVS_ERR_INVALID_PARAM;
    }
    wm_log_info("write err=%d", err);

    return err;
}

int wm_nvs_debug_del(int id, const char *key)
{
    int err = WM_NVS_ERR_INVALID_HANDLE;
    if (wm_nvs_debug_nvs_check_handle_id(id)) {
        err = wm_nvs_del_key(g_cmd_handle[id], key);
    }
    return err;
}

int wm_nvs_debug_del_group(int id)
{
    int err = WM_NVS_ERR_INVALID_HANDLE;

    if (wm_nvs_debug_nvs_check_handle_id(id)) {
        err = wm_nvs_del_group(g_cmd_handle[id]);
    }

    return err;
}

void wm_nvs_debug_cmd(int argc, char *argv[])
{
    int err = 0;

    if ((3 == argc || 4 == argc) && !strcmp(argv[1], "print")) {
        err = wm_nvs_print(argv[2], argc == 4 ? argv[3] : NULL);

    } else if (3 == argc && !strcmp(argv[1], "erase")) {
        wm_nvs_debug_clear_partiton_handle(argv[2]);

        err = wm_nvs_deinit(argv[2]);

        wm_log_info("deinit,err=%d", err);

        err = wm_nvs_erase(argv[2]);

        wm_log_info("erase partition,err=%d", err);

    } else if (5 == argc && !strcmp(argv[1], "open")) {
        wm_nvs_debug_nvs_open(argv[2], argv[3], (wm_nvs_open_mode_t)strtoul(argv[4], NULL, 0));

    } else if (3 == argc && !strcmp(argv[1], "close")) {
        wm_nvs_debug_nvs_close(strtoul(argv[2], NULL, 0));

    } else if (4 == argc && !strcmp(argv[1], "get")) {
        wm_nvs_debug_cmd_get_item(strtoul(argv[2], NULL, 0), argv[3]);

    } else if (5 == argc && !strcmp(argv[1], "ws")) {
        if (wm_nvs_debug_nvs_check_handle_id(strtoul(argv[2], NULL, 0))) {
            wm_nvs_set_str(g_cmd_handle[strtoul(argv[2], NULL, 0)], argv[3], argv[4]);
        }

    } else if (5 == argc && !strcmp(argv[1], "wslen")) {
        wm_nvs_debug_set_item_with_size(strtoul(argv[2], NULL, 0), argv[3], argv[4], WM_NVS_TYPE_STRING);

    } else if (5 == argc && !strcmp(argv[1], "wbin")) {
        wm_nvs_debug_set_item_with_size(strtoul(argv[2], NULL, 0), argv[3], argv[4], WM_NVS_TYPE_BINARY);

    } else if (5 == argc && !strcmp(argv[1], "wblob")) {
        wm_nvs_debug_set_item_with_size(strtoul(argv[2], NULL, 0), argv[3], argv[4], WM_NVS_TYPE_BLOB);

    } else if (6 == argc && !strcmp(argv[1], "wbase")) {
        wm_nvs_debug_set_base_item(strtoul(argv[2], NULL, 0), argv[3], argv[4], argv[5]);

    } else if (4 == argc && !strcmp(argv[1], "del")) {
        wm_nvs_debug_del(strtoul(argv[2], NULL, 0), argv[3]);

    } else if (3 == argc && !strcmp(argv[1], "delgroup")) {
        wm_nvs_debug_del_group(strtoul(argv[2], NULL, 0));

    } else if (argc >= 2 && !strcmp(argv[1], "debug")) {
        wm_nvs_debug_print_storage(1, 2);

    } else if (argc == 5 && !strcmp(argv[1], "dump")) {
        wm_nvs_debug_print_sector(argv[2], strtoul(argv[3], NULL, 0), strtoul(argv[4], NULL, 0));

    } else if (argc == 3 && !strcmp(argv[1], "status")) {
        wm_nvs_debug_print_status(argv[2]);

    } else {
        goto usage;
    }

    if (err != WM_ERR_SUCCESS) {
        wm_log_info("err=%d", err);
    }

    return;

usage:
    wm_log_raw_info("\nnvs usage:\n"
                    "  print          : nvs print    <partition>\n"
                    "  erase          : nvs erase    <partition> \n"
                    "  open handle    : nvs open     <partition> <group> <mode>\n"
                    "  close handle   : nvs close    <handle_index>\n"
                    "  get item       : nvs get      <handle_index> <key>\n"
                    "  del item       : nvs del      <handle_index> <key>\n"
                    "  set string item: nvs ws       <handle_index> <key> <value>\n"
                    "  set string len : nvs wslen    <handle_index> <key> <len>\n"
                    "  set binary len : nvs wbin     <handle_index> <key> <len>\n"
                    "  set blob       : nvs wblob    <handle_index> <key> <len>\n"
                    "  set base item  : nvs wbase    <handle_index> <key> <type=[u8,i8,u16,i16,u32,i32,u64,i64,float]> <value> "
                    "\n"
                    "  del group      : nvs delgroup <handle_index> \n"
                    "  dump sector    : nvs dump     <partition> <sec_index> <size>\n"
                    "  status         : nvs status\n"
                    "  debug          : nvs debug\n");

    return;
}

/**
 * @file wm_nvs_iterator.c
 *
 * @brief nvs iterator
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
#define LOG_TAG "nvs_iterator"
#include "wm_nvs_porting.h"
#include "wm_nvs.h"
#include "wm_nvs_item.h"
#include "wm_nvs_sector.h"
#include "wm_nvs_iterator.h"
#include "wm_nvs_storage.h"
#include "wm_nvs_partition_manager.h"

static int iterator_find(wm_nvs_storage_t *storage, uint8_t group_id, wm_nvs_type_t type, wm_nvs_iterator_t *output_iterator)
{
    wm_nvs_iterator_info_t *it;

    it = WM_NVS_CALLOC(1, sizeof(*it));

    if (!it) {
        WM_NVS_LOGV("No mem");
        return WM_NVS_ERR_NO_MEM;
    }

    it->storage  = storage;
    it->group_id = group_id;
    it->type     = type;

    *output_iterator = (wm_nvs_iterator_t)it;

    /*find next*/
    return wm_nvs_iterator_next(output_iterator);
}

int wm_nvs_iterator_find(const char *partition_name, const char *group, wm_nvs_type_t type, wm_nvs_iterator_t *output_iterator)
{
    wm_nvs_group_t *group_node;
    wm_nvs_storage_t *storage;

    uint8_t group_id = WM_NVS_GROUP_ID_ANY;

    storage = wm_nvs_ptm_find_storage(partition_name);
    if (!storage) {
        WM_NVS_LOGD("Not init");
        return WM_NVS_ERR_NOT_INIT;
    }

    if (group) {
        group_node = wm_nvs_storage_find_group_by_name(storage, group);
        if (group_node) {
            group_id = group_node->id;
        } else {
            WM_NVS_LOGD("group %s not found", group);
            return WM_NVS_ERR_NOT_FOUND;
        }
    }

    return iterator_find(storage, group_id, type, output_iterator);
}

int wm_nvs_iterator_find_by_handle(wm_nvs_handle_t handle, wm_nvs_type_t type, wm_nvs_iterator_t *output_iterator)
{
    wm_nvs_handle_info_t *h = (wm_nvs_handle_info_t *)handle;

    return iterator_find(h->storage, h->group_id, type, output_iterator);
}

int wm_nvs_iterator_next(wm_nvs_iterator_t *iterator)
{
    int err;
    wm_nvs_iterator_info_t *it  = (wm_nvs_iterator_info_t *)(*iterator);
    wm_nvs_sector_manager_t *sm = &it->storage->sm;
    wm_nvs_sector_t *sec        = NULL;
    wm_nvs_item_t item;
    wm_nvs_group_t *group;
    wm_nvs_type_t look_up_type;

    /*don't look up blob segment*/
    look_up_type = (it->type == WM_NVS_TYPE_ANY ? WM_NVS_TYPE_ANY_WITHOUT_SEG : it->type);

    if (!it->sector) {
        /*first*/
        it->sector      = dl_list_first(&sm->active, wm_nvs_sector_t, list);
        it->slice_index = 0;
    }

    WM_NVS_LOGD("iterator next: gid=%d,type=%d,index=%d", it->group_id, it->type, it->slice_index);

    for (sec = it->sector; &sec->list != &sm->active;) {
        err = wm_nvs_sector_find_item(sec, it->group_id, look_up_type, NULL, &it->slice_index, &item, WM_NVS_SEG_ID_ANY,
                                      WM_NVS_SEG_START_ANY);

        if (err == WM_NVS_ERR_OK) {
            it->slice_index += wm_nvs_item_get_span(&item);

            /*found*/

            /*get group*/
            group = NULL;
            if (item.group_id > 0) {
                group = wm_nvs_storage_find_group_by_id(it->storage, item.group_id);
            }

            /*set or clear group name*/
            if (group) {
                snprintf(it->info.group, sizeof(it->info.group), "%s", group->name);
            } else {
                it->info.group[0] = 0;
            }

            snprintf(it->info.key, sizeof(it->info.key), "%.*s", WM_NVS_MAX_KEY_LEN, item.name);

            it->info.type     = item.type;
            it->info.length   = (item.type == WM_NVS_TYPE_BLOB ? item.all_size : item.length);
            it->info.group_id = item.group_id;

            return err;

        } else {
            /*error*/
            WM_NVS_LOGD("find err=%d", err);

            /*next sector*/
            sec             = dl_list_entry(sec->list.next, wm_nvs_sector_t, list);
            it->sector      = sec;
            it->slice_index = 0;

            WM_NVS_LOGV("state=%x, sec offset=%u, sn=%d, next=%d, used=%d, droped=%d\r\n", sec->state, sec->address,
                        sec->serial_number, sec->next_free_slice, sec->used_slice, sec->droped_slice);
        }
    }

    /*iterator end*/
    WM_NVS_FREE(it);
    *iterator = NULL;

    return WM_NVS_ERR_NOT_FOUND;
}

int wm_nvs_iterator_info(wm_nvs_iterator_t iterator, wm_nvs_entry_t *info)
{
    *info = ((wm_nvs_iterator_info_t *)iterator)->info;
    return WM_NVS_ERR_OK;
}

int wm_nvs_iterator_data(wm_nvs_iterator_t iterator, void *data, size_t *data_len)
{
    wm_nvs_iterator_info_t *it = (wm_nvs_iterator_info_t *)iterator;
    wm_nvs_entry_t *pinfo      = &it->info;

    return wm_nvs_storage_read_item(it->storage, pinfo->group_id, pinfo->type, pinfo->key, data, data_len);
}

int wm_nvs_iterator_release(wm_nvs_iterator_t iterator)
{
    if (iterator) {
        WM_NVS_FREE(iterator);
    }
    return WM_NVS_ERR_OK;
}

static void iterator_print_value(const char *group, const char *key, wm_nvs_type_t type, const void *p, size_t length)
{
    switch (type) {
        case WM_NVS_TYPE_STRING:
        {
            wm_log_info("%s:%s=%s", group, key, (length > 0 ? (char *)p : ""));
            break;
        }
        case WM_NVS_TYPE_INT32:
        {
            wm_log_info("%s:%s=%d", group, key, *((int *)p));
            break;
        }
        case WM_NVS_TYPE_UINT32:
        {
            wm_log_info("%s:%s=%u", group, key, *((unsigned int *)p));
            break;
        }
        case WM_NVS_TYPE_INT8:
        {
            wm_log_info("%s:%s=%d", group, key, *((signed char *)p));
            break;
        }
        case WM_NVS_TYPE_INT16:
        {
            wm_log_info("%s:%s=%d", group, key, *((short *)p));
            break;
        }
        case WM_NVS_TYPE_UINT8:
        {
            wm_log_info("%s:%s=%u", group, key, *((unsigned char *)p));
            break;
        }
        case WM_NVS_TYPE_UINT16:
        {
            wm_log_info("%s:%s=%u", group, key, *((unsigned short *)p));
            break;
        }
        case WM_NVS_TYPE_INT64:
        {
            wm_log_info("%s:%s=%lld", group, key, (int64_t)(*((int64_t *)p)));
            break;
        }
        case WM_NVS_TYPE_UINT64:
        {
            wm_log_info("%s:%s=%llu", group, key, (uint64_t)(*((uint64_t *)p)));
            break;
        }
        case WM_NVS_TYPE_DOUBLE:
        {
            wm_log_info("%s:%s=%lf", group, key, *((double *)p));
            break;
        }
        case WM_NVS_TYPE_BINARY:
        case WM_NVS_TYPE_BLOB:
        {
            wm_log_info("%s:%s", group, key);
            wm_log_dump_info(key, p, length);
            break;
        }
        default:
        {
            WM_NVS_LOGE("error type %d", type);
            break;
        }
    }
}

int wm_nvs_iterator_print(const char *partition_name, const char *group_name)
{
    int err;
    wm_nvs_iterator_t iterator = NULL;
    wm_nvs_entry_t entry;
    uint8_t buf[32];
    size_t length;

    if (!partition_name) {
        return WM_NVS_ERR_INVALID_PARAM;
    }

    WM_NVS_LOGD("start print,pt=%s,group=%s", partition_name, group_name ? group_name : "NULL");

    err = wm_nvs_iterator_find(partition_name, group_name, WM_NVS_TYPE_ANY_WITHOUT_SEG, &iterator);
    if (err == WM_NVS_ERR_OK) {
        WM_NVS_LOGD("find %s iterator=%p", partition_name, iterator);
        while (iterator) {
            wm_nvs_iterator_info(iterator, &entry);

            WM_NVS_LOGD("get iterator info %s:%s,length=%u", entry.group, entry.key, entry.length);

            if (entry.length <= sizeof(buf)) {
                length = sizeof(buf);
                err    = wm_nvs_iterator_data(iterator, buf, &length);
                if (err == WM_NVS_ERR_OK) {
                    iterator_print_value(entry.group, entry.key, entry.type, buf, length);
                }
            } else {
                char *p = WM_NVS_MALLOC(entry.length);
                if (!p) {
                    break;
                }

                length = entry.length;
                err    = wm_nvs_iterator_data(iterator, p, &length);
                if (err == WM_NVS_ERR_OK) {
                    iterator_print_value(entry.group, entry.key, entry.type, p, length);
                }
                WM_NVS_FREE(p);
            }
            wm_nvs_iterator_next(&iterator);
        }
        wm_nvs_iterator_release(iterator);
    } else {
        WM_NVS_LOGD("find %s error", partition_name);
    }

    return WM_NVS_ERR_OK;
}


/**
 * @file wm_nvs_storage.c
 *
 * @brief nvs storage module
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

#define LOG_TAG "nvs_store"
#include "wm_nvs_porting.h"
#include "wm_nvs.h"
#include "wm_nvs_storage.h"
#include "wm_nvs_debug.h"

typedef struct {
    struct dl_list list;               /**< blob check list          */
    char name[WM_NVS_MAX_KEY_LEN + 1]; /**< blob desc name           */
    uint8_t group_id;                  /**< group id                 */
    uint8_t seg_count;                 /**< segment count            */
    uint8_t count_seg_cnt;             /**< calculated segment count */
    uint8_t seg_start;                 /**< segment start            */
    uint32_t desc_data_size;           /**< blob data all size       */
    uint32_t count_data_size;          /**< calculated segment size  */
} wm_nvs_blob_into_t;

/*
find group id or find a free id for new group
*/
static int storage_find_group(wm_nvs_storage_t *storage, const char *group, uint8_t *id)
{
    wm_nvs_group_t *entry = NULL;
    uint32_t i;

    uint64_t id_map = 0;

    /* Look up storage from storage list */
    dl_list_for_each(entry, &storage->group_list, wm_nvs_group_t, list)
    {
        if (!strcmp(entry->name, group)) {
            *id = entry->id;
            return WM_NVS_ERR_OK;
        }

        id_map |= ((uint64_t)1) << entry->id;
    }

    /* Get a free id */
    for (i = 1; i < WM_NVS_GROUP_ID_MAX; i++) {
        if (!(id_map & (((uint64_t)1) << i))) {
            break;
        }
    }

    *id = i;
    WM_NVS_LOGD("get free id=%d", i);

    return WM_NVS_ERR_FAIL;
}

static int storage_add_group(wm_nvs_storage_t *storage, uint8_t group_id, const char *group_name, int max_size)
{
    wm_nvs_group_t *group_node;

    group_node = WM_NVS_CALLOC(1, sizeof(*group_node));
    if (!group_node) {
        return WM_NVS_ERR_NO_MEM;
    }

    /*init the group node and add it to list*/
    dl_list_init(&group_node->list);
    group_node->id = group_id;
    snprintf(group_node->name, sizeof(group_node->name), "%.*s", max_size, group_name);

    dl_list_add_tail(&storage->group_list, &group_node->list);

    return WM_NVS_ERR_OK;
}

static int storage_load_groups(wm_nvs_storage_t *storage)
{
    int err;
    int item_index = 0;
    wm_nvs_item_t item;

    struct dl_list *active = &storage->sm.active;

    wm_nvs_sector_t *entry = NULL;

    /* Look up storage from storage list */
    dl_list_for_each(entry, active, wm_nvs_sector_t, list)
    {
        item_index = 0;

        while (1) {
            err = wm_nvs_sector_find_item(entry, WM_NVS_GROUP_ITSELF_ID, WM_NVS_TYPE_UINT8, NULL, &item_index, &item,
                                          WM_NVS_SEG_ID_ANY, WM_NVS_SEG_START_ANY);
            if (err == WM_NVS_ERR_OK) {
                storage_add_group(storage, item.data[0], item.name, WM_NVS_MAX_KEY_LEN);
                WM_NVS_LOGD("add group %.*s", WM_NVS_MAX_KEY_LEN, item.name);

                item_index += wm_nvs_item_get_span(&item);
            } else {
                break;
            }
        }
    }

    wm_log_debug("group load end\n");

    return WM_NVS_ERR_OK;
}

static int storage_cmp_find_item(wm_nvs_storage_t *storage, wm_nvs_sector_t *find_sector, int item_index,
                                 wm_nvs_item_t *find_item, const void *data, size_t size)
{
    int err;

    if (!find_sector) {
        return WM_NVS_ERR_FAIL;
    }

    if (size != find_item->length) {
        return WM_NVS_ERR_FAIL;
    }

    if (size <= 8) {
        return memcmp(data, find_item->data, size) == 0 ? WM_NVS_ERR_OK : WM_NVS_ERR_FAIL;
    } else {
        uint8_t *p = WM_NVS_MALLOC(size);
        if (p) {
            err = wm_nvs_pt_read(find_sector->pt, find_sector->address + (item_index + 2) * WM_NVS_SLICE_SIZE, p, size);
            WM_NVS_LOGD("read data, index=%d,size=%d,err=%d", item_index, size, err);
            if (err == WM_NVS_ERR_OK) {
                err = (memcmp(data, p, size) == 0 ? WM_NVS_ERR_OK : WM_NVS_ERR_FAIL);
            }
            WM_NVS_FREE(p);
            return err;
        } else {
            return WM_NVS_ERR_FAIL;
        }
    }
}

static int storage_blob_init_info(wm_nvs_storage_t *storage, struct dl_list *info)
{
    int err;
    struct dl_list *active = &storage->sm.active;
    wm_nvs_sector_t *it    = NULL;
    int item_index;
    wm_nvs_item_t item;
    wm_nvs_blob_into_t *blob;

    dl_list_for_each(it, active, wm_nvs_sector_t, list)
    {
        item_index = 0;

        while (1) {
            /*find blob descriptor*/
            err = wm_nvs_sector_find_item(it, WM_NVS_GROUP_ID_ANY, WM_NVS_TYPE_BLOB, NULL, &item_index, &item,
                                          WM_NVS_SEG_ID_ANY, WM_NVS_SEG_START_ANY);
            if (err != WM_NVS_ERR_OK) {
                break;
            }

            blob = WM_NVS_MALLOC(sizeof(*blob));
            if (!blob) {
                return WM_NVS_ERR_NO_MEM;
            }

            /*record blob infomation*/

            dl_list_init(&blob->list);

            memcpy(blob->name, item.name, WM_NVS_MAX_KEY_LEN);
            blob->name[WM_NVS_MAX_KEY_LEN] = 0;

            blob->group_id       = item.group_id;
            blob->seg_count      = item.seg_count;
            blob->seg_start      = item.seg_start;
            blob->desc_data_size = item.all_size;

            blob->count_seg_cnt   = 0;
            blob->count_data_size = 0;

            dl_list_add_tail(info, &blob->list);

            /*goto next item*/
            item_index += wm_nvs_item_get_span(&item);
        }
    }

    return WM_NVS_ERR_OK;
}

static int storage_blob_deinit_info(struct dl_list *info)
{
    wm_nvs_sector_t *it = NULL;
    wm_nvs_sector_t *next;

    dl_list_for_each_safe(it, next, info, wm_nvs_sector_t, list)
    {
        dl_list_del(&it->list);
        WM_NVS_FREE(it);
    }

    return WM_NVS_ERR_OK;
}

static int storage_blob_check_match(wm_nvs_storage_t *storage, struct dl_list *info)
{
    int err;
    struct dl_list *active = &storage->sm.active;
    wm_nvs_sector_t *it    = NULL;
    int item_index;
    wm_nvs_item_t item;
    wm_nvs_blob_into_t *desc = NULL;
    wm_nvs_blob_into_t *desc_next;
    wm_nvs_sector_t *found_sec = NULL;

    WM_NVS_LOGD("blob check match");

    if (!dl_list_len(info)) {
        WM_NVS_LOGD("No blob desc");
        return WM_NVS_ERR_OK;
    }

    dl_list_for_each(it, active, wm_nvs_sector_t, list)
    {
        item_index = 0;

        while (1) {
            /*find blob segments*/
            err = wm_nvs_sector_find_item(it, WM_NVS_GROUP_ID_ANY, WM_NVS_TYPE_BLOB_SEG, NULL, &item_index, &item,
                                          WM_NVS_SEG_ID_ANY, WM_NVS_SEG_START_ANY);
            if (err != WM_NVS_ERR_OK) {
                break;
            }

            dl_list_for_each(desc, info, wm_nvs_blob_into_t, list)
            {
                if (!strncmp(item.name, desc->name, WM_NVS_MAX_KEY_LEN) && item.group_id == desc->group_id &&
                    item.seg_id >= desc->seg_start &&
                    item.seg_id < (desc->seg_start == WM_NVS_SEG_START_VER_1 ? WM_NVS_SEG_START_ANY : WM_NVS_SEG_START_VER_1)) {
                    /*statistic segments count and segments size*/
                    wm_log_debug("count seg: %.*s, desc=%s,seg_id=%d,gid=[%d,%d]", WM_NVS_MAX_KEY_LEN, item.name, desc->name,
                                 item.seg_id, item.group_id, desc->group_id);
                    desc->count_seg_cnt++;
                    desc->count_data_size += item.length;
                    break;
                }
            }

            /*goto next item*/
            item_index += wm_nvs_item_get_span(&item);
        }
    }

    /*find and erase dismatch blob descriptor*/
    dl_list_for_each_safe(desc, desc_next, info, wm_nvs_blob_into_t, list)
    {
        if (desc->count_seg_cnt != desc->seg_count || desc->count_data_size != desc->desc_data_size) {
            WM_NVS_LOGD("blob mismatch,name=%s,count=%u,%u, size=%u,%u", desc->name, desc->count_seg_cnt, desc->seg_count,
                        desc->count_data_size, desc->desc_data_size);

            /*erase blob descriptor*/
            err = wm_nvs_sm_find_item(&storage->sm, desc->group_id, WM_NVS_TYPE_BLOB, desc->name, &item_index, &found_sec,
                                      &item, WM_NVS_SEG_ID_ANY, WM_NVS_SEG_START_ANY);
            if (err == WM_NVS_ERR_OK) {
                WM_NVS_LOGD("erase blob desc %s", desc->name);
                wm_nvs_sector_erase_item(found_sec, item_index, &item, true);
            }

            /*remove from list and delete it*/
            dl_list_del(&desc->list);
            WM_NVS_FREE(desc);
        }
    }

    WM_NVS_LOGD("blob check match end\n");

    return WM_NVS_ERR_OK;
}

static int storage_blob_check_drop(wm_nvs_storage_t *storage, struct dl_list *info)
{
    int err;
    struct dl_list *active = &storage->sm.active;
    wm_nvs_sector_t *it    = NULL;
    int item_index;
    wm_nvs_item_t item;
    wm_nvs_blob_into_t *desc = NULL;
    bool found;

    WM_NVS_LOGD("blob check drop");

    dl_list_for_each(it, active, wm_nvs_sector_t, list)
    {
        item_index = 0;

        while (1) {
            /*find blob segments*/
            err = wm_nvs_sector_find_item(it, WM_NVS_GROUP_ID_ANY, WM_NVS_TYPE_BLOB_SEG, NULL, &item_index, &item,
                                          WM_NVS_SEG_ID_ANY, WM_NVS_SEG_START_ANY);
            if (err != WM_NVS_ERR_OK) {
                WM_NVS_LOGD("no seg: 0x%x,slice_index=%d", it->address, item_index);
                break;
            }

            WM_NVS_LOGD("find seg: %.*s,seg_id=%d", WM_NVS_MAX_KEY_LEN, item.name, item.seg_id);

            found = 0;
            dl_list_for_each(desc, info, wm_nvs_blob_into_t, list)
            {
                /*look for the blob descriptor*/
                if (!strncmp(item.name, desc->name, strnlen(desc->name, WM_NVS_MAX_KEY_LEN)) &&
                    item.group_id == desc->group_id && item.seg_id >= desc->seg_start &&
                    item.seg_id < (desc->seg_start == WM_NVS_SEG_START_VER_1 ? WM_NVS_SEG_START_ANY : WM_NVS_SEG_START_VER_1)) {
                    /*match*/
                    found = 1;
                    break;
                }
            }

            WM_NVS_LOGD("find seg: %.*s,seg_id=%d, found_desc=%d", WM_NVS_MAX_KEY_LEN, item.name, item.seg_id, found);

            if (!found) {
                /*the blob descriptor is droped, erase the segment*/
                wm_nvs_sector_erase_item(it, item_index, &item, true);
            }

            item_index += wm_nvs_item_get_span(&item);
        }
    }

    WM_NVS_LOGD("blob check drop end\n");

    return WM_NVS_ERR_OK;
}

static int storage_blob_check(wm_nvs_storage_t *storage)
{
    int err;
    struct dl_list info = DL_LIST_HEAD_INIT(info);

    WM_NVS_LOGD("start blob check");

    /*init blob desc information*/
    err = storage_blob_init_info(storage, &info);
    if (err != WM_NVS_ERR_OK) {
        goto BLOB_CHECK_END;
    }

    WM_NVS_LOGD("get blob info num=%d", dl_list_len(&info));

    /*check length and segments count match*/
    err = storage_blob_check_match(storage, &info);
    if (err != WM_NVS_ERR_OK) {
        goto BLOB_CHECK_END;
    }

    /*check and erase the droped segments*/
    err = storage_blob_check_drop(storage, &info);
    if (err != WM_NVS_ERR_OK) {
        goto BLOB_CHECK_END;
    }

BLOB_CHECK_END:

    WM_NVS_LOGD("end blob check\n");

    /*delete blob info*/
    storage_blob_deinit_info(&info);

    return err;
}

int wm_nvs_storage_init(wm_nvs_partition_t *pt, wm_nvs_storage_t **storage)
{
    int err;
    wm_nvs_storage_t *store = NULL;

    store = WM_NVS_CALLOC(1, sizeof(*store));
    if (!store) {
        return WM_NVS_ERR_NO_MEM;
    }

    store->pt = *pt;
    dl_list_init(&store->group_list);

    /*sector manager load */
    err = wm_nvs_sm_load(&store->sm, &store->pt);
    if (err != WM_NVS_ERR_OK) {
        return err;
    }

    /*load groups */
    err = storage_load_groups(store);
    if (err != WM_NVS_ERR_OK) {
        return err;
    }

    /* check blob data*/
    err = storage_blob_check(store);
    if (err != WM_NVS_ERR_OK) {
        return err;
    }

    *storage = store;

    WM_NVS_LOGD("pt=%s", store->pt.name);

    return WM_NVS_ERR_OK;
}

int wm_nvs_storage_deinit(wm_nvs_storage_t *storage)
{
    wm_nvs_group_t *entry;
    wm_nvs_group_t *next;

    /*delete groups*/
    dl_list_for_each_safe(entry, next, &storage->group_list, wm_nvs_group_t, list)
    {
        WM_NVS_LOGD("del group %s", entry->name);

        dl_list_del(&entry->list);

        WM_NVS_FREE(entry);
    }

    /*unload*/
    wm_nvs_sm_unload(&storage->sm);

    return WM_NVS_ERR_OK;
}

int wm_nvs_storage_open_group(wm_nvs_storage_t *storage, const char *group, bool create_new, uint8_t *group_id)
{
    int err;

    /*check create before*/
    err = storage_find_group(storage, group, group_id);
    if (err == WM_NVS_ERR_OK) {
        /*found*/
        return WM_NVS_ERR_OK;
    }

    if (*group_id == WM_NVS_GROUP_ID_ANY) {
        return WM_NVS_ERR_NO_SPACE;
    }

    WM_NVS_LOGD("open group %s", group);

    /*create new group*/
    err = wm_nvs_storage_write_item(storage, WM_NVS_GROUP_ITSELF_ID, WM_NVS_TYPE_UINT8, group, group_id, sizeof(*group_id));
    if (err != WM_NVS_ERR_OK) {
        WM_NVS_LOGD("open group,write fail %s", group);
        return err;
    }

    err = storage_add_group(storage, *group_id, group, strnlen(group, WM_NVS_MAX_KEY_LEN));

    return err;
}

int wm_nvs_storage_erase_group_id(wm_nvs_storage_t *storage, uint8_t group_id)
{
    int err;
    wm_nvs_group_t *entry = NULL;
    wm_nvs_group_t *next;

    /* Look up group list */
    dl_list_for_each_safe(entry, next, &storage->group_list, wm_nvs_group_t, list)
    {
        if (entry->id == group_id) {
            err = wm_nvs_storage_del_item(storage, WM_NVS_GROUP_ITSELF_ID, WM_NVS_TYPE_UINT8, entry->name);
            if (err == WM_NVS_ERR_OK) {
                dl_list_del(&entry->list);
                WM_NVS_FREE(entry);
            }
            return err;
        }
    }

    return WM_NVS_ERR_OK;
}

wm_nvs_group_t *wm_nvs_storage_find_group_by_name(wm_nvs_storage_t *storage, const char *group_name)
{
    wm_nvs_group_t *entry = NULL;

    /* Look up group list */
    dl_list_for_each(entry, &storage->group_list, wm_nvs_group_t, list)
    {
        if (!strcmp(entry->name, group_name)) {
            return entry;
        }
    }

    return NULL;
}

wm_nvs_group_t *wm_nvs_storage_find_group_by_id(wm_nvs_storage_t *storage, uint8_t group_id)
{
    wm_nvs_group_t *entry = NULL;

    /* Look up group list */
    dl_list_for_each(entry, &storage->group_list, wm_nvs_group_t, list)
    {
        if (entry->id == group_id) {
            return entry;
        }
    }

    return NULL;
}

static int storage_get_non_blob_write_req_size(wm_nvs_type_t type, size_t size)
{
    int request_size = 0;

    if (type == WM_NVS_TYPE_STRING || type == WM_NVS_TYPE_BINARY) {
        if (size <= 8) {
            request_size = WM_NVS_SLICE_SIZE;
        } else {
            request_size = WM_NVS_SLICE_SIZE + size;
        }
    } else if (size < WM_NVS_SLICE_SIZE) {
        request_size = WM_NVS_SLICE_SIZE;
    } else {
        WM_NVS_LOGE("error!");
        request_size = WM_NVS_SLICE_SIZE;
    }

    return request_size;
}

static int storage_write_blob_desc(wm_nvs_sector_t *sec, uint8_t group_id, const char *key, size_t size, uint8_t seg_count,
                                   wm_nvs_seg_start_t seg_start)
{
    wm_nvs_item_t desc_item;

    desc_item.all_size  = size;
    desc_item.seg_count = seg_count;
    desc_item.seg_start = seg_start;
    desc_item.resv2     = 0xffff;

    WM_NVS_LOGD("write desc, key=%s,size=%u,seg_count=%d,seg_start=%d", key, size, seg_count, seg_start);

    return wm_nvs_sector_write_item(sec, group_id, WM_NVS_TYPE_BLOB, key, desc_item.data, sizeof(desc_item.data),
                                    WM_NVS_SEG_ID_ANY);
}

static int storage_cmp_blob(wm_nvs_storage_t *storage, wm_nvs_item_t *item, const void *data, size_t size)
{
    int seg_count = item->seg_count;
    int seg_start = item->seg_start;

    uint32_t offset = 0;
    wm_nvs_item_t seg;
    wm_nvs_sector_t *seg_sec;
    int seg_index;
    int i = 0;

    int err = WM_NVS_ERR_NOT_FOUND;

    uint8_t *pdata;

    /* check blob size */
    if (item->all_size != size) {
        WM_NVS_LOGD("blob old:new=%u:%u", item->all_size, size);
        return WM_NVS_ERR_FAIL;
    }

    pdata = WM_NVS_MALLOC(WM_NVS_SINGLE_ITEM_MAX_DATA_SIZE);
    if (!pdata) {
        return WM_NVS_ERR_NO_MEM;
    }

    for (i = 0; i < seg_count; i++) {
        seg_index = 0;
        seg_sec   = NULL;

        /*look for segments data */
        err = wm_nvs_sm_find_item(&storage->sm, item->group_id, WM_NVS_TYPE_BLOB_SEG, item->name, &seg_index, &seg_sec, &seg,
                                  seg_start + i, seg_start);
        if (err != WM_NVS_ERR_OK) {
            WM_NVS_LOGD("not found %d", i);
            break;
        }

        /*check segments data size */
        if (seg.length + offset > size) {
            WM_NVS_LOGD("size out %u,%u", seg.length + offset, size);
            break;
        }

        /*read blob segments data*/
        err = wm_nvs_sector_read_item_data(seg_sec, seg_index, &seg, pdata, seg.length);
        if (err != WM_NVS_ERR_OK) {
            break;
        }

        /*compare blob segments data*/
        if (memcmp(pdata, (uint8_t *)data + offset, seg.length)) {
            /*not same*/
            WM_NVS_LOGD("seg same err");
            break;
        }

        offset += seg.length;
    }

    WM_NVS_FREE(pdata);

    if (i == seg_count && offset == size) {
        /*all segements same*/
        return WM_NVS_ERR_OK;
    } else {
        /*not same*/
        WM_NVS_LOGD("blob cmp cnt=%u:%u,size=%u:%u", i, seg_count, offset, size);
        return WM_NVS_ERR_FAIL;
    }
}

static int storage_read_blob(wm_nvs_storage_t *storage, wm_nvs_item_t *item, void *data, size_t size)
{
    int seg_count = item->seg_count;
    int seg_start = item->seg_start;

    uint32_t offset = 0;
    wm_nvs_item_t seg;
    wm_nvs_sector_t *seg_sec;
    int seg_index;
    int i = 0;

    int err = WM_NVS_ERR_NOT_FOUND;

    for (i = 0; i < seg_count; i++) {
        seg_index = 0;
        seg_sec   = NULL;

        WM_NVS_LOGD("read seg: %d,[seg_index=%d,seg_start=%d]", i, seg_start + i, seg_start);

        /*look for segments*/
        err = wm_nvs_sm_find_item(&storage->sm, item->group_id, WM_NVS_TYPE_BLOB_SEG, item->name, &seg_index, &seg_sec, &seg,
                                  seg_start + i, seg_start);
        if (err != WM_NVS_ERR_OK) {
            WM_NVS_LOGD("find seg %d, err=%d", i, err);
            return err;
        }

        /*check segments length*/
        if (seg.length + offset > size) {
            WM_NVS_LOGD("bad length");
            return WM_NVS_ERR_INVALID_LENGTH;
        }

        WM_NVS_LOGD("start read seg %d, size=%d", i, seg.length);

        /*read segments data*/
        err = wm_nvs_sector_read_item_data(seg_sec, seg_index, &seg, (uint8_t *)data + offset, seg.length);
        if (err != WM_NVS_ERR_OK) {
            return err;
        }

        offset += seg.length;
    }

    WM_NVS_LOGD("i=%d,seg_count=%d,offset=%d,size=%d", i, seg_count, offset, size);

    if (i == seg_count && offset == size) {
        /*read all segments data ok*/
        return WM_NVS_ERR_OK;
    } else {
        if (offset > 0) {
            return WM_NVS_ERR_INVALID_LENGTH;
        } else {
            return WM_NVS_ERR_NOT_FOUND;
        }
    }
}

static int storage_erase_blob(wm_nvs_storage_t *storage, wm_nvs_sector_t *find_sector, int found_index, wm_nvs_item_t *item)
{
    int seg_count = item->seg_count;
    int seg_start = item->seg_start;

    wm_nvs_item_t seg;
    wm_nvs_sector_t *seg_sec;
    int seg_index;
    int i = 0;

    int err = WM_NVS_ERR_NOT_FOUND;

    /*delete blob descriptor*/
    WM_NVS_LOGD("erase desc %.*s:%d ", WM_NVS_MAX_KEY_LEN, item->name, i);
    err = wm_nvs_sector_erase_item(find_sector, found_index, item, true);

    WM_NVS_TRACE_POWER_OFF(WM_NVS_TRACE_TYPE_BLOB, WM_NVS_TRACE_AFTER_ERASE_OLD_DESC);

    for (i = 0; i < seg_count; i++) {
        seg_index = 0;
        seg_sec   = NULL;

        /*look for segments*/
        err = wm_nvs_sm_find_item(&storage->sm, item->group_id, WM_NVS_TYPE_BLOB_SEG, item->name, &seg_index, &seg_sec, &seg,
                                  seg_start + i, seg_start);
        if (err != WM_NVS_ERR_OK) {
            WM_NVS_LOGD("find erase seg %.*s:%d err", WM_NVS_MAX_KEY_LEN, item->name, i);
            break;
        }

        /*delete segments*/
        WM_NVS_LOGD("erase seg %.*s:%d ", WM_NVS_MAX_KEY_LEN, seg.name, i);
        wm_nvs_sector_erase_item(seg_sec, seg_index, &seg, true);
    }

    return err;
}

static int storage_write_blob(wm_nvs_storage_t *storage, uint8_t group_id, const char *key, const void *data, size_t dataSize,
                              wm_nvs_seg_start_t seg_start)
{
    int err = WM_NVS_ERR_OK;

    wm_nvs_sector_t *start_sec;
    wm_nvs_sector_t *sec;
    int free_size;

    int left_size = dataSize;
    int offset    = 0;
    int cur_seg_size;

    uint8_t seg_id = seg_start;

    /*get current sector*/
    sec = wm_nvs_sm_get_current_sector(&storage->sm);
    if (!sec) {
        WM_NVS_LOGE("no valid sector");
        return WM_NVS_ERR_FAIL;
    }

    if ((sec->droped_slice > 0 && left_size + WM_NVS_SLICE_SIZE > wm_nvs_sm_get_free_size(sec)) ||
        left_size > (WM_NVS_SEG_NUM_MAX - 1) * WM_NVS_SINGLE_ITEM_MAX_DATA_SIZE) {
        /*
        sector is dirty and need split: request a sector first, let the segments are written to the slimed secctors,
        so the free size and the gc size are same. then the check size and the write size are matched.
        */

        wm_nvs_sm_request_sector(&storage->sm, WM_NVS_SLICE_SIZE);
        sec = wm_nvs_sm_get_current_sector(&storage->sm);
    }

    start_sec = sec;

    while (1) {
        /*too many segments*/
        if (seg_id - seg_start >= WM_NVS_SEG_NUM_MAX && left_size > 0) {
            WM_NVS_LOGD("too many segs");
            break;
        }

        /* get free size for write */
        free_size = wm_nvs_sm_get_free_size(sec);

        WM_NVS_LOGD("free_size=%d,left=%d", free_size, left_size);

        if (left_size > 0) {
            /*check data write*/
            if (left_size + WM_NVS_SLICE_SIZE <= free_size) {
                /*can write all left data*/
                WM_NVS_LOGD("blob w: %d,left=%d", seg_id, left_size);
                err = wm_nvs_sector_write_item(sec, group_id, WM_NVS_TYPE_BLOB_SEG, key, (uint8_t *)data + offset, left_size,
                                               seg_id++);
                if (err != WM_NVS_ERR_OK) {
                    return err;
                }
                left_size = 0;

                if (sec->next_free_slice + 1 <= WM_NVS_ENTRY_COUNT) {
                    WM_NVS_LOGD("blob w: desc");

                    WM_NVS_TRACE_POWER_OFF(WM_NVS_TRACE_TYPE_BLOB, WM_NVS_TRACE_AFTER_WRITE_ALL_SEG);

                    return storage_write_blob_desc(sec, group_id, key, dataSize, seg_id - seg_start, seg_start);
                } else {
                    WM_NVS_LOGD("blob w: skip desc");
                    /*need write desc next loop*/
                }
            } else {
                /* free space is larger than the size of minimum */

                if (free_size >= WM_NVS_BLOB_MIN_SEG_SIZE + WM_NVS_SLICE_SIZE) {
                    cur_seg_size = free_size - WM_NVS_SLICE_SIZE;
                    /*write_segment*/
                    WM_NVS_LOGD("blob w: %d,left=%d", seg_id, cur_seg_size);
                    err = wm_nvs_sector_write_item(sec, group_id, WM_NVS_TYPE_BLOB_SEG, key, (uint8_t *)data + offset,
                                                   cur_seg_size, seg_id++);
                    if (err != WM_NVS_ERR_OK) {
                        return err;
                    }
                    offset += cur_seg_size;
                    left_size -= cur_seg_size;

                    WM_NVS_TRACE_POWER_OFF(WM_NVS_TRACE_TYPE_BLOB, WM_NVS_TRACE_AFTER_WRITE_A_SEG);

                } else {
                    /*skip curren sector, it too small*/
                    WM_NVS_LOGD("blob w: skip free_size=%d", free_size);
                }
            }

        } else if (free_size >= WM_NVS_SLICE_SIZE) {
            /*check write descriptor*/
            WM_NVS_LOGD("blob w: desc");

            WM_NVS_TRACE_POWER_OFF(WM_NVS_TRACE_TYPE_BLOB, WM_NVS_TRACE_AFTER_WRITE_ALL_SEG);

            return storage_write_blob_desc(sec, group_id, key, dataSize, seg_id - seg_start, seg_start);
        }

        WM_NVS_LOGD("blob write: request next sector, left=%d", left_size);

        /*request a sector*/
        err = wm_nvs_sm_request_sector(&storage->sm, WM_NVS_SLICE_SIZE);
        if (err != WM_NVS_ERR_OK) {
            WM_NVS_LOGD("blob w: req fail");
            return WM_NVS_ERR_NO_SPACE;
        }

        /*get current sector*/
        sec = wm_nvs_sm_get_current_sector(&storage->sm);
        if (!sec) {
            WM_NVS_LOGE("no valid sector");
            return WM_NVS_ERR_FAIL;
        }

        if (sec == start_sec) {
            WM_NVS_LOGD("Cycle to start point");
            return WM_NVS_ERR_NO_SPACE;
        }
    }

    return WM_NVS_ERR_NO_SPACE;
}

int wm_nvs_storage_write_item(wm_nvs_storage_t *storage, uint8_t group_id, wm_nvs_type_t type, const char *key,
                              const void *data, size_t size)
{
    int err;
    wm_nvs_sector_t *find_sector = NULL;
    wm_nvs_sector_t *cur_sector  = NULL;
    int found_item_index         = 0;
    int request_size;

    wm_nvs_item_t item;
    wm_nvs_seg_start_t seg_start = WM_NVS_SEG_START_VER_0;

    err = wm_nvs_sm_find_item(&storage->sm, group_id, WM_NVS_TYPE_ANY_WITHOUT_SEG, key, &found_item_index, &find_sector, &item,
                              WM_NVS_SEG_ID_ANY, WM_NVS_SEG_START_ANY);

    if (!(err == WM_NVS_ERR_OK || err == WM_NVS_ERR_NOT_FOUND)) {
        return err;
    }

    WM_NVS_LOGD("find %s err=%d", key, err);

    if (type == WM_NVS_TYPE_BLOB) {
        /*compare old blob*/
        if (find_sector && type == item.type) {
            err = storage_cmp_blob(storage, &item, data, size);
            if (err == WM_NVS_ERR_OK) {
                WM_NVS_LOGD("blob: found same, not write");
                return err;
            }

            /*find old blob*/
            if (item.seg_start == WM_NVS_SEG_START_VER_0) {
                seg_start = WM_NVS_SEG_START_VER_1;
            } else {
                seg_start = WM_NVS_SEG_START_VER_0;
            }

            WM_NVS_LOGD("blob write: cmp old,err=%d,old_start=%d,new_start=%d", err, item.seg_start, seg_start);
        }

        /*check free space is enough*/
        err = wm_nvs_sm_check_write_blob_size(&storage->sm, size);
        if (err != WM_NVS_ERR_OK) {
            WM_NVS_LOGE("blob write: fail,size=%u,err=%d", size, err);
            return err;
        }

        WM_NVS_LOGD("blob write: check space ok");

        /* write blob*/
        err = storage_write_blob(storage, group_id, key, data, size, seg_start);
        if (err != WM_NVS_ERR_OK) {
            WM_NVS_LOGD("write blob fail");
            return err;
        }

        WM_NVS_LOGD("blob write: OK");

    } else {
        /*same value data, not need write again*/
        if (find_sector && type == item.type) {
            err = storage_cmp_find_item(storage, find_sector, found_item_index, &item, data, size);
            if (err == WM_NVS_ERR_OK) {
                WM_NVS_LOGD("found same, do nothing");
                return err;
            }
            WM_NVS_LOGD("cmp %s err=%d", key, err);
        }

        cur_sector = wm_nvs_sm_get_current_sector(&storage->sm);
        if (!cur_sector) {
            WM_NVS_LOGE("no valid sector");
            return WM_NVS_ERR_OK;
        }

        err = wm_nvs_sector_write_item(cur_sector, group_id, type, key, data, size, WM_NVS_SEG_ID_ANY);
        if (err == WM_NVS_ERR_SECTOR_FULL) {
            /*full*/
            WM_NVS_LOGD("write cur sec full");

            request_size = storage_get_non_blob_write_req_size(type, size);

            /*request a sector*/
            err = wm_nvs_sm_request_sector(&storage->sm, request_size);
            if (err != WM_NVS_ERR_OK) {
                return err;
            }

            /*get current sector*/
            cur_sector = wm_nvs_sm_get_current_sector(&storage->sm);
            if (!cur_sector) {
                WM_NVS_LOGD("no valid sector");
                return WM_NVS_ERR_OK;
            }

            /*write item*/
            err = wm_nvs_sector_write_item(cur_sector, group_id, type, key, data, size, WM_NVS_SEG_ID_ANY);
            if (err != WM_NVS_ERR_OK) {
                WM_NVS_LOGD("write next sec, err=%d", err);
                return err;
            } else {
                WM_NVS_LOGD("write next sec ok");
            }

        } else if (err != WM_NVS_ERR_OK) {
            /*error*/
            WM_NVS_LOGD("write error, err=%d", err);
            return err;
        } else {
            /*OK*/
            WM_NVS_LOGD("write to cur sec ok");
        }
    }

    if (find_sector) {
        if (item.type == WM_NVS_TYPE_BLOB) {
            WM_NVS_TRACE_POWER_OFF(WM_NVS_TRACE_TYPE_BLOB, WM_NVS_TRACE_AFTER_MODIFY_NEW);

            /*erase old blob data*/
            err = storage_erase_blob(storage, find_sector, found_item_index, &item);

            WM_NVS_LOGD("blob erase old: err=%d", err);

        } else {
            /*erase old value*/
            WM_NVS_TRACE_POWER_OFF(WM_NVS_TRACE_TYPE_WRITE, WM_NVS_TRACE_BEFPRE_ERASE_OLD);

            err = wm_nvs_sector_erase_item(find_sector, found_item_index, &item, true);

            WM_NVS_LOGD("erase old type=%d: err=%d", item.type, err);
        }
    }

    return err;
}

int wm_nvs_storage_read_item(wm_nvs_storage_t *storage, uint8_t group_id, wm_nvs_type_t type, const char *key, void *data,
                             size_t *size)
{
    int err;
    wm_nvs_sector_t *find_sector = NULL;
    int found_item_index         = 0;
    size_t data_size;
    wm_nvs_item_t item;

    err = wm_nvs_sm_find_item(&storage->sm, group_id, type, key, &found_item_index, &find_sector, &item, WM_NVS_SEG_ID_ANY,
                              WM_NVS_SEG_START_ANY);
    if (err != WM_NVS_ERR_OK) {
        WM_NVS_LOGD("read %s not found", key);
        return err;
    }

    /*get data size*/
    data_size = (type == WM_NVS_TYPE_BLOB ? item.all_size : item.length);

    /*check read size*/
    if (*size < data_size) {
        *size = data_size;
        return WM_NVS_ERR_VALUE_TOO_LONG;
    }

    *size = data_size;

    if (type == WM_NVS_TYPE_BLOB) {
        WM_NVS_LOGD("start read blob:");

        /*read blob segments*/
        err = storage_read_blob(storage, &item, data, data_size);

        if (err == WM_NVS_ERR_NOT_FOUND || err == WM_NVS_ERR_INVALID_LENGTH) {
            /*read blob segments fail, delete item*/
            wm_nvs_sector_erase_item(find_sector, found_item_index, &item, true);
        }
    } else {
        /*read non blob items*/
        err = wm_nvs_sector_read_item_data(find_sector, found_item_index, &item, data, data_size);
    }

    return err;
}

int wm_nvs_storage_del_item(wm_nvs_storage_t *storage, uint8_t group_id, wm_nvs_type_t type, const char *key)
{
    int err;
    wm_nvs_sector_t *find_sector = NULL;
    int found_item_index         = 0;

    wm_nvs_item_t item;

    err = wm_nvs_sm_find_item(&storage->sm, group_id, type, key, &found_item_index, &find_sector, &item, WM_NVS_SEG_ID_ANY,
                              WM_NVS_SEG_START_ANY);
    if (err != WM_NVS_ERR_OK) {
        WM_NVS_LOGD("read %s not found", key);
        return err;
    }

    if (item.type == WM_NVS_TYPE_BLOB) {
        /* erase blob */
        err = storage_erase_blob(storage, find_sector, found_item_index, &item);
    } else {
        /* erase item */
        err = wm_nvs_sector_erase_item(find_sector, found_item_index, &item, true);
        WM_NVS_LOGD("del %s,err=%d", key, err);
    }

    return err;
}

int wm_nvs_storage_del_group(wm_nvs_storage_t *storage, uint8_t group_id)
{
    int err;
    wm_nvs_sector_t *entry = NULL;
    wm_nvs_sector_t *next;
    wm_nvs_item_t item;

    int start_index = 0;

    /* Look up group list */
    dl_list_for_each_safe(entry, next, &storage->sm.active, wm_nvs_sector_t, list)
    {
        start_index = 0;

        while (1) {
            err = wm_nvs_sector_find_item(entry, group_id, WM_NVS_TYPE_ANY_WITHOUT_SEG, NULL, &start_index, &item,
                                          WM_NVS_SEG_ID_ANY, WM_NVS_SEG_START_ANY);

            if (err == WM_NVS_ERR_NOT_FOUND) {
                /*go to next sector*/
                break;
            } else if (err != WM_NVS_ERR_OK) {
                return err;
            }

            /*del blob segment*/
            if (item.type == WM_NVS_TYPE_BLOB) {
                WM_NVS_LOGE("del blob item");
                err = storage_erase_blob(storage, entry, start_index, &item);

            } else {
                /*del normal item itself*/
                err = wm_nvs_sector_erase_item(entry, start_index, &item, true);
            }

            /*go to next item*/
            start_index += wm_nvs_item_get_span(&item);
        }
    }
    return WM_NVS_ERR_OK;
}

int wm_nvs_storage_find_key(wm_nvs_storage_t *storage, uint8_t group_id, const char *key, wm_nvs_item_t *item)
{
    int err;
    wm_nvs_sector_t *find_sector = NULL;
    int found_item_index         = 0;

    err = wm_nvs_sm_find_item(&storage->sm, group_id, WM_NVS_TYPE_ANY_WITHOUT_SEG, key, &found_item_index, &find_sector, item,
                              WM_NVS_SEG_ID_ANY, WM_NVS_SEG_START_ANY);
    if (err != WM_NVS_ERR_OK) {
        WM_NVS_LOGD("find %s not found", key);
        return err;
    }

    return err;
}


/**
 * @file wm_nvs_sector_manager.c
 *
 * @brief nvs sector manager module
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

#define LOG_TAG "nvs_sm"
#include "wm_nvs_porting.h"
#include "wm_nvs.h"
#include "wm_nvs_sector_manager.h"
#include "wm_nvs_debug.h"

static int sm_init_default(wm_nvs_sector_manager_t *sm, wm_nvs_partition_t *pt)
{
    sm->sec_arr = WM_NVS_CALLOC(1, pt->sec_num * sizeof(wm_nvs_sector_t));
    sm->pt      = pt;

    dl_list_init(&sm->active);
    dl_list_init(&sm->idle);

    return WM_NVS_ERR_OK;
}

static int sm_active_sector(wm_nvs_sector_manager_t *sm)
{
    int err;
    wm_nvs_sector_t *sec = NULL;

    sec = dl_list_first(&sm->idle, wm_nvs_sector_t, list);

    if (sec->state == WM_NVS_SECTOR_STATE_CRASH || sec->state == WM_NVS_SECTOR_STATE_INVALID) {
        err = wm_nvs_sector_erase(sec);
        if (err != WM_NVS_ERR_OK) {
            return err;
        }
    }

    sec->serial_number = sm->serial_number++;

    dl_list_del(&sec->list);
    dl_list_add_tail(&sm->active, &sec->list);

    return WM_NVS_ERR_OK;
}

static int sm_load_sectors(wm_nvs_sector_manager_t *sm, wm_nvs_partition_t *pt)
{
    int err = WM_NVS_ERR_OK;
    int i;

    wm_nvs_sector_state_t state;
    wm_nvs_sector_t *sec        = NULL;
    wm_nvs_sector_t *entry      = NULL;
    wm_nvs_sector_t *entry_next = NULL;
    int found;

    WM_NVS_LOGD("load sectors");

    for (i = 0; i < pt->sec_num; i++) {
        sec = &sm->sec_arr[i];

        err = wm_nvs_sector_load(pt, sec, i);
        if (err != WM_NVS_ERR_OK) {
            return err;
        }

        state = sec->state;

        if (state == WM_NVS_SECTOR_STATE_UNINIT || state == WM_NVS_SECTOR_STATE_CRASH || state == WM_NVS_SECTOR_STATE_INVALID) {
            /* invalid, unuse, crash sectors */
            WM_NVS_LOGD("add %d 0x%x, state=%x, add to idle", i, sec->address, state);
            dl_list_add_tail(&(sm->idle), &sec->list);
        } else {
            /* using, full, deleting sectors */
            /* look up the position */
            found = 0;
            dl_list_for_each_safe(entry, entry_next, &sm->active, wm_nvs_sector_t, list)
            {
                if (entry->serial_number > sec->serial_number) {
                    found = 1;
                    break;
                }
            }

            if (found && entry) {
                /*insert sector before entry, entry's SN is larger than current sector*/
                dl_list_add_tail(&entry->list, &sec->list);
                WM_NVS_LOGD("insert sec_id=%d,sn=%u, state=%x, to active", i, entry->serial_number, state);
            } else {
                /*add to the and of active list*/
                dl_list_add_tail(&sm->active, &sec->list);
                WM_NVS_LOGD("add sec_id=%d,sn=%u, state=%x, to active tail", i, entry->serial_number, state);
            }
        }
    }

    /* update global serial number */
    if (dl_list_empty(&sm->active)) {
        sm->serial_number = 1;
        err               = sm_active_sector(sm);
        return err;
    } else {
        entry             = dl_list_last(&sm->active, wm_nvs_sector_t, list);
        sm->serial_number = entry->serial_number + 1;
        wm_log_debug("last sn=%u", entry->serial_number);
    }

    dl_list_for_each(entry, &sm->active, wm_nvs_sector_t, list)
    {
        wm_log_debug("active sec_id=%d,sn=%u", entry->address / WM_NVS_SECTION_SIZE, entry->serial_number);
    }

    return err;
}

static int sm_check_item_data(wm_nvs_sector_t *sec, int index, wm_nvs_item_t *item)
{
    int err = WM_NVS_ERR_OK;

    /*check have additional data*/
    if ((item->type == WM_NVS_TYPE_STRING || item->type == WM_NVS_TYPE_BINARY || item->type == WM_NVS_TYPE_BLOB_SEG) &&
        item->length > 8) {
        uint8_t *p = WM_NVS_MALLOC(item->length);

        if (p) {
            /*read data*/
            err = wm_nvs_pt_read(sec->pt, sec->address + (index + 2) * WM_NVS_SLICE_SIZE, p, item->length);

            if (err == WM_NVS_ERR_OK) {
                /*check data crc*/
                uint32_t crc = wm_nvs_port_crc32(UINT32_MAX, p, item->length);
                if (crc != item->crc_data) {
                    WM_NVS_LOGW("crc:drop last item %d:%.*s", item->group_id, WM_NVS_MAX_KEY_LEN, item->name);

                    /*drop write imcomplete item*/
                    wm_nvs_sector_erase_item(sec, index, item, true);

                    err = WM_NVS_ERR_FAIL;
                }
            }

            WM_NVS_FREE(p);
        }
    }

    return err;
}

static int sm_check_imcomplete_write(wm_nvs_sector_manager_t *sm)
{
    int err;

    bool found     = false;
    int last_index = 0;
    int next_index = 0;
    wm_nvs_seg_start_t seg_start;
    uint8_t seg_id;
    wm_nvs_item_t item;

    wm_nvs_sector_t *last = dl_list_last(&sm->active, wm_nvs_sector_t, list);

    if (!last) {
        return WM_NVS_ERR_FAIL;
    }

    WM_NVS_LOGD("power off imcomplete_write check");

    /*find last item*/
    while (1) {
        err = wm_nvs_sector_find_item(last, WM_NVS_GROUP_ID_ANY, WM_NVS_TYPE_ANY, NULL, &next_index, &item, WM_NVS_SEG_ID_ANY,
                                      WM_NVS_SEG_START_ANY);
        if (err != WM_NVS_ERR_OK) {
            break;
        } else {
            found      = true;
            last_index = next_index;
            next_index += wm_nvs_item_get_span(&item);
        }
    }

    if (found) {
        wm_nvs_sector_t *entry = NULL;
        wm_nvs_item_t old;
        int old_index;

        /*check last item data*/
        WM_NVS_LOGD("check last data");

        if (sm_check_item_data(last, last_index, &item) != WM_NVS_ERR_OK) {
            /*The last item is not write OK, droped.*/
            return WM_NVS_ERR_OK;
        }

        WM_NVS_LOGD("last:sec_index=%d,gid=%d,type=%d,name=%.*s,len=%d,seg_id=%d,seg_start=%d",
                    last->address / WM_NVS_SECTION_SIZE, item.group_id, item.type, WM_NVS_MAX_KEY_LEN, item.name, item.length,
                    item.seg_id, item.seg_start);

        seg_id    = (item.type == WM_NVS_TYPE_BLOB_SEG ? item.seg_id : WM_NVS_SEG_ID_ANY);
        seg_start = WM_NVS_SEG_START_ANY;

        /*find old items*/
        dl_list_for_each(entry, &sm->active, wm_nvs_sector_t, list)
        {
            old_index = 0;

            if (entry != last) {
                /*check other sector*/
                err = wm_nvs_sector_find_item(entry, item.group_id, item.type, item.name, &old_index, &old, seg_id, seg_start);
                if (err == WM_NVS_ERR_OK) {
                    WM_NVS_LOGD("found double in sec 0x%x,del old", entry->address);
                    wm_nvs_sector_erase_item(entry, old_index, &old, true);
                    break;
                }
            } else {
                /*check the last sector*/
                err = wm_nvs_sector_find_item(entry, item.group_id, item.type, item.name, &old_index, &old, seg_id, seg_start);
                if (err == WM_NVS_ERR_OK && old_index != last_index) {
                    WM_NVS_LOGD("found double in last sector,del old");
                    wm_nvs_sector_erase_item(entry, old_index, &old, true);
                    break;
                } else {
                    WM_NVS_LOGD("not found. old_index=%d,last=%d", old_index, last_index);
                }
            }
        }
    }

    return WM_NVS_ERR_OK;
}

static int sm_check_imcomplete_gc(wm_nvs_sector_manager_t *sm)
{
    int err;
    wm_nvs_sector_t *it    = NULL;
    wm_nvs_sector_t *entry = NULL;

    wm_nvs_sector_t *last;
    wm_nvs_sector_t *new_sec;

    WM_NVS_LOGD("power off GC check");

    dl_list_for_each(entry, &sm->active, wm_nvs_sector_t, list)
    {
        if (entry->state == WM_NVS_SECTOR_STATE_DELETTING) {
            /*found the deleting sector*/
            it = entry;
            WM_NVS_LOGD("it=0x%x", it->address);
            break;
        }
    }

    if (!it) {
        /*No deleting sector*/
        return WM_NVS_ERR_OK;
    }

    last = dl_list_last(&sm->active, wm_nvs_sector_t, list);

    if (last->state == WM_NVS_SECTOR_STATE_USING) {
        /* STEP0 : erase not complete sector*/

        WM_NVS_LOGD("GC-0: erase the last");
        err = wm_nvs_sector_erase(last);
        if (err != WM_NVS_ERR_OK) {
            return err;
        }

        /*add the last to idle*/
        dl_list_del(&last->list);
        dl_list_add_tail(&sm->idle, &last->list);
    }

    /*make a new sector to the active list end*/
    err = sm_active_sector(sm);
    if (err != WM_NVS_ERR_OK) {
        return err;
    }

    /* STEP1 : Set new sector status to using*/
    WM_NVS_LOGD("GC-1:set new using");
    new_sec = dl_list_last(&sm->active, wm_nvs_sector_t, list);
    if (new_sec->state == WM_NVS_SECTOR_STATE_UNINIT) {
        /*write sector header*/
        err = wm_nvs_sector_init(new_sec);
        if (err != WM_NVS_ERR_OK) {
            return err;
        }
    }
    /* STEP2 : Set dirst sector status to deleting, it seted before*/

    /* STEP3 : Copy dirtiest sector to the GC sector*/
    WM_NVS_LOGD("GC-3:copy");
    err = wm_nvs_sector_copy(new_sec, it);
    if (err != WM_NVS_ERR_OK) {
        return err;
    }

    WM_NVS_LOGD("GC-4:erase old");

    /* STEP4 : erase the dirtiest secto*/
    err = wm_nvs_sector_erase(it);
    if (err != WM_NVS_ERR_OK) {
        return err;
    }

    /*remove the dirtiest sector from active*/
    dl_list_del(&it->list);

    /*Add the dirtiest sector to idle list*/
    dl_list_add_tail(&sm->idle, &it->list);

    return WM_NVS_ERR_OK;
}

static int sm_garbage_collection(wm_nvs_sector_manager_t *sm, int need_size)
{
    int err;
    wm_nvs_sector_t *entry = NULL;
    wm_nvs_sector_t *entry_next;
    wm_nvs_sector_t *dirtiest = NULL;

    wm_nvs_sector_t *new_sec;

    int can_get_size;
    int most_dirty_size = 0;

    dl_list_for_each_safe(entry, entry_next, &sm->active, wm_nvs_sector_t, list)
    {
        can_get_size = wm_nvs_sm_get_gc_size(entry);

        if (can_get_size > most_dirty_size) {
            most_dirty_size = can_get_size;
            dirtiest        = entry;
        }
    }

    if (most_dirty_size >= need_size) {
        WM_NVS_LOGD("GC:get dirtiest=0x%x,size=%d", dirtiest->address, most_dirty_size);
        WM_NVS_LOGD("GC:active new sector");

        /*prepare the GC sector*/
        err = sm_active_sector(sm);
        if (err != WM_NVS_ERR_OK) {
            return err;
        }

        /* STEP1 : Set new sector status to using*/
        WM_NVS_LOGD("GC-1:set new using");
        new_sec = dl_list_last(&sm->active, wm_nvs_sector_t, list);
        if (new_sec->state == WM_NVS_SECTOR_STATE_UNINIT) {
            /*write sector header*/
            err = wm_nvs_sector_init(new_sec);
            if (err != WM_NVS_ERR_OK) {
                return err;
            }
        }

        WM_NVS_TRACE_POWER_OFF(WM_NVS_TRACE_TYPE_GC, WM_NVS_TRACE_GC_1_NEW_SECTOR);

        /* STEP2 : Set dirst sector status to deleting*/
        WM_NVS_LOGD("GC-2:set deletting");
        err = wm_nvs_sector_set_state(dirtiest, WM_NVS_SECTOR_STATE_DELETTING);
        if (err != WM_NVS_ERR_OK) {
            return err;
        }

        WM_NVS_TRACE_POWER_OFF(WM_NVS_TRACE_TYPE_GC, WM_NVS_TRACE_GC_2_SET_OLD_DELETING);

        /* STEP3 : Copy dirtiest sector to the GC sector*/
        WM_NVS_LOGD("GC-3:copy");
        err = wm_nvs_sector_copy(new_sec, dirtiest);
        if (err != WM_NVS_ERR_OK) {
            return err;
        }

        WM_NVS_LOGD("GC-4:erase old");

        WM_NVS_TRACE_POWER_OFF(WM_NVS_TRACE_TYPE_GC, WM_NVS_TRACE_GC_3_COPY);

        /* STEP4 : erase the dirtiest secto*/
        err = wm_nvs_sector_erase(dirtiest);
        if (err != WM_NVS_ERR_OK) {
            return err;
        }

        WM_NVS_TRACE_POWER_OFF(WM_NVS_TRACE_TYPE_GC, WM_NVS_TRACE_GC_4_ERASE_OLD);

        /*remove the dirtiest sector from active*/
        dl_list_del(&dirtiest->list);

        /*Add the dirtiest sector to idle list*/
        dl_list_add_tail(&sm->idle, &dirtiest->list);

        WM_NVS_LOGD("GC:ok");

        return WM_NVS_ERR_OK;
    } else {
        WM_NVS_LOGE("GC not do, need %d,only %d", need_size, most_dirty_size);
        return WM_NVS_ERR_NO_SPACE;
    }
}

int wm_nvs_sm_load(wm_nvs_sector_manager_t *storage_manager, wm_nvs_partition_t *pt)
{
    int err;
    wm_nvs_sector_manager_t *sm = storage_manager;
    wm_nvs_sector_t *sec;

    /*init sector manager*/
    err = sm_init_default(sm, pt);
    if (err != WM_NVS_ERR_OK) {
        return WM_NVS_ERR_NO_MEM;
    }

    /*load sectors and update global sn to last + 1*/
    err = sm_load_sectors(sm, pt);
    if (err != WM_NVS_ERR_OK) {
        return err;
    }

    /* 1 : Check the last item wrote OK
       2 : Check whether the old data was deleted last time due to power failure
    */

    sm_check_imcomplete_write(sm);

    /* Check whether the old GC page was deleted due to power failure*/

    sm_check_imcomplete_gc(sm);

    /* Check GC sector exist*/
    if (!pt->readonly && dl_list_empty(&sm->idle)) {
        /* The last sector use to GC but not do, roll back */
        WM_NVS_LOGE("move last active to idle");

        sec = dl_list_last(&sm->active, wm_nvs_sector_t, list);
        dl_list_del(&sec->list);
        dl_list_add_tail(&sm->idle, &sec->list);
    }

    wm_log_debug("sec load end\n");

    return WM_NVS_ERR_OK;
}

int wm_nvs_sm_unload(wm_nvs_sector_manager_t *sm)
{
    wm_nvs_sector_t *sec;

    WM_NVS_LOGD("unload sectors in %s", sm->pt->name);

    /*remove hash table for each sector*/
    dl_list_for_each(sec, &sm->active, wm_nvs_sector_t, list)
    {
        wm_nvs_hash_clear(&sec->hash);
    }

    /*free sector array*/
    WM_NVS_FREE(sm->sec_arr);

    return WM_NVS_ERR_OK;
}

int wm_nvs_sm_request_sector(wm_nvs_sector_manager_t *sm, int need_size)
{
    int err;
    int num = dl_list_len(&sm->idle);

    if (num == 0) {
        WM_NVS_LOGE("no idle sector");
        return WM_NVS_ERR_NO_MEM;
    }

    if (num == 1) {
        /*no enough idle sector now , do GC*/
        err = sm_garbage_collection(sm, need_size);
    } else if (num > 1) {
        /*no enough idle sector now , do GC*/
        err = sm_active_sector(sm);
    } else {
        WM_NVS_LOGD("no idle sector");
        err = WM_NVS_ERR_FAIL;
    }

    return err;
}

int wm_nvs_sm_find_item(wm_nvs_sector_manager_t *sm, uint8_t group_id, wm_nvs_type_t type, const char *key, int *item_index,
                        wm_nvs_sector_t **sector, wm_nvs_item_t *item, uint8_t seg_index, wm_nvs_seg_start_t seg_start)
{
    int err;
    wm_nvs_sector_t *entry = NULL;

    /* Look up active sector list */
    dl_list_for_each(entry, &sm->active, wm_nvs_sector_t, list)
    {
        err = wm_nvs_sector_find_item(entry, group_id, type, key, item_index, item, seg_index, seg_start);
        if (err == WM_NVS_ERR_OK) {
            *sector = entry;
            return WM_NVS_ERR_OK;
        }
    }

    return WM_NVS_ERR_NOT_FOUND;
}

int wm_nvs_sm_check_write_blob_size(wm_nvs_sector_manager_t *sm, size_t size)
{
    wm_nvs_sector_t *entry = NULL;

    int left_size      = size;
    int idle_num       = dl_list_len(&sm->idle);
    int max_write_size = (sm->pt->sec_num - 1) * WM_NVS_SINGLE_ITEM_MAX_DATA_SIZE;
    int free_size;
    int cur_seg_size;
    int seg_count = 0;

    /*check all partition size*/
    if (left_size > max_write_size) {
        return WM_NVS_ERR_NO_SPACE;
    }

    if (left_size > WM_NVS_SEG_NUM_MAX * WM_NVS_SINGLE_ITEM_MAX_DATA_SIZE) {
        return WM_NVS_ERR_INVALID_LENGTH;
    }

    /*check idle size*/
    left_size -= (idle_num - 1) * WM_NVS_SINGLE_ITEM_MAX_DATA_SIZE;

    /*-WM_NVS_SLICE_SIZE for addtional desc size*/
    if (left_size <= -WM_NVS_SLICE_SIZE) {
        return WM_NVS_ERR_OK;
    }

    /* check active sectors*/
    dl_list_for_each(entry, &sm->active, wm_nvs_sector_t, list)
    {
        free_size = wm_nvs_sm_get_gc_size(entry);

        if (left_size > 0 && seg_count >= WM_NVS_SEG_NUM_MAX) {
            WM_NVS_LOGD("up to max segments");
            break;
        }
        if (left_size > 0) {
            /*check data write*/
            if (left_size + WM_NVS_SLICE_SIZE <= free_size) {
                /*can write all left data*/
                seg_count++;
                left_size = 0;

                if (entry->next_free_slice + 1 <= WM_NVS_ENTRY_COUNT) {
                    return WM_NVS_ERR_OK;
                } else {
                    /*need write desc next loop*/
                }
            } else {
                /* free space is larger than the size of minimum */

                if (free_size >= WM_NVS_BLOB_MIN_SEG_SIZE + WM_NVS_SLICE_SIZE) {
                    cur_seg_size = free_size - WM_NVS_SLICE_SIZE;

                    left_size -= cur_seg_size;
                    seg_count++;
                } else {
                    /*skip curren sector, it too small*/
                }
            }

        } else if (free_size >= WM_NVS_SLICE_SIZE) {
            /*check write descriptor*/
            return WM_NVS_ERR_OK;
        }
    }

    return WM_NVS_ERR_NO_SPACE;
}

int wm_nvs_sm_get_status(wm_nvs_sector_manager_t *sm, wm_nvs_status_t *status)
{
    wm_nvs_sector_t *entry = NULL;

    uint32_t used_slice   = 0;
    uint32_t droped_slice = 0;

    dl_list_for_each(entry, &sm->active, wm_nvs_sector_t, list)
    {
        /*add sector header to statistics*/
        used_slice += entry->used_slice + 1;
        droped_slice += entry->droped_slice;
    }

    status->total_size  = sm->pt->sec_size * sm->pt->sec_num;
    status->using_size  = used_slice * WM_NVS_SLICE_SIZE;
    status->droped_size = droped_slice * WM_NVS_SLICE_SIZE;
    status->free_size   = status->total_size - status->using_size;

    return WM_NVS_ERR_OK;
}

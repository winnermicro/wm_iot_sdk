
/**
 * @file wm_nvs_sector.c
 *
 * @brief nvs sector module
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

#define LOG_TAG "nvs_sec"
#include "wm_nvs_porting.h"
#include "wm_nvs.h"
#include "wm_nvs_sector_manager.h"
#include "wm_nvs_sector.h"
#include "wm_nvs_debug.h"

#define WM_NVS_SECTOR_CRC_LEN 24

static uint32_t sector_crc32(wm_nvs_sector_header_t *header)
{
    return wm_nvs_port_crc32(UINT32_MAX, &header->serial_number, WM_NVS_SECTOR_CRC_LEN);
}

int wm_nvs_sector_set_state(wm_nvs_sector_t *sec, wm_nvs_sector_state_t state)
{
    sec->state = state;
    return wm_nvs_pt_write_raw(sec->pt, sec->address + WM_NVS_SECTOR_STATE_OFF_SET, &sec->state, sizeof(sec->state));
}

int wm_nvs_sector_erase(wm_nvs_sector_t *sec)
{
    int err;

    wm_nvs_sector_set_state(sec, WM_NVS_SECTOR_STATE_CRASH);

    /*erase sector*/
    err = wm_nvs_pt_erase(sec->pt, sec->address);
    if (err != WM_NVS_ERR_OK) {
        return err;
    }

    /*reset sector attibute*/
    sec->state   = WM_NVS_SECTOR_STATE_UNINIT;
    sec->version = CONFIG_NVS_VER_NUM;

    sec->next_free_slice = 0;
    sec->used_slice      = 0;
    sec->droped_slice    = 0;

    wm_nvs_hash_clear(&sec->hash);

    return WM_NVS_ERR_OK;
}

static uint32_t sector_get_next_address(wm_nvs_sector_t *sec)
{
    return sec->address + WM_NVS_ENTRY_DATA_OFFSET + sec->next_free_slice * WM_NVS_SLICE_SIZE;
}

int wm_nvs_sector_init(wm_nvs_sector_t *sec)
{
    wm_nvs_sector_header_t header;

    sec->state   = WM_NVS_SECTOR_STATE_USING;
    sec->version = CONFIG_NVS_VER_NUM;

    sec->next_free_slice = 0;
    sec->used_slice      = 0;
    sec->droped_slice    = 0;

    memset(&header, 0xff, sizeof(header));

    header.magic         = WM_NVS_SECTOR_MAGIC;
    header.state         = WM_NVS_SECTOR_STATE_USING;
    header.version       = CONFIG_NVS_VER_NUM;
    header.serial_number = sec->serial_number;
    header.crc32         = sector_crc32(&header);

    return wm_nvs_pt_write_raw(sec->pt, sec->address, &header, sizeof(header));
}

/*
    update the follow 3 attribute and hash list
    sec->next_free_slice
    sec->used_slice
    sec->droped_slice
*/
static int sector_update_slice_and_hash(wm_nvs_sector_t *sec)
{
    wm_nvs_item_t item;
    uint32_t offset;
    int err;
    int i;

    WM_NVS_LOGD("before update == %d %d %d", sec->next_free_slice, sec->used_slice, sec->droped_slice);

    /*point to first item, skip sector header */
    offset = sec->address + WM_NVS_SLICE_SIZE;

    for (i = 0; i < WM_NVS_ENTRY_COUNT;) {
        err = wm_nvs_pt_read_raw(sec->pt, offset, &item, sizeof(item));
        if (err != WM_NVS_ERR_OK) {
            sec->state = WM_NVS_SECTOR_STATE_INVALID;
            WM_NVS_LOGD("read fail:sec=%d,offset=0x%x", i, sec->address);
            return err;
        }

        if (item.state == WM_NVS_ITEM_STATE_UNUSED) {
            /*to last slice*/
            WM_NVS_LOGD("end:sec=%d,offset=0x%x", i, sec->address);
            sec->next_free_slice = i;
            break;
        } else {
            int span = wm_nvs_item_get_span(&item);

            if (item.state == WM_NVS_ITEM_STATE_USING) {
                if (item.crc_item == wm_nvs_item_crc_head(&item)) {
                    wm_nvs_hash_append(&sec->hash, &item, i);

                    /*using slice*/
                    sec->used_slice += span;
                    WM_NVS_LOGD("add using %.*s", WM_NVS_MAX_KEY_LEN, item.name);

                } else {
                    /*crc fail: drop the item not write done, droped_slice will change in function*/
                    sec->used_slice += span;
                    sec->droped_slice += span;

                    WM_NVS_LOGD("crc fail: drop %.*s", WM_NVS_MAX_KEY_LEN, item.name);
                    err = wm_nvs_sector_erase_item(sec, (uint8_t)i, &item, false);
                    if (err != WM_NVS_ERR_OK) {
                        sec->state = WM_NVS_SECTOR_STATE_INVALID;
                        return err;
                    }
                }

            } else if (item.state == WM_NVS_ITEM_STATE_DROPED) {
                /*droped slice*/
                WM_NVS_LOGD("droped entry, item=%.*s,span=%d", WM_NVS_MAX_KEY_LEN, item.name, span);
                sec->droped_slice += span;
                sec->used_slice += span;
            }

            i += span;
            offset += span * WM_NVS_SLICE_SIZE;

            WM_NVS_LOGD("loop i=%d,offset=0x%x,%d", i, offset, offset);
        }
    }

    if (i >= WM_NVS_ENTRY_COUNT) {
        /*to the sector end*/
        sec->next_free_slice = WM_NVS_ENTRY_COUNT;
    }

    WM_NVS_LOGD("end update == %d %d %d", sec->next_free_slice, sec->used_slice, sec->droped_slice);

    return WM_NVS_ERR_OK;
}

static int sector_check_empty(wm_nvs_partition_t *pt, wm_nvs_sector_t *sec, wm_nvs_sector_header_t *header)
{
    int err          = WM_NVS_ERR_FAIL;
    uint32_t *pblock = NULL;
    uint32_t *p      = NULL;
    uint32_t *pend   = NULL;

    sec->state = header->state;

    pblock = WM_NVS_MALLOC(pt->sec_size);
    if (!pblock) {
        WM_NVS_LOGD("no men");
        return WM_NVS_ERR_NO_MEM;
    }

    /*read sector*/
    err = wm_nvs_pt_read_raw(pt, sec->address, pblock, pt->sec_size);
    if (err != WM_NVS_ERR_OK) {
        sec->state = WM_NVS_SECTOR_STATE_INVALID;
        WM_NVS_LOGD("check read raw fail");
    } else {
        p    = pblock;
        pend = (uint32_t *)(((char *)pblock) + pt->sec_size);

        /*check sector is empty*/
        while (p < pend) {
            if (*p != 0xffffffff) {
                sec->state = WM_NVS_SECTOR_STATE_CRASH;
                WM_NVS_LOGD("check crash");
                break;
            }
            p++;
        }
    }

    WM_NVS_FREE(pblock);

    return err;
}

/*index not include section header*/
int wm_nvs_sector_erase_item(wm_nvs_sector_t *sec, int index, wm_nvs_item_t *item, bool erase_hash)
{
    int err;

    uint32_t offset = sec->address + (index + 1) * WM_NVS_SLICE_SIZE;
    uint8_t state   = WM_NVS_ITEM_STATE_DROPED;
    int span        = wm_nvs_item_get_span(item);

    WM_NVS_LOGD("erase %.*s,span=%d", WM_NVS_MAX_KEY_LEN, item->name, span);
    sec->droped_slice += span;

    err = wm_nvs_pt_write_raw(sec->pt, offset, &state, sizeof(state));

    if (erase_hash) {
        wm_nvs_hash_erase(&sec->hash, index);
    }

    return err;
}

int wm_nvs_sector_load(wm_nvs_partition_t *pt, wm_nvs_sector_t *sec, int sec_index)
{
    int err;
    wm_nvs_sector_header_t header;

    sec->address      = sec_index * pt->sec_size;
    sec->used_slice   = 0;
    sec->droped_slice = 0;
    sec->pt           = pt;

    wm_nvs_hash_init(&sec->hash);

    err = wm_nvs_pt_read_raw(pt, sec->address, &header, sizeof(header));
    if (err != WM_NVS_ERR_OK) {
        sec->state = WM_NVS_SECTOR_STATE_INVALID;
        WM_NVS_LOGI("read raw error");
        return err;
    }

    sec->state = header.state;

    if (header.state == WM_NVS_SECTOR_STATE_UNINIT) {
        /* check empty sector */
        err = sector_check_empty(pt, sec, &header);
        if (err != WM_NVS_ERR_OK) {
            WM_NVS_LOGI("check fail %d", sec_index);
            return err;
        }
    } else if (header.magic != WM_NVS_SECTOR_MAGIC || header.crc32 != sector_crc32(&header)) {
        /* bad sector */
        sec->state = WM_NVS_SECTOR_STATE_CRASH;
        WM_NVS_LOGD("check crash %d", sec_index);
    } else if (header.version != CONFIG_NVS_VER_NUM) {
        /* nvs version changed, erase old version data default */
        sec->state = WM_NVS_SECTOR_STATE_CRASH;
        WM_NVS_LOGW("new version [%d,%d], del %d", header.version, CONFIG_NVS_VER_NUM, sec_index);
    } else {
        /* good sector */
        sec->serial_number = header.serial_number;
        sec->state         = header.state;
        WM_NVS_LOGD("check %d ok", sec_index);
    }

    if (sec->state == WM_NVS_SECTOR_STATE_FULL || sec->state == WM_NVS_SECTOR_STATE_USING ||
        sec->state == WM_NVS_SECTOR_STATE_DELETTING) {
        return sector_update_slice_and_hash(sec);
    }

    WM_NVS_LOGD("load %d, state=%x,header.state=%x", sec_index, sec->state, header.state);

    return WM_NVS_ERR_OK;
}

int wm_nvs_sector_write_item_data(wm_nvs_sector_t *sec, wm_nvs_item_t *item, const void *extra_data, size_t len, int entry_cnt)
{
    uint32_t offset = sector_get_next_address(sec);

    wm_nvs_pt_write_item(sec->pt, offset, item);

    WM_NVS_TRACE_POWER_OFF(WM_NVS_TRACE_TYPE_WRITE, WM_NVS_TRACE_BEFORE_WRITE_DATA);

    if (extra_data) {
        wm_nvs_pt_write(sec->pt, offset + WM_NVS_SLICE_SIZE, extra_data, len);
    }

    sec->next_free_slice += entry_cnt;
    sec->used_slice += entry_cnt;

    return WM_ERR_SUCCESS;
}

int wm_nvs_sector_read_item_data(wm_nvs_sector_t *sec, int found_slice_index, wm_nvs_item_t *item, void *data, size_t size)
{
    if (size <= 8) {
        memcpy(data, item->data, size);
        return WM_NVS_ERR_OK;
    } else {
        /*skip sector header and item itself*/
        uint32_t offset = sec->address + (found_slice_index + 1 + 1) * WM_NVS_SLICE_SIZE;

        return wm_nvs_pt_read(sec->pt, offset, data, size);
    }
}

int wm_nvs_sector_write_item(wm_nvs_sector_t *sec, uint8_t gid, wm_nvs_type_t type, const char *key, const void *data,
                             size_t size, uint8_t seg_id)
{
    int err;
    wm_nvs_item_t item;
    int write_cntry;

    if (sec->state == WM_NVS_SECTOR_STATE_INVALID) {
        WM_NVS_LOGD("w:bad state");
        return WM_NVS_ERR_FAIL;
    }

    if (sec->state == WM_NVS_SECTOR_STATE_FULL) {
        WM_NVS_LOGD("w:full");
        return WM_NVS_ERR_SECTOR_FULL;
    }

    if (sec->state == WM_NVS_SECTOR_STATE_UNINIT) {
        err = wm_nvs_sector_init(sec);
        if (err != WM_NVS_ERR_OK) {
            WM_NVS_LOGD("w:sec init err");
            return err;
        }
    }

    if (size > WM_NVS_SINGLE_ITEM_MAX_DATA_SIZE) {
        WM_NVS_LOGD("w:bad size=%d", size);
        return WM_NVS_ERR_VALUE_TOO_LONG;
    }

    size_t totalSize = WM_NVS_SLICE_SIZE;
    size_t entry_cnt = 1;

    /*calculate use entrys*/
    if (size > 8) {
        size_t roundedSize = (size + WM_NVS_SLICE_SIZE - 1) & (~(WM_NVS_SLICE_SIZE - 1));
        totalSize += roundedSize;
        entry_cnt += roundedSize / WM_NVS_SLICE_SIZE;
    }

    if (sec->next_free_slice + entry_cnt > WM_NVS_ENTRY_COUNT) {
        /*data size out of sector free size*/
        WM_NVS_LOGD("w:bad cnt,free=%d,entry_cnt=%d", sec->next_free_slice, entry_cnt);
        return WM_NVS_ERR_SECTOR_FULL;
    }

    wm_nvs_item_init(&item, WM_NVS_ITEM_STATE_USING, gid, type, key, data, size, seg_id);

    write_cntry = sec->next_free_slice;

    if (size > 8) {
        /*multi entry item, write item head and data*/
        err = wm_nvs_sector_write_item_data(sec, &item, data, size, entry_cnt);

    } else {
        /*one entry item*/
        err = wm_nvs_sector_write_item_data(sec, &item, NULL, 0, entry_cnt);
    }

    /*write OK, add to hash table*/
    if (err == WM_NVS_ERR_OK) {
        wm_nvs_hash_append(&sec->hash, &item, write_cntry);
    }

    WM_NVS_LOGD("write 0x%x | gid=%d,type=%d,key=%.*s,size=%d,err=%d", sec->address, item.group_id, item.type,
                WM_NVS_MAX_KEY_LEN, item.name, item.length, err);

    if (err == WM_NVS_ERR_SECTOR_FULL && sec->state != WM_NVS_SECTOR_STATE_FULL) {
        wm_nvs_sector_set_state(sec, WM_NVS_SECTOR_STATE_FULL);
    }

    return err;
}

int wm_nvs_sector_find_item(wm_nvs_sector_t *sec, uint8_t group_id, wm_nvs_type_t type, const char *key, int *item_index,
                            wm_nvs_item_t *item, uint8_t seg_index, wm_nvs_seg_start_t seg_start)
{
    size_t start = *item_index;
    size_t end   = sec->next_free_slice;

    uint32_t offset;
    int slice_index;
    int span;
    int err;

    WM_NVS_LOGD("sec find: gid=%d,type=%d,key=%s,index=%d, seg=%d,%d", group_id, type, (key ? key : "null"), *item_index,
                seg_index, seg_start);

    if (sec->state == WM_NVS_SECTOR_STATE_CRASH || sec->state == WM_NVS_SECTOR_STATE_INVALID ||
        sec->state == WM_NVS_SECTOR_STATE_UNINIT) {
        WM_NVS_LOGD("find: bad state");
        return WM_NVS_ERR_NOT_FOUND;
    }

    if (start < 0 || start >= WM_NVS_ENTRY_COUNT) {
        WM_NVS_LOGD("find err: start=%d", start);
        return WM_NVS_ERR_NOT_FOUND;
    }

    if (end > WM_NVS_ENTRY_COUNT) {
        end = WM_NVS_ENTRY_COUNT;
    }

    WM_NVS_LOGD("read item start =%d,end=%d", start, end);

    while (start < end) {
        if (key) {
            /*The key is valid, so the hash value can be calculated and compared*/

            wm_nvs_item_init(item, WM_NVS_ITEM_STATE_USING, group_id, type, key, NULL, 0, seg_index);

            WM_NVS_LOGD("start =%d", start);

            slice_index = wm_nvs_hash_find(&sec->hash, start, item);
            if (slice_index < 0) {
                return WM_NVS_ERR_NOT_FOUND;
            }

            /*found hash match*/
            start = slice_index;
            WM_NVS_LOGD("found start =%d", start);
        }

        /*point to first item, skip sector header */
        offset = sec->address + (start + 1) * WM_NVS_SLICE_SIZE;

        WM_NVS_LOGD("read item start =%d,offset=%d", start, offset);

        err = wm_nvs_pt_read_item(sec->pt, offset, item);
        if (err != WM_NVS_ERR_OK) {
            sec->state = WM_NVS_SECTOR_STATE_INVALID;
            return err;
        }

        span = wm_nvs_item_get_span(item);

        WM_NVS_LOGD("found start =%d,span=%d", start, span);

        if (item->state == WM_NVS_ITEM_STATE_DROPED) {
            /*droped*/
            start += span;

            WM_NVS_LOGD("found drop");

        } else if (item->state == WM_NVS_ITEM_STATE_USING) {
            /*using, check item match*/
            if (item->state == WM_NVS_ITEM_STATE_USING && (group_id == WM_NVS_GROUP_ID_ANY || group_id == item->group_id) &&
                ((type == WM_NVS_TYPE_ANY || type == item->type) ||
                 (type == WM_NVS_TYPE_ANY_WITHOUT_SEG && item->type != WM_NVS_TYPE_BLOB_SEG)) &&
                (seg_index == WM_NVS_SEG_ID_ANY || seg_index == item->seg_id) &&
                (seg_start == WM_NVS_SEG_START_ANY || (item->seg_id >= seg_start && item->seg_id - seg_start < 0x80)) &&
                (!key || !strncmp(item->name, key, strlen(key)))) {
                *item_index = start;
                WM_NVS_LOGD("found match,start=%d,type=%d,item.type=%d", start, type, item->type);

                return WM_NVS_ERR_OK;

            } else {
                if (type == WM_NVS_TYPE_BLOB_SEG && item->type == WM_NVS_TYPE_BLOB_SEG) {
                    WM_NVS_LOGD("for: group_id=%d,type=%d,seg_index=%d,seg_start=%d,key=%s", group_id, type, seg_index,
                                seg_start, key ? key : "NULL");
                    WM_NVS_LOGD("it: group_id=%d,type=%d,seg_index=%d,seg_start=%d,key=%.*s", item->group_id, item->type,
                                item->seg_id, item->seg_start, WM_NVS_MAX_KEY_LEN, item->name);
                } else if (type == WM_NVS_TYPE_BLOB && item->type == WM_NVS_TYPE_BLOB) {
                    WM_NVS_LOGD("for: group_id=%d,type=%d,seg_index=%d,seg_start=%d,key=%s", group_id, type, seg_index,
                                seg_start, key ? key : "NULL");
                    WM_NVS_LOGD("it: group_id=%d,type=%d,seg_index=%d,seg_start=%d,key=%.*s", item->group_id, item->type,
                                item->seg_id, item->seg_start, WM_NVS_MAX_KEY_LEN, item->name);
                }
                start += span;
            }
        } else {
            /*unusing*/
            WM_NVS_LOGD("not found ");
            return WM_NVS_ERR_NOT_FOUND;
        }
    }

    return WM_NVS_ERR_NOT_FOUND;
}

int wm_nvs_sector_copy(wm_nvs_sector_t *dst, wm_nvs_sector_t *src)
{
    int err;

    wm_nvs_item_t item;
    int span;

    uint32_t src_index = 0;
    uint32_t dst_index = 0;

    while (src_index < WM_NVS_ENTRY_COUNT) {
        /*read item form source sector*/
        err = wm_nvs_pt_read_item(src->pt, src->address + (src_index + 1) * WM_NVS_SLICE_SIZE, &item);
        if (err != WM_NVS_ERR_OK) {
            src->state = WM_NVS_SECTOR_STATE_INVALID;
            return err;
        }

        /*get item span*/

        span = wm_nvs_item_get_span(&item);
        if (item.state == WM_NVS_ITEM_STATE_DROPED) {
            /*droped item, skip it*/
            src_index += span;

            WM_NVS_LOGD("found drop");

        } else if (item.state == WM_NVS_ITEM_STATE_USING) {
            /*using item, start copy*/

            /*write item to the dest sector*/
            err = wm_nvs_pt_write_item(dst->pt, dst->address + (dst_index + 1) * WM_NVS_SLICE_SIZE, &item);
            if (err != WM_NVS_ERR_OK) {
                dst->state = WM_NVS_SECTOR_STATE_INVALID;
                return err;
            }

            /*need copy item data*/
            if (span > 1) {
                /*malloc memory for item data*/
                int data_size = item.length;
                uint8_t *p    = WM_NVS_MALLOC(data_size);
                if (!p) {
                    return WM_NVS_ERR_NO_MEM;
                }

                /*read item data from the source sector*/
                err = wm_nvs_pt_read(src->pt, src->address + (src_index + 2) * WM_NVS_SLICE_SIZE, p, data_size);
                if (err != WM_NVS_ERR_OK) {
                    src->state = WM_NVS_SECTOR_STATE_INVALID;
                    WM_NVS_FREE(p);
                    return err;
                }

                /*write item data to the dest sector*/
                err = wm_nvs_pt_write(dst->pt, dst->address + (dst_index + 2) * WM_NVS_SLICE_SIZE, p, data_size);
                if (err != WM_NVS_ERR_OK) {
                    dst->state = WM_NVS_SECTOR_STATE_INVALID;
                    WM_NVS_FREE(p);
                    return err;
                }

                WM_NVS_FREE(p);
            }

            /*update dst sector info*/
            wm_nvs_hash_append(&dst->hash, &item, dst_index);
            dst->used_slice += span;
            dst->next_free_slice += span;

            /*loop next item*/
            src_index += span;
            dst_index += span;

        } else {
            /*unusing, copy end*/
            dst->next_free_slice = dst->used_slice;
            WM_NVS_LOGD("copy to end,src_index=0x%x,dst_used=%d,dst_next_free=%d", src_index, dst->used_slice,
                        dst->next_free_slice);
            break;
        }
    }

    WM_NVS_LOGD("copy end");

    return WM_NVS_ERR_OK;
}

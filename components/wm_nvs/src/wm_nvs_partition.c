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

#define LOG_TAG "nvs_pt"
#include "wm_nvs_porting.h"
#include "wm_partition_table.h"
#include "wm_dt.h"
#include "wm_drv_flash.h"
#include "wm_nvs_partition.h"
#include "wm_nvs.h"

int wm_nvs_pt_init(const char *partition_name, wm_nvs_partition_t *pt)
{
    wm_partition_item_t ptable;
    wm_device_t *dev;

    if (wm_partition_table_find(partition_name, &ptable) != WM_NVS_ERR_OK) {
        WM_NVS_LOGE("read pt %s fail", partition_name);
        return WM_NVS_ERR_FAIL;
    }

    dev = wm_dt_get_device_by_name(WM_NVS_FLASH_DEVICE);
    if (!dev) {
        WM_NVS_LOGE("get flash dev %s fail", WM_NVS_FLASH_DEVICE);
        return WM_NVS_ERR_FAIL;
    }

    pt->dev = dev;
    snprintf(pt->name, sizeof(pt->name), "%s", partition_name);
    pt->offset    = ptable.offset;
    pt->sec_size  = WM_NVS_SECTION_SIZE;
    pt->sec_num   = ptable.size / pt->sec_size;
    pt->size      = ptable.size;
    pt->encrypted = 0;
    pt->readonly  = 0;

    WM_NVS_LOGD("pt init, pt=%p,name=%s,offset=0x%x,size=0x%x,sec_num=%d", pt, partition_name, pt->offset, pt->sec_size,
                pt->sec_num);

    if ((pt->sec_num < 2 && !pt->readonly) || pt->sec_num < 1) {
        return WM_NVS_ERR_FAIL;
    } else {
        return WM_NVS_ERR_OK;
    }
}

int wm_nvs_pt_erase_all(const char *partition_name)
{
    wm_partition_item_t ptable;
    wm_device_t *dev;

    if (wm_partition_table_find(partition_name, &ptable) != WM_NVS_ERR_OK) {
        WM_NVS_LOGE("read pt %s fail", partition_name);
        return WM_NVS_ERR_FAIL;
    }

    dev = wm_dt_get_device_by_name(WM_NVS_FLASH_DEVICE);
    if (!dev) {
        WM_NVS_LOGE("get flash dev %s fail", WM_NVS_FLASH_DEVICE);
        return WM_NVS_ERR_FAIL;
    }

    WM_NVS_LOGW("erase %s", partition_name);

    return wm_drv_flash_erase_region(dev, ptable.offset, ptable.size);
}

int wm_nvs_pt_erase(wm_nvs_partition_t *pt, uint32_t address)
{
    WM_NVS_LOGD("erase %s, 0x%x", pt->name, address);

    if (pt->readonly) {
        return WM_NVS_ERR_READ_ONLY;
    }

    if (!(address >= 0 && address + WM_NVS_SECTION_SIZE <= pt->size)) {
        return WM_NVS_ERR_INVALID_PARAM;
    }

    return wm_drv_flash_erase_sector(pt->dev, (pt->offset + address) / WM_NVS_SECTION_SIZE, 1);
}

int wm_nvs_pt_read(wm_nvs_partition_t *pt, uint32_t address, void *data, uint32_t length)
{
    WM_NVS_LOGD("read %s, off=0x%x,len=%u", pt->name, address, length);

    if (!(address >= 0 && address + length <= pt->size)) {
        return WM_NVS_ERR_INVALID_PARAM;
    }

    return wm_drv_flash_read(pt->dev, pt->offset + address, data, length);
}

int wm_nvs_pt_write(wm_nvs_partition_t *pt, uint32_t address, const void *data, uint32_t length)
{
    WM_NVS_LOGD("write %s, off=0x%x,len=%u", pt->name, address, length);

    if (!(address >= 0 && address + length <= pt->size)) {
        return WM_NVS_ERR_INVALID_PARAM;
    }

    if (pt->readonly) {
        return WM_NVS_ERR_READ_ONLY;
    }

    return wm_drv_flash_write(pt->dev, pt->offset + address, (uint8_t *)data, length);
}

int wm_nvs_pt_read_raw(wm_nvs_partition_t *pt, uint32_t address, void *data, uint32_t length)
{
    WM_NVS_LOGV("read raw %s, off=0x%x,len=%u", pt->name, address, length);

    if (!(address >= 0 && address + length <= pt->size)) {
        return WM_NVS_ERR_INVALID_PARAM;
    }

    return wm_drv_flash_read(pt->dev, pt->offset + address, data, length);
}

int wm_nvs_pt_write_raw(wm_nvs_partition_t *pt, uint32_t address, const void *data, uint32_t length)
{
    WM_NVS_LOGD("write raw %s, off=0x%x,len=%u", pt->name, address, length);

    if (!(address >= 0 && address + length <= pt->size)) {
        return WM_NVS_ERR_INVALID_PARAM;
    }

    if (pt->readonly) {
        return WM_NVS_ERR_READ_ONLY;
    }

    return wm_drv_flash_write(pt->dev, pt->offset + address, (uint8_t *)data, length);
}

int wm_nvs_pt_read_item(wm_nvs_partition_t *pt, uint32_t address, wm_nvs_item_t *item)
{
    WM_NVS_LOGV("read item %s, off=0x%x,len=%u", pt->name, address, sizeof(*item));

    if (!(address >= 0 && address + sizeof(*item) <= pt->size)) {
        return WM_NVS_ERR_INVALID_PARAM;
    }

    return wm_drv_flash_read(pt->dev, pt->offset + address, (uint8_t *)item, sizeof(*item));
}

int wm_nvs_pt_write_item(wm_nvs_partition_t *pt, uint32_t address, const wm_nvs_item_t *item)
{
    WM_NVS_LOGD("write item %s, off=0x%x,len=%u", pt->name, address, sizeof(*item));

    if (!(address >= 0 && address + sizeof(*item) <= pt->size)) {
        return WM_NVS_ERR_INVALID_PARAM;
    }

    if (pt->readonly) {
        return WM_NVS_ERR_READ_ONLY;
    }

    return wm_drv_flash_write(pt->dev, pt->offset + address, (uint8_t *)item, sizeof(*item));
}

/**
 * @file wm_drv_ops_internal_flash.c
 *
 * @brief internal flash ops Driver Module
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
#include "wm_types.h"
#include "wm_osal.h"
#include "wm_drv_flash.h"
#include "wm_dt_hw.h"
#include "wm_drv_flash_ops.h"

#undef LOG_TAG
#define LOG_TAG "ifls"
#include "wm_log.h"

#define WM_FT_REGION 0x2000 //ref: wm_partition_table_print

typedef struct {
    wm_os_mutex_t *mutex;
    wm_hal_flash_dev_t hal_dev;
} wm_drv_iflash_ctx_t;

static int wm_drv_internal_flash_write_with_erase(wm_device_t *dev, uint32_t offset, uint8_t *in_buf, uint32_t in_buf_len)
{
    uint8_t *tmp_buf    = NULL;
    uint32_t secotr_idx = 0, sector_offset = 0, remain_size = 0;
    uint32_t flash_addr = offset, write_len = in_buf_len;
    uint8_t *src_buf             = in_buf;
    uint32_t i                   = 0;
    wm_hal_flash_dev_t *hal_dev  = NULL;
    wm_drv_iflash_ctx_t *drv_ctx = NULL;

    if (!dev || !in_buf || !dev->drv || !in_buf_len) {
        return WM_ERR_INVALID_PARAM;
    }

    drv_ctx = (wm_drv_iflash_ctx_t *)dev->drv;
    hal_dev = &drv_ctx->hal_dev;

    if ((offset + in_buf_len) > hal_dev->device_info.size) {
        wm_log_error("write offset:%u, in_len:%u, flash size:%u\n", offset, in_buf_len, hal_dev->device_info.size);
        return WM_ERR_INVALID_PARAM;
    }

    if (offset <= WM_FT_REGION) {
        wm_log_error("cannot write or erase FT region: 0 ~ 0x%x\n", WM_FT_REGION);
        return WM_ERR_INVALID_PARAM;
    }

    wm_os_internal_mutex_acquire(drv_ctx->mutex, WM_OS_WAIT_TIME_MAX);
    tmp_buf = (uint8_t *)wm_os_internal_malloc(hal_dev->device_info.sector_size);
    if (tmp_buf == NULL) {
        wm_log_error("allocate sector cache memory fail!\n");
        wm_os_internal_mutex_release(drv_ctx->mutex);
        return WM_ERR_NO_MEM;
    }

    secotr_idx    = flash_addr / hal_dev->device_info.sector_size;    //Section addr
    sector_offset = flash_addr % hal_dev->device_info.sector_size;    //Offset in section
    remain_size   = hal_dev->device_info.sector_size - sector_offset; // remain size in sector
    if (write_len <= remain_size)
        remain_size = write_len; //Not bigger with remain size in section

    while (1) {
        if (remain_size != hal_dev->device_info.sector_size) { //will keep some region value
            wm_hal_flash_read_pages(hal_dev, secotr_idx * hal_dev->device_info.sector_size, tmp_buf,
                                    hal_dev->device_info.sector_size / hal_dev->device_info.page_size);
        }

        //step1: erase current sector
        wm_hal_flash_erase_sectors(hal_dev, secotr_idx * hal_dev->device_info.sector_size, 1);

        //step2: fill write value in current sector
        for (i = 0; i < remain_size; i++) {
            tmp_buf[i + sector_offset] = src_buf[i];
        }

        //step3: write current sector
        wm_hal_flash_write_pages(hal_dev, secotr_idx * hal_dev->device_info.sector_size, tmp_buf,
                                 hal_dev->device_info.sector_size / hal_dev->device_info.page_size);

        //setp4: check current sector whether enough to fill write data
        if (write_len == remain_size) {
            break;        //write done
        } else {          //will write anthoer sector
            secotr_idx++; // secotr index + 1
            sector_offset = 0;
            src_buf += remain_size;   // secremain is current write data length
            write_len -= remain_size; // the legth will write to sector
            if (write_len > (hal_dev->device_info.sector_size)) {
                remain_size = hal_dev->device_info.sector_size; // next sector still not enough fill the write value
            } else {
                if (write_len == 0)
                    break;
                remain_size = write_len; //Next section will finish
            }
        }
    }

    wm_os_internal_free(tmp_buf);
    wm_os_internal_mutex_release(drv_ctx->mutex);

    return WM_ERR_SUCCESS;
}

//internal use, for FT
int wm_drv_iflash_wr_with_erase_private(wm_device_t *dev, uint32_t offset, uint8_t *in_buf, uint32_t in_buf_len)
{
    uint8_t *tmp_buf    = NULL;
    uint32_t secotr_idx = 0, sector_offset = 0, remain_size = 0;
    uint32_t flash_addr = offset, write_len = in_buf_len;
    uint8_t *src_buf             = in_buf;
    uint32_t i                   = 0;
    wm_hal_flash_dev_t *hal_dev  = NULL;
    wm_drv_iflash_ctx_t *drv_ctx = NULL;

    if (!dev || !in_buf || !dev->drv || !in_buf_len) {
        return WM_ERR_INVALID_PARAM;
    }

    drv_ctx = (wm_drv_iflash_ctx_t *)dev->drv;
    hal_dev = &drv_ctx->hal_dev;

    if ((offset + in_buf_len) > hal_dev->device_info.size) {
        wm_log_error("write offset:%u, in_len:%u, flash size:%u\n", offset, in_buf_len, hal_dev->device_info.size);
        return WM_ERR_INVALID_PARAM;
    }

    wm_os_internal_mutex_acquire(drv_ctx->mutex, WM_OS_WAIT_TIME_MAX);
    tmp_buf = (uint8_t *)wm_os_internal_malloc(hal_dev->device_info.sector_size);
    if (tmp_buf == NULL) {
        wm_log_error("allocate sector cache memory fail!\n");
        wm_os_internal_mutex_release(drv_ctx->mutex);
        return WM_ERR_NO_MEM;
    }

    secotr_idx    = flash_addr / hal_dev->device_info.sector_size;    //Section addr
    sector_offset = flash_addr % hal_dev->device_info.sector_size;    //Offset in section
    remain_size   = hal_dev->device_info.sector_size - sector_offset; // remain size in sector
    if (write_len <= remain_size)
        remain_size = write_len; //Not bigger with remain size in section

    while (1) {
        if (remain_size != hal_dev->device_info.sector_size) { //will keep some region value
            wm_hal_flash_read_pages(hal_dev, secotr_idx * hal_dev->device_info.sector_size, tmp_buf,
                                    hal_dev->device_info.sector_size / hal_dev->device_info.page_size);
        }

        //step1: erase current sector
        wm_hal_flash_erase_sectors(hal_dev, secotr_idx * hal_dev->device_info.sector_size, 1);

        //step2: fill write value in current sector
        for (i = 0; i < remain_size; i++) {
            tmp_buf[i + sector_offset] = src_buf[i];
        }

        //step3: write current sector
        wm_hal_flash_write_pages(hal_dev, secotr_idx * hal_dev->device_info.sector_size, tmp_buf,
                                 hal_dev->device_info.sector_size / hal_dev->device_info.page_size);

        //setp4: check current sector whether enough to fill write data
        if (write_len == remain_size) {
            break;        //write done
        } else {          //will write anthoer sector
            secotr_idx++; // secotr index + 1
            sector_offset = 0;
            src_buf += remain_size;   // secremain is current write data length
            write_len -= remain_size; // the legth will write to sector
            if (write_len > (hal_dev->device_info.sector_size)) {
                remain_size = hal_dev->device_info.sector_size; // next sector still not enough fill the write value
            } else {
                if (write_len == 0)
                    break;
                remain_size = write_len; //Next section will finish
            }
        }
    }

    wm_os_internal_free(tmp_buf);
    wm_os_internal_mutex_release(drv_ctx->mutex);

    return WM_ERR_SUCCESS;
}

//write by page
static int wm_drv_internal_flash_write(wm_device_t *dev, uint32_t offset, uint8_t *in_buf, uint32_t in_buf_len)
{
    uint8_t *tmp_buf  = NULL;
    uint32_t page_idx = 0, page_offset = 0, remain_size = 0;
    uint32_t flash_addr = offset, write_len = in_buf_len;
    uint8_t *src_buf             = in_buf;
    uint32_t i                   = 0;
    wm_hal_flash_dev_t *hal_dev  = NULL;
    wm_drv_iflash_ctx_t *drv_ctx = NULL;

    if (!dev || !in_buf || !dev->drv || !in_buf_len) {
        return WM_ERR_INVALID_PARAM;
    }

    drv_ctx = (wm_drv_iflash_ctx_t *)dev->drv;
    hal_dev = &drv_ctx->hal_dev;

    if ((offset + in_buf_len) > hal_dev->device_info.size) {
        wm_log_error("write offset:%u, in_len:%u, flash size:%u\n", offset, in_buf_len, hal_dev->device_info.size);
        return WM_ERR_INVALID_PARAM;
    }

    if (offset <= WM_FT_REGION) {
        wm_log_error("cannot write or erase FT region: 0 ~ 0x%x\n", WM_FT_REGION);
        return WM_ERR_INVALID_PARAM;
    }

    wm_os_internal_mutex_acquire(drv_ctx->mutex, WM_OS_WAIT_TIME_MAX);

    tmp_buf = (uint8_t *)wm_os_internal_malloc(hal_dev->device_info.page_size);
    if (tmp_buf == NULL) {
        wm_log_error("allocate sector cache memory fail!\n");
        wm_os_internal_mutex_release(drv_ctx->mutex);
        return WM_ERR_NO_MEM;
    }
    page_idx    = flash_addr / hal_dev->device_info.page_size;  //page addr
    page_offset = flash_addr % hal_dev->device_info.page_size;  //Offset in page
    remain_size = hal_dev->device_info.page_size - page_offset; // remain size in page
    if (write_len <= remain_size)
        remain_size = write_len; //Not bigger with remain size in page

    while (1) {
        if (remain_size !=
            hal_dev->device_info.page_size) { //offset != 0, and  not write whole page, will keep some region value
            wm_hal_flash_read_pages(hal_dev, page_idx * hal_dev->device_info.page_size, tmp_buf, 1);
        }

        //step1: fill write value in current page
        for (i = 0; i < remain_size; i++) {
            tmp_buf[i + page_offset] = src_buf[i];
        }

        //step2: write current page
        wm_hal_flash_write_pages(hal_dev, page_idx * hal_dev->device_info.page_size, tmp_buf, 1);

        //setp3: check current page whether enough to fill write data
        if (write_len == remain_size) {
            break;      //write done
        } else {        //will write anthoer sector
            page_idx++; // secotr index + 1
            page_offset = 0;
            src_buf += remain_size;   // secremain is current write data length
            write_len -= remain_size; // the legth will write to page
            if (write_len > (hal_dev->device_info.page_size)) {
                remain_size = hal_dev->device_info.page_size; // next page still not enough fill the write value
            } else {
                if (write_len == 0)
                    break;
                remain_size = write_len; //Next section will finish
            }
        }
    }

    wm_os_internal_free(tmp_buf);
    wm_os_internal_mutex_release(drv_ctx->mutex);

    return WM_ERR_SUCCESS;
}

static int wm_drv_internal_flash_read(wm_device_t *dev, uint32_t offset, uint8_t *out_buf, uint32_t out_len)
{
    uint8_t *tmp_buf    = NULL;
    uint8_t *read_buf   = out_buf;
    uint32_t flash_addr = offset, read_len = out_len;
    uint32_t page_idx = 0, page_offset = 0, remain_size = 0;
    wm_hal_flash_dev_t *hal_dev  = NULL;
    wm_drv_iflash_ctx_t *drv_ctx = NULL;

    if (!dev || !out_buf || !dev->drv || !out_len) {
        return WM_ERR_INVALID_PARAM;
    }

    drv_ctx = (wm_drv_iflash_ctx_t *)dev->drv;
    hal_dev = &drv_ctx->hal_dev;

    if ((offset + out_len) > hal_dev->device_info.size) {
        wm_log_error("read offset:%u, out_len:%u, flash size:%u\n", offset, out_len, hal_dev->device_info.size);
        return WM_ERR_INVALID_PARAM;
    }

    wm_os_internal_mutex_acquire(drv_ctx->mutex, WM_OS_WAIT_TIME_MAX);

    tmp_buf = (uint8_t *)wm_os_internal_malloc(hal_dev->device_info.page_size);
    if (tmp_buf == NULL) {
        wm_log_error("allocate page memory fail!\n");
        wm_os_internal_mutex_release(drv_ctx->mutex);
        return WM_ERR_NO_MEM;
    }

    page_idx    = flash_addr / hal_dev->device_info.page_size;  //Section addr
    page_offset = flash_addr % hal_dev->device_info.page_size;  //Offset in section
    remain_size = hal_dev->device_info.page_size - page_offset; // remain size in sector

    if (read_len <= remain_size) {
        remain_size = read_len;
    }

    while (1) {
        wm_hal_flash_read_pages(hal_dev, page_idx * hal_dev->device_info.page_size, tmp_buf, 1);
        memcpy(read_buf, tmp_buf + page_offset, remain_size);

        if (read_len == remain_size) {
            break;      //read done
        } else {        //will write anthoer sector
            page_idx++; // secotr index + 1
            page_offset = 0;
            read_buf += remain_size; // secremain is current write data length
            read_len -= remain_size; // the legth will write to sector

            if (read_len > (hal_dev->device_info.page_size)) {
                remain_size = hal_dev->device_info.page_size;
            } else {
                if (read_len == 0) {
                    break;
                }
                remain_size = read_len; //Next section will finish
            }
        }
    }

    wm_os_internal_free(tmp_buf);
    wm_os_internal_mutex_release(drv_ctx->mutex);

    return WM_ERR_SUCCESS;
}

static int wm_drv_internal_flash_get_device_info(wm_device_t *dev, wm_drv_flash_info_t *flash_info)
{
    wm_hal_flash_dev_t *hal_dev = NULL;

    wm_drv_iflash_ctx_t *drv_ctx = NULL;

    if (!dev || !dev->drv || !flash_info) {
        return WM_ERR_INVALID_PARAM;
    }

    drv_ctx = (wm_drv_iflash_ctx_t *)dev->drv;
    hal_dev = &drv_ctx->hal_dev;

    wm_os_internal_mutex_acquire(drv_ctx->mutex, WM_OS_WAIT_TIME_MAX);
    flash_info->page_size   = hal_dev->device_info.page_size;
    flash_info->sector_size = hal_dev->device_info.sector_size;
    flash_info->flash_size  = hal_dev->device_info.size;
    flash_info->vendor_id   = hal_dev->device_info.vendor_id;
    memcpy(&flash_info->unique_id, &hal_dev->device_info.unique_id, FLASH_UNIQUE_ID_SIZE);

    wm_os_internal_mutex_release(drv_ctx->mutex);

    return WM_ERR_SUCCESS;
}

static int wm_drv_internal_flash_erase_region(wm_device_t *dev, uint32_t offset, uint32_t erase_len)
{
    uint8_t *tmp_buf       = NULL;
    uint32_t secotr_offset = 0, left_erase_len = erase_len, secotr_start_idx = 0, erase_len_offset = 0;
    uint32_t flash_addr          = offset;
    wm_hal_flash_dev_t *hal_dev  = NULL;
    wm_drv_iflash_ctx_t *drv_ctx = NULL;
    int ret                      = WM_ERR_SUCCESS;

    if (!dev || !dev->drv || !erase_len) {
        return WM_ERR_INVALID_PARAM;
    }

    drv_ctx = (wm_drv_iflash_ctx_t *)dev->drv;
    hal_dev = &drv_ctx->hal_dev;

    if ((offset + erase_len) > hal_dev->device_info.size) {
        wm_log_error("erase offset:%u, out_len:%u, flash size:%u\n", offset, erase_len, hal_dev->device_info.size);
        return WM_ERR_INVALID_PARAM;
    }

    if (offset <= WM_FT_REGION) {
        wm_log_error("cannot write or erase FT region: 0 ~ 0x%x\n", WM_FT_REGION);
        return WM_ERR_INVALID_PARAM;
    }

    wm_os_internal_mutex_acquire(drv_ctx->mutex, WM_OS_WAIT_TIME_MAX);

    tmp_buf = (uint8_t *)wm_os_internal_malloc(hal_dev->device_info.sector_size);
    if (tmp_buf == NULL) {
        wm_log_error("allocate sector tmp_buf memory fail!\n");
        wm_os_internal_mutex_release(drv_ctx->mutex);
        return WM_ERR_NO_MEM;
    }

    /********************************************************
    **
    ** x: is erase region
    **
    ** case1:  |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|___________________________________|______________________
    **      sector1       secotr_offset              sector2                            sector3
    **
    ** case2:  |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|xxxxxxxx___________________________|______________________
    **      sector1       secotr_offset              sector2  erase_len_offset           sector3
    **
    ** case3:  |__________xxxxxxxxxxxxxxxxxxxx_______|___________________________________|______________________
    **      sector1       secotr_offset              sector2                            sector3
    **
    ** case4:  |__________xxxxxxxxxxxxxxxxxxxxxxxxxxx|xxxxxxxxxxxxx_____________________|______________________
    **      sector1       secotr_offset              sector2      erase_len_offset       sector3
    **
    ** case5:  |__________xxxxxxxxxxxxxxxxxxxxxxxxxxx|xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|xxxxxxxxxxxxx__________
    **      sector1       secotr_offset              sector2                            sector3       erase_len_offset
    **
    **
    ***********************************************************/
    secotr_start_idx = flash_addr / hal_dev->device_info.sector_size;
    secotr_offset    = flash_addr % hal_dev->device_info.sector_size;
    //erase_len_offset = erase_len % hal_dev->device_info.sector_size;
    erase_len_offset = (erase_len + secotr_offset) % hal_dev->device_info.sector_size;

    do {
        /** case1:  |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|___________________________________|______________________
        **      secotr_start_idx                    sector2                              sector3
        **/
        if (!secotr_offset && !erase_len_offset) {
            wm_hal_flash_erase_sectors(hal_dev, secotr_start_idx * hal_dev->device_info.sector_size,
                                       erase_len / hal_dev->device_info.sector_size);
            break;
        }

        /** case2:  |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|xxxxxx________________________________|______________________
        **      secotr_start_idx                     sector2    erase_len_offset                          sector3
        **/
        if (!secotr_offset && erase_len_offset) {
            uint32_t ers_sector_num      = erase_len / hal_dev->device_info.sector_size + 1;
            uint32_t recovery_start_addr = 0;
            //step1: save last sector data
            wm_hal_flash_read_sectors(hal_dev, (secotr_start_idx + (ers_sector_num - 1)) * hal_dev->device_info.sector_size,
                                      tmp_buf, 1);

            //step2 erase sector
            wm_hal_flash_erase_sectors(hal_dev, secotr_start_idx * hal_dev->device_info.sector_size, ers_sector_num);

            //handle the last sector
            recovery_start_addr = (secotr_start_idx + ers_sector_num - 1) * hal_dev->device_info.sector_size;
            for (int i = 0; i < erase_len_offset; i++) {
                tmp_buf[i] = 0xFF;
            }
            wm_hal_flash_write_pages(hal_dev, recovery_start_addr, tmp_buf,
                                     hal_dev->device_info.sector_size / hal_dev->device_info.page_size);

            break;
        }

        /** case3:  |__________xxxxxxxxxxxxxxxxxxxx_______|___________________________________|______________________
        **      sector1       secotr_offset              sector2                            sector3
        **/
        if (secotr_offset && ((secotr_offset + erase_len) <= hal_dev->device_info.sector_size)) {
            wm_hal_flash_read_sectors(hal_dev, secotr_start_idx * hal_dev->device_info.sector_size, tmp_buf, 1);
            for (int i = secotr_offset; i < (secotr_offset + erase_len); i++) {
                tmp_buf[i] = 0xFF;
            }
            wm_hal_flash_erase_sectors(hal_dev, secotr_start_idx * hal_dev->device_info.sector_size, 1);
            wm_hal_flash_write_pages(hal_dev, secotr_start_idx * hal_dev->device_info.sector_size, tmp_buf,
                                     hal_dev->device_info.sector_size / hal_dev->device_info.page_size);

            break;
        }

        /** case4:  |__________xxxxxxxxxxxxxxxxxxxxxxxxxxx|xxxxxxxxxxxxx_____________________|______________________
        **      sector1       secotr_offset              sector2     erase_len_offset         sector3
        **/
        if (secotr_offset && ((secotr_offset + erase_len) > hal_dev->device_info.sector_size)) {
            uint32_t fist_page_addr = 0, last_page_addr = 0;
            //step1: read the first erase sector
            fist_page_addr = secotr_start_idx * hal_dev->device_info.sector_size;
            wm_hal_flash_read_sectors(hal_dev, fist_page_addr, tmp_buf, 1);
            wm_hal_flash_erase_sectors(hal_dev, fist_page_addr, 1);
            for (int i = secotr_offset; i < hal_dev->device_info.sector_size; i++) {
                tmp_buf[i] = 0xFF;
            }
            wm_hal_flash_write_pages(hal_dev, fist_page_addr, tmp_buf,
                                     hal_dev->device_info.sector_size / hal_dev->device_info.page_size);

            left_erase_len = left_erase_len - (hal_dev->device_info.sector_size - secotr_offset);
            if (left_erase_len / hal_dev->device_info.sector_size) {
                wm_hal_flash_erase_sectors(hal_dev, (secotr_start_idx + 1) * hal_dev->device_info.sector_size,
                                           left_erase_len / hal_dev->device_info.sector_size);
            }
            //step4 erase last sector and recover the region shoule not be erase
            last_page_addr =
                (secotr_start_idx + left_erase_len / hal_dev->device_info.sector_size + 1) * hal_dev->device_info.sector_size;
            wm_hal_flash_read_sectors(hal_dev, last_page_addr, tmp_buf, 1);
            wm_hal_flash_erase_sectors(hal_dev, last_page_addr, 1);

            for (int i = 0; i < erase_len_offset; i++) {
                tmp_buf[i] = 0xFF;
            }
            wm_hal_flash_write_pages(hal_dev, last_page_addr, tmp_buf,
                                     hal_dev->device_info.sector_size / hal_dev->device_info.page_size);

            break;
        }

        wm_log_error("err case, offset: %u, len_off: %u, ers_len: %u\n", secotr_offset, erase_len_offset, erase_len);
        ret = WM_ERR_FAILED;
        break;

    } while (1);

    wm_os_internal_free(tmp_buf);
    wm_os_internal_mutex_release(drv_ctx->mutex);

    return ret;
}

static int wm_drv_internal_flash_erase_sector(wm_device_t *dev, uint32_t sector_idx, uint32_t sector_count)
{
    wm_hal_flash_dev_t *hal_dev  = NULL;
    int ret                      = WM_ERR_SUCCESS;
    wm_drv_iflash_ctx_t *drv_ctx = NULL;

    if (!dev || !dev->drv || !sector_count) {
        return WM_ERR_INVALID_PARAM;
    }

    drv_ctx = (wm_drv_iflash_ctx_t *)dev->drv;
    hal_dev = &drv_ctx->hal_dev;

    if ((sector_idx + sector_count) * hal_dev->device_info.sector_size > hal_dev->device_info.size) {
        wm_log_error("sec_idx:%d, sec_cnt:%d\n", sector_idx, sector_count);
        return WM_ERR_INVALID_PARAM;
    }

    if (sector_idx * hal_dev->device_info.sector_size <= WM_FT_REGION) {
        wm_log_error("cannot write or erase FT region: 0 ~ 0x%x\n", WM_FT_REGION);
        return WM_ERR_INVALID_PARAM;
    }

    wm_os_internal_mutex_acquire(drv_ctx->mutex, WM_OS_WAIT_TIME_MAX);
    wm_hal_flash_erase_sectors(hal_dev, sector_idx * hal_dev->device_info.sector_size, sector_count);
    wm_os_internal_mutex_release(drv_ctx->mutex);

    return ret;
}

static int wm_drv_internal_flash_erase_chip(wm_device_t *dev)
{
    wm_hal_flash_dev_t *hal_dev  = NULL;
    int ret                      = WM_ERR_SUCCESS;
    wm_drv_iflash_ctx_t *drv_ctx = NULL;

    if (!dev || !dev->drv) {
        return WM_ERR_INVALID_PARAM;
    }

    drv_ctx = (wm_drv_iflash_ctx_t *)dev->drv;
    hal_dev = &drv_ctx->hal_dev;

    wm_os_internal_mutex_acquire(drv_ctx->mutex, WM_OS_WAIT_TIME_MAX);
    wm_hal_flash_erase_chip(hal_dev);
    wm_os_internal_mutex_release(drv_ctx->mutex);

    return ret;
}

//TODO: the init function will enhancement in feature
static int wm_drv_internal_flash_init(wm_device_t *dev)
{
    wm_drv_iflash_ctx_t *drv_ctx  = NULL;
    wm_dt_hw_iflash_t *dt_hw_info = NULL;
    int ret                       = WM_ERR_SUCCESS;

    if (dev->drv == NULL && dev->state == WM_DEV_ST_UNINIT) {
        dev->state = WM_DEV_ST_INITING;

        //alloc drv ctx
        drv_ctx = (wm_drv_iflash_ctx_t *)wm_os_internal_malloc(sizeof(wm_drv_iflash_ctx_t));
        if (!drv_ctx) {
            dev->state = WM_DEV_ST_UNINIT;
            return WM_ERR_NO_MEM;
        }
        memset(drv_ctx, 0x00, sizeof(wm_drv_iflash_ctx_t));

        dt_hw_info = (wm_dt_hw_iflash_t *)dev->hw;
        // hal dev init
        drv_ctx->hal_dev.reg_base  = dt_hw_info->reg_base;
        drv_ctx->hal_dev.quad_spi  = dt_hw_info->flash_cfg.quad_spi;
        drv_ctx->hal_dev.chip_name = dev->name;
        wm_hal_flash_init(&drv_ctx->hal_dev);

        //create mutex
        if (wm_os_internal_mutex_create(&drv_ctx->mutex) != WM_OS_STATUS_SUCCESS) {
            wm_os_internal_free(drv_ctx);
            dev->state = WM_DEV_ST_UNINIT;

            return WM_ERR_FAILED;
        }
        ret        = WM_ERR_SUCCESS;
        dev->drv   = drv_ctx;
        dev->state = WM_DEV_ST_INITED;
    }

    return ret;
}

static int wm_drv_internal_flash_deinit(wm_device_t *dev)
{
    wm_drv_iflash_ctx_t *drv_ctx = NULL;
    int ret                      = WM_ERR_SUCCESS;

    drv_ctx = (wm_drv_iflash_ctx_t *)dev->drv;
    if (drv_ctx && dev->state == WM_DEV_ST_INITED) {
        dev->drv = NULL;
        ret      = wm_os_internal_mutex_acquire(drv_ctx->mutex, WM_OS_WAIT_TIME_MAX);
        if (ret == WM_ERR_SUCCESS) {
            wm_hal_flash_deinit(&drv_ctx->hal_dev);
            wm_os_internal_mutex_delete(drv_ctx->mutex);
            drv_ctx->mutex = NULL;
            wm_os_internal_free(drv_ctx);
            dev->state = WM_DEV_ST_UNINIT;
        } else {
            ret        = WM_ERR_FAILED;
            dev->state = WM_DEV_ST_UNKNOWN;
        }
    }

    return ret;
}

//fix RSA & iflash share 1K memory issue
wm_os_status_t wm_drv_internal_flash_acquire_mutex(wm_device_t *flash_dev, uint32_t wait_time)
{
    wm_drv_iflash_ctx_t *drv_ctx = NULL;
    wm_os_status_t ret           = WM_OS_STATUS_SUCCESS;

    if (!flash_dev || flash_dev->state == WM_DEV_ST_UNINIT || flash_dev->state == WM_DEV_ST_UNKNOWN) {
        return WM_OS_STATUS_SUCCESS;
    }

    drv_ctx = (wm_drv_iflash_ctx_t *)flash_dev->drv;
    if (drv_ctx && drv_ctx->mutex) {
        ret = wm_os_internal_mutex_acquire(drv_ctx->mutex, wait_time);
    }

    return ret;
}

//fix RSA & iflash share 1K memory issue
wm_os_status_t wm_drv_internal_flash_release_mutex(wm_device_t *flash_dev)
{
    wm_drv_iflash_ctx_t *drv_ctx = NULL;

    if (!flash_dev || flash_dev->state == WM_DEV_ST_UNINIT || flash_dev->state == WM_DEV_ST_UNKNOWN) {
        return WM_OS_STATUS_SUCCESS;
    }

    drv_ctx = (wm_drv_iflash_ctx_t *)flash_dev->drv;
    if (drv_ctx && drv_ctx->mutex) {
        wm_os_internal_mutex_release(drv_ctx->mutex);
    }

    return WM_OS_STATUS_SUCCESS;
}

const wm_drv_flash_ops_t wm_drv_internal_flash_ops = {
    .init             = wm_drv_internal_flash_init,
    .deinit           = wm_drv_internal_flash_deinit,
    .write_with_erase = wm_drv_internal_flash_write_with_erase,
    .write            = wm_drv_internal_flash_write,
    .read             = wm_drv_internal_flash_read,
    .get_info         = wm_drv_internal_flash_get_device_info,
    .erase_region     = wm_drv_internal_flash_erase_region,
    .erase_sector     = wm_drv_internal_flash_erase_sector,
    .erase_chip       = wm_drv_internal_flash_erase_chip,
};

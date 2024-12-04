/**
 * @file wm_drv_ops_touch_sensor.c
 *
 * @brief touch sensor ops Driver Module
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
#include "wm_drv_gpio.h"
#include "wm_drv_ops_external_flash_spi.h"
#include "wm_drv_eflash_chips.h"
#include "wm_drv_ops_external_flash_spi.c"
#include "wm_drv_rcc.h"

#undef LOG_TAG
#define LOG_TAG "efls"
#include "wm_log.h"

static const wm_eflash_t *wm_flash_get_current_chip(char *name)
{
    int i                       = 0;
    int support_num             = sizeof(wm_external_flash_chips) / sizeof(wm_eflash_t *);
    const wm_eflash_t *p_result = NULL;

    for (i = 0; i < support_num; i++) {
        if (memcmp(name, wm_external_flash_chips[i]->name, strlen(wm_external_flash_chips[i]->name)) == 0) {
            p_result = wm_external_flash_chips[i];
            break;
        }
    }

    return p_result;
}

static int wm_drv_external_flash_write_with_erase(wm_device_t *dev, uint32_t offset, uint8_t *in_buf, uint32_t in_buf_len)
{
    uint8_t *tmp_buf    = NULL;
    uint32_t sector_idx = 0, sector_offset = 0, remain_size = 0;
    uint32_t flash_addr = offset, write_len = in_buf_len;
    uint8_t *src_buf             = in_buf;
    uint32_t i                   = 0;
    wm_drv_eflash_ctx_t *drv_ctx = NULL;
    const wm_eflash_t *cur_flash = NULL;
    int ret                      = WM_ERR_SUCCESS;

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    if (!dev || !in_buf || !dev->drv || !in_buf_len) {
        return WM_ERR_INVALID_PARAM;
    }

    drv_ctx   = (wm_drv_eflash_ctx_t *)dev->drv;
    cur_flash = drv_ctx->p_flash;
    assert(cur_flash != NULL);

    if ((offset + in_buf_len) > drv_ctx->size) {
        wm_log_error("write offset:%u, in_len:%u, flash size:%u\n", offset, in_buf_len, drv_ctx->size);
        return WM_ERR_INVALID_PARAM;
    }

    tmp_buf = (uint8_t *)wm_os_internal_calloc(1, cur_flash->sector_size);
    if (tmp_buf == NULL) {
        wm_log_error("allocate sector cache memory fail!\n");
        return WM_ERR_NO_MEM;
    }

    wm_os_internal_mutex_acquire(drv_ctx->mutex, WM_OS_WAIT_TIME_MAX);
    sector_idx    = flash_addr / cur_flash->sector_size;    //Section addr
    sector_offset = flash_addr % cur_flash->sector_size;    //Offset in section
    remain_size   = cur_flash->sector_size - sector_offset; // remain size in sector
    if (write_len <= remain_size)
        remain_size = write_len; //Not bigger with remain size in section

    while (1) {
        if (remain_size != cur_flash->sector_size) { //will keep some region value
            ret = wm_spi_flash_read_pages(dev, cur_flash, sector_idx * cur_flash->sector_size, tmp_buf,
                                          cur_flash->sector_size / cur_flash->page_size);
            if (ret != WM_ERR_SUCCESS) {
                break;
            }
        }

        //step1: erase current sector
        ret = wm_spi_flash_erase_sectors(dev, cur_flash, sector_idx * cur_flash->sector_size, 1);
        if (ret != WM_ERR_SUCCESS) {
            break;
        }

        //step2: fill write value in current sector
        for (i = 0; i < remain_size; i++) {
            tmp_buf[i + sector_offset] = src_buf[i];
        }

        //step3: write current sector
        ret = wm_spi_flash_program_pages(dev, cur_flash, sector_idx * cur_flash->sector_size, tmp_buf,
                                         cur_flash->sector_size / cur_flash->page_size);
        if (ret != WM_ERR_SUCCESS) {
            break;
        }

        //setp4: check current sector whether enough to fill write data
        if (write_len == remain_size) {
            break;        //write done
        } else {          //will write anthoer sector
            sector_idx++; // secotr index + 1
            sector_offset = 0;
            src_buf += remain_size;   // secremain is current write data length
            write_len -= remain_size; // the legth will write to sector
            if (write_len > (cur_flash->sector_size)) {
                remain_size = cur_flash->sector_size; // next sector still not enough fill the write value
            } else {
                if (write_len == 0)
                    break;
                remain_size = write_len; //Next section will finish
            }
        }
    }

    wm_os_internal_free(tmp_buf);
    wm_os_internal_mutex_release(drv_ctx->mutex);

    return ret;
}

//write by page
static int wm_drv_external_flash_write(wm_device_t *dev, uint32_t offset, uint8_t *in_buf, uint32_t in_buf_len)
{
    uint8_t *tmp_buf  = NULL;
    uint32_t page_idx = 0, page_offset = 0, remain_size = 0;
    uint32_t flash_addr = offset, write_len = in_buf_len;
    uint8_t *src_buf             = in_buf;
    uint32_t i                   = 0;
    wm_drv_eflash_ctx_t *drv_ctx = NULL;
    const wm_eflash_t *cur_flash = NULL;
    int ret                      = WM_ERR_SUCCESS;

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    if (!dev || !in_buf || !dev->drv || !in_buf_len) {
        return WM_ERR_INVALID_PARAM;
    }

    drv_ctx   = (wm_drv_eflash_ctx_t *)dev->drv;
    cur_flash = drv_ctx->p_flash;
    assert(cur_flash != NULL);

    drv_ctx = (wm_drv_eflash_ctx_t *)dev->drv;

    if ((offset + in_buf_len) > drv_ctx->size) {
        wm_log_error("write offset:%u, in_len:%u, flash size:%u\n", offset, in_buf_len, drv_ctx->size);
        return WM_ERR_INVALID_PARAM;
    }

    tmp_buf = (uint8_t *)wm_os_internal_malloc(cur_flash->page_size);
    if (tmp_buf == NULL) {
        wm_log_error("allocate sector cache memory fail!\n");
        return WM_ERR_NO_MEM;
    }

    wm_os_internal_mutex_acquire(drv_ctx->mutex, WM_OS_WAIT_TIME_MAX);
    page_idx    = flash_addr / cur_flash->page_size;  //page addr
    page_offset = flash_addr % cur_flash->page_size;  //Offset in page
    remain_size = cur_flash->page_size - page_offset; // remain size in page
    if (write_len <= remain_size)
        remain_size = write_len; //Not bigger with remain size in page

    while (1) {
        if (remain_size != cur_flash->page_size) { //will keep some region value
            ret = wm_spi_flash_read_pages(dev, cur_flash, page_idx * cur_flash->page_size, tmp_buf, 1);
            if (ret != WM_ERR_SUCCESS) {
                break;
            }
        }

        //step1: fill write value in current page
        for (i = 0; i < remain_size; i++) {
            tmp_buf[i + page_offset] = src_buf[i];
        }

        //step2: write current page
        ret = wm_spi_flash_program_pages(dev, cur_flash, page_idx * cur_flash->page_size, tmp_buf, 1);
        if (ret != WM_ERR_SUCCESS) {
            break;
        }

        //setp3: check current sector whether enough to fill write data
        if (write_len == remain_size) {
            break;      //write done
        } else {        //will write anthoer page
            page_idx++; // secotr index + 1
            page_offset = 0;
            src_buf += remain_size;   // secremain is current write data length
            write_len -= remain_size; // the legth will write to sector
            if (write_len > (cur_flash->page_size)) {
                remain_size = cur_flash->page_size; // next page still not enough fill the write value
            } else {
                if (write_len == 0)
                    break;
                remain_size = write_len; //Next page will finish
            }
        }
    }

    wm_os_internal_free(tmp_buf);
    wm_os_internal_mutex_release(drv_ctx->mutex);

    return WM_ERR_SUCCESS;
}

static int wm_drv_external_flash_read(wm_device_t *dev, uint32_t offset, uint8_t *out_buf, uint32_t out_len)
{
    uint8_t *tmp_buf    = NULL;
    uint8_t *read_buf   = out_buf;
    uint32_t flash_addr = offset, read_len = out_len;
    uint32_t page_idx = 0, page_offset = 0, remain_size = 0;
    wm_drv_eflash_ctx_t *drv_ctx = NULL;
    const wm_eflash_t *cur_flash = NULL;
    int ret                      = WM_ERR_SUCCESS;

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    if (!dev || !out_buf || !dev->drv || !out_len) {
        return WM_ERR_INVALID_PARAM;
    }

    drv_ctx   = (wm_drv_eflash_ctx_t *)dev->drv;
    cur_flash = drv_ctx->p_flash;
    assert(cur_flash != NULL);

    if ((offset + out_len) > drv_ctx->size) {
        wm_log_error("read offset:%u, out_len:%u, flash size:%u\n", offset, out_len, drv_ctx->size);
        return WM_ERR_INVALID_PARAM;
    }

    tmp_buf = (uint8_t *)wm_os_internal_malloc(cur_flash->page_size);
    if (tmp_buf == NULL) {
        wm_log_error("allocate page memory fail!\n");
        return WM_ERR_NO_MEM;
    }

    wm_os_internal_mutex_acquire(drv_ctx->mutex, WM_OS_WAIT_TIME_MAX);

    page_idx    = flash_addr / cur_flash->page_size;  //Section addr
    page_offset = flash_addr % cur_flash->page_size;  //Offset in section
    remain_size = cur_flash->page_size - page_offset; // remain size in sector

    if (read_len <= remain_size) {
        remain_size = read_len;
    }

    while (1) {
        ret = wm_spi_flash_read_pages(dev, cur_flash, page_idx * cur_flash->page_size, tmp_buf, 1);
        if (ret != WM_ERR_SUCCESS) {
            break;
        }
        memcpy(read_buf, tmp_buf + page_offset, remain_size);

        if (read_len == remain_size) {
            break;      //read done
        } else {        //will write anthoer sector
            page_idx++; // secotr index + 1
            page_offset = 0;
            read_buf += remain_size; // secremain is current write data length
            read_len -= remain_size; // the legth will write to sector

            if (read_len > (cur_flash->page_size)) {
                remain_size = cur_flash->page_size;
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

    return ret;
}

static int wm_drv_external_flash_get_device_info(wm_device_t *dev, wm_drv_flash_info_t *flash_info)
{
    const wm_eflash_t *cur_flash = NULL;
    wm_drv_eflash_ctx_t *drv_ctx = NULL;

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    if (!dev || !dev->drv || !flash_info) {
        return WM_ERR_INVALID_PARAM;
    }

    drv_ctx   = (wm_drv_eflash_ctx_t *)dev->drv;
    cur_flash = drv_ctx->p_flash;
    assert(cur_flash != NULL);

    flash_info->page_size   = cur_flash->page_size;
    flash_info->sector_size = cur_flash->sector_size;
    flash_info->flash_size  = drv_ctx->size;
    flash_info->vendor_id   = cur_flash->vendor_id;
    memcpy(&flash_info->unique_id, &drv_ctx->unique_id, FLASH_UNIQUE_ID_SIZE);

    return WM_ERR_SUCCESS;
}

static int wm_drv_external_flash_erase_region(wm_device_t *dev, uint32_t offset, uint32_t erase_len)
{
    uint8_t *tmp_buf       = NULL;
    uint32_t secotr_offset = 0, left_erase_len = erase_len, secotr_start_idx = 0, erase_len_offset = 0;
    uint32_t flash_addr          = offset;
    wm_drv_eflash_ctx_t *drv_ctx = NULL;
    const wm_eflash_t *cur_flash = NULL;
    int ret                      = WM_ERR_SUCCESS;

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    if (!dev || !dev->drv || !erase_len) {
        return WM_ERR_INVALID_PARAM;
    }

    drv_ctx   = (wm_drv_eflash_ctx_t *)dev->drv;
    cur_flash = drv_ctx->p_flash;
    assert(cur_flash != NULL);

    if ((offset + erase_len) > drv_ctx->size) {
        wm_log_error("erase offset:%u, out_len:%u, flash size:%u\n", offset, erase_len, drv_ctx->size);
        return WM_ERR_INVALID_PARAM;
    }

    tmp_buf = (uint8_t *)wm_os_internal_malloc(cur_flash->sector_size);
    if (tmp_buf == NULL) {
        wm_log_error("allocate sector tmp_buf memory fail!\n");
        return WM_ERR_NO_MEM;
    }

    wm_os_internal_mutex_acquire(drv_ctx->mutex, WM_OS_WAIT_TIME_MAX);

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
    secotr_start_idx = flash_addr / cur_flash->sector_size;
    secotr_offset    = flash_addr % cur_flash->sector_size;
    //erase_len_offset = erase_len % cur_flash->sector_size;
    erase_len_offset = (erase_len + secotr_offset) % cur_flash->sector_size;

    do {
        /** case1:  |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|___________________________________|______________________
        **      secotr_start_idx                    sector2                              sector3
        **/
        if (!secotr_offset && !erase_len_offset) {
            ret = wm_spi_flash_erase_sectors(dev, cur_flash, secotr_start_idx * cur_flash->sector_size,
                                             erase_len / cur_flash->sector_size);
            break;
        }

        /** case2:  |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|xxxxxx________________________________|______________________
        **      secotr_start_idx                     sector2    erase_len_offset                          sector3
        **/
        if (!secotr_offset && erase_len_offset) {
            uint32_t ers_sector_num      = erase_len / cur_flash->sector_size + 1;
            uint32_t recovery_start_addr = 0;
            //step1: save last sector data
            ret = wm_spi_flash_read_sectors(dev, cur_flash, (secotr_start_idx + (ers_sector_num - 1)) * cur_flash->sector_size,
                                            tmp_buf, 1);
            if (ret != WM_ERR_SUCCESS) {
                break;
            }

            //step2 erase sector
            ret = wm_spi_flash_erase_sectors(dev, cur_flash, secotr_start_idx * cur_flash->sector_size, ers_sector_num);
            if (ret != WM_ERR_SUCCESS) {
                break;
            }

            //handle the last sector
            recovery_start_addr = (secotr_start_idx + ers_sector_num - 1) * cur_flash->sector_size;
            for (int i = 0; i < erase_len_offset; i++) {
                tmp_buf[i] = 0xFF;
            }
            ret = wm_spi_flash_program_pages(dev, cur_flash, recovery_start_addr, tmp_buf,
                                             cur_flash->sector_size / cur_flash->page_size);
            if (ret != WM_ERR_SUCCESS) {
                break;
            }

            break;
        }

        /** case3:  |__________xxxxxxxxxxxxxxxxxxxx_______|___________________________________|______________________
        **      sector1       secotr_offset              sector2                            sector3
        **/
        if (secotr_offset && ((secotr_offset + erase_len) <= cur_flash->sector_size)) {
            ret = wm_spi_flash_read_sectors(dev, cur_flash, secotr_start_idx * cur_flash->sector_size, tmp_buf, 1);
            if (ret != WM_ERR_SUCCESS) {
                break;
            }

            for (int i = secotr_offset; i < (secotr_offset + erase_len); i++) {
                tmp_buf[i] = 0xFF;
            }
            ret = wm_spi_flash_erase_sectors(dev, cur_flash, secotr_start_idx * cur_flash->sector_size, 1);
            if (ret != WM_ERR_SUCCESS) {
                break;
            }

            ret = wm_spi_flash_program_pages(dev, cur_flash, secotr_start_idx * cur_flash->sector_size, tmp_buf,
                                             cur_flash->sector_size / cur_flash->page_size);
            if (ret != WM_ERR_SUCCESS) {
                break;
            }

            break;
        }

        /** case4:  |__________xxxxxxxxxxxxxxxxxxxxxxxxxxx|xxxxxxxxxxxxx_____________________|______________________
        **      sector1       secotr_offset              sector2     erase_len_offset         sector3
        **/
        if (secotr_offset && ((secotr_offset + erase_len) > cur_flash->sector_size)) {
            uint32_t fist_page_addr = 0, last_page_addr = 0;
            //step1: read the first erase sector
            fist_page_addr = secotr_start_idx * cur_flash->sector_size;
            ret            = wm_spi_flash_read_sectors(dev, cur_flash, fist_page_addr, tmp_buf, 1);
            if (ret != WM_ERR_SUCCESS) {
                break;
            }

            ret = wm_spi_flash_erase_sectors(dev, cur_flash, fist_page_addr, 1);
            if (ret != WM_ERR_SUCCESS) {
                break;
            }

            for (int i = secotr_offset; i < cur_flash->sector_size; i++) {
                tmp_buf[i] = 0xFF;
            }
            ret = wm_spi_flash_program_pages(dev, cur_flash, fist_page_addr, tmp_buf,
                                             cur_flash->sector_size / cur_flash->page_size);
            if (ret != WM_ERR_SUCCESS) {
                break;
            }

            left_erase_len = left_erase_len - (cur_flash->sector_size - secotr_offset);
            if (left_erase_len / cur_flash->sector_size) {
                ret = wm_spi_flash_erase_sectors(dev, cur_flash, (secotr_start_idx + 1) * cur_flash->sector_size,
                                                 left_erase_len / cur_flash->sector_size);
                if (ret != WM_ERR_SUCCESS) {
                    break;
                }
            }
            //step4 erase last sector and recover the region shoule not be erase
            last_page_addr = (secotr_start_idx + left_erase_len / cur_flash->sector_size + 1) * cur_flash->sector_size;
            ret            = wm_spi_flash_read_sectors(dev, cur_flash, last_page_addr, tmp_buf, 1);
            if (ret != WM_ERR_SUCCESS) {
                break;
            }

            ret = wm_spi_flash_erase_sectors(dev, cur_flash, last_page_addr, 1);
            if (ret != WM_ERR_SUCCESS) {
                break;
            }

            for (int i = 0; i < erase_len_offset; i++) {
                tmp_buf[i] = 0xFF;
            }
            ret = wm_spi_flash_program_pages(dev, cur_flash, last_page_addr, tmp_buf,
                                             cur_flash->sector_size / cur_flash->page_size);
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

static int wm_drv_external_flash_erase_sector(wm_device_t *dev, uint32_t sector_idx, uint32_t sector_count)
{
    int ret                      = WM_ERR_SUCCESS;
    wm_drv_eflash_ctx_t *drv_ctx = NULL;
    const wm_eflash_t *cur_flash = NULL;

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    if (!dev || !dev->drv || !sector_count) {
        return WM_ERR_INVALID_PARAM;
    }

    drv_ctx   = (wm_drv_eflash_ctx_t *)dev->drv;
    cur_flash = drv_ctx->p_flash;
    assert(cur_flash != NULL);

    if ((sector_idx + sector_count) * cur_flash->sector_size > drv_ctx->size) {
        wm_log_error("sec_idx:%d, sec_cnt:%d\n", sector_idx, sector_count);
        return WM_ERR_INVALID_PARAM;
    }

    wm_os_internal_mutex_acquire(drv_ctx->mutex, WM_OS_WAIT_TIME_MAX);
    ret = wm_spi_flash_erase_sectors(dev, cur_flash, sector_idx * cur_flash->sector_size, sector_count);
    wm_os_internal_mutex_release(drv_ctx->mutex);

    return ret;
}

static int wm_drv_external_flash_erase_chip(wm_device_t *dev)
{
    int ret                      = WM_ERR_SUCCESS;
    wm_drv_eflash_ctx_t *drv_ctx = NULL;
    const wm_eflash_t *cur_flash = NULL;

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    if (!dev || !dev->drv) {
        return WM_ERR_INVALID_PARAM;
    }

    drv_ctx   = (wm_drv_eflash_ctx_t *)dev->drv;
    cur_flash = drv_ctx->p_flash;
    assert(cur_flash != NULL);

    wm_os_internal_mutex_acquire(drv_ctx->mutex, WM_OS_WAIT_TIME_MAX);
    ret = wm_spi_flash_erase_chip(dev, cur_flash);
    wm_os_internal_mutex_release(drv_ctx->mutex);

    return ret;
}

static int wm_drv_external_flash_init(wm_device_t *dev)
{
    wm_drv_eflash_ctx_t *drv_ctx = NULL;
    const wm_eflash_t *cur_flash = NULL;
    wm_dt_hw_eflash_t *hw        = NULL;
    wm_dt_hw_pin_cfg_t *pin_cs;
    wm_device_t *gpio_dev = NULL;
    int ret               = WM_ERR_SUCCESS;

    cur_flash = wm_flash_get_current_chip(dev->name);
    if (!cur_flash) {
        wm_log_error("get external %s fail\n", dev->name);
        return WM_ERR_FAILED;
    }

    if (dev->drv == NULL && dev->state == WM_DEV_ST_UNINIT) {
        dev->state = WM_DEV_ST_INITING;

        //alloc drv ctx
        drv_ctx = (wm_drv_eflash_ctx_t *)wm_os_internal_malloc(sizeof(wm_drv_eflash_ctx_t));
        if (!drv_ctx) {
            return WM_ERR_NO_MEM;
        }
        memset(drv_ctx, 0x00, sizeof(wm_drv_eflash_ctx_t));
        //create mutex
        if (wm_os_internal_mutex_create(&drv_ctx->mutex) != WM_OS_STATUS_SUCCESS) {
            wm_os_internal_free(drv_ctx);
            dev->state = WM_DEV_ST_UNINIT;
            return WM_ERR_FAILED;
        }

        gpio_dev = wm_dt_get_device_by_name("gpio");
        if (gpio_dev->state != WM_DEV_ST_INITED) {
            wm_drv_gpio_init("gpio");
        }

        ret = wm_os_internal_mutex_acquire(drv_ctx->mutex, WM_OS_WAIT_TIME_MAX);

        if (ret == WM_ERR_SUCCESS) {
            hw                  = dev->hw;
            drv_ctx->spi_device = wm_drv_spim_init(hw->spi_device_name); //TODO  spi master should be init by system
            assert(drv_ctx->spi_device != NULL);

            drv_ctx->rcc_device = wm_dt_get_device_by_name("rcc");
            if (drv_ctx->rcc_device->state != WM_DEV_ST_INITED) {
                drv_ctx->rcc_device = wm_drv_clock_init("rcc");
            }

            pin_cs = &hw->spi_cfg.pin_cs;
            wm_drv_gpio_iomux_func_sel(pin_cs->pin_num, pin_cs->pin_mux);
            wm_drv_gpio_set_pullmode(pin_cs->pin_num, pin_cs->pin_pupd);
            wm_drv_gpio_set_dir(pin_cs->pin_num, pin_cs->pin_dir);
            //default CS is invalid
            wm_drv_gpio_data_set(pin_cs->pin_num);

            wm_spi_flash_init(dev, cur_flash);
            drv_ctx->p_flash = cur_flash;
            dev->drv         = drv_ctx;

            ret = wm_spi_flash_get_unique_id(dev, cur_flash, drv_ctx->unique_id);

            if (ret == WM_ERR_SUCCESS) {
                ret = wm_spi_flash_get_capacity(dev, cur_flash, &drv_ctx->size);
            }

            dev->state = WM_DEV_ST_INITED;
            wm_os_internal_mutex_release(drv_ctx->mutex);
        } else {
            wm_os_internal_free(drv_ctx);
            dev->state = WM_DEV_ST_UNKNOWN;
            ret        = WM_ERR_FAILED;
        }
    }

    return ret;
}

static int wm_drv_external_flash_deinit(wm_device_t *dev)
{
    wm_drv_eflash_ctx_t *drv_ctx = NULL;
    int ret                      = WM_ERR_SUCCESS;

    drv_ctx = (wm_drv_eflash_ctx_t *)dev->drv;
    if (drv_ctx && dev->state == WM_DEV_ST_INITED) {
        dev->state = WM_DEV_ST_UNKNOWN;
        ret        = wm_os_internal_mutex_acquire(drv_ctx->mutex, WM_OS_WAIT_TIME_MAX);
        if (ret == WM_ERR_SUCCESS) {
            dev->drv = NULL;
            wm_os_internal_mutex_delete(drv_ctx->mutex);
            drv_ctx->mutex = NULL;
            wm_os_internal_free(drv_ctx);
            dev->state = WM_DEV_ST_UNINIT;
        } else {
            dev->state = WM_DEV_ST_UNKNOWN;
        }
    }

    return ret;
}

const wm_drv_flash_ops_t wm_drv_external_flash_ops = {
    .init             = wm_drv_external_flash_init,
    .deinit           = wm_drv_external_flash_deinit,
    .write_with_erase = wm_drv_external_flash_write_with_erase,
    .write            = wm_drv_external_flash_write,
    .read             = wm_drv_external_flash_read,
    .get_info         = wm_drv_external_flash_get_device_info,
    .erase_region     = wm_drv_external_flash_erase_region,
    .erase_sector     = wm_drv_external_flash_erase_sector,
    .erase_chip       = wm_drv_external_flash_erase_chip,
};

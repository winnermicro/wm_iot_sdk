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
#include "wm_log.h"
#include "wm_types.h"
#include "wm_osal.h"
#include "wm_hal_flash.h"
#include "wm_hal_iflash_chips.h"

#define WM_HAL_FLASH_HAL_DEBUG 1
#if WM_HAL_FLASH_HAL_DEBUG
#undef LOG_TAG
#define LOG_TAG               "flash_hal"
#define wm_fls_log_info(...)  wm_log_info(__VA_ARGS__)
#define wm_fls_log_debug(...) wm_log_debug(__VA_ARGS__)
#define wm_fls_log_error(...) wm_log_error(__VA_ARGS__)
#define WM_FLS_LOG_MARKER     wm_spim_log_debug("[%s %d]\n", __func__, __LINE__);
#else
#define wm_fls_log_info(...)
#define wm_fls_log_debug(...)
#define wm_fls_log_error(...)
#define WM_FLS_LOG_MARKER
#endif

#include "wm_log.h"

#define FLS_CMD_RD_DEVICE_ID 0x9F

typedef struct {
    const wm_flash_t *current_flash;
} wm_flash_priv_t;

static inline void flash_exec_cmd(wm_flash_reg_t *hw_reg)
{
    //start cmd
    wm_ll_flash_set_cmd_start_start(hw_reg, 1);
    //wait operation done
    while (wm_ll_flash_get_cmd_start_start(hw_reg))
        ;
}

//write enable
static inline void flash_write_enable(wm_hal_flash_dev_t *dev, wm_flash_reg_t *hw_reg)
{
    wm_flash_priv_t *priv       = NULL;
    const wm_flash_t *curr_chip = NULL;

    priv      = dev->priv_data;
    curr_chip = priv->current_flash;

    wm_ll_flash_reset_cmd_info(hw_reg, 0);
    wm_ll_flash_set_cmd_info_cmd(hw_reg, curr_chip->cmd_set.wren.commd_id);
    flash_exec_cmd(hw_reg);
}

/**
**  Capacity  |   memory type | manufacturer ID
**      1B    |     1B        |    1B
*/
/*output JEDEC ID: 1-byte manufacturer ID & 2-byte device ID*/
uint8_t wm_hal_flash_get_manufacturer_id(wm_hal_flash_dev_t *dev)
{
    uint8_t manufacturer_id = 0;
    wm_flash_reg_t *hw_reg  = (wm_flash_reg_t *)dev->reg_base;

    wm_ll_flash_reset_cmd_info(hw_reg, 0);
    wm_ll_flash_set_cmd_info_cmd(hw_reg, FLS_CMD_RD_DEVICE_ID);
    wm_ll_flash_set_cmd_info_is_read_cmd(hw_reg, 1);
    wm_ll_flash_set_cmd_info_has_data(hw_reg, 1);
    wm_ll_flash_set_cmd_info_data_size(hw_reg, 2);
    flash_exec_cmd(hw_reg);

    manufacturer_id = wm_ll_flash_read_data_from_fifo(0) & 0xFF;

    return manufacturer_id;
}

static const wm_flash_t *wm_iflash_get_current_chip(wm_hal_flash_dev_t *dev)
{
    int i                      = 0;
    int support_num            = sizeof(wm_iflash_support_chips) / sizeof(wm_flash_t *);
    const wm_flash_t *p_result = NULL;
    uint8_t vendor_id          = 0;

    vendor_id = wm_hal_flash_get_manufacturer_id(dev);

    for (i = 0; i < support_num; i++) {
        if (wm_iflash_support_chips[i]->vendor_id == vendor_id) {
            p_result = wm_iflash_support_chips[i];
            break;
        }
    }

    return p_result;
}

/**
**  Capacity  |   memory type | manufacturer ID
**      1B    |     1B        |    1B
*/
int32_t wm_hal_flash_get_capacity(wm_hal_flash_dev_t *dev)
{
    uint8_t capacity            = 0;
    wm_flash_reg_t *hw_reg      = (wm_flash_reg_t *)dev->reg_base;
    wm_flash_priv_t *priv       = NULL;
    const wm_flash_t *curr_chip = NULL;

    priv      = dev->priv_data;
    curr_chip = priv->current_flash;

    wm_ll_flash_reset_cmd_info(hw_reg, 0);
    wm_ll_flash_set_cmd_info_cmd(hw_reg, curr_chip->cmd_set.rdid.commd_id);
    wm_ll_flash_set_cmd_info_is_read_cmd(hw_reg, 1);
    wm_ll_flash_set_cmd_info_has_data(hw_reg, 1);
    wm_ll_flash_set_cmd_info_data_size(hw_reg, 2);
    flash_exec_cmd(hw_reg);

    capacity = (wm_ll_flash_read_data_from_fifo(0) >> 16) & 0xFF;

    return (1 << capacity);
}

static void wm_hal_flash_program_page(wm_hal_flash_dev_t *dev, wm_flash_reg_t *hw_reg, uint32_t flash_addr)
{
    wm_flash_priv_t *priv       = NULL;
    const wm_flash_t *curr_chip = NULL;

    priv      = dev->priv_data;
    curr_chip = priv->current_flash;

    flash_write_enable(dev, hw_reg);
    wm_ll_flash_reset_cmd_info(hw_reg, 0);
    wm_ll_flash_set_cmd_info_cmd(hw_reg, curr_chip->cmd_set.pp.commd_id);
    wm_ll_flash_set_cmd_info_is_pp(hw_reg, 1);
    wm_ll_flash_set_cmd_info_has_data(hw_reg, 1);
    wm_ll_flash_set_cmd_info_data_size(hw_reg, curr_chip->page_size - 1);
    wm_ll_flash_set_cmd_info_has_address(hw_reg, 1);
    wm_ll_flash_set_addr_address(hw_reg, flash_addr);
    flash_exec_cmd(hw_reg);
}

int wm_hal_flash_write_pages(wm_hal_flash_dev_t *dev, uint32_t flash_addr, uint8_t *buf, uint32_t write_page_num)
{
    uint32_t write_value        = 0;
    wm_flash_reg_t *hw_reg      = (wm_flash_reg_t *)dev->reg_base;
    wm_flash_priv_t *priv       = NULL;
    const wm_flash_t *curr_chip = NULL;

    priv      = dev->priv_data;
    curr_chip = priv->current_flash;

    if (flash_addr % curr_chip->page_size) {
        return WM_ERR_INVALID_PARAM;
    }

    while (write_page_num) {
        for (int i = 0; i < curr_chip->page_size; i += 4) {
            write_value = (buf[i]) | (buf[i + 1] << 8) | (buf[i + 2] << 16) | (buf[i + 3] << 24);
            wm_ll_flash_write_data_to_fifo(i / 4, write_value);
        }
        wm_hal_flash_program_page(dev, hw_reg, flash_addr);
        flash_addr += curr_chip->page_size;
        buf += curr_chip->page_size;
        write_page_num--;
    }

    return WM_ERR_SUCCESS;
}

static int wm_hal_flash_read_data_by_cmd(wm_hal_flash_dev_t *dev, wm_flash_reg_t *hw_reg, uint8_t cmd, uint32_t flash_addr,
                                         uint8_t *read_buf, uint32_t read_len)
{
    int i                       = 0;
    int word                    = read_len / 4;
    int byte                    = read_len % 4;
    wm_flash_priv_t *priv       = NULL;
    const wm_flash_t *curr_chip = NULL;

    priv      = dev->priv_data;
    curr_chip = priv->current_flash;

    /*check whether only support stand SPI mode in QIOM*/
    if (!wm_ll_flash_get_ctrl_qiom(hw_reg)) {
        if (cmd > WM_FLS_CMD_FREAD) {
            cmd = WM_FLS_CMD_FREAD;
        }
    }

    wm_ll_flash_reset_cmd_info(hw_reg, 0);
    wm_ll_flash_set_cmd_info_cmd(hw_reg, cmd);

    if (cmd == curr_chip->cmd_set.read.commd_id) {
        wm_ll_flash_set_cmd_info_is_read_cmd(hw_reg, 1);
        wm_ll_flash_set_cmd_info_has_data(hw_reg, 1);
        wm_ll_flash_set_cmd_info_has_address(hw_reg, 1);
        wm_ll_flash_set_cmd_info_data_size(hw_reg, (read_len - 1) & 0x3FF);
        wm_ll_flash_set_addr_address(hw_reg, flash_addr & WM_FLS_ADDR_MAX_BIT);
        flash_exec_cmd(hw_reg);
    } else if (cmd == curr_chip->cmd_set.fread.commd_id) {
        wm_ll_flash_set_cmd_info_is_read_cmd(hw_reg, 1);
        wm_ll_flash_set_cmd_info_has_data(hw_reg, 1);
        wm_ll_flash_set_cmd_info_has_address(hw_reg, 1);
        wm_ll_flash_set_cmd_info_data_size(hw_reg, (read_len - 1) & 0x3FF);
        wm_ll_flash_set_cmd_info_has_dummy(hw_reg, 1);
        //execute command as stand SPI
        wm_ll_flash_set_cmd_info_dummy_num(hw_reg, curr_chip->cmd_set.fread.dummy_bit);
        wm_ll_flash_set_addr_address(hw_reg, flash_addr & WM_FLS_ADDR_MAX_BIT);
        flash_exec_cmd(hw_reg);
    } else if (cmd == curr_chip->cmd_set.two_read.commd_id) {
        wm_ll_flash_set_cmd_info_is_read_cmd(hw_reg, 1);
        wm_ll_flash_set_cmd_info_has_data(hw_reg, 1);
        wm_ll_flash_set_cmd_info_has_address(hw_reg, 1);
        wm_ll_flash_set_cmd_info_data_size(hw_reg, (read_len - 1) & 0x3FF);
        wm_ll_flash_set_cmd_info_has_crm(hw_reg, 1);
        wm_ll_flash_set_cmd_info_has_dummy(hw_reg, 1);
        wm_ll_flash_set_cmd_info_dummy_num(hw_reg, curr_chip->cmd_set.two_read.dummy_bit);
        wm_ll_flash_set_addr_address(hw_reg, flash_addr & WM_FLS_ADDR_MAX_BIT);
        flash_exec_cmd(hw_reg);
    } else if (cmd == curr_chip->cmd_set.four_read.commd_id) {
        wm_ll_flash_set_cmd_info_is_read_cmd(hw_reg, 1);
        wm_ll_flash_set_cmd_info_has_data(hw_reg, 1);
        wm_ll_flash_set_cmd_info_has_address(hw_reg, 1);
        wm_ll_flash_set_cmd_info_data_size(hw_reg, (read_len - 1) & 0x3FF);
        wm_ll_flash_set_cmd_info_has_crm(hw_reg, 1);
        wm_ll_flash_set_cmd_info_has_dummy(hw_reg, 1);
        wm_ll_flash_set_cmd_info_dummy_num(hw_reg, curr_chip->cmd_set.four_read.dummy_bit);
        wm_ll_flash_set_addr_address(hw_reg, flash_addr & WM_FLS_ADDR_MAX_BIT);
        flash_exec_cmd(hw_reg);
    } else {
        return WM_ERR_INVALID_PARAM;
    }

    for (i = 0; i < word; i++) {
        *(uint32_t *)(read_buf) = wm_ll_flash_read_data_from_fifo(i);
        read_buf += 4;
    }

    //not 4byte align
    if (byte > 0) {
        *(uint32_t *)(read_buf) = wm_ll_flash_read_data_from_fifo(0);
        read_buf += 3; //point last byte
        byte = 4 - byte;
        while (byte) {
            *read_buf = 0;
            read_buf--;
            byte--;
        }
    }

    return WM_ERR_SUCCESS;
}

int wm_hal_flash_read_pages(wm_hal_flash_dev_t *dev, uint32_t flash_addr, uint8_t *read_buf, uint32_t read_page_num)
{
    uint8_t rd_cmd_id           = 0xFF;
    int ret                     = WM_ERR_SUCCESS;
    wm_flash_reg_t *hw_reg      = (wm_flash_reg_t *)dev->reg_base;
    wm_flash_priv_t *priv       = NULL;
    const wm_flash_t *curr_chip = NULL;

    priv      = dev->priv_data;
    curr_chip = priv->current_flash;

    if (flash_addr % curr_chip->page_size) {
        return WM_ERR_INVALID_PARAM; //start address must algin with page
    }

    if (curr_chip->support_mode & SUPPORT_QUAD_SPI && dev->quad_spi) {
        rd_cmd_id = curr_chip->cmd_set.four_read.commd_id;
    } else {
        rd_cmd_id = curr_chip->cmd_set.read.commd_id;
    }

    while (read_page_num) {
        ret = wm_hal_flash_read_data_by_cmd(dev, hw_reg, rd_cmd_id, flash_addr, read_buf, curr_chip->page_size);
        if (ret == WM_ERR_SUCCESS) {
            flash_addr += curr_chip->page_size;
            read_buf += curr_chip->page_size;
            read_page_num--;
        } else {
            break;
        }
    }

    return ret;
}

int wm_hal_flash_read_sectors(wm_hal_flash_dev_t *dev, uint32_t flash_addr, uint8_t *read_buf, uint32_t read_sector_num)
{
    int ret                     = WM_ERR_SUCCESS;
    uint32_t page_num           = 0;
    wm_flash_priv_t *priv       = NULL;
    const wm_flash_t *curr_chip = NULL;

    priv      = dev->priv_data;
    curr_chip = priv->current_flash;

    if (flash_addr % curr_chip->sector_size) {
        return WM_ERR_INVALID_PARAM; //start address must algin with sector
    }

    page_num = read_sector_num * curr_chip->sector_size / curr_chip->page_size;
    ret      = wm_hal_flash_read_pages(dev, flash_addr, read_buf, page_num);

    return ret;
}

/*
** The Read Unique ID command accesses a factory-set read-only 128bit number that is unique to each
** P25QSxx device
*/
void wm_hal_flash_get_unique_id(wm_hal_flash_dev_t *dev, uint32_t uuid[4])
{
    wm_flash_reg_t *hw_reg      = (wm_flash_reg_t *)dev->reg_base;
    wm_flash_priv_t *priv       = NULL;
    const wm_flash_t *curr_chip = NULL;

    priv      = dev->priv_data;
    curr_chip = priv->current_flash;

    wm_ll_flash_reset_cmd_info(hw_reg, 0);
    wm_ll_flash_set_cmd_info_cmd(hw_reg, curr_chip->cmd_set.ruid.commd_id);
    wm_ll_flash_set_cmd_info_is_read_cmd(hw_reg, 1);
    wm_ll_flash_set_cmd_info_has_data(hw_reg, 1);
    wm_ll_flash_set_cmd_info_data_size(hw_reg, 16 - 1);
    wm_ll_flash_set_cmd_info_has_dummy(hw_reg, 1);
    wm_ll_flash_set_cmd_info_dummy_num(hw_reg, curr_chip->cmd_set.ruid.dummy_bit);
    flash_exec_cmd(hw_reg);

    for (int i = 0; i < 4; i++) {
        uuid[i] = wm_ll_flash_read_data_from_fifo(i);
    }
}

int wm_hal_flash_erase_sectors(wm_hal_flash_dev_t *dev, uint32_t flash_addr, uint32_t sector_num)
{
    wm_flash_reg_t *hw_reg      = (wm_flash_reg_t *)dev->reg_base;
    wm_flash_priv_t *priv       = NULL;
    const wm_flash_t *curr_chip = NULL;

    priv      = dev->priv_data;
    curr_chip = priv->current_flash;

    if (flash_addr % curr_chip->sector_size) {
        return WM_ERR_INVALID_PARAM;
    }

    while (sector_num) {
        flash_write_enable(dev, hw_reg);
        wm_ll_flash_reset_cmd_info(hw_reg, 0);
        wm_ll_flash_set_cmd_info_cmd(hw_reg, curr_chip->cmd_set.se.commd_id);
        wm_ll_flash_set_cmd_info_is_se(hw_reg, 1);
        wm_ll_flash_set_cmd_info_has_address(hw_reg, 1);
        wm_ll_flash_set_addr_address(hw_reg, flash_addr & WM_FLS_ADDR_MAX_BIT);
        flash_exec_cmd(hw_reg);

        flash_addr += curr_chip->sector_size;

        sector_num--;
    }

    return WM_ERR_SUCCESS;
}

//erase whole chip
void wm_hal_flash_erase_chip(wm_hal_flash_dev_t *dev)
{
    wm_flash_reg_t *hw_reg      = (wm_flash_reg_t *)dev->reg_base;
    wm_flash_priv_t *priv       = NULL;
    const wm_flash_t *curr_chip = NULL;

    priv      = dev->priv_data;
    curr_chip = priv->current_flash;

    flash_write_enable(dev, hw_reg);
    wm_ll_flash_reset_cmd_info(hw_reg, 0);
    wm_ll_flash_set_cmd_info_cmd(hw_reg, curr_chip->cmd_set.ce.commd_id);
    wm_ll_flash_set_cmd_info_is_se(hw_reg, 1);
    flash_exec_cmd(hw_reg);
}

int wm_hal_flash_init(wm_hal_flash_dev_t *dev)
{
    wm_flash_reg_t *hw_reg     = NULL;
    const wm_flash_t *cur_chip = NULL;
    wm_flash_priv_t *priv      = NULL;
    int ret                    = 0;

    if (!dev->reg_base) {
        ret = WM_ERR_INVALID_PARAM;
        goto out;
    }
    hw_reg = (wm_flash_reg_t *)dev->reg_base;

    cur_chip = wm_iflash_get_current_chip(dev);
    if (cur_chip == NULL) {
        wm_fls_log_error("cannot support current flash chip: %s, use default flash chip\n", dev->chip_name);
        cur_chip = &wm_iflash_chip_unknown;
    }

    //default not enable QPI mode
    wm_ll_flash_set_ctrl_qpim(hw_reg, 0);

    if (dev->quad_spi) {
        wm_ll_flash_set_ctrl_qiom(hw_reg, 1);
    }

    //for unknow flash chip model, mark these code temp
#if 0
    //check current chip vendor id
    if (wm_hal_flash_get_manufacturer_id(dev) != cur_chip->vendor_id) {
        wm_fls_log_error("vendor id error, flash:0x%x, support:0x%x\n", wm_hal_flash_get_manufacturer_id(dev),
                         cur_chip->vendor_id);
        ret = WM_ERR_FAILED;
        goto out;
    }
#endif

    dev->priv_data = wm_os_internal_malloc(sizeof(wm_flash_priv_t));
    if (dev->priv_data) {
        priv                = dev->priv_data;
        priv->current_flash = cur_chip;
        //get flash information
        wm_hal_flash_get_unique_id(dev, dev->device_info.unique_id);
        dev->device_info.page_size   = cur_chip->page_size;
        dev->device_info.sector_size = cur_chip->sector_size;
        dev->device_info.block_size  = cur_chip->block_size;
        dev->device_info.size        = wm_hal_flash_get_capacity(dev);
        dev->device_info.vendor_id   = cur_chip->vendor_id;
        ret                          = WM_ERR_SUCCESS;
    } else {
        ret = WM_ERR_NO_MEM;
    }

out:
    return ret;
}

int wm_hal_flash_deinit(wm_hal_flash_dev_t *dev)
{
    if (dev->priv_data) {
        wm_os_internal_free(dev->priv_data);
        dev->priv_data = NULL;
    }

    return WM_ERR_SUCCESS;
}

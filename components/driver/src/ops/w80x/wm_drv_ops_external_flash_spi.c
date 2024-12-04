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
#include "wm_drv_ops_external_flash_spi.h"
#include "wm_drv_spi_master.h"
#include "wm_drv_rcc.h"

#define WM_EFLASH_DEBUG 1
#if WM_EFLASH_DEBUG
#undef LOG_TAG
#define LOG_TAG                "efls"
#define wm_efls_log_info(...)  //wm_log_info(__VA_ARGS__)
#define wm_efls_log_debug(...) wm_log_debug(__VA_ARGS__)
#define wm_efls_log_error(...) wm_log_error(__VA_ARGS__)
#define WM_EFLS_LOG_MARKER     //wm_efls_log_debug("[%s %d]\n", __func__, __LINE__)
#else
#define wm_efls_log_info(...)
#define wm_efls_log_debug(...)
#define wm_efls_log_error(...)
#define WM_EFLS_LOG_MARKER
#endif

#define FLASH_MAX_WAIT_SPI_TIME 1000
#define FLASH_CMD_SIZE          1
#define FLASH_ADDR_SIZE         3

ATTRIBUTE_INLINE void swap_bytes(uint8_t *data, int len)
{
    uint8_t *bytes = (uint8_t *)data;
    uint8_t temp;

    for (int i = 0; i < len / 2; i++) {
        temp               = bytes[i];
        bytes[i]           = bytes[len - 1 - i];
        bytes[len - 1 - i] = temp;
    }
}

ATTRIBUTE_INLINE int start_spi_xfer(wm_device_t *dev, const wm_dt_hw_spim_dev_cfg_t *config, spim_transceive_t *desc)
{
    return wm_drv_spim_transceive_sync(dev, config, desc, FLASH_MAX_WAIT_SPI_TIME);
}

static int wm_spi_flash_exec_basic_cmd(wm_device_t *dev, eflash_cmd_t cmd_id, uint8_t *rx_buf, uint8_t rx_len)
{
    int ret                      = 0;
    spim_transceive_ex_t desc_ex = { 0 };
    wm_drv_eflash_ctx_t *drv_ctx = NULL;
    wm_dt_hw_eflash_t *hw        = NULL;

    desc_ex.cmd     = cmd_id.commd_id;
    desc_ex.cmd_len = FLASH_CMD_SIZE;
    desc_ex.basic.flags |= SPI_TRANS_VARIABLE_CMD;

    desc_ex.basic.tx_buf = NULL;
    desc_ex.basic.tx_len = 0;
    if (rx_len) {
        desc_ex.basic.rx_buf = rx_buf;
        desc_ex.basic.rx_len = rx_len;
    }
    if (cmd_id.dummy_bit) {
        desc_ex.basic.flags |= SPI_TRANS_DUMMY_BITS;
        desc_ex.dummy_bits = cmd_id.dummy_bit * 8;
    }

    drv_ctx = dev->drv;
    hw      = (wm_dt_hw_eflash_t *)dev->hw;

    ret = start_spi_xfer(drv_ctx->spi_device, &hw->spi_cfg, &desc_ex.basic);

    return ret;
}

int wm_spi_flash_wait_idle(wm_device_t *dev, const wm_eflash_t *fls)
{
#define FLASH_IDLE_BIT 0x1
    wm_drv_eflash_ctx_t *drv_ctx = dev->drv;
    uint8_t rdsr1                = 0;
    uint32_t turnaround_cnt      = 0;
    uint32_t max_support_us      = 0;
    uint32_t load_value          = 0;
    uint32_t cpu_clock           = 0;
    uint32_t start_tick = 0, cur_tick = 0;
    uint8_t stat_case = 0;
    int ret           = WM_ERR_SUCCESS;

    cpu_clock      = wm_drv_rcc_get_config_clock(drv_ctx->rcc_device, WM_RCC_TYPE_CPU);
    load_value     = csi_coret_get_load();
    max_support_us = load_value / cpu_clock;
    turnaround_cnt = 1000 * 1000 / max_support_us;
    start_tick     = csi_coret_get_load() / 2;
    turnaround_cnt += turnaround_cnt;
    cur_tick = csi_coret_get_value();

    wm_spi_flash_exec_basic_cmd(dev, fls->cmd_set.rdsr1, &rdsr1, 1);

    while ((rdsr1 & FLASH_IDLE_BIT) && turnaround_cnt) { //max wailt 1000ms
        wm_spi_flash_exec_basic_cmd(dev, fls->cmd_set.rdsr1, &rdsr1, 1);

        cur_tick = csi_coret_get_value();
        if (stat_case == 1) {
            if (start_tick > cur_tick) {
                stat_case = 2;
                turnaround_cnt--;
            }
        } else {
            if (start_tick <= cur_tick) {
                stat_case = 1;
                turnaround_cnt--;
            }
        }
    }
    if (!turnaround_cnt) {
        wm_log_error("wait idle timeout\n");
        ret = WM_ERR_TIMEOUT;
    }

    return ret;
}

int wm_spi_flash_wait_wren(wm_device_t *dev, const wm_eflash_t *fls)
{
#define FLASH_WREN_BIT 0x2

    uint8_t rdsr1                = 0;
    wm_drv_eflash_ctx_t *drv_ctx = dev->drv;
    uint32_t turnaround_cnt      = 0;
    uint32_t max_support_us      = 0;
    uint32_t load_value          = 0;
    uint32_t cpu_clock           = 0;
    uint32_t start_tick = 0, cur_tick = 0;
    uint8_t stat_case = 0;
    int ret           = WM_ERR_SUCCESS;

    cpu_clock = wm_drv_rcc_get_config_clock(drv_ctx->rcc_device, WM_RCC_TYPE_CPU);
    if (cpu_clock != 0) {
        load_value     = csi_coret_get_load();
        max_support_us = load_value / cpu_clock;
        turnaround_cnt = 1000 * 1000 / max_support_us;
        start_tick     = csi_coret_get_load() / 2;
        turnaround_cnt += turnaround_cnt;
        cur_tick = csi_coret_get_value();

        wm_spi_flash_exec_basic_cmd(dev, fls->cmd_set.rdsr1, &rdsr1, 1);

        while (!(rdsr1 & FLASH_WREN_BIT) && turnaround_cnt) { //max wait 1000ms
            wm_spi_flash_exec_basic_cmd(dev, fls->cmd_set.rdsr1, &rdsr1, 1);

            cur_tick = csi_coret_get_value();
            if (stat_case == 1) {
                if (start_tick > cur_tick) {
                    stat_case = 2;
                    turnaround_cnt--;
                }
            } else {
                if (start_tick <= cur_tick) {
                    stat_case = 1;
                    turnaround_cnt--;
                }
            }
        }

        if (!turnaround_cnt) {
            wm_log_error("wait wren timeout\n");
            ret = WM_ERR_TIMEOUT;
        }
    } else {
        wm_log_error("get cpu clock error\n");
        ret = WM_ERR_TIMEOUT;
    }

    return ret;
}

static int wm_spi_flash_write_en(wm_device_t *dev, const wm_eflash_t *fls)
{
    wm_spi_flash_exec_basic_cmd(dev, fls->cmd_set.wren, NULL, 0);
    return wm_spi_flash_wait_wren(dev, fls);
}

int wm_spi_flash_program_pages(wm_device_t *dev, const wm_eflash_t *fls, uint32_t flash_addr, uint8_t *buf,
                               uint32_t write_page_num)
{
    int ret                      = 0;
    wm_drv_eflash_ctx_t *drv_ctx = NULL;
    const eflash_cmd_t *fls_cmd  = &(fls->cmd_set.pp);
    spim_transceive_ex_t desc_ex = { 0 };
    wm_dt_hw_eflash_t *hw        = NULL;
    uint32_t tmp_addr            = flash_addr;

    desc_ex.cmd     = fls_cmd->commd_id;
    desc_ex.cmd_len = FLASH_CMD_SIZE;
    desc_ex.basic.flags |= SPI_TRANS_VARIABLE_CMD;
    swap_bytes((uint8_t *)&tmp_addr, FLASH_ADDR_SIZE);
    desc_ex.addr     = tmp_addr;
    desc_ex.addr_len = FLASH_ADDR_SIZE;
    desc_ex.basic.flags |= SPI_TRANS_VARIABLE_ADDR;
    if (fls_cmd->dummy_bit) {
        desc_ex.dummy_bits = fls_cmd->dummy_bit * 8;
        desc_ex.basic.flags |= SPI_TRANS_DUMMY_BITS;
    }
    desc_ex.basic.tx_buf = buf;
    desc_ex.basic.tx_len = fls->page_size;
    desc_ex.basic.rx_buf = NULL;
    desc_ex.basic.rx_len = 0;

    drv_ctx = dev->drv;
    hw      = (wm_dt_hw_eflash_t *)dev->hw;
    while (write_page_num) {
        ret = wm_spi_flash_write_en(dev, fls);
        if (ret != WM_ERR_SUCCESS) {
            break;
        }

        ret = start_spi_xfer(drv_ctx->spi_device, &hw->spi_cfg, &(desc_ex.basic));
        if (ret != WM_ERR_SUCCESS) {
            break;
        }

        ret = wm_spi_flash_wait_idle(dev, fls);
        if (ret != WM_ERR_SUCCESS) {
            break;
        }

        write_page_num--;
        desc_ex.basic.tx_buf += fls->page_size;
        flash_addr += fls->page_size;
        tmp_addr = flash_addr;
        swap_bytes((uint8_t *)&tmp_addr, FLASH_ADDR_SIZE);
        desc_ex.addr = tmp_addr;
    }

    return ret;
}

int wm_spi_flash_read_pages(wm_device_t *dev, const wm_eflash_t *fls, uint32_t flash_addr, uint8_t *read_buf,
                            uint32_t read_page_num)
{
    wm_drv_eflash_ctx_t *drv_ctx = NULL;
    const eflash_cmd_t *fls_cmd  = &(fls->cmd_set.fread);
    spim_transceive_ex_t desc_ex = { 0 };
    wm_dt_hw_eflash_t *hw        = NULL;
    int ret                      = 0;
    uint32_t tmp_addr            = flash_addr;

    desc_ex.cmd     = fls_cmd->commd_id;
    desc_ex.cmd_len = FLASH_CMD_SIZE;
    desc_ex.basic.flags |= SPI_TRANS_VARIABLE_CMD;
    swap_bytes((uint8_t *)&tmp_addr, FLASH_ADDR_SIZE);
    desc_ex.addr     = tmp_addr;
    desc_ex.addr_len = FLASH_ADDR_SIZE;
    desc_ex.basic.flags |= SPI_TRANS_VARIABLE_ADDR;
    if (fls_cmd->dummy_bit) {
        desc_ex.dummy_bits = fls_cmd->dummy_bit * 8;
        desc_ex.basic.flags |= SPI_TRANS_DUMMY_BITS;
    }
    desc_ex.basic.tx_buf = NULL;
    desc_ex.basic.tx_len = 0;
    desc_ex.basic.rx_buf = read_buf;
    desc_ex.basic.rx_len = fls->page_size;
    drv_ctx              = dev->drv;
    hw                   = (wm_dt_hw_eflash_t *)dev->hw;
    while (read_page_num) {
        ret = start_spi_xfer(drv_ctx->spi_device, &hw->spi_cfg, &desc_ex.basic);
        if (ret != WM_ERR_SUCCESS) {
            break;
        }
        ret = wm_spi_flash_wait_idle(dev, fls);
        if (ret != WM_ERR_SUCCESS) {
            break;
        }
        flash_addr += fls->page_size;
        tmp_addr = flash_addr;
        swap_bytes((uint8_t *)&tmp_addr, FLASH_ADDR_SIZE);
        desc_ex.addr = tmp_addr;
        desc_ex.basic.rx_buf += fls->page_size;
        read_page_num--;
    }

    return ret;
}

int wm_spi_flash_read_sectors(wm_device_t *dev, const wm_eflash_t *fls, uint32_t flash_addr, uint8_t *read_buf,
                              uint32_t read_sector_num)
{
    wm_drv_eflash_ctx_t *drv_ctx = NULL;
    const eflash_cmd_t *fls_cmd  = &(fls->cmd_set.fread);
    spim_transceive_ex_t desc_ex = { 0 };
    wm_dt_hw_eflash_t *hw        = NULL;
    int ret                      = WM_ERR_SUCCESS;
    uint32_t tmp_addr            = flash_addr;

    desc_ex.cmd     = fls_cmd->commd_id;
    desc_ex.cmd_len = FLASH_CMD_SIZE;
    desc_ex.basic.flags |= SPI_TRANS_VARIABLE_CMD;
    swap_bytes((uint8_t *)&tmp_addr, FLASH_ADDR_SIZE);
    desc_ex.addr     = tmp_addr;
    desc_ex.addr_len = FLASH_ADDR_SIZE;
    desc_ex.basic.flags |= SPI_TRANS_VARIABLE_ADDR;
    if (fls_cmd->dummy_bit) {
        desc_ex.dummy_bits = fls_cmd->dummy_bit * 8;
        desc_ex.basic.flags |= SPI_TRANS_DUMMY_BITS;
    }
    desc_ex.basic.tx_buf = NULL;
    desc_ex.basic.tx_len = 0;
    desc_ex.basic.rx_buf = read_buf;
    desc_ex.basic.rx_len = fls->sector_size;

    drv_ctx = dev->drv;
    hw      = (wm_dt_hw_eflash_t *)dev->hw;
    while (read_sector_num) {
        ret = start_spi_xfer(drv_ctx->spi_device, &hw->spi_cfg, &desc_ex.basic);
        if (ret != WM_ERR_SUCCESS) {
            break;
        }
        ret = wm_spi_flash_wait_idle(dev, fls);
        if (ret != WM_ERR_SUCCESS) {
            break;
        }

        flash_addr += fls->sector_size;
        tmp_addr = flash_addr;
        swap_bytes((uint8_t *)&tmp_addr, FLASH_ADDR_SIZE);
        desc_ex.addr = tmp_addr;
        desc_ex.basic.rx_buf += fls->sector_size;
        read_sector_num--;
    }

    return ret;
}

int wm_spi_flash_erase_sectors(wm_device_t *dev, const wm_eflash_t *fls, uint32_t flash_addr, uint32_t sector_num)
{
    wm_drv_eflash_ctx_t *drv_ctx = NULL;
    const eflash_cmd_t *fls_cmd  = &(fls->cmd_set.se);
    spim_transceive_ex_t desc_ex = { 0 };
    wm_dt_hw_eflash_t *hw        = NULL;
    int ret                      = 0;
    uint32_t tmp_addr            = flash_addr;

    desc_ex.cmd     = fls_cmd->commd_id;
    desc_ex.cmd_len = FLASH_CMD_SIZE;
    desc_ex.basic.flags |= SPI_TRANS_VARIABLE_CMD;
    swap_bytes((uint8_t *)&tmp_addr, FLASH_ADDR_SIZE);
    desc_ex.addr     = tmp_addr;
    desc_ex.addr_len = FLASH_ADDR_SIZE;
    desc_ex.basic.flags |= SPI_TRANS_VARIABLE_ADDR;
    if (fls_cmd->dummy_bit) {
        desc_ex.dummy_bits = fls_cmd->dummy_bit * 8;
        desc_ex.basic.flags |= SPI_TRANS_DUMMY_BITS;
    }
    desc_ex.basic.tx_buf = NULL;
    desc_ex.basic.tx_len = 0;
    desc_ex.basic.rx_buf = NULL;
    desc_ex.basic.rx_len = 0;
    drv_ctx              = dev->drv;
    hw                   = (wm_dt_hw_eflash_t *)dev->hw;
    while (sector_num) {
        ret = wm_spi_flash_write_en(dev, fls);
        if (ret != WM_ERR_SUCCESS) {
            break;
        }
        ret = start_spi_xfer(drv_ctx->spi_device, &hw->spi_cfg, &desc_ex.basic);
        ret = wm_spi_flash_wait_idle(dev, fls);
        if (ret != WM_ERR_SUCCESS) {
            break;
        }

        sector_num--;
        flash_addr += fls->sector_size;
        tmp_addr = flash_addr;
        swap_bytes((uint8_t *)&tmp_addr, FLASH_ADDR_SIZE);
        desc_ex.addr = tmp_addr;
    }

    return ret;
}

int wm_spi_flash_erase_chip(wm_device_t *dev, const wm_eflash_t *fls)
{
    int ret = WM_ERR_SUCCESS;

    ret = wm_spi_flash_write_en(dev, fls);
    if (ret != WM_ERR_SUCCESS) {
        return ret;
    }

    wm_spi_flash_exec_basic_cmd(dev, fls->cmd_set.ce, NULL, 0);
    wm_spi_flash_wait_idle(dev, fls);

    return ret;
}

int wm_spi_flash_get_unique_id(wm_device_t *dev, const wm_eflash_t *fls, uint32_t uuid[4])
{
    int ret                      = 0;
    wm_drv_eflash_ctx_t *drv_ctx = NULL;
    spim_transceive_ex_t desc_ex = { 0 };
    wm_dt_hw_eflash_t *hw        = NULL;
    const eflash_cmd_t *fls_cmd  = &(fls->cmd_set.ruid);

    desc_ex.cmd     = fls_cmd->commd_id;
    desc_ex.cmd_len = FLASH_CMD_SIZE;
    desc_ex.basic.flags |= SPI_TRANS_VARIABLE_CMD;

    if (fls_cmd->dummy_bit) {
        desc_ex.dummy_bits = fls_cmd->dummy_bit * 8;
        desc_ex.basic.flags |= SPI_TRANS_DUMMY_BITS;
    }

    desc_ex.basic.tx_buf = NULL;
    desc_ex.basic.tx_len = 0;
    desc_ex.basic.rx_buf = (uint8_t *)uuid;
    desc_ex.basic.rx_len = 16;

    drv_ctx = dev->drv;
    hw      = (wm_dt_hw_eflash_t *)dev->hw;
    ret     = start_spi_xfer(drv_ctx->spi_device, &hw->spi_cfg, &desc_ex.basic);

    return ret;
}

/**
**  Capacity  |   memory type | manufacturer ID
**      1B    |     1B        |    1B
*/
/*output JEDEC ID: 1-byte manufacturer ID & 2-byte device ID*/

uint8_t wm_spi_flash_get_manufacturer_id(wm_device_t *dev, const wm_eflash_t *fls, uint8_t *manufacturer_id)
{
    wm_drv_eflash_ctx_t *drv_ctx = NULL;
    spim_transceive_ex_t desc_ex = { 0 };
    wm_dt_hw_eflash_t *hw        = NULL;
    const eflash_cmd_t *fls_cmd  = &(fls->cmd_set.rdid);
    int ret                      = 0;

    desc_ex.cmd     = fls_cmd->commd_id;
    desc_ex.cmd_len = FLASH_CMD_SIZE;
    desc_ex.basic.flags |= SPI_TRANS_VARIABLE_CMD;

    if (fls_cmd->dummy_bit) {
        desc_ex.dummy_bits = fls_cmd->dummy_bit * 8;
        desc_ex.basic.flags |= SPI_TRANS_DUMMY_BITS;
    }

    desc_ex.basic.tx_buf = NULL;
    desc_ex.basic.tx_len = 0;
    desc_ex.basic.rx_buf = manufacturer_id;
    desc_ex.basic.rx_len = 1;

    drv_ctx = dev->drv;
    hw      = (wm_dt_hw_eflash_t *)dev->hw;
    ret     = start_spi_xfer(drv_ctx->spi_device, &hw->spi_cfg, &desc_ex.basic);

    return ret;
}

/**
**  Capacity  |   memory type | manufacturer ID
**      1B    |     1B        |    1B
*/
int wm_spi_flash_get_capacity(wm_device_t *dev, const wm_eflash_t *fls, uint32_t *flash_size)
{
    wm_drv_eflash_ctx_t *drv_ctx = NULL;
    spim_transceive_ex_t desc_ex = { 0 };
    wm_dt_hw_eflash_t *hw        = NULL;
    const eflash_cmd_t *fls_cmd  = &(fls->cmd_set.rdid);
    uint32_t read_data           = 0;
    int ret                      = 0;

    desc_ex.cmd     = fls_cmd->commd_id;
    desc_ex.cmd_len = FLASH_CMD_SIZE;
    desc_ex.basic.flags |= SPI_TRANS_VARIABLE_CMD;

    if (fls_cmd->dummy_bit) {
        desc_ex.dummy_bits = fls_cmd->dummy_bit * 8;
        desc_ex.basic.flags |= SPI_TRANS_DUMMY_BITS;
    }

    desc_ex.basic.tx_buf = NULL;
    desc_ex.basic.tx_len = 0;
    desc_ex.basic.rx_buf = (uint8_t *)&read_data;
    desc_ex.basic.rx_len = 3;

    drv_ctx = dev->drv;
    hw      = (wm_dt_hw_eflash_t *)dev->hw;
    ret     = start_spi_xfer(drv_ctx->spi_device, &hw->spi_cfg, &desc_ex.basic);

    *flash_size = (1 << (read_data >> 16 & 0xFF)); // get capacity, it's only one byte

    wm_log_info("%s size=%u", dev->name, *flash_size);

    return ret;
}

int wm_spi_flash_init(wm_device_t *dev, const wm_eflash_t *fls)
{
    return 0;
}

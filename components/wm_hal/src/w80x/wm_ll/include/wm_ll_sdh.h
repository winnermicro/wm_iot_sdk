/**
 * @file wm_ll_sdh.h
 *
 * @brief SD/SDIO host low level operation
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

#ifndef __WM_LL_SDH_H__
#define __WM_LL_SDH_H__

#include "wm_attr.h"
#include "wm_types.h"
#include "wm_sdh_reg_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get port mode from SD host mmc control register
 *
 * @param reg Pointer to the SD host mmc control register
 * @return Port mode value
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_port_mode(wm_sdh_reg_t *reg)
{
    return reg->mmc_ctrl.port_mode;
}

/**
 * @brief Set port mode in SD host mmc control register
 *
 * @param reg Pointer to the SD host mmc control register
 * @param value Port mode value to set
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_port_mode(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_ctrl.port_mode = value;
}

/**
 * @brief Get signal mode from SD host mmc control register
 *
 * @param reg Pointer to the SD host mmc control register
 * @return Signal mode value
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_sig_mode(wm_sdh_reg_t *reg)
{
    return reg->mmc_ctrl.sig_mode;
}

/**
 * @brief Set signal mode in SD host mmc control register
 *
 * @param reg Pointer to the SD host mmc control register
 * @param value Signal mode value to set
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_sig_mode(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_ctrl.sig_mode = value;
}

/**
 * @brief Get SDIO drive mode selection from SD host mmc control register
 *
 * @param reg Pointer to the SD host mmc control register
 * @return SDIO drive mode selection value
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_sdio_drive_mode_sel(wm_sdh_reg_t *reg)
{
    return reg->mmc_ctrl.sdio_drive_mode_sel;
}

/**
 * @brief Set SDIO drive mode selection in SD host mmc control register
 *
 * @param reg Pointer to the SD host mmc control register
 * @param value SDIO drive mode selection value to set
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_sdio_drive_mode_sel(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_ctrl.sdio_drive_mode_sel = value;
}

/**
 * @brief Get SDIO port clock selection from SD host mmc control register
 *
 * @param reg Pointer to the SD host mmc control register
 * @return SDIO port clock selection value
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_sdio_port_clock_sel(wm_sdh_reg_t *reg)
{
    return reg->mmc_ctrl.sdio_port_clock_sel;
}

/**
 * @brief Set SDIO port clock selection in SD host mmc control register
 *
 * @param reg Pointer to the SD host mmc control register
 * @param value SDIO port clock selection value to set
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_sdio_port_clock_sel(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_ctrl.sdio_port_clock_sel = value;
}

/**
 * @brief Get SDIO transfer mode from SD host mmc control register
 *
 * @param reg Pointer to the SD host mmc control register
 * @return SDIO transfer mode value
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_sdio_transfer_mode(wm_sdh_reg_t *reg)
{
    return reg->mmc_ctrl.sdio_transfer_mode;
}

/**
 * @brief Set SDIO transfer mode in SD host mmc control register
 *
 * @param reg Pointer to the SD host mmc control register
 * @param value SDIO transfer mode value to set
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_sdio_transfer_mode(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_ctrl.sdio_transfer_mode = value;
}

/**
 * @brief Get SDIO bus width from SD host mmc control register
 *
 * @param reg Pointer to the SD host mmc control register
 * @return SDIO bus width value
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_sdio_bus_width(wm_sdh_reg_t *reg)
{
    return reg->mmc_ctrl.sdio_bus_width;
}

/**
 * @brief Set SDIO bus width in SD host mmc control register
 *
 * @param reg Pointer to the SD host mmc control register
 * @param value SDIO bus width value to set
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_sdio_bus_width(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_ctrl.sdio_bus_width = value;
}

/**
 * @brief Get SDIO mode enable from SD host mmc control register
 *
 * @param reg Pointer to the SD host mmc control register
 * @return SDIO mode enable value
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_sdio_mode_en(wm_sdh_reg_t *reg)
{
    return reg->mmc_ctrl.sdio_mode_en;
}

/**
 * @brief Set SDIO mode enable in SD host mmc control register
 *
 * @param reg Pointer to the SD host mmc control register
 * @param value SDIO mode enable value to set
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_sdio_mode_en(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_ctrl.sdio_mode_en = value;
}

/**
 * @brief Get SDIO interrupt enable from SD host mmc control register
 *
 * @param reg Pointer to the SD host mmc control register
 * @return SDIO interrupt enable value
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_sdio_int_en(wm_sdh_reg_t *reg)
{
    return reg->mmc_ctrl.sdio_int_en;
}

/**
 * @brief Set SDIO interrupt enable in SD host mmc control register
 *
 * @param reg Pointer to the SD host mmc control register
 * @param value SDIO interrupt enable value to set
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_sdio_int_en(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_ctrl.sdio_int_en = value;
}

/**
 * @brief Get SDIO read wait enable from SD host mmc control register
 *
 * @param reg Pointer to the SD host mmc control register
 * @return SDIO read wait enable value
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_sdio_read_wait_en(wm_sdh_reg_t *reg)
{
    return reg->mmc_ctrl.sdio_read_wait_en;
}

/**
 * @brief Set SDIO read wait enable in SD host mmc control register
 *
 * @param reg Pointer to the SD host mmc control register
 * @param value SDIO read wait enable value to set
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_sdio_read_wait_en(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_ctrl.sdio_read_wait_en = value;
}

/**
 * @brief Get auto data transfer from SD host mmc IO control register
 *
 * @param reg Pointer to the SD host mmc IO control register
 * @return Auto data transfer value
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_auto_data_trans(wm_sdh_reg_t *reg)
{
    return reg->mmc_io.auto_data_trans;
}

/**
 * @brief Set auto data transfer in SD host mmc IO control register
 *
 * @param reg Pointer to the SD host mmc IO control register
 * @param value Auto data transfer value to set
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_auto_data_trans(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_io.auto_data_trans = value;
}

/**
 * @brief Get data direction from SD host mmc IO control register
 *
 * @param reg Pointer to the SD host mmc IO control register
 * @return Data direction value
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_data_dir(wm_sdh_reg_t *reg)
{
    return reg->mmc_io.data_dir;
}

/**
 * @brief Set data direction in SD host mmc IO control register
 *
 * @param reg Pointer to the SD host mmc IO control register
 * @param value Data direction value to set
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_data_dir(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_io.data_dir = value;
}

/**
 * @brief Get auto 8 null clock generation from SD host mmc IO control register
 *
 * @param reg Pointer to the SD host mmc IO control register
 * @return Auto 8 null clock generation value
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_auto_8_null_clk_gen(wm_sdh_reg_t *reg)
{
    return reg->mmc_io.auto_8_null_clk_gen;
}

/**
 * @brief Set auto 8 null clock generation in SD host mmc IO control register
 *
 * @param reg Pointer to the SD host mmc IO control register
 * @param value Auto 8 null clock generation value to set
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_auto_8_null_clk_gen(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_io.auto_8_null_clk_gen = value;
}

/**
 * @brief Get command select from SD host mmc IO control register
 *
 * @param reg Pointer to the SD host mmc IO control register
 * @return Command select value
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_cmd_sel(wm_sdh_reg_t *reg)
{
    return reg->mmc_io.cmd_sel;
}

/**
 * @brief Set command select in SD host mmc IO control register
 *
 * @param reg Pointer to the SD host mmc IO control register
 * @param value Command select value to set
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_cmd_sel(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_io.cmd_sel = value;
}

/**
 * @brief Get CID/CSD read from SD host mmc IO control register
 *
 * @param reg Pointer to the SD host mmc IO control register
 * @return CID/CSD read value
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_cid_csd_read(wm_sdh_reg_t *reg)
{
    return reg->mmc_io.cid_csd_read;
}

/**
 * @brief Set CID/CSD read in SD host mmc IO control register
 *
 * @param reg Pointer to the SD host mmc IO control register
 * @param value CID/CSD read value to set
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_cid_csd_read(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_io.cid_csd_read = value;
}

/**
 * @brief Get port clock generation from SD host mmc IO control register
 *
 * @param reg Pointer to the SD host mmc IO control register
 * @return Port clock generation value
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_port_clk_gen(wm_sdh_reg_t *reg)
{
    return reg->mmc_io.port_clk_gen;
}

/**
 * @brief Set port clock generation in SD host mmc IO control register
 *
 * @param reg Pointer to the SD host mmc IO control register
 * @param value Port clock generation value to set
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_port_clk_gen(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_io.port_clk_gen = value;
}

/**
 * @brief Get auto response receive from SD host mmc IO control register
 *
 * @param reg Pointer to the SD host mmc IO control register
 * @return Auto response receive value
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_auto_resp_recv(wm_sdh_reg_t *reg)
{
    return reg->mmc_io.auto_resp_recv;
}

/**
 * @brief Set auto response receive in SD host mmc IO control register
 *
 * @param reg Pointer to the SD host mmc IO control register
 * @param value Auto response receive value to set
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_auto_resp_recv(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_io.auto_resp_recv = value;
}

/**
 * @brief Get auto null clock generation/response/command transfer from SD host mmc IO control register
 *
 * @param reg Pointer to the SD host mmc IO control register
 * @return Auto null clock generation/response/command transfer value
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_auto_null_clk_gen_resp_cmd_trans(wm_sdh_reg_t *reg)
{
    return reg->mmc_io.auto_null_clk_gen_resp_cmd_trans;
}

/**
 * @brief Set auto null clock generation/response/command transfer in SD host mmc IO control register
 *
 * @param reg Pointer to the SD host mmc IO control register
 * @param value Auto null clock generation/response/command transfer value to set
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_auto_null_clk_gen_resp_cmd_trans(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_io.auto_null_clk_gen_resp_cmd_trans = value;
}

/**
 * @brief Get command attribute from SD host mmc IO control register
 *
 * @param reg Pointer to the SD host mmc IO control register
 * @return Command attribute value
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_cmd_attr(wm_sdh_reg_t *reg)
{
    return reg->mmc_io.cmd_attr;
}

/**
 * @brief Set command attribute in SD host mmc IO control register
 *
 * @param reg Pointer to the SD host mmc IO control register
 * @param value Command attribute value to set
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_cmd_attr(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_io.cmd_attr = value;
}

/**
 * @brief Get command abort flag from SD host mmc IO control register
 *
 * @param reg Pointer to the SD host mmc IO control register
 * @return Command abort flag value
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_cmd_abort_flag(wm_sdh_reg_t *reg)
{
    return reg->mmc_io.cmd_abort_flag;
}

/**
 * @brief Set command abort flag in SD host mmc IO control register
 *
 * @param reg Pointer to the SD host mmc IO control register
 * @param value Command abort flag value to set
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_cmd_abort_flag(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_io.cmd_abort_flag = value;
}

/**
 * @brief Get SD host mmc IO control register
 *
 * @param reg Pointer to the SD host mmc IO control register
 * @return mmc io value
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_mmc_io(wm_sdh_reg_t *reg)
{
    return reg->mmc_io.val;
}

/**
 * @brief Set SD host mmc IO control register
 *
 * @param reg Pointer to the SD host mmc IO control register
 * @param value mmc io value to set
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_mmc_io(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_io.val = value;
}

/**
 * @brief Get byte count for data transfer
 *
 * @param reg Pointer to SD host byte count control register
 * @return Byte count value
 */
ATTRIBUTE_INLINE uint16_t wm_ll_sdh_mmc_get_byte_count(wm_sdh_reg_t *reg)
{
    return WM_LL_REG_FILED_COPY_READ(reg->mmc_bytecntl, byte_count);
}

/**
 * @brief Set byte count for data transfer
 *
 * @param reg Pointer to SD host byte count control register
 * @param value Byte count value to set
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_byte_count(wm_sdh_reg_t *reg, uint32_t value)
{
    WM_LL_REG_FILED_COPY_WRTIE(reg->mmc_bytecntl, byte_count, value);
}

/**
 * @brief Get block count for multi-block transfer
 *
 * @param reg Pointer to SD host block count control register
 * @return Block count value
 */
ATTRIBUTE_INLINE uint16_t wm_ll_sdh_mmc_get_block_count(wm_sdh_reg_t *reg)
{
    return WM_LL_REG_FILED_COPY_READ(reg->mmc_tr_blockcnt, block_count);
}

/**
 * @brief Get data CRC error status
 *
 * @param reg Pointer to SD host CRC control register
 * @return Data CRC error status (1 bit)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_dat_crc_err(wm_sdh_reg_t *reg)
{
    return reg->mmc_crcctl.dat_crc_err;
}

/**
 * @brief Get command CRC error status
 *
 * @param reg Pointer to SD host CRC control register
 * @return Command CRC error status (1 bit)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_cmd_crc_err(wm_sdh_reg_t *reg)
{
    return reg->mmc_crcctl.cmd_crc_err;
}

/**
 * @brief Get data CRC selection
 *
 * @param reg Pointer to SD host CRC control register
 * @return Data CRC selection (2 bits)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_dat_crc_sel(wm_sdh_reg_t *reg)
{
    return reg->mmc_crcctl.dat_crc_sel;
}

/**
 * @brief Set data CRC selection
 *
 * @param reg Pointer to SD host CRC control register
 * @param value Data CRC selection value to set (2 bits)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_dat_crc_sel(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_crcctl.dat_crc_sel = value;
}

/**
 * @brief Get read before response in multi-block status
 *
 * @param reg Pointer to SD host CRC control register
 * @return Read before response in multi-block status (1 bit)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_read_before_resp(wm_sdh_reg_t *reg)
{
    return reg->mmc_crcctl.read_before_resp;
}

/**
 * @brief Set read before response in multi-block status
 *
 * @param reg Pointer to SD host CRC control register
 * @param value Read before response in multi-block status value to set (1 bit)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_read_before_resp(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_crcctl.read_before_resp = value;
}

/**
 * @brief Get automatic CRC check status
 *
 * @param reg Pointer to SD host CRC control register
 * @return Automatic CRC check status (1 bit)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_auto_crc_check(wm_sdh_reg_t *reg)
{
    return reg->mmc_crcctl.auto_crc_check;
}

/**
 * @brief Set automatic CRC check status
 *
 * @param reg Pointer to SD host CRC control register
 * @param value Automatic CRC check status value to set (1 bit)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_auto_crc_check(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_crcctl.auto_crc_check = value;
}

/**
 * @brief Get data line CRC function enable status
 *
 * @param reg Pointer to SD host CRC control register
 * @return Data line CRC function enable status (1 bit)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_data_line_crc_en(wm_sdh_reg_t *reg)
{
    return reg->mmc_crcctl.data_line_crc_en;
}

/**
 * @brief Set data line CRC function enable status
 *
 * @param reg Pointer to SD host CRC control register
 * @param value Data line CRC function enable status value to set (1 bit)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_data_line_crc_en(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_crcctl.data_line_crc_en = value;
}

/**
 * @brief Get command line CRC function enable status
 *
 * @param reg Pointer to SD host CRC control register
 * @return Command line CRC function enable status (1 bit)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_cmd_line_crc_en(wm_sdh_reg_t *reg)
{
    return reg->mmc_crcctl.cmd_line_crc_en;
}

/**
 * @brief Set command line CRC function enable status
 *
 * @param reg Pointer to SD host CRC control register
 * @param value Command line CRC function enable status value to set (1 bit)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_cmd_line_crc_en(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_crcctl.cmd_line_crc_en = value;
}

/**
 * @brief Get command CRC value
 *
 * @param reg Pointer to SD host command CRC register
 * @return Command CRC value (7 bits)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_cmd_crc_value(wm_sdh_reg_t *reg)
{
    return reg->cmd_crc.cmd_crc_value;
}

/**
 * @brief Get DAT CRC low register value
 *
 * @param reg Pointer to SD host DAT CRC low register
 * @return DAT CRC low register value (8 bits)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_dat_crc_low(wm_sdh_reg_t *reg)
{
    return WM_LL_REG_FILED_COPY_READ(reg->dat_crcl, dat_crc_low);
}

/**
 * @brief Get DAT CRC high register value
 *
 * @param reg Pointer to SD host DAT CRC high register
 * @return DAT CRC high register value (8 bits)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_dat_crc_high(wm_sdh_reg_t *reg)
{
    return WM_LL_REG_FILED_COPY_READ(reg->dat_crch, dat_crc_high);
}

/**
 * @brief Get Ncr timeout count value
 *
 * @param reg Pointer to SD host MM port control register
 * @return Ncr timeout count value (4 bits)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_ncr_timeout_val(wm_sdh_reg_t *reg)
{
    return reg->mm_port.ncr_timeout_val;
}

/**
 * @brief Set Ncr timeout count value
 *
 * @param reg Pointer to SD host MM port control register
 * @param value Ncr timeout count value to set (4 bits)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_ncr_timeout_val(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mm_port.ncr_timeout_val = value;
}

/**
 * @brief Get automatic Ncr timeout check function status
 *
 * @param reg Pointer to SD host MM port control register
 * @return Automatic Ncr timeout check function status (1 bit)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_auto_ncr_timeout(wm_sdh_reg_t *reg)
{
    return reg->mm_port.auto_ncr_timeout;
}

/**
 * @brief Set automatic Ncr timeout check function status
 *
 * @param reg Pointer to SD host MM port control register
 * @param value Automatic Ncr timeout check function status value to set (1 bit)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_auto_ncr_timeout(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mm_port.auto_ncr_timeout = value;
}

/**
 * @brief Get SD/MMC/SDIO port data line signal status
 *
 * @param reg Pointer to SD host MM port control register
 * @return SD/MMC/SDIO port data line signal status (1 bit)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_port_data_signal(wm_sdh_reg_t *reg)
{
    return reg->mm_port.port_data_signal;
}

/**
 * @brief Set SD/MMC/SDIO port data line signal status
 *
 * @param reg Pointer to SD host MM port control register
 * @param value SD/MMC/SDIO port data line signal status value to set (1 bit)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_port_data_signal(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mm_port.port_data_signal = value;
}

/**
 * @brief Get SD/MMC/SDIO port command line signal status
 *
 * @param reg Pointer to SD host MM port control register
 * @return SD/MMC/SDIO port command line signal status (1 bit)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_port_cmd_signal(wm_sdh_reg_t *reg)
{
    return reg->mm_port.port_cmd_signal;
}

/**
 * @brief Set SD/MMC/SDIO port command line signal status
 *
 * @param reg Pointer to SD host MM port control register
 * @param value SD/MMC/SDIO port command line signal status value to set (1 bit)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_port_cmd_signal(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mm_port.port_cmd_signal = value;
}

/**
 * @brief Get SD/MMC/SDIO port clock line signal status
 *
 * @param reg Pointer to SD host MM port control register
 * @return SD/MMC/SDIO port clock line signal status (1 bit)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_port_clk_signal(wm_sdh_reg_t *reg)
{
    return reg->mm_port.port_clk_signal;
}

/**
 * @brief Set SD/MMC/SDIO port clock line signal status
 *
 * @param reg Pointer to SD host MM port control register
 * @param value SD/MMC/SDIO port clock line signal status value to set (1 bit)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_port_clk_signal(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mm_port.port_clk_signal = value;
}

/**
 * @brief Get cmd transfer complete interrupt mask status
 *
 * @param reg Pointer to SD host interrupt mask register
 * @return CMD transfer complete interrupt mask status (1 bit)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_cmd_complete_int_mask(wm_sdh_reg_t *reg)
{
    return reg->mmc_int_mask.cmd_complete_int_mask;
}

/**
 * @brief Set cmd transfer complete interrupt mask status
 *
 * @param reg Pointer to SD host interrupt mask register
 * @param value CMD transfer complete interrupt mask status value to set (1 bit)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_cmd_complete_int_mask(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_int_mask.cmd_complete_int_mask = value;
}

/**
 * @brief Get data transfer complete interrupt mask status
 *
 * @param reg Pointer to SD host interrupt mask register
 * @return Data transfer complete interrupt mask status (1 bit)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_data_complete_int_mask(wm_sdh_reg_t *reg)
{
    return reg->mmc_int_mask.data_complete_int_mask;
}

/**
 * @brief Set data transfer complete interrupt mask status
 *
 * @param reg Pointer to SD host interrupt mask register
 * @param value Data transfer complete interrupt mask status value to set (1 bit)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_data_complete_int_mask(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_int_mask.data_complete_int_mask = value;
}

/**
 * @brief Get data CRC error interrupt mask status
 *
 * @param reg Pointer to SD host interrupt mask register
 * @return Data CRC error interrupt mask status (1 bit)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_data_crc_err_int_mask(wm_sdh_reg_t *reg)
{
    return reg->mmc_int_mask.data_crc_err_int_mask;
}

/**
 * @brief Set data CRC error interrupt mask status
 *
 * @param reg Pointer to SD host interrupt mask register
 * @param value Data CRC error interrupt mask status value to set (1 bit)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_data_crc_err_int_mask(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_int_mask.data_crc_err_int_mask = value;
}

/**
 * @brief Get command CRC error interrupt mask status
 *
 * @param reg Pointer to SD host interrupt mask register
 * @return Command CRC error interrupt mask status (1 bit)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_cmd_crc_err_int_mask(wm_sdh_reg_t *reg)
{
    return reg->mmc_int_mask.cmd_crc_err_int_mask;
}

/**
 * @brief Set command CRC error interrupt mask status
 *
 * @param reg Pointer to SD host interrupt mask register
 * @param value Command CRC error interrupt mask status value to set (1 bit)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_cmd_crc_err_int_mask(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_int_mask.cmd_crc_err_int_mask = value;
}

/**
 * @brief Get multi-block transfer complete interrupt mask status
 *
 * @param reg Pointer to SD host interrupt mask register
 * @return Multi-block transfer complete interrupt mask status (1 bit)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_multi_block_tran_int_mask(wm_sdh_reg_t *reg)
{
    return reg->mmc_int_mask.multi_block_tran_int_mask;
}

/**
 * @brief Set multi-block transfer complete interrupt mask status
 *
 * @param reg Pointer to SD host interrupt mask register
 * @param value Multi-block transfer complete interrupt mask status value to set (1 bit)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_multi_block_tran_int_mask(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_int_mask.multi_block_tran_int_mask = value;
}

/**
 * @brief Get multi-block timeout interrupt mask status
 *
 * @param reg Pointer to SD host interrupt mask register
 * @return Multi-block timeout interrupt mask status (1 bit)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_multi_block_timeout_int_mask(wm_sdh_reg_t *reg)
{
    return reg->mmc_int_mask.multi_block_timeout_int_mask;
}

/**
 * @brief Set multi-block timeout interrupt mask status
 *
 * @param reg Pointer to SD host interrupt mask register
 * @param value Multi-block timeout interrupt mask status value to set (1 bit)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_multi_block_timeout_int_mask(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_int_mask.multi_block_timeout_int_mask = value;
}

/**
 * @brief Get command and response Ncr timeout interrupt mask status
 *
 * @param reg Pointer to SD host interrupt mask register
 * @return Command and response Ncr timeout interrupt mask status (1 bit)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_cmd_resp_ncr_timeout_int_mask(wm_sdh_reg_t *reg)
{
    return reg->mmc_int_mask.cmd_resp_ncr_timeout_int_mask;
}

/**
 * @brief Set command and response Ncr timeout interrupt mask status
 *
 * @param reg Pointer to SD host interrupt mask register
 * @param value Command and response Ncr timeout interrupt mask status value to set (1 bit)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_cmd_resp_ncr_timeout_int_mask(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_int_mask.cmd_resp_ncr_timeout_int_mask = value;
}

/**
 * @brief Get CRC status token error interrupt mask status
 *
 * @param reg Pointer to SD host interrupt mask register
 * @return CRC status token error interrupt mask status (1 bit)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_crc_status_token_err_int_mask(wm_sdh_reg_t *reg)
{
    return reg->mmc_int_mask.crc_status_token_err_int_mask;
}

/**
 * @brief Set CRC status token error interrupt mask status
 *
 * @param reg Pointer to SD host interrupt mask register
 * @param value CRC status token error interrupt mask status value to set (1 bit)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_crc_status_token_err_int_mask(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_int_mask.crc_status_token_err_int_mask = value;
}

/**
 * @brief Get SDIO data line 1 interrupt mask
 *
 * @param reg Pointer to sdio data1 interrupt mask register
 * @return interrupt mask status value
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_sdio_data1_int_mask(wm_sdh_reg_t *reg)
{
    return reg->mmc_int_mask.sdio_data1_int_mask;
}

/**
 * @brief Set SDIO data line 1 interrupt mask
 *
 * @param reg Pointer to sdio data1 interrupt mask register
 * @param value interrupt mask status value to set (1 bit)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_sdio_data1_int_mask(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_int_mask.sdio_data1_int_mask = value;
}

/**
 * @brief Get multi-block timeout interrupt clear status
 *
 * @param reg Pointer to SD host interrupt clear register
 * @return Multi-block timeout interrupt clear status (1 bit)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_multi_block_timeout_int_clr(wm_sdh_reg_t *reg)
{
    return reg->clr_mmc_int.multi_block_timeout_int_clr;
}

/**
 * @brief Clear multi-block timeout interrupt
 *
 * @param reg Pointer to SD host interrupt clear register
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_clr_multi_block_timeout_int(wm_sdh_reg_t *reg)
{
    reg->clr_mmc_int.multi_block_timeout_int_clr = 1;
}

/**
 * @brief Get command and response Ncr timeout interrupt clear status
 *
 * @param reg Pointer to SD host interrupt clear register
 * @return Command and response Ncr timeout interrupt clear status (1 bit)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_cmd_resp_ncr_timeout_int_clr(wm_sdh_reg_t *reg)
{
    return reg->clr_mmc_int.cmd_resp_ncr_timeout_int_clr;
}

/**
 * @brief Clear command and response Ncr timeout interrupt
 *
 * @param reg Pointer to SD host interrupt clear register
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_clr_cmd_resp_ncr_timeout_int(wm_sdh_reg_t *reg)
{
    reg->clr_mmc_int.cmd_resp_ncr_timeout_int_clr = 1;
}

/**
 * @brief Get CRC status token error interrupt clear status
 *
 * @param reg Pointer to SD host interrupt clear register
 * @return CRC status token error interrupt clear status (1 bit)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_crc_status_token_err_int_clr(wm_sdh_reg_t *reg)
{
    return reg->clr_mmc_int.crc_status_token_err_int_clr;
}

/**
 * @brief Clear CRC status token error interrupt
 *
 * @param reg Pointer to SD host interrupt clear register
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_clr_crc_status_token_err_int(wm_sdh_reg_t *reg)
{
    reg->clr_mmc_int.crc_status_token_err_int_clr = 1;
}

/**
 * @brief Clear SDIO data line 1 interrupt
 *
 * @param reg Pointer to SD host interrupt clear register
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_clr_sdio_data1_int(wm_sdh_reg_t *reg)
{
    reg->clr_mmc_int.clr_sdio_data1_int = 1;
}

/**
 * @brief          get sdh interrupt status
 * @param[in]      sdh hardware register base address
 * @return         int status
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_get_int_sts(wm_sdh_reg_t *reg)
{
    return reg->clr_mmc_int.val;
}

/**
 * @brief          set sdh interrupt status
 * @param[in]      sdh hardware register base address
 * @param[in]      interrupt status
 * @return         void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_set_int_sts(wm_sdh_reg_t *reg, uint32_t sts)
{
    reg->clr_mmc_int.val |= sts;
}

/**
 * @brief Get SD/MMC/SDIO time base coefficient
 *
 * @param reg Pointer to SD host card selection register
 * @return SD/MMC/SDIO time base coefficient (6 bits)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_time_base_coefficient(wm_sdh_reg_t *reg)
{
    return reg->mmc_cardsel.time_base_coefficient;
}

/**
 * @brief Set SD/MMC/SDIO time base coefficient
 *
 * @param reg Pointer to SD host card selection register
 * @param value SD/MMC/SDIO time base coefficient value to set (6 bits)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_time_base_coefficient(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_cardsel.time_base_coefficient = value;
}

/**
 * @brief Get enable SD/MMC/SDIO card device clock line status
 *
 * @param reg Pointer to SD host card selection register
 * @return Enable SD/MMC/SDIO card device clock line status (1 bit)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_enable_card_clk(wm_sdh_reg_t *reg)
{
    return reg->mmc_cardsel.enable_card_clk;
}

/**
 * @brief Set enable SD/MMC/SDIO card device clock line status
 *
 * @param reg Pointer to SD host card selection register
 * @param value Enable SD/MMC/SDIO card device clock line value to set (1 bit)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_enable_card_clk(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_cardsel.enable_card_clk = value;
}

/**
 * @brief Get enable SD/MMC/SDIO controller status
 *
 * @param reg Pointer to SD host card selection register
 * @return Enable SD/MMC/SDIO controller status (1 bit)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_controller_enable(wm_sdh_reg_t *reg)
{
    return reg->mmc_cardsel.controller_enable;
}

/**
 * @brief Set enable SD/MMC/SDIO controller status
 *
 * @param reg Pointer to SD host card selection register
 * @param value Enable SD/MMC/SDIO controller value to set (1 bit)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_controller_enable(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_cardsel.controller_enable = value;
}

/**
 * @brief Get SD/MMC/SDIO port DAT0 data signal status
 *
 * @param reg Pointer to SD host signal register
 * @return SD/MMC/SDIO port DAT0 data signal status (1 bit)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_dat0_signal(wm_sdh_reg_t *reg)
{
    return reg->mmc_sig.dat0_signal;
}

/**
 * @brief Set SD/MMC/SDIO port DAT0 data signal status
 *
 * @param reg Pointer to SD host signal register
 * @param value SD/MMC/SDIO port DAT0 data signal value to set (1 bit)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_dat0_signal(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_sig.dat0_signal = value;
}

/**
 * @brief Get SD/MMC/SDIO port DAT1 data signal status
 *
 * @param reg Pointer to SD host signal register
 * @return SD/MMC/SDIO port DAT1 data signal status (1 bit)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_dat1_signal(wm_sdh_reg_t *reg)
{
    return reg->mmc_sig.dat1_signal;
}

/**
 * @brief Set SD/MMC/SDIO port DAT1 data signal status
 *
 * @param reg Pointer to SD host signal register
 * @param value SD/MMC/SDIO port DAT1 data signal value to set (1 bit)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_dat1_signal(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_sig.dat1_signal = value;
}

/**
 * @brief Get SD/MMC/SDIO port DAT2 data signal status
 *
 * @param reg Pointer to SD host signal register
 * @return SD/MMC/SDIO port DAT2 data signal status (1 bit)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_dat2_signal(wm_sdh_reg_t *reg)
{
    return reg->mmc_sig.dat2_signal;
}

/**
 * @brief Set SD/MMC/SDIO port DAT2 data signal status
 *
 * @param reg Pointer to SD host signal register
 * @param value SD/MMC/SDIO port DAT2 data signal value to set (1 bit)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_dat2_signal(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_sig.dat2_signal = value;
}

/**
 * @brief Get SD/MMC/SDIO port DAT3 data signal status
 *
 * @param reg Pointer to SD host signal register
 * @return SD/MMC/SDIO port DAT3 data signal status (1 bit)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_dat3_signal(wm_sdh_reg_t *reg)
{
    return reg->mmc_sig.dat3_signal;
}

/**
 * @brief Set SD/MMC/SDIO port DAT3 data signal status
 *
 * @param reg Pointer to SD host signal register
 * @param value SD/MMC/SDIO port DAT3 data signal value to set (1 bit)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_dat3_signal(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_sig.dat3_signal = value;
}

/**
 * @brief Get CRC status when writing data CRC status token
 *
 * @param reg Pointer to SD host signal register
 * @return CRC status when writing data CRC status token (3 bits)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_crc_status(wm_sdh_reg_t *reg)
{
    return reg->mmc_sig.crc_status;
}

/**
 * @brief Set CRC status when writing data CRC status token
 *
 * @param reg Pointer to SD host signal register
 * @param value CRC status value to set (3 bits)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_crc_status(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_sig.crc_status = value;
}

/**
 * @brief Get SD/MMC/SDIO port CMD line signal status
 *
 * @param reg Pointer to SD host signal register
 * @return SD/MMC/SDIO port CMD line signal status (1 bit)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_cmd_signal(wm_sdh_reg_t *reg)
{
    return reg->mmc_sig.cmd_signal;
}

/**
 * @brief Set SD/MMC/SDIO port CMD line signal status
 *
 * @param reg Pointer to SD host signal register
 * @param value SD/MMC/SDIO port CMD line signal value to set (1 bit)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_cmd_signal(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_sig.cmd_signal = value;
}

/**
 * @brief Get SD/MMC/SDIO port auto multiple block data transfer setting
 *
 * @param reg Pointer to SD host IO and multi-block control register
 * @return SD/MMC/SDIO port auto multiple block data transfer setting (1 bit)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_auto_multi_block_tran(wm_sdh_reg_t *reg)
{
    return reg->mmc_io_mbctl.auto_multi_block_tran;
}

/**
 * @brief Set SD/MMC/SDIO port auto multiple block data transfer setting
 *
 * @param reg Pointer to SD host IO and multi-block control register
 * @param value SD/MMC/SDIO port auto multiple block data transfer value to set (1 bit)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_auto_multi_block_tran(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_io_mbctl.auto_multi_block_tran = value;
}

/**
 * @brief Get SD/MMC/SDIO port multiple block data transfer direction
 *
 * @param reg Pointer to SD host IO and multi-block control register
 * @return SD/MMC/SDIO port multiple block data transfer direction (1 bit)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_multi_block_direction(wm_sdh_reg_t *reg)
{
    return reg->mmc_io_mbctl.multi_block_direction;
}

/**
 * @brief Set SD/MMC/SDIO port multiple block data transfer direction
 *
 * @param reg Pointer to SD host IO and multi-block control register
 * @param value SD/MMC/SDIO port multiple block data transfer direction value to set (1 bit)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_multi_block_direction(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_io_mbctl.multi_block_direction = value;
}

/**
 * @brief Get SD/MMC/SDIO port full automatic command and multiple block transfer setting
 *
 * @param reg Pointer to SD host IO and multi-block control register
 * @return SD/MMC/SDIO port full automatic command and multiple block transfer setting (1 bit)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_auto_cmd_multi_tran(wm_sdh_reg_t *reg)
{
    return reg->mmc_io_mbctl.auto_cmd_multi_tran;
}

/**
 * @brief Set SD/MMC/SDIO port full automatic command and multiple block transfer setting
 *
 * @param reg Pointer to SD host IO and multi-block control register
 * @param value SD/MMC/SDIO port full automatic command and multiple block transfer value to set (1 bit)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_auto_cmd_multi_tran(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_io_mbctl.auto_cmd_multi_tran = value;
}

/**
 * @brief Get SD/MMC/SDIO port clock line polarity
 *
 * @param reg Pointer to SD host IO and multi-block control register
 * @return SD/MMC/SDIO port clock line polarity (1 bit)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_port_clock_polarity(wm_sdh_reg_t *reg)
{
    return reg->mmc_io_mbctl.port_clock_polarity;
}

/**
 * @brief Set SD/MMC/SDIO port clock line polarity
 *
 * @param reg Pointer to SD host IO and multi-block control register
 * @param value SD/MMC/SDIO port clock line polarity value to set (1 bit)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_port_clock_polarity(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_io_mbctl.port_clock_polarity = value;
}

/**
 * @brief Get SD/MMC/SDIO Busy timeout scale selection
 *
 * @param reg Pointer to SD host IO and multi-block control register
 * @return SD/MMC/SDIO Busy timeout scale selection (2 bits)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_busy_timeout_scale(wm_sdh_reg_t *reg)
{
    return reg->mmc_io_mbctl.busy_timeout_scale;
}

/**
 * @brief Set SD/MMC/SDIO Busy timeout scale selection
 *
 * @param reg Pointer to SD host IO and multi-block control register
 * @param value SD/MMC/SDIO Busy timeout scale value to set (2 bits)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_busy_timeout_scale(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_io_mbctl.busy_timeout_scale = value;
}

/**
 * @brief Get SD/MMC/SDIO NAC timeout scale selection
 *
 * @param reg Pointer to SD host IO and multi-block control register
 * @return SD/MMC/SDIO NAC timeout scale selection (2 bits)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_mmc_get_nac_timeout_scale(wm_sdh_reg_t *reg)
{
    return reg->mmc_io_mbctl.nac_timeout_scale;
}

/**
 * @brief Set SD/MMC/SDIO NAC timeout scale selection
 *
 * @param reg Pointer to SD host IO and multi-block control register
 * @param value SD/MMC/SDIO NAC timeout scale value to set (2 bits)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_mmc_set_nac_timeout_scale(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->mmc_io_mbctl.nac_timeout_scale = value;
}

/**
 * @brief Get the value of the data block count field
 *
 * @param reg Pointer to SD host data block count register
 * @return Data block count value (16 bits)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_get_block_count(wm_sdh_reg_t *reg)
{
    return WM_LL_REG_FILED_COPY_READ(reg->blockcnt, cnt);
}

/**
 * @brief Set the value of the data block count field
 *
 * @param reg Pointer to SD host data block count register
 * @param value Data block count value to set (16 bits)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_set_block_count(wm_sdh_reg_t *reg, uint32_t value)
{
    WM_LL_REG_FILED_COPY_WRTIE(reg->blockcnt, cnt, value);
}

/**
 * @brief Get the value of the data transfer timeout count field
 *
 * @param reg Pointer to SD host data transfer timeout count register
 * @return Data transfer timeout count value (8 bits)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_get_timeout_count(wm_sdh_reg_t *reg)
{
    return WM_LL_REG_FILED_COPY_READ(reg->timeoutcnt, timeout_cnt);
}

/**
 * @brief Set the value of the data transfer timeout count field
 *
 * @param reg Pointer to SD host data transfer timeout count register
 * @param value Data transfer timeout count value to set (8 bits)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_set_timeout_count(wm_sdh_reg_t *reg, uint32_t value)
{
    WM_LL_REG_FILED_COPY_WRTIE(reg->timeoutcnt, timeout_cnt, value);
}

/**
 * @brief Get the value of the command buffer byte 0 field
 *
 * @param reg Pointer to SD host command buffer byte 0 register
 * @param buf_id Index of the buffer
 * @return Command buffer byte 0 value (8 bits)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_get_cmd_buf(wm_sdh_reg_t *reg, uint8_t buf_id)
{
    return WM_LL_REG_FILED_COPY_READ(reg->cmd_buf[buf_id], byte);
}

/**
 * @brief Set the value of the command buffer byte 0 field
 *
 * @param reg Pointer to SD host command buffer byte 0 register
 * @param buf_id Index of the buffer
 * @param value Command buffer byte 0 value to set (8 bits)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_set_cmd_buf(wm_sdh_reg_t *reg, uint8_t buf_id, uint32_t value)
{
    WM_LL_REG_FILED_COPY_WRTIE(reg->cmd_buf[buf_id], byte, value);
}

/**
 * @brief Get the value of the data full signal field
 *
 * @param reg Pointer to SD host buffer control register
 * @return Data full signal value (1 bit)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_get_data_full_sig(wm_sdh_reg_t *reg)
{
    return reg->buf_ctrl.data_full_sig;
}

/**
 * @brief Get the value of the data empty signal field
 *
 * @param reg Pointer to SD host buffer control register
 * @return Data empty signal value (1 bit)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_get_data_empty_sig(wm_sdh_reg_t *reg)
{
    return reg->buf_ctrl.data_empty_sig;
}

/**
 * @brief Get the value of the data pipeline setting field
 *
 * @param reg Pointer to SD host buffer control register
 * @return Data pipeline setting value (8 bits)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_get_data_pipeline_set(wm_sdh_reg_t *reg)
{
    return WM_LL_REG_FILED_COPY_READ(reg->buf_ctrl, data_pipeline_set);
}

/**
 * @brief Set the value of the data pipeline setting field
 *
 * @param reg Pointer to SD host buffer control register
 * @param value Data pipeline setting value to set (8 bits)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_set_data_pipeline_set(wm_sdh_reg_t *reg, uint32_t value)
{
    WM_LL_REG_FILED_COPY_WRTIE(reg->buf_ctrl, data_pipeline_set, value);
}

/**
 * @brief Get the value of the DMA hardware interface enable field
 *
 * @param reg Pointer to SD host buffer control register
 * @return DMA hardware interface enable value (1 bit)
 */
ATTRIBUTE_INLINE bool wm_ll_sdh_get_dma_hw_intf_en(wm_sdh_reg_t *reg)
{
    return !!(reg->buf_ctrl.dma_hw_intf_en);
}

/**
 * @brief Set the value of the DMA hardware interface enable field
 *
 * @param reg Pointer to SD host buffer control register
 * @param value DMA hardware interface enable value to set (1 bit)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_set_dma_hw_intf_en(wm_sdh_reg_t *reg, bool en)
{
    reg->buf_ctrl.dma_hw_intf_en = en;
}

/**
 * @brief Get the value of the buffer access direction field
 *
 * @param reg Pointer to SD host buffer control register
 * @return Buffer access direction value (1 bit)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_get_buf_access_dir(wm_sdh_reg_t *reg)
{
    return reg->buf_ctrl.buf_access_dir;
}

/**
 * @brief Set the value of the buffer access direction field
 *
 * @param reg Pointer to SD host buffer control register
 * @param value Buffer access direction value to set (1 bit)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_set_buf_access_dir(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->buf_ctrl.buf_access_dir = value;
}

/**
 * @brief Get the value of the FIFO status mask configuration field
 *
 * @param reg Pointer to SD host buffer control register
 * @return FIFO status mask configuration value (1 bit)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_get_fifo_status_mask(wm_sdh_reg_t *reg)
{
    return reg->buf_ctrl.fifo_status_mask;
}

/**
 * @brief Set the value of the FIFO status mask configuration field
 *
 * @param reg Pointer to SD host buffer control register
 * @param value FIFO status mask configuration value to set (1 bit)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_set_fifo_status_mask(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->buf_ctrl.fifo_status_mask = value;
}

/**
 * @brief Get the value of the DMA request mask field
 *
 * @param reg Pointer to SD host buffer control register
 * @return DMA request mask value (1 bit)
 */
ATTRIBUTE_INLINE uint32_t wm_ll_sdh_get_dma_req_mask(wm_sdh_reg_t *reg)
{
    return reg->buf_ctrl.dma_req_mask;
}

/**
 * @brief Set the value of the DMA request mask field
 *
 * @param reg Pointer to SD host buffer control register
 * @param value DMA request mask value to set (1 bit)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_set_dma_req_mask(wm_sdh_reg_t *reg, uint32_t value)
{
    reg->buf_ctrl.dma_req_mask = value;
}

/**
 * @brief Get the value of the data buffer flush enable field
 *
 * @param reg Pointer to SD host buffer control register
 * @return Data buffer flush enable value (1 bit)
 */
ATTRIBUTE_INLINE bool wm_ll_sdh_get_data_flush_en(wm_sdh_reg_t *reg)
{
    return !!(reg->buf_ctrl.data_flush_en);
}

/**
 * @brief Set the value of the data buffer flush enable field
 *
 * @param reg Pointer to SD host buffer control register
 * @param value Data buffer flush enable value to set (1 bit)
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_set_data_flush_en(wm_sdh_reg_t *reg, bool en)
{
    reg->buf_ctrl.data_flush_en = en;
}

/**
 * @brief Set the tx data to sdh buf
 *
 * @param reg Pointer to SD host buffer control register
 * @param reg Pointer to data that need to tx
 * @param reg Len of data that need to tx
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_set_data_buf(wm_sdh_reg_t *reg, uint8_t *data, uint32_t len)
{
    if (data != NULL && len <= WM_SDH_DATA_BUF_BYTES) {
        for (uint32_t i = 0; i < len / 4; i++) {
            reg->data_buf[i] = ((uint32_t)data[4 * i + 0]) | ((uint32_t)data[4 * i + 1]) << 8 |
                               ((uint32_t)data[4 * i + 2]) << 16 | ((uint32_t)data[4 * i + 3]) << 24;
        }
        if ((len % 4) > 0) {
            uint32_t last_word = 0;
            for (uint8_t j = 0; j < len % 4; j++) {
                last_word |= (data[4 * (len / 4) + j]) << (8 * j);
            }
            reg->data_buf[len / 4] = last_word;
        }
    }
}

/**
 * @brief Get the rx data
 *
 * @param reg Pointer to SD host buffer control register
 * @param reg Pointer to the rx buf
 * @param reg Len of data that need to rx
 * @return void
 */
ATTRIBUTE_INLINE void wm_ll_sdh_get_data_buf(wm_sdh_reg_t *reg, uint8_t *buf, uint32_t len)
{
    if (buf != NULL && len <= WM_SDH_DATA_BUF_BYTES) {
        for (uint32_t i = 0; i < len / 4; i++) {
            buf[4 * i + 0] = (uint8_t)((reg->data_buf[i]) & 0xFF);
            buf[4 * i + 1] = (uint8_t)((reg->data_buf[i] >> 8) & 0xFF);
            buf[4 * i + 2] = (uint8_t)((reg->data_buf[i] >> 16) & 0xFF);
            buf[4 * i + 3] = (uint8_t)((reg->data_buf[i] >> 24) & 0xFF);
        }
        if ((len % 4) > 0) {
            uint32_t last_word = reg->data_buf[len / 4];
            for (uint8_t j = 0; j < len % 4; j++) {
                buf[4 * (len / 4) + j] = (uint8_t)((last_word >> (8 * j)) & 0xFF);
            }
        }
    }
}

/**
 * @brief Get the sdio data buf
 *
 * @param reg Pointer to SD host buffer control register
 * @return sdio data buf addr
 */
ATTRIBUTE_INLINE volatile uint32_t *wm_ll_sdh_get_data_buf_addr(wm_sdh_reg_t *reg)
{
    return &reg->data_buf[0];
}

#ifdef __cplusplus
}
#endif

#endif /* __WM_LL_SDH_H__ */

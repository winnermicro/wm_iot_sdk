/**
 * @file wm_ll_flash.h
 *
 * @brief flash Driver Module
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

#ifndef __WM_LL_FLASH_H__
#define __WM_LL_FLASH_H__

#include "wm_attr.h"
#include "wm_flash_reg_struct.h"
#include "wm_soc_cfgs.h"
#include "wm_reg_op.h"
#include "wm_regs_base.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WM_FLS_MAX_RD_SIZE  (1024)

#define WM_FLS_ADDR_MAX_BIT 0x1FFFFFF

typedef enum {
    WM_FLS_CMD_WREN   = 0x06, // Write Enable
    WM_FLS_CMD_RDID   = 0x9F, // Read Manufacturer/device ID
    WM_FLS_CMD_RDSR   = 0x5,  // Read Status Register
    WM_FLS_CMD_RDSR1  = 0x35, // Read out status register-1
    WM_FLS_CMD_WRSR   = 0x1,  // Write data to status registers
    WM_FLS_CMD_WRSR1  = 0x31, // Write data to status registers-1
    WM_FLS_CMD_PP     = 0x2,  // program selected page
    WM_FLS_CMD_PRSCUR = 0x42, // Program Security Registers
    WM_FLS_CMD_PE     = 0x81, // erase selected page
    WM_FLS_CMD_SE     = 0x20, // erase selected sector, Sector Erase (4K bytes)
    WM_FLS_CMD_CE     = 0x60, // erase whole chip
    WM_FLS_CMD_ERSCUR = 0x44, // Erase security registers
    WM_FLS_CMD_READ   = 0x3,  // Read Array (low power)
    WM_FLS_CMD_FREAD  = 0xB,  // Read Array (fast)
    WM_FLS_CMD_2READ  = 0xBB, // n bytes read out by 2IO
    WM_FLS_CMD_4READ  = 0xEB, // n bytes read out by 4IO
    WM_FLS_CMD_RUID   = 0x4B, // Read unique ID
    WM_FLS_CMD_RDSCUR = 0x48, // Read value of security register
    WM_FLS_CMD_SFDP   = 0x5A, // Read Serial Flash Discoverable Parameter (SFDP) Table
    WM_FLS_CMD_REMS   = 0x90, //Read manufacturer ID/device ID data
} wm_fls_cmd_t;

/**
 * @brief Reset the value of cmd_info.val in wm_wm_flash_reg_t structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @param value Reset value for cmd_info.val
 */
ATTRIBUTE_INLINE void wm_ll_flash_reset_cmd_info(wm_flash_reg_t *hw, uint32_t value)
{
    hw->cmd_info.val = value;
}

/**
 * @brief Set the value of cmd field in flash_cmd_t_info structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @param value Value to be set for the cmd field (uint8_t)
 */
ATTRIBUTE_INLINE void wm_ll_flash_set_cmd_info_cmd(wm_flash_reg_t *hw, uint8_t value)
{
    WM_LL_REG_FILED_COPY_WRTIE(hw->cmd_info, cmd, value);
}

/**
 * @brief Get the value of cmd field in flash_cmd_t_info structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @return uint8_t Value of the cmd field
 */
ATTRIBUTE_INLINE uint8_t wm_ll_flash_get_cmd_info_cmd(wm_flash_reg_t *hw)
{
    return WM_LL_REG_FILED_COPY_READ(hw->cmd_info, cmd);
}

/**
 * @brief Set the value of is_ce field in flash_cmd_t_info structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @param value Value to be set for the is_ce field (uint8_t)
 */
ATTRIBUTE_INLINE void wm_ll_flash_set_cmd_info_is_ce(wm_flash_reg_t *hw, uint8_t value)
{
    hw->cmd_info.is_ce = value;
}

/**
 * @brief Get the value of cmd field in flash_cmd_t_info structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @return uint8_t Value of the is_ce field
 */
ATTRIBUTE_INLINE uint8_t wm_ll_flash_get_cmd_info_is_ce(wm_flash_reg_t *hw)
{
    return (uint8_t)(hw->cmd_info.is_ce);
}

/**
 * @brief Set the value of is_hbe field in flash_cmd_t_info structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @param value Value to be set for the is_hbe field (uint8_t)
 */
ATTRIBUTE_INLINE void wm_ll_flash_set_cmd_info_is_hbe(wm_flash_reg_t *hw, uint8_t value)
{
    hw->cmd_info.is_hbe = value;
}

/**
 * @brief Get the value of cmd field in flash_cmd_t_info structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @return uint8_t Value of the is_hbe field
 */
ATTRIBUTE_INLINE uint8_t wm_ll_flash_get_cmd_info_is_hbe(wm_flash_reg_t *hw)
{
    return (uint8_t)(hw->cmd_info.is_hbe);
}

/**
 * @brief Set the value of is_be field in flash_cmd_t_info structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @param value Value to be set for the is_be field (uint8_t)
 */
ATTRIBUTE_INLINE void wm_ll_flash_set_cmd_info_is_be(wm_flash_reg_t *hw, uint8_t value)
{
    hw->cmd_info.is_be = value;
}

/**
 * @brief Get the value of cmd field in flash_cmd_t_info structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @return uint8_t Value of the is_be field
 */
ATTRIBUTE_INLINE uint8_t wm_ll_flash_get_cmd_info_is_be(wm_flash_reg_t *hw)
{
    return (uint8_t)(hw->cmd_info.is_be);
}

/**
 * @brief Set the value of is_se field in flash_cmd_t_info structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @param value Value to be set for the is_se field (uint8_t)
 */
ATTRIBUTE_INLINE void wm_ll_flash_set_cmd_info_is_se(wm_flash_reg_t *hw, uint8_t value)
{
    hw->cmd_info.is_se = value;
}

/**
 * @brief Get the value of cmd field in flash_cmd_t_info structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @return uint8_t Value of the is_se field
 */
ATTRIBUTE_INLINE uint8_t wm_ll_flash_get_cmd_info_is_se(wm_flash_reg_t *hw)
{
    return (uint8_t)(hw->cmd_info.is_se);
}

/**
 * @brief Set the value of is_pp field in flash_cmd_t_info structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @param value Value to be set for the is_pp field (uint8_t)
 */
ATTRIBUTE_INLINE void wm_ll_flash_set_cmd_info_is_pp(wm_flash_reg_t *hw, uint8_t value)
{
    hw->cmd_info.is_pp = value;
}

/**
 * @brief Get the value of cmd field in flash_cmd_t_info structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @return uint8_t Value of the is_pp field
 */
ATTRIBUTE_INLINE uint8_t wm_ll_flash_get_cmd_info_is_pp(wm_flash_reg_t *hw)
{
    return (uint8_t)(hw->cmd_info.is_pp);
}

/**
 * @brief Set the value of is_wrsr field in flash_cmd_t_info structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @param value Value to be set for the is_wrsr field (uint8_t)
 */
ATTRIBUTE_INLINE void wm_ll_flash_set_cmd_info_is_wrsr(wm_flash_reg_t *hw, uint8_t value)
{
    hw->cmd_info.is_wrsr = value;
}

/**
 * @brief Get the value of cmd field in flash_cmd_t_info structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @return uint8_t Value of the is_wrsr field
 */
ATTRIBUTE_INLINE uint8_t wm_ll_flash_get_cmd_info_is_wrsr(wm_flash_reg_t *hw)
{
    return (uint8_t)(hw->cmd_info.is_wrsr);
}

/**
 * @brief Set the value of is_read_cmd field in flash_cmd_t_info structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @param value Value to be set for the is_read_cmd field (uint8_t)
 */
ATTRIBUTE_INLINE void wm_ll_flash_set_cmd_info_is_read_cmd(wm_flash_reg_t *hw, uint8_t value)
{
    hw->cmd_info.is_read_cmd = value;
}

/**
 * @brief Get the value of cmd field in flash_cmd_t_info structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @return uint8_t Value of the is_read_cmd field
 */
ATTRIBUTE_INLINE uint8_t wm_ll_flash_get_cmd_info_is_read_cmd(wm_flash_reg_t *hw)
{
    return (uint8_t)(hw->cmd_info.is_read_cmd);
}

/**
 * @brief Set the value of has_data field in flash_cmd_t_info structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @param value Value to be set for the has_data field (uint8_t)
 */
ATTRIBUTE_INLINE void wm_ll_flash_set_cmd_info_has_data(wm_flash_reg_t *hw, uint8_t value)
{
    hw->cmd_info.has_data = value;
}

/**
 * @brief Get the value of cmd field in flash_cmd_t_info structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @return uint8_t Value of the has_data field
 */
ATTRIBUTE_INLINE uint8_t wm_ll_flash_get_cmd_info_has_data(wm_flash_reg_t *hw)
{
    return (uint8_t)(hw->cmd_info.has_data);
}

/**
 * @brief Set the value of data_size field in flash_cmd_t_info structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @param value Value to be set for the data_size field (uint16_t)
 */
ATTRIBUTE_INLINE void wm_ll_flash_set_cmd_info_data_size(wm_flash_reg_t *hw, uint16_t value)
{
    hw->cmd_info.data_size = value;
}

/**
 * @brief Get the value of cmd field in flash_cmd_t_info structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @return uint8_t Value of the data_size field
 */
ATTRIBUTE_INLINE uint8_t wm_ll_flash_get_cmd_info_data_size(wm_flash_reg_t *hw)
{
    return (uint8_t)(hw->cmd_info.data_size);
}

/**
 * @brief Set the value of dummy_num field in flash_cmd_t_info structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @param value Value to be set for the dummy_num field (uint8_t)
 */
ATTRIBUTE_INLINE void wm_ll_flash_set_cmd_info_dummy_num(wm_flash_reg_t *hw, uint8_t value)
{
    hw->cmd_info.dummy_num = value;
}

/**
 * @brief Get the value of cmd field in flash_cmd_t_info structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @return uint8_t Value of the cmd field
 */
ATTRIBUTE_INLINE uint8_t wm_ll_flash_get_cmd_info_dummy_num(wm_flash_reg_t *hw)
{
    return (uint8_t)(hw->cmd_info.dummy_num);
}

/**
 * @brief Set the value of has_dummy field in flash_cmd_t_info structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @param value Value to be set for the has_dummy field (uint8_t)
 */
ATTRIBUTE_INLINE void wm_ll_flash_set_cmd_info_has_dummy(wm_flash_reg_t *hw, uint8_t value)
{
    hw->cmd_info.has_dummy = value;
}

/**
 * @brief Get the value of cmd field in flash_cmd_t_info structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @return uint8_t Value of the has_dummy field
 */
ATTRIBUTE_INLINE uint8_t wm_ll_flash_get_cmd_info_has_dummy(wm_flash_reg_t *hw)
{
    return (uint8_t)(hw->cmd_info.has_dummy);
}

/**
 * @brief Set the value of has_crm field in flash_cmd_t_info structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @param value Value to be set for the has_crm field (uint8_t)
 */
ATTRIBUTE_INLINE void wm_ll_flash_set_cmd_info_has_crm(wm_flash_reg_t *hw, uint8_t value)
{
    hw->cmd_info.has_crm = value;
}

/**
 * @brief Get the value of cmd field in flash_cmd_t_info structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @return uint8_t Value of the has_crm field
 */
ATTRIBUTE_INLINE uint8_t wm_ll_flash_get_cmd_info_has_crm(wm_flash_reg_t *hw)
{
    return (uint8_t)(hw->cmd_info.has_crm);
}

/**
 * @brief Set the value of has_address field in flash_cmd_t_info structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @param value Value to be set for the has_address field (uint8_t)
 */
ATTRIBUTE_INLINE void wm_ll_flash_set_cmd_info_has_address(wm_flash_reg_t *hw, uint8_t value)
{
    hw->cmd_info.has_address = value;
}

/**
 * @brief Get the value of cmd field in flash_cmd_t_info structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @return uint8_t Value of the has_address field
 */
ATTRIBUTE_INLINE uint8_t wm_ll_flash_get_cmd_info_has_address(wm_flash_reg_t *hw)
{
    return (uint8_t)(hw->cmd_info.has_address);
}

/**
 * @brief Reset the value of cmd_start.val in wm_flash_reg_t structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @param value Reset value for cmd_start.val
 */
ATTRIBUTE_INLINE void wm_ll_flash_reset_cmd_start(wm_flash_reg_t *hw, uint32_t value)
{
    hw->cmd_start.val = value;
}

/**
 * @brief Get the value of crm field in flash_cmd_start_t structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @return uint8_t Value of the crm field
 */
ATTRIBUTE_INLINE uint8_t wm_ll_flash_get_cmd_start_crm(wm_flash_reg_t *hw)
{
    return WM_LL_REG_FILED_COPY_READ(hw->cmd_start, crm);
}

/**
 * @brief Set the value of crm field in flash_cmd_start_t structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @param value Value to be set for the crm field (uint8_t)
 */
ATTRIBUTE_INLINE void wm_ll_flash_set_cmd_start_crm(wm_flash_reg_t *hw, uint8_t value)
{
    WM_LL_REG_FILED_COPY_WRTIE(hw->cmd_start, crm, value);
}

/**
 * @brief Get the value of crm field in flash_cmd_start_t structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @return uint8_t Value of the start field
 */
ATTRIBUTE_INLINE uint8_t wm_ll_flash_get_cmd_start_start(wm_flash_reg_t *hw)
{
    return (uint8_t)(hw->cmd_start.start);
}

/**
 * @brief Set the value of start field in flash_cmd_start_t structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @param value Value to be set for the start field (uint8_t)
 */
ATTRIBUTE_INLINE void wm_ll_flash_set_cmd_start_start(wm_flash_reg_t *hw, uint8_t value)
{
    hw->cmd_start.start = value;
}

/**
 * @brief Reset the value of cntrl.val in wm_flash_reg_t structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @param value Reset value for cntrl.val
 */
ATTRIBUTE_INLINE void wm_ll_flash_reset_cntrl(wm_flash_reg_t *hw, uint32_t value)
{
    hw->cntrl.val = value;
}

/**
 * @brief Get the value of qiom field in flash_ctrl_t structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @return uint8_t Value of the qiom field
 */
ATTRIBUTE_INLINE uint8_t wm_ll_flash_get_ctrl_qiom(wm_flash_reg_t *hw)
{
    return (uint8_t)(hw->cntrl.qiom);
}

/**
 * @brief Set the value of qiom field in flash_ctrl_t structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @param value Value to be set for the qiom field (uint8_t)
 */
ATTRIBUTE_INLINE void wm_ll_flash_set_ctrl_qiom(wm_flash_reg_t *hw, uint8_t value)
{
    hw->cntrl.qiom = value;
}

/**
 * @brief Get the value of qpim field in flash_ctrl_t structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @return uint8_t Value of the qpim field
 */
ATTRIBUTE_INLINE uint8_t wm_ll_flash_get_ctrl_qpim(wm_flash_reg_t *hw)
{
    return (uint8_t)(hw->cntrl.qpim);
}

/**
 * @brief Set the value of qpim field in flash_ctrl_t structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @param value Value to be set for the qpim field (uint8_t)
 */
ATTRIBUTE_INLINE void wm_ll_flash_set_ctrl_qpim(wm_flash_reg_t *hw, uint8_t value)
{
    hw->cntrl.qpim = value;
}

/**
 * @brief Get the value of tshs field in flash_ctrl_t structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @return uint8_t Value of the tshs field
 */
ATTRIBUTE_INLINE uint8_t wm_ll_flash_get_ctrl_tshs(wm_flash_reg_t *hw)
{
    return (uint8_t)(hw->cntrl.tshs);
}

/**
 * @brief Set the value of tshs field in flash_ctrl_t structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @param value Value to be set for the tshs field (uint8_t)
 */
ATTRIBUTE_INLINE void wm_ll_flash_set_ctrl_tshs(wm_flash_reg_t *hw, uint8_t value)
{
    hw->cntrl.tshs = value;
}

/**
 * @brief Get the value of mode field in flash_ctrl_t structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @return uint8_t Value of the mode field
 */
ATTRIBUTE_INLINE uint8_t wm_ll_flash_get_ctrl_mode(wm_flash_reg_t *hw)
{
    return (uint8_t)(hw->cntrl.mode);
}

/**
 * @brief Set the value of mode field in flash_ctrl_t structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @param value Value to be set for the mode field (uint8_t)
 */
ATTRIBUTE_INLINE void wm_ll_flash_set_ctrl_mode(wm_flash_reg_t *hw, uint8_t value)
{
    hw->cntrl.mode = value;
}

/**
 * @brief Reset the value of remap.val in wm_flash_reg_t structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @param value Reset value for remap.val
 */
ATTRIBUTE_INLINE void wm_ll_flash_reset_remap(wm_flash_reg_t *hw, uint32_t value)
{
    hw->remap.val = value;
}

/**
 * @brief Get the value of cntrl field in flash_remap_t structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @return uint8_t Value of the cntrl field
 */
ATTRIBUTE_INLINE uint8_t wm_ll_flash_get_remap_cntrl(wm_flash_reg_t *hw)
{
    return (uint8_t)(hw->remap.cntrl);
}

/**
 * @brief Set the value of cntrl field in flash_remap_t structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @param value Value to be set for the cntrl field (uint8_t)
 */
ATTRIBUTE_INLINE void wm_ll_flash_set_remap_cntrl(wm_flash_reg_t *hw, uint8_t value)
{
    hw->remap.cntrl = value;
}

/**
 * @brief Reset the value of addr.val in wm_flash_reg_t structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @param value Reset value for addr.val
 */
ATTRIBUTE_INLINE void wm_ll_flash_reset_addr(wm_flash_reg_t *hw, uint32_t value)
{
    hw->addr.val = value;
}

/**
 * @brief Get the value of address field in flash_addr_t structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @return uint32_t Value of the address field
 */
ATTRIBUTE_INLINE uint32_t wm_ll_flash_get_addr_address(wm_flash_reg_t *hw)
{
    return hw->addr.address;
}

/**
 * @brief Set the value of address field in flash_addr_t structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @param value Value to be set for the address field (uint32_t)
 */
ATTRIBUTE_INLINE void wm_ll_flash_set_addr_address(wm_flash_reg_t *hw, uint32_t value)
{
    hw->addr.address = value;
}

/**
 * @brief Reset the value of decrypt_crtl.val in wm_flash_reg_t structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @param value Reset value for decrypt_crtl.val
 */
ATTRIBUTE_INLINE void wm_ll_flash_reset_decrypt_crtl(wm_flash_reg_t *hw, uint32_t value)
{
    hw->decrypt_crtl.val = value;
}

/**
 * @brief Get the value of data_ahb_decrypt field in flash_decrypt_crtl_t structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @return uint8_t Value of the code_ahb_decrypt field
 */
ATTRIBUTE_INLINE uint8_t wm_ll_flash_get_decrypt_crtl_code_ahb_decrypt(wm_flash_reg_t *hw)
{
    return (uint8_t)(hw->decrypt_crtl.code_ahb_decrypt);
}

/**
 * @brief Set the value of data_ahb_decrypt field in flash_decrypt_crtl_t structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @param value Value to be set for the code_ahb_decrypt field (uint8_t)
 */
ATTRIBUTE_INLINE void wm_ll_flash_set_decrypt_crtl_code_ahb_decrypt(wm_flash_reg_t *hw, uint8_t value)
{
    hw->decrypt_crtl.code_ahb_decrypt = value;
}

/**
 * @brief Get the value of data_dbus_decrypt field in flash_decrypt_crtl_t structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @return uint8_t Value of the data_decrypt field
 */
ATTRIBUTE_INLINE uint8_t wm_ll_flash_get_decrypt_crtl_data_dbus_decrypt(wm_flash_reg_t *hw)
{
    return (uint8_t)(hw->decrypt_crtl.data_decrypt);
}

/**
 * @brief Set the value of data_dbus_decrypt field in flash_decrypt_crtl_t structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @param value Value to be set for the data_decrypt field (uint8_t)
 */
ATTRIBUTE_INLINE void wm_ll_flash_set_decrypt_crtl_data_dbus_decrypt(wm_flash_reg_t *hw, uint8_t value)
{
    hw->decrypt_crtl.data_decrypt = value;
}

/**
 * @brief Get the value of data_ahb_decrypt field in flash_decrypt_crtl_t structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @return uint8_t Value of the data_ahb_decrypt field
 */
ATTRIBUTE_INLINE uint8_t wm_ll_flash_get_decrypt_crtl_data_ahb_decrypt(wm_flash_reg_t *hw)
{
    return (uint8_t)(hw->decrypt_crtl.data_ahb_decrypt);
}

/**
 * @brief Set the value of data_ahb_decrypt field in flash_decrypt_crtl_t structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @param value Value to be set for the data_ahb_decrypt field (uint8_t)
 */
ATTRIBUTE_INLINE void wm_ll_flash_set_decrypt_crtl_data_ahb_decrypt(wm_flash_reg_t *hw, uint8_t value)
{
    hw->decrypt_crtl.data_ahb_decrypt = value;
}

/**
 * @brief Get the value of exptsel field in flash_decrypt_crtl_t structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @return uint8_t Value of the exptsel field
 */
ATTRIBUTE_INLINE uint8_t wm_ll_flash_get_decrypt_crtl_exptsel(wm_flash_reg_t *hw)
{
    return (uint8_t)(hw->decrypt_crtl.exptsel);
}

/**
 * @brief Set the value of exptsel field in flash_decrypt_crtl_t structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @param value Value to be set for the exptsel field (uint8_t)
 */
ATTRIBUTE_INLINE void wm_ll_flash_set_decrypt_crtl_exptsel(wm_flash_reg_t *hw, uint8_t value)
{
    hw->decrypt_crtl.exptsel = value;
}

/**
 * @brief Get the value of expt_start field in flash_decrypt_crtl_t structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @return uint8_t Value of the exptsel field
 */
ATTRIBUTE_INLINE uint8_t wm_ll_flash_get_decrypt_crtl_expt_start(wm_flash_reg_t *hw)
{
    return (uint8_t)(hw->decrypt_crtl.expt_start);
}

/**
 * @brief Set the value of expt_start field in flash_decrypt_crtl_t structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @param value Value to be set for the expt_start field (uint8_t)
 */
ATTRIBUTE_INLINE void wm_ll_flash_set_decrypt_crtl_expt_start(wm_flash_reg_t *hw, uint8_t value)
{
    hw->decrypt_crtl.expt_start = value;
}

/**
 * @brief Reset the value of decrypt_status.val in wm_flash_reg_t structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @param value Reset value for decrypt_status.val
 */
ATTRIBUTE_INLINE void wm_ll_flash_reset_decrypt_status(wm_flash_reg_t *hw, uint32_t value)
{
    hw->decrypt_status.val = value;
}

/**
 * @brief Get the value of rsa_mode field in flash_decrypt_status_t structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @return uint8_t Value of the rsa_mode field
 */
ATTRIBUTE_INLINE uint8_t wm_ll_flash_get_decrypt_status_rsa_mode(wm_flash_reg_t *hw)
{
    return (uint8_t)(hw->decrypt_status.rsa_mode);
}

/**
 * @brief Set the value of rsa_mode field in decrypt_status structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @param value Value to be set for the rsa_mode field (uint8_t)
 */
ATTRIBUTE_INLINE void wm_ll_flash_set_decrypt_status_rsa_mode(wm_flash_reg_t *hw, uint8_t value)
{
    hw->decrypt_status.rsa_mode = value;
}

/**
 * @brief Get the value of key_ready field in flash_decrypt_status_t structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @return uint8_t Value of the key_ready field
 */
ATTRIBUTE_INLINE uint8_t wm_ll_flash_get_decrypt_status_key_ready(wm_flash_reg_t *hw)
{
    return (uint8_t)(hw->decrypt_status.key_ready);
}

/**
 * @brief Set the value of key_ready field in decrypt_status structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @param value Value to be set for the key_ready field (uint8_t)
 */
ATTRIBUTE_INLINE void wm_ll_flash_set_decrypt_status_key_ready(wm_flash_reg_t *hw, uint8_t value)
{
    hw->decrypt_status.key_ready = value;
}

/**
 * @brief Get the value of key_err field in flash_decrypt_status_t structure
 * @param hw Pointer to the wm_flash_reg_t structure
 * @return uint8_t Value of the key_err field
 */
ATTRIBUTE_INLINE uint8_t wm_ll_flash_get_decrypt_status_key_err(wm_flash_reg_t *hw)
{
    return (uint8_t)(hw->decrypt_status.key_err);
}

/**
 * @brief write data to flash
 *
 * @param offset address offset
 * @param value  write data value
 *
 * @return None
 *
 * TODO:  will replace it in the feature,  need discuss with RSA mode, mutex read/write these buffer
 */
ATTRIBUTE_INLINE void wm_ll_flash_write_data_to_fifo(uint32_t offset, uint32_t value)
{
    *(volatile uint32_t *)((WM_RSA_BASE_ADDR + offset * 4)) = value;
}

/**
 * @brief get data from flash
 *
 * @param offset address offset
 *
 * @return read value
 *
 * TODO:  will replace it in the feature,  need discuss with RSA mode, mutex read/write these buffer
 */
ATTRIBUTE_INLINE uint32_t wm_ll_flash_read_data_from_fifo(uint32_t offset)
{
    return *(volatile uint32_t *)((WM_RSA_BASE_ADDR + offset * 4));
}

#ifdef __cplusplus
}
#endif

#endif /* __WM_LL_FLASH_H__ */

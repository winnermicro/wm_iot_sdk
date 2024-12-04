/**
 * @file wm_dt_op.h
 *
 * @brief Device Table Operate Module
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

#ifndef __WM_DT_OP_H__
#define __WM_DT_OP_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_DT_OP_Structures WM DT OP Structures
 * @brief WinnerMicro DT OP Structures
 */

/**
 * @addtogroup WM_DT_OP_Structures
 * @{
 */

/**
 * @brief dt table structure
 *
 * Structure of the dt table entity
 */
struct wm_dt_table_entry {
    char *dev_name; /**< device name */
    void *hw_addr;  /**< hardware info, ref wm_dt_hw_xxx_t */
    void *ops_addr; /**< device operation interface */
};

/**
 * @brief dt table index structure
 *
 * Structure of the dt table index entity
 */
struct wm_dt_table_index {
    char *table_name;                     /**< table name */
    uint32_t entry_count;                 /**< table entry count */
    struct wm_dt_table_entry *table_addr; /**< table entry addr */
};

/**
 * @}
 */

/**
 * @defgroup WM_DT_OP_MACROs WM DT OP MACROs
 * @brief WinnerMicro DT OP MACROs
 */

/**
 * @addtogroup WM_DT_OP_MACROs
 * @{
 */

/**
 * @brief  define an dt table
 *
 * @param[in] name           table name
 * @param[in] count          table entry count
 * @param[in] addr           table entry addr
 *
 * @return  None
 */
#define WM_DT_TABLE_DEFINE(name, count, addr)                                                     \
    struct wm_dt_table_index __dt_table_##name __attribute__((used, section(".wm_dt_table"))) = { \
        .table_name  = #name,                                                                     \
        .entry_count = count,                                                                     \
        .table_addr  = addr,                                                                      \
    }

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_DT_OP_H__ */

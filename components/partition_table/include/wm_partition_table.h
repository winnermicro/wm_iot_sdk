/**
 * @file wm_partition_table.h
 *
 * @brief patition table Module
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
#ifndef __WM_PARTITION_TABLE_H__
#define __WM_PARTITION_TABLE_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_PARTITION_Structs WM partition Structs
 * @brief WinnerMicro partition Structs
 */

/**
 * @brief partition name size
 */
#define WM_PARTITION_NAME_SIZE 16

/**
 * @addtogroup WM_PARTITION_Structs
 * @{
 */

/**
 * @struct wm_partition_item_t
 * @brief partition infomation
 */
typedef struct {
    char name[WM_PARTITION_NAME_SIZE]; /**< partition name   */
    uint32_t offset;                   /**< partition offset */
    uint32_t size;                     /**< partition size   */
    uint32_t flag;                     /**< partition flag   */
} wm_partition_item_t;
/**
 * @}
 */

/**
 * @defgroup WM_PARTITION_APIs partition table APIs
 * @brief WinnerMicro partition table APIs
 */

/**
 * @addtogroup WM_PARTITION_APIs
 * @{
 */

/**
  * @brief  initialize partition table module
  *
  * @return
  *    - WM_ERR_SUCCESS: success
  *    - WM_ERR_NOT_FOUND: fail
  * @note Partition table must exist in flash, and it must be burned to CONFIG_WM_PARTITION_TABLE_OFFSET
  */
int wm_partition_table_init(void);

/**
  * @brief     find a partition information by partition name
  *
  * @param[in] name  partition name
  * @param[out] partition partition information, @ref wm_partition_item_t
  *
  * @return
  *    - WM_ERR_SUCCESS: success
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - WM_ERR_NOT_FOUND: Not found the partition
  *
  * @note  must called after wm_partition_table_init
  */
int wm_partition_table_find(const char *name, wm_partition_item_t *partition);

/**
  * @brief  print partition table information
  *
  * @return
  *    - WM_ERR_SUCCESS: success
  *    - WM_ERR_NOT_FOUND: fail
  */
void wm_partition_table_print(void);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_PARTITION_TABLE_H__ */

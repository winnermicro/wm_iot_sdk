
/**
 * @file wm_nvs.h
 *
 * @brief WM_NVS Module
 *
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

#ifndef __WM_NVS_H__
#define __WM_NVS_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_NVS_Macros WM NVS Macros
 * @brief WinnerMicro NVS Macros
 */

/**
 * @addtogroup WM_NVS_Macros
 * @{
 */

/**
  * @brief  nvs default partition name
  */
#define WM_NVS_DEF_PARTITION      "nvs"

/**
  * @brief  nvs handle max num
  * Max num of nvs handle
  */
#define WM_NVS_MAX_HANDLE_NUM     64

/**
  * @brief  nvs key or group name max size
  * Max size of nvs key, maximum 15 valid characters support, not include '\0'
  */
#define WM_NVS_MAX_KEY_LEN        15

/**
  * @brief  Mininum BLOB segment size, Not include item segment header
  */
#define WM_NVS_BLOB_MIN_SEG_SIZE  512

/**
  * @brief  nvs error code
  */
#define WM_NVS_ERR_BASE           -20000                 /**< Base number of error codes    */
#define WM_NVS_ERR_OK             0                      /**< OK                            */
#define WM_NVS_ERR_FAIL           (WM_NVS_ERR_BASE - 1)  /**< failure                       */
#define WM_NVS_ERR_INVALID_PARAM  (WM_NVS_ERR_BASE - 2)  /**< invalid params                */
#define WM_NVS_ERR_NOT_INIT       (WM_NVS_ERR_BASE - 3)  /**< The nvs is not initialized    */
#define WM_NVS_ERR_AREADY_INIT    (WM_NVS_ERR_BASE - 4)  /**< The nvs is aready initialized */
#define WM_NVS_ERR_NO_MEM         (WM_NVS_ERR_BASE - 5)  /**< No enough memory              */
#define WM_NVS_ERR_NOT_FOUND      (WM_NVS_ERR_BASE - 6)  /**< gourp or key not found        */
#define WM_NVS_ERR_READ_ONLY      (WM_NVS_ERR_BASE - 7)  /**< partition is read only        */
#define WM_NVS_ERR_NO_SPACE       (WM_NVS_ERR_BASE - 8)  /**< no enough space for saving    */
#define WM_NVS_ERR_INVALID_HANDLE (WM_NVS_ERR_BASE - 9)  /**< invalid handle                 */
#define WM_NVS_ERR_SECTOR_FULL    (WM_NVS_ERR_BASE - 10) /**< sector is full                */
#define WM_NVS_ERR_INVALID_LENGTH (WM_NVS_ERR_BASE - 11) /**< invalid data length           */
#define WM_NVS_ERR_VALUE_TOO_LONG (WM_NVS_ERR_BASE - 12) /**< Value is too long             */
#define WM_NVS_ERR_CALL_IN_ISR    (WM_NVS_ERR_BASE - 13) /**< call in isr error             */

/**
 * @}
 */

/**
 * @defgroup WM_NVS_Enumerations WM NVS Enumerations
 * @brief WinnerMicro NVS Enumerations
 */

/**
 * @addtogroup WM_NVS_Enumerations
 * @{
 */

/**
* @enum     wm_nvs_open_mode_t
* @brief    nvs open mode
*/
typedef enum {
    WM_NVS_OP_READ_ONLY  = 0, /**< read only mode      */
    WM_NVS_OP_READ_WRITE = 1, /**< read and write mode */
    WM_NVS_OP_MAX,
} wm_nvs_open_mode_t;

/**
* @enum     wm_nvs_type_t
* @brief    nvs item data type
*/
typedef enum {
    WM_NVS_TYPE_ANY,    /**< any type             */
    WM_NVS_TYPE_STRING, /**< string end with '\0', max length 4031 */
    WM_NVS_TYPE_INT8,   /**< int8_t               */
    WM_NVS_TYPE_UINT8,  /**< uint8_t              */
    WM_NVS_TYPE_INT16,  /**< int16_t              */
    WM_NVS_TYPE_UINT16, /**< uint16_t             */
    WM_NVS_TYPE_INT32,  /**< int32_t              */
    WM_NVS_TYPE_UINT32, /**< uint32_t             */
    WM_NVS_TYPE_INT64,  /**< int64_t              */
    WM_NVS_TYPE_UINT64, /**< uint64_t             */
    WM_NVS_TYPE_DOUBLE, /**< float data           */
    WM_NVS_TYPE_BINARY, /**< small binary data, size from 1~4032, not split to segments */
    WM_NVS_TYPE_BLOB,   /**< Binary Large OBject, It will be divided into multiple segments (1~127) for storage.
                          Except the last piece, other segments are no less than 512 bytes. In addition,
                          a description item will also be stored ,all the size no larger than 512064 */
    WM_NVS_TYPE_MAX
} wm_nvs_type_t;

/**
 * @}
 */

/**
 * @defgroup WM_NVS_Structures WM NVS Structures
 * @brief WinnerMicro NVS Structures
 */

/**
 * @addtogroup WM_NVS_Structures
 * @{
 */

/**
  * @brief  nvs handle , handle read and write items
  */
typedef void *wm_nvs_handle_t;

/**
  * @brief  iterator , handle for poll all nvs items
  */
typedef struct wm_nvs_iterator_info_t *wm_nvs_iterator_t;

/**
 * @struct  wm_nvs_entry_t
 * @brief   iterator entry information
 */
typedef struct {
    char group[WM_NVS_MAX_KEY_LEN + 1]; /**< item group     */
    char key[WM_NVS_MAX_KEY_LEN + 1];   /**< item key       */
    wm_nvs_type_t type;                 /**< item type      */
    uint8_t group_id;                   /**< group id       */
    size_t length;                      /**< data length    */
} wm_nvs_entry_t;

/**
 * @struct  wm_nvs_status_t
 * @brief   nvs status
 */
typedef struct {
    uint32_t total_size;  /**< total size         */
    uint32_t using_size;  /**< using item size    */
    uint32_t droped_size; /**< drop item size     */
    uint32_t free_size;   /**< free size          */
    uint8_t group_num;    /**< num of groups      */
    uint8_t handle_num;   /**< num of the handles */
} wm_nvs_status_t;

/**
 * @}
 */

/**
 * @defgroup WM_NVS_Functions WM NVS Functions
 * @brief WinnerMicro NVS Functions
 */

/**
 * @addtogroup WM_NVS_Functions
 * @{
 */

/**
  * @brief  Initialize nvs
  *
  * @param[in]  partition_name nvs partition name
  *
  * @return
  *    - WM_NVS_ERR_OK: succeed
  *    - WM_NVS_ERR_NO_MEM: no memery
  *    - WM_NVS_ERR_FAIL: failed
  * @note This API must be called before all other nvs API can be called
  */
int wm_nvs_init(const char *partition_name);

/**
  * @brief  Deinitialize nvs
  *
  * @param[in]  partition_name nvs partition name
  *
  * @return
  *    - WM_NVS_ERR_OK: succeed
  *    - WM_NVS_ERR_FAIL: failed
  * @note The handle open by the api wm_nvs_open must be closed before wm_nvs_deinit called.
  */
int wm_nvs_deinit(const char *partition_name);

/**
  * @brief  erase the whole nvs partition
  *
  * @param[in]  partition_name nvs partition name
  *
  * @return
  *    - WM_NVS_ERR_OK: succeed
  *    - WM_NVS_ERR_FAIL: failed
  *
  * @warning All the data in the partition will be lost after wm_nvs_erase called. wm_nvs_deinit shoud be called
  *     first if the partition is initialized. It is usually called when setting factory recovery.
  *     It is better to reboot the system after calling this interface.
  */
int wm_nvs_erase(const char *partition_name);

/**
  * @brief  Show all nvs information
  *
  * @param[in]  partition_name nvs partition name
  * @param[in]  group_name nvs group name, NULL for all group in partition
  * @return
  *    - WM_NVS_ERR_OK: succeed
  *    - WM_NVS_ERR_FAIL: failed
  */
int wm_nvs_print(const char *partition_name, const char *group_name);

/**
  * @brief  open nvs operation handle
  *
  * @param[in]  partition_name nvs partition name, it will use default nvs partition if partition_name is null.
  * @param[in]  group_name nvs group name, max characters is 15 .
  * @param[in]  mode read write or only read, @ref wm_nvs_open_mode_t
  * @param[out]  handle nvs operation handle
  *
  * @return
  *    - WM_NVS_ERR_OK: succeed
  *    - WM_NVS_ERR_FAIL: failed
  * @note shoud be closed by wm_nvs_close if it is not used.
  */
int wm_nvs_open(const char *partition_name, const char *group_name, wm_nvs_open_mode_t mode, wm_nvs_handle_t *handle);

/**
  * @brief  close nvs operation handle
  *
  * @param[in]  handle nvs operation handle,obtained from wm_nvs_open.
  *
  * @return
  *    - WM_NVS_ERR_OK: succeed
  *    - WM_NVS_ERR_FAIL: failed
  */
int wm_nvs_close(wm_nvs_handle_t handle);

/**
 * @brief  Get string by key name
 *
 * @param[in]  handle nvs operation handle,obtained from wm_nvs_open.
 * @param[in]  key      nvs name
 * @param[out]  out_value    string data
 * @param[inout]  length    input out_value buffer size, output real value size
 *                          return WM_NVS_ERR_INVALID_PARAM if buf is not enough;
 *                          "hello" need 6 bytes, the out_value will be: "hello"\0
 *
 * @return
 *         - WM_NVS_ERR_OK on success
 *         - WM_NVS_ERR_INVALID_PARAM param error or buffer is not enough
 *         - WM_NVS_ERR_NOT_FOUND item is not found
 */
int wm_nvs_get_str(wm_nvs_handle_t handle, const char *key, char *out_value, size_t *length);

/**
 * @brief  Save string data
 *
 * @param[in]  handle nvs operation handle,obtained from wm_nvs_open.
 * @param[in]  key      nvs name
 * @param[out]  value    string data,must be end of \0
 *
 * @return
 *         - WM_NVS_ERR_OK on success
 *         - WM_NVS_ERR_INVALID_PARAM param error
 *         - WM_NVS_ERR_FAIL save item fail
 */
int wm_nvs_set_str(wm_nvs_handle_t handle, const char *key, const char *value);

/**
 * @brief  Get small binary data
 *
 * @param[in]  handle nvs operation handle,obtained from wm_nvs_open.
 * @param[in]     key      nvs name
 * @param[out]    data     binary data
 * @param[inout]  data_len binary data length
 *
 * Notes:
 *   If data is empty, data_len returns the actual length to hold the data.
 *
 * @return
 *         - WM_NVS_ERR_OK on success
 *         - WM_NVS_ERR_INVALID_PARAM param error or buffer is not enough
 *         - WM_NVS_ERR_NOT_FOUND item is not found
 */
int wm_nvs_get_binary(wm_nvs_handle_t handle, const char *key, void *data, size_t *data_len);

/**
 * @brief  Save small binary data
 *
 * @param[in]  handle nvs operation handle,obtained from wm_nvs_open.
 * @param[in]  key      nvs name
 * @param[in]  data     binary data
 * @param[in]  data_len binary data length
 *
 * @return
 *         - WM_NVS_ERR_OK on success
 *         - WM_NVS_ERR_INVALID_PARAM param error
 *         - WM_NVS_ERR_FAIL save item fail
 */
int wm_nvs_set_binary(wm_nvs_handle_t handle, const char *key, const void *data, size_t data_len);

/**
 * @brief  Get Binary Large OBject
 *
 * @param[in]  handle nvs operation handle,obtained from wm_nvs_open.
 * @param[in]     key      nvs name
 * @param[out]    blob     blob data
 * @param[inout]  blob_len blob data length
 *
 * Notes:
 *   If blob is empty, blob_len returns the actual length to hold the data.
 *
 * @return
 *         - WM_NVS_ERR_OK on success
 *         - WM_NVS_ERR_INVALID_PARAM param error or buffer is not enough
 *         - WM_NVS_ERR_NOT_FOUND item is not found
 */
int wm_nvs_get_blob(wm_nvs_handle_t handle, const char *key, void *blob, size_t *blob_len);

/**
 * @brief  Save binary data
 *
 * @param[in]  handle nvs operation handle,obtained from wm_nvs_open.
 * @param[in]  key      nvs name
 * @param[in]  blob     binary data
 * @param[in]  blob_len binary data length
 *
 * @return
 *         - WM_NVS_ERR_OK on success
 *         - WM_NVS_ERR_INVALID_PARAM param error
 *         - WM_NVS_ERR_FAIL save item fail
 */
int wm_nvs_set_blob(wm_nvs_handle_t handle, const char *key, const void *blob, size_t blob_len);
/**
 * @brief  Save int8 data
 *
 * @param[in]  handle nvs operation handle,obtained from wm_nvs_open.
 * @param[in]  key      nvs name
 * @param[in]  value    int8 value
 *
 * @return
 *         - WM_NVS_ERR_OK on success
 *         - WM_NVS_ERR_INVALID_PARAM param error
 *         - WM_NVS_ERR_FAIL save item fail
 */
int wm_nvs_set_i8(wm_nvs_handle_t handle, const char *key, int8_t value);

/**
 * @brief  Save uint8 data
 *
 * @param[in]  handle nvs operation handle,obtained from wm_nvs_open.
 * @param[in]  key      nvs name
 * @param[in]  value    uint8 value
 *
 * @return
 *         - WM_NVS_ERR_OK on success
 *         - WM_NVS_ERR_INVALID_PARAM param error
 *         - WM_NVS_ERR_FAIL save item fail
 */
int wm_nvs_set_u8(wm_nvs_handle_t handle, const char *key, uint8_t value);

/**
 * @brief  Save int16 data
 *
 * @param[in]  handle nvs operation handle,obtained from wm_nvs_open.
 * @param[in]  key      nvs name
 * @param[in]  value    int16 value
 *
 * @return
 *         - WM_NVS_ERR_OK on success
 *         - WM_NVS_ERR_INVALID_PARAM param error
 *         - WM_NVS_ERR_FAIL save item fail
 */
int wm_nvs_set_i16(wm_nvs_handle_t handle, const char *key, int16_t value);

/**
 * @brief  Save uint16 data
 *
 * @param[in]  handle nvs operation handle,obtained from wm_nvs_open.
 * @param[in]  key      nvs name
 * @param[in]  value    uint16 value
 *
 * @return
 *         - WM_NVS_ERR_OK on success
 *         - WM_NVS_ERR_INVALID_PARAM param error
 *         - WM_NVS_ERR_FAIL save item fail
 */
int wm_nvs_set_u16(wm_nvs_handle_t handle, const char *key, uint16_t value);

/**
 * @brief  Save int32 data
 *
 * @param[in]  handle nvs operation handle,obtained from wm_nvs_open.
 * @param[in]  key      nvs name
 * @param[in]  value    int32 value
 *
 * @return
 *         - WM_NVS_ERR_OK on success
 *         - WM_NVS_ERR_INVALID_PARAM param error
 *         - WM_NVS_ERR_FAIL save item fail
 */
int wm_nvs_set_i32(wm_nvs_handle_t handle, const char *key, int32_t value);

/**
 * @brief  Save uint32 data
 *
 * @param[in]  handle nvs operation handle,obtained from wm_nvs_open.
 * @param[in]  key      nvs name
 * @param[in]  value    uint32 value
 *
 * @return
 *         - WM_NVS_ERR_OK on success
 *         - WM_NVS_ERR_INVALID_PARAM param error
 *         - WM_NVS_ERR_FAIL save item fail
 */
int wm_nvs_set_u32(wm_nvs_handle_t handle, const char *key, uint32_t value);

/**
 * @brief  Save int64 data
 *
 * @param[in]  handle nvs operation handle,obtained from wm_nvs_open.
 * @param[in]  key      nvs name
 * @param[in]  value    int64 value
 *
 * @return
 *         - WM_NVS_ERR_OK on success
 *         - WM_NVS_ERR_INVALID_PARAM param error
 *         - WM_NVS_ERR_FAIL save item fail
 */
int wm_nvs_set_i64(wm_nvs_handle_t handle, const char *key, int64_t value);

/**
 * @brief  Save uint64 data
 *
 * @param[in]  handle nvs operation handle,obtained from wm_nvs_open.
 * @param[in]  key      nvs name
 * @param[in]  value    uint64 value
 *
 * @return
 *         - WM_NVS_ERR_OK on success
 *         - WM_NVS_ERR_INVALID_PARAM param error
 *         - WM_NVS_ERR_FAIL save item fail
 */
int wm_nvs_set_u64(wm_nvs_handle_t handle, const char *key, uint64_t value);

/**
 * @brief  Save float data
 *
 * @param[in]  handle nvs operation handle,obtained from wm_nvs_open.
 * @param[in]  key      nvs name
 * @param[in]  value    float value
 *
 * @return
 *         - WM_NVS_ERR_OK on success
 *         - WM_NVS_ERR_INVALID_PARAM param error
 *         - WM_NVS_ERR_FAIL save item fail
 */
int wm_nvs_set_float(wm_nvs_handle_t handle, const char *key, double value);

/**
 * @brief  Get int8 data
 *
 * @param[in]  handle nvs operation handle,obtained from wm_nvs_open.
 * @param[in]  key      nvs name
 * @param[out]  out_value    int8 value
 *
 * @return
 *         - WM_NVS_ERR_OK on success
 *         - WM_NVS_ERR_INVALID_PARAM param error
 *         - WM_NVS_ERR_NOT_FOUND item is not found
 */
int wm_nvs_get_i8(wm_nvs_handle_t handle, const char *key, int8_t *out_value);

/**
 * @brief  Get uint8 data
 *
 * @param[in]  handle nvs operation handle,obtained from wm_nvs_open.
 * @param[in]  key      nvs name
 * @param[out]  out_value    uint8 value
 *
 * @return
 *         - WM_NVS_ERR_OK on success
 *         - WM_NVS_ERR_INVALID_PARAM param error
 *         - WM_NVS_ERR_NOT_FOUND item is not found
 */
int wm_nvs_get_u8(wm_nvs_handle_t handle, const char *key, uint8_t *out_value);

/**
 * @brief  Get int16 data
 *
 * @param[in]  handle nvs operation handle,obtained from wm_nvs_open.
 * @param[in]  key      nvs name
 * @param[out]  out_value    int16 value
 *
 * @return
 *         - WM_NVS_ERR_OK on success
 *         - WM_NVS_ERR_INVALID_PARAM param error
 *         - WM_NVS_ERR_NOT_FOUND item is not found
 */
int wm_nvs_get_i16(wm_nvs_handle_t handle, const char *key, int16_t *out_value);

/**
 * @brief  Get uint16 data
 *
 * @param[in]  handle nvs operation handle,obtained from wm_nvs_open.
 * @param[in]  key      nvs name
 * @param[out]  out_value    uint16 value
 *
 * @return
 *         - WM_NVS_ERR_OK on success
 *         - WM_NVS_ERR_INVALID_PARAM param error
 *         - WM_NVS_ERR_NOT_FOUND item is not found
 */
int wm_nvs_get_u16(wm_nvs_handle_t handle, const char *key, uint16_t *out_value);

/**
 * @brief  Get int32 data
 *
 * @param[in]  handle nvs operation handle,obtained from wm_nvs_open.
 * @param[in]  key      nvs name
 * @param[out]  out_value    int32 value
 *
 * @return
 *         - WM_NVS_ERR_OK on success
 *         - WM_NVS_ERR_INVALID_PARAM param error
 *         - WM_NVS_ERR_NOT_FOUND item is not found
 */
int wm_nvs_get_i32(wm_nvs_handle_t handle, const char *key, int32_t *out_value);

/**
 * @brief  Get uint32 data
 *
 * @param[in]  handle nvs operation handle,obtained from wm_nvs_open.
 * @param[in]  key      nvs name
 * @param[out]  out_value    uint32 value
 *
 * @return
 *         - WM_NVS_ERR_OK on success
 *         - WM_NVS_ERR_INVALID_PARAM param error
 *         - WM_NVS_ERR_NOT_FOUND item is not found
 */
int wm_nvs_get_u32(wm_nvs_handle_t handle, const char *key, uint32_t *out_value);

/**
 * @brief  Get int64 data
 *
 * @param[in]  handle nvs operation handle,obtained from wm_nvs_open.
 * @param[in]  key      nvs name
 * @param[out]  out_value    int64 value
 *
 * @return
 *         - WM_NVS_ERR_OK on success
 *         - WM_NVS_ERR_INVALID_PARAM param error
 *         - WM_NVS_ERR_NOT_FOUND item is not found
 */
int wm_nvs_get_i64(wm_nvs_handle_t handle, const char *key, int64_t *out_value);

/**
 * @brief  Get uint64 data
 *
 * @param[in]  handle nvs operation handle,obtained from wm_nvs_open.
 * @param[in]  key      nvs name
 * @param[out]  out_value    uint64 value
 *
 * @return
 *         - WM_NVS_ERR_OK on success
 *         - WM_NVS_ERR_INVALID_PARAM param error
 *         - WM_NVS_ERR_NOT_FOUND item is not found
 */
int wm_nvs_get_u64(wm_nvs_handle_t handle, const char *key, uint64_t *out_value);

/**
 * @brief  Get float data
 *
 * @param[in]  handle nvs operation handle,obtained from wm_nvs_open.
 * @param[in]  key      nvs name
 * @param[out]  out_value    float value
 *
 * @return
 *         - WM_NVS_ERR_OK on success
 *         - WM_NVS_ERR_INVALID_PARAM param error
 *         - WM_NVS_ERR_NOT_FOUND item is not found
 */
int wm_nvs_get_float(wm_nvs_handle_t handle, const char *key, double *out_value);

/**
 * @brief  Delete the nvs data for the key
 *
 * @param[in]  handle nvs operation handle,obtained from wm_nvs_open.
 * @param[in]  key nvs name
 *
 * @return
 *         - WM_NVS_ERR_OK on success
 *         - WM_NVS_ERR_INVALID_PARAM param error
 *         - WM_NVS_ERR_NOT_FOUND item is not found
 */
int wm_nvs_del_key(wm_nvs_handle_t handle, const char *key);

/**
  * @brief  reset nvs. It is usually called at factory settings, and the system is restarted after the nvs reset.
  *
  * @param[in]  handle nvs operation handle,obtained from wm_nvs_open.
  *
  * @return
  *    - WM_NVS_ERR_OK: succeed
  *    - WM_NVS_ERR_FAIL: failed
  * @warning Calling this interface will clear all group data and cannot be recovered.
  */
int wm_nvs_del_group(wm_nvs_handle_t handle);

/**
 * @brief  find iterator, used for poll all or specified items.
 *
 * @param[in]  partition_name  nvs partition name
 * @param[in]  group    group name
 * @param[in]  type    data type,WM_NVS_TYPE_ANY for all types
 * @param[out]  output_iterator    iterator
 *
 * @return
 *         - WM_NVS_ERR_OK on success
 *         - WM_NVS_ERR_INVALID_PARAM param error
 *         - WM_NVS_ERR_FAIL fail
 * @note The output iterator handle must be release by wm_nvs_release_iterator
 */
int wm_nvs_entry_find(const char *partition_name, const char *group, wm_nvs_type_t type, wm_nvs_iterator_t *output_iterator);

/**
 * @brief  find iterator, used for poll all or specified items.
 *
 * @param[in]  handle  nvs operation handle, obtained from wm_nvs_open
 * @param[in]  type    data type,WM_NVS_TYPE_ANY for all types
 * @param[out]  output_iterator    iterator
 *
 * @return
 *         - WM_NVS_ERR_OK on success
 *         - WM_NVS_ERR_INVALID_PARAM param error
 *         - WM_NVS_ERR_FAIL fail
 * @note The output iterator handle must be release by wm_nvs_release_iterator
 */
int wm_nvs_entry_find_by_handle(wm_nvs_handle_t handle, wm_nvs_type_t type, wm_nvs_iterator_t *output_iterator);

/**
 * @brief  change to next iterator
 *
 * @param[inout]  iterator  iterator to next item
 *                out iterator:
 *                  ! NULL : next item
 *                  NULL   : iterator end, the memery is released
 *
 * @return
 *         - WM_NVS_ERR_OK on success
 *         - WM_NVS_ERR_INVALID_PARAM param error
 *         - WM_NVS_ERR_FAIL iterator end or fail
 */
int wm_nvs_entry_next(wm_nvs_iterator_t *iterator);

/**
 * @brief  get iterator info
 *
 * @param[in]  iterator  iterator
 * @param[out]  info    iterator entry information
 *
 * @return
 *         - WM_NVS_ERR_OK on success
 *         - WM_NVS_ERR_INVALID_PARAM param error
 *         - WM_NVS_ERR_FAIL fail
 */
int wm_nvs_entry_info(wm_nvs_iterator_t iterator, wm_nvs_entry_t *info);

/**
 * @brief  get iterator data
 *
 * @param[in]  iterator  iterator
 * @param[out]  data    iterator entry information
 * @param[inout]  data_len  data len, input buffer len, output real len,
 *                     return WM_NVS_ERR_INVALID_PARAM if input data_len is not enough
 *
 * @return
 *         - WM_NVS_ERR_OK on success
 *         - WM_NVS_ERR_INVALID_PARAM param error
 *         - WM_NVS_ERR_FAIL fail
 */
int wm_nvs_entry_data(wm_nvs_iterator_t iterator, void *data, size_t *data_len);

/**
 * @brief  get iterator data
 *
 * @param[in]  iterator  iterator
 *             release iteraotr memory
 *
 * @return
 *         - WM_NVS_ERR_OK on success
 *         - WM_NVS_ERR_INVALID_PARAM param error
 * @note
 *  iterator can be NULL
 */
int wm_nvs_release_iterator(wm_nvs_iterator_t iterator);

/**
 * @brief  get item type and size
 *
 * @param[in]  handle nvs operation handle,obtained from wm_nvs_open.
 * @param[in]  key  nvs item name
 * @param[out]  type  data type
 * @param[out]  size  data size
 * @return
 *         - WM_NVS_ERR_OK on success
 *         - WM_NVS_ERR_INVALID_PARAM param error
 *         - WM_NVS_ERR_NOT_FOUND
 */
int wm_nvs_get_info(wm_nvs_handle_t handle, const char *key, wm_nvs_type_t *type, size_t *size);

/**
 * @brief  get nvs status
 *
 * @param[out]  partition_name  nvs partition name
 * @param[out]  status  nvs status, @ref wm_nvs_status_t
 * @return
 *         - WM_NVS_ERR_OK on success
 *         - WM_NVS_ERR_INVALID_PARAM param error
 */
int wm_nvs_get_status(const char *partition_name, wm_nvs_status_t *status);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* end of __WM_NVS_H__ */

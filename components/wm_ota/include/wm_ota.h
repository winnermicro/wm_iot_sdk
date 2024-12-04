/**
 * @file wm_ota.h
 *
 * @brief OTA Module
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

#ifndef __WM_OTA_H__
#define __WM_OTA_H__

#include "wm_ota_ops.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_OTA_Enumerations WM OTA Enumerations
 * @brief WinnerMicro OTA Enumerations
 */

/**
 * @addtogroup WM_OTA_Enumerations
 * @{
 */

/**
 * @brief OTA status enumeration.
 *
 * Defines the various states of the OTA process.
 */
typedef enum {
    WM_OTA_STATUS_START,             /**< The OTA process has started */
    WM_OTA_STATUS_CONNECTION_START,  /**< Starting the connection for OTA */
    WM_OTA_STATUS_CONNECTION_FAILED, /**< Connection for OTA has failed */
    WM_OTA_STATUS_CONNECTED,         /**< Successfully connected for OTA */
    WM_OTA_STATUS_DOWNLOAD_START,    /**< Starting the download of the OTA image */
    WM_OTA_STATUS_DOWNLOAD_FAILED,   /**< Download of the OTA image has failed */
    WM_OTA_STATUS_DOWNLOADED,        /**< OTA image has been downloaded */
    WM_OTA_STATUS_UPGRADE_START,     /**< Starting the upgrade process */
    WM_OTA_STATUS_UPGRADE_FAILED,    /**< Upgrade process has failed */
    WM_OTA_STATUS_ABORT              /**< OTA process has been aborted */
} wm_ota_status_t;

/**
 * @}
 */

/**
 * @defgroup WM_OTA_Structures WM OTA Structures
 * @brief WinnerMicro OTA Structures
 */

/**
 * @addtogroup WM_OTA_Structures
 * @{
 */

/**
 * @brief OTA state structure.
 *
 * Holds the current status and progress of the OTA operation.
 */
typedef struct {
    wm_ota_status_t status; /**< Current status of the OTA operation */
    uint32_t progress;      /**< Download progress in percentage */
} wm_ota_state_t;

/**
 * @}
 */

/**
 * @defgroup WM_OTA_Type_Definitions WM OTA Type Definitions
 * @brief WinnerMicro OTA Type Definitions
 */

/**
 * @addtogroup WM_OTA_Type_Definitions
 * @{
 */

/**
 * @brief OTA connection function pointer type.
 *
 * Pointer to a function that establishes a connection for OTA.
 *
 * @param[in] handle Pointer to the connection handle.
 * @param[in] offset Offset for the connection.
 *
 * @return An integer result of the connection attempt.
 */
typedef int (*wm_ota_connect_f)(void **handle, uint32_t offset);

/**
 * @brief OTA firmware retrieval function pointer type.
 *
 * Pointer to a function that retrieves firmware data for OTA.
 *
 * @param[in] handle The handle from which to retrieve firmware.
 * @param[in] offset Offset from which to start retrieving data.
 * @param[in] fw_buf Buffer to store the retrieved firmware data.
 * @param[in] buf_size Size of the firmware buffer.
 * @param[in] got Pointer to store the amount of data retrieved.
 *
 * @return An integer result of the firmware retrieval attempt.
 */
typedef int (*wm_ota_get_firmware_f)(void *handle, uint32_t offset, uint8_t *fw_buf, uint32_t buf_size, uint32_t *got);

/**
 * @brief OTA session close function pointer type.
 *
 * Pointer to a function that closes an OTA session.
 *
 * @param[in] handle The handle to close.
 *
 * @return An integer result of the close operation.
 */
typedef int (*wm_ota_close_f)(void *handle);

/**
 * @brief OTA state callback function pointer type.
 *
 * Pointer to a function that is called to update the state of the OTA operation.
 *
 * @param[in] ota_state Pointer to the OTA state structure to be updated.
 *
 * @return None
 */
typedef void (*wm_ota_state_callback_t)(wm_ota_state_t *ota_state);

/**
 * @}
 */

/**
 * @addtogroup WM_OTA_Structures
 * @{
 */

/**
 * @brief OTA session callback functions structure.
 *
 * Contains the callback functions for establishing a connection, getting firmware, and closing the session.
 */
typedef struct {
    wm_ota_connect_f ota_connect_cb;           /**< Callback for connecting to the OTA server */
    wm_ota_get_firmware_f ota_get_firmware_cb; /**< Callback for getting firmware data */
    wm_ota_close_f ota_close_cb;               /**< Callback for closing the OTA session */
} wm_ota_session_t;

/**
 * @brief OTA context structure.
 *
 * Contains the context information for the OTA operation.
 */
typedef struct {
    void *handle;                         /**< Socket used for OTA communication */
    int ota_conn_ret;                     /**< Return value of the last connection attempt */
    uint32_t wrote_offset;                /**< Offset of the data written to the OTA partition */
    wm_ota_ops_ctx_t ota_ops_ctx;         /**< OTA operations context */
    wm_ota_session_t ota_session;         /**< OTA session with callback functions */
    wm_ota_state_callback_t ota_state_cb; /**< Callback for state updates */
    wm_ota_state_t ota_state;             /**< Current state of the OTA operation */
} wm_ota_ctx_t;

/**
 * @}
 */

/**
 * @defgroup WM_OTA_Functions WM OTA Functions
 * @brief WinnerMicro OTA Functions
 */

/**
 * @addtogroup WM_OTA_Functions
 * @{
 */

/**
 * @brief Initializes the OTA context for an update.
 *
 * @param[in] ota_ctx Pointer to the OTA context structure to initialize.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 */
int wm_ota_init(wm_ota_ctx_t *ota_ctx);

/**
 * @brief Public interface to start the OTA update process with error handling.
 *
 * @param[in] ota_ctx Pointer to the OTA context.
 * @param[in] reboot Flag indicating if a system reboot is required post-update.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - WM_ERR_OTA_NO_INIT: OTA context not initialized
 *    - Other error codes based on the underlying operations
 */
int wm_ota_start(wm_ota_ctx_t *ota_ctx, bool reboot);

/**
 * @brief Stops the OTA update process.
 *
 * @param[in] ota_ctx Pointer to the OTA context.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 */
int wm_ota_stop(wm_ota_ctx_t *ota_ctx);
/**
 * @brief Deinitializes the OTA context and aborts any ongoing OTA operation.
 *
 * @param[in] ota_ctx Pointer to the OTA context structure to be cleared.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 */
int wm_ota_deinit(wm_ota_ctx_t *ota_ctx);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_OTA_H__ */

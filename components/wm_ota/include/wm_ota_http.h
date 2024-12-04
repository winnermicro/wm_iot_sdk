/**
 * @file wm_ota_http.h
 *
 * @brief OTA_HTTP Module
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

#ifndef __WM_OTA_HTTP_H__
#define __WM_OTA_HTTP_H__

#include "wm_ota.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_OTA_HTTP_Type_Definitions WM OTA_HTTP Type Definitions
 * @brief WinnerMicro OTA_HTTP Type Definitions
 */

/**
 * @addtogroup WM_OTA_HTTP_Type_Definitions
 * @{
 */

/**
 * @brief Type definition for a callback function used during the OTA HTTP firmware download process.
 *
 * This callback function is invoked whenever a chunk of firmware data is received.
 * It takes two parameters: a pointer to the data buffer and the length of the data received.
 *
 * @param[out] data A pointer to the buffer containing the received firmware data.
 * @param[out] len The length of the received data in bytes.
 *
 * @return None
 */
typedef void (*wm_ota_http_get_file_callback_t)(uint8_t *data, uint32_t len);

/**
 * @brief Structure definition for the configuration of an OTA HTTP firmware update.
 *
 * This structure is used to pass configuration parameters to the OTA HTTP update function.
 */

/**
 * @}
 */

/**
 * @defgroup WM_OTA_HTTP_Structures WM OTA_HTTP Structures
 * @brief WinnerMicro OTA_HTTP Structures
 */

/**
 * @addtogroup WM_OTA_HTTP_Structures
 * @{
 */
typedef struct {
    char *fw_url; /**< Pointer to a string containing the URL of the firmware to be downloaded */
    wm_ota_http_get_file_callback_t ota_get_file_cb; /**< Callback function to be called when firmware data is received */
    wm_ota_state_callback_t ota_state_cb; /**< Callback function to be called to notify about the state of the OTA process */
    bool reboot; /**< Flag indicating whether to automatically reboot the device after successful firmware download and verification */
} wm_ota_http_cfg_t;

/**
 * @}
 */

/**
 * @defgroup WM_OTA_HTTP_Functions WM OTA_HTTP Functions
 * @brief WinnerMicro OTA_HTTP Functions
 */

/**
 * @addtogroup WM_OTA_HTTP_Functions
 * @{
 */

/**
 * @brief Function to initiate an OTA HTTP firmware update using the provided configuration.
 *
 * This function starts the OTA process by downloading the firmware from the specified URL.
 * It uses the callbacks provided in the configuration structure to handle the received data
 * and to notify about the state of the OTA process.
 *
 * @param[in] cfg A pointer to a wm_ota_http_cfg_t structure containing the configuration for the OTA update.
 *
 * @return
 * - WM_ERR_SUCCESS: Indicates that the OTA update has been successfully started.
 * - WM_ERR_OTA_ALREADY_RUNNING: Indicates that an OTA update is currently underway.
 * - WM_ERR_NO_MEM: Memory allocation for the OTA context or URL has failed.
 * - WM_ERR_INVALID_PARAM: One or more parameters provided are invalid.
 * - WM_ERR_OTA_NO_GOT_IP: The device is either not connected to Wi-Fi or has not yet acquired an IP address.
 * - Other errors may be propagated from the functions invoked during this process.
 */
int wm_ota_http_update(wm_ota_http_cfg_t *cfg);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_OTA_HTTP_H__ */

/**
 * @file wm_ota_ops.h
 *
 * @brief OTA OPS Module
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

#ifndef __WM_OTA_OPS_H__
#define __WM_OTA_OPS_H__

#include "wm_types.h"
#include "wm_drv_crc.h"
#include "wm_error.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_OTA_OPS_Macros WM OTA_OPS Macros
 * @brief WinnerMicro OTA_OPS Macros
 */

/**
 * @addtogroup WM_OTA_OPS_Macros
 * @{
 */

#define OTA_FW_HEADER_SIZE_IN_BIN  (0x40)       /**< Firmware header size in binary: 64 bytes */
#define OTA_SIZE_UNKNOWN           (0xFFFFFFFF) /**< Unknown new image size */
#define OTA_WITH_SEQUENTIAL_WRITES (0xFFFFFFFE) /**< Unknown size, incremental erase possible */

/**
 * @}
 */

/**
 * @defgroup WM_OTA_OPS_Enumerations WM OTA_OPS Enumerations
 * @brief WinnerMicro OTA_OPS Enumerations
 */

/**
 * @addtogroup WM_OTA_OPS_Enumerations
 * @{
 */

typedef enum {
    WM_OTA_OPS_COMPRESS_NONE = 0, /**< No compression */
    WM_OTA_OPS_COMPRESS_XZ   = 1, /**< xz compression */
    WM_OTA_OPS_COMPRESS_MAX,
} wm_ota_ops_compress_type_t;

typedef enum {
    WM_ERR_OTA_NO_INIT           = WM_ERR_OTA_BASE - 1, /**< Error: OTA not initialized */
    WM_ERR_OTA_HEADER_INVALID    = WM_ERR_OTA_BASE - 2, /**< Error: Invalid OTA header */
    WM_ERR_OTA_SAME_VERSION      = WM_ERR_OTA_BASE - 3, /**< Error: Version unchanged */
    WM_ERR_OTA_CRC_ERROR         = WM_ERR_OTA_BASE - 4, /**< Error: CRC error in OTA data */
    WM_ERR_OTA_FW_OVERFLOW       = WM_ERR_OTA_BASE - 5, /**< Error: Firmware overflow during OTA */
    WM_ERR_OTA_TIMEOUT           = WM_ERR_OTA_BASE - 6, /**< Error: Timeout occurred during OTA */
    WM_ERR_OTA_NO_GOT_IP         = WM_ERR_OTA_BASE - 7, /**< Error: No IP address obtained for OTA */
    WM_ERR_OTA_CONNECTION_FAILED = WM_ERR_OTA_BASE - 8, /**< Error: Connection failed for OTA */
    WM_ERR_OTA_ALREADY_RUNNING   = WM_ERR_OTA_BASE - 9, /**< Error: OTA operation already running */
    WM_ERR_OTA_SHA256_ECDSA      = WM_ERR_OTA_BASE - 10 /**< Error: SHA256-ECDSA verification error */
} wm_ota_ops_err_t;

/**
 * @}
 */

/**
 * @defgroup WM_OTA_OPS_Unions WM OTA_OPS Unions
 * @brief WinnerMicro OTA_OPS Unions
 */

/**
 * @addtogroup WM_OTA_OPS_Unions
 * @{
 */

/**
 * @brief Image attribute union.
 *
 * This union allows access to image attributes either through individual
 * bitfields or as a whole 32-bit unsigned integer. It includes flags for
 * image type, encryption, private key selection, signature presence, zip type,
 * PSRAM I/O, and reserved bits.
 */
typedef union {
    struct {
        uint32_t img_type       : 4;  /**< [3:0] Image type enumeration */
        uint32_t code_encrypt   : 1;  /**< [4] Indicates if the code in flash is encrypted */
        uint32_t prikey_sel     : 3;  /**< [7:5] Private key selection for encryption */
        uint32_t signature      : 1;  /**< [8] Presence of a 128-byte signature at the end of the image */
        uint32_t reserved_9     : 7;  /**< [15:9] Reserved for future use */
        uint32_t gzip_enable    : 1;  /**< [16] Enable gzip compression for the image */
        uint32_t psram_io       : 1;  /**< [17] PSRAM I/O configuration */
        uint32_t erase_block_en : 1;  /**< [18] Flash erase block enable bit */
        uint32_t erase_always   : 1;  /**< [19] Flash erase always bit */
        uint32_t compress_type  : 2;  /**< [21:20] Compression algorithm type, 1:xz */
        uint32_t reserved_22    : 10; /**< [31:22] Reserved for future use */
    } bits;
    uint32_t val; /**< The raw value of the image attribute */
} wm_img_attr_t;

/**
 * @}
 */

/**
 * @defgroup WM_OTA_OPS_Structures WM OTA_OPS Structures
 * @brief WinnerMicro OTA_OPS Structures
 */

/**
 * @addtogroup WM_OTA_OPS_Structures
 * @{
 */

/**
 * @brief OTA Header structure.
 *
 * The OTA Header structure contains various attributes and metadata required
 * for Over-The-Air (OTA) update functionality. It includes image attributes,
 * addresses, lengths, version strings, and checksums.
 */
typedef struct {
    uint32_t magic_no;                  /**< Magic number to identify the OTA image */
    wm_img_attr_t img_attr;             /**< Bitfields for image attributes */
    uint32_t img_addr;                  /**< Address where the image will be executed */
    uint32_t img_len;                   /**< Length of the binary image in bytes */
    uint32_t img_header_addr;           /**< Address of the image header */
    uint32_t upgrade_img_addr;          /**< Address of the upgrade image */
    uint32_t org_checksum;              /**< Original checksum of the image for verification */
    uint32_t upd_no;                    /**< Update number for version tracking */
    unsigned char ver[16];              /**< Version string, 16 bytes long */
    uint32_t log_level            : 3;  /**< Log level */
    uint32_t skip_esc_check       : 1;  /**< Skip escape check */
    uint32_t skip_upgrade_check   : 1;  /**< Skip upgrade check */
    uint32_t skip_integrity_check : 1;  /**< Skip integrity check */
    uint32_t _reserved0           : 26; /**< Reserved for future use */
    uint32_t _reserved1;                /**< Reserved for future use */
    struct wm_ota_header_t *next;       /**< Pointer to the next header for a possible linked list */
    uint32_t hd_checksum;               /**< Checksum of the header for integrity verification */
} wm_ota_header_t;

/**
 * @brief OTA Operations Context structure.
 *
 * The `wm_ota_ops_ctx_t` structure holds the context information required for
 * performing Over-The-Air (OTA) updates. It includes the application size,
 * OTA addresses, flags indicating erase needs, OTA header, and CRC context
 * for data integrity verification.
 */
typedef struct {
    uint32_t app_addr;          /**< Address of the application partition */
    uint32_t app_size;          /**< Size of the application partition */
    uint32_t app_ota_addr;      /**< Address in flash where the OTA image is stored */
    uint32_t app_ota_size;      /**< Size of the OTA partition */
    uint32_t need_erase;        /**< Flag indicating if the flash needs to be erased before writing */
    uint32_t wrote_addr;        /**< Address of the data written to the OTA partition */
    wm_ota_header_t ota_header; /**< OTA Header containing metadata about the firmware image */
    wm_drv_crc_cfg_t crc_ctx;   /**< CRC context used for calculating the checksum of the OTA data */
} wm_ota_ops_ctx_t;

/**
 * @}
 */

/**
 * @defgroup WM_OTA_OPS_Functions WM OTA_OPS Functions
 * @brief WinnerMicro OTA_OPS Functions
 */

/**
 * @addtogroup WM_OTA_OPS_Functions
 * @{
 */

/**
 * @brief Retrieve the partition information for the OTA update.
 *
 * This function is used to determine the location and size of the OTA (Over-The-Air) partitions before performing an OTA upgrade,
 * and it then stores this information in the OTA operation context structure.
 *
 * @param[in] wm_ota_ops_ctx Pointer to the OTA context structure.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - WM_ERR_FAILED: failed to find the partition
 */
int wm_ota_ops_get_ota_partition(wm_ota_ops_ctx_t *wm_ota_ops_ctx);

/**
 * @brief Begin the OTA update process.
 *
 * This function is used to initialize the resources and state necessary for OTA (Over-The-Air) updates,
 * which includes erasing the required flash memory areas and preparing to receive new firmware data.
 *
 * @param[in] wm_ota_ops_ctx Pointer to the OTA context structure.
 * @param[in] image_size The size of the OTA image in bytes.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - WM_ERR_FAILED: failed to erase the flash
 *
 * @note
 *      Determine whether a full erase of the OTA partition is necessary based on the image_size value:
 *      - 0 or OTA_SIZE_UNKNOWN: Perform a full erase of the 'app_ota' partition.
 *      - OTA_WITH_SEQUENTIAL_WRITES: No need to erase now; the erase will occur during the subsequent write process.
 *      - Actual image size: Erase the 'app_ota' partition to match the actual size of the image.
 */
int wm_ota_ops_begin(wm_ota_ops_ctx_t *wm_ota_ops_ctx, size_t image_size);

/**
 * @brief Write data to the OTA partition.
 *
 * This function is used to write firmware data into the device's flash memory during the OTA (Over-The-Air) update process,
 * handling necessary sector erasure and performing CRC verification to ensure the integrity and consistency of the data.
 *
 * @param[in] wm_ota_ops_ctx Pointer to the OTA context structure.
 * @param[in] data Pointer to the data buffer to be written.
 * @param[in] size Size of the data buffer in bytes.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - Other error codes based on the underlying flash write operation
 */
int wm_ota_ops_write(wm_ota_ops_ctx_t *wm_ota_ops_ctx, const uint8_t *data, uint32_t size);

/**
 * @brief Finalize the OTA update process and verify the integrity of the written data.
 *
 * This function is used to complete the final step of the OTA (Over-The-Air) process,
 * with its primary task being to verify the newly written firmware image to ensure its integrity and executability.
 *
 * @param[in] wm_ota_ops_ctx Pointer to the OTA context structure.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument
 *    - WM_ERR_OTA_CRC_ERROR: CRC error indicating data corruption
 */
int wm_ota_ops_end(wm_ota_ops_ctx_t *wm_ota_ops_ctx);

/**
 * @brief Reboot the system to apply the OTA update.
 *
 * This function is used to trigger a system reboot in order to apply an OTA (Over-The-Air) update.
 * It is called in the final stage of the OTA update process, after the new firmware has been successfully written to
 * the device's flash memory and verified, to restart the device.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - Other error codes based on the underlying system reset operation
 */
int wm_ota_ops_reboot(void);

/**
 * @brief Abort the OTA update process and clean up the OTA context.
 *
 * The function is used to terminate the current OTA (Over-The-Air) operation.
 * When an error occurs during the OTA upgrade process or the user wishes to cancel the ongoing upgrade,
 * this function can clear all information in the OTA context structure and reset the OTA status.
 *
 * @param[in] wm_ota_ops_ctx Pointer to the OTA context structure to be cleared.
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - WM_ERR_INVALID_PARAM: invalid argument if wm_ota_ops_ctx is NULL
 */
int wm_ota_ops_abort(wm_ota_ops_ctx_t *wm_ota_ops_ctx);

/**
 * @brief Retrieves the version information of the currently running firmware.
 *
 * This function is used to obtain the version number of the firmware that is currently running on the device.
 * It copies the version string to the provided character array.
 *
 * @param[out] version Pointer to a character array where the current firmware version will be stored.
 *
 * @return
 *    - WM_ERR_SUCCESS: Successfully retrieved the current firmware version.
 *    - WM_ERR_INVALID_PARAM: If the provided version pointer is NULL, this error code is returned.
 */
int wm_ota_ops_get_version(char *version);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_OTA_OPS_H__ */

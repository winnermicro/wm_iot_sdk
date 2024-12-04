/**
 * @file wm_drv_sdh_sdmmc.h
 *
 * @brief DRV_SDH_SDMMC Module
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

#ifndef __WM_DRV_SDH_SDMMC_H__
#define __WM_DRV_SDH_SDMMC_H__

#include "wm_dt.h"
#include "wm_hal_sdh.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_DRV_SDH_SDMMC_Macros WM DRV_SDH_SDMMC Macros
 * @brief WinnerMicro DRV_SDH_SDMMC Macros
 */

/**
 * @addtogroup WM_DRV_SDH_SDMMC_Macros
 * @{
 */

/**
 * @brief Defines the maximum number of blocks supported by the SDHC/SDMMC driver.
 *
 */
#define WM_DRV_SDH_SDMMC_MAX_NUM_BLOCKS (127)

/**
 * @}
 */

/**
 * @defgroup WM_DRV_SDH_SDMMC_Enumerations WM DRV_SDH_SDMMC Enumerations
 * @brief WinnerMicro DRV_SDH_SDMMC Enumerations
 */

/**
 * @addtogroup WM_DRV_SDH_SDMMC_Enumerations
 * @{
 */

/**
 * @brief Enumeration for SDH_SDMMC card types
 */
typedef enum {
    WM_DRV_SDH_SDMMC_CARD_TYPE_SD   = 0x01, /**< Standard SD card */
    WM_DRV_SDH_SDMMC_CARD_TYPE_SDSC = 0x02, /**< SD Standard Capacity */
    WM_DRV_SDH_SDMMC_CARD_TYPE_SDHC = 0x03, /**< SD High Capacity */
    WM_DRV_SDH_SDMMC_CARD_TYPE_MAX
} wm_drv_sdh_sdmmc_card_type_t;

/**
 * @brief Enumeration for SDH_SDMMC card CSD versions
 */
typedef enum {
    WM_DRV_SDH_SDMMC_CARD_CSD_VER_1 = 0x01, /**< CSD version 1 */
    WM_DRV_SDH_SDMMC_CARD_CSD_VER_2 = 0x02, /**< CSD version 2 */
    WM_DRV_SDH_SDMMC_CARD_CSD_VER_MAX
} wm_drv_sdh_sdmmc_card_csd_ver_t;

/**
 * @}
 */

/**
 * @defgroup WM_DRV_SDH_SDMMC_Structures WM DRV_SDH_SDMMC Structures
 * @brief WinnerMicro DRV_SDH_SDMMC Structures
 */

/**
 * @addtogroup WM_DRV_SDH_SDMMC_Structures
 * @{
 */

/**
 * @brief Structure representing the card information
 */
typedef struct {
    long long capacity;                      /**< Card capacity in bytes */
    uint32_t block_size;                     /**< Card block size */
    wm_drv_sdh_sdmmc_card_csd_ver_t csd_ver; /**< CSD version */
    wm_drv_sdh_sdmmc_card_type_t type;       /**< Card type */
    uint32_t rca;                            /**< Relative Card Address */
} wm_drv_sdh_sdmmc_card_info_t;

/**
 * @}
 */

/**
 * @defgroup WM_DRV_SDH_SDMMC_Functions WM DRV_SDH_SDMMC Functions
 * @brief WinnerMicro DRV_SDH_SDMMC Functions
 */

/**
 * @addtogroup WM_DRV_SDH_SDMMC_Functions
 * @{
 */

/**
 * @brief Initializes the SDH SDMMC device.
 *
 * This function initializes the SDH SDMMC device using the specified device name.
 * It allocates resources, sets up necessary configurations, and prepares the device for further operations.
 *
 * @param[in] dev_name The name of the device to initialize, as defined in the device tree.
 *
 * @return
 *    - A pointer to the device structure if the initialization succeeds.
 *    - NULL if the initialization fails or the device name is not recognized, or the device has already been initialized.
 */
wm_device_t *wm_drv_sdh_sdmmc_init(char *dev_name);

/**
 * @brief Deinitializes the SDH SDMMC device.
 *
 * This function is used to release the resources and stop any ongoing operations of the SDH SDMMC device.
 * It should be called when the device is no longer needed to ensure clean resource management.
 *
 * @param[in] dev Pointer to the device structure that needs to be deinitialized.
 *
 * @return
 *    - WM_ERR_SUCCESS if the deinitialization succeeds.
 *    - WM_ERR_INVALID_PARAM if the provided device pointer is NULL or invalid.
 *    - Other error codes may be returned based on the specific failure.
 */
int wm_drv_sdh_sdmmc_deinit(wm_device_t *dev);

/**
 * @brief Reads blocks from the SDH SDMMC device.
 *
 * This function reads a specified number of data blocks from the SDH SDMMC device starting from the specified block address.
 * It's used for transferring data from the SDH SDMMC device to the host memory.
 *
 * @param[in] dev Pointer to the device structure of the SDH SDMMC.
 * @param[in] buf Pointer to the buffer where the read data will be stored.
 * @param[in] start_block The starting block number from where to read the data.
 * @param[in] num_blocks The number of blocks to read.
 *
 * @return
 *    - WM_ERR_SUCCESS if the read operation completes successfully.
 *    - WM_ERR_INVALID_PARAM if the device pointer or buffer pointer is NULL, or if the parameters are out of valid range.
 *    - Other error codes may be returned to reflect different failure conditions.
 *
 * @note
 *    - The operation block cannot exceed the capacity of the card, otherwise it will result in data loss
 *    - The maximum number of num_blocks is 127 blocks
 */
int wm_drv_sdh_sdmmc_read_blocks(wm_device_t *dev, uint8_t *buf, uint32_t start_block, uint32_t num_blocks);

/**
 * @brief Writes blocks to the SDH SDMMC device.
 *
 * This function writes a specified number of data blocks to the SDH SDMMC device starting from the specified block address.
 * It's used for transferring data from the host memory to the SDH SDMMC device.
 *
 * @param[in] dev Pointer to the device structure of the SDH SDMMC.
 * @param[in] buf Pointer to the buffer containing the data to be written.
 * @param[in] start_block The starting block number where the data will be written.
 * @param[in] num_blocks The number of blocks to write.
 *
 * @return
 *    - WM_ERR_SUCCESS if the write operation completes successfully.
 *    - WM_ERR_INVALID_PARAM if the device pointer or buffer pointer is NULL, or if the parameters are out of valid range.
 *    - Other error codes may be returned to reflect different failure conditions.
 *
 * @note
 *    - The operation block cannot exceed the capacity of the card, otherwise it will result in data loss
 *    - The maximum number of num_blocks is 127 blocks
 */
int wm_drv_sdh_sdmmc_write_blocks(wm_device_t *dev, const uint8_t *buf, uint32_t start_block, uint32_t num_blocks);

/**
 * @brief Gets the card information of the SDH_SDMMC device.
 *
 * This function retrieves the card information such as capacity, block size, card type, 
 * CSD version, RCA, and other relevant details from the SDH_SDMMC device.
 *
 * @param[in] dev Pointer to the device structure.
 * @param[out] card_info Pointer to the structure where the card information will be stored.
 *
 * @return
 *    - WM_ERR_SUCCESS Success
 *    - WM_ERR_INVALID_PARAM Invalid parameter
 */
int wm_drv_sdh_sdmmc_get_card_info(wm_device_t *dev, wm_drv_sdh_sdmmc_card_info_t *card_info);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_DRV_SDH_SDMMC_H__ */

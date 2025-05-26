/**
 * @file wm_drv_flash.h
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

#ifndef __WM_DRV_FLASH_H__
#define __WM_DRV_FLASH_H__

#include "wm_dt.h"
#include "wm_hal_flash.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_FLASH_Structs WM flash Structs
 * @brief WinnerMicro flash Structs
 */

/**
 * @addtogroup WM_FLASH_Structs
 * @{
 */

/**
 * @struct wm_drv_flash_info_t
 * @brief flash infomation
 */
typedef struct {
    uint32_t flash_size;                          /**< unit is byte */
    uint16_t sector_size;                         /**< unit is byte */
    uint16_t page_size;                           /**< unit is byte */
    uint8_t vendor_id;                            /**< vendor ID */
    uint32_t unique_id[FLASH_UNIQUE_ID_SIZE / 4]; /**< store flash unique id */
} wm_drv_flash_info_t;

/**
 * @}
 */

/**
 * @defgroup WM_DRV_FLASH_APIs WM DRV Flash APIs
 * @brief WinnerMicro DRV Flash APIs
 */

/**
 * @addtogroup WM_DRV_FLASH_APIs
 * @{
 */

/**
  * @brief     write data to flash without erase
  *
  * @param[in] dev  flash driver device
  * @param[in] addr begin address in flash that will be write
  * @param[in] wr_buf  the source data address
  * @param[in] wr_len write data length
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  *
  * @warning, must erase before call this API, othewise the write data maybe incorrert
  */
int wm_drv_flash_write(wm_device_t *dev, uint32_t addr, uint8_t *wr_buf, uint32_t wr_len);

/**
  * @brief     write data to flash with erase
  *            The non-written data in the erased sector block will be read out first.
  *            This data will be combined with the data to be written, and then they will
  *            be written together after the sector is erased.
  *
  * @param[in] dev  flash driver device
  * @param[in] addr begin address in flash that will be write
  * @param[in] wr_buf  the source data address
  * @param[in] wr_len write data length

  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  *
  * @warning, Do not poweroff or reboot system when excecute this API, othewise flash data maybe lost
  */
int wm_drv_flash_write_with_erase(wm_device_t *dev, uint32_t addr, uint8_t *wr_buf, uint32_t wr_len);

/**
  * @brief     read data from flash
  *
  * @param[in] dev  flash driver device
  * @param[in] addr  begin address in flash that will be read
  * @param[out] rd_buf store read data address
  * @param[in] rd_len read data length
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_drv_flash_read(wm_device_t *dev, uint32_t addr, uint8_t *rd_buf, uint32_t rd_len);

/**
  * @brief     get flash device information
  *
  * @param [in] dev flash driver device, flash device information will store into dev->device_info
  * @param [out] flash_info store get information about this flash
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_drv_flash_get_device_info(wm_device_t *dev, wm_drv_flash_info_t *flash_info);

/**
  * @brief     erase flash begin with user's specicy address
  *
  * @param [in] dev  flash driver device
  * @param [in] addr begin address in flash that will be erase
  * @param [in] erase_len erase length
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_drv_flash_erase_region(wm_device_t *dev, uint32_t addr, uint32_t erase_len);

/**
  * @brief  erase flash by sector
  *
  * @param [in] dev  flash driver device
  * @param [in] sector_idx the start index will be erased
  * @param [in] sector_count erase sector number
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_drv_flash_erase_sector(wm_device_t *dev, uint32_t sector_idx, uint32_t sector_count);

/**
  * @brief     erase flash all region
  *
  * @param [in] dev  flash driver device
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_drv_flash_erase_chip(wm_device_t *dev);

/**
  * @brief     Initialize flash driver
  *
  * @param [in] dev_name  flash name will be inititaled
  *
  * @return
  *    - flash device poiniter: succeed
  *    - NULL: invalid argument or failed
  */
wm_device_t *wm_drv_flash_init(char *dev_name);

/**
  * @brief     deinit flash driver
  *
  * @param [in] dev  flash driver device
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_drv_flash_deinit(wm_device_t *dev);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_DRV_FLASH_H__ */

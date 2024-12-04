/**

@file wm_hal_flash.h
*
@brief flash Driver Module
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

#ifndef __WM_HAL_FLASH_H__
#define __WM_HAL_FLASH_H__

#include "wm_flash_reg_struct.h"
#include "wm_ll_flash.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FLASH_UNIQUE_ID_SIZE 16

typedef struct {
    uint8_t vendor_id;                            //flash verndor id
    uint32_t size;                                //flash size, unit is byte
    uint32_t page_size;                           //page size, uint is byte
    uint32_t sector_size;                         //sector size, unit is byte
    uint32_t block_size;                          //block size, unit is byte
    uint32_t unique_id[FLASH_UNIQUE_ID_SIZE / 4]; //flash unique id
} wm_flash_device_info_t;

typedef struct {
    char *chip_name;
    uint8_t quad_spi;  //init paramter, 0: disable quad SPI, 1: enable quad SPI.  support stand SPI, dual SPI, quad SPI
    uint32_t reg_base; //init paramter, flash reaister base address
    wm_flash_device_info_t device_info;
    void *priv_data;
} wm_hal_flash_dev_t;

/**
  * @brief     Initialize flash
  *
  * @param dev  config about flash device
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_hal_flash_init(wm_hal_flash_dev_t *dev);

/**
  * @brief     deinit flash
  *
  * @param dev  config about flash device
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_hal_flash_deinit(wm_hal_flash_dev_t *dev);

/**
  * @brief     write data by page( one page is 256byte)
  *
  * @param dev  flash device object pointer
  * @param flash_addr the flash address will be writed
  * @param buf the data source address
  * @param write_page_num the page number will be write
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_hal_flash_write_pages(wm_hal_flash_dev_t *dev, uint32_t flash_addr, uint8_t *buf, uint32_t write_page_num);

/**
  * @brief     read data by page
  *
  * @param dev  flash device object pointer
  * @param flash_addr the flash address will be writed
  * @param read_buf the buffer store read data
  * @param read_page_num the page number will be read
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_hal_flash_read_pages(wm_hal_flash_dev_t *dev, uint32_t flash_addr, uint8_t *read_buf, uint32_t read_page_num);

/**
  * @brief     read data by sector(one sector is 4096byte)
  *
  * @param dev  flash device object pointer
  * @param flash_addr the flash address will be writed
  * @param read_buf the buffer store read data
  * @param read_sector_num the sector number will be read
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_hal_flash_read_sectors(wm_hal_flash_dev_t *dev, uint32_t flash_addr, uint8_t *read_buf, uint32_t read_sector_num);

/**
  * @brief  erase data by sector
  *
  * @param dev  flash device object pointer
  * @param flash_addr the flash address will be writed
  * @param sector_num the sector number will be erased
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_hal_flash_erase_sectors(wm_hal_flash_dev_t *dev, uint32_t flash_addr, uint32_t sector_num);

/**
  * @brief erase whole chip
  *
  * @param dev  flash device object pointer
  *
  * @return  None
  */
void wm_hal_flash_erase_chip(wm_hal_flash_dev_t *dev);

/**
  * @brief get flash unique id
  *
  * @param dev  flash device object pointer
  * @param uuid store uuid will read out
  *
  * @return  None
  */
void wm_hal_flash_get_unique_id(wm_hal_flash_dev_t *dev, uint32_t uuid[4]);

/**
  * @brief  get flash manufactur id
  *
  * @param dev  flash device object pointer
  *
  * @return
  *    - manufactur id: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
uint8_t wm_hal_flash_get_manufacturer_id(wm_hal_flash_dev_t *dev);

/**
  * @brief  get flash size, uinit is byte
  *
  * @param dev  flash device object pointer
  *
  * @return
  *    - capacity: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int32_t wm_hal_flash_get_capacity(wm_hal_flash_dev_t *dev);

#ifdef __cplusplus
}
#endif

#endif

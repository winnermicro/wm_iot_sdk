/**
 * @file wm_drv_ops_external_flash_spi.h
 *
 * @brief extern flash drv module
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
#ifndef __WM_DRV_OPS_EXTERNAL_FLASH_SPI_H__
#define __WM_DRV_OPS_EXTERNAL_FLASH_SPI_H__

#define FLASH_UNIQUE_ID_SIZE 16
#define WM_FLS_UNSUPPORT_CMD 0xFF

#define SUPPORT_STAND_SPI    (1 << 0)
#define SUPPORT_DUAL_SPI     (1 << 1)
#define SUPPORT_QUAD_SPI     (1 << 2)
#define SUPPORT_QPI          (1 << 3)
#define SUPPORT_DTR          (1 << 4)

typedef struct {
    uint8_t commd_id;
    uint8_t dummy_bit;
} eflash_cmd_t;

typedef struct {
    eflash_cmd_t wren;      //write enable
    eflash_cmd_t pp;        //program page
    eflash_cmd_t rdid;      //read identification
    eflash_cmd_t rdsr1;     //read status register1
    eflash_cmd_t wrsr1;     //write status register1
    eflash_cmd_t prscur;    //program secturity register
    eflash_cmd_t pe;        //page erase
    eflash_cmd_t se;        //sectore erase
    eflash_cmd_t ce;        //chip erase
    eflash_cmd_t read;      //read data
    eflash_cmd_t fread;     //fast read
    eflash_cmd_t two_read;  //dual I/O fast read
    eflash_cmd_t four_read; //quad I/O fast read
    eflash_cmd_t ruid;      //read unique ID
} eflash_cmd_set_t;

typedef struct {
    const char *name;
    uint8_t vendor_id;
    uint8_t support_mode;
    uint8_t addr_width;
    uint32_t flash_size;
    uint32_t page_size;
    uint32_t sector_size;
    uint32_t block_size;
    eflash_cmd_set_t cmd_set;
} wm_eflash_t;

typedef struct {
    uint8_t vendor_id;                            //flash verndor id
    uint32_t size;                                //flash size, unit is byte
    uint32_t page_size;                           //page size, uint is byte
    uint32_t sector_size;                         //sector size, unit is byte
    uint32_t block_size;                          //block size, unit is byte
    uint32_t unique_id[FLASH_UNIQUE_ID_SIZE / 4]; //flash unique id
    wm_os_mutex_t *mutex;
    wm_device_t *spi_device;
    wm_device_t *rcc_device;
    const wm_eflash_t *p_flash;
} wm_drv_eflash_ctx_t;

/**
  * @brief     Initialize external flash
  *
  * @param [in] dev  external flash driver device
  * @param [in] fls  external flash devie pointer
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_spi_flash_init(wm_device_t *dev, const wm_eflash_t *fls);

/**
  * @brief     write data by page
  *
  * @param [in] dev  external flash driver device
  * @param [in] fls  external flash devie pointer
  * @param [in] buf  the data will be write to flash
  * @param [in] write_page_num  write page number
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_spi_flash_program_pages(wm_device_t *dev, const wm_eflash_t *fls, uint32_t flash_addr, uint8_t *buf,
                               uint32_t write_page_num);

/**
  * @brief     read data by page
  *
  * @param [in] dev  external flash driver device
  * @param [in] fls  external flash devie pointer
  * @param [out read_buf  store the data read from flash
  * @param [in] read_page_num  read page number
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_spi_flash_read_pages(wm_device_t *dev, const wm_eflash_t *fls, uint32_t flash_addr, uint8_t *read_buf,
                            uint32_t read_page_num);

/**
  * @brief     read data by sector
  *
  * @param [in] dev  external flash driver device
  * @param [in] fls  external flash devie pointer
  * @param [out read_buf  store the data read from flash
  * @param [in] read_sector_num  read sector number
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_spi_flash_read_sectors(wm_device_t *dev, const wm_eflash_t *fls, uint32_t flash_addr, uint8_t *read_buf,
                              uint32_t read_sector_num);

/**
  * @brief     erase data by sector
  *
  * @param [in] dev  external flash driver device
  * @param [in] fls  external flash devie pointer
  * @param [in flash_addr  the begin flash address will to be erase
  * @param [in] sector_num  erase sector number
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_spi_flash_erase_sectors(wm_device_t *dev, const wm_eflash_t *fls, uint32_t flash_addr, uint32_t sector_num);

/**
  * @brief     erase whole chip
  *
  * @param [in] dev  external flash driver device
  * @param [in] fls  external flash devie pointer
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_spi_flash_erase_chip(wm_device_t *dev, const wm_eflash_t *fls);

/**
  * @brief     get unique ID from external flash
  *
  * @param [in] dev  external flash driver device
  * @param [in] fls  external flash devie pointer
  * @param [out] uuid  store the uuid from flash
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
int wm_spi_flash_get_unique_id(wm_device_t *dev, const wm_eflash_t *fls, uint32_t uuid[4]);

/**
  * @brief     get manufacturer ID from external flash
  *
  * @param [in] dev  external flash driver device
  * @param [in] fls  external flash devie pointer
  * @param [out] manufacturer_id  store manufacturer id from flash
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */
uint8_t wm_spi_flash_get_manufacturer_id(wm_device_t *dev, const wm_eflash_t *fls, uint8_t *manufacturer_id);

/**
  * @brief     get flash size from external flash
  *
  * @param [in] dev  external flash driver device
  * @param [in] fls  external flash devie pointer
  * @param [out] flash_size  store flash size from flash
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM: invalid argument
  *    - others: failed
  */

int wm_spi_flash_get_capacity(wm_device_t *dev, const wm_eflash_t *fls, uint32_t *flash_size);

#endif

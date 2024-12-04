/**
 * @file wm_drv_eeprom.h
 *
 * @brief device driver eeprom header
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

#ifndef __WM_DRV_EEPROM_H__
#define __WM_DRV_EEPROM_H__

#include "wm_types.h"
#include "wm_error.h"
#include "wm_attr.h"
#include "wm_soc_cfgs.h"
#include "wm_dt.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_EEPROM_Structs WM EEPROM Structs
 * @brief WinnerMicro EEPROM Structs
 */

/**
 * @addtogroup WM_EEPROM_Structs
 * @{
 */

/**
 * @brief instance api types
 */
typedef struct {
    int (*init)(wm_device_t *device);   /**< initialize   */
    int (*deinit)(wm_device_t *device); /**< deinitialize */

    int (*read)(wm_device_t *device, uint32_t offset, void *buf, uint32_t read_size);    /**< read                 */
    int (*write)(wm_device_t *device, uint32_t offset, const void *data, uint32_t size); /**< write                */
    int (*get_size)(wm_device_t *device);                                                /**< get eeprom size      */
    int (*set_wp)(wm_device_t *device, bool on);                                         /**< set write protection */
} wm_drv_eeprom_ops_t;

/**
 * @}
 */

/**
 * @defgroup WM_DRV_EEPROM_APIs WM DRV EEPROM APIs
 * @brief WinnerMicro DRV EEPROM APIs
 */

/**
 * @addtogroup WM_DRV_EEPROM_APIs
 * @{
 */

/**
  * @brief  eeprom drvier initialize
  *
  * @param[in] eeprom_dev_name  The eeprom name, defined in device table
  * @return
  *    - !0 : eeprom device, return previous handle if called twice
  *    - NULL: fail
  */
wm_device_t *wm_drv_eeprom_init(const char *eeprom_dev_name);

/**
  * @brief  eeprom drvier deinitialize
  *
  * @param[in] eeprom_device  eeprom device,type of  @ref wm_device_t *
  * @return
  *    - WM_ERR_SUCCESS : deinitialize ok
  *    - WM_ERR_INVALID_PARAM: bad param
  */
int wm_drv_eeprom_deinit(wm_device_t *eeprom_device);

/**
  * @brief  read data from eeprom device
  *
  * @param[in] eeprom_device  eeprom device,type of  @ref wm_device_t *
  * @param[in] offset  read offset
  * @param[out] buf  The read buffer
  * @param[in] len  The buffer size
  * @return
  *    - WM_ERR_SUCCESS : read ok
  *    - WM_ERR_FAILED : read fail
  *    - WM_ERR_INVALID_PARAM: bad param
  */
int wm_drv_eeprom_read(wm_device_t *eeprom_device, uint32_t offset, void *buf, uint32_t len);

/**
  * @brief  write data to eeprom device
  *
  * @param[in] eeprom_device  eeprom device,type of  @ref wm_device_t *
  * @param[in] offset  write offset
  * @param[in] data  The write data
  * @param[in] len  The size of write data
  * @return
  *    - WM_ERR_SUCCESS : read ok
  *    - WM_ERR_FAILED : read fail
  *    - WM_ERR_INVALID_PARAM: bad param
  */
int wm_drv_eeprom_write(wm_device_t *eeprom_device, uint32_t offset, const void *data, uint32_t len);

/**
  * @brief  Get eeprom size, the size is configured in device table
  *
  * @param[in] eeprom_device  eeprom device,type of  @ref wm_device_t *
  * @return
  *    - > 0: eeprom size
  *    - < 0: WM_ERR_INVALID_PARAM
  */
int wm_drv_eeprom_get_size(wm_device_t *eeprom_device);

/**
  * @brief  set write protection
  *
  * @param[in] eeprom_device  eeprom device,type of  @ref wm_device_t *
  * @param[in] on  1: write protection, 0: no write protection
  * @return
  *    - WM_ERR_SUCCESS : deinitialize ok
  *    - WM_ERR_INVALID_PARAM: bad param
  * @note The eeprom WP pin must connect to the gpio pin that configured in device table
  * @note If the write protection function is enabled, the wm_drv_eeprom_write function
  * will automatically turn off write protection before writing, and then turn on write
  * protection after writing.
  */
int wm_drv_eeprom_set_wp(wm_device_t *eeprom_device, bool on);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_DRV_EEPROM_H__ */

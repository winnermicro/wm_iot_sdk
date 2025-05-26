/**
 * @file wm_drv_i2c.h
 *
 * @brief driver i2c master Module
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

#ifndef __WM_DRV_I2C_H__
#define __WM_DRV_I2C_H__

#include "wm_types.h"
#include "wm_error.h"
#include "wm_attr.h"
#include "wm_soc_cfgs.h"
#include "wm_dt.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_I2C_Structs WM I2C Structs
 * @brief WinnerMicro I2C Structs
 */

/**
 * @addtogroup WM_I2C_Structs
 * @{
 */
/**
  * @brief  driver i2c device structure
  * @note   The 7 bits address without read/write bit
  *
  * If the address of the upper 4 bits of eeprom is 1010, the address of the lower 3 bits is 0,
  * and the read/write bits are not required, then the address is 0x50
*/
typedef struct {
    uint16_t addr;           /**< i2c device 7 bits raw address */
    wm_i2c_speed_t speed_hz; /**< scl clock speed, unit hz      */
} wm_drv_i2c_config_t;

/**
 * @brief Local api types
 */
typedef struct {
    int (*init)(wm_device_t *device);
    int (*deinit)(wm_device_t *device);

    int (*read)(wm_device_t *device, wm_drv_i2c_config_t *config, const void *sub_addr, uint32_t sub_addr_size, void *read_buf,
                uint32_t read_size);
    int (*write)(wm_device_t *device, wm_drv_i2c_config_t *config, const void *sub_addr, uint32_t sub_addr_size,
                 const void *data, uint32_t data_size);
} wm_drv_i2c_ops_t;

/**
 * @}
 */

/**
 * @defgroup WM_DRV_I2C_APIs WM DRV I2C TYPEs
 * @brief WinnerMicro DRV I2C TYPEs
 */

/**
 * @addtogroup WM_DRV_I2C_APIs
 * @{
 */

/**
  * @brief  i2c drvier initialize
  *
  * @param[in] device_name  The i2c controler name, defined in device table
  * @return
  *    - !0 : i2c controler handle, return previous handle if called twice
  *    - NULL: fail
  */
wm_device_t *wm_drv_i2c_init(const char *device_name);

/**
  * @brief  i2c drvier deinitialize
  *
  * @param[in] device  The i2c controler device, @arg wm_device_t
  * @return
  *    - WM_ERR_SUCCESS : deinitialize ok
  *    - WM_ERR_INVALID_PARAM: bad param
  */
int wm_drv_i2c_deinit(wm_device_t *device);

/**
  * @brief  i2c drvier write read api
  *
  * @param[in] device  The i2c controler device, @arg wm_device_t
  * @param[in] config  The i2c target device config, @arg wm_drv_i2c_config_t
  * @param[in] sub_addr  The device sub address, such as eeprom offset
  * @param[in] sub_addr_size  The device sub address size
  * @param[out] read_buf  The read buffer
  * @param[in] read_size  The data size of read buffer
  * @return
  *    - WM_ERR_SUCCESS : read ok
  *    - WM_ERR_FAILED : read fail
  *    - WM_ERR_INVALID_PARAM: bad param
  * @note The second and third param, sometime used to write the device sub address or command,
  * If the sub address is a variable with more than 1 byte, you need to pay attention to the big-endian conversion
  */
int wm_drv_i2c_read(wm_device_t *device, wm_drv_i2c_config_t *config, const void *sub_addr, uint32_t sub_addr_size,
                    void *read_buf, uint32_t read_size);

/**
  * @brief  i2c drvier write api
  *
  * @param[in] device  The i2c controler device, @arg wm_device_t
  * @param[in] config  The i2c target device config, @arg wm_drv_i2c_config_t
  * @param[in] sub_addr  The device sub address, such as eeprom offset
  * @param[in] sub_addr_size  The device sub address size
  * @param[in] data  The write data
  * @param[in] data_size  The size of write data
  * @return
  *    - WM_ERR_SUCCESS : write ok
  *    - WM_ERR_FAILED : write fail
  *    - WM_ERR_INVALID_PARAM: bad param
  * @note The second and third param, sometime used to write the device sub address or command,
  * If the sub address is a variable with more than 1 byte, you need to pay attention to the big-endian conversion
  * @note The fouth param data set to NULL and the fifth param data_size set to 0 if only one address need send
  */
int wm_drv_i2c_write(wm_device_t *device, wm_drv_i2c_config_t *config, const void *sub_addr, uint32_t sub_addr_size,
                     const void *data, uint32_t data_size);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_DRV_I2C_H__ */

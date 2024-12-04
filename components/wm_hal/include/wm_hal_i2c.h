
/**
 * @file wm_hal_i2c.h
 *
 * @brief hal i2c master Module
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

#ifndef __WM_HAL_I2C_H__
#define __WM_HAL_I2C_H__

#include "wm_types.h"
#include "wm_soc_cfgs.h"
#include "wm_ll_i2c.h"
#include "wm_irq.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Hal i2c write/read flag, not stop i2c controler after write/read
 */
#define WM_HAL_I2C_FLAG_NOSTOP  (1 << 0)

/**
 * @brief Hal i2c write/read flag, not start i2c controler before write/read
 */
#define WM_HAL_I2C_FLAG_NOSTART (1 << 1)

/**
 * @brief Hal i2c device type
 */
typedef struct {
    wm_i2c_reg_t *reg;    /**< register base address     */
    wm_irq_no_t irq_num;  /**< irq num                   */
    uint8_t irq_priority; /**< irq priority              */
    void *ctx;            /**< internal context          */
} wm_hal_i2c_dev_t;

/**
 * @brief Hal i2c config type
 */
typedef struct {
    uint8_t addr_bits;    /**< address bits         */
    wm_i2c_speed_t speed; /**< speed , unit  Hz     */
    uint32_t ref_clock;   /**< reference clock,default APB ,unit Hz   */
} wm_hal_i2c_config_t;

/**
  * @brief  hal i2c initialize
  *
  * @param[in] dev  The i2c device struct, @ref wm_hal_i2c_dev_t
  * @param[in] cfg  The init config, @ref wm_hal_i2c_config_t
  * @return
  *    - WM_ERR_SUCCESS : initialize OK
  *    - WM_ERR_FAILED : initialize FAIL
  *    - WM_ERR_INVALID_PARAM: bad param
  */
int wm_hal_i2c_init(wm_hal_i2c_dev_t *dev, wm_hal_i2c_config_t *cfg);

/**
  * @brief  hal i2c deinitialize
  *
  * @param[in] dev  The i2c device struct, @ref wm_hal_i2c_dev_t
  * @return
  *    - WM_ERR_SUCCESS : deinitialize OK
  *    - WM_ERR_INVALID_PARAM: bad param
  */
int wm_hal_i2c_deinit(wm_hal_i2c_dev_t *dev);

/**
  * @brief  hal i2c set speed
  *
  * @param[in] dev  The i2c device struct, @ref wm_hal_i2c_dev_t
  * @param[in] speed_hz  i2c controler speed, @ref wm_i2c_speed_t
  * @return
  *    - WM_ERR_SUCCESS : initialize OK
  *    - WM_ERR_INVALID_PARAM: bad param
  */
int wm_hal_i2c_set_speed(wm_hal_i2c_dev_t *dev, wm_i2c_speed_t speed_hz);

/**
  * @brief  hal i2c get speed
  *
  * @param[in] dev  The i2c device struct, @ref wm_hal_i2c_dev_t
  * @param[in] speed_hz  i2c controler speed, @ref wm_i2c_speed_t
  * @return
  *    - WM_ERR_SUCCESS : initialize OK
  *    - WM_ERR_INVALID_PARAM: bad param
  */
int wm_hal_i2c_get_speed(wm_hal_i2c_dev_t *dev, wm_i2c_speed_t *speed_hz);

/**
  * @brief  hal i2c recover controler
  *
  * @param[in] dev  The i2c device struct, @ref wm_hal_i2c_dev_t
  * @return
  *    - WM_ERR_SUCCESS : initialize OK
  *    - WM_ERR_INVALID_PARAM: bad param
  */
int wm_hal_i2c_recover(wm_hal_i2c_dev_t *dev);

/**
  * @brief  i2c drvier write data with polling mode
  *
  * @param[in] dev  The i2c device struct, @ref wm_hal_i2c_dev_t
  * @param[in] dev_addr  The the i2c target device address, 7 bits address without W/R bit
  * @param[in] data  The data to be write
  * @param[in] size  The write data size
  * @param[in] flag  write flag, mask as WM_HAL_I2C_FLAG_NOSTART and WM_HAL_I2C_FLAG_NOSTOP
  * @param[in] timeout time out time
  * @return
  *    - WM_ERR_SUCCESS : read ok
  *    - WM_ERR_FAILED : read fail
  *    - WM_ERR_INVALID_PARAM: bad param
  */
int wm_hal_i2c_tx_polling(wm_hal_i2c_dev_t *dev, uint16_t dev_addr, const uint8_t *data, uint32_t size, uint32_t flag,
                          uint32_t timeout);

/**
  * @brief  i2c drvier read data with polling mode
  *
  * @param[in] dev  The i2c device struct, @ref wm_hal_i2c_dev_t
  * @param[in] dev_addr  The the i2c target device address, 7 bits address without W/R bit
  * @param[in] buffer  The buffer to be read in
  * @param[in] size  The read buffer size
  * @param[in] flag  read flag, mask as WM_HAL_I2C_FLAG_NOSTART and WM_HAL_I2C_FLAG_NOSTOP
  * @param[in] timeout time out time
  * @return
  *    - WM_ERR_SUCCESS : read ok
  *    - WM_ERR_FAILED : read fail
  *    - WM_ERR_INVALID_PARAM: bad param
  */
int wm_hal_i2c_rx_polling(wm_hal_i2c_dev_t *dev, uint16_t dev_addr, uint8_t *buffer, uint32_t size, uint32_t flag,
                          uint32_t timeout);

#ifdef __cplusplus
}
#endif

#endif

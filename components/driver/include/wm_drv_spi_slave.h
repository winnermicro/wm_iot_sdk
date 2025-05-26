/**
 * @file wm_drv_spi_slave.h
 *
 * @brief spi slave drv api
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

#ifndef __WM_DRV_SPI_SLAVE_H__
#define __WM_DRV_SPI_SLAVE_H__

#include "wm_types.h"
#include "wm_dt.h"
#include "wm_dt_hw.h"
#include "wm_hal_spi_master.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_SPIS_MACROs WM SPI SLAVE Macros
 * @brief WinnerMicro SPI SLAVE Macros
 */

/**
 * @addtogroup WM_SPIS_MACROs
 * @{
 */

//command bit in struct spis_transceive_t flag
typedef enum {
    SPIS_TRANS_DUMMY_BITS     = (1 << 0), /**< dummy bit valid bit*/
    SPIS_TRANS_CS_KEEP_ACTIVE = (1 << 1), /**<if set, CS will keep active after transceive done */
    SPIS_TRANS_BIG_ENDIAN     = (1 << 2), /**< if set, transmit high address byte first in a word*/
    SPIS_TRANS_VARIABLE_CMD   = (1 << 3), /**< if set, cmd and cmd_len will valid in spis_transceive_ex_t*/
    SPIS_TRANS_VARIABLE_ADDR  = (1 << 4), /**< if set, addr and addr_len will valid in spis_transceive_ex_t */
} WM_SPIS_TRANS_FLAG;

/**
 * @}
 */

/**
 * @defgroup WM_SPIS_Structs WM SLAVE Structs
 * @brief WinnerMicro SPI SLAVE Structs
 */

/**
 * @addtogroup WM_SPIS_Structs
 * @{
 */

typedef struct {
    uint8_t *tx_buf; /**< [input]pointer to transmit buffer, or NULL no data to be transmit, if use DMA must 4 byte algin */
    uint8_t *rx_buf; /**< [output]pointer to transmit buffer, or NULL no data receive, if use DMA must 4 byte algin */
    uint32_t tx_len; /**< [input]total transmit length */
    uint32_t rx_len; /**< [intput]total receive length */
    uint32_t flags;  /**< [input]transceive flag ref: WM_SPIS_TRANS_FLAG*/
} spis_transceive_t;

//***Only support half duplex mode
typedef struct {
    spis_transceive_t basic; /**< it's must be as the first member */
    uint16_t cmd;            /**< [input] tx command data, if you want to use this member must set "cmd_len" at the same time,
                                  if transmit low byte first, must set SPI_TRANS_CMD_SMALL_ENDIAN */
    uint32_t addr;           /**< [input] tx address data, if you want to use this member must set "addr_len" at the same time,
                                  if transmit low byte first, must set SPI_TRANS_ADDR_SMALL_ENDIAN */
    uint8_t cmd_len;         /**< [input] command length, unit is byte [0~2]*/
    uint8_t addr_len;        /**< [input] address length, unit is byte [0~4] */
    uint8_t dummy_bits; /**< [input] dummy length, unit is bit, must set SPI_TRANS_DUMMY_BITS flag if want to use this member */
} spis_transceive_ex_t;

/**
 * @}
 */

/**
 * @defgroup WM_DRV_SPIS_APIs WM DRV SPI SLAVE APIs
 * @brief WinnerMicro DRV Spi slave APIs
 */

/**
 * @addtogroup WM_DRV_SPIS_APIs
 * @{
 */

// asynchronous transceive done callback
typedef void (*wm_spis_callback_t)(int result, void *data);

/**
  * @brief transceive data synchronously, the api return until synchronously done or timeout
  *
  * @param[in] dev  SPI device pointer
  * @param[in] config Pointer to a valid wm_hw_spi_device_config_t structure instance.
  * @param[in] desc transceive descrption about tx and rx
  * @param[in] ms_to_wait the max wait time for transeceive done, unit is millisecon
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_drv_spis_transceive_sync(wm_device_t *dev, const wm_dt_hw_spis_dev_cfg_t *config, spis_transceive_t *desc,
                                uint32_t ms_to_wait);

/**
  * @brief transceive data asynchronously, callback will be trigger after transceive done
  *
  * @param[in] dev  SPI device pointer
  * @param[in] config Pointer to a valid wm_hw_spi_device_config_t structure instance.
  * @param[in] desc transceive descrption about tx and rx
  * @param[in] callback the function will be trigger after transceive done
  * @param[in] usr_data the argument for callback function
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_drv_spis_transceive_async(wm_device_t *dev, const wm_dt_hw_spis_dev_cfg_t *config, spis_transceive_t *desc,
                                 wm_spis_callback_t callback, void *usr_data);

/**
  * @brief     Initialize SPI slave driver
  *
  * @param[in] dev_name  spi slave device name
  *
  * @return
  *    - spi slave device pointer: succeed
  *    - NULL : failed
  */
wm_device_t *wm_drv_spis_init(const char *dev_name);

/**
  * @brief     deinit SPI slave driver
  *
  * @param[in] dev SPI slave device pointer
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_drv_spis_deinit(wm_device_t *dev);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_DRV_SPIS_H__ */

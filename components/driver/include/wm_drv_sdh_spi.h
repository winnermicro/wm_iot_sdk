/**
 * @file wm_drv_sdh_spi.h
 *
 * @brief DRV_SDH_SPI Module
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

#ifndef __WM_DRV_SDH_SPI_H__
#define __WM_DRV_SDH_SPI_H__

#include "wm_dt.h"
#include "wm_hal_sdh.h"
#include "wm_drv_spi_master.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_DRV_SDH_SPI_Functions WM DRV_SDH_SPI Functions
 * @brief WinnerMicro DRV_SDH_SPI Functions
 */

/**
 * @addtogroup WM_DRV_SDH_SPI_Functions
 * @{
 */

/**
 * @brief Initializes the SDH_SPI device.
 *
 * This function initializes the SDH_SPI device using the specified device name.
 * It allocates resources, sets up necessary configurations, and prepares the device for further operations.
 *
 * @param[in] dev_name The name of the device to initialize, as defined in the device tree.
 *
 * @return
 *    - A pointer to the device structure if the initialization succeeds.
 *    - NULL if the initialization fails or the device name is not recognized, or the device has already been initialized.
 */
wm_device_t *wm_drv_sdh_spi_init(char *dev_name);

/**
 * @brief Deinitializes the SDH_SPI device.
 *
 * This function is used to release the resources and stop any ongoing operations of the SDH_SPI device.
 * It should be called when the device is no longer needed to ensure clean resource management.
 *
 * @param[in] dev Pointer to the device structure that needs to be deinitialized.
 *
 * @return
 *    - WM_ERR_SUCCESS if the deinitialization succeeds.
 *    - WM_ERR_INVALID_PARAM if the provided device pointer is NULL or invalid.
 *    - Other error codes may be returned based on the specific failure.
 */
int wm_drv_sdh_spi_deinit(wm_device_t *dev);

/**
 * @brief Synchronously transmits and receives data through the SDH_SPI device.
 *
 * This function performs a synchronous data transfer using the SDH_SPI device.
 * It blocks until the operation is complete or the specified timeout is reached.
 *
 * @param[in] dev Pointer to the device structure of the SDH_SPI.
 * @param[in] config Pointer to a structure containing the configuration parameters for the SDH_SPI device.
 * @param[in] desc Pointer to a structure containing the transmission and reception parameters.
 * @param[in] ms_to_wait Maximum time in milliseconds to wait for the operation to complete.
 *
 * @return
 *    - WM_ERR_SUCCESS if the operation completes successfully.
 *    - WM_ERR_INVALID_PARAM if the device pointer or description pointer is NULL, or if the parameters are out of valid range.
 *    - Other error codes may be returned to reflect different failure conditions.
 *
 * @note
 *    - The freq in the config parameter supports 1/2, 1/4, 1/6, 1/8, 1/10, 1/12, 1/14, 1/16 of the CPU clock, it is recommended not to exceed 60MHz,
 *      and the CPU clock corresponding to WM_RCC_TYPE_CPU can be obtained through the RCC module.
 *    - The mode in the config parameter only supports 0 (CPOL=0,CPHA=0).
 *    - The spim_transceive_t structure's member parameters tx_buf and tx_len can be used,
 *      while other member parameters are not available and are only included to maintain the same API parameters as the SPI master.
 *    - The maximum data length is 65536 bytes.
 *    - The SDH_SPI is a half-duplex communication interface and cannot perform read and write operations simultaneously.
 *    - It is primarily used in scenarios that involve only write operations, such as driving TFT displays.
 */
int wm_drv_sdh_spi_transceive_sync(wm_device_t *dev, const wm_dt_hw_spim_dev_cfg_t *config, spim_transceive_t *desc,
                                   uint32_t ms_to_wait);

/**
 * @brief Asynchronously transmits and receives data through the SDH_SPI device.
 *
 * This function initiates an asynchronous data transfer using the SDH_SPI device.
 * It returns immediately, and the specified callback function is called when the operation is complete or an error occurs.
 *
 * @param[in] dev Pointer to the device structure of the SDH_SPI.
 * @param[in] config Pointer to a structure containing the configuration parameters for the SDH_SPI device.
 * @param[in] desc Pointer to a structure containing the transmission and reception parameters.
 * @param[in] callback Function to be called when the operation is complete, cannot perform blocking operations within callback.
 * @param[in] usr_data User-defined data to pass to the callback function.
 *
 * @return
 *    - WM_ERR_SUCCESS if the operation is successfully initiated.
 *    - WM_ERR_INVALID_PARAM if the device pointer, description pointer, or callback function is NULL.
 *    - Other error codes may be returned to reflect different failure conditions.
 *
 * @note
 *    - The freq in the config parameter supports 1/2, 1/4, 1/6, 1/8, 1/10, 1/12, 1/14, 1/16 of the CPU clock, it is recommended not to exceed 60MHz,
 *      and the CPU clock corresponding to WM_RCC_TYPE_CPU can be obtained through the RCC module.
 *    - The mode in the config parameter only supports 0 (CPOL=0,CPHA=0)
 *    - The maximum data length is 65532 bytes.
 *    - The spim_transceive_t structure's member parameters tx_buf and tx_len can be used,
 *      while other member parameters are not available and are only included to maintain the same API parameters as the SPI master.
 *    - The SDH_SPI is a half-duplex communication interface and cannot perform read and write operations simultaneously.
 *    - It is primarily used in scenarios that involve only write operations, such as driving TFT displays.
 *    - The data length must be aligned with 4 bytes.
 */
int wm_drv_sdh_spi_transceive_async(wm_device_t *dev, const wm_dt_hw_spim_dev_cfg_t *config, spim_transceive_t *desc,
                                    wm_spim_callback_t callback, void *usr_data);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_DRV_SDH_SPI_H__ */

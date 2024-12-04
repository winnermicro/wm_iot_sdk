/**
 * @file wm_drv_sdio_slave.h
 *
 * @brief SDIO Slave Driver Module
 *
 * This module provides APIs for SDIO slave device initialization, data transfer and command handling.
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

#ifndef __WM_DRV_SDIO_SLAVE_H__
#define __WM_DRV_SDIO_SLAVE_H__

#include "wm_dt.h"
#include "wm_hal_sdio_slave.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_DRV_SDIO_SLAVE_Macros WM DRV SDIO Slave Macros
 * @brief WinnerMicro SDIO Slave Driver Macros
 */

/**
 * @addtogroup WM_DRV_SDIO_SLAVE_Macros
 * @{
 */

#define WM_DRV_SDIO_SLAVE_CIS_SIZE        (128)  /**< Size of CIS buffer */
#define WM_DRV_SDIO_SLAVE_CMD_RX_BUF_SIZE (256)  /**< Size of command receive buffer */
#define WM_DRV_SDIO_SLAVE_TX_BUF_SIZE     (1024) /**< Size of transmit buffer */
#define WM_DRV_SDIO_SLAVE_TX_BD_NUM       (8)    /**< Number of transmit buffer descriptors */
#define WM_DRV_SDIO_SLAVE_RX_BUF_SIZE     (1024) /**< Size of receive buffer */
#define WM_DRV_SDIO_SLAVE_RX_BD_NUM       (4)    /**< Number of receive buffer descriptors */

/**
 * @}
 */

/**
 * @defgroup WM_DRV_SDIO_SLAVE_Enumerations WM DRV SDIO Slave Enumerations
 * @brief WinnerMicro SDIO Slave Driver Enumerations
 */

/**
 * @addtogroup WM_DRV_SDIO_SLAVE_Enumerations
 * @{
 */

/**
 * @brief SDIO slave function enumeration
 *
 * This enumeration defines the available SDIO slave functions that can be accessed.
 * SDIO supports multiple functions per card, with function 0 being a special function
 * that is used for card configuration and control.
 *
 * Function 0:
 * - Used for card configuration and control
 * - Always exists and is mandatory
 * - Can only perform control operations, not data transfers
 * - Accesses common card registers (CCR)
 *
 * Function 1:
 * - Used for actual data transfers
 * - Optional function that provides data transfer capability
 * - Can perform both control and data operations
 * - Has its own function-specific registers (FBR)
 */
typedef enum {
    WM_DRV_SDIO_SLAVE_FUNC_0 = 0, /**< SDIO slave function 0 - Configuration and control function */
    WM_DRV_SDIO_SLAVE_FUNC_1 = 1, /**< SDIO slave function 1 - Data transfer function */
    WM_DRV_SDIO_SLAVE_FUNC_MAX    /**< Maximum number of supported SDIO functions */
} wm_drv_sdio_slave_func_t;

/**
 * @brief SDIO slave event types
 */
typedef enum {
    WM_DRV_SDIO_SLAVE_EVENT_TX     = 0, /**< Data transmit complete event */
    WM_DRV_SDIO_SLAVE_EVENT_RX     = 1, /**< Data receive complete event */
    WM_DRV_SDIO_SLAVE_EVENT_TX_CMD = 2, /**< Command transmit complete event */
    WM_DRV_SDIO_SLAVE_EVENT_RX_CMD = 3, /**< Command receive complete event */
    WM_DRV_SDIO_SLAVE_EVENT_MAX
} wm_drv_sdio_slave_event_t;

/**
 * @}
 */

/**
 * @defgroup WM_DRV_SDIO_SLAVE_Type_Definitions WM DRV SDIO Slave Type Definitions
 * @brief WinnerMicro SDIO Slave Driver Type Definitions
 */

/**
 * @addtogroup WM_DRV_SDIO_SLAVE_Type_Definitions
 * @{
 */

/**
 * @brief SDIO slave event callback function type
 *
 * This callback function is invoked when SDIO slave events occur. The events include:
 * - Data transmission complete (TX)
 * - Data reception complete (RX)
 * - Command transmission complete (TX CMD)
 * - Command reception complete (RX CMD)
 *
 * @param dev   Device handle for the SDIO slave device
 * @param event Type of event that triggered the callback (TX/RX/TX_CMD/RX_CMD)
 * @param addr  Buffer address containing the transmitted/received data or command
 * @param priv  Private data pointer that was passed during callback registration
 */
typedef void (*wm_drv_sdio_slave_callback_t)(wm_device_t *dev, wm_drv_sdio_slave_event_t event, void *addr, void *priv);

/**
 * @}
 */

/**
 * @defgroup WM_DRV_SDIO_SLAVE_Functions WM DRV SDIO Slave Functions
 * @brief WinnerMicro SDIO Slave Driver Functions
 */

/**
 * @addtogroup WM_DRV_SDIO_SLAVE_Functions
 * @{
 */

/**
 * @brief Initializes the SDIO slave device.
 *
 * This function initializes the SDIO slave device using the specified device name.
 * It allocates resources, configures GPIO pins, initializes buffer descriptors,
 * sets up CIS, registers interrupt callback and creates synchronization semaphores.
 *
 * @param[in] dev_name The name of the device to initialize, as defined in the device tree.
 *
 * @return
 *    - A pointer to the device structure if the initialization succeeds.
 *    - NULL if the initialization fails or the device name is not recognized.
 */
wm_device_t *wm_drv_sdio_slave_init(char *dev_name);

/**
 * @brief Deinitializes the SDIO slave device.
 *
 * This function is used to release the resources and stop any ongoing operations of the SDIO slave device.
 * It frees buffer descriptors, deletes semaphores, unregisters callbacks, resets GPIO pins and frees driver data structures.
 *
 * @param[in] dev Pointer to the device structure that needs to be deinitialized.
 *
 * @return
 *    - WM_ERR_SUCCESS if the deinitialization succeeds.
 *    - WM_ERR_INVALID_PARAM if the provided device pointer is NULL or invalid.
 *    - Other error codes may be returned based on the specific failure.
 */
int wm_drv_sdio_slave_deinit(wm_device_t *dev);

/**
 * @brief Registers event callback function for the SDIO slave device.
 *
 * This function registers a callback function that will be invoked when SDIO slave events occur.
 * The callback runs in interrupt context and should be kept as short as possible.
 * Events include data/command transmission and reception completion.
 *
 * @param[in] dev Pointer to the device structure of the SDIO slave.
 * @param[in] cb Callback function to be invoked when events occur.
 * @param[in] priv User-defined data to pass to the callback function.
 *
 * @return
 *    - WM_ERR_SUCCESS if the registration succeeds.
 *    - WM_ERR_INVALID_PARAM if the device pointer or callback is NULL.
 *    - Other error codes may be returned to reflect different failure conditions.
 */
int wm_drv_sdio_slave_register_callback(wm_device_t *dev, wm_drv_sdio_slave_callback_t cb, void *priv);

/**
 * @brief Unregisters the event callback function for the SDIO slave device.
 *
 * This function removes the previously registered callback function for SDIO slave events.
 * After unregistering, no callbacks will be invoked when events occur.
 *
 * @param[in] dev Pointer to the device structure of the SDIO slave.
 *
 * @return
 *    - WM_ERR_SUCCESS if the unregistration succeeds.
 *    - WM_ERR_INVALID_PARAM if the device pointer is NULL.
 *    - Other error codes may be returned to reflect different failure conditions.
 */
int wm_drv_sdio_slave_unregister_callback(wm_device_t *dev);

/**
 * @brief Reads a register value from the SDIO slave device.
 *
 * This function reads the value of a register at the specified position for the given SDIO function.
 * It can access both Function 0 (common registers) and Function 1 (data transfer) registers.
 *
 * @param[in] dev Pointer to the device structure of the SDIO slave.
 * @param[in] func SDIO function number (0 or 1) to read from.
 * @param[in] pos Register position/offset to read (0-123).
 * @param[out] reg_val Pointer to store the read register value.
 *
 * @return
 *    - WM_ERR_SUCCESS if the read operation succeeds.
 *    - WM_ERR_INVALID_PARAM if any parameter is invalid.
 *    - Other error codes may be returned to reflect different failure conditions.
 *
 * @note For Function 0/1, pos must not exceed the CIS size (128 bytes).
 */
int wm_drv_sdio_slave_read_cis_reg(wm_device_t *dev, wm_drv_sdio_slave_func_t func, uint32_t pos, uint32_t *reg_val);

/**
 * @brief Writes a value to a register in the SDIO slave device.
 *
 * This function writes the specified value to a register at the given position for the selected SDIO function.
 * It can access both Function 0 (common registers) and Function 1 (data transfer) registers.
 *
 * @param[in] dev Pointer to the device structure of the SDIO slave.
 * @param[in] func SDIO function number (0 or 1) to write to.
 * @param[in] pos Register position/offset to write (0-123).
 * @param[in] reg_val Value to write to the register.
 *
 * @return
 *    - WM_ERR_SUCCESS if the write operation succeeds.
 *    - WM_ERR_INVALID_PARAM if any parameter is invalid.
 *    - Other error codes may be returned to reflect different failure conditions.
 *
 * @note For Function 0/1, pos must not exceed the CIS size (128 bytes).
 */
int wm_drv_sdio_slave_write_cis_reg(wm_device_t *dev, wm_drv_sdio_slave_func_t func, uint32_t pos, uint32_t reg_val);

/**
 * @brief Synchronously transmits data through the SDIO slave device.
 *
 * This function performs a synchronous data transfer to the host using the SDIO slave device.
 * It initiates DMA transfer and blocks until the operation is complete or the specified timeout is reached.
 *
 * @param[in] dev Pointer to the device structure of the SDIO slave.
 * @param[in] addr Address of buffer containing data to transmit.
 * @param[in] len Length of data to transmit in bytes.
 * @param[in] timeout Maximum time in milliseconds to wait for the operation to complete.
 *
 * @return
 *    - WM_ERR_SUCCESS if the transmission completes successfully.
 *    - WM_ERR_INVALID_PARAM if any parameter is invalid.
 *    - WM_ERR_TIMEOUT if the operation times out.
 *    - Other error codes may be returned to reflect different failure conditions.
 *
 * @note
 *    - The addr parameter must be allocated from DRAM region using wm_heap_caps_alloc() with WM_HEAP_CAP_SHARED flags
 *    - The len parameter must not exceed WM_DRV_SDIO_SLAVE_TX_BUF_SIZE * WM_DRV_SDIO_SLAVE_TX_BD_NUM (1024 * 8 = 8192 bytes).
 */
int wm_drv_sdio_slave_tx_data_sync(wm_device_t *dev, void *addr, uint32_t len, uint32_t timeout);

/**
 * @brief Asynchronously transmits data through the SDIO slave device.
 *
 * This function initiates an asynchronous data transfer to the host using the SDIO slave device.
 * It returns immediately and the registered callback function is called when the operation completes.
 *
 * @param[in] dev Pointer to the device structure of the SDIO slave.
 * @param[in] addr Address of buffer containing data to transmit.
 * @param[in] len Length of data to transmit in bytes.
 *
 * @return
 *    - WM_ERR_SUCCESS if the transmission is successfully initiated.
 *    - WM_ERR_INVALID_PARAM if any parameter is invalid.
 *    - Other error codes may be returned to reflect different failure conditions.
 *
 * @note
 *    - The addr parameter must be allocated from DRAM region using wm_heap_caps_alloc() with WM_HEAP_CAP_SHARED flags
 *    - The len parameter must not exceed WM_DRV_SDIO_SLAVE_TX_BUF_SIZE * WM_DRV_SDIO_SLAVE_TX_BD_NUM (1024 * 8 = 8192 bytes).
 */
int wm_drv_sdio_slave_tx_data_async(wm_device_t *dev, void *addr, uint32_t len);

/**
 * @brief Receives data from the host through the SDIO slave device.
 *
 * This function waits for data reception from the host with a specified timeout.
 * When data is received, it updates the address pointer and invokes the registered callback.
 *
 * @param[in] dev Pointer to the device structure of the SDIO slave.
 * @param[out] addr Pointer to store the address of received data buffer.
 * @param[in] timeout Maximum time in milliseconds to wait for reception.
 *
 * @return
 *    - WM_ERR_SUCCESS if data is received successfully.
 *    - WM_ERR_INVALID_PARAM if any parameter is invalid.
 *    - WM_ERR_TIMEOUT if reception times out.
 *    - Other error codes may be returned to reflect different failure conditions.
 *
 * @note
 *    - The maximum receivable data length must not exceed WM_DRV_SDIO_SLAVE_RX_BUF_SIZE * WM_DRV_SDIO_SLAVE_RX_BD_NUM (1024 * 4 = 4096 bytes).
 */
int wm_drv_sdio_slave_rx_data(wm_device_t *dev, void **addr, uint32_t timeout);

/**
 * @brief Receives command from the host through the SDIO slave device.
 *
 * This function waits for command reception from the host with a specified timeout.
 * When a command is received, it updates the address pointer to the received command buffer.
 *
 * @param[in] dev Pointer to the device structure of the SDIO slave.
 * @param[out] addr Pointer to store the address of received command buffer.
 * @param[in] timeout Maximum time in milliseconds to wait for reception.
 *
 * @return
 *    - WM_ERR_SUCCESS if command is received successfully.
 *    - WM_ERR_INVALID_PARAM if any parameter is invalid.
 *    - WM_ERR_TIMEOUT if reception times out.
 *    - Other error codes may be returned to reflect different failure conditions.
 *
 * @note
 *    - The maximum receivable command length must not exceed WM_DRV_SDIO_SLAVE_CMD_RX_BUF_SIZE (256 bytes).
 */
int wm_drv_sdio_slave_rx_cmd(wm_device_t *dev, void **addr, uint32_t timeout);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_DRV_SDIO_SLAVE_H__ */

/**
 * @file wm_hal_hspi_slave.h
 *
 * @brief HSPI Slave Hardware Abstraction Layer Module
 *
 * This module provides hardware abstraction layer APIs for HSPI slave device configuration,
 * buffer management, data transfer and interrupt handling.
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

#ifndef __WM_HAL_HSPI_SLAVE_H__
#define __WM_HAL_HSPI_SLAVE_H__

#include "wm_irq.h"
#include "wm_ll_hspi_slave.h"
#include "wm_hal_sdio_wrapper.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_HAL_HSPI_SLAVE_Macros WM HAL HSPI SLAVE Macros
 * @brief WinnerMicro HSPI Slave HAL Macros
 */

/**
 * @addtogroup WM_HAL_HSPI_SLAVE_Macros
 * @{
 */

#define WM_HAL_HSPI_SLAVE_CMD_BUF_SIZE_MAX (256)  /**< Maximum command buffer size */
#define WM_HAL_HSPI_SLAVE_DAT_BUF_SIZE_MAX (1500) /**< Maximum command buffer size */
#define WM_HAL_HSPI_SLAVE_TIMEOUT          (100)  /**< Timeout in milliseconds */

/**
 * @}
 */

/**
 * @defgroup WM_HAL_HSPI_SLAVE_Enumerations WM HAL HSPI SLAVE Enumerations
 * @brief WinnerMicro HSPI Slave HAL Enumerations
 */

/**
 * @addtogroup WM_HAL_HSPI_SLAVE_Enumerations
 * @{
 */

/**
 * @brief HSPI slave event types enumeration
 */
typedef enum {
    WM_HAL_HSPI_SLAVE_EVENT_TX     = 0, /**< Data transmit complete event */
    WM_HAL_HSPI_SLAVE_EVENT_RX     = 1, /**< Data receive complete event */
    WM_HAL_HSPI_SLAVE_EVENT_TX_CMD = 2, /**< Command transmit complete event */
    WM_HAL_HSPI_SLAVE_EVENT_RX_CMD = 3, /**< Command receive complete event */
    WM_HAL_HSPI_SLAVE_EVENT_MAX
} wm_hal_hspi_slave_event_t;

/**
 * @}
 */

/**
 * @defgroup WM_HAL_HSPI_SLAVE_Structures WM HAL HSPI SLAVE Structures
 * @brief WinnerMicro HSPI Slave HAL Structures
 */

/**
 * @addtogroup WM_HAL_HSPI_SLAVE_Structures
 * @{
 */

/**
 * @brief HSPI slave buffer descriptor structure
 */
typedef struct {
    void *cmd_rxbuf_addr;             /**< Command receive buffer address */
    uint32_t cmd_rxbuf_size;          /**< Command receive buffer size */
    wm_hal_sdio_wrapper_txbd_t *txbd; /**< Transmit buffer descriptor */
    wm_hal_sdio_wrapper_rxbd_t *rxbd; /**< Receive buffer descriptor */
    uint32_t tx_buf_len;              /**< Transmit buffer length */
    uint32_t rx_buf_len;              /**< Receive buffer length */
    uint16_t tx_link_num;             /**< Number of transmit buffer descriptors */
    uint16_t rx_link_num;             /**< Number of receive buffer descriptors */
} wm_hal_hspi_slave_buf_desc_t;

/**
 * @brief HSPI slave configuration structure
 */
typedef struct {
    uint8_t cpol; /**< cpol 0: low  1: high */
    uint8_t cpha; /**< cpha 0: first edge 2: second edge */
    uint8_t
        tx_pin_drive; /**< SPI Output Drive Configuration 0: SPI output only active when selected; 1: SPI output always active */
    uint8_t byte_order; /**< byte_order Configuration 0: low byte first; 1: high byte first */
    uint8_t burst_len;  /**< Burst Length Configuration 0: Burst length 1 word; 1: Burst length 4 words */
} wm_hal_hspi_slave_cfg_t;

/**
 * @brief HSPI slave event callback function type
 */
typedef void (*wm_hal_hspi_slave_callback_t)(void *hal_dev, wm_hal_hspi_slave_event_t event, void *addr, void *priv);

/**
 * @brief HSPI slave callback information structure
 */
typedef struct {
    wm_hal_hspi_slave_callback_t cb; /**< Event callback function */
    void *priv;                      /**< Private data for callback */
} wm_hal_hspi_slave_cb_info_t;

/**
 * @brief HSPI slave context structure
 */
typedef struct {
    wm_hal_hspi_slave_cb_info_t cb_info;   /**< Callback information */
    wm_hal_sdio_wrapper_txbd_t *curr_txbd; /**< Current transmit buffer descriptor */
    wm_hal_sdio_wrapper_rxbd_t *curr_rxbd; /**< Current receive buffer descriptor */
    uint16_t tx_buf_len;                   /**< Transmit buffer length */
    uint16_t tx_link_num;                  /**< Number of transmit buffer descriptors */
    uint16_t rx_buf_len;                   /**< Receive buffer length */
    uint16_t rx_link_num;                  /**< Number of receive buffer descriptors */
} wm_hal_hspi_slave_ctx_t;

/**
 * @brief HSPI slave device structure
 */
typedef struct {
    wm_hspi_slave_reg_t *hspi_slave_reg_base; /**< HSPI slave register base address */
    wm_sdio_wrapper_reg_t *wrapper_reg_base;  /**< wrapper register base address */
    wm_hal_hspi_slave_cfg_t hspi_slave_cfg;   /**< HSPI slave configuration */
    wm_irq_no_t irq_num;                      /**< Interrupt number */
    uint8_t irq_priority;                     /**< Interrupt priority */
    uint32_t sysclk;                          /**< system clock */
    wm_hal_hspi_slave_ctx_t ctx;              /**< HSPI slave context */
} wm_hal_hspi_slave_dev_t;

/**
 * @}
 */

/**
 * @defgroup WM_HAL_HSPI_SLAVE_Functions WM HAL HSPI SLAVE Functions
 * @brief WinnerMicro HSPI Slave HAL Functions
 */

/**
 * @addtogroup WM_HAL_HSPI_SLAVE_Functions
 * @{
 */

/**
 * @brief Initialize HSPI slave device
 *
 * This function initializes the HSPI slave device by configuring:
 * - Interrupt handler registration
 * - Interrupt mask settings
 *
 * @param[in] dev Pointer to HSPI slave device structure
 *
 * @return
 *    - WM_ERR_SUCCESS if initialization succeeds
 *    - WM_ERR_INVALID_PARAM if dev pointer is NULL or configuration is invalid
 */
int wm_hal_hspi_slave_init(wm_hal_hspi_slave_dev_t *dev);

/**
 * @brief Deinitialize HSPI slave device
 *
 * This function deinitializes the HSPI slave device by:
 * - Disabling interrupts
 * - Detaching interrupt handler
 * - Resetting device registers
 *
 * @param[in] dev Pointer to HSPI slave device structure
 *
 * @return
 *    - WM_ERR_SUCCESS if deinitialization succeeds
 *    - WM_ERR_INVALID_PARAM if dev pointer is NULL
 */
int wm_hal_hspi_slave_deinit(wm_hal_hspi_slave_dev_t *dev);

/**
 * @brief          config dma, cpol/cpha/tx_pin_drive/byte_order/burst_len
 * @param[in]      dev device reference
 * @param[in]      cfg config
 * @return         config operation status
 */
int wm_hal_hspi_slave_config(wm_hal_hspi_slave_dev_t *dev, wm_hal_hspi_slave_cfg_t *cfg);

/**
 * @brief Register buffer descriptors for HSPI slave device
 *
 * This function configures:
 * - CIS buffer addresses for Function 0 and 1
 * - Command buffer address and size
 * - TX/RX buffer descriptors for data transfer
 *
 * @param[in] dev Pointer to HSPI slave device structure
 * @param[in] buf_desc Pointer to buffer descriptor structure containing:
 *                     - CIS buffer addresses and sizes
 *                     - Command buffer address and size
 *                     - TX/RX buffer descriptors
 *
 * @return
 *    - WM_ERR_SUCCESS if registration succeeds
 *    - WM_ERR_INVALID_PARAM if parameters are NULL or buffer addresses invalid
 */
int wm_hal_hspi_slave_register_buf(wm_hal_hspi_slave_dev_t *dev, wm_hal_hspi_slave_buf_desc_t *buf_desc);

/**
 * @brief Unregister buffer descriptors from HSPI slave device
 *
 * This function clears:
 * - CIS buffer configurations
 * - Command buffer settings
 * - TX/RX buffer descriptors
 *
 * @param[in] dev Pointer to HSPI slave device structure
 *
 * @return
 *    - WM_ERR_SUCCESS if unregistration succeeds
 *    - WM_ERR_INVALID_PARAM if dev pointer is NULL
 */
int wm_hal_hspi_slave_unregister_buf(wm_hal_hspi_slave_dev_t *dev);

/**
 * @brief Register event callback for HSPI slave device
 *
 * This function registers a callback to handle HSPI slave events:
 * - Data transmission complete
 * - Data reception complete
 * - Command transmission complete
 * - Command reception complete
 *
 * @param[in] dev Pointer to HSPI slave device structure
 * @param[in] cb Callback function to handle events
 * @param[in] priv Private data passed to callback function
 *
 * @return
 *    - WM_ERR_SUCCESS if registration succeeds
 *    - WM_ERR_INVALID_PARAM if dev pointer is NULL
 */
int wm_hal_hspi_slave_register_callback(wm_hal_hspi_slave_dev_t *dev, wm_hal_hspi_slave_callback_t cb, void *priv);

/**
 * @brief Unregister event callback from HSPI slave device
 *
 * This function removes the previously registered event callback.
 *
 * @param[in] dev Pointer to HSPI slave device structure
 *
 * @return
 *    - WM_ERR_SUCCESS if unregistration succeeds
 *    - WM_ERR_INVALID_PARAM if dev pointer is NULL
 */
int wm_hal_hspi_slave_unregister_callback(wm_hal_hspi_slave_dev_t *dev);

/**
 * @brief Transmit data using DMA through HSPI slave device
 *
 * This function initiates DMA transfer of data to host by:
 * - Validating buffer address and length
 * - Setting up transmit buffer descriptors
 * - Enabling DMA transfer
 *
 * @param[in] dev Pointer to HSPI slave device structure
 * @param[in] addr Address of data buffer to transmit (must be in DRAM)
 * @param[in] len Length of data to transmit (must not exceed max buffer size)
 *
 * @return
 *    - WM_ERR_SUCCESS if transmission starts successfully
 *    - WM_ERR_INVALID_PARAM if parameters invalid or no available buffer descriptors
 */
int wm_hal_hspi_slave_tx_dma(wm_hal_hspi_slave_dev_t *dev, void *addr, int len);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_HAL_HSPI_SLAVE_H__ */

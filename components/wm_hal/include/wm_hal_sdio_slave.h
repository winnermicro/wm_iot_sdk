/**
 * @file wm_hal_sdio_slave.h
 *
 * @brief SDIO Slave Hardware Abstraction Layer Module
 *
 * This module provides hardware abstraction layer APIs for SDIO slave device configuration,
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

#ifndef __WM_HAL_SDIO_SLAVE_H__
#define __WM_HAL_SDIO_SLAVE_H__

#include "wm_irq.h"
#include "wm_ll_sdio_slave.h"
#include "wm_hal_sdio_wrapper.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_HAL_SDIO_SLAVE_Macros WM HAL SDIO SLAVE Macros
 * @brief WinnerMicro SDIO Slave HAL Macros
 */

/**
 * @addtogroup WM_HAL_SDIO_SLAVE_Macros
 * @{
 */

#define WM_HAL_SDIO_SLAVE_CIS_F0_BASE_ADDR (0x01000) /**< CIS function0 base address */
#define WM_HAL_SDIO_SLAVE_CIS_F1_BASE_ADDR (0x02000) /**< CIS function1 base address */
#define WM_HAL_SDIO_SLAVE_CMD_BUF_SIZE_MAX (0xFFF)   /**< Maximum command buffer size */
#define WM_HAL_SDIO_SLAVE_TIMEOUT          (100)     /**< Timeout in milliseconds */

/**
 * @}
 */

/**
 * @defgroup WM_HAL_SDIO_SLAVE_Enumerations WM HAL SDIO SLAVE Enumerations
 * @brief WinnerMicro SDIO Slave HAL Enumerations
 */

/**
 * @addtogroup WM_HAL_SDIO_SLAVE_Enumerations
 * @{
 */

/**
 * @brief SDIO slave function enumeration
 */
typedef enum {
    WM_HAL_SDIO_SLAVE_FUNC_0 = 0, /**< SDIO slave function 0 */
    WM_HAL_SDIO_SLAVE_FUNC_1 = 1, /**< SDIO slave function 1 */
    WM_HAL_SDIO_SLAVE_FUNC_MAX
} wm_hal_sdio_slave_func_t;

/**
 * @brief CCCR/FBR version enumeration
 */
typedef enum {
    WM_HAL_SDIO_SLAVE_CCCR_REV_1_00 = 0, /**< CCCR/FBR Version 1.00 */
    WM_HAL_SDIO_SLAVE_CCCR_REV_1_10,     /**< CCCR/FBR Version 1.10 */
    WM_HAL_SDIO_SLAVE_CCCR_REV_1_20,     /**< CCCR/FBR Version 1.20 */
    WM_HAL_SDIO_SLAVE_CCCR_REV_MAX
} wm_hal_sdio_slave_cccr_rev_t;

/**
 * @brief SDIO specification version enumeration
 */
typedef enum {
    WM_HAL_SDIO_SLAVE_SDIO_REV_1_00 = 0, /**< SDIO Specification 1.00 */
    WM_HAL_SDIO_SLAVE_SDIO_REV_1_10,     /**< SDIO Version 1.10 */
    WM_HAL_SDIO_SLAVE_SDIO_REV_1_20,     /**< SDIO Version 1.20 */
    WM_HAL_SDIO_SLAVE_SDIO_REV_2_00,     /**< SDIO Version 2.0 */
    WM_HAL_SDIO_SLAVE_SDIO_REV_MAX
} wm_hal_sdio_slave_sdio_rev_t;

/**
 * @brief SD Physical Specification version enumeration
 */
typedef enum {
    WM_HAL_SDIO_SLAVE_SD_REV_1_01 = 0, /**< SD Physical Specification 1.01 */
    WM_HAL_SDIO_SLAVE_SD_REV_1_10,     /**< SD Physical Specification 1.10 */
    WM_HAL_SDIO_SLAVE_SD_REV_2_00,     /**< SD Physical Specification 2.0 */
    WM_HAL_SDIO_SLAVE_SD_REV_MAX
} wm_hal_sdio_slave_sd_rev_t;

/**
 * @brief Function ready status enumeration
 */
typedef enum {
    WM_HAL_SDIO_SLAVE_F1_NOT_READY = 0, /**< Function 1 not ready */
    WM_HAL_SDIO_SLAVE_F1_READY     = 1, /**< Function 1 ready */
    WM_HAL_SDIO_SLAVE_F1_MAX
} wm_hal_sdio_slave_func_ready_t;

/**
 * @brief Function 1 read data ready status enumeration
 */
typedef enum {
    WM_HAL_SDIO_SLAVE_F1_READ_DATA_NOT_READY = 0, /**< Function 1 read data not ready */
    WM_HAL_SDIO_SLAVE_F1_READ_DATA_READY     = 1, /**< Function 1 read data ready */
    WM_HAL_SDIO_SLAVE_F1_READ_DATA_MAX
} wm_hal_sdio_slave_f1_read_ready_t;

/**
 * @brief SCSI support status enumeration
 */
typedef enum {
    WM_HAL_SDIO_SLAVE_SCSI_NOT_SUPPORT = 0, /**< SCSI not supported */
    WM_HAL_SDIO_SLAVE_SCSI_SUPPORT     = 1, /**< SCSI supported */
    WM_HAL_SDIO_SLAVE_SCSI_MAX
} wm_hal_sdio_slave_scsi_support_t;

/**
 * @brief SDC support status enumeration
 */
typedef enum {
    WM_HAL_SDIO_SLAVE_SDC_NOT_SUPPORT = 0, /**< SDC not supported */
    WM_HAL_SDIO_SLAVE_SDC_SUPPORT     = 1, /**< SDC supported */
    WM_HAL_SDIO_SLAVE_SDC_MAX
} wm_hal_sdio_slave_sdc_support_t;

/**
 * @brief SMB support status enumeration
 */
typedef enum {
    WM_HAL_SDIO_SLAVE_SMB_NOT_SUPPORT = 0, /**< SMB not supported */
    WM_HAL_SDIO_SLAVE_SMB_SUPPORT     = 1, /**< SMB supported */
    WM_HAL_SDIO_SLAVE_SMB_MAX
} wm_hal_sdio_slave_smb_support_t;

/**
 * @brief Read wait support status enumeration
 */
typedef enum {
    WM_HAL_SDIO_SLAVE_SRW_NOT_SUPPORT = 0, /**< Read wait not supported */
    WM_HAL_SDIO_SLAVE_SRW_SUPPORT     = 1, /**< Read wait supported */
    WM_HAL_SDIO_SLAVE_SRW_MAX
} wm_hal_sdio_slave_srw_support_t;

/**
 * @brief Suspend/resume support status enumeration
 */
typedef enum {
    WM_HAL_SDIO_SLAVE_SBS_NOT_SUPPORT = 0, /**< Suspend/resume not supported */
    WM_HAL_SDIO_SLAVE_SBS_SUPPORT     = 1, /**< Suspend/resume supported */
    WM_HAL_SDIO_SLAVE_SBS_MAX
} wm_hal_sdio_slave_sbs_support_t;

/**
 * @brief 4-bit interrupt mode support status enumeration
 */
typedef enum {
    WM_HAL_SDIO_SLAVE_S4MI_NOT_SUPPORT = 0, /**< 4-bit interrupt mode not supported */
    WM_HAL_SDIO_SLAVE_S4MI_SUPPORT     = 1, /**< 4-bit interrupt mode supported */
    WM_HAL_SDIO_SLAVE_S4MI_MAX
} wm_hal_sdio_slave_s4mi_support_t;

/**
 * @brief LSC device type enumeration
 */
typedef enum {
    WM_HAL_SDIO_SLAVE_LSC_FULL_SPEED = 0, /**< Full speed device */
    WM_HAL_SDIO_SLAVE_LSC_LOW_SPEED  = 1, /**< Low speed device */
    WM_HAL_SDIO_SLAVE_LSC_MAX
} wm_hal_sdio_slave_lsc_t;

/**
 * @brief 4-bit data transfer support status for low speed device enumeration
 */
typedef enum {
    WM_HAL_SDIO_SLAVE_BLS4_NOT_SUPPORT = 0, /**< 4-bit data transfer not supported */
    WM_HAL_SDIO_SLAVE_BLS4_SUPPORT     = 1, /**< 4-bit data transfer supported */
    WM_HAL_SDIO_SLAVE_BLS4_MAX
} wm_hal_sdio_slave_bls4_support_t;

/**
 * @brief Card ready status enumeration
 */
typedef enum {
    WM_HAL_SDIO_SLAVE_CARD_NOT_READY = 0, /**< Card not ready after power-on reset */
    WM_HAL_SDIO_SLAVE_CARD_READY     = 1, /**< Card ready after power-on reset */
    WM_HAL_SDIO_SLAVE_CARD_MAX
} wm_hal_sdio_slave_card_ready_t;

/**
 * @brief SDIO slave event types enumeration
 */
typedef enum {
    WM_HAL_SDIO_SLAVE_EVENT_TX     = 0, /**< Data transmit complete event */
    WM_HAL_SDIO_SLAVE_EVENT_RX     = 1, /**< Data receive complete event */
    WM_HAL_SDIO_SLAVE_EVENT_TX_CMD = 2, /**< Command transmit complete event */
    WM_HAL_SDIO_SLAVE_EVENT_RX_CMD = 3, /**< Command receive complete event */
    WM_HAL_SDIO_SLAVE_EVENT_MAX
} wm_hal_sdio_slave_event_t;

/**
 * @}
 */

/**
 * @defgroup WM_HAL_SDIO_SLAVE_Structures WM HAL SDIO SLAVE Structures
 * @brief WinnerMicro SDIO Slave HAL Structures
 */

/**
 * @addtogroup WM_HAL_SDIO_SLAVE_Structures
 * @{
 */

/**
 * @brief SDIO slave buffer descriptor structure
 */
typedef struct {
    void *f0_cis_addr;                /**< Function 0 CIS buffer address */
    void *f1_cis_addr;                /**< Function 1 CIS buffer address */
    uint16_t f0_cis_size;             /**< Function 0 CIS buffer size */
    uint16_t f1_cis_size;             /**< Function 1 CIS buffer size */
    void *cmd_rxbuf_addr;             /**< Command receive buffer address */
    uint32_t cmd_rxbuf_size;          /**< Command receive buffer size */
    wm_hal_sdio_wrapper_txbd_t *txbd; /**< Transmit buffer descriptor */
    wm_hal_sdio_wrapper_rxbd_t *rxbd; /**< Receive buffer descriptor */
    uint32_t tx_buf_len;              /**< Transmit buffer length */
    uint32_t rx_buf_len;              /**< Receive buffer length */
    uint16_t tx_link_num;             /**< Number of transmit buffer descriptors */
    uint16_t rx_link_num;             /**< Number of receive buffer descriptors */
} wm_hal_sdio_slave_buf_desc_t;

/**
 * @brief SDIO slave configuration structure
 */
typedef struct {
    wm_hal_sdio_slave_cccr_rev_t ccr_rev;  /**< CCCR/FBR version */
    wm_hal_sdio_slave_sdio_rev_t sdio_rev; /**< SDIO specification version */
    wm_hal_sdio_slave_sd_rev_t sd_rev;     /**< SD Physical Specification version */
} wm_hal_sdio_slave_cfg_t;

/**
 * @brief SDIO slave event callback function type
 */
typedef void (*wm_hal_sdio_slave_callback_t)(void *hal_dev, wm_hal_sdio_slave_event_t event, void *addr, void *priv);

/**
 * @brief SDIO slave callback information structure
 */
typedef struct {
    wm_hal_sdio_slave_callback_t cb; /**< Event callback function */
    void *priv;                      /**< Private data for callback */
} wm_hal_sdio_slave_cb_info_t;

/**
 * @brief SDIO slave context structure
 */
typedef struct {
    wm_hal_sdio_slave_cb_info_t cb_info;   /**< Callback information */
    wm_hal_sdio_wrapper_txbd_t *curr_txbd; /**< Current transmit buffer descriptor */
    wm_hal_sdio_wrapper_rxbd_t *curr_rxbd; /**< Current receive buffer descriptor */
    uint16_t f0_cis_size;                  /**< Function 0 CIS buffer size */
    uint16_t f1_cis_size;                  /**< Function 1 CIS buffer size */
    uint16_t tx_buf_len;                   /**< Transmit buffer length */
    uint16_t tx_link_num;                  /**< Number of transmit buffer descriptors */
    uint16_t rx_buf_len;                   /**< Receive buffer length */
    uint16_t rx_link_num;                  /**< Number of receive buffer descriptors */
} wm_hal_sdio_slave_ctx_t;

/**
 * @brief SDIO slave device structure
 */
typedef struct {
    wm_sdio_slave_reg_t *sdio_slave_reg_base; /**< SDIO slave register base address */
    wm_sdio_wrapper_reg_t *wrapper_reg_base;  /**< SDIO wrapper register base address */
    wm_hal_sdio_slave_cfg_t sdio_slave_cfg;   /**< SDIO slave configuration */
    wm_irq_no_t irq_num;                      /**< Interrupt number */
    uint8_t irq_priority;                     /**< Interrupt priority */
    uint32_t sysclk;                          /**< system clock */
    wm_hal_sdio_slave_ctx_t ctx;              /**< SDIO slave context */
} wm_hal_sdio_slave_dev_t;

/**
 * @}
 */

/**
 * @defgroup WM_HAL_SDIO_SLAVE_Functions WM HAL SDIO SLAVE Functions
 * @brief WinnerMicro SDIO Slave HAL Functions
 */

/**
 * @addtogroup WM_HAL_SDIO_SLAVE_Functions
 * @{
 */

/**
 * @brief Initialize SDIO slave device
 *
 * This function initializes the SDIO slave device by configuring:
 * - CCCR/SDIO/SD version registers
 * - Function 1 ready status
 * - Interrupt handler registration
 * - Interrupt mask settings
 *
 * @param[in] dev Pointer to SDIO slave device structure
 *
 * @return
 *    - WM_ERR_SUCCESS if initialization succeeds
 *    - WM_ERR_INVALID_PARAM if dev pointer is NULL or configuration is invalid
 */
int wm_hal_sdio_slave_init(wm_hal_sdio_slave_dev_t *dev);

/**
 * @brief Deinitialize SDIO slave device
 *
 * This function deinitializes the SDIO slave device by:
 * - Disabling interrupts
 * - Detaching interrupt handler
 * - Resetting device registers
 *
 * @param[in] dev Pointer to SDIO slave device structure
 *
 * @return
 *    - WM_ERR_SUCCESS if deinitialization succeeds
 *    - WM_ERR_INVALID_PARAM if dev pointer is NULL
 */
int wm_hal_sdio_slave_deinit(wm_hal_sdio_slave_dev_t *dev);

/**
 * @brief Register buffer descriptors for SDIO slave device
 *
 * This function configures:
 * - CIS buffer addresses for Function 0 and 1
 * - Command buffer address and size
 * - TX/RX buffer descriptors for data transfer
 *
 * @param[in] dev Pointer to SDIO slave device structure
 * @param[in] buf_desc Pointer to buffer descriptor structure containing:
 *                     - CIS buffer addresses and sizes
 *                     - Command buffer address and size
 *                     - TX/RX buffer descriptors
 *
 * @return
 *    - WM_ERR_SUCCESS if registration succeeds
 *    - WM_ERR_INVALID_PARAM if parameters are NULL or buffer addresses invalid
 */
int wm_hal_sdio_slave_register_buf(wm_hal_sdio_slave_dev_t *dev, wm_hal_sdio_slave_buf_desc_t *buf_desc);

/**
 * @brief Unregister buffer descriptors from SDIO slave device
 *
 * This function clears:
 * - CIS buffer configurations
 * - Command buffer settings
 * - TX/RX buffer descriptors
 *
 * @param[in] dev Pointer to SDIO slave device structure
 *
 * @return
 *    - WM_ERR_SUCCESS if unregistration succeeds
 *    - WM_ERR_INVALID_PARAM if dev pointer is NULL
 */
int wm_hal_sdio_slave_unregister_buf(wm_hal_sdio_slave_dev_t *dev);

/**
 * @brief Read register value from SDIO slave device
 *
 * This function reads a 32-bit value from the specified register offset
 * in either Function 0 or Function 1 CIS space.
 *
 * @param[in] dev Pointer to SDIO slave device structure
 * @param[in] func SDIO function number (0 or 1)
 * @param[in] pos Register offset within CIS space
 * @param[out] reg_val Pointer to store the read register value
 *
 * @return
 *    - WM_ERR_SUCCESS if read succeeds
 *    - WM_ERR_INVALID_PARAM if parameters invalid or offset exceeds CIS size
 */
int wm_hal_sdio_slave_read_cis_reg(wm_hal_sdio_slave_dev_t *dev, wm_hal_sdio_slave_func_t func, uint32_t pos,
                                   uint32_t *reg_val);

/**
 * @brief Write register value to SDIO slave device
 *
 * This function writes a 32-bit value to the specified register offset
 * in either Function 0 or Function 1 CIS space.
 *
 * @param[in] dev Pointer to SDIO slave device structure
 * @param[in] func SDIO function number (0 or 1)
 * @param[in] pos Register offset within CIS space
 * @param[in] reg_val Value to write to the register
 *
 * @return
 *    - WM_ERR_SUCCESS if write succeeds
 *    - WM_ERR_INVALID_PARAM if parameters invalid or offset exceeds CIS size
 */
int wm_hal_sdio_slave_write_cis_reg(wm_hal_sdio_slave_dev_t *dev, wm_hal_sdio_slave_func_t func, uint32_t pos,
                                    uint32_t reg_val);

/**
 * @brief Register event callback for SDIO slave device
 *
 * This function registers a callback to handle SDIO slave events:
 * - Data transmission complete
 * - Data reception complete
 * - Command transmission complete
 * - Command reception complete
 *
 * @param[in] dev Pointer to SDIO slave device structure
 * @param[in] cb Callback function to handle events
 * @param[in] priv Private data passed to callback function
 *
 * @return
 *    - WM_ERR_SUCCESS if registration succeeds
 *    - WM_ERR_INVALID_PARAM if dev pointer is NULL
 */
int wm_hal_sdio_slave_register_callback(wm_hal_sdio_slave_dev_t *dev, wm_hal_sdio_slave_callback_t cb, void *priv);

/**
 * @brief Unregister event callback from SDIO slave device
 *
 * This function removes the previously registered event callback.
 *
 * @param[in] dev Pointer to SDIO slave device structure
 *
 * @return
 *    - WM_ERR_SUCCESS if unregistration succeeds
 *    - WM_ERR_INVALID_PARAM if dev pointer is NULL
 */
int wm_hal_sdio_slave_unregister_callback(wm_hal_sdio_slave_dev_t *dev);

/**
 * @brief Transmit data using DMA through SDIO slave device
 *
 * This function initiates DMA transfer of data to host by:
 * - Validating buffer address and length
 * - Setting up transmit buffer descriptors
 * - Enabling DMA transfer
 *
 * @param[in] dev Pointer to SDIO slave device structure
 * @param[in] addr Address of data buffer to transmit (must be in DRAM)
 * @param[in] len Length of data to transmit (must not exceed max buffer size)
 *
 * @return
 *    - WM_ERR_SUCCESS if transmission starts successfully
 *    - WM_ERR_INVALID_PARAM if parameters invalid or no available buffer descriptors
 */
int wm_hal_sdio_slave_tx_dma(wm_hal_sdio_slave_dev_t *dev, void *addr, int len);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_HAL_SDIO_SLAVE_H__ */

/**
 * @file wm_hal_sdio_wrapper.h
 *
 * @brief sdio slave hardware abstraction layer header file
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

#ifndef __WM_HAL_SDIO_WRAPPER_H__
#define __WM_HAL_SDIO_WRAPPER_H__

#include <string.h>
#include "wm_osal.h"
#include "wm_types.h"
#include "wm_soc_cfgs.h"
#include "wm_ll_sdio_wrapper.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief TX Link Enable Status enumeration
 *
 * Controls whether hardware continues processing next TX descriptor after current one.
 * When enabled, hardware will process next TX descriptor pointed to by next_txbd_addr.
 * When disabled, hardware stops after processing current TX descriptor.
 */
typedef enum {
    WM_HAL_SDIO_WRAPPER_TX_LINK_DISABLE = 0, /**< Stop after current TX descriptor */
    WM_HAL_SDIO_WRAPPER_TX_LINK_ENABLE  = 1, /**< Continue to next TX descriptor */
    WM_HAL_SDIO_WRAPPER_TX_LINK_MAX
} wm_hal_sdio_wrapper_tx_link_t;

/**
 * @brief RX Link Enable Status enumeration
 *
 * Controls whether hardware continues processing next RX descriptor after current one.
 * When enabled, hardware will process next RX descriptor pointed to by next_rxbd_addr.
 * When disabled, hardware stops after processing current RX descriptor.
 */
typedef enum {
    WM_HAL_SDIO_WRAPPER_RX_LINK_DISABLE = 0, /**< Stop after current RX descriptor */
    WM_HAL_SDIO_WRAPPER_RX_LINK_ENABLE  = 1, /**< Continue to next RX descriptor */
    WM_HAL_SDIO_WRAPPER_RX_LINK_MAX
} wm_hal_sdio_wrapper_rx_link_t;

/**
 * @brief SDIO Transmit Enable Status enumeration
 *
 * Controls whether hardware starts processing TX descriptors.
 * When enabled, hardware will start reading current TX descriptor and transmit frame.
 * Hardware automatically clears this bit after transmission completes.
 */
typedef enum {
    WM_HAL_SDIO_WRAPPER_TX_DISABLE = 0, /**< TX disabled */
    WM_HAL_SDIO_WRAPPER_TX_ENABLE  = 1, /**< TX enabled - start transmitting */
    WM_HAL_SDIO_WRAPPER_TX_MAX
} wm_hal_sdio_wrapper_tx_en_t;

/**
 * @brief SDIO Receive Enable Status enumeration
 *
 * Controls whether hardware starts processing RX descriptors.
 * When enabled, hardware will start reading current RX descriptor and receive frame.
 * Hardware automatically clears this bit after reception completes.
 */
typedef enum {
    WM_HAL_SDIO_WRAPPER_RX_DISABLE = 0, /**< RX disabled */
    WM_HAL_SDIO_WRAPPER_RX_ENABLE  = 1, /**< RX enabled - start receiving */
    WM_HAL_SDIO_WRAPPER_RX_MAX
} wm_hal_sdio_wrapper_rx_en_t;

/**
 * @brief SDIO Wrapper Downlink Command Ready Status enumeration
 *
 * Controls downlink command buffer ready status.
 * Hardware clears this bit and generates interrupt after sending downlink command.
 * Firmware sets this bit after processing the downlink command.
 */
typedef enum {
    WM_HAL_SDIO_WRAPPER_DOWNLINK_CMD_BUF_NOT_READY = 0, /**< Downlink command buffer not ready */
    WM_HAL_SDIO_WRAPPER_DOWNLINK_CMD_BUF_READY     = 1, /**< Downlink command buffer ready */
    WM_HAL_SDIO_WRAPPER_DOWNLINK_CMD_BUF_MAX
} wm_hal_sdio_wrapper_downlink_cmd_buf_ready_t;

/**
 * @brief SDIO Wrapper Event Type enumeration
 *
 * Defines the different types of events that can occur during SDIO wrapper operation:
 * - Host to slave data transfer (RX)
 * - Slave to host data transfer (TX)
 * - Host to slave command (UP_RX_CMD)
 * - Slave to host command (DOWN_TX_CMD)
 */
typedef enum {
    WM_HAL_SDIO_WRAPPER_EVENT_RX          = 0, /**< Host to slave data transfer */
    WM_HAL_SDIO_WRAPPER_EVENT_TX          = 1, /**< Slave to host data transfer */
    WM_HAL_SDIO_WRAPPER_EVENT_UP_RX_CMD   = 2, /**< Host to slave command */
    WM_HAL_SDIO_WRAPPER_EVENT_DOWN_TX_CMD = 3, /**< Slave to host command */
    WM_HAL_SDIO_WRAPPER_EVENT_MAX
} wm_hal_sdio_wrapper_event_t;

/**
 * @brief SDIO Wrapper Interrupt Status enumeration
 *
 * Defines the interrupt status bits for SDIO wrapper.
 * Writing 1 clears the corresponding interrupt bit.
 */
typedef enum {
    WM_HAL_SDIO_WRAPPER_INT_STS_UP_FRAME_DONE   = WM_BIT(0), /**< Uplink data frame complete interrupt */
    WM_HAL_SDIO_WRAPPER_INT_STS_DOWN_FRAME_DONE = WM_BIT(1), /**< Downlink data frame complete interrupt */
    WM_HAL_SDIO_WRAPPER_INT_STS_UP_CMD_DONE     = WM_BIT(2), /**< Uplink command frame complete interrupt */
    WM_HAL_SDIO_WRAPPER_INT_STS_DOWN_CMD_DONE   = WM_BIT(3)  /**< Downlink command frame complete interrupt */
} wm_hal_sdio_wrapper_int_sts_t;

/**
 * @brief SDIO Wrapper Transmit Buffer Descriptor structure (Host --> Slave)
 *
 * Used to create a linked list of transmit buffer descriptors for Host --> Slave SDIO transfers.
 */
typedef struct wm_hal_sdio_wrapper_txbd {
    uint32_t reserved     : 31;            /**< [30:0] Reserved bits */
    uint32_t valid        : 1;             /**< [31] Valid flag */
    uint32_t txbuf_offset : 12;            /**< [11:0] TX buffer 0 offset */
    uint32_t frm_len      : 14;            /**< [25:12] Frame length */
    uint32_t tx_info_size : 6;             /**< [31:26] SDIO TX info size */
    uint32_t txbuf_addr[3];                /**< TX buffer address */
    struct wm_hal_sdio_wrapper_txbd *next; /**< Pointer to next TX buffer descriptor */
} wm_hal_sdio_wrapper_txbd_t;

/**
 * @brief SDIO Wrapper Receive Buffer Descriptor structure (Slave --> Host)
 *
 * Used to create a linked list of receive buffer descriptors for Slave --> Host SDIO transfers.
 */
typedef struct wm_hal_sdio_wrapper_rxbd {
    uint32_t reserved : 31;                /**< [30:0] Reserved bits */
    uint32_t valid    : 1;                 /**< [31] Valid flag */
    uint32_t rxbuf_addr;                   /**< RX buffer address */
    struct wm_hal_sdio_wrapper_rxbd *next; /**< Pointer to next RX buffer descriptor */
} wm_hal_sdio_wrapper_rxbd_t;

#ifdef __cplusplus
}
#endif

#endif /* __WM_HAL_SDIO_WRAPPER_H__ */

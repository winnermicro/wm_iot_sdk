/**
 * @file wm_hal_uart.h
 *
 * @brief hal uart Module
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

#ifndef __WM_HAL_UART_H__
#define __WM_HAL_UART_H__

#include "wm_types.h"
#include "wm_soc_cfgs.h"
#include "wm_hal_dma.h"
#include "wm_ll_uart.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Maximum number of buffers for receiving,Number of nodes in chain.
 */
#define WM_HAL_RX_RECV_BUF_NUM 4

/**
 * @enum wm_hal_uart_evt_type_t
 * @brief Uart event type.
 */
typedef enum {
    WM_HAL_UART_TX_DONE,  /**< Transfer done                                 */
    WM_HAL_UART_RX_READY, /**< A block data received or rx fifo timeout came */
    WM_HAL_UART_RX_ERROR, /**< Receive error                                 */
} wm_hal_uart_evt_type_t;

/**
 * @enum wm_hal_uart_evt_error_t
 * @brief uart event error type.
 */
typedef enum {
    WM_HAL_UART_ERROR_OVERRUN   = (1 << 0), /**< fifo overrun          */
    WM_HAL_UART_ERROR_PARITY    = (1 << 1), /**< parity error          */
    WM_HAL_UART_ERROR_FRAMING   = (1 << 2), /**< frame error           */
    WM_HAL_UART_BREAK           = (1 << 3), /**< break event           */
    WM_HAL_UART_ERROR_COLLISION = (1 << 4), /**< 485 half duplex error */
} wm_hal_uart_evt_error_t;

/**
 * @struct wm_hal_uart_evt_t
 * @brief Uart event for RX/TX/Error.
 */
typedef struct {
    wm_hal_uart_evt_type_t type; /**< uart event type             */
    union {
        struct {
            wm_hal_uart_evt_error_t error; /**< uart error        */
        };
        struct {
            uint32_t tx_data_len; /**< tx send data len           */
        };
        struct {
            uint8_t *rx_data;        /**< rx ready data           */
            uint16_t rx_len;         /**< rx ready data len       */
            uint8_t rx_timeout_flag; /**< rx timeout flag         */
        };
    };
} wm_hal_uart_evt_t;

/**
 * @struct wm_hal_uart_dma_t
 * @brief uart dma information, used for the api wm_hal_uart_set_dma.
 */
typedef struct {
    wm_hal_dma_dev_t *rx_dma; /**< rx dma                    */
    wm_hal_dma_dev_t *tx_dma; /**< tx dma                    */
    uint8_t rx_dma_ch;        /**< rx dma  channel           */
    uint8_t tx_dma_ch;        /**< tx dma  channel           */
} wm_hal_uart_dma_t;

/**
 * @struct wm_hal_uart_config_t
 * @brief uart hardware configuration type.
 */
typedef struct {
    uint32_t baudrate;             /**< baudrate reg value        */
    wm_uart_data_bits_t data_bits; /**< data bits                 */
    wm_uart_stop_bits_t stop_bits; /**< stop bits                 */
    wm_uart_parity_t parity;       /**< parity type               */
    wm_uart_flowctrl_t flow_ctrl;  /**< flow ctrl                 */
} wm_hal_uart_config_t;

/**
 * @struct wm_hal_uart_dev_t
 * @brief The fisrt 4 attribute must set by user before call wm_hal_uart_init.
 */
typedef struct {
    wm_uart_reg_t *reg;       /**< register base address     */
    wm_irq_no_t irq_num;      /**< irq num                   */
    uint8_t irq_priority;     /**< irq priority              */
    uint8_t port;             /**< uart port index           */
    uint8_t rx_dma_ch;        /**< rx dma  channel           */
    uint8_t tx_dma_ch;        /**< tx dma  channel           */
    wm_hal_dma_dev_t *rx_dma; /**< rx dma                    */
    wm_hal_dma_dev_t *tx_dma; /**< tx dma                    */
    uint32_t ref_clock;       /**< reference clock,def APB   */
    void *ctx;                /**< hal internal context      */
} wm_hal_uart_dev_t;

/**
 * @struct wm_hal_uart_callback_t
 */
typedef void (*wm_hal_uart_callback_t)(void *hal_dev, wm_hal_uart_evt_t *evt, void *user_data);

/**
  * @brief  uart initialize
  *
  * @param[in] dev  uart device,type of  @ref wm_hal_uart_dev_t *
  * @param[in] config  uart configuation,type of  @ref wm_hal_uart_config_t *
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_FAILED: failed
  *    - WM_ERR_INVALID_PARAM： invalid param
  */
int wm_hal_uart_init(wm_hal_uart_dev_t *dev, wm_hal_uart_config_t *config);

/**
  * @brief  uart deinitialize
  *
  * @param[in] dev  uart device,type of  @ref wm_hal_uart_dev_t *
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM： invalid param
  */
int wm_hal_uart_deinit(wm_hal_uart_dev_t *dev);

/**
  * @brief  uart io control
  *
  * @param[in] dev  uart device,type of  @ref wm_hal_uart_dev_t *
  * @param[in] cmd  uart io control command type
  * @param[in] param  uart io control integer param
  * @param[in] arg  uart io control pointer argument
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_FAILED: failed
  *    - WM_ERR_INVALID_PARAM： invalid param
  */

int wm_hal_uart_control(wm_hal_uart_dev_t *dev, int cmd, uint32_t param, void *arg);

/**
  * @brief  set tx,rx dma device and channel
  *
  * @param[in] dev  uart device,type of  @ref wm_hal_uart_dev_t *
  * @param[in] dma  dma device and channel, @ref wm_hal_uart_dma_t *
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM： invalid param
  */

int wm_hal_uart_set_dma(wm_hal_uart_dev_t *dev, wm_hal_uart_dma_t *dma);

/**
  * @brief  uart put char
  *
  * @param[in] dev  uart device,type of  @ref wm_hal_uart_dev_t *
  * @param[in] ch  uart char
  *
  */
int wm_hal_uart_putchar(wm_hal_uart_dev_t *dev, int ch);

/**
  * @brief  uart get char
  *
  * @param[in] dev  uart device,type of  @ref wm_hal_uart_dev_t *
  *
  * @return
  *    - >=0 : char
  *      <0  : error
  */
int wm_hal_uart_getchar(wm_hal_uart_dev_t *dev);

/**
  * @brief  Uart TX synchronous transmission
  *
  * @param[in] dev  uart device,type of  @ref wm_hal_uart_dev_t *
  * @param[in] data  data to be sent
  * @param[in] size  data size
  *
  * @return
  *    - > 0 bytes sent
  *    - WM_ERR_INVALID_PARAM
  */
int wm_hal_uart_tx_polling(wm_hal_uart_dev_t *dev, const uint8_t *data, uint32_t size);

/**
  * @brief  Uart rx synchronous reception
  *
  * @param[in] dev  uart device,type of  @ref wm_hal_uart_dev_t *
  * @param[in] buffer  buffer for reception
  * @param[in] size  data size
  * @param[in] timeout_ms  timeout millisecond
  *
  * @return
  *    - > 0 bytes received
  *    - WM_ERR_INVALID_PARAM
  */
int wm_hal_uart_rx_polling(wm_hal_uart_dev_t *dev, uint8_t *buffer, uint32_t size, uint32_t timeout_ms);

/**
  * @brief  uart tx asynchronous transmission with  dma
  *
  * @param[in] dev  uart device,type of  @ref wm_hal_uart_dev_t *
  * @param[in] data  data to be sent
  * @param[in] size  data size
  *
  * @return
  *    - > 0: bytes sent
  *    - WM_ERR_FAILED: failed
  *    - WM_ERR_INVALID_PARAM： invalid param
  */
int wm_hal_uart_tx_dma(wm_hal_uart_dev_t *dev, const uint8_t *data, uint32_t size);

/**
  * @brief  uart tx asynchronous transmission
  *
  * @param[in] dev  uart device,type of  @ref wm_hal_uart_dev_t *
  * @param[in] data  data to be sent
  * @param[in] size  data size
  *
  * @return
  *    - > 0: bytes sent
  *    - WM_ERR_FAILED: failed
  *    - WM_ERR_INVALID_PARAM： invalid param
  */
int wm_hal_uart_tx_it(wm_hal_uart_dev_t *dev, const uint8_t *data, uint32_t size);

/**
  * @brief  uart rx asynchronous reception with dma
  *
  * @param[in] dev  uart device,type of  @ref wm_hal_uart_dev_t *
  * @param[in] buffer  buffer for reception
  * @param[in] size  data size
  *
  * @return
  *    - > 0: bytes received
  *    - WM_ERR_FAILED: failed
  *    - WM_ERR_INVALID_PARAM： invalid param
  */
int wm_hal_uart_rx_dma(wm_hal_uart_dev_t *dev, uint8_t *buffer, uint32_t size);

/**
  * @brief  uart rx asynchronous reception
  *
  * @param[in] dev  uart device,type of  @ref wm_hal_uart_dev_t *
  * @param[in] buffer  buffer for reception
  * @param[in] size  data size
  *
  * @return
  *    - > 0: bytes received
  *    - WM_ERR_FAILED: failed
  *    - WM_ERR_INVALID_PARAM： invalid param
  */
int wm_hal_uart_rx_it(wm_hal_uart_dev_t *dev, uint8_t *buffer, uint32_t size);

/**
  * @brief  uart register tx callback
  *
  * @param[in] dev  uart device,type of  @ref wm_hal_uart_dev_t *
  * @param[in] user_callback  callback function, @ref wm_hal_uart_callback_t
  * @param[in] user_data  user data,it will be passed to the user when calling the callback function
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM： invalid param
  */
int wm_hal_uart_register_tx_callback(wm_hal_uart_dev_t *dev, wm_hal_uart_callback_t user_callback, void *user_data);

/**
  * @brief  uart register tx callback
  *
  * @param[in] dev  uart device,type of  @ref wm_hal_uart_dev_t *
  * @param[in] user_callback  callback function, @ref wm_hal_uart_callback_t
  * @param[in] user_data  user data,it will be passed to the user when calling the callback function
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM： invalid param
  */
int wm_hal_uart_register_rx_callback(wm_hal_uart_dev_t *dev, wm_hal_uart_callback_t user_callback, void *user_data);

/**
  * @brief  set uart config,include baudrate,data_bits,stop_bits,parity,flow_ctrl
  *
  * @param[in] dev  uart device,type of  @ref wm_hal_uart_dev_t *
  * @param[in] config  config, @ref wm_hal_uart_config_t
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM： invalid param
  */
int wm_hal_uart_set_config(wm_hal_uart_dev_t *dev, wm_hal_uart_config_t *config);

/**
  * @brief  get uart config,include baudrate,data_bits,stop_bits,parity,flow_ctrl
  *
  * @param[in] dev  uart device,type of  @ref wm_hal_uart_dev_t *
  * @param[out] config  config, @ref wm_hal_uart_config_t
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM： invalid param
  */
int wm_hal_uart_get_config(wm_hal_uart_dev_t *dev, wm_hal_uart_config_t *config);

/**
  * @brief  set baudrate
  *
  * @param[in] dev  uart device,type of  @ref wm_hal_uart_dev_t *
  * @param[in] baudrate baudrate define as wm_uart_baudrate_t
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM： invalid param
  */
int wm_hal_uart_set_baudrate(wm_hal_uart_dev_t *dev, uint32_t baudrate);

/**
  * @brief  get baudrate
  *
  * @param[in] dev  uart device,type of  @ref wm_hal_uart_dev_t *
  * @param[out] baudrate baudrate define as wm_uart_baudrate_t
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM： invalid param
  */
int wm_hal_uart_get_baudrate(wm_hal_uart_dev_t *dev, uint32_t *baudrate);

/**
  * @brief  set data bits
  *
  * @param[in] dev  uart device,type of  @ref wm_hal_uart_dev_t *
  * @param[in] data_bits data bits, type of  @ref  wm_uart_data_bits_t
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM： invalid param
  */
int wm_hal_uart_set_data_bits(wm_hal_uart_dev_t *dev, wm_uart_data_bits_t data_bits);

/**
  * @brief  get data bits
  *
  * @param[in] dev  uart device,type of  @ref wm_hal_uart_dev_t *
  * @param[out] data_bits data bits, type of  @ref  wm_uart_data_bits_t
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM： invalid param
  */
int wm_hal_uart_get_data_bits(wm_hal_uart_dev_t *dev, wm_uart_data_bits_t *data_bits);

/**
  * @brief  set stop bits
  *
  * @param[in] dev  uart device,type of  @ref wm_hal_uart_dev_t *
  * @param[in] stop_bits stop bits, type of  @ref  wm_uart_stop_bits_t
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM： invalid param
  */
int wm_hal_uart_set_stop_bits(wm_hal_uart_dev_t *dev, wm_uart_stop_bits_t stop_bits);

/**
  * @brief  get stop bits
  *
  * @param[in] dev  uart device,type of  @ref wm_hal_uart_dev_t *
  * @param[out] stop_bits stop bits, type of  @ref  wm_uart_stop_bits_t
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM： invalid param
  */
int wm_hal_uart_get_stop_bits(wm_hal_uart_dev_t *dev, wm_uart_stop_bits_t *stop_bits);

/**
  * @brief  set parity
  *
  * @param[in] dev  uart device,type of  @ref wm_hal_uart_dev_t *
  * @param[in] parity stop parity, type of  @ref  wm_uart_parity_t
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM： invalid param
  */
int wm_hal_uart_set_parity(wm_hal_uart_dev_t *dev, wm_uart_parity_t parity);

/**
  * @brief  get parity
  *
  * @param[in] dev  uart device,type of  @ref wm_hal_uart_dev_t *
  * @param[out] parity stop parity, type of  @ref  wm_uart_parity_t
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM： invalid param
  */
int wm_hal_uart_get_parity(wm_hal_uart_dev_t *dev, wm_uart_parity_t *parity);

/**
  * @brief  set flow_ctrl
  *
  * @param[in] dev  uart device,type of  @ref wm_hal_uart_dev_t *
  * @param[in] flow_ctrl flow_ctrl, type of  @ref  wm_uart_flowctrl_t
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM： invalid param
  */
int wm_hal_uart_set_flow_ctrl(wm_hal_uart_dev_t *dev, wm_uart_flowctrl_t flow_ctrl);

/**
  * @brief  get flow_ctrl
  *
  * @param[in] dev  uart device,type of  @ref wm_hal_uart_dev_t *
  * @param[out] flow_ctrl flow_ctrl, type of  @ref  wm_uart_flowctrl_t
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM： invalid param
  */
int wm_hal_uart_get_flow_ctrl(wm_hal_uart_dev_t *dev, wm_uart_flowctrl_t *flow_ctrl);

#ifdef __cplusplus
}
#endif

#endif
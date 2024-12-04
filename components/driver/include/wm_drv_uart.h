/**
 * @file wm_drv_uart.h
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

#ifndef __WM_DRV_UART_H__
#define __WM_DRV_UART_H__

#include "wm_types.h"
#include "wm_error.h"
#include "wm_attr.h"
#include "wm_soc_cfgs.h"
#include "wm_dt.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_UART_MACROs WM UART Macros
 * @brief WinnerMicro UART Macros
 */

/**
 * @addtogroup WM_UART_MACROs
 * @{
 */
/**
 * @brief RX minimum buffer size
 */
#define WM_DRV_UART_RX_MIN_BUF_SIZE    128

/**
 * @brief TX minimum buffer size
 */
#define WM_DRV_UART_TX_MIN_BUF_SIZE    128

/**
  * @brief  MAX buffer size
  */
#define WM_DRV_UART_BUF_BLOCK_MAX_SIZE (64 * 1024)

/**
 * @}
 */

/**
 * @defgroup WM_UART_Enumerations WM UART Enumerations
 * @brief WinnerMicro UART Enumerations
 */

/**
 * @addtogroup WM_UART_Enumerations
 * @{
 */

/**
 * @enum wm_drv_uart_evt_type_t
 * @brief Event type, used in callback
 */
typedef enum {
    WM_DRV_UART_TX_DONE,  /**< If tx_buf_size is specified, the Uart driver has sent some buffer data to the hardware fifo.
                            The user can now write another packet to the uart buffer.
                          If tx_buf_size is not specified, the Uart driver has sent the written data to the hardware fifo.*/
    WM_DRV_UART_RX_READY, /**< RX data ready ,some data is cached in uart buffer, The user can read it now */
    WM_DRV_UART_RX_ERROR, /**< RX error           */
} wm_drv_uart_evt_type_t;

/**
 * @enum wm_drv_uart_evt_error_t
 * @brief Event error type, used in callback
 */
typedef enum {
    WM_DRV_UART_ERROR_OVERRUN   = (1 << 0), /**< fifo overrun           */
    WM_DRV_UART_ERROR_PARITY    = (1 << 1), /**< parity error           */
    WM_DRV_UART_ERROR_FRAMING   = (1 << 2), /**< frame error            */
    WM_DRV_UART_BREAK           = (1 << 3), /**< break event            */
    WM_DRV_UART_ERROR_COLLISION = (1 << 4), /**< 485 half duplex error  */
} wm_drv_uart_evt_error_t;
/**
 * @}
 */

/**
 * @defgroup WM_UART_Structs WM UART Structs
 * @brief WinnerMicro UART Structs
 */

/**
 * @addtogroup WM_UART_Structs
 * @{
 */

/**
 * @struct wm_drv_uart_evt_t
 * @brief Event information type
 */
typedef struct {
    wm_drv_uart_evt_type_t type; /**< Event type                         */
    union {                      /**< Event Additional information       */
        struct {
            uint32_t len; /**< rx data len or tx free buffer size       */
        };
        struct {
            wm_drv_uart_evt_error_t error; /**< rx error                 */
        };
    };
} wm_drv_uart_evt_t;

/**
 * @struct wm_drv_uart_config_t
 * @brief Uart configuration
 */
typedef struct {
    wm_uart_baudrate_t baudrate;   /**< baudrate     */
    wm_uart_data_bits_t data_bits; /**< data bits    */
    wm_uart_stop_bits_t stop_bits; /**< stop bits    */
    wm_uart_parity_t parity;       /**< parity type  */
    wm_uart_flowctrl_t flow_ctrl;  /**< flow ctrl    */
} wm_drv_uart_config_t;

/**
 * @}
 */

/**
 * @defgroup WM_DRV_UART_TYPEs WM DRV Uart TYPEs
 * @brief WinnerMicro DRV Uart TYPEs
 */

/**
 * @addtogroup WM_DRV_UART_TYPEs
 * @{
 */

/**
 * @typedef wm_drv_uart_callback_t
 * @brief Uart callback type
 * @note Do not call printf,wm_log_xxx and other uart functions in the interrupt function
 */
typedef void (*wm_drv_uart_callback_t)(wm_device_t *dev, wm_drv_uart_evt_t *evt, void *user_data);
/**
 * @}
 */

/**
 * @defgroup WM_DRV_UART_APIs WM DRV Uart APIs
 * @brief WinnerMicro DRV Uart APIs
 */

/**
 * @addtogroup WM_DRV_UART_APIs
 * @{
 */

/**
 * @brief  initialize uart driver
 *
 * @param[in]  dev_name      uart name: [uart0,uart1,uart2,uart3,uart4,uart5], it must same to the name in device table
 * @param[in]  rx_buf_size    rx buffer size, WM_DRV_UART_RX_MIN_BUF_SIZE <= rx_buf_size < 64K
 *                            The more baudrate set , the more rx buffer size needed. At least 4K when the baudrate is 2M
 * @param[in]  tx_buf_size    tx buffer size, 0 or WM_DRV_UART_TX_MIN_BUF_SIZE <= tx_buf_size < 64K
 *                            When the wm_drv_uart_write interface has enough tx buffer space, it will return immediately
 *                            after copying the data to the tx buffer. If you need the write interface to return quickly,
 *                            you need to set the TX buffer.
 * @return
 *         - !NULL : Initialize success or initialized before
 *         - NULL :  Param error or initialize failed
 * @note Repeated initialization will return NULL.
 */
wm_device_t *wm_drv_uart_init(const char *dev_name, uint32_t rx_buf_size, uint32_t tx_buf_size);

/**
  * @brief  deinitialize uart driver
  *
  * @param[in] dev  uart device,type of  @ref wm_device_t *
  *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM： invalid param
  */
int wm_drv_uart_deinit(wm_device_t *dev);

/**
  * @brief  control uart driver by command
  *
  * @param[in] dev  uart device,type of  @ref wm_device_t *
  * @param[in] cmd  uart ioctrl command.
  * @param[in] param  command param.
  * @param[inout] arg  command argument.
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM： invalid param
  */
int wm_drv_uart_ioctrl(wm_device_t *dev, int cmd, uint32_t param, void *arg);

/**
  * @brief  register callback to receive uart event
  *
  * @param[in] dev  uart device,type of  @ref wm_device_t *
  * @param[in] cb  user callback fucntion, type of  @ref wm_drv_uart_callback_t
  * @param[in] user_data  user data, it will transfer to user in callback.
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM： invalid param
  */
int wm_drv_uart_register_callback(wm_device_t *dev, wm_drv_uart_callback_t cb, void *user_data);

/**
  * @brief  write data to uart port
  *
  * @param[in] dev  uart device,type of  @ref wm_device_t *
  * @param[in] data  data to be send
  * @param[in] size  the data size.
  * @return
  *    - < 0: WM_ERR_INVALID_PARAM, param error.
  *    - > 0: Real wirte bytes.
  * @note If tx_buf_size is specified during initialization,
  * the data will be copied to the buffer until all copies can be completed before returning.
  * If tx_buf_size is not specified, data is transferred directly,
  * and then returned after the transfer is completed
  * @warning If tx_buf_size is not specified,the data address is on flash and
  * the uart in the device table is configured with DMA mode, it will not output correctly.
  * @warning Call printing in the interrupt. It will use polling mode to put character to
  * hardware TX fifo, the interrup will be blocked until print end. So it is not recommended to
  * print in interrupt processing function because too much printing in the interrupt affects
  * the ability of other tasks to be scheduled in time. Each callback of driver module is
  * called from the interrupt, and it is not recommended to print in the driver callback.
  */
int wm_drv_uart_write(wm_device_t *dev, const uint8_t *data, uint32_t size);

/**
  * @brief  read data from uart port
  *
  * @param[in] dev  uart device,type of  @ref wm_device_t *
  * @param[out] buf  buffer for receiving data
  * @param[in] size  the buffer size.
  * @param[in] timeout_ms  receiving timeout.
  * @return
  *    - < 0: WM_ERR_INVALID_PARAM, param error.
  *    - >= 0: Real read bytes.
  * @note the data will be copied from the receiving buffer.
  * There are three conditions that will trigger the return:
  * a: The read length reaches the length of the buffer size.
  * b: The read time reaches the end of the timeout time.
  * c: The other end does not send continuously then the hardware trigger a rx fifo timeout.
  */
int wm_drv_uart_read(wm_device_t *dev, uint8_t *buf, uint32_t size, uint32_t timeout_ms);

/**
  * @brief  Get RX data pointer and size from buffer
  *
  * @param[in] dev  uart device,type of  @ref wm_device_t *
  * @param[out] buf  data pointer from buffer
  * @param[out] size  the continuous data size
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM： invalid param
  *    - WM_ERR_FAILED： no data in the driver buffer
  * @warning The receive buffer pointer and size must release by wm_drv_uart_release_data.
  * @warning The internal RX buffer will be divided into four sub buffers. Sometimes the received data packet
  *     will be divided into two blocks and put into two sub buffers. In this case, the interface needs to
  *     call twice to get the complete data.
  */
int wm_drv_uart_receive_data(wm_device_t *dev, uint8_t **buf, uint32_t *size);

/**
  * @brief  Release RX data pointer and size from buffer
  *
  * @param[in] dev  uart device,type of  @ref wm_device_t *
  * @param[in] buf  data pointer from buffer
  * @param[in] size  the continuous data size
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM： invalid param
  */
int wm_drv_uart_release_data(wm_device_t *dev, uint8_t *buf, uint32_t size);

/**
  * @brief  Set uart config, include baudrate,data bits,stop bits, parity, flow ctrl
  *
  * @param[in] dev  uart device,type of  @ref wm_device_t *
  * @param[in] config  uart config, type of  @ref wm_drv_uart_config_t *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM： invalid param
  */
int wm_drv_uart_set_config(wm_device_t *dev, wm_drv_uart_config_t *config);

/**
  * @brief  Get uart config, include baudrate,data bits,stop bits, parity, flow ctrl
  *
  * @param[in] dev  uart device,type of  @ref wm_device_t *
  * @param[out] config  uart config, type of  @ref wm_drv_uart_config_t *
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM： invalid param
  */
int wm_drv_uart_get_config(wm_device_t *dev, wm_drv_uart_config_t *config);

/**
  * @brief  Set uart baudrate
  *
  * @param[in] dev  uart device,type of  @ref wm_device_t *
  * @param[in] baudrate  uart baudrate, only support the value enumeration in  @ref wm_uart_baudrate_t
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM： invalid param
  */
int wm_drv_uart_set_baudrate(wm_device_t *dev, uint32_t baudrate);

/**
  * @brief  Get uart baudrate
  *
  * @param[in] dev  uart device,type of  @ref wm_device_t *
  * @param[out] baudrate  uart baudrate, type of  @ref wm_uart_baudrate_t
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM： invalid param
  */
int wm_drv_uart_get_baudrate(wm_device_t *dev, uint32_t *baudrate);

/**
  * @brief  Set uart data bits
  *
  * @param[in] dev  uart device,type of  @ref wm_device_t *
  * @param[in] data_bits  uart data_bits, type of  @ref wm_uart_data_bits_t
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM： invalid param
  */
int wm_drv_uart_set_data_bits(wm_device_t *dev, wm_uart_data_bits_t data_bits);

/**
  * @brief Get uart data bits
  *
  * @param[in] dev  uart device,type of  @ref wm_device_t *
  * @param[out] data_bits  uart data_bits, type of  @ref wm_uart_data_bits_t
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM： invalid param
  */
int wm_drv_uart_get_data_bits(wm_device_t *dev, wm_uart_data_bits_t *data_bits);

/**
  * @brief Set uart stop bits
  *
  * @param[in] dev  uart device,type of  @ref wm_device_t *
  * @param[in] stop_bits  uart stop_bits, type of  @ref wm_uart_stop_bits_t
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM： invalid param
  */
int wm_drv_uart_set_stop_bits(wm_device_t *dev, wm_uart_stop_bits_t stop_bits);

/**
  * @brief Get uart stop bits
  *
  * @param[in] dev  uart device,type of  @ref wm_device_t *
  * @param[out] stop_bits  uart stop_bits, type of  @ref wm_uart_stop_bits_t
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM： invalid param
  */
int wm_drv_uart_get_stop_bits(wm_device_t *dev, wm_uart_stop_bits_t *stop_bits);

/**
  * @brief Set uart parity
  *
  * @param[in] dev  uart device,type of  @ref wm_device_t *
  * @param[in] parity  uart parity, type of  @ref wm_uart_parity_t
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM： invalid param
  */
int wm_drv_uart_set_parity(wm_device_t *dev, wm_uart_parity_t parity);

/**
  * @brief Get uart parity
  *
  * @param[in] dev  uart device,type of  @ref wm_device_t *
  * @param[out] parity  uart parity, type of  @ref wm_uart_parity_t
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM： invalid param
  */
int wm_drv_uart_get_parity(wm_device_t *dev, wm_uart_parity_t *parity);

/**
  * @brief Set uart flow_ctrl
  *
  * @param[in] dev  uart device,type of  @ref wm_device_t *
  * @param[in] flow_ctrl  uart flow_ctrl, type of  @ref wm_uart_flowctrl_t
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM： invalid param
  */
int wm_drv_uart_set_flow_ctrl(wm_device_t *dev, wm_uart_flowctrl_t flow_ctrl);

/**
  * @brief Get uart flow_ctrl
  *
  * @param[in] dev  uart device,type of  @ref wm_device_t *
  * @param[out] flow_ctrl  uart flow_ctrl, type of  @ref wm_uart_flowctrl_t
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - WM_ERR_INVALID_PARAM： invalid param
  */
int wm_drv_uart_get_flow_ctrl(wm_device_t *dev, wm_uart_flowctrl_t *flow_ctrl);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_DRV_UART_H__ */

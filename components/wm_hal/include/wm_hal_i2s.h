/**
 * @file wm_hal_i2s.h
 *
 * @brief HAL_I2S Module
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

#ifndef __WM_HAL_I2S_H__
#define __WM_HAL_I2S_H__

#include <stdint.h>
#include "wm_types.h"
#include "wm_ll_i2s.h"
#include "wm_i2s_reg_struct.h"
#include "wm_error.h"
#include "wm_irq.h"
#include "wm_hal_dma.h"
#include "wm_soc_cfgs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_HAL_I2S_Macros WM HAL_I2S Macros
 * @brief WinnerMicro HAL_I2S Macros
 */

/**
 * @addtogroup WM_HAL_I2S_Macros
 * @{
 */

/**
 * @}
 */

/**
 * @defgroup WM_HAL_I2S_Enumerations WM HAL_I2S Enumerations
 * @brief WinnerMicro HAL_I2S Enumerations
 */

/**
 * @addtogroup WM_HAL_I2S_Enumerations
 * @{
 */

/**
 * @brief index for desc to indicate that which type does this desc belong to
 */
typedef enum wm_hal_i2s_tr_type {
    WM_HAL_I2S_TR_TX, /**< Transmit data in the output direction. */
    WM_HAL_I2S_TR_RX, /**< Transmit data in the input direction. */
    WM_HAL_I2S_TR_MAX
} wm_hal_i2s_tr_type_t;

/**
 * @}
 */

/**
 * @defgroup WM_HAL_I2S_Unions WM HAL_I2S Unions
 * @brief WinnerMicro HAL_I2S Unions
 */

/**
 * @addtogroup WM_HAL_I2S_Unions
 * @{
 */

/**
 * @brief xxx
 *
 * xxx
 */

/**
 * @}
 */

/**
 * @defgroup WM_HAL_I2S_Structures WM HAL_I2S Structures
 * @brief WinnerMicro HAL_I2S Structures
 */

/**
 * @addtogroup WM_HAL_I2S_Structures
 * @{
 */

/**
 * @brief The TX (transmit) and RX (receive) completion interrupts
 * invoke the notification with the parameter structure to the driver layer.
 *
 */
typedef struct wm_hal_i2s_trx_param {
    void *start_buf;    /**< Used to check if the first of multiple completed buffers is expected. */
    int len;            /**< the actual length of received buffers */
    int nodes_consumed; /**< the number of completed buffers(tx/rx) */
    int slots_remained; /**< the idle DMA desc can be used for next loop  */
} wm_hal_i2s_trx_param_t;

/**
 * @brief index for desc to indicate that which type does this desc belong to
 *
 * @param[in] master_dev the uper layer(driver) device handle
 * @param[in] param @ref wm_hal_i2s_trx_param_t
 *
 * @note Under normal circumstances, len is rx_pkt_len,but when the length of
 * the last packet of data received is insufficient, len represents the actual valid data.
 */
typedef void (*trx_cb)(void *master_dev, wm_hal_i2s_trx_param_t *param);

/**
 * @brief Used to notify the driver layer of the interrupt callback for the
 * completion of transmission and reception.
 *
 */
typedef void (*irq_cb)(void *master_dev, uint32_t flag);

/**
 * @brief I2S device configuration items
 */
typedef struct {
    enum wm_i2s_mode mode;                             /**< I2S role mode */
    enum wm_i2s_dir dir;                               /**< I2S xfer direction */
    enum wm_i2s_std std;                               /**< I2S protocol standard */
    enum wm_i2s_fmt fmt;                               /**< I2S frame format standard */
    enum wm_i2s_chan_type ctype;                       /**< if use stereo */
    enum wm_i2s_xfer_type xtype;                       /**< the xfer type */
    uint8_t dma_ch[WM_HAL_I2S_TR_MAX];                 /**< driver request dma channel and pass to HAL */
    uint8_t desc_node_depth[WM_HAL_I2S_TR_MAX];        /**< the count of dma desc nodes on the list */
    uint32_t desc_rxbuf_size;                          /**< the max size buffer belongs to rx dma desc nodes */
    void *(*desc_calloc)(size_t nelem, size_t elsize); /**< the function used to calloc dma desc */
    void (*desc_free)(void *desc);                     /**< the function used to free dma desc */
    trx_cb cb[WM_HAL_I2S_TR_MAX];
} wm_hal_i2s_cfg_t;

/**
 * @brief HAL I2S device private storage
 */
typedef struct {
    wm_hal_i2s_cfg_t cfg;                       /**< for configuration storage */
    wm_hal_dma_desc_t *desc[WM_HAL_I2S_TR_MAX]; /**< tx/rx dma desc, depth define by user */
    bool desc_en[WM_HAL_I2S_TR_MAX];            /**< if DMA is running xfer desc */
    uint8_t desc_cid[WM_HAL_I2S_TR_MAX];        /**< checked index */
    uint8_t desc_cnt[WM_HAL_I2S_TR_MAX];        /**< valid buffer count in list */
    uint8_t desc_start_cnt[WM_HAL_I2S_TR_MAX];  /**< temp, remove, indicate that the dma has been (re)start */
    uint32_t desc_push_cnt[WM_HAL_I2S_TR_MAX];  /**< total xfer buffer counts */
    uint32_t xfer_cnt[WM_HAL_I2S_TR_MAX];       /**< total xfer buffer counts */
    uint32_t rx_flush_total;                    /**< the total flush times to get the remained data */
    uint32_t rx_flush_valid;                    /**< the valid flush times to get the remained data */
    uint32_t rx_flush_bytes;                    /**< The length of data obtained through the flush method. */
} wm_hal_i2s_priv_t;

/**
 * @brief I2S device descriptor in HAL layer
 */
typedef struct {
    void *master_dev;          /**< which parent device does this device belong to */
    wm_irq_no_t irq_num;       /**< I2S IRQ number */
    wm_i2s_reg_t *reg_dev;     /**< register addr of I2S */
    wm_hal_i2s_cfg_t *cfg;     /**< I2S hal configuration point to cfg in priv */
    wm_hal_dma_dev_t *dma_dev; /**< DMA hal device */
    wm_hal_i2s_priv_t priv;    /**< private data section */
} wm_hal_i2s_dev_t;

/**
 * @}
 */

/**
 * @defgroup WM_HAL_I2S_Type_Definitions WM HAL_I2S Type Definitions
 * @brief WinnerMicro HAL_I2S Type Definitions
 */

/**
 * @addtogroup WM_HAL_I2S_Type_Definitions
 * @{
 */

/**
 * @brief Initializes the HAL I2S device.
 *
 * This function initializes the HAL I2S device using the provided configuration.
 * It sets up the device registers, DMA descriptors, and other necessary resources.
 *
 * @param[in, out] dev: A pointer to the HAL I2S device structure.
 * @param[in] cfg: A pointer to the I2S configuration structure.
 *
 * @return
 *    - WM_ERR_SUCCESS: The initialization was successful.
 *    - WM_ERR_INVALID_PARAM: Invalid argument(s) were provided.
 *    - Others: Initialization failed due to an unknown error.
 */
int wm_hal_i2s_init(wm_hal_i2s_dev_t *dev, wm_hal_i2s_cfg_t *cfg);

/**
 * @brief Deinitializes the HAL I2S device.
 *
 * This function deinitializes the HAL I2S device, releasing all resources allocated during initialization.
 * It stops any ongoing DMA transfers, resets the device registers, and frees any allocated memory.
 *
 * @param[in] dev: A pointer to the HAL I2S device structure.
 *
 * @return
 *    - WM_ERR_SUCCESS: The deinitialization was successful.
 *    - WM_ERR_INVALID_PARAM: Invalid argument(s) were provided.
 *    - Others: Deinitialization failed due to an unknown error.
 */
int wm_hal_i2s_deinit(wm_hal_i2s_dev_t *dev);

/**
 * @brief Sets the zero-crossing enable flag for the left channel of HAL I2S device.
 *
 * This function sets the zero-crossing enable flag for the HAL I2S device.
 * When enabled, the I2S module performs correlation detection on two data
 * symbol bits for the left audio channel. In the event of a reversal change,
 * the next data is forcibly set to 0 to prevent noise and other issues that
 * may arise from zero crossings.
 *
 * @param[in] dev: A pointer to the HAL I2S device structure.
 * @param[in] en: A boolean value indicating whether to enable (true) or
 * disable (false) the zero-crossing feature.
 *
 * @return None.
 */
void wm_hal_i2s_set_left_zero_cross_en(wm_hal_i2s_dev_t *dev, bool en);

/**
 * @brief Sets the zero-crossing enable flag for the right channel of HAL I2S device.
 *
 * This function sets the zero-crossing enable flag for the HAL I2S device.
 * When enabled, the I2S module performs correlation detection on two data
 * symbol bits for the right audio channel. In the event of a reversal change,
 * the next data is forcibly set to 0 to prevent noise and other issues that
 * may arise from zero crossings.
 *
 * @param[in] dev: A pointer to the HAL I2S device structure.
 * @param[in] en: A boolean value indicating whether to enable (true) or
 * disable (false) the zero-crossing feature.
 *
 * @return None.
 */
void wm_hal_i2s_set_right_zero_cross_en(wm_hal_i2s_dev_t *dev, bool en);

/**
 * @brief Sets the mono left enable flag for the HAL I2S device.
 *
 * This function sets the mono left enable flag for the HAL I2S device.
 * When enabled, the device will process the left audio channel as mono.
 *
 * @param[in] dev: A pointer to the HAL I2S device structure.
 * @param[in] en: A boolean value indicating whether to enable (true) or disable (false) the mono left feature.
 *
 * @return None.
 */
void wm_hal_i2s_set_mono_left_en(wm_hal_i2s_dev_t *dev, bool en);

/**
 * @brief Sets the transmit enable flag for the HAL I2S device.
 *
 * This function sets the transmit enable flag for the HAL I2S device.
 * When enabled, the device will start transmitting data.
 *
 * @param[in] dev: A pointer to the HAL I2S device structure.
 * @param[in] en: A boolean value indicating whether to enable (true) or disable (false) the transmit feature.
 *
 * @return None.
 */
void wm_hal_i2s_set_tx_en(wm_hal_i2s_dev_t *dev, bool en);

/**
 * @brief Starts the DMA transmission for the HAL I2S device.
 *
 * This function starts the DMA transmission for the HAL I2S device.
 * It configures the DMA controller to transfer data from the device's
 * transmit buffer to the I2S peripheral.
 *
 * @param[in] dev: A pointer to the HAL I2S device structure.
 *
 * @return
 *    - WM_ERR_SUCCESS: The DMA transmission was successfully started.
 *    - WM_ERR_INVALID_PARAM: Invalid argument(s) were provided.
 *    - WM_ERR_BUSY: The DMA transmission is already in progress.
 *    - WM_ERR_DMA_CONFIG: Failed to configure the DMA controller.
 *    - WM_ERR_DMA_START: Failed to start the DMA transmission.
 */
int wm_hal_i2s_tx_dma_start(wm_hal_i2s_dev_t *dev);

/**
 * @brief Stops the DMA transmission for the HAL I2S device.
 *
 * This function stops the DMA transmission for the HAL I2S device.
 * It configures the DMA controller to stop transferring data from
 * the device's transmit buffer to the I2S peripheral.
 *
 * @param[in] dev: A pointer to the HAL I2S device structure.
 *
 * @return
 *    - WM_ERR_SUCCESS: The DMA transmission was successfully stopped.
 *    - WM_ERR_INVALID_PARAM: Invalid argument(s) were provided.
 *    - WM_ERR_DMA_STOP: Failed to stop the DMA transmission.
 */
int wm_hal_i2s_tx_dma_stop(wm_hal_i2s_dev_t *dev);

/**
 * @brief Appends a buffer to the DMA transmit list for the HAL I2S device.
 *
 * This function appends a buffer to the DMA transmit list for the HAL I2S device.
 * It adds the specified buffer to the list of buffers that will be transmitted using DMA.
 *
 * @param[in] dev: A pointer to the HAL I2S device structure.
 * @param[in] buf: A pointer to the buffer that will be transmitted.
 * @param[in] len: The length of the buffer in bytes.
 *
 * @return
 *    - WM_ERR_SUCCESS: The buffer was successfully appended to the DMA transmit list.
 *    - WM_ERR_INVALID_PARAM: Invalid argument(s) were provided.
 *    - WM_ERR_DMA_CONFIG: Failed to configure the DMA controller.
 *    - WM_ERR_DMA_START: Failed to start the DMA transmission.
 */
int wm_hal_i2s_tx_dma_buf_append(wm_hal_i2s_dev_t *dev, void *buf, int len);

/**
 * @brief Sets the transmit clock inversion flag for the HAL I2S device.
 *
 * This function sets the transmit clock inversion flag for the HAL I2S device.
 * When enabled, the device will invert the clock signal for the transmit audio signal.
 *
 * @param[in] dev: A pointer to the HAL I2S device structure.
 * @param[in] en: A boolean value indicating whether to enable (true) or disable (false) the transmit clock inversion feature.
 *
 * @return None.
 */
void wm_hal_i2s_set_tx_clock_inv_en(wm_hal_i2s_dev_t *dev, bool en);

/**
 * @brief Sets the transmit mute enable flag for the HAL I2S device.
 *
 * This function sets the transmit mute enable flag for the HAL I2S device.
 * When enabled, the device will mute the transmit audio signal.
 *
 * @param[in] dev: A pointer to the HAL I2S device structure.
 * @param[in] mute: A boolean value indicating whether to enable (true) or disable (false) the transmit mute feature.
 *
 * @return None.
 */
void wm_hal_i2s_set_tx_mute_en(wm_hal_i2s_dev_t *dev, bool mute);

/**
 * @brief Checks if the TX DMA is in a zombie state.
 *
 * This function checks if the TX DMA is in a zombie state, meaning that
 * all DMA descriptors have been consumed and there are no more buffers
 * to transmit. This can occur when a user packet experiences blockage or a
 * temporary flow interruption, causing DMA to consume the entire driver cache.
 *
 * @param[in] dev: A pointer to the HAL I2S device structure.
 *
 * @return A boolean value indicating whether the TX DMA is in a zombie state (true) or not (false).
 */
bool wm_hal_i2s_is_txdma_zombie(wm_hal_i2s_dev_t *dev);

/**
 * @brief Checks if the RX DMA is in a zombie state.
 *
 * This function checks if the RX DMA is in a zombie state, meaning that
 * all DMA descriptors have been consumed and there are no more buffers
 * to receive. This can occur when a user packet experiences blockage or a
 * temporary flow interruption, causing DMA to consume the entire driver cache.
 *
 * @param[in] dev: A pointer to the HAL I2S device structure.
 *
 * @return A boolean value indicating whether the RX DMA is in a zombie state (true) or not (false).
 */
bool wm_hal_i2s_is_rxdma_zombie(wm_hal_i2s_dev_t *dev);

/**
 * @brief Pauses the DMA transmission for the HAL I2S device.
 *
 * This function pauses the DMA transmission for the HAL I2S device.
 * It temporarily stops the DMA controller from transferring data from the device's
 * transmit buffer to the I2S peripheral.
 *
 * @param[in] dev: A pointer to the HAL I2S device structure.
 *
 * @return
 *    - WM_ERR_SUCCESS: The DMA transmission was successfully paused.
 *    - WM_ERR_INVALID_PARAM: Invalid argument(s) were provided.
 *    - WM_ERR_DMA_PAUSE: Failed to pause the DMA transmission.
 */
int wm_hal_i2s_tx_pause(wm_hal_i2s_dev_t *dev);

/**
 * @brief Resumes the DMA transmission for the HAL I2S device.
 *
 * This function resumes the DMA transmission for the HAL I2S device.
 * It starts the DMA controller from where it left off, continuing to transfer data from the device's
 * transmit buffer to the I2S peripheral.
 *
 * @param[in] dev: A pointer to the HAL I2S device structure.
 *
 * @return
 *    - WM_ERR_SUCCESS: The DMA transmission was successfully resumed.
 *    - WM_ERR_INVALID_PARAM: Invalid argument(s) were provided.
 *    - WM_ERR_DMA_RESUME: Failed to resume the DMA transmission.
 */
int wm_hal_i2s_tx_resume(wm_hal_i2s_dev_t *dev);

/**
 * @brief Sets the receive enable flag for the HAL I2S device.
 *
 * This function sets the receive enable flag for the HAL I2S device.
 * When enabled, the device will start receiving data.
 *
 * @param[in] dev: A pointer to the HAL I2S device structure.
 * @param[in] en: A boolean value indicating whether to enable (true) or disable (false) the receive feature.
 *
 * @return None.
 */
void wm_hal_i2s_set_rx_en(wm_hal_i2s_dev_t *dev, bool en);

/**
 * @brief Starts the DMA reception for the HAL I2S device.
 *
 * This function starts the DMA reception for the HAL I2S device.
 * It configures the DMA controller to transfer data from the device's
 * receive buffer to the I2S peripheral.
 *
 * @param[in] dev: A pointer to the HAL I2S device structure.
 *
 * @return
 *    - WM_ERR_SUCCESS: The DMA reception was successfully started.
 *    - WM_ERR_INVALID_PARAM: Invalid argument(s) were provided.
 *    - WM_ERR_BUSY: The DMA reception is already in progress.
 *    - WM_ERR_DMA_CONFIG: Failed to configure the DMA controller.
 *    - WM_ERR_DMA_START: Failed to start the DMA reception.
 */
int wm_hal_i2s_rx_dma_start(wm_hal_i2s_dev_t *dev);

/**
 * @brief Starts the DMA reception for the HAL I2S device.
 *
 * This function starts the DMA reception for the HAL I2S device.
 * It configures the DMA controller to transfer data from the device's
 * receive buffer to the I2S peripheral.
 *
 * @param[in] dev: A pointer to the HAL I2S device structure.
 * @param[in] buf: A pointer to the buffer that will be received.
 * @param[in] len: The length of the buffer in bytes.
 *
 * @return
 *    - WM_ERR_SUCCESS: The DMA reception was successfully started.
 *    - WM_ERR_INVALID_PARAM: Invalid argument(s) were provided.
 *    - WM_ERR_BUSY: The DMA reception is already in progress.
 *    - WM_ERR_DMA_CONFIG: Failed to configure the DMA controller.
 *    - WM_ERR_DMA_START: Failed to start the DMA reception.
 */
int wm_hal_i2s_rx_dma(wm_hal_i2s_dev_t *dev, void *buf, int len);

/**
 * @brief Stops the DMA reception for the HAL I2S device.
 *
 * This function stops the DMA reception for the HAL I2S device.
 * It configures the DMA controller to stop transferring data from
 * the device's receive buffer to the I2S peripheral.
 *
 * @param[in] dev: A pointer to the HAL I2S device structure.
 *
 * @return
 *    - WM_ERR_SUCCESS: The DMA reception was successfully stopped.
 *    - WM_ERR_INVALID_PARAM: Invalid argument(s) were provided.
 *    - WM_ERR_DMA_STOP: Failed to stop the DMA reception.
 */
int wm_hal_i2s_rx_dma_stop(wm_hal_i2s_dev_t *dev);

/**
 * @brief Flushes the receive FIFO of the HAL I2S device.
 *
 * This function flushes the receive FIFO of the HAL I2S device.
 * It resets the receive FIFO to its initial state, discarding any pending data.
 *
 * @param[in] dev: A pointer to the HAL I2S device structure.
 *
 * @return
 *    - WM_ERR_SUCCESS: The receive FIFO was successfully flushed.
 *    - WM_ERR_INVALID_OPERATION: The receive FIFO cannot be flushed while DMA is in progress.
 */
int wm_hal_i2s_dma_rx_flush(wm_hal_i2s_dev_t *dev);

/**
 * @brief Appends a buffer to the DMA receive list for the HAL I2S device.
 *
 * This function appends a buffer to the DMA receive list for the HAL I2S device.
 * It adds the specified buffer to the list of buffers that will be received using DMA.
 *
 * @param[in] dev: A pointer to the HAL I2S device structure.
 * @param[in] buf: A pointer to the buffer that will be received.
 * @param[in] len: The length of the buffer in bytes.
 *
 * @return
 *    - WM_ERR_SUCCESS: The buffer was successfully appended to the DMA receive list.
 *    - WM_ERR_INVALID_PARAM: Invalid argument(s) were provided.
 *    - WM_ERR_DMA_CONFIG: Failed to configure the DMA controller.
 *    - WM_ERR_DMA_START: Failed to start the DMA reception.
 */
int wm_hal_i2s_rx_dma_buf_append(wm_hal_i2s_dev_t *dev, void *buf, int len);

/**
 * @brief Sets the receive clock inversion flag for the HAL I2S device.
 *
 * This function sets the receive clock inversion flag for the HAL I2S device.
 * When enabled, the device will invert the clock signal for the receive audio signal.
 *
 * @param[in] dev: A pointer to the HAL I2S device structure.
 * @param[in] en: A boolean value indicating whether to enable (true) or disable (false) the receive clock inversion feature.
 *
 * @return None.
 */
void wm_hal_i2s_set_rx_clock_inv_en(wm_hal_i2s_dev_t *dev, bool en);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_HAL_I2S_H__ */

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
 * @defgroup WM_HAL_I2S_Enums WM HAL_I2S Enums
 * @brief WinnerMicro HAL_I2S Enums
 */

/**
 * @addtogroup WM_HAL_I2S_Enums
 * @{
 */
typedef enum {
    WM_HAL_I2S_EVENT_TX_DONE,  /**< send one packet end                     */
    WM_HAL_I2S_EVENT_RX_READY, /**< one packet receive end                  */
    WM_HAL_I2S_EVENT_RX_DONE,  /**< hardware detect no rx data for a moment */
} wm_hal_i2s_event_type_t;
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
 * @brief The TX (transmit) and RX (receive) completion event
 * @note The attribute len maybe zero when rx receive done
 */
typedef struct {
    wm_hal_i2s_event_type_t type; /**< curernt event type                    */
    uint8_t *buf;                 /**< curernt transfer end buffer           */
    int len;                      /**< the actual length of xfer buffer,
                                        only notify remove buffer if len = 0 */
    void *priv;                   /**< Not use by hal                        */
} wm_hal_i2s_event_t;

/**
 * @brief index for desc to indicate that which type does this desc belong to
 *
 * @param[in] master_dev the uper layer(driver) device handle
 * @param[in] event @ref wm_hal_i2s_event_t
 */
typedef void (*wm_hal_i2s_callback_t)(void *master_dev, wm_hal_i2s_event_t *event);

/**
 * @brief I2S device configuration items
 */
typedef struct {
    enum wm_i2s_mode mode;             /**< I2S role mode             */
    enum wm_i2s_dir dir;               /**< I2S xfer direction        */
    enum wm_i2s_std std;               /**< I2S protocol standard     */
    enum wm_i2s_xfer_type xtype;       /**< the xfer type             */
    wm_hal_dma_dev_t *rx_dma;          /**< rx dma                    */
    wm_hal_dma_dev_t *tx_dma;          /**< tx dma                    */
    uint8_t rx_dma_ch;                 /**< rx dma  channel           */
    uint8_t tx_dma_ch;                 /**< tx dma  channel           */
    uint8_t rx_pkt_num;                /**< rx dma data chunk number  */
    uint8_t tx_pkt_num;                /**< tx dma data chunk number  */
    uint32_t rx_pkt_size;              /**< rx receive packet size    */
    wm_hal_i2s_callback_t rx_callback; /**< rx transfer done callback */
    wm_hal_i2s_callback_t tx_callback; /**< tx transfer done callback */
} wm_hal_i2s_cfg_t;

/**
 * @brief I2S device format items
 */
typedef struct {
    enum wm_i2s_bits bits;         /**< I2S frame format standard */
    enum wm_i2s_chan_type channel; /**< if use stereo             */
} wm_hal_i2s_fromat_t;

/**
 * @brief HAL I2S device transfer information
 */
typedef struct {
    wm_hal_dma_desc_t *r;    /**< DMA receiving chain read  */
    wm_hal_dma_desc_t *w;    /**< DMA receiving chain write */
    wm_hal_dma_desc_t *desc; /**< DMA receiving desc node   */
    uint32_t xfer_pack_cnt;  /**< transfer package count    */
    uint64_t xfer_size;      /**< transfer total size       */
} wm_hal_i2s_xfer_t;

/**
 * @brief I2S device descriptor in HAL layer
 */
typedef struct {
    void *master_dev;        /**< which parent device ,set by caller  */
    wm_i2s_reg_t *reg_dev;   /**< register addr of I2S ,set by caller */
    wm_irq_no_t irq_num;     /**< I2S IRQ number,set by caller        */
    wm_hal_i2s_cfg_t cfg;    /**< I2S hal configuration, iternal use  */
    wm_hal_i2s_fromat_t fmt; /**< I2S hal format                      */
    wm_hal_i2s_xfer_t rx;    /**< rx xfer inforamtion, iternal use    */
    wm_hal_i2s_xfer_t tx;    /**< tx xfer inforamtion, iternal use    */
} wm_hal_i2s_dev_t;

/**
 * @}
 */

/**
 * @defgroup WM_HAL_I2S_APIs WM HAL_I2S APIs
 * @brief WinnerMicro HAL_I2S APIs
 */

/**
 * @addtogroup WM_HAL_I2S_APIs
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
 * @brief Sets i2s format.
 *
 * @param[in] dev: A pointer to the HAL I2S device structure.
 * @param[in] bits: sample bits width.
 * @param[in] channel: sample channel type.
 *
 * @return None.
 */
int wm_hal_i2s_set_format(wm_hal_i2s_dev_t *dev, enum wm_i2s_bits bits, enum wm_i2s_chan_type channel);

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
int wm_hal_i2s_tx_start(wm_hal_i2s_dev_t *dev);

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
int wm_hal_i2s_tx_stop(wm_hal_i2s_dev_t *dev);

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
 * @brief Checks if the TX DMA is running or stoped.
 *
 * @param[in] dev: A pointer to the HAL I2S device structure.
 *
 * @return A boolean value indicating whether the TX DMA is running (true) or not (false).
 */
bool wm_hal_i2s_is_tx_dma_running(wm_hal_i2s_dev_t *dev);

/**
 * @brief Checks if the RX DMA is running or stoped.
 *
 * @param[in] dev: A pointer to the HAL I2S device structure.
 *
 * @return A boolean value indicating whether the RX DMA is running (true) or not (false).
 */
bool wm_hal_i2s_is_rx_dma_running(wm_hal_i2s_dev_t *dev);

/**
 * @brief Checks if the TX is enabled or not.
 *
 * @param[in] dev: A pointer to the HAL I2S device structure.
 *
 * @return A boolean value indicating whether the TX is enabled (true) or not (false).
 */
bool wm_hal_i2s_is_tx_enabled(wm_hal_i2s_dev_t *dev);

/**
 * @brief Checks if the RX DMA is enabled or not.
 *
 * @param[in] dev: A pointer to the HAL I2S device structure.
 *
 * @return A boolean value indicating whether the RX is enabled (true) or not (false).
 */
bool wm_hal_i2s_is_rx_enabled(wm_hal_i2s_dev_t *dev);

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
int wm_hal_i2s_rx_start(wm_hal_i2s_dev_t *dev);

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
int wm_hal_i2s_rx_stop(wm_hal_i2s_dev_t *dev);

/**
 * @brief Get the number of rx idle DMA descriptor number.
 *
 * @param[in] dev: A pointer to the HAL I2S device structure.
 *
 * @return
 *    >0: The number of rx idle DMA descriptor.
 *    =0: No idle DMA descriptor,can't append rx buffer.
 *    -1: fail
 */
int wm_hal_i2s_rx_dma_get_idle_desc_num(wm_hal_i2s_dev_t *dev);

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
 * @brief hal i2s rx pause.
 *
 * @param[in] dev: A pointer to the HAL I2S device structure.
 *
 * @return
 *    - WM_ERR_SUCCESS: The DMA transmission was successfully paused.
 *    - WM_ERR_INVALID_PARAM: Invalid argument(s) were provided.
 *    - WM_ERR_DMA_PAUSE: Failed to pause the DMA transmission.
 */
int wm_hal_i2s_rx_pause(wm_hal_i2s_dev_t *dev);

/**
 * @brief hal i2s rx resume.
 *
 * @param[in] dev: A pointer to the HAL I2S device structure.
 *
 * @return
 *    - WM_ERR_SUCCESS: The DMA transmission was successfully paused.
 *    - WM_ERR_INVALID_PARAM: Invalid argument(s) were provided.
 *    - WM_ERR_DMA_PAUSE: Failed to pause the DMA transmission.
 */
int wm_hal_i2s_rx_resume(wm_hal_i2s_dev_t *dev);

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
 * @brief dump hal i2s information.
 *
 *
 * @param[in] dev: A pointer to the HAL I2S device structure.
 *
 * @return None.
 */
int wm_hal_i2s_dump_info(wm_hal_i2s_dev_t *dev);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_HAL_I2S_H__ */

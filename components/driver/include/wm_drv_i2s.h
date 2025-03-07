/**
 * @file wm_drv_i2s.h
 *
 * @brief DRV_I2S Module
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

#ifndef __WM_DRV_I2S_H__
#define __WM_DRV_I2S_H__

#include <stddef.h>
#include <stdint.h>
#include "wmsdk_config.h"
#include "wm_error.h"
#include "wm_osal.h"
#include "wm_types.h"
#include "wm_dt.h"
#include "wm_hal_i2s.h"
#include "wm_list.h"
#include "wm_hal_dma.h"
#include "wm_soc_cfgs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_DRV_I2S_Macros WM DRV_I2S Macros
 * @brief WinnerMicro DRV_I2S Macros
 */

/**
 * @addtogroup WM_DRV_I2S_Macros
 * @{
 */

/**
 * @brief The number of TX and RX packets and the size boundary detection value.
 * The actual size is passed in the initialization parameters, but cannot exceed
 * these boundary values, how to use pkt_size and pkt num, @ref wm_drv_i2s_cfg_t*
 */
#define WM_DRV_I2S_RX_MIN_PKT_LEN 80    /**< RX: minimum packet size       */
#define WM_DRV_I2S_RX_MAX_PKT_LEN 15600 /**< RX: maximum packet size       */
#define WM_DRV_I2S_RX_MIN_PKT     2     /**< RX: minimum number of packets */
#define WM_DRV_I2S_RX_MAX_PKT     16    /**< RX: maximum number of packets */
#define WM_DRV_I2S_TX_MIN_PKT     2     /**< TX: minimum number of packets */
#define WM_DRV_I2S_TX_MAX_PKT     16    /**< TX: maximum number of packets */

/**
 * @}
 */

/**
 * @defgroup WM_DRV_I2S_Enumerations WM DRV_I2S Enumerations
 * @brief WinnerMicro DRV_I2S Enumerations
 */

/**
 * @addtogroup WM_DRV_I2S_Enumerations
 * @{
 */

/**
 * @brief command id for I2S IOCTL
 */
typedef enum {
    WM_DRV_I2S_CMD_SET_TX_BITSHIFT, /**< enable / disable tx clock phase shift mode (disable by default)              */
    WM_DRV_I2S_CMD_SET_RX_BITSHIFT, /**< enable / disable rx clock phase shift mode (disable by default)              */
    WM_DRV_I2S_CMD_SET_LZC,         /**< enable / disable left zero cross check for rx(disable by default)            */
    WM_DRV_I2S_CMD_SET_RZC,         /**< enable / disable right zero cross check for rx(disable by default)           */
    WM_DRV_I2S_CMD_SET_MAX
} wm_drv_i2s_ioctl_cmd_t;

/**
 * @brief sample rate supported by HW
 *
 * @note The following supported sampling rates may not result in an exact master
 * clock output under different configurations because, according to the formula:
 * clock_div = clock_master / (sample_rate * channels * bitwidth), it may not be
 * possible to obtain an integer value for clock_div. The default value of the
 * current I2S's clock_master is 160000000. Please refer to the RCC module
 * for the final effective value.
 * */
enum wm_drv_i2s_sample_rates {
    WM_DRV_I2S_SP_8000   = 8000,   /**< i2s sample rate set to 8KHz */
    WM_DRV_I2S_SP_11025  = 11025,  /**< i2s sample rate set to 11.025KHz */
    WM_DRV_I2S_SP_16000  = 16000,  /**< i2s sample rate set to 16KHz */
    WM_DRV_I2S_SP_22050  = 22050,  /**< i2s sample rate set to 22.05KHz */
    WM_DRV_I2S_SP_24000  = 24000,  /**< i2s sample rate set to 24KHz */
    WM_DRV_I2S_SP_32000  = 32000,  /**< i2s sample rate set to 32KHz */
    WM_DRV_I2S_SP_44100  = 44100,  /**< i2s sample rate set to 44.1KHz */
    WM_DRV_I2S_SP_47250  = 47250,  /**< i2s sample rate set to 47.25KHz */
    WM_DRV_I2S_SP_48000  = 48000,  /**< i2s sample rate set to 48KHz */
    WM_DRV_I2S_SP_50000  = 50000,  /**< i2s sample rate set to 50KHz */
    WM_DRV_I2S_SP_50400  = 50400,  /**< i2s sample rate set to 50.04KHz */
    WM_DRV_I2S_SP_96000  = 96000,  /**< i2s sample rate set to 96KHz */
    WM_DRV_I2S_SP_192000 = 192000, /**< i2s sample rate set to 192KHz */

    WM_DRV_I2S_SP_MAX,
};

/**
 * @brief i2s event type
 */
typedef enum {
    WM_DRV_I2S_EVENT_TX_DONE,  /**< send one packet end                     */
    WM_DRV_I2S_EVENT_RX_READY, /**< one packet receive end                  */
    WM_DRV_I2S_EVENT_RX_DONE,  /**< hardware detect no rx data for a moment */
} wm_drv_i2s_event_type_t;

/**
 * @}
 */

/**
 * @defgroup WM_DRV_I2S_Structures WM DRV_I2S Structures
 * @brief WinnerMicro DRV_I2S Structures
 */

/**
 * @addtogroup WM_DRV_I2S_Structures
 * @{
 */

/**
 * @brief The TX (transmit) and RX (receive) event
 * @note Event has the following cases
 *      WM_DRV_I2S_EVENT_TX_DONE    len >  0  notify user 1 pkt sent
 *      WM_DRV_I2S_EVENT_TX_DONE    len == 0  notify user to remove buffer when tx stoped
 *      WM_DRV_I2S_EVENT_RX_READY   len >  0  notify user 1 pkt ok received
 *      WM_DRV_I2S_EVENT_RX_READY   len == 0  notify user to remove buffer when rx stoped
 *      WM_DRV_I2S_EVENT_RX_DONE    len == 0  rx no data now, the peer send end
 */
typedef struct {
    wm_drv_i2s_event_type_t type; /**< curernt event type                    */
    uint8_t *buf;                 /**< curernt transfer end buffer           */
    int len;                      /**< the actual length of received data    */
    void *priv;                   /**< For user use, not use by i2s driver   */
} wm_drv_i2s_event_t;

/**
 * @brief configuration of I2S controller
 */
typedef struct {
    enum wm_i2s_mode mode;       /**< I2S role mode                                         */
    enum wm_i2s_dir dir;         /**< I2S xfer direction                                    */
    enum wm_i2s_std std;         /**< I2S protocol standard                                 */
    enum wm_i2s_xfer_type xtype; /**< the transfer type select from DMA or others           */
    uint32_t rx_pkt_size;        /**< The buffer size of each received packet [80 ~ 15360].
                                    Usually set to 10/20 millisecond transmission data size.
                                    4 bytes aligned.
                                    The larger the rx_pkt_size, the longer the delay of notifying
                                    the upper layer of processing, and the longer the delay of
                                    sound output*/
    uint8_t rx_pkt_num;          /**< Max number of the RX package receive buffer [2~16],
                                    recommended 4, can be large if rx_pkt_size is small     */
    uint8_t tx_pkt_num;          /**< Max number of the TX packages wait to send, [2~16]
                                    recommended 4, can be large if tx send pkt size is small*/

} wm_drv_i2s_cfg_t;

/**
 * @brief I2S format information
 */
typedef struct {
    uint32_t sample_rate_hz;     /**< the sample rate for the audio data     */
    enum wm_i2s_bits bits;       /**< I2S frame format standard             */
    enum wm_i2s_chan_type ctype; /**< the channel type, mono or stereo       */
} wm_drv_i2s_format_t;

/**
 * @}
 */

/**
 * @defgroup WM_DRV_I2S_Type_Definitions WM DRV_I2S Type Definitions
 * @brief WinnerMicro DRV_I2S Type Definitions
 */

/**
 * @addtogroup WM_DRV_I2S_Type_Definitions
 * @{
 */

/**
 * @brief callback type
 * @param[in] dev The I2S device.
 * @param[in] event event type, @ref wm_drv_i2s_event_t
 */
typedef int (*wm_drv_i2s_callback_t)(wm_device_t *dev, wm_drv_i2s_event_t *event);

/**
 * @}
 */

/**
 * @defgroup WM_DRV_I2S_Functions WM DRV_I2S Functions
 * @brief WinnerMicro DRV_I2S Functions
 */

/**
 * @addtogroup WM_DRV_I2S_Functions
 * @{
 */
/**
 * @brief Initializes the I2S driver with the given configuration.
 *
 * This function initializes the I2S driver with the provided configuration.
 * It sets up the I2S controller according to the specified parameters and
 * prepares the driver for data transfer.
 *
 * @param[in] name The name of the I2S device.
 * @param[in] cfg A pointer to the configuration structure for the I2S driver.
 *
 * @return
 *  - A pointer to the initialized I2S device.
 *  - NULL: If the initialization fails,
 *
 * @note I2S transmit and receive data will be valid on the rising edge of BCLK.
 * @warning For 24 bit sampling, it is necessary to expand 24 bits to 32 bits for each sample before sending,
 *      and all the high 8 bits are 0. After receiving, it is necessary to remove the high 8 bits
 *      and restore the samples to 24 bits.
 */
wm_device_t *wm_drv_i2s_init(const char *name, wm_drv_i2s_cfg_t *cfg);

/**
 * @brief Deinitializes the I2S driver and releases all associated resources.
 *
 * This function deinitializes the I2S driver and releases all associated resources.
 * It stops any ongoing data transfers, disables the I2S controller, and frees any allocated memory.
 *
 * @param[in] device A pointer to the I2S device to be deinitialized.
 *
 * @return
 *  - WM_ERR_SUCESS: If the deinitialization is successful.
 *  - others: If the deinitialization fails
 */
int wm_drv_i2s_deinit(wm_device_t *device);

/**
 * @brief set xfer audio format
 *
 * @param[in] device A pointer to the I2S device to be deinitialized.
 * @param[in] sample_rate_hz sample rate, unit Hz.
 * @param[in] bits the sample bits witdh
 * @param[in] channel channel type
 *
 * @return
 *  - WM_ERR_SUCESS: If the deinitialization is successful.
 *  - others: If the deinitialization fails
 * @note
 *      Called before starting read/write
 */
int wm_drv_i2s_set_format(wm_device_t *device, uint32_t sample_rate_hz, enum wm_i2s_bits bits, enum wm_i2s_chan_type channel);

/**
 * @brief set mclk, Usually output to codec for use
 *
 * @param[in] device A pointer to the I2S device to be deinitialized.
 * @param[in] mclk_hz mclk, unit Hz.
 *
 * @return
 *  - WM_ERR_SUCESS: If the deinitialization is successful.
 *  - others: If the deinitialization fails
 * @note
 *      Called before starting read/write
 */
int wm_drv_i2s_set_mclk(wm_device_t *device, uint32_t mclk_hz);

/**
 * @brief Asynchronously writes data to the I2S device.
 *
 * This function asynchronously writes data to the I2S device. The data is
 * stored in a buffer provided by the user and is written to the device when
 * the driver is ready. The user can register a callback function to be
 * notified when the data has been successfully transmitted.
 *
 * @param[in] device A pointer to the I2S device to which the data will be written.
 * @param[in] buf A pointer to the buffer containing the data to be written.
 * @param[in] len The length of the data in the buffer.
 *
 * @return
 *  - WM_ERR_SUCCESS: If the write operation is successful.
 *  - WM_ERR_NO_MEN: The DMA list is full, Can't append write buffer now, wait until the TX done event
 *  - others: If the write operation fails
 *
 * @note Please note that after the user's buffer is sent, the registered tx done callback
 * will be called to notify the user to complete operations such as releasing.
 *
 * @note The starting address of buf must be 4-byte aligned; otherwise, otherwise the driver will report an error.
 * @note The data length needs to 4bytes aligned , otherwise the driver will report an error.
 * @note The maximum transfer length is less than 64K.
 */
int wm_drv_i2s_write_async(wm_device_t *device, void *buf, int len);

/**
 * @brief Pauses the ongoing write operation to the I2S device.
 *
 * This function pauses the ongoing write operation to the I2S device.
 * It stops any further data transmission and prevents any new write operations.
 * The clock is not stoped if it is master mode. The peer side can receive dummy data continuously.
 * The buffer data is hold on.
 * Call `wm_drv_i2s_write_resume()` to resume.
 *
 * @param[in] device A pointer to the I2S device to which the data will be written.
 *
 * @return
 *  - WM_ERR_SUCCESS: If the write operation is successfully paused.
 *  - others: If the write operation fails
 */
int wm_drv_i2s_write_pause(wm_device_t *device);

/**
 * @brief Resumes the ongoing write operation to the I2S device.
 *
 * This function resumes the ongoing write operation to the I2S device.
 * It allows any previously paused write operations to continue data transmission.
 * The write operation must be paused using `wm_drv_i2s_write_pause()` before it can be resumed.
 *
 * @param[in] device A pointer to the I2S device to which the data will be written.
 *
 * @return
 *  - WM_ERR_SUCCESS: If the write operation is successfully resumed.
 *  - others: If the write operation fails
 */
int wm_drv_i2s_write_resume(wm_device_t *device);

/**
 * @brief Stops the ongoing write operation to the I2S device.
 *
 * This function stops the ongoing write operation to the I2S device.
 * It stops any further data transmission and prevents any new write operations
 * from being initiated.
 * All write data buffers are released.
 * The clock is stoped if it is master mode and duplex mode.
 *
 * @param[in] device A pointer to the I2S device to which the data will be written.
 *
 * @return
 *  - WM_ERR_SUCCESS: If the write operation is successfully stopped.
 *  - others: If the write operation fails
 */
int wm_drv_i2s_write_stop(wm_device_t *device);

/**
 * @brief Append a read buffer to DMA receive list.
 *
 * This function asynchronously reads data from the I2S device. The data is
 * stored in a buffer provided by the user and is read from the device when
 * the driver is ready. The user can register a callback function to be
 * notified when the data has been successfully received.
 *
 * @param[in] device A pointer to the I2S device from which the data will be read.
 * @param[in] buf buffer for recevie data, must 4 bytes align
 * @param[in] len recive buffer size, must equal to rx_pkt_size.
 *
 * @return
 *  - WM_ERR_SUCCESS: If the read operation is successful.
 *  - WM_ERR_NO_MEN: The DMA list is full, Can't append read buffer now, wait until the RX ready event
 *  - others: If the read operation fails
 *
 * @note When receiving is started, call rx_pkt_num times, and then call
 * once for each packet of data received. Do not execute in an interrupt
 */
int wm_drv_i2s_read_async(wm_device_t *device, void *buf, int len);

/**
 * @brief Pauses the ongoing read operation to the I2S device.
 *
 * This function pauses the ongoing read operation to the I2S device.
 * It stops any further data reception and prevents any new read operations
 * from being initiated until the read operation is resumed using
 * The receive buffers with data will not be freed
 * `wm_drv_i2s_read_resume()`.
 *
 * @param[in] device A pointer to the I2S device from which the data will be read.
 *
 * @return
 *  - WM_ERR_SUCCESS: If the read operation is successfully paused.
 *  - others: If the read operation fails
 */
int wm_drv_i2s_read_pause(wm_device_t *device);

/**
 * @brief Resumes the ongoing read operation to the I2S device.
 *
 * This function resumes the ongoing read operation to the I2S device.
 * It allows any previously paused read operations to continue data reception.
 * The read operation must be paused using `wm_drv_i2s_read_pause()` before it can be resumed.
 *
 * @param[in] device A pointer to the I2S device from which the data will be read.
 *
 * @return
 *  - WM_ERR_SUCCESS: If the read operation is successfully resumed.
 *  - others: If the read operation fails
 */
int wm_drv_i2s_read_resume(wm_device_t *device);

/**
 * @brief Stops the ongoing read operation to the I2S device.
 *
 * This function stops the ongoing read operation to the I2S device.
 * It stops any further data reception and prevents any new read operations
 * It will free the read buffers.
 * from being initiated.
 *
 * @param[in] device A pointer to the I2S device from which the data will be read.
 *
 * @return
 *  - WM_ERR_SUCCESS: If the read operation is successfully stopped.
 *  - others: If the read operation fails
 */
int wm_drv_i2s_read_stop(wm_device_t *device);

/**
 * @brief Registers a callback function to be called when the data has been successfully transmitted.
 *
 * This function registers a callback function to be called when the data has been successfully transmitted during an asynchronous write operation.
 * The callback function is called with the buffer that was written to the I2S device.
 *
 * @param[in] device A pointer to the I2S device to which the data will be written.
 * @param[in] callback A pointer to the callback function that will be called when the data has been successfully transmitted.
 *
 * @return
 *  - WM_ERR_SUCCESS: If the callback function is successfully registered.
 *  - others: If the callback function registration fails
 *
 * @note The registered callback function SHOULD NOT be blocked
 */
int wm_drv_i2s_register_write_cb(wm_device_t *device, wm_drv_i2s_callback_t callback);

/**
 * @brief Registers a callback function to be called when the data has been successfully received.
 *
 * This function registers a callback function to be called when the data has been successfully received during an asynchronous read operation.
 * The callback function is called with the buffer that was read from the I2S device.
 *
 * @param[in] device A pointer to the I2S device from which the data will be read.
 * @param[in] callback A pointer to the callback function that will be called when the data has been successfully received.
 *
 * @return
 *  - WM_ERR_SUCCESS: If the callback function is successfully registered.
 *  - others: If the callback function registration fails
 *
 * @note User SHOULD remember to free the buffer by calling the free in the buffer structure
 */
int wm_drv_i2s_register_read_cb(wm_device_t *device, wm_drv_i2s_callback_t callback);

/**
 * @brief Performs input/output control operations on the specified I2S device.
 *
 * This function allows the user to perform various input/output control operations
 * on the I2S device, such as setting configuration parameters, enabling/disabling
 * features, and retrieving status information.
 *
 * @param[in] device A pointer to the I2S device on which the IOCTL operation will be performed.
 * @param[in] cmd command type, @ref wm_drv_i2s_ioctl_cmd_t
 * @param[in] enable enable or not
 * @param[in] arg external argument for the spedific command
 *
 * @return
 *  - WM_ERR_SUCCESS: If the IOCTL operation is successful.
 *  - others: If the IOCTL operation fails.
 * @note when the cmd use WM_DRV_I2S_CMD_SET_CLK_SRC, arg is the external clock, unit in HZ,
 *      If set to true, a valid mclk_hz must be input from the GPIO bound to mclk (e.g., PA7),
        which should be at least 2 * 256 * N * Fs Hz (where N is an integer), this is set to
        false by default that means generate mclk with internal PLL
 */
int wm_drv_i2s_ioctl(wm_device_t *device, wm_drv_i2s_ioctl_cmd_t cmd, bool enable, uint32_t arg);

/**
 * @brief dump i2s information, Used for debug.
 *
 * @param[in] device A pointer to the I2S device.
 *
 * @return
 *  - WM_ERR_SUCCESS: If the callback function is successfully registered.
 *  - WM_ERR_INVALID_PARAM: invalid param
 */
int wm_drv_i2s_dump_info(wm_device_t *device);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_DRV_I2S_H__ */

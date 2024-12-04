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

#define WM_DRV_I2S_NBC_TDEPTH \
    (16) /**< During the transmission process, the buffer will be cached
                                     at the driver (BD) layer, and will be appropriately handed down as needed.
                                     The macro `WM_DRV_I2S_NBC_TDEPTH` is used to
                                     determine the number of Buffer Descriptors (BDs) of TX
                                     at the driver layer. Each BD can be associated with
                                     a user buffer, so a larger value indicates that
                                     more user buffers can be cached,
                                     which means a greater tolerance for link fluctuations.
                                     Conversely, a smaller value indicates fewer BDs at
                                     the driver layer, which results in less memory usage,
                                     but also less tolerance for link fluctuations */
#define WM_DRV_I2S_NBC_RDEPTH \
    (16) /**< During the reception process, the buffer will be cached
                                     at the driver (BD) layer, and will be appropriately handed up as needed.
                                     The macro `WM_DRV_I2S_NBC_RDEPTH` is used to
                                     determine the number of Buffer Descriptors (BDs) of RX
                                     at the driver layer. Each BD can be associated with
                                     a user buffer, so a larger value indicates that
                                     more user buffers can be cached,
                                     which means a greater tolerance for link fluctuations.
                                     Conversely, a smaller value indicates fewer BDs at
                                     the driver layer, which results in less memory usage,
                                     but also less tolerance for link fluctuations */

#define WM_DRV_I2S_DESC_TDEPTH \
    (8) /**< During the transmission process, the buffer will be cached
                                     at the HAL (Hardware Abstraction Layer) (DESC) layer,
                                     and will be appropriately handed up or down as needed.
                                     The macro `WM_DRV_I2S_DESC_TDEPTH` will determine
                                     the number of DMA descriptors at the HAL layer,
                                     similar to `WM_DRV_I2S_NBC_TDEPTH`, which will determine
                                     the maximum allowable link fluctuation tolerated by the DMA hardware.
                                     The larger this value is set, the more buffers can be provided
                                     to the DMA. This allows for more continuous and smooth data transfer
                                     by the hardware, reducing the probability of DMA stopping
                                     due to lack of data to transmit. */
#define WM_DRV_I2S_DESC_RDEPTH \
    (8) /**< During the reception process, the buffer will be cached
                                     at the HAL (Hardware Abstraction Layer) (DESC) layer,
                                     and will be appropriately handed up or down as needed.
                                     The macro `WM_DRV_I2S_DESC_RDEPTH` will determine
                                     the number of DMA descriptors at the HAL layer,
                                     similar to `WM_DRV_I2S_NBC_TDEPTH`, which will determine
                                     the maximum allowable link fluctuation tolerated by the DMA hardware.
                                     The larger this value is set, the more buffers can be provided
                                     to the DMA. This allows for more continuous and smooth data transfer
                                     by the hardware, reducing the probability of DMA stopping
                                     due to lack of data to transmit. */

#define WM_DRV_I2S_DESC_MAX_BUF_LEN \
    (15360) /**<The macro `WM_DRV_I2S_DESC_MAX_BUF_LEN` represents the maximum
                                           buffer length that each RX DMA descriptor can be associated with,
                                           and the user's initialization configuration parameter `rx_pkt_size`
                                           must not exceed this set value. */

#define WM_DRV_I2S_DESC_MIN_BUF_LEN \
    (80) /**<The macro `WM_DRV_I2S_DESC_MIN_BUF_LEN` represents the minimum
                                           buffer length that each RX DMA descriptor can be associated with,
                                           and the user's initialization configuration parameter `rx_pkt_size`
                                           must not less than this set value. */

#define WM_DRV_I2S_DESC_DEF_BUF_LEN \
    (640) /**< The default buffer size for the RX DMA DESC, when the user does
                                          not set `rx_pkt_size` in the configuration parameters. */

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
enum wm_drv_i2s_ioctl_cmd {
    WM_DRV_I2S_CMD_SET_MUTE,      /**< enable / disable mute for tx/rx(disable mute by default) */
    WM_DRV_I2S_CMD_SET_BITSHIFT,  /**< enable / disable clock phase shift mode for tx/rx(enable by default) */
    WM_DRV_I2S_CMD_SET_LZC,       /**< enable / disable left zero cross check for rx(disable by default) */
    WM_DRV_I2S_CMD_SET_RZC,       /**< enable / disable right zero cross check for rx(disable by default) */
    WM_DRV_I2S_CMD_SET_MONO_LEFT, /**< true if set mono channel to left, else right channel(left channel by default) */
    WM_DRV_I2S_CMD_SET_CLK_SRC,   /**< set I2S clock source and mclk source(internal clock by default) */
    WM_DRV_I2S_CMD_SET_MAX
};

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
 * @}
 */

/**
 * @defgroup WM_DRV_I2S_Unions WM DRV_I2S Unions
 * @brief WinnerMicro DRV_I2S Unions
 */

/**
 * @addtogroup WM_DRV_I2S_Unions
 * @{
 */

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

typedef struct wm_drv_i2s_rbuf wm_drv_i2s_rxbuf_t;

/**
 * @brief User callback interface for the completion of `buf` transmission,
 * generally used for operations such as release.
 *
 * @param[in] buf the buffer from user to be transmitted
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
typedef int (*i2s_tx_done_cb_t)(void *buf);

/**
 * @brief User callback interface for data reception completion;
 * after the user is finished with it, they need to call the 'free' function
 * carried in the structure to release the resources.
 *
 * @param[in] buf the buffer from driver which include valid data and some helper function
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
typedef int (*i2s_rx_ready_cb_t)(wm_drv_i2s_rxbuf_t *buf);

/**
 * @brief Used for releasing the RX buffer.
 *
 * @param[in] buf the buffer from driver which include valid data and some helper function
 *
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */

typedef int (*i2s_free_buf_cb_t)(void *buf);

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
 * @brief parameter struct for I2S IOCTL
 *
 * @note The clock configuration section must be carried out after
 * I2S initialization and before starting data transmission and reception;
 * otherwise, it will affect the data transfer.
 */
typedef struct {
    enum wm_drv_i2s_ioctl_cmd cmd;
    bool tx; /**< Select tx/rx for the relevant CMD */
    union {
        struct {
            bool extal_en;    /**< Used to select whether to use an external clock to
                                       generate mclk. If set to true, a valid mclk_hz
                                       must be input from the GPIO bound to mclk (e.g., PA7),
                                       which should be at least 2 * 256 * N * Fs Hz
                                       (where N is an integer), this is set to false by
                                       default that means generate mclk with internal PLL */
            uint32_t mclk_hz; /**< the external clock value in unit Hz */
        };
        bool en; /**< If the corresponding function of the enable
                            command is activated, set it to true; otherwise, set it to false. */
    };
} wm_drv_i2s_ioctl_args_t;

/**
 * @brief ring node body for user ready buffers
 */
typedef struct wm_drv_i2s_tbuf {
    int len;    /**< user data length */
    void *data; /**< user data */
} wm_drv_i2s_tx_nb_t;

/**
 * @brief ring node body for received ready buffers
 */
struct wm_drv_i2s_rbuf {
    uint32_t magic;               /**< Used to check if the memory space is valid */
    void *priv;                   /** privated field used by driver internal */
    struct wm_drv_i2s_rbuf *next; /** the next rx ready buffer */
    i2s_free_buf_cb_t free;       /** used to free this node */
    int len;     /**< Typically, equal to rx_pkt_size,  the actual received length in case of exceptions or timeouts. */
    char data[]; /**< size from rx_pkt_size in user cfg */
};

/**
 * @brief configuration of I2S controller
 */
typedef struct {
    enum wm_i2s_mode mode;       /**< I2S role mode */
    enum wm_i2s_dir dir;         /**< I2S xfer direction */
    enum wm_i2s_std std;         /**< I2S protocol standard */
    enum wm_i2s_fmt fmt;         /**< I2S frame format standard */
    enum wm_i2s_chan_type ctype; /**< the channel type, mono or stereo */
    enum wm_i2s_xfer_type xtype; /**< the transfer type select from DMA or others */
    uint32_t sample_rate_hz;     /**< the sample rate for the audio data */
    uint32_t rx_pkt_size;        /**< The buffer size of each received packet.  4bytes aligned,
                                    Usually set to 10/20 millisecond transmission data size */
    uint32_t rx_ready_th;        /**< rx ready pkts cache threshold number before upload to user ,usually set to 0 */
    uint32_t tx_ready_th;        /**< tx ready pkts cache threshold number before deliver to HAL ,usually set to 0 */
} wm_drv_i2s_cfg_t;

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
 * It stops any further data transmission and prevents any new write operations
 * from being initiated until the write operation is resumed using
 * `wm_drv_i2s_write_resume()`.
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
 *
 * @param[in] device A pointer to the I2S device to which the data will be written.
 *
 * @return
 *  - WM_ERR_SUCCESS: If the write operation is successfully stopped.
 *  - others: If the write operation fails
 */
int wm_drv_i2s_write_stop(wm_device_t *device);

/**
 * @brief Asynchronously reads data start from the I2S device.
 *
 * This function asynchronously reads data from the I2S device. The data is
 * stored in a buffer provided by the user and is read from the device when
 * the driver is ready. The user can register a callback function to be
 * notified when the data has been successfully received.
 *
 * @param[in] device A pointer to the I2S device from which the data will be read.
 *
 * @return
 *  - WM_ERR_SUCCESS: If the read operation is successful.
 *  - others: If the read operation fails
 *
 * @note This function can not be called more than once,
 * it used to start the asynchronous read operation
 */
int wm_drv_i2s_read_async(wm_device_t *device);

/**
 * @brief Pauses the ongoing read operation to the I2S device.
 *
 * This function pauses the ongoing read operation to the I2S device.
 * It stops any further data reception and prevents any new read operations
 * from being initiated until the read operation is resumed using
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
 * @param[in] cb A pointer to the callback function that will be called when the data has been successfully transmitted.
 *
 * @return
 *  - WM_ERR_SUCCESS: If the callback function is successfully registered.
 *  - others: If the callback function registration fails
 *
 * @note The registered callback function SHOULD NOT be blocked
 */
int wm_drv_i2s_register_write_cb(wm_device_t *device, i2s_tx_done_cb_t cb);

/**
 * @brief Registers a callback function to be called when the data has been successfully received.
 *
 * This function registers a callback function to be called when the data has been successfully received during an asynchronous read operation.
 * The callback function is called with the buffer that was read from the I2S device.
 *
 * @param[in] device A pointer to the I2S device from which the data will be read.
 * @param[in] cb A pointer to the callback function that will be called when the data has been successfully received.
 *
 * @return
 *  - WM_ERR_SUCCESS: If the callback function is successfully registered.
 *  - others: If the callback function registration fails
 *
 * @note User SHOULD remember to free the buffer by calling the free in the buffer structure
 */
int wm_drv_i2s_register_read_cb(wm_device_t *device, i2s_rx_ready_cb_t cb);

/**
 * @brief Performs input/output control operations on the specified I2S device.
 *
 * This function allows the user to perform various input/output control operations
 * on the I2S device, such as setting configuration parameters, enabling/disabling
 * features, and retrieving status information.
 *
 * @param[in] device A pointer to the I2S device on which the IOCTL operation will be performed.
 * @param[in] arg A pointer to the structure containing the IOCTL arguments.
 *
 * @return
 *  - WM_ERR_SUCCESS: If the IOCTL operation is successful.
 *  - others: If the IOCTL operation fails.
 *
 * @note The specific IOCTL operations and their parameters are defined by the
 *       `wm_drv_i2s_ioctl_args_t` structure and the `wm_drv_i2s_ioctl_cmd` enum.
 */
int wm_drv_i2s_ioctl(wm_device_t *device, wm_drv_i2s_ioctl_args_t *arg);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_DRV_I2S_H__ */

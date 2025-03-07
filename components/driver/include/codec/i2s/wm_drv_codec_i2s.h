/**
 * @file wm_drv_codec_i2s.h
 *
 * @brief DRV_CODEC_I2S Module
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

#ifndef __WM_DRV_CODEC_I2S_H__
#define __WM_DRV_CODEC_I2S_H__

#include <stddef.h>
#include <stdint.h>
#include "wmsdk_config.h"
#include "wm_error.h"
#include "wm_osal.h"
#include "wm_types.h"
#include "wm_dt.h"
#include "wm_drv_i2c.h"
#include "wm_drv_gpio.h"
#include "wm_soc_cfgs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_DRV_I2S_CODEC_Enumerations WM DRV_I2S_CODEC Enumerations
 * @brief WinnerMicro DRV_I2S_CODEC Enumerations
 */

/**
 * @addtogroup WM_DRV_I2S_CODEC_Enumerations
 * @{
 */

/**
 * @enum wm_drv_codec_i2s_type_t
 * @brief codec i2s work modules
 */
typedef enum {
    WM_DRV_CODEC_I2S_MODE_NONE = 0,
    WM_DRV_CODEC_I2S_MODE_IN   = (1 << 0), /**< Use ADC, only support input    */
    WM_DRV_CODEC_I2S_MODE_OUT  = (1 << 1), /**< Use DAC, only support output   */

    /**< Use ADC and DAC, support input and output  */
    WM_DRV_CODEC_I2S_MODE_INOUT = (WM_DRV_CODEC_I2S_MODE_IN | WM_DRV_CODEC_I2S_MODE_OUT),

    /**< Use ADC and DAC, support input and output, The voice from mic will mix to output  */
    WM_DRV_CODEC_I2S_MODE_INOUT_MIX_MIC_OUT = ((1 << 2) | WM_DRV_CODEC_I2S_MODE_INOUT),

    WM_DRV_CODEC_I2S_MODE_MAX
} wm_drv_codec_i2s_type_t;

/**
 * @enum wm_drv_codec_i2s_output_type_t
 * @brief codec i2s output type
 */
typedef enum {
    WM_DRV_CODEC_I2S_OUTPORT_SPEAKER   = (1 << 0), /**< Output to speaker    */
    WM_DRV_CODEC_I2S_OUTPORT_HEADPHONE = (1 << 1), /**< Output to headphone   */

    /**< Output to speaker and headphone */
    WM_DRV_CODEC_I2S_OUTPORT_SPEAKER_HEADPHONE = (WM_DRV_CODEC_I2S_OUTPORT_SPEAKER | WM_DRV_CODEC_I2S_OUTPORT_HEADPHONE),

} wm_drv_codec_i2s_output_type_t;

/**
 * @}
 */

/**
 * @defgroup WM_DRV_I2S_CODEC_Structures WM DRV_I2S_CODEC Structures
 * @brief WinnerMicro DRV_I2S_CODEC Structures
 */

/**
 * @addtogroup WM_DRV_I2S_CODEC_Structures
 * @{
 */

/**
 * @struct wm_drv_codec_i2s_cfg_t
 * @brief codec i2s config
 */
typedef struct {
    enum wm_i2s_mode mode;        /**< master or slave mode               */
    enum wm_i2s_std std;          /**< codec i2s protocol standard        */
    wm_drv_codec_i2s_type_t type; /**< codec type use                     */
    uint32_t mclk;                /**< mclk,unit hz ,set to 0 before init */
} wm_drv_codec_i2s_cfg_t;

/**
 * @struct wm_drv_codec_i2s_ctx_t
 * @brief codec i2s context
 */
typedef struct {
    void *mutex;                /**< mutex to access codec api  */
    bool is_start;              /**< start or not               */
    void *hw;                   /**< codec hardware infomation  */
    wm_device_t *i2s_dev;       /**< i2s controller device      */
    wm_drv_codec_i2s_cfg_t cfg; /**< configuration for codec    */
    wm_device_t *ctrl_dev;      /**< i2c contol device          */
    uint16_t ctrl_addr;         /**< i2c address                */
    uint32_t sample_rate;       /**< sample rate                */
    wm_gpio_num_t power_pin;    /**< power pin                  */
    void *pri;                  /**< private data               */
} wm_drv_codec_i2s_ctx_t;

/**
 * @struct wm_drv_codec_i2s_ops_t
 * @brief codec i2s ops struct
 */
typedef struct {
    int (*init)(wm_device_t *device, wm_drv_codec_i2s_cfg_t *cfg);
    int (*deinit)(wm_device_t *device);

    int (*set_format)(wm_device_t *device, uint32_t sample_rate_hz, enum wm_i2s_bits bits, enum wm_i2s_chan_type channel);

    int (*start)(wm_device_t *device);
    int (*stop)(wm_device_t *device);

    int (*set_mute)(wm_device_t *device, bool mute);
    int (*set_volume)(wm_device_t *device, int vol); /**< vol : 0~100 */

    int (*set_mic_mute)(wm_device_t *device, bool mute);
    int (*set_mic_volume)(wm_device_t *device, int vol); /**< vol : 0~100*/

    int (*set_reg)(wm_device_t *device, int reg, int value);  /**< Set value to codec register */
    int (*get_reg)(wm_device_t *device, int reg, int *value); /**< Get valule from codec register */

    int (*dump)(wm_device_t *device);

} wm_drv_codec_i2s_ops_t;

/**
 * @}
 */

/**
 * @defgroup WM_DRV_I2S_CODEC_Functions WM DRV_I2S_CODEC Functions
 * @brief WinnerMicro DRV_I2S_CODEC Functions
 */

/**
 * @addtogroup WM_DRV_I2S_CODEC_Functions
 * @{
 */

/**
 * @brief Initializes the WM DRV_I2S_CODEC device.
 *
 * This function initializes the WM DRV_I2S_CODEC device with the given configuration.
 * It allocates memory for the device structure, sets the device name, and initializes
 * the device operations structure with the provided function pointers.
 *
 * @param[in] name The name of the device to be initialized.
 * @param[in] cfg  A pointer to the configuration structure for the device.
 *
 * @return A pointer to the initialized device structure on success, or NULL on failure.
 */
wm_device_t *wm_drv_codec_i2s_init(const char *name, wm_drv_codec_i2s_cfg_t *cfg);

/**
 * @brief Deinitializes the WM DRV_I2S_CODEC device.
 *
 * This function deinitializes the WM DRV_I2S_CODEC device and frees any allocated resources.
 * It calls the deinit function pointer in the device operations structure to perform the actual
 * deinitialization.
 *
 * @param[in] device A pointer to the device structure representing the WM DRV_I2S_CODEC device.
 *
 * @return WM_SUCCESS on success, or an appropriate error code on failure.
 */
int wm_drv_codec_i2s_deinit(wm_device_t *device);

/**
 * @brief set codec audio format
 *
 * @param[in] device A pointer to the codec device to be deinitialized.
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
int wm_drv_codec_i2s_set_format(wm_device_t *device, uint32_t sample_rate_hz, enum wm_i2s_bits bits,
                                enum wm_i2s_chan_type channel);

/**
 * @brief Starts the I2S codec device for data transfer.
 *
 * This function initializes the I2S codec device for data transfer, enabling the
 * device to start receiving or transmitting data based on the configured mode.
 *
 * @param[in] device A pointer to the device structure representing the I2S codec device.
 *
 * @return WM_SUCCESS on success, or an appropriate error code on failure.
 *
 * @note This function should be called after the device has been initialized and configured.
 */
int wm_drv_codec_i2s_start(wm_device_t *device);

/**
 * @brief Stops the I2S codec device for data transfer.
 *
 * This function disables the I2S codec device, stopping it from receiving or transmitting data.
 * It is typically called when the device is no longer needed for data transfer.
 *
 * @param[in] device A pointer to the device structure representing the I2S codec device.
 *
 * @return WM_SUCCESS on success, or an appropriate error code on failure.
 *
 * @note This function should be called after the device has been initialized and configured.
 *       It is important to ensure that the device is properly deinitialized after stopping
 *       to free any allocated resources.
 */
int wm_drv_codec_i2s_stop(wm_device_t *device);

/**
 * @brief set output mute.
 *
 * @param[in] device A pointer to the device structure representing the I2S codec device.
 * @param[in] mute ture:mute output, false: not mute
 *
 * @return
 *      - WM_ERR_SUCCESS
 *      - WM_ERR_INVALID_PARAM
 */
int wm_drv_codec_i2s_set_mute(wm_device_t *device, bool mute);

/**
 * @brief set output volume level.
 *
 * @param[in] device A pointer to the device structure representing the I2S codec device.
 * @param[in] vol volume level, from 0 to 100
 *
 * @return
 *      - WM_ERR_SUCCESS
 *      - WM_ERR_INVALID_PARAM
 */
int wm_drv_codec_i2s_set_volume(wm_device_t *device, int vol);

/**
 * @brief set mic mute.
 *
 * @param[in] device A pointer to the device structure representing the I2S codec device.
 * @param[in] mute ture:mute output, false: not mute
 *
 * @return
 *      - WM_ERR_SUCCESS
 *      - WM_ERR_INVALID_PARAM
 */
int wm_drv_codec_i2s_set_mic_mute(wm_device_t *device, bool mute);

/**
 * @brief set mic level
 *
 * @param[in] device A pointer to the device structure representing the I2S codec device.
 * @param[in] vol volume level, 0~100.
 *
 * @return
 *      - WM_ERR_SUCCESS
 *      - WM_ERR_INVALID_PARAM
 */
int wm_drv_codec_i2s_set_mic_volume(wm_device_t *device, int vol);

/**
 * @brief Set value to codec register
 *
 * @param[in] device A pointer to the device structure representing the I2S codec device.
 * @param[in] reg codec register address
 * @param[in] value the value to set
 * @return
 *      - WM_ERR_SUCCESS
 *      - WM_ERR_INVALID_PARAM
 */
int wm_drv_codec_i2s_set_reg(wm_device_t *device, int reg, int value);

/**
 * @brief Get valule from codec register
 *
 * @param[in] device A pointer to the device structure representing the I2S codec device.
 * @param[in] reg codec register address
 * @param[out] value the value to set
 * @return
 *      - WM_ERR_SUCCESS
 *      - WM_ERR_INVALID_PARAM
 */
int wm_drv_codec_i2s_get_reg(wm_device_t *device, int reg, int *value);

/**
 * @brief Prints debug information about the I2S codec device.
 *
 * This function prints debug information about the I2S codec device, including
 * relevant configuration settings, internal state, and any other relevant information.
 * It is intended for debugging purposes and should not be used in production code.
 *
 * @param[in] device A pointer to the device structure representing the I2S codec device.
 *
 * @return WM_SUCCESS on success, or an appropriate error code on failure.
 *
 * @note This function should be called after the device has been initialized and configured.
 *       The output of this function may vary depending on the specific implementation of the I2S codec device.
 */
int wm_drv_codec_i2s_dump(wm_device_t *device);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_DRV_CODEC_I2S_H__ */

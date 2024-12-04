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
 * @defgroup WM_DRV_I2S_CODEC_Macros WM DRV_I2S_CODEC Macros
 * @brief WinnerMicro DRV_I2S_CODEC Macros
 */

/**
 * @addtogroup WM_DRV_I2S_CODEC_Macros
 * @{
 */

/**
 * @}
 */

/**
 * @defgroup WM_DRV_I2S_CODEC_Enumerations WM DRV_I2S_CODEC Enumerations
 * @brief WinnerMicro DRV_I2S_CODEC Enumerations
 */

/**
 * @addtogroup WM_DRV_I2S_CODEC_Enumerations
 * @{
 */

/**
 * @brief codec clock mode
 *
 * codec will generate bclk and lrclk via internal PLL when
 * work in master mode, otherwise will need bclk and lrclk
 * from the GPIOs
 */
enum wm_drv_codec_i2s_clk_mode {
    WM_DRV_CODEC_I2S_CLK_MASTER, /**< codec work in master mode */
    WM_DRV_CODEC_I2S_CLK_SLAVE,  /**< codec work in slave mode */
    WM_DRV_CODEC_I2S_CLK_MAX
};

/**
 * @brief codec i2s transfer direction
 *
 */
enum wm_drv_codec_i2s_mode {
    WM_DRV_CODEC_I2S_MODE_CODEC,     /**< codec work in both playback and recording mode */
    WM_DRV_CODEC_I2S_MODE_RECORD,    /**< codec work in recording mode that means enable ADC */
    WM_DRV_CODEC_I2S_MODE_PLAY_BACK, /**< codec work in playback mode that means enable DAC */

    WM_DRV_CODEC_I2S_MODE_MAX
};

/**
 * @brief codec i2s channel mode
 *
 */
enum wm_drv_codec_i2s_chan {
    WM_DRV_CODEC_I2S_CHAN_MONO = 1, /**< mono channel */
    WM_DRV_CODEC_I2S_CHAN_STEREO,   /**< stereo channel */

    WM_DRV_CODEC_I2S_CHAN_MAX,
};

/**
 * @brief codec i2s standard selection
 *
 */
enum wm_drv_codec_i2s_std {
    WM_DRV_CODEC_I2S_STD_I2S,  /**< philips I2S standard */
    WM_DRV_CODEC_I2S_STD_MSB,  /**< Most Significant Bit first standard */
    WM_DRV_CODEC_I2S_STD_DSPA, /**< DSP/PCM A standard */
    WM_DRV_CODEC_I2S_STD_DSPB, /**< DSP/PCM B standard */
    WM_DRV_CODEC_I2S_STD_MAX
};

/**
 * @brief codec i2s channel data bit width
 *
 */
enum wm_drv_codec_i2s_fmt {
    WM_DRV_CODEC_I2S_FMT_8BIT,  /**< 8 bits for each channel data per sample */
    WM_DRV_CODEC_I2S_FMT_16BIT, /**< 16 bits for each channel data per sample */
    WM_DRV_CODEC_I2S_FMT_18BIT, /**< 18 bits for each channel data per sample */
    WM_DRV_CODEC_I2S_FMT_20BIT, /**< 20 bits for each channel data per sample */
    WM_DRV_CODEC_I2S_FMT_24BIT, /**< 24 bits for each channel data per sample */
    WM_DRV_CODEC_I2S_FMT_32BIT, /**< 32 bits for each channel data per sample */
    WM_DRV_CODEC_I2S_FMT_MAX
};

/**
 * @brief codec i2s operations dynamically via ioctl
 *
 */
enum wm_drv_codec_i2s_ioctl_cmd {
    WM_DRV_CODEC_I2S_CMD_SET_VOL,   /**< set DAC volum, range [0, 96] */
    WM_DRV_CODEC_I2S_CMD_GET_VOL,   /**< get DAC volum, range [0, 96] */
    WM_DRV_CODEC_I2S_CMD_SET_MUTE,  /**< set DAC to mute */
    WM_DRV_CODEC_I2S_CMD_GET_MUTE,  /**< get DAC mute state */
    WM_DRV_CODEC_I2S_CMD_WRITE_REG, /**< write the register with the specified value */
    WM_DRV_CODEC_I2S_CMD_READ_REG,  /**< read the specified address of register */

    WM_DRV_CODEC_I2S_CMD_MAX
};

/**
 * @brief codec i2s output inerface selection
 *
 */
enum wm_drv_codec_i2s_output_intf {
    WM_DRV_CODEC_I2S_OUTPUT_MONOOUT, /**< output via phonejack */
    WM_DRV_CODEC_I2S_OUTPUT_SPKOUT,  /**< output via speaker */
    WM_DRV_CODEC_I2S_OUTPUT_BOTH,    /**< output via speaker and phonejack */
};

/**
 * @}
 */

/**
 * @defgroup WM_DRV_I2S_CODEC_Unions WM DRV_I2S_CODEC Unions
 * @brief WinnerMicro DRV_I2S_CODEC Unions
 */

/**
 * @addtogroup WM_DRV_I2S_CODEC_Unions
 * @{
 */

/**
 * @}
 */

/**
 * @defgroup WM_DRV_I2S_CODEC_Type_Definitions WM DRV_I2S_CODEC Type Definitions
 * @brief WinnerMicro DRV_I2S_CODEC Type Definitions
 */

/**
 * @addtogroup WM_DRV_I2S_CODEC_Type_Definitions
 * @{
 */

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
 * @brief ioctl argument structure to set codec dynamically
 *
 */
typedef struct {
    enum wm_drv_codec_i2s_ioctl_cmd cmd; /**< the ioctl command */
    union {
        int volume; /**< the volum value to set or get */
        bool mute;  /**< the mute state to set or get */
        struct {
            uint8_t addr; /**< the register address to operation */
            uint8_t val;  /**< the value of the register to be read or write */
        } reg;
    } u;
} wm_drv_codec_i2s_ioctl_arg_t;

typedef struct {
    enum wm_drv_codec_i2s_clk_mode i2s_mode; /**< codec i2s role mode */
    enum wm_drv_codec_i2s_mode codec_mode;   /**< codec xfer mode */
    enum wm_drv_codec_i2s_std std;           /**< codec i2s protocol standard */
    enum wm_drv_codec_i2s_fmt fmt;           /**< codec i2s frame format standard */
    enum wm_drv_codec_i2s_chan ctype;        /**< channel type */
    enum wm_drv_codec_i2s_output_intf intf;  /**< output interface selection */
    uint32_t mclk_hz;                        /**< the mclk input for codec generate the imclk,
                                  should be in range [1 18] MHz, It is recommended to use integer frequency values,
                                  as different codec chips may have varying requirements. For instance, the ES8374
                                  has been verified to function well with input frequencies greater than 8 MHz. */
    uint32_t sample_rate_hz;                 /**< the i2s sample rate, should be 8000 at least */
} wm_drv_codec_i2s_cfg_t;

typedef struct {
    int (*init)(wm_device_t *device, wm_drv_codec_i2s_cfg_t *cfg);
    int (*deinit)(wm_device_t *device);
    int (*start)(wm_device_t *device);
    int (*stop)(wm_device_t *device);
    int (*ioctl)(wm_device_t *device, wm_drv_codec_i2s_ioctl_arg_t *arg);
    int (*prev_ops)(wm_device_t *device);
    int (*post_ops)(wm_device_t *device);
    int (*dumps)(wm_device_t *device);
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
 * @brief Performs various operations on the I2S codec device using the specified command and argument.
 *
 * This function allows for dynamic control of the I2S codec device by executing specific commands
 * and providing the necessary arguments. The supported commands include setting the volume, getting the volume,
 * setting the mute state, getting the mute state, writing to a codec register, and reading from a codec register.
 *
 * @param[in] device A pointer to the device structure representing the I2S codec device.
 * @param[in,out] arg A pointer to the argument structure containing the command and any necessary data.
 *
 * @return WM_SUCCESS on success, or an appropriate error code on failure.
 *
 * @note The argument structure should be initialized with the desired command and any required data before calling this function.
 *
 * @see wm_drv_codec_i2s_ioctl_cmd
 * @see wm_drv_codec_i2s_ioctl_arg_t
 */
int wm_drv_codec_i2s_ioctl(wm_device_t *device, wm_drv_codec_i2s_ioctl_arg_t *arg);

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
int wm_drv_codec_i2s_dumps(wm_device_t *device);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_DRV_CODEC_I2S_H__ */

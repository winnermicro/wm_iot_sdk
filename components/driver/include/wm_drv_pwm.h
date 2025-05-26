/**
 * @file wm_drv_pwm.h
 *
 * @brief DRV_PWM Module
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

#ifndef __WM_DRV_PWM_H__
#define __WM_DRV_PWM_H__

#include <stdint.h>
#include "wmsdk_config.h"
#include "wm_error.h"
#include "wm_osal.h"
#include "wm_types.h"
#include "wm_dt.h"
#include "wm_hal_pwm.h"
#include "wm_cli.h"
#include "wm_list.h"
#include "wm_hal_dma.h"
#include "wm_soc_cfgs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_DRV_PWM_Macros WM DRV_PWM Macros
 * @brief WinnerMicro DRV_PWM Macros
 */

/**
 * @addtogroup WM_DRV_PWM_Macros
 * @{
 */

#define WM_DRV_PWM_MUTEX_TIMEOUT       (1000)     /**< PWM driver's mutex access wait timeout time, unit ms */
#define WM_DRV_PWM_DEADTIME_CLKDIV_MAX (3)        /**< PWM Dead Time Maximum Prescaler Factor */
#define WM_DRV_PWM_FREQ_MAX            (20000000) /**< Maximum PWM Output Frequency, unit Hertz (Hz) */
#define WM_DRV_PWM_FREQ_MIN            (3)        /**< Minimum PWM Output Frequency, unit Hertz (Hz) */
#define WM_DRV_PWM_CAP_CACHE_MAX       (20)       /**< Maximum number of capture cache data */

/**
 * @}
 */

/**
 * @defgroup WM_DRV_PWM_Enumerations WM DRV_PWM Enumerations
 * @brief WinnerMicro DRV_PWM Enumerations
 */

/**
 * @addtogroup WM_DRV_PWM_Enumerations
 * @{
 */

/**
 * @}
 */

/**
 * @defgroup WM_DRV_PWM_Unions WM DRV_PWM Unions
 * @brief WinnerMicro DRV_PWM Unions
 */

/**
 * @addtogroup WM_DRV_PWM_Unions
 * @{
 */

/**
 * @}
 */

/**
 * @defgroup WM_DRV_PWM_Type_Definitions WM DRV_PWM Type Definitions
 * @brief WinnerMicro DRV_PWM Type Definitions
 */

/**
 * @addtogroup WM_DRV_PWM_Type_Definitions
 * @{
 */

/**
 * @brief A user-defined callback function type for PWM capture and output reports.
 *
 * @param[in] report The pointer to the report data.
 * @param[in] user_arg The argument passed to the callback function.
 *
 * @return None. This function only processes the report data and may release the `report` pointer when it is no longer needed.
 *
 * @note This function type is used as a parameter type for the `wm_drv_pwm_set_channel_capture_cb()` and `wm_drv_pwm_set_channel_output_cb()` functions.
 * It is expected to process the report data and perform any necessary actions based on the report data.
 *
 * @note The function parameter `void *report` may be of type `wm_drv_pwm_cap_report_t` or `wm_drv_pwm_out_report_t`. At the end of the callback,
 * the lifecycle of `report` ends and the memory will be reclaimed. Additionally, when it is capture data, the user's callback needs to copy the content.
 * Once the callback ends, `value` will no longer hold a meaningful value.
 *
 * @note The function does not return any value, but only processes the report data.
 */
typedef void (*wm_drv_pwm_report_cb_t)(void *report, void *user_arg);

/**
 * @}
 */

/**
 * @defgroup WM_DRV_PWM_Structures WM DRV_PWM Structures
 * @brief WinnerMicro DRV_PWM Structures
 */

/**
 * @addtogroup WM_DRV_PWM_Structures
 * @{
 */

/**
 * @brief descript the report capture data structure
 */
typedef struct {
    uint16_t rval; /**< capture data from rising edge */
    uint16_t fval; /**< capture data from falling edge */
} wm_drv_pwm_cap_data_report_t;

/**
 * @brief used to report capture data when data is ready
 */
typedef struct {
    uint8_t chan;       /**< the channel associated with this interrupt report */
    uint8_t report_num; /* the number of report value */
    wm_drv_pwm_cap_data_report_t *value;
} wm_drv_pwm_cap_report_t;

/**
 * @brief used to report output data when period num exhaust
 */
typedef struct {
    uint8_t period_num; /**< output interrupt report the period num setting */
    uint8_t chan;       /**< the channel associated with this interrupt report */
} wm_drv_pwm_out_report_t;

/**
 * @brief PWM channel configure items of PWM driver
 */
typedef struct {
    enum wm_pwm_channels channel;
    enum wm_pwm_mode mode;
    enum wm_pwm_cap_int_type int_type; /**< which interrupt type used for capture */
    uint16_t clkdiv;                   /**< clock divider for this channel,[0, 65535] */
    uint8_t period_cycle; /**< the period cycles of divided clock, [0, 255], This variable, along with the duty_cycle, will
                                         determine the frequency of the PWM output. The formula for frequency is expressed as:
                                         freq = (pwm_clock_hz) / ((period_cycle + 1) * clkdiv), The current pwm_clock_hz is 40MHz. In practical
                                         scenarios, please refer to the RCC configuration for the PWM clock */
    uint8_t duty_cycle;   /**< the duty cycles of divided clock, [0, 255], This variable will determine the duty ratio of the
                                         output PWM, with the calculation formula being: duty_ratio = (duty_cycle + 1) / (period_cycle + 1) */
    uint8_t dead_time_clkdiv; /**< for complement mode, clock divide value for dead time, 0: master_clock,
                                            1: master_clock / 2, 2: master_clock / 4, 3: master_clock / 8 */
    uint8_t dead_time_cnt;    /**< for complement mode, dead time clock cycle count,
                             decided by dead_time_clkdiv, 0 means disable it */
    uint8_t period_num; /**< for output mode, the period count to be generated before rise the interrupt, 0 means disable it */
    uint8_t cap_cache_cnt; /**< the number of wm_drv_pwm_cap_report_t to cache for capture mode,
                                         this value should be less than WM_DRV_PWM_CAP_CACHE_MAX and
                                         bigger than 0 */
    bool period_int; /**< if enable the period output interrupt when period number reached, it takes effect when autoload is set to true */
    bool inverse;     /**< if inverse the output / capture ploarity */
    bool autoload;    /**< if atuo reload for loop output */
    bool output_high; /**< if output high level for brake mode */
} wm_drv_pwm_channel_cfg_t;

/**
 * @}
 */

/**
 * @defgroup WM_DRV_PWM_Functions WM DRV_PWM Functions
 * @brief WinnerMicro DRV_PWM Functions
 */

/**
 * @addtogroup WM_DRV_PWM_Functions
 * @{
 */

/**
 * @brief Initializes the PWM driver with the given name and parameters.
 *
 * @param[in] name The name of the PWM driver device.
 *
 * @return A pointer to the initialized PWM driver device on success, or NULL on failure.
 *
 * @note This function initializes the PWM driver with the provided name and parameters.
 * It also allocates memory for the driver context and initializes the necessary data structures.
 * The function returns a pointer to the initialized PWM driver device, which can be used for subsequent operations.
 */
wm_device_t *wm_drv_pwm_init(const char *name);

/**
 * @brief Deinitializes the PWM driver and releases all allocated resources.
 *
 * @param[in] device The pointer to the PWM driver device to be deinitialized.
 *
 * @return 0 on success, or a negative error code on failure.
 *
 * @note This function deinitializes the PWM driver and releases all allocated resources.
 * It stops all PWM channels, deinitializes the driver context, and frees the allocated memory.
 * The function returns 0 on success, or a negative error code on failure.
 */
int wm_drv_pwm_deinit(wm_device_t *device);

/**
 * @brief Initializes the PWM channel with the given configuration.
 *
 * @param[in] device The pointer to the PWM driver device.
 * @param[in] cfg The configuration parameters for the PWM channel.
 *
 * @return 0 on success, or a negative error code on failure.
 *
 * @note This function initializes the PWM channel with the provided configuration.
 * It sets the mode, capture interrupt type, GPIOs, brake input GPIO, clock divider, period cycles,
 * duty cycles, dead time clock divide value, dead time clock cycle count, period number, and output interrupt enable flag.
 *
 * @note When configuring the PWM channel in complementary or channel-synchronized mode, it is only necessary to set the
 * primary channel. For example, to set channels ch0 and ch1 as dual-channel synchronized, only ch0 needs to be configured.
 *
 * @note When set to capture mode, it is recommended to use DMA mode for `int_type` to achieve better performance.
 * Using interrupt mode may result in missed cases, leading to suboptimal results or slow reporting.
 *
 * @note When using capture mode, the best practice is to initialize the local device's frequency to be ten times the target
 * frequency of capture to obtain accurate results. For example, if the target frequency is 10 kHz, configure the period
 * cycle to 199 and the clock divider to 2 to achieve a local frequency of 100 kHz.
 *
 * @note If the current mode is operating in full channel synchronization, setting a specific channel to another mode
 * (break, independent, etc.) will terminate the full channel synchronization output.
 * */
int wm_drv_pwm_channel_init(wm_device_t *device, wm_drv_pwm_channel_cfg_t *cfg);

/**
 * @brief Deinitializes the PWM channel and releases all allocated resources.
 *
 * @param[in] device The pointer to the PWM driver device.
 * @param[in] channel The channel number of the PWM channel to be deinitialized.
 *
 * @return 0 on success, or a negative error code on failure.
 *
 * @note This function deinitializes the PWM channel with the provided channel number.
 * It stops the PWM channel, deinitializes the driver context, and frees the allocated memory.
 * The function returns 0 on success, or a negative error code on failure.
 */
int wm_drv_pwm_channel_deinit(wm_device_t *device, uint8_t channel);

/**
 * @brief Starts the specified PWM channel.
 *
 * @param[in] device The pointer to the PWM driver device.
 * @param[in] channel The channel number of the PWM channel to be started.
 *
 * @return 0 on success, or a negative error code on failure.
 *
 * @note This function starts the specified PWM channel with the provided channel number.
 * It enables the PWM output and sets the PWM channel to generate the specified frequency and duty cycle.
 * The function returns 0 on success, or a negative error code on failure.
 *
 * @note In PWM brake mode, when set to output a high level upon triggering, it will take effect by default.
 */
int wm_drv_pwm_channel_start(wm_device_t *device, uint8_t channel);

/**
 * @brief Deinitializes the PWM channel and releases all allocated resources.
 *
 * @param[in] device The pointer to the PWM driver device.
 * @param[in] channel The channel number of the PWM channel to be deinitialized.
 *
 * @return 0 on success, or a negative error code on failure.
 *
 * @note This function deinitializes the PWM channel with the provided channel number.
 * It stops the PWM channel, deinitializes the driver context, and frees the allocated memory.
 * The function returns 0 on success, or a negative error code on failure.
 */
int wm_drv_pwm_channel_stop(wm_device_t *device, uint8_t channel);

/**
 * @brief Starts all specified PWM channels.
 *
 * @param[in] device The pointer to the PWM driver device.
 *
 * @return 0 on success, or a negative error code on failure.
 *
 * @note This function starts all specified PWM channels.
 * It enables the PWM output and sets the PWM channels to generate the specified frequency and duty cycle.
 * The function returns 0 on success, or a negative error code on failure.
 */
int wm_drv_pwm_channels_start(wm_device_t *device);

/**
 * @brief Stops all specified PWM channels.
 *
 * @param[in] device The pointer to the PWM driver device.
 *
 * @return 0 on success, or a negative error code on failure.
 *
 * @note This function stops all specified PWM channels.
 * It stops the PWM output and sets the PWM channels to generate the specified frequency and duty cycle.
 * The function returns 0 on success, or a negative error code on failure.
 */
int wm_drv_pwm_channels_stop(wm_device_t *device);

/**
 * @brief Sets the frequency for the specified PWM channel.
 *
 * @param[in] device The pointer to the PWM driver device.
 * @param[in] channel The channel number of the PWM channel to be set.
 * @param[in] freq_hz The desired frequency in Hz for the specified PWM channel.
 *
 * @return 0 on success, or a negative error code on failure.
 *
 * @note This function sets the frequency for the specified PWM channel.
 * It configures the PWM channel to generate the specified frequency , duty cycle and PWM period num.
 * Developers can inspect the duty cycle, clock division, and period number using the provided getter functions.
 * But if the PWM period number is changed, the range of the duty cycle may be limited to a value less than 255.
 * The function returns 0 on success, or a negative error code on failure.
 */
int wm_drv_pwm_set_channel_freq(wm_device_t *device, uint8_t channel, uint32_t freq_hz);

/**
 * @brief Gets the frequency for the specified PWM channel.
 *
 * @param[in] device The pointer to the PWM driver device.
 * @param[in] channel The channel number of the PWM channel to be queried.
 * @param[in,out] freq_hz The pointer to the configured frequency in Hz for the specified PWM channel.
 *
 * @return 0 on success, or a negative error code on failure.
 *
 * @note This function gets the frequency for the specified PWM channel.
 * It retrieves the frequency and duty cycle configuration for the specified PWM channel.
 * The function returns 0 on success, or a negative error code on failure.
 */
int wm_drv_pwm_get_channel_freq(wm_device_t *device, uint8_t channel, uint32_t *freq_hz);

/**
 * @brief Sets the frequency for the specified PWM channel using period and clock divider.
 * freq = master_clock / clkdiv / (period + 1)
 *
 * @param[in] device The pointer to the PWM driver device.
 * @param[in] channel The channel number of the PWM channel to be set.
 * @param[in] period The desired period count in cycles.
 * @param[in] clkdiv The desired clock divider value.
 *
 * @return 0 on success, or a negative error code on failure.
 *
 * @note This function sets the frequency for the specified PWM channel using period and clock divider.
 * It configures the PWM channel to generate the specified frequency and duty cycle.
 * The function returns 0 on success, or a negative error code on failure.
 *
 * @note Frequency setting strategy:
 * 1. Frequency must be within [WM_DRV_PWM_FREQ_MIN(3Hz), WM_DRV_PWM_FREQ_MAX(160KHz)] range
 * 2. First try to achieve target frequency by adjusting clkdiv while keeping current period value
 *    Frequency calculation formula: freq = master_clock / (period + 1) / clkdiv
 *    Where:
 *    - master_clock is PWM clock frequency (typically 40MHz)
 *    - period remains unchanged from current configuration
 *    - clkdiv is automatically calculated, range is [1, 65535]
 * 3. If target frequency cannot be achieved by adjusting clkdiv alone,
 *    function will try to adjust both period and clkdiv to achieve target frequency
 * 4. If it is not possible to calculate an appropriate period and clkdiv, the setting will fail
 */
int wm_drv_pwm_set_channel_period_clkdiv(wm_device_t *device, uint8_t channel, uint8_t period, uint16_t clkdiv);

/**
 * @brief Gets the period and clock divider for the specified PWM channel.
 *
 * @param[in] device The pointer to the PWM driver device.
 * @param[in] channel The channel number of the PWM channel to be queried.
 * @param[in,out] period The pointer to the desired period count in cycles.
 * @param[in,out] clkdiv The pointer to the desired clock divider value.
 *
 * @return 0 on success, or a negative error code on failure.
 *
 * @note This function gets the frequency for the specified PWM channel using period and clock divider.
 * It retrieves the frequency and duty cycle configuration for the specified PWM channel.
 * The function returns 0 on success, or a negative error code on failure.
 */
int wm_drv_pwm_get_channel_period_clkdiv(wm_device_t *device, uint8_t channel, uint8_t *period, uint16_t *clkdiv);

/**
 * @brief Sets the duty cycle for the specified PWM channel.
 *
 * @param[in] device The pointer to the PWM driver device.
 * @param[in] channel The channel number of the PWM channel to be set.
 * @param[in] duty_cycle The desired duty cycle value in percentage (0-255).
 *
 * @return 0 on success, or a negative error code on failure.
 *
 * @note This function sets the duty cycle for the specified PWM channel.
 *
 * @note The output duty cycle is calculated as (duty_cycle + 1) / (period + 1),
 * if duty_cycle > period_cycle then the output is fixed at a high level. if
 * duty_cycle = 0, the low level width is PERIOD, and the high level width is 1
 * PWM divided clock cycle
 */
int wm_drv_pwm_set_channel_duty(wm_device_t *device, uint8_t channel, uint8_t duty_cycle);

/**
 * @brief Gets the duty cycle for the specified PWM channel.
 *
 * @param[in] device The pointer to the PWM driver device.
 * @param[in] channel The channel number of the PWM channel to be queried.
 * @param[in,out] duty_cycle The pointer to the desired duty cycle value.
 *
 * @return 0 on success, or a negative error code on failure.
 *
 * @note This function gets the duty cycle for the specified PWM channel.
 * It retrieves the duty cycle configuration for the specified PWM channel.
 * The duty cycle value is in percentage (0-255).
 * The function returns 0 on success, or a negative error code on failure.
 */
int wm_drv_pwm_get_channel_duty(wm_device_t *device, uint8_t channel, uint8_t *duty_cycle);

/**
 * @brief Sets the period number to be generated before rise the interrupt for the specified PWM channel.
 *
 * @param[in] device The pointer to the PWM driver device.
 * @param[in] channel The channel number of the PWM channel to be set.
 * @param[in] period_num The desired period number to be generated before rising the interrupt.
 *
 * @return 0 on success, or a negative error code on failure.
 *
 * @note This function sets the period number to be generated before rise the interrupt for the specified PWM channel.
 * It configures the PWM channel to generate the specified frequency and duty cycle,
 * and to generate the specified period number before rising the interrupt.
 * The function returns 0 on success, or a negative error code on failure.
 *
 * @note Dynamic setting of period_num in PWM:
 * If currently outputting, the setting will take effect immediately.
 * If not currently outputting, the setting will only take effect after calling start API.
 */
int wm_drv_pwm_set_channel_period_num(wm_device_t *device, uint8_t channel, uint8_t period_num);

/**
 * @brief Gets the period number to be generated before rise the interrupt for the specified PWM channel.
 *
 * @param[in] device The pointer to the PWM driver device.
 * @param[in] channel The channel number of the PWM channel to be queried.
 * @param[in,out] period_num The pointer to the desired period number to be generated before rising the interrupt.
 *
 * @return 0 on success, or a negative error code on failure.
 *
 * @note This function gets the period number to be generated before rise the interrupt for the specified PWM channel.
 * It retrieves the period number configuration for the specified PWM channel.
 * The function returns 0 on success, or a negative error code on failure.
 */
int wm_drv_pwm_get_channel_period_num(wm_device_t *device, uint8_t channel, uint8_t *period_num);

/**
 * @brief Enable /Disable the period number interrupt for the specified PWM channel.
 *
 * @param[in] device The pointer to the PWM driver device.
 * @param[in] channel The channel number of the PWM channel to be set.
 * @param[in] en if enable the interrupt.
 *
 * @return 0 on success, or a negative error code on failure.
 *
 * @note This function sets interrupt for the period number for the specified PWM channel.
 * It configures the PWM channel to generate the specified frequency and duty cycle,
 * and to generate the specified period number before rising the interrupt.
 * The function returns 0 on success, or a negative error code on failure.
 *
 * @note Dynamic setting of period_int in PWM:
 * If currently outputting, the setting will take effect immediately.
 * If not currently outputting, the setting will only take effect after calling start API.
 *
 */
int wm_drv_pwm_set_channel_period_int(wm_device_t *device, uint8_t channel, bool en);

/**
 * @brief Gets the period number interrupt flag for the specified PWM channel.
 *
 * @param[in] device The pointer to the PWM driver device.
 * @param[in] channel The channel number of the PWM channel to be queried.
 * @param[in,out] en The pointer to the desired period number interrupt flag.
 *
 * @return 0 on success, or a negative error code on failure.
 *
 * @note This function gets the period number interrupt flag for the specified PWM channel.
 * It retrieves the period number interrupt flag configuration for the specified PWM channel.
 * The function returns 0 on success, or a negative error code on failure.
 */
int wm_drv_pwm_get_channel_period_int(wm_device_t *device, uint8_t channel, bool *en);

/**
 * @brief Sets the inverse flag for the specified PWM channel.
 *
 * @param[in] device The pointer to the PWM driver device.
 * @param[in] channel The channel number of the PWM channel to be set.
 * @param[in] en if enable the inverse flag.
 *
 * @return 0 on success, or a negative error code on failure.
 *
 * @note This function sets the inverse flag for the specified PWM channel.
 * It configures the PWM channel to invert the output / capture polarity.
 * The function returns 0 on success, or a negative error code on failure.
 */
int wm_drv_pwm_set_channel_inverse(wm_device_t *device, uint8_t channel, bool en);

/**
 * @brief Gets the inverse flag for the specified PWM channel.
 *
 * @param[in] device The pointer to the PWM driver device.
 * @param[in] channel The channel number of the PWM channel to be queried.
 * @param[in,out] en The pointer to the desired inverse flag.
 *
 * @return 0 on success, or a negative error code on failure.
 *
 * @note This function gets the inverse flag for the specified PWM channel.
 * It retrieves the inverse flag configuration for the specified PWM channel.
 * The function returns the inverse flag for the specified PWM channel.
 */
int wm_drv_pwm_get_channel_inverse(wm_device_t *device, uint8_t channel, bool *en);

/**
 * @brief Sets the brake level flag for the specified PWM channel.
 *
 * @param[in] device The pointer to the PWM driver device.
 * @param[in] channel The channel number of the PWM channel to be set.
 * @param[in] en if enable the brake level flag.
 *
 * @return 0 on success, or a negative error code on failure.
 *
 * @note This function sets the brake level flag for the specified PWM channel.
 * It configures the PWM channel to enable or disable the brake level flag.
 *
 */
int wm_drv_pwm_set_channel_brake_level(wm_device_t *device, uint8_t channel, bool en);

/**
 * @brief Gets the brake level flag for the specified PWM channel.
 *
 * @param[in] device The pointer to the PWM driver device.
 * @param[in] channel The channel number of the PWM channel to be queried.
 * @param[in,out] en The pointer to the desired brake level flag.
 *
 * @return 0 on success, or a negative error code on failure.
 *
 * @note This function gets the brake level flag for the specified PWM channel.
 * It retrieves the brake level flag configuration for the specified PWM channel.
 * The function returns the brake level flag for the specified PWM channel.
 */
int wm_drv_pwm_get_channel_brake_level(wm_device_t *device, uint8_t channel, bool *en);

/**
 * @brief Sets the deadtime for the specified PWM channel.
 *
 * @param[in] device The pointer to the PWM driver device.
 * @param[in] channel The channel number of the PWM channel to be set.
 * @param[in] cnt The desired deadtime count in cycles.
 * @param[in] clkdiv The desired clock divider value.
 *
 * @return 0 on success, or a negative error code on failure.
 *
 * @note This function sets the deadtime for the specified PWM channel.
 * It configures the PWM channel to generate the specified frequency and duty cycle,
 * and to set the specified deadtime count and clock divider value.
 * The function returns 0 on success, or a negative error code on failure.
 */
int wm_drv_pwm_set_channel_deadtime(wm_device_t *device, uint8_t channel, uint8_t cnt, uint8_t clkdiv);

/**
 * @brief Gets the deadtime for the specified PWM channel.
 *
 * @param[in] device The pointer to the PWM driver device.
 * @param[in] channel The channel number of the PWM channel to be queried.
 * @param[in,out] cnt The pointer to the desired deadtime count in cycles.
 * @param[in,out] clkdiv The pointer to the desired clock divider value.
 *
 * @return 0 on success, or a negative error code on failure.
 *
 * @note This function gets the deadtime for the specified PWM channel.
 * It retrieves the deadtime configuration for the specified PWM channel,
 * including the deadtime count in cycles and the clock divider value.
 * The function returns 0 on success, or a negative error code on failure.
 */
int wm_drv_pwm_get_channel_deadtime(wm_device_t *device, uint8_t channel, uint8_t *cnt, uint8_t *clkdiv);

/**
 * @brief Sets the auto reload flag for the specified PWM channel.
 *
 * @param[in] dev The pointer to the PWM driver device.
 * @param[in] channel The channel number of the PWM channel to be set.
 * @param[in] en if enable the auto reload flag.
 *
 * @return 0 on success, or a negative error code on failure.
 *
 * @note This function sets the auto reload flag for the specified PWM channel.
 * It configures the PWM channel to enable or disable the auto reload flag.
 * The function returns 0 on success, or a negative error code on failure.
 */
int wm_drv_pwm_set_channel_autoreload(wm_device_t *dev, uint8_t channel, bool en);

/**
 * @brief Gets the auto reload flag for the specified PWM channel.
 *
 * @param[in] device The pointer to the PWM driver device.
 * @param[in] channel The channel number of the PWM channel to be queried.
 * @param[in,out] en The auto reload flag for the specified PWM channel.
 *
 * @return 0 on success, or a negative error code on failure.
 *
 * @note This function gets the auto reload flag for the specified PWM channel.
 * It retrieves the auto reload flag configuration for the specified PWM channel.
 * The function returns the auto reload flag for the specified PWM channel.
 */
int wm_drv_pwm_get_channel_autoreload(wm_device_t *device, uint8_t channel, bool *en);

/**
 * @brief Sets the output report callback function for the specified PWM channel.
 *
 * @param[in] device The pointer to the PWM driver device.
 * @param[in] channel The channel number of the PWM channel to be set.
 * @param[in] cb The output report callback function.
 * @param[in] arg The argument to be passed to the output report callback function.
 *
 * @return 0 on success, or a negative error code on failure.
 *
 * @note This function sets the output report callback function for the specified PWM channel.
 * It configures the PWM channel to call the specified output report callback function when the period number is exhausted.
 * The function returns 0 on success, or a negative error code on failure.
 */
int wm_drv_pwm_set_channel_output_cb(wm_device_t *device, uint8_t channel, wm_drv_pwm_report_cb_t cb, void *arg);

/**
 * @brief Sets the capture report callback function for the specified PWM channel.
 *
 * @param[in] device The pointer to the PWM driver device.
 * @param[in] channel The channel number of the PWM channel to be set.
 * @param[in] cb The capture report callback function.
 * @param[in] arg The argument to be passed to the capture report callback function.
 *
 * @return 0 on success, or a negative error code on failure.
 *
 * @note This function sets the capture report callback function for the specified PWM channel.
 * It configures the PWM channel to call the specified capture report callback function when capture data is ready.
 * The function returns 0 on success, or a negative error code on failure.
 */
int wm_drv_pwm_set_channel_capture_cb(wm_device_t *device, uint8_t channel, wm_drv_pwm_report_cb_t cb, void *arg);

/**
 * @brief Displays the configuration registers of the specified PWM channel.
 *
 * @param[in] device The pointer to the PWM driver device.
 *
 * @return None. This function only displays the configuration registers.
 *
 * @note This function displays the configuration registers of the specified PWM channel.
 * It retrieves the configuration registers for the specified PWM channel,
 * including the clock divider, period cycles, duty cycles, dead time clock divide value, dead time clock cycle count,
 * period number, and output interrupt enable flag.
 * The function does not return any value, but only displays the configuration registers.
 */
void wm_drv_pwm_show_cfg_regs(wm_device_t *device);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_DRV_PWM_H__ */

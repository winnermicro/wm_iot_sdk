/**
 * @file wm_hal_pwm.h
 *
 * @brief PWM HAL Module
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

#ifndef __WM_HAL_PWM_H__
#define __WM_HAL_PWM_H__

#include <stdint.h>
#include "wm_types.h"
#include "wm_ll_pwm.h"
#include "wm_pwm_reg_struct.h"
#include "wm_error.h"
#include "wm_list.h"
#include "wm_irq.h"
#include "wm_hal_dma.h"
#include "wm_soc_cfgs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_PWM_Macros WM PWM Macros
 * @brief WinnerMicro PWM Macros
 */

/**
 * @addtogroup WM_PWM_Macros
 * @{
 */

/**
 * @}
 */

/**
 * @defgroup WM_PWM_Enumerations WM PWM Enumerations
 * @brief WinnerMicro PWM Enumerations
 */

/**
 * @defgroup WM_PWM_Structures WM PWM Structures
 * @brief WinnerMicro PWM Structures
 */

/**
 * @addtogroup WM_PWM_Structures
 * @{
 */

/**
 * @brief xxx
 *
 * xxx
 */

struct wm_hal_pwm_dev;
typedef struct wm_hal_pwm_dev wm_hal_pwm_dev_t;

/** @brief arg structer of user for input /output interrupt callback
 *
 * user callback function should make a copy of this structure before return
 * while the structure will be free by HAL
 */
typedef struct wm_hal_pwm_cap_value {
    uint16_t rval; /**< capture data from rising edge */
    uint16_t fval; /**< capture data from falling edge */
} wm_hal_pwm_cap_report_t;

typedef struct wm_hal_pwm_out_value {
    uint8_t period_num; /**< output interrupt report the period num setting */
} wm_hal_pwm_out_report_t;

/** @brief indicatate that which channel's capture data is ready
 *
 * capture mode will store data for rising edge and falling edge,
 * so we should record this state to indicate that data is ready.
 * this structure is for internal use only
 */
typedef struct {
    union {
        struct {
            uint8_t falling_done_cnt;
            uint8_t rising_done_cnt;
        } e;
        uint16_t edge_done_cnt;
    } u;
    uint8_t reserved;
} wm_hal_pwm_int_done_bitmap_t;

typedef struct {
    struct dl_list item;
    enum wm_pwm_channels channel;
    enum wm_pwm_mode mode;
    enum wm_pwm_cnt_type cnt_type;
    uint16_t clkdiv;
    uint8_t period;                    /**< period value(output frequency F = CLK/CLK_DIV/PERIOD) */
    uint8_t period_num;                /**< the period count to be generated before rise the interrupt */
    uint8_t duty_cycle;                /**< duty cycle(range 0~255, high level or low level by out_inversion decided */
    uint8_t dead_time_clkdiv;          /**< clock divide value for dead time,
                                    0: master_clock,
                                    1: master_clock / 2,
                                    2: master_clock / 4,
                                    3: master_clock / 8 */
    uint8_t dead_time_cnt;             /**< dead time clock cycle count, decided by dead_time_clkdiv */
    bool dead_time_en;                 /**< if enable dead time for complement mode */
    bool period_int;                   /**< if enable the period num interrupt */
    bool inverse;                      /**< if inverse the output / capture ploarity */
    bool autoload;                     /**< if atuo reload for loop output */
    bool output_high;                  /**< if output high level for brake mode */
    enum wm_pwm_cap_int_type int_type; /**< which interrupt type used for capture */
    wm_dma_ch_t dma_channel;           /**< DMA channel associated with this PWM capture channel */
    wm_hal_dma_dev_t *dma_dev;         /**< the DMA dev for this PWM channel */
    wm_hal_dma_desc_t dma_cfg;         /**< DMA configure items for PWM capture channel */
    void (*o_cb)(wm_hal_pwm_dev_t *dev, uint8_t channel); /**< driver callback for PWM output period interrupt rising*/
    void (*i_cb)(wm_hal_pwm_dev_t *dev, uint8_t channel); /**< driver callback for PWM capture interrupt rising */
    wm_hal_pwm_out_report_t vo;
    wm_hal_pwm_int_done_bitmap_t int_done; /**< HAL internal use */
    uint32_t report_cap_size;              /**< the cache number of report data */
    wm_hal_pwm_cap_report_t vi[];          /**< report data container, should be the last one  */
} wm_hal_pwm_channel_cfg_t;

struct wm_hal_pwm_dev {
    void *master_dev;                                  /**< which drv dev dose this dev belongs to */
    wm_irq_no_t irq_num;                               /**< interrupt number for PWM module */
    wm_pwm_reg_t *reg_dev;                             /**< PWM controller reg base addr info from device table */
    uint32_t master_clock;                             /**< PWM master clock in HZ */
    wm_hal_pwm_channel_cfg_t *cfg[WM_PWM_CHANNEL_MAX]; /**< HAL internal use */
};

/**
 * @}
 */

/**
 * @defgroup WM_PWM_Type_Definitions WM PWM Type Definitions
 * @brief WinnerMicro PWM Type Definitions
 */

/**
 * @addtogroup WM_PWM_Type_Definitions
 * @{
 */

/**
 * @brief          the interrupt callback of PWM HAL,
 *                 this callback should be register to IRQ
 * @param[in]      irq the irq number of PWM module
 * @param[in]      arg bind to the hal dev
 */
void wm_hal_pwm_isr_handler(wm_irq_no_t irq, void *arg);

/**
 * @brief          initialize pwm channel
 * @param[in]      dev Peripheral PWM hal instance address
 *                 dev is the type of @arg wm_pwm_reg_t
 * @param[in]      cfg pwm channel configure items
 * @return         WM_ERR_SUCCESS success, otherwise Fail
 */
int wm_hal_pwm_channel_init(wm_hal_pwm_dev_t *dev, wm_hal_pwm_channel_cfg_t *cfg);

/**
 * @brief          de-initialize pwm channel
 * @param[in]      dev Peripheral PWM hal instance address
 *                 dev is the type of @arg wm_pwm_reg_t
 * @param[in]      channel the channel to be de-initialized
 * @return         WM_ERR_SUCCESS success, otherwise Fail
 */
int wm_hal_pwm_channel_deinit(wm_hal_pwm_dev_t *dev, uint8_t channel);

/**
 * @brief          start all configured channel
 * @param[in]      dev Peripheral PWM hal instance
 * @return         WM_ERR_SUCCESS success, otherwise Fail
 */
int wm_hal_pwm_chans_start(wm_hal_pwm_dev_t *dev);

/**
 * @brief          stop all configured channel
 * @param[in]      dev Peripheral PWM hal instance
 * @return         WM_ERR_SUCCESS success, otherwise Fail
 */
int wm_hal_pwm_chans_stop(wm_hal_pwm_dev_t *dev);

/**
 * @brief          start the configured channel
 * @param[in]      dev Peripheral PWM hal instance
 * @param[in]      channel the channel to be started
 * @return         WM_ERR_SUCCESS success, otherwise Fail
 */
int wm_hal_pwm_chan_start(wm_hal_pwm_dev_t *dev, uint8_t channel);

/**
 * @brief          stop the configured channel
 * @param[in]      dev Peripheral PWM hal instance
 * @param[in]      channel the channel to be started
 * @return         WM_ERR_SUCCESS success, otherwise Fail
 */
int wm_hal_pwm_chan_stop(wm_hal_pwm_dev_t *dev, uint8_t channel);

/**
 * @brief          initialize PWM module
 * @param[in]      dev Peripheral PWM hal instance
 * @return         WM_ERR_SUCCESS success, otherwise Fail
 */
int wm_hal_pwm_init(wm_hal_pwm_dev_t *dev);

/**
 * @brief          de-initialize PWM module
 * @param[in]      dev Peripheral PWM hal instance
 * @return         WM_ERR_SUCCESS success, otherwise Fail
 */
int wm_hal_pwm_deinit(wm_hal_pwm_dev_t *dev);

/**
 * @brief Sets the absolute frequency for the specified PWM channel.
 *
 * The function calculates the period value based on the provided frequency,
 * peripheral clock, and clock divider. However, it may not support all
 * continuous frequencies within the range of [3, 160] kHz due to limitations
 * in hardware or formula constraints.
 *
 * The formula used is: freq = peripheral_clock / ((period + 1) * clkdiv)
 *
 * @param[in] dev Pointer to the peripheral PWM HAL instance
 * @param[in] channel The channel to be configured (0-based)
 * @param[in] freq The desired final frequency in Hz
 *
 * @warning Adjusting the frequency may cause the period value to change.
 * The function may not support all continuous frequencies within the
 * range of [3, 160] kHz. Refer to the hardware documentation for specific
 * limitations or use a table to pre-calculate supported frequencies
 * based on the peripheral clock and clock divider values.
 *
 * @return WM_ERR_SUCCESS on success, otherwise an error code
 */
int wm_hal_pwm_set_freq(wm_hal_pwm_dev_t *dev, uint8_t channel, uint32_t freq);

/**
 * @brief          set frequency for the channel
 *                  freq = main_clock / (period + 1 ) / clkdiv
 * @param[in]      dev Peripheral PWM hal instance
 * @param[in]      channel the channel to be configured
 * @param[in]      period the period cycle counter
 * @param[in]      clkdiv the main clock divider
 * @return         WM_ERR_SUCCESS success, otherwise Fail
 */
int wm_hal_pwm_set_freq_inner(wm_hal_pwm_dev_t *dev, uint8_t channel, uint8_t period, uint16_t clkdiv);

/**
 * @brief          get frequency for the channel
 * @param[in]      dev Peripheral PWM hal instance
 * @param[in]      channel the channel to get the frequency
 * @param[in, out] freq_hz the frequency of this channel to be configured
 * @return         WM_ERR_SUCCESS success, otherwise Fail
 */
int wm_hal_pwm_get_freq(wm_hal_pwm_dev_t *dev, uint8_t channel, uint32_t *freq_hz);

/**
 * @brief          Get main clodk divider for PWM module channel
 * @param[in]      dev Peripheral PWM hal instance
 * @param[in]      channel the channel to be configured
 * @param[in, out] clkdiv the clock divider value for the PWM channel
 * @return         WM_ERR_SUCCESS success, otherwise Fail
 */
int wm_hal_pwm_get_clkdiv(wm_hal_pwm_dev_t *dev, uint8_t channel, uint16_t *clkdiv);

/**
 * @brief          Get period cycle of the PWM channel
 * @param[in]      dev Peripheral PWM hal instance
 * @param[in]      channel the channel to be configured
 * @param[in, out] period the period cycle of this channel
 * @return         WM_ERR_SUCCESS success, otherwise Fail
 */
int wm_hal_pwm_get_period(wm_hal_pwm_dev_t *dev, uint8_t channel, uint8_t *period);

/**
 * @brief          set period cycle number for the channel
 * @param[in]      dev Peripheral PWM hal instance
 * @param[in]      channel the channel to be configured
 * @param[in]      num the period num of this channel to be configured
 * @return         WM_ERR_SUCCESS success, otherwise Fail
 */
int wm_hal_pwm_set_period_num(wm_hal_pwm_dev_t *dev, uint8_t channel, uint8_t num);

/**
 * @brief          get period cycle number for the channel
 * @param[in]      dev Peripheral PWM hal instance
 * @param[in]      channel the channel to be configured
 * @param[in, out] period_num the period cycle number to be configured
 * @return         WM_ERR_SUCCESS success, otherwise Fail
 */
int wm_hal_pwm_get_period_num(wm_hal_pwm_dev_t *dev, uint8_t channel, uint8_t *period_num);

/**
 * @brief          set duty cycle counter for the channel
 * @param[in]      dev Peripheral PWM hal instance
 * @param[in]      channel the channel to be configured
 * @param[in]      duty_cycle of this channel to be configured
 * @return         WM_ERR_SUCCESS success, otherwise Fail
 */
int wm_hal_pwm_set_duty_cycle(wm_hal_pwm_dev_t *dev, uint8_t channel, uint8_t duty_cycle);

/**
 * @brief          get duty cycle counter for the channel
 * @param[in]      dev Peripheral PWM hal instance
 * @param[in]      channel the channel to be configured
 * @param[in, out] duty_cycle the duty cycle number to be configured on this channel
 * @return         WM_ERR_SUCCESS success, otherwise Fail
 */
int wm_hal_pwm_get_duty_cycle(wm_hal_pwm_dev_t *dev, uint8_t channel, uint8_t *duty_cycle);

/**
 * @brief Calculates the duty cycle counter value from a given duty ratio for the specified PWM channel.
 *
 * This function converts a duty ratio (between 0.0 and 1.0) to a duty cycle counter value
 * using the formula: duty_cycle = (duty_ratio * (period + 1)) - 1.
 *
 * @warning The result may not be very precise, as the outcome of the formula may not necessarily be an integer.
 * In such cases, the returned value will be truncated to the nearest integer.
 *
 * @param[in] dev Peripheral PWM hal instance
 * @param[in] channel The channel to be configured
 * @param[in] duty The desired duty ratio (0.0 to 1.0)
 *
 * @return The calculated duty cycle counter (may be a non-integer value due to floating-point conversion)
 */
uint8_t wm_hal_pwm_get_cycle_from_duty(wm_hal_pwm_dev_t *dev, uint8_t channel, float duty);

/**
 * @brief          enable / disable pwm inversion function for the channel
 * @param[in]      dev Peripheral PWM hal instance
 * @param[in]      channel the channel to be configured
 * @param[in]      inverse true if inverse is enabled
 * @return         WM_ERR_SUCCESS success, otherwise Fail
 */
int wm_hal_pwm_set_inverse(wm_hal_pwm_dev_t *dev, uint8_t channel, bool inverse);

/**
 * @brief          Get if the inverse is enabled or not
 * @param[in]      dev Peripheral PWM hal instance
 * @param[in]      channel the channel to be configured
 * @param[in, out] en true if inverse is enabled, otherwise False
 * @return         WM_ERR_SUCCESS success, otherwise Fail
 */
int wm_hal_pwm_get_inverse(wm_hal_pwm_dev_t *dev, uint8_t channel, bool *en);

/**
 * @brief          set the PWM output level for the brake mode
 * @param[in]      dev Peripheral PWM hal instance
 * @param[in]      channel the channel to be configured
 * @param[in]      high if output high level for the brake mode
 * @return         WM_ERR_SUCCESS success, otherwise Fail
 */
int wm_hal_pwm_set_brake_level(wm_hal_pwm_dev_t *dev, uint8_t channel, bool high);

/**
 * @brief          Get if brake output level is high
 * @param[in]      dev Peripheral PWM hal instance
 * @param[in]      channel the channel to be configured
 * @param[in, out] en true if output high level, otherwise False
 * @return         WM_ERR_SUCCESS success, otherwise Fail
 */
int wm_hal_pwm_get_brake_level(wm_hal_pwm_dev_t *dev, uint8_t channel, bool *en);

/**
 * @brief          set the PWM dead time cycle counter
 * @param[in]      dev Peripheral PWM hal instance
 * @param[in]      channel the channel to be configured
 * @param[in]      cnt the dead time cycle counter to be set
 * @param[in]      clkdiv the dead time clock divider value to be set
 * @return         WM_ERR_SUCCESS success, otherwise Fail
 */
int wm_hal_pwm_set_deadtime(wm_hal_pwm_dev_t *dev, uint8_t channel, uint8_t cnt, uint8_t clkdiv);

/**
 * @brief          get the PWM dead time cycle counter
 * @param[in]      dev Peripheral PWM hal instance
 * @param[in]      channel the channel to be configured
 * @param[in, out] cnt the deadtime cycle counter
 * @param[in, out] clkdiv the deadtime clock divider
 * @param[in, out] en the deadtime enabled state
 * @return         if operation is successfully or not
 */
int wm_hal_pwm_get_deadtime(wm_hal_pwm_dev_t *dev, uint8_t channel, uint8_t *cnt, uint8_t *clkdiv, bool *en);

/**
 * @brief          enable / disable dead time zone for PWM
 * @param[in]      dev Peripheral PWM hal instance
 * @param[in]      channel the channel to be configured
 * @param[in]      en true to enable, else disable
 * @return         WM_ERR_SUCCESS success, otherwise Fail
 */
int wm_hal_pwm_set_deadtime_en(wm_hal_pwm_dev_t *dev, uint8_t channel, bool en);

/**
 * @brief          enable / disable auto reload for PWM
 * @param[in]      dev Peripheral PWM hal instance
 * @param[in]      channel the channel to be configured
 * @param[in]      en true to enable, else disable
 * @return         WM_ERR_SUCCESS success, otherwise Fail
 */
int wm_hal_pwm_set_autoreload_en(wm_hal_pwm_dev_t *dev, uint8_t channel, bool en);

/**
 * @brief          Get if enable / disable auto reload for PWM
 * @param[in]      dev Peripheral PWM hal instance
 * @param[in]      channel the channel to be configured
 * @param[in]      en true to enable, else disable
 */
int wm_hal_pwm_get_autoreload_en(wm_hal_pwm_dev_t *dev, uint8_t channel, bool *en);

/**
 * @brief          enable / disable period interrupt for the channel
 * @param[in]      dev Peripheral PWM hal instance
 * @param[in]      channel the channel to be configured
 * @param[in]      en true to enable, else disable
 * @return         WM_ERR_SUCCESS success, otherwise Fail
 */
int wm_hal_pwm_set_period_int(wm_hal_pwm_dev_t *dev, uint8_t channel, bool en);

/**
 * @brief          Get if enable / disable period interrupt for the channel
 *
 * @param[in]      dev Peripheral PWM hal instance
 * @param[in]      channel the channel to be configured
 * @param[in, out] en true to enable, else disable. The function will return the current state of the period interrupt.
 *
 * @return         WM_ERR_SUCCESS success, otherwise Fail
 */
int wm_hal_pwm_get_period_int(wm_hal_pwm_dev_t *dev, uint8_t channel, bool *en);

/**
 * @brief          Get the configuration of PWM HAL
 *
 * @param[in]      dev Peripheral PWM hal instance
 * @param[in]      channel the channel to be configured
 * @param[in, out] cfg the configuration pointer to PWM HAL
 *
 * @return         WM_ERR_SUCCESS success, otherwise Fail
 */
int wm_hal_pwm_get_chan_cfg(wm_hal_pwm_dev_t *dev, uint8_t channel, wm_hal_pwm_channel_cfg_t **cfg);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_HAL_PWM_H__ */

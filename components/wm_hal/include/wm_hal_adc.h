/**
 * @file wm_hal_adc.h
 *
 * @brief adc hal api
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

#ifndef __WM_HAL_ADC_H__
#define __WM_HAL_ADC_H__

#include "wm_soc_cfgs.h"
#include "wm_adc_reg_struct.h"
#include "wm_hal_dma.h"
#include "wm_ll_adc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*wm_hal_adc_callback_t)(uint8_t channel, int *buf, uint16_t len, void *user_data);

#define WM_ADC_SIGNED_TO_UNSIGNED(value) \
    do {                                 \
        if (value & 0x20000) {           \
            value = value & 0x1FFFF;     \
        } else {                         \
            value = value | 0x20000;     \
        }                                \
    } while (0);

typedef struct {
    wm_adc_channel_t adc_channel;
    wm_adc_gain1_level_t pga_gain1;
    wm_adc_gain2_level_t pga_gain2;
    uint8_t adc_cmp;
    int cmp_data;
    uint8_t
        cmp_pol; //1: an interrupt is generated when adc_result < cmp_value; 0: an interrupt is generated when adc_result >= cmp_value
} wm_hal_adc_config_t;

typedef struct {
    wm_hal_adc_callback_t callback;
    void *arg;
} wm_hal_adc_isr_t;

typedef struct {
    int poly_n;
    double a[3];
} wm_adc_polyfit_param_t;

typedef struct {
    wm_adc_reg_t *reg_base;
    uint32_t irq_num;
    uint32_t irq_priority;
    uint8_t dma_ch;
    wm_hal_dma_dev_t *dma;
    wm_hal_dma_desc_t dma_desc;
    wm_hal_adc_isr_t irq_callback[WM_ADC_INTR_TYPE_MAX];
    int adc_offset;
    wm_adc_polyfit_param_t polyfit_param;
} wm_hal_adc_dev_t;

/**
  * @brief Init adc.
  *
  * @param[in] dev use @arg wm_hal_adc_dev_t
  * @param[in] cfg use @arg wm_hal_adc_config_t
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_hal_adc_init(wm_hal_adc_dev_t *dev, const wm_hal_adc_config_t *cfg);

/**
  * @brief Deinit adc.
  *
  * @param[in] dev use @arg wm_hal_adc_dev_t
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_hal_adc_deinit(wm_hal_adc_dev_t *dev);

/**
  * @brief Register adc callback.
  *
  * @param[in] adc_dev use @arg wm_hal_adc_dev_t  
  * @param[in] intr_type use @arg wm_adc_intr_type_t  
  * @param[in] adc_callback use @arg wm_hal_adc_callback_t  
  * @param[in] user_data
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_hal_adc_register_callback(wm_hal_adc_dev_t *adc_dev, wm_adc_intr_type_t intr_type, wm_hal_adc_callback_t adc_callback,
                                 void *user_data);

/**
  * @brief Start adc with interrupt.
  *
  * @param[in] dev use @arg wm_hal_adc_dev_t  
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_hal_adc_start_it(wm_hal_adc_dev_t *dev);

/**
  * @brief Stop adc interrupt.
  *
  * @param[in] dev use @arg wm_hal_adc_dev_t  
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_hal_adc_stop_it(wm_hal_adc_dev_t *dev);

/**
  * @brief Start adc with dma.
  *
  * @param[in] adc_dev use @arg wm_hal_adc_dev_t  
  * @param[in] buf store adc data
  * @param[in] len length of the buf
  * @param[in] dma_ch channel
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_hal_adc_start_dma(wm_hal_adc_dev_t *adc_dev, int *buf, uint32_t len, uint8_t dma_ch);

/**
  * @brief Stop adc dma.
  *
  * @param[in] adc_dev use @arg wm_hal_adc_dev_t
  * @param[in] dma_ch channel
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_hal_adc_stop_dma(wm_hal_adc_dev_t *adc_dev, uint8_t dma_ch);

/**
  * @brief Adc get polling data.
  *
  * @param[in] adc_dev use @arg wm_hal_adc_dev_t  
  * @param[in] buf  
  * @param[in] count result count
  * @param[in] timeout polling timeout. not used
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_hal_adc_polling(wm_hal_adc_dev_t *adc_dev, uint32_t *buf, uint16_t count, uint32_t timeout);

/**
  * @brief Adc get oneshort data.
  *
  * @param[in] dev use @arg wm_hal_adc_dev_t  
  * @param[in] timeout polling timeout. not used
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_hal_adc_oneshot(wm_hal_adc_dev_t *dev, uint32_t timeout);

/**
  * @brief Adc get inner temperature data.
  *
  * @param[in] dev use @ref wm_hal_adc_dev_t  
  * @param[out] temperature_val data
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_hal_thermal_sensor_read_temperature(wm_hal_adc_dev_t *dev, int *temperature_val);

/**
  * @brief Adc get inner voltage data.
  *
  * @param[in] dev use @arg wm_hal_adc_dev_t  
  * @param[out] voltage data
  * @return
  *    - WM_ERR_SUCCESS: succeed
  *    - others: failed
  */
int wm_hal_adc_chip_voltage_read_vdd(wm_hal_adc_dev_t *dev, int *voltage);

/**
  * @brief Adc calculate external voltage.
  *
  * @param[in] dev use @arg wm_hal_adc_dev_t  
  * @param[in] vol data , read from adc result register
  * @return
  *    - vlotage data
  */
int wm_hal_adc_cal_voltage(wm_hal_adc_dev_t *dev, double vol);

#endif /*__WM_HAL_ADC_H__*/

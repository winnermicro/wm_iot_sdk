/**
 * @file wm_drv_adc.h
 *
 * @brief Adc Module
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

#ifndef __WM_DRV_ADC_H__
#define __WM_DRV_ADC_H__
#include "wm_types.h"
#include "wm_soc_cfgs.h"
#include "wm_dt.h"
#include "wm_dt_hw.h"
#include "wm_hal_adc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_ADC_MACROs WM ADC Macros
 * @brief WinnerMicro ADC Macros
 */

/**
 * @addtogroup WM_ADC_MACROs
 * @{
 */

/**
 * @brief ADC get result
 */

#define WM_ADC_GET_RESULT(value) (value = value & WM_ADC_RESULT_MASK)

/**
 * @}
 */

/**
 * @defgroup WM_ADC_TYPEs WM ADC TYPEs
 * @brief WinnerMicro ADC TYPEs
 */

/**
 * @addtogroup WM_DRV_ADC_TYPEs
 * @{
 */

/**
 * @typedef wm_drv_adc_callback_t
 * @brief ADC callback type
 */

typedef void (*wm_drv_adc_callback_t)(uint8_t channel, int *buf, uint16_t len, void *user_data);

/**
 * @}
 */

/**
 * @defgroup WM_ADC_Structs WM DRV ADC TYPEs
 * @brief WinnerMicro DRV ADC TYPEs
 */

/**
 * @addtogroup WM_ADC_Structs
 * @{
 */

/**
 * @struct wm_drv_adc_cfg_t
 * @brief ADC cfg type
 */

typedef struct {
    uint8_t adc_channel_count;
    wm_dt_hw_adc_cfg_t cfg[0];
} wm_drv_adc_cfg_t;

/**
 * @}
 */

/**
 * @defgroup WM_ADC_APIs WM DRV ADC APIs
 * @brief WinnerMicro DRV ADC APIs
 */

/**
 * @addtogroup WM_ADC_APIs
 * @{
 */

/**
 * @brief Initialize adc dev.
 *
 * @param [in] dev_name like "adc", if device tree set
 * @return
 *    - device handle: device handle, used wm_device_t
 *    - NULL: failed
 */
wm_device_t *wm_drv_adc_init(char *dev_name);

/**
 * @brief Deinit adc dev.
 *
 * @param [in] dev use @arg wm_device_t 
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_drv_adc_deinit(wm_device_t *dev);

/**
 * @brief Set adc dev config.
 *
 * @param [in] dev use @arg wm_device_t 
 * @param [in] cfg use @arg wm_drv_adc_cfg_t
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed 
 * @warning In the w80x chip, both gian1 and gain 2 in cfg need to be set to LEVEL0 
 */
int wm_drv_adc_cfg(wm_device_t *dev, const wm_drv_adc_cfg_t *cfg);

/**
 * @brief Register adc interrupt callback.
 *
 * @param [in] dev use @arg wm_device_t 
 * @param [in] intr_type use @arg wm_drv_adc_intr_type_t 
 * @param [in] adc_callback use @arg wm_drv_adc_callback_t
 * @param [in] user_data
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_drv_adc_register_callback(wm_device_t *dev, wm_adc_intr_type_t intr_type, wm_drv_adc_callback_t adc_callback,
                                 void *user_data);

/**
 * @brief Get adc oneshot result.
 *
 * @param [in] dev use @arg wm_device_t 
 * @param [in] adc_channel use @arg wm_adc_channel_t
 * @param [out] result unit: millivolt
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_drv_adc_oneshot(wm_device_t *dev, wm_adc_channel_t adc_channel, int *result);

/**
 * @brief Get adc polling result.
 *
 * @param [in] dev use @arg wm_device_t 
 * @param [in] adc_channel use @arg wm_adc_channel_t
 * @param [out] buf adc polling data unit: millivolt
 * @param [in] count adc polling data count 
 * @param [in] timeout  not used for w800
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_drv_adc_polling(wm_device_t *dev, wm_adc_channel_t adc_channel, int *buf, uint16_t count, uint32_t timeout);

/**
 * @brief Start adc interrupt, you must register adc interrupt callback and interrupted data will not be converted and requires calling wm_drv_adc_cal_voltage to convert voltage.
 *
 * @param [in] dev use @arg wm_device_t 
 * @param [in] adc_channel use @arg wm_adc_channel_t
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 * @warning We do not recommend using this method unless you need to continuously obtain data.
 */
int wm_drv_adc_start_it(wm_device_t *dev, wm_adc_channel_t adc_channel);

/**
 * @brief Stop adc interrupt.
 *
 * @param [in] dev use @arg wm_device_t 
 * @param [in] adc_channel use @arg wm_adc_channel_t
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_drv_adc_stop_it(wm_device_t *dev, wm_adc_channel_t adc_channel);

/**
 * @brief Get chip inner temperature.
 *
 * @param [in] dev use @arg wm_device_t 
 * @param [out] temperature_val unit: millidegree
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_drv_thermal_sensor_read_temperature(wm_device_t *dev, int *temperature_val);

/**
 * @brief Get chip voltage.
 *
 * @param [in] dev use @arg wm_device_t 
 * @param [out] voltage unit: millivolt
 * @return
 *    - WM_ERR_SUCCESS: succeed
 *    - others: failed
 */
int wm_drv_adc_chip_voltage_read_vdd(wm_device_t *dev, int *voltage);

/**
 * @brief Calculate voltage value based on register value.
 *
 * @param [in] dev use @arg wm_device_t 
 * @param [in] vol adc result register value
 * @return
 *    - calculate voltage result
 */
int wm_drv_adc_cal_voltage(wm_device_t *dev, int vol);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_DRV_ADC_H__ */

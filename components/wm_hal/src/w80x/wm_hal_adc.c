/**
 * @file wm_hal_adc.c
 *
 * @brief Adc Hal Module
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

#include "wm_types.h"
#include "wm_error.h"
#include "wm_hal_irq.h"
#include "wm_hal_adc.h"

#define WM_HAL_ADC_RESULT_BIT_LEN                sizeof(int32_t)
#define WM_HAL_ADC_WAIT_DONE_TIMEOUT             15000
#define WM_HAL_ADC_GET_VOLT_NUM                  10
#define WM_HAL_ADC_GET_TEMP(temp)                (temp = ((temp * 1000 / (int)(2 * 2 * 4) - 4120702) * 1000 / 15548))
#define WM_HAL_ADC_GET_INNER_VOLT(value, offset) (value = ((value - offset) * 685 / 20 + 1200000) * 2)

/*ADC IRQ Status*/
#define CMP_INT_MASK                             (0x1 << 1)
#define ADC_INT_MASK                             (0x1 << 0)

#define WM_ADC_DEFAULT_KEEP_TIME                 (0x50)

/*CMP Value*/
#define ADC_INPUT_CMP_VAL(n)                     ((n) & 0x3FFFF)

static void wm_hal_adc_isr_handler(wm_irq_no_t irq, void *arg)
{
    uint32_t adcvalue;
    uint32_t reg;
    uint8_t channel       = 0;
    wm_hal_adc_dev_t *dev = NULL;

    if (arg) {
        dev     = (wm_hal_adc_dev_t *)arg;
        reg     = wm_ll_adc_get_adc_int_val(dev->reg_base);
        channel = wm_ll_adc_get_chl_choice(dev->reg_base);

        if (reg & ADC_INT_MASK) { // ADC中断
            wm_ll_adc_set_adc_int(dev->reg_base, 1);

            if (dev->irq_callback[WM_ADC_INTR_TYPE_ADC].callback) {
                adcvalue = wm_ll_adc_get_result_val(dev->reg_base);
                adcvalue = ADC_INPUT_CMP_VAL(adcvalue);
                dev->irq_callback[WM_ADC_INTR_TYPE_ADC].callback(channel, (int *)&adcvalue, 1,
                                                                 dev->irq_callback[WM_ADC_INTR_TYPE_ADC].arg);
            }
        }

        if (reg & CMP_INT_MASK) { // 比较中断
            wm_ll_adc_set_cmp_int(dev->reg_base, 1);

            if (dev->irq_callback[WM_ADC_INTR_TYPE_COMP].callback) {
                dev->irq_callback[WM_ADC_INTR_TYPE_COMP].callback(channel, NULL, 0,
                                                                  dev->irq_callback[WM_ADC_INTR_TYPE_COMP].arg);
            }
        }
    }
}

//static void wm_hal_adc_dma_callback(wm_dma_ch_t ch, uint32_t sts, void *user_data)
static void wm_hal_adc_dma_callback(wm_dma_ch_t ch, uint32_t sts, void *user_data)
{
    uint8_t channel       = 0;
    wm_hal_adc_dev_t *dev = NULL;

    if (user_data) {
        dev     = (wm_hal_adc_dev_t *)user_data;
        channel = wm_ll_adc_get_chl_choice(dev->reg_base);

        if (dev->irq_callback[WM_ADC_INTR_TYPE_DMA].callback) {
            dev->irq_callback[WM_ADC_INTR_TYPE_DMA].callback(channel, (int *)(dev->dma_desc.dest_addr),
                                                             (dev->dma_desc.ctrl.nonchain.len / WM_HAL_ADC_RESULT_BIT_LEN),
                                                             dev->irq_callback[WM_ADC_INTR_TYPE_DMA].arg);
        }

        wm_hal_dma_release_ch(dev->dma, ch);
    }
}

static void wm_hal_adc_stop(wm_hal_adc_dev_t *dev)
{
    //Clean pga ctrl
    wm_ll_adc_clr_pga_ctrl(dev->reg_base);

    //Stop adc
    wm_ll_adc_en_pd_sdadc(dev->reg_base, 1);
    wm_ll_adc_en_ldo_sdadc(dev->reg_base, 0);
    wm_ll_adc_reset_sdadc(dev->reg_base, 0);

    // Disable compare function and compare irq
    wm_ll_adc_en_cmp(dev->reg_base, 0);
    wm_ll_adc_en_cmp_int(dev->reg_base, 0);

    // Disable dma
    wm_ll_adc_en_dma(dev->reg_base, 0);
}

static void wm_hal_adc_wait_done(wm_hal_adc_dev_t *dev)
{
    uint32_t counter = 0;
    uint32_t timeout = WM_HAL_ADC_WAIT_DONE_TIMEOUT;
    uint32_t reg     = 0;

    /*wait for transfer success*/
    wm_hal_irq_disable(dev->irq_num);

    while (timeout--) {
        reg = wm_ll_adc_get_adc_int_val(dev->reg_base);
        if (reg & ADC_INT_MASK) {
            counter++;
            wm_ll_adc_set_adc_int(dev->reg_base, 1);
            if (counter == 4) {
                break;
            }
        } else if (reg & CMP_INT_MASK) {
            counter++;
            wm_ll_adc_set_cmp_int(dev->reg_base, 1);
            if (counter == 4) {
                break;
            }
        }
    }

    wm_hal_irq_enable(dev->irq_num);
}

int wm_hal_adc_cal_voltage(wm_hal_adc_dev_t *dev, double vol)
{
    double y1, voltage;
    int average = ((int)vol >> 2) & 0xFFFF;

    if (dev->polyfit_param.poly_n == 1) {
        y1 = dev->polyfit_param.a[1] * average + dev->polyfit_param.a[0];
    } else {
        voltage = ((double)vol - (double)(dev->adc_offset)) / 4.0;
        voltage = 1.196 + voltage * (126363 / 1000.0) / 1000000;

        y1 = voltage * 10000;
    }

    return (int)(y1 / 10);
}

static void wm_hal_adc_set_channel(wm_hal_adc_dev_t *dev, int adc_ch)
{
    adc_ch = adc_ch & 0x0f;

    // Set Channel
    wm_ll_adc_set_chl_choice(dev->reg_base, adc_ch);
}

static int wm_hal_adc_get_offset(wm_hal_adc_dev_t *dev)
{
    //Set Ldo
    wm_ll_adc_en_ldo_sdadc(dev->reg_base, 1);

    // Stop adc
    wm_ll_adc_en_pd_sdadc(dev->reg_base, 1);
    wm_ll_adc_reset_sdadc(dev->reg_base, 0);

    //Set default Time
    wm_ll_adc_set_ana_swth_time(dev->reg_base, WM_ADC_DEFAULT_KEEP_TIME);
    wm_ll_adc_set_ana_init_time(dev->reg_base, WM_ADC_DEFAULT_KEEP_TIME);

    //enable irq
    //wm_ll_adc_en_int(dev->reg_base, 1);
    wm_hal_irq_attach_sw_vector(dev->irq_num, wm_hal_adc_isr_handler, NULL);
    wm_hal_irq_enable(dev->irq_num);

    // Set reference sel
    wm_ll_adc_en_bypass_ref(dev->reg_base, 0);

    // Set pga
    wm_ll_adc_clr_pga_ctrl(dev->reg_base);
    wm_ll_adc_set_pga_gain(dev->reg_base, WM_ADC_GAIN1_LEVEL_0 | (WM_ADC_GAIN2_LEVEL_0 << 3));
    wm_ll_adc_en_bypass_pga(dev->reg_base, 0);

    wm_ll_adc_en_chop(dev->reg_base, 1);
    wm_ll_adc_en_pga(dev->reg_base, 1);

    wm_hal_adc_set_channel(dev, WM_ADC_CHANNEL_OFFSET);

    // Start adc
    wm_ll_adc_en_pd_sdadc(dev->reg_base, 0);
    wm_ll_adc_reset_sdadc(dev->reg_base, 1);

    wm_hal_adc_wait_done(dev);
    dev->adc_offset = wm_ll_adc_get_result_val(dev->reg_base);
    WM_ADC_SIGNED_TO_UNSIGNED(dev->adc_offset);

    wm_hal_adc_stop(dev);

    return WM_ERR_SUCCESS;
}

int wm_hal_adc_init(wm_hal_adc_dev_t *dev, const wm_hal_adc_config_t *cfg)
{
    if (dev->polyfit_param.poly_n == 0 || WM_ADC_CHANNEL_0_1_DIFF_INPUT == cfg->adc_channel ||
        WM_ADC_CHANNEL_2_3_DIFF_INPUT == cfg->adc_channel) {
        wm_hal_adc_get_offset(dev);
    }

    //Set Ldo
    wm_ll_adc_en_ldo_sdadc(dev->reg_base, 1);

    // Stop adc
    wm_ll_adc_en_pd_sdadc(dev->reg_base, 1);
    wm_ll_adc_reset_sdadc(dev->reg_base, 0);

    //Set default Time
    wm_ll_adc_set_ana_swth_time(dev->reg_base, WM_ADC_DEFAULT_KEEP_TIME);
    wm_ll_adc_set_ana_init_time(dev->reg_base, WM_ADC_DEFAULT_KEEP_TIME);

    // Set irq
    wm_ll_adc_en_int(dev->reg_base, 0);
    wm_hal_irq_attach_sw_vector(dev->irq_num, wm_hal_adc_isr_handler, dev);
    wm_hal_irq_enable(dev->irq_num);

    // Set reference sel
    wm_ll_adc_en_bypass_ref(dev->reg_base, 0);

    // Set pga
    wm_ll_adc_clr_pga_ctrl(dev->reg_base);
    wm_ll_adc_set_pga_gain(dev->reg_base, cfg->pga_gain1 | (cfg->pga_gain2 << 3));
    wm_ll_adc_en_bypass_pga(dev->reg_base, 0);

    wm_ll_adc_en_chop(dev->reg_base, 1);
    wm_ll_adc_en_pga(dev->reg_base, 1);

    wm_hal_adc_set_channel(dev, cfg->adc_channel);

    if (cfg->adc_cmp) {
        wm_ll_adc_set_cmp_value(dev->reg_base, ADC_INPUT_CMP_VAL(cfg->cmp_data));
        if (cfg->cmp_pol) {
            wm_ll_adc_set_cmp_pol(dev->reg_base, 1);
        } else {
            wm_ll_adc_set_cmp_pol(dev->reg_base, 0);
        }

        wm_ll_adc_en_cmp(dev->reg_base, 1);
        wm_ll_adc_en_cmp_int(dev->reg_base, 1);
    }

    return WM_ERR_SUCCESS;
}

int wm_hal_adc_deinit(wm_hal_adc_dev_t *dev)
{
    //Clean pga ctrl
    wm_ll_adc_clr_pga_ctrl(dev->reg_base);

    wm_ll_adc_en_pd_sdadc(dev->reg_base, 1);
    wm_ll_adc_en_ldo_sdadc(dev->reg_base, 0);
    wm_ll_adc_reset_sdadc(dev->reg_base, 0);

    // Disable compare function and compare irq
    wm_ll_adc_en_cmp(dev->reg_base, 0);
    wm_ll_adc_en_cmp_int(dev->reg_base, 0);

    //Disable irq
    wm_ll_adc_en_int(dev->reg_base, 0);

    // Disable dma
    wm_ll_adc_en_dma(dev->reg_base, 0);

    // Disable cmp
    wm_ll_adc_en_cmp(dev->reg_base, 0);
    wm_ll_adc_en_cmp_int(dev->reg_base, 0);

    return WM_ERR_SUCCESS;
}

int wm_hal_adc_register_callback(wm_hal_adc_dev_t *dev, wm_adc_intr_type_t intr_type, wm_hal_adc_callback_t adc_callback,
                                 void *user_data)
{
    dev->irq_callback[intr_type].callback = adc_callback;
    dev->irq_callback[intr_type].arg      = user_data;

    return WM_ERR_SUCCESS;
}

int wm_hal_adc_start_it(wm_hal_adc_dev_t *dev)
{
    // Start adc
    wm_ll_adc_en_pd_sdadc(dev->reg_base, 0);
    wm_ll_adc_reset_sdadc(dev->reg_base, 1);

    wm_hal_adc_wait_done(dev);

    wm_ll_adc_en_int(dev->reg_base, 1);
    // Disable dma
    wm_ll_adc_en_dma(dev->reg_base, 0);

    return WM_ERR_SUCCESS;
}

int wm_hal_adc_stop_it(wm_hal_adc_dev_t *dev)
{
    wm_hal_adc_stop(dev);

    //Disable irq
    wm_ll_adc_en_int(dev->reg_base, 0);

    return WM_ERR_SUCCESS;
}

static int wm_hal_adc_cal_inputvolt(wm_hal_adc_dev_t *dev, uint8_t channel, int volt)
{
    double voltage = 0.0;
    int average    = 0;

    average = volt;
    if ((channel == WM_ADC_CHANNEL_0_1_DIFF_INPUT) || (channel == WM_ADC_CHANNEL_2_3_DIFF_INPUT)) {
        voltage = ((double)average - (double)(dev->adc_offset)) / 4.0;
        voltage = voltage * (126363 / 1000) / 1000000;
    } else {
        return wm_hal_adc_cal_voltage(dev, (double)average);
    }

    average = (int)(voltage * 1000);

    return average;
}

int wm_hal_adc_polling(wm_hal_adc_dev_t *dev, uint32_t *buf, uint16_t count, uint32_t timeout)
{
    uint16_t i      = 0;
    uint8_t channel = 0;

    // Get channel
    channel = wm_ll_adc_get_chl_choice(dev->reg_base);

    // Start adc
    wm_ll_adc_en_pd_sdadc(dev->reg_base, 0);
    wm_ll_adc_reset_sdadc(dev->reg_base, 1);

    for (i = 0; i < count; i++) {
        wm_hal_adc_wait_done(dev);
        buf[i] = wm_ll_adc_get_result_val(dev->reg_base);
        buf[i] = buf[i] & WM_ADC_RESULT_MASK;
        WM_ADC_SIGNED_TO_UNSIGNED(buf[i]);
        buf[i] = wm_hal_adc_cal_inputvolt(dev, channel, buf[i]);
    }

    wm_hal_adc_stop(dev);

    return WM_ERR_SUCCESS;
}

int wm_hal_adc_oneshot(wm_hal_adc_dev_t *dev, uint32_t timeout)
{
    int value       = 0;
    uint8_t channel = 0;

    // Get channel
    channel = wm_ll_adc_get_chl_choice(dev->reg_base);

    // Start adc
    wm_ll_adc_en_pd_sdadc(dev->reg_base, 0);
    wm_ll_adc_reset_sdadc(dev->reg_base, 1);

    wm_hal_adc_wait_done(dev);
    value = wm_ll_adc_get_result_val(dev->reg_base);
    value = value & WM_ADC_RESULT_MASK;
    WM_ADC_SIGNED_TO_UNSIGNED(value);

    wm_hal_adc_stop(dev);

    value = wm_hal_adc_cal_inputvolt(dev, channel, value);

    return value;
}

int wm_hal_thermal_sensor_read_temperature(wm_hal_adc_dev_t *dev, int *temperature_val)
{
    int err                 = 0;
    uint32_t code1          = 0;
    uint32_t code2          = 0;
    int temperature         = 0;
    wm_hal_adc_config_t cfg = { 0 };

    cfg.pga_gain1   = WM_ADC_GAIN1_LEVEL_0;
    cfg.pga_gain2   = WM_ADC_GAIN2_LEVEL_3;
    cfg.adc_channel = WM_ADC_CHANNEL_TEMP;

    if (WM_ERR_SUCCESS != (err = wm_hal_adc_init(dev, &cfg))) {
        return err;
    }

    // Start adc
    wm_ll_adc_en_pd_sdadc(dev->reg_base, 0);
    wm_ll_adc_reset_sdadc(dev->reg_base, 1);

    // Temp start
    wm_ll_adc_set_temp_gain(dev->reg_base, 0);
    wm_ll_adc_en_temp(dev->reg_base, 1);
    wm_ll_adc_set_cal_offset_temp(dev->reg_base, 0);
    wm_hal_adc_wait_done(dev);
    code1 = wm_ll_adc_get_result_val(dev->reg_base);
    WM_ADC_SIGNED_TO_UNSIGNED(code1);
    wm_ll_adc_set_cal_offset_temp(dev->reg_base, 1);
    wm_hal_adc_wait_done(dev);
    code2 = wm_ll_adc_get_result_val(dev->reg_base);
    WM_ADC_SIGNED_TO_UNSIGNED(code2);
    wm_ll_adc_en_temp(dev->reg_base, 0);

    wm_hal_adc_stop(dev);
    temperature = ((int)code1 - (int)code2);
    WM_HAL_ADC_GET_TEMP(temperature);

    *temperature_val = temperature;

    return WM_ERR_SUCCESS;
}

int wm_hal_adc_chip_voltage_read_vdd(wm_hal_adc_dev_t *dev, int *voltage)
{
    int err                 = 0;
    uint32_t voltValue      = 0;
    uint32_t value          = 0;
    wm_hal_adc_config_t cfg = { 0 };

    wm_hal_adc_get_offset(dev);

    cfg.pga_gain1   = WM_ADC_GAIN1_LEVEL_0;
    cfg.pga_gain2   = WM_ADC_GAIN2_LEVEL_0;
    cfg.adc_channel = WM_ADC_CHANNEL_VOLT;

    if (WM_ERR_SUCCESS != (err = wm_hal_adc_init(dev, &cfg))) {
        return err;
    }

    // Start adc
    wm_ll_adc_en_pd_sdadc(dev->reg_base, 0);
    wm_ll_adc_reset_sdadc(dev->reg_base, 1);

    for (int i = 0; i < WM_HAL_ADC_GET_VOLT_NUM; i++) {
        wm_hal_adc_wait_done(dev);
        value = wm_ll_adc_get_result_val(dev->reg_base);
        WM_ADC_SIGNED_TO_UNSIGNED(value);
        voltValue += value;
    }

    voltValue = voltValue / WM_HAL_ADC_GET_VOLT_NUM;
    wm_hal_adc_stop(dev);

    WM_HAL_ADC_GET_INNER_VOLT(voltValue, dev->adc_offset);
    value    = voltValue - voltValue * WM_HAL_ADC_GET_VOLT_NUM / 100;
    *voltage = (value / 1000);

    return WM_ERR_SUCCESS;
}

int wm_hal_adc_start_dma(wm_hal_adc_dev_t *dev, int *buf, uint32_t len, uint8_t dma_ch)
{
    uint8_t channel           = 0;
    wm_dma_sts_info_t dma_sts = {
        .sts      = WM_DMA_RUNNIG,
        .xfer_cnt = 0,
    };

    if (!dev->irq_callback[WM_ADC_INTR_TYPE_DMA].callback) {
        return WM_ERR_NOT_ALLOWED;
    }

    // Wait dma done
    do {
        if (dma_sts.sts == WM_DMA_IDLE) {
            break;
        }
    } while (wm_hal_dma_get_status(dev->dma, dma_ch, &dma_sts));

    // Get channel
    channel = wm_ll_adc_get_chl_choice(dev->reg_base);

    switch (channel) {
        case WM_ADC_CHANNEL_0:
        case WM_ADC_CHANNEL_0_1_DIFF_INPUT:
        {
            dev->dma_desc.extend_ctrl.req_sel = WM_DMA_ADC_CH0_REQ;
            break;
        }
        case WM_ADC_CHANNEL_1:
        {
            dev->dma_desc.extend_ctrl.req_sel = WM_DMA_ADC_CH1_REQ;
            break;
        }
        case WM_ADC_CHANNEL_2:
        case WM_ADC_CHANNEL_2_3_DIFF_INPUT:
        {
            dev->dma_desc.extend_ctrl.req_sel = WM_DMA_ADC_CH2_REQ;
            break;
        }
        case WM_ADC_CHANNEL_3:
        {
            dev->dma_desc.extend_ctrl.req_sel = WM_DMA_ADC_CH3_REQ;
            break;
        }
        default:
        {
            return WM_ERR_INVALID_PARAM;
        }
    }

    // Register dma cb
    wm_hal_dma_register_callback(dev->dma, dma_ch, wm_hal_adc_dma_callback, dev);

    // Stop first
    wm_hal_dma_stop(dev->dma, dma_ch);

    // clr dma req
    wm_hal_dma_clr_req(dev->dma, dev->dma_desc.extend_ctrl.req_sel);

    // Set dma config
    dev->dma_desc.dest_addr                   = (int)buf;
    dev->dma_desc.src_addr                    = (int)(&(dev->reg_base->result.val));
    dev->dma_desc.ctrl.nonchain.src_addr_inc  = WM_DMA_ADDR_FIXED;
    dev->dma_desc.ctrl.nonchain.dest_addr_inc = WM_DMA_ADDR_INC;
    dev->dma_desc.ctrl.nonchain.len           = len * WM_HAL_ADC_RESULT_BIT_LEN;
    dev->dma_desc.ctrl.nonchain.auto_reload   = WM_DMA_RELOAD_DISABLE;
    dev->dma_desc.ctrl.nonchain.data_unit     = WM_DMA_XFER_UNIT_WORD;
    dev->dma_desc.ctrl.nonchain.burst_size    = WM_DMA_BURST_1UNIT;

    dev->dma_desc.extend_ctrl.ch           = dma_ch;
    dev->dma_desc.extend_ctrl.int_en       = WM_DMA_CH_EN_CTRL_ENABLE;
    dev->dma_desc.extend_ctrl.int_type     = WM_DMA_INT_XFER_DONE;
    dev->dma_desc.extend_ctrl.mode         = WM_DMA_HW_MODE;
    dev->dma_desc.extend_ctrl.chain_mode   = WM_DMA_CHAIN_MODE_NORMAL;
    dev->dma_desc.extend_ctrl.warp_mode_en = WM_DMA_WARP_CTRL_DISABLE;

    wm_hal_dma_config(dev->dma, &dev->dma_desc);

    // Start adc
    wm_ll_adc_en_dma(dev->reg_base, 1);
    wm_ll_adc_en_pd_sdadc(dev->reg_base, 0);
    wm_ll_adc_reset_sdadc(dev->reg_base, 1);

    // Start
    wm_hal_dma_start(dev->dma, dma_ch);

    return WM_ERR_SUCCESS;
}

int wm_hal_adc_stop_dma(wm_hal_adc_dev_t *dev, uint8_t dma_ch)
{
    wm_hal_adc_stop(dev);
    wm_hal_dma_stop(dev->dma, dma_ch);
    wm_hal_dma_unregister_callback(dev->dma, dma_ch);
    wm_hal_dma_release_ch(dev->dma, dma_ch);

    return WM_ERR_SUCCESS;
}

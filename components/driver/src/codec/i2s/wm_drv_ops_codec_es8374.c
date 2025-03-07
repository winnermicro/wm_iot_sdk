/**
 * @file wm_drv_ops_codec_es8374.c
 *
 * @brief DRV_OPS_CODEC_ES8374 Module
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

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "wm_dt.h"
#include "wm_osal.h"
#include "wm_reg_op.h"
#include "wm_soc.h"
#include "wm_error.h"
#include "wm_drv_codec_i2s.h"
#include "wm_drv_i2c.h"
#include "wm_drv_i2s.h"
#include "wm_dt_hw.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "wm_log.h"
#undef LOG_TAG
#define LOG_TAG                "es8374"

#define WM_DRV_CODEC_LOGE      wm_log_error
#define WM_DRV_CODEC_LOGD      wm_log_info

#define WM_DRV_CODEC_I2C_RW_MS 4

#define WM_ES8374_MDELAY(ms)   vTaskDelay(pdMS_TO_TICKS(2));

#define REG_00_RESET           0x00 /**< reset */

#define REG_01_CLKM            0x01 /**< clock manager */
#define REG_02_CLKM            0x02 /**< clock manager */
#define REG_03_CLKM            0x03 /**< clock manager */
#define REG_04_CLKM            0x04 /**< clock manager */
#define REG_05_CLKM            0x05 /**< clock manager */
#define REG_06_CLKM            0x06 /**< clock manager */
#define REG_07_CLKM            0x07 /**< clock manager */
#define REG_08_CLKM            0x08 /**< clock manager */
#define REG_09_CLKM            0x09 /**< clock manager */
#define REG_0A_CLKM            0x0A /**< clock manager */
#define REG_0B_CLKM            0x0B /**< clock manager */
#define REG_0C_CLKM            0x0C /**< clock manager */
#define REG_0D_CLKM            0x0D /**< clock manager */
#define REG_0E_CLKM            0x0E /**< clock manager */

#define REG_0F_SDP             0x0F /**< SDP */
#define REG_10_SDP             0x10 /**< SDP */
#define REG_11_SDP             0x11 /**< SDP */

#define REG_12_SYSTEM          0x12 /**< system */
#define REG_13_SYSTEM          0x13 /**< system */

#define REG_14_ANALOG_REF      0x14 /**< analog reference */
#define REG_15_ANALOG_PD       0x15 /**< analog power down*/

#define REG_16_ANALOG_LP       0x16 /**< analog low power    */
#define REG_17_REF_POWER       0x17 /**< reference power mode*/
#define REG_18_BIAS_SEL        0x18 /**< bias seclection     */
#define REG_1A_MONO_OUT_SEL    0x1A /**< mono out seclection */
#define REG_1B_MONO_OUT_GAIN   0x1B /**< mono out gain  */
#define REG_1C_MIXER           0x1C /**< mixer          */
#define REG_1D_MIXER_GAIN      0x1D /**< mixer gain     */

#define REG_1E_SPEAKER         0x1E /**< speaker*/
#define REG_1F_SPEAKER         0x1F /**< speaker*/
#define REG_20_SPEAKER         0x20 /**< speaker*/

#define REG_21_PGA             0x21 /**< PGA     */
#define REG_22_PGA_GAIN        0x22 /**< PGA gain*/

#define REG_24_ADC_CTRL        0x24 /**< ADC ctrl*/
#define REG_25_ADC_CTRL        0x25 /**< ADC ctrl*/
#define REG_26_ALC_CTRL        0x26 /**< ALC ctrl*/
#define REG_27_ALC_CTRL        0x27 /**< ALC ctrl*/
#define REG_28_ALC_CTRL        0x28 /**< ALC ctrl*/
#define REG_29_ALC_CTRL        0x29 /**< ALC ctrl*/
#define REG_2A_ALC_CTRL        0x2A /**< ALC ctrl*/
#define REG_2B_ALC_CTRL        0x2B /**< ALC ctrl*/

#define REG_2C_ADC_CTRL        0x2C /**< ADC ctrl*/
#define REG_2D_ADC_CTRL        0x2D /**< ADC ctrl*/
#define REG_2E_ADC_CTRL        0x2E /**< ADC ctrl*/
#define REG_2F_ADC_CTRL        0x2F /**< ADC ctrl*/
#define REG_30_ADC_CTRL        0x30 /**< ADC ctrl*/
#define REG_31_ADC_CTRL        0x31 /**< ADC ctrl*/
#define REG_32_ADC_CTRL        0x32 /**< ADC ctrl*/
#define REG_33_ADC_CTRL        0x33 /**< ADC ctrl*/
#define REG_34_ADC_CTRL        0x34 /**< ADC ctrl*/
#define REG_35_ADC_CTRL        0x35 /**< ADC ctrl*/

#define REG_36_DAC_CTRL        0x36 /**< DAC ctrl*/
#define REG_37_DAC_CTRL        0x37 /**< DAC ctrl*/
#define REG_38_DAC_CTRL        0x38 /**< DAC ctrl*/

#define REG_6D_GPIO_INT        0x6D /**< GPIO and interrupt */
#define REG_6E_FLAGS           0x6E /**< Flags              */
#define REG_6E_PLL_EN          0x6F /**< pll enable         */

#define WM_CODEC_VOL_MAX       100
#define WM_8374_MIN_REG_VOL    0xC0
#define WM_8374_MIN_DB_VOL     (-96.0)

/**
 * @brief Default sampling rate and clock settings. When playing or recording,
 * the actual sampling rate and clock will be set
 */
#define WM_8374_DEF_SMAPLE     16000
#define WM_8374_DEF_MCLK       5000000

/**
 * @brief codec 8374 channel data bit width
 *
 */
typedef enum {
    WM_ES8374_FMT_16BITS = 0x03, /**< 16 bits for each channel data per sample */
    WM_ES8374_FMT_18BITS = 0x02, /**< 18 bits for each channel data per sample */
    WM_ES8374_FMT_20BITS = 0x01, /**< 20 bits for each channel data per sample */
    WM_ES8374_FMT_24BITS = 0x00, /**< 24 bits for each channel data per sample */
    WM_ES8374_FMT_32BITS = 0x04, /**< 32 bits for each channel data per sample */
    WM_ES8374_FMT_MAX
} wm_drv_codec_i2s_bits_t;

/**
 * @brief codec 8374 mclk div
 *
 */
typedef enum {
    WM_ES8374_MCLK_DIV_1 = 1,
    WM_ES8374_MCLK_DIV_2 = 2,
    WM_ES8374_MCLK_DIV_3 = 3,
    WM_ES8374_MCLK_DIV_4 = 4,
    WM_ES8374_MCLK_DIV_6 = 5,
    WM_ES8374_MCLK_DIV_8 = 6,
    WM_ES8374_MCLK_DIV_9 = 7,
    WM_ES8374_MCLK_DIV_MAX,
} wm_es8374_sclk_div_t;

/**
 * @brief codec 8374 lclk div
 *
 */
typedef enum {
    WM_ES8374_LCLK_DIV_128  = 0,
    WM_ES8374_LCLK_DIV_192  = 1,
    WM_ES8374_LCLK_DIV_256  = 2,
    WM_ES8374_LCLK_DIV_384  = 3,
    WM_ES8374_LCLK_DIV_512  = 4,
    WM_ES8374_LCLK_DIV_576  = 5,
    WM_ES8374_LCLK_DIV_768  = 6,
    WM_ES8374_LCLK_DIV_1024 = 7,
    WM_ES8374_LCLK_DIV_MAX,
} wm_es8374_lclk_div_t;

/**
 * @brief codec 8374 pga gain type
 *
 */
typedef enum {
    DF2SE_PGA_GAIN_DIS = 0,
    DF2SE_PGA_GAIN_EN  = 1,
} wm_es8374_df2se_pga_t;

/**
  * @brief  codec_i2s device struct
  */
typedef struct {
    char *name;                  /**< device name                      */
    wm_dt_hw_codec_i2s_t *hw;    /**< hardware info                    */
    wm_drv_codec_i2s_ops_t *ops; /**< device operation interface       */
    wm_drv_codec_i2s_ctx_t *drv; /**< driver context data              */
    wm_device_status_t state;    /**< device state                     */
    void *priv;                  /**< user private data                */
} wm_drv_codec_i2s_dev_t;

static int read_reg(wm_drv_codec_i2s_ctx_t *ctx, uint8_t reg, uint8_t *value)
{
    int err;
    wm_drv_i2c_config_t cfg;

    cfg.addr     = ctx->ctrl_addr;
    cfg.speed_hz = WM_I2C_SPEED_STANDARD;

    WM_ES8374_MDELAY(WM_DRV_CODEC_I2C_RW_MS);

    err = wm_drv_i2c_read(ctx->ctrl_dev, &cfg, &reg, 1, value, 1);

    if (err != WM_ERR_SUCCESS) {
        WM_DRV_CODEC_LOGE("i2c write err=%d", err);
    }

    return err;
}

static int write_reg(wm_drv_codec_i2s_ctx_t *ctx, uint8_t reg, uint8_t value)
{
    int err;

    wm_drv_i2c_config_t cfg;

    cfg.addr     = ctx->ctrl_addr;
    cfg.speed_hz = WM_I2C_SPEED_STANDARD;

    err = wm_drv_i2c_write(ctx->ctrl_dev, &cfg, &reg, 1, &value, 1);

    WM_ES8374_MDELAY(WM_DRV_CODEC_I2C_RW_MS);

    if (err != WM_ERR_SUCCESS) {
        WM_DRV_CODEC_LOGE("i2c write err=%d", err);
    }

    return err;
}

static int es8374_ctrl_setup_pa_pin(wm_drv_codec_i2s_ctx_t *ctx)
{
    WM_DRV_CODEC_LOGD("setup gpio");

    if (ctx->power_pin >= 0 && ctx->power_pin < WM_GPIO_NUM_MAX) {
        wm_drv_gpio_iomux_func_sel(ctx->power_pin, WM_GPIO_IOMUX_FUN5);
        wm_drv_gpio_set_pullmode(ctx->power_pin, WM_GPIO_PULL_UP);
        wm_drv_gpio_set_dir(ctx->power_pin, WM_GPIO_DIR_OUTPUT);
    }

    return WM_ERR_SUCCESS;
}

static int es8374_ctrl_setup_jack_pin(wm_dt_hw_codec_i2s_t *hw)
{
    WM_DRV_CODEC_LOGD("setup gpio");

    if (hw->codec_cfg.jack_pin >= 0 && hw->codec_cfg.jack_pin < WM_GPIO_NUM_MAX) {
        wm_drv_gpio_iomux_func_sel(hw->codec_cfg.jack_pin, WM_GPIO_IOMUX_FUN5);
        wm_drv_gpio_set_pullmode(hw->codec_cfg.jack_pin, WM_GPIO_PULL_UP);
        wm_drv_gpio_set_dir(hw->codec_cfg.jack_pin, WM_GPIO_DIR_INPUT);
    }

    return WM_ERR_SUCCESS;
}

static int es8374_ctrl_set_gpio(wm_drv_codec_i2s_ctx_t *ctx, bool on)
{
    int err = WM_ERR_SUCCESS;

    if (ctx->power_pin != WM_GPIO_NUM_MAX) {
        if (on) {
            err = wm_drv_gpio_data_set(ctx->power_pin);
        } else {
            err = wm_drv_gpio_data_reset(ctx->power_pin);
        }
    }

    WM_DRV_CODEC_LOGD("set gpio %d=%d,err = %d", ctx->power_pin, on, err);

    return err;
}

static int es8374_dac_vol_to_reg_value(int vol, float max_db, int outport)
{
    /*
        00000000 － 0dB
        00000001 － -0.5dB
        00000010 － -1dB
        …
        11000000 － -96dB(default)
    */
    int ext_reg = 0;

    if (outport & WM_DRV_CODEC_I2S_OUTPORT_SPEAKER) {
        /*speaker need sub ext_reg*/
        ext_reg = ((int)(2 * max_db) + 1);
    }

    int reg_val = (WM_CODEC_VOL_MAX - vol) * (WM_8374_MIN_REG_VOL - ext_reg) / WM_CODEC_VOL_MAX + ext_reg;

    return reg_val;
}

static int es8374_adc_vol_to_reg_value(int vol)
{
    /*
        00000000 － 0dB
        00000001 － -0.5dB
        00000010 － -1dB
        …
        11000000 － -96dB(default)
    */

    int reg_val = (WM_CODEC_VOL_MAX - vol) * (WM_8374_MIN_REG_VOL) / WM_CODEC_VOL_MAX;

    return reg_val;
}

static int es8374_set_volume_internal(wm_drv_codec_i2s_ctx_t *ctx, int vol)
{
    uint8_t reg_vol;

    wm_dt_hw_codec_i2s_t *hw = ctx->hw;

    float max_db = hw->codec_cfg.max_gain;

    reg_vol = (uint8_t)es8374_dac_vol_to_reg_value(vol, max_db, hw->codec_cfg.out_port);

    wm_log_info("set out vol=%d,reg=%d,ex_db=%f", vol, reg_vol, max_db);

    return write_reg(ctx, REG_38_DAC_CTRL, (uint8_t)reg_vol);
}

static int es8374_set_mic_volume_internal(wm_drv_codec_i2s_ctx_t *ctx, int vol)
{
    uint8_t reg_vol;

    reg_vol = (uint8_t)es8374_adc_vol_to_reg_value(vol);

    wm_log_info("set mic vol=%d,reg=%d", vol, reg_vol);

    return write_reg(ctx, REG_25_ADC_CTRL, (uint8_t)reg_vol);
}

static int es8374_check_cfg(wm_drv_codec_i2s_cfg_t *cfg)
{
    if (!(cfg)) {
        WM_DRV_CODEC_LOGE("bad cfg");
        return WM_ERR_INVALID_PARAM;
    }

    /*Master or Slave*/
    if (!(cfg->mode == WM_I2S_MODE_MASTER || cfg->mode == WM_I2S_MODE_SLAVE)) {
        WM_DRV_CODEC_LOGE("bad mode=%d", cfg->mode);
        return WM_ERR_INVALID_PARAM;
    }

    /*ADC,DAC,Both,Line*/
    if (!(cfg->type > WM_DRV_CODEC_I2S_MODE_NONE || cfg->type < WM_DRV_CODEC_I2S_MODE_MAX)) {
        WM_DRV_CODEC_LOGE("bad type=%d", cfg->type);
        return WM_ERR_INVALID_PARAM;
    }

    /*Only I2S*/
    if (!(cfg->std == WM_I2S_STD_I2S)) {
        WM_DRV_CODEC_LOGE("bad std=%d", cfg->std);
        return WM_ERR_INVALID_PARAM;
    }

    return WM_ERR_SUCCESS;
}

static int es8374_init_ctrl(wm_device_t *device, wm_drv_codec_i2s_ctx_t *ctx)
{
    wm_dt_hw_codec_i2s_t *hw = (wm_dt_hw_codec_i2s_t *)device->hw;

    ctx->ctrl_dev = wm_dt_get_device_by_name(hw->i2c_device_name);
    if (!ctx->ctrl_dev) {
        WM_DRV_CODEC_LOGE("get i2c dev fail,name=%s", hw->i2c_device_name);
        return WM_ERR_FAILED;
    }

    ctx->ctrl_addr = hw->codec_cfg.i2c_address;

    return WM_ERR_SUCCESS;
}

static int es8374_i2s_cfg_clock(wm_drv_codec_i2s_ctx_t *ctx, wm_es8374_sclk_div_t sclk_div, wm_es8374_lclk_div_t lclk_div)
{
    int ret      = 0;
    int divratio = 0;
    uint8_t reg  = 0;

    ret = read_reg(ctx, REG_0F_SDP, &reg); // power up adc and input
    reg &= 0xe0;

    switch (sclk_div) {
        case WM_ES8374_MCLK_DIV_4: // = 4,
            divratio = 4;
            break;
        default:
            break;
    }
    reg |= divratio;
    ret |= write_reg(ctx, REG_0F_SDP, reg);

    int dacratio_l = 0;
    int dacratio_h = 1;

    switch (lclk_div) {
        case WM_ES8374_LCLK_DIV_256:
            dacratio_l = (256 & 0x0f); /**< low 8bits   */
            dacratio_h = (256 >> 8);   /**< hight 4bits */
            break;
        default:
            break;
    }
    ret |= write_reg(ctx, REG_06_CLKM, dacratio_h); // ADCFsMode,singel SPEED,RATIO=256
    ret |= write_reg(ctx, REG_07_CLKM, dacratio_l); // ADCFsMode,singel SPEED,RATIO=256

    return ret;
}

static int es8374_cfg_fmt(wm_drv_codec_i2s_ctx_t *ctx, enum wm_i2s_std std)
{
    int ret     = 0;
    uint8_t reg = 0;
    int fmt_i2s = std & 0x0f;

    if (ctx->cfg.type & WM_DRV_CODEC_I2S_MODE_IN) {
        ret |= read_reg(ctx, REG_10_SDP, &reg);
        if (ret == 0) {
            reg = reg & 0xfc;
            ret |= write_reg(ctx, REG_10_SDP, reg | fmt_i2s);
        }
    }
    if (ctx->cfg.type & WM_DRV_CODEC_I2S_MODE_OUT) {
        ret |= read_reg(ctx, REG_11_SDP, &reg);
        if (ret == 0) {
            reg = reg & 0xfc;
            ret |= write_reg(ctx, REG_11_SDP, reg | (fmt_i2s));
        }
    }
    return ret;
}

static int es8374_config_dac_output(wm_drv_codec_i2s_ctx_t *ctx)
{
    int ret     = 0;
    uint8_t reg = 0;

    ret = write_reg(ctx, REG_1D_MIXER_GAIN, 0x02); /**< speak mixer, -5dB/-6.5dB            */
    ret |= read_reg(ctx, REG_1C_MIXER, &reg);
    reg |= 0x80;
    ret |= write_reg(ctx, REG_1C_MIXER, reg);       /**< set spk mixer , select DAC to mixer */
    ret |= write_reg(ctx, REG_1D_MIXER_GAIN, 0x02); /**< speak mixer, -5dB/-6.5dB            */
    ret |= write_reg(ctx, REG_1F_SPEAKER, 0x00);    /**< speaker , normal                    */
    ret |= write_reg(ctx, REG_1E_SPEAKER, 0xA0);    /**< PGA 0x1E, 0 dB , enable classD , enable speaker */

    return ret;
}

/*
input:
0 – no in
1 – MIC1P-MIC1N
2 – MIC2P-MIC2N
3 – MIC1P-MIC1N and MIC2P-MIC2N
*/
static int es8374_config_adc_input(wm_drv_codec_i2s_ctx_t *ctx, uint8_t input)
{
    int ret     = 0;
    uint8_t reg = 0;

    ret |= read_reg(ctx, REG_21_PGA, &reg);
    if (ret == 0) {
        reg = (reg & 0xcf) | (input << 4) | 0x04;
        ret |= write_reg(ctx, REG_21_PGA, reg);
    }
    return ret;
}

static int es8374_set_mic_gain(wm_drv_codec_i2s_ctx_t *ctx, int gain)
{
    int ret = 0;

    if (gain >= 0 && gain < 24) {
        int gain_n = 0;
        gain_n     = (int)gain / 3;
        ret        = write_reg(ctx, REG_22_PGA_GAIN, gain_n | (gain_n << 4));
    } else {
        WM_DRV_CODEC_LOGE("es8374_set_mic_gain fail,gain=%d", gain);
        ret = -1;
    }
    return ret;
}

static int es8374_set_df2se_pga(wm_drv_codec_i2s_ctx_t *ctx, wm_es8374_df2se_pga_t gain)
{
    int ret     = 0;
    uint8_t reg = 0;

    if (gain == DF2SE_PGA_GAIN_DIS || gain == DF2SE_PGA_GAIN_EN) {
        ret = read_reg(ctx, REG_21_PGA, &reg);
        reg &= 0xfb;
        reg |= gain << 2;
        ret = write_reg(ctx, REG_21_PGA, reg);
    }
    return ret;
}

static int es8374_set_voice_mute(wm_drv_codec_i2s_ctx_t *ctx, bool enable)
{
    int ret     = 0;
    uint8_t reg = 0;

    ret |= read_reg(ctx, REG_36_DAC_CTRL, &reg);
    if (ret == 0) {
        reg = reg & 0xdf;
        ret |= write_reg(ctx, REG_36_DAC_CTRL, reg | (((int)enable) << 5));
    }
    return ret;
}

static int es8374_set_bits_per_sample(wm_drv_codec_i2s_ctx_t *ctx, wm_drv_codec_i2s_bits_t codec_bits)
{
    int ret     = WM_ERR_SUCCESS;
    uint8_t reg = 0;

    if (ctx->cfg.type & WM_DRV_CODEC_I2S_MODE_IN) {
        ret |= read_reg(ctx, REG_10_SDP, &reg);
        if (ret == 0) {
            reg = reg & 0xe3;
            ret |= write_reg(ctx, REG_10_SDP, reg | ((uint8_t)codec_bits << 2));
        }
    }

    if (ctx->cfg.type & WM_DRV_CODEC_I2S_MODE_OUT) {
        ret |= read_reg(ctx, REG_11_SDP, &reg);
        if (ret == 0) {
            reg = reg & 0xe3;
            ret |= write_reg(ctx, REG_11_SDP, reg | ((uint8_t)codec_bits << 2));
        }
    }
    return ret;
}

static void es8374_pa_power(wm_drv_codec_i2s_ctx_t *ctx, bool enable)
{
    es8374_ctrl_setup_pa_pin(ctx);
    es8374_ctrl_set_gpio(ctx, enable);
}

static int es8374_get_pll_param(wm_drv_codec_i2s_ctx_t *ctx, uint32_t vcodiv, uint8_t *pll_N, uint32_t *pll_K)
{
    /*
        reference by ES8374 user Guide.pdf

        F(MCLK)=12MHz,
        F(PLLOUT)=12.288MHz, because LRCK = 48KHZ
        Use 1/8 divider, F(vcoout) = 12.288MHz * 8 = 98.304MHz.
        R = F(vcoout) / F(CLK_IN) = 98.304 / 12.0 = 8.192
        PLL_N = int(R) = 8
        PLL_K = int(2^22 * (8.192-8) * 0.65735982222960) = 529376 = 0x813E0
        So,user should to be set the N=8,K=0x0813E0,VCODIV=8
    */

    double pll_out = ctx->sample_rate * 256;
    double vcoout  = pll_out * vcodiv; /* vcodiv=8 */
    double R       = vcoout / ctx->cfg.mclk;

    *pll_N = (int)R;
    *pll_K = (uint32_t)((1 << 22) * (R - *pll_N) * 0.65735982222960);

    wm_log_debug("N=%d,k=0x%0X", *pll_N, *pll_K);

    return WM_ERR_SUCCESS;
}

static int es8374_init_reg(wm_drv_codec_i2s_ctx_t *ctx, enum wm_i2s_std std, wm_es8374_sclk_div_t sclk_div,
                           wm_es8374_lclk_div_t lclk_div, uint8_t in_channel, uint8_t outport)
{
    int ret     = 0;
    uint8_t reg = 0;
    int vcodiv  = 8;
    uint8_t pll_N;
    uint32_t pll_K;

    ret |= write_reg(ctx, REG_00_RESET, 0x3F); /**<  Codec Rst start    */
    ret |= write_reg(ctx, REG_00_RESET, 0x03); /**< Codec Rst stop      */
    ret |= write_reg(ctx, REG_01_CLKM, 0x7F);  /**< Codec clk on,normal MCLK,not divide by 2 */

    ret |= read_reg(ctx, REG_0F_SDP, &reg);
    reg &= 0x7f;
    reg |= ((ctx->cfg.mode == WM_I2S_MODE_SLAVE ? 0 : 1) << 7);
    ret |= write_reg(ctx, REG_0F_SDP, reg); /**< Codec work on slave mode   */

    es8374_get_pll_param(ctx, vcodiv, &pll_N, &pll_K);

    ret |= write_reg(ctx, REG_6E_PLL_EN, 0xA0);               /**< pll set:mode enable        */
    ret |= write_reg(ctx, 0x72, 0x41);                        /**< pll set:mode set           */
    ret |= write_reg(ctx, REG_09_CLKM, 0x01);                 /**< pll set:reset on ,set start*/
    ret |= write_reg(ctx, REG_0C_CLKM, (pll_K >> 16) & 0xff); /**< pll set:k                  */
    ret |= write_reg(ctx, REG_0D_CLKM, (pll_K >> 8) & 0xff);  /**< pll set:k                  */
    ret |= write_reg(ctx, REG_0E_CLKM, pll_K & 0xff);         /**< pll set:k                  */
    ret |= write_reg(ctx, REG_0A_CLKM, 0x3A);                 /**< pll set: low_power=normal,vddd=3.3V, vco gain=2, cp gain=3 */
    ret |= write_reg(ctx, REG_0B_CLKM, pll_N);                /**< pll set:n                  */
    ret |= write_reg(ctx, REG_09_CLKM, 0x41); /**< pll set:reset off ,set stop; 1/[1:0] – vcoout divide by 8  */

    ret |= es8374_i2s_cfg_clock(ctx, sclk_div, lclk_div);

    ret |= write_reg(ctx, REG_24_ADC_CTRL, 0x08); /**< adc set, enable ADC left channel high pass filter (default) */
    ret |= write_reg(ctx, REG_36_DAC_CTRL, 0x00); /**< dac set, nute=0,no phase inverse */
    ret |= write_reg(ctx, REG_12_SYSTEM, 0x30);   /**< timming set */
    ret |= write_reg(ctx, REG_13_SYSTEM, 0x20);   /**< timming set */

    ret |= es8374_cfg_fmt(ctx, 0);

    ret |= write_reg(ctx, REG_21_PGA, 0x50);      /**< adc set: SEL LIN1 CH+PGAGAIN=0DB */
    ret |= write_reg(ctx, REG_22_PGA_GAIN, 0xFF); /**< adc set: PGA GAIN=0DB            */
    ret |= write_reg(ctx, REG_21_PGA, 0x24);      /**< adc set: SEL LIN2 CH+PGAGAIN=18DB*/
    ret |= write_reg(ctx, REG_22_PGA_GAIN, 0x55); /**< pga = +15db                      */
    ret |= write_reg(ctx, REG_08_CLKM, 0x21);  /**< set class d divider = 33, to avoid the high frequency tone on laudspeaker */
    ret |= write_reg(ctx, REG_00_RESET, 0x80); /**< IC START, class D speaker divider , DIV_CLKD=16 */

    es8374_set_mic_volume_internal(ctx, 100); /**< 0 db , max mic volume */

    ret |= write_reg(ctx, REG_14_ANALOG_REF, 0x8A); /**< IC START    */
    ret |= write_reg(ctx, REG_15_ANALOG_PD, 0x40);  /**< IC START    */

    /**< monoout set: select DAC to mono output, enable mono output */
    ret |= write_reg(ctx, REG_1A_MONO_OUT_SEL, (outport & WM_DRV_CODEC_I2S_OUTPORT_HEADPHONE) ? 0xA0 : 0);

    /**< monoout set: high driving level mono output bias,LOUT_VOL:+1db/-0.5db   */
    ret |= write_reg(ctx, REG_1B_MONO_OUT_GAIN, 0x4B);
    ret |= write_reg(ctx, REG_1C_MIXER, 0x90); /**< spk set,MIXER, 7:select DAC to mixer ,4: low power level mixer bias     */
    ret |= write_reg(ctx, REG_1D_MIXER_GAIN, 0x01); /**< spk set,MIXER GAIN,-6.5dB/-8dB  */
    ret |= write_reg(ctx, REG_1F_SPEAKER, 0x00);    /**< spk set,SPEAKER,normal          */
    ret |= write_reg(ctx, REG_1E_SPEAKER, 0x20);    /**< spk on,SPEAKER,select mixer output to speaker output    */
    ret |= write_reg(ctx, REG_28_ALC_CTRL, 0x00);   /**< alc set,ALC CONTROL, 0ms    */
    ret |= write_reg(ctx, REG_25_ADC_CTRL, 0x00);   /**< ADCVOLUME on,0db    */
    ret |= write_reg(ctx, REG_38_DAC_CTRL, 0x00);   /**< DACVOLUME on,0db    */
    ret |= write_reg(ctx, REG_37_DAC_CTRL, 0x30);   /**< dac set             */
    ret |= write_reg(ctx, REG_6D_GPIO_INT, 0x60);   /**< SEL:GPIO1=DMIC CLK OUT+SEL:GPIO2=PLL CLK OUT    */
    ret |= write_reg(ctx, 0x71, 0x05);              /**< for automute setting                            */
    ret |= write_reg(ctx, 0x73, 0x70);

    ret |= es8374_config_dac_output(ctx);            /**< 0x3c Enable DAC and Enable Lout/Rout/1/2 */
    ret |= es8374_config_adc_input(ctx, in_channel); /**< 0x00 LINSEL & RINSEL                     */

    ret |= es8374_set_volume_internal(ctx, 0);    /**<  vol = 0, -96db */
    ret |= write_reg(ctx, REG_37_DAC_CTRL, 0x00); /**<  dac set */

    if (ret != 0) {
        wm_log_error("init reg fail,ret=%d", ret);
    }
    return ret;
}

static int es8374_stop_do(wm_drv_codec_i2s_ctx_t *ctx)
{
    int ret     = 0;
    uint8_t reg = 0;

    if (ctx->cfg.type == WM_DRV_CODEC_I2S_MODE_INOUT_MIX_MIC_OUT) {
        ret |= read_reg(ctx, REG_1A_MONO_OUT_SEL, &reg); /**< disable lout      */
        reg |= 0x08;
        ret |= write_reg(ctx, REG_1A_MONO_OUT_SEL, reg);
        reg &= 0x9f;
        ret |= write_reg(ctx, REG_1A_MONO_OUT_SEL, reg);
        ret |= write_reg(ctx, REG_1D_MIXER_GAIN, 0x12); /**< mute speaker       */
        ret |= write_reg(ctx, REG_1E_SPEAKER, 0x20);    /**< disable class d    */
        ret |= read_reg(ctx, REG_1C_MIXER, &reg);       /**< disable spkmixer   */
        reg &= 0xbf;
        ret |= write_reg(ctx, REG_1C_MIXER, reg);
        ret |= write_reg(ctx, REG_1F_SPEAKER, 0x00); /**< spk set            */
    }

    if (ctx->cfg.type & WM_DRV_CODEC_I2S_MODE_OUT) {
        ret |= es8374_set_voice_mute(ctx, true);

        ret |= read_reg(ctx, REG_1A_MONO_OUT_SEL, &reg); /**< disable lout   */
        reg |= 0x08;
        ret |= write_reg(ctx, REG_1A_MONO_OUT_SEL, reg);
        reg &= 0xdf;
        ret |= write_reg(ctx, REG_1A_MONO_OUT_SEL, reg);
        ret |= write_reg(ctx, REG_1D_MIXER_GAIN, 0x12); /**< mute speaker    */
        ret |= write_reg(ctx, REG_1E_SPEAKER, 0x20);    /**< disable class d */
        ret |= read_reg(ctx, REG_15_ANALOG_PD, &reg);   /**< power up dac    */
        reg |= 0x20;
        ret |= write_reg(ctx, REG_15_ANALOG_PD, reg);
    }

    if (ctx->cfg.type & WM_DRV_CODEC_I2S_MODE_IN) {
        ret |= read_reg(ctx, REG_10_SDP, &reg); /**< power up adc and input  */
        reg |= 0xc0;
        ret |= write_reg(ctx, REG_10_SDP, reg);
        ret |= read_reg(ctx, REG_21_PGA, &reg); /**< power up adc and input  */
        reg |= 0xc0;
        ret |= write_reg(ctx, REG_21_PGA, reg);
    }

    if (ret != 0) {
        wm_log_error("es8374_stop_do fail,ret=%d", ret);
    }
    return ret;
}

static int es8374_start_do(wm_drv_codec_i2s_ctx_t *ctx)
{
    int ret     = WM_ERR_SUCCESS;
    uint8_t reg = 0;
    uint8_t output;

    bool mode_line = (ctx->cfg.type == WM_DRV_CODEC_I2S_MODE_INOUT_MIX_MIC_OUT);

    if (mode_line) {
        ret |= read_reg(ctx, REG_1A_MONO_OUT_SEL, &reg); /**< set monomixer */
        reg |= 0x60;
        reg |= 0x20;
        reg &= 0xf7;
        ret |= write_reg(ctx, REG_1A_MONO_OUT_SEL, reg);
        ret |= read_reg(ctx, REG_1C_MIXER, &reg); /**< set spk mixer  */
        reg |= 0x40;
        ret |= write_reg(ctx, REG_1C_MIXER, reg);
        ret |= write_reg(ctx, REG_1D_MIXER_GAIN, 0x02); /**< spk set */
        ret |= write_reg(ctx, REG_1F_SPEAKER, 0x00);    /**< spk set */
        ret |= write_reg(ctx, REG_1E_SPEAKER, 0xA0);    /**< spk on  */
    }

    if (mode_line || (ctx->cfg.type & WM_DRV_CODEC_I2S_MODE_IN)) {
        ret |= read_reg(ctx, REG_21_PGA, &reg); /**<  power up adc and input */
        reg &= 0x3f;
        ret |= write_reg(ctx, REG_21_PGA, reg);

        ret |= read_reg(ctx, REG_10_SDP, &reg); /**<  power up adc and input */
        reg &= 0x3f;
        ret |= write_reg(ctx, REG_10_SDP, reg); /**< unmute sdp */

        wm_log_info("set adc in");
    }

    if (mode_line || (ctx->cfg.type & WM_DRV_CODEC_I2S_MODE_OUT)) {
        ret |= read_reg(ctx, REG_1A_MONO_OUT_SEL, &reg); /**<  disable lout */
        reg |= 0x08;
        ret |= write_reg(ctx, REG_1A_MONO_OUT_SEL, reg);
        reg &= 0xdf;
        ret |= write_reg(ctx, REG_1A_MONO_OUT_SEL, reg);
        ret |= write_reg(ctx, REG_1D_MIXER_GAIN, 0x12); /**<  mute speaker     */
        ret |= write_reg(ctx, REG_1E_SPEAKER, 0x20);    /**<  disable class d  */
        ret |= read_reg(ctx, REG_15_ANALOG_PD, &reg);   /**<  power up dac     */
        reg &= 0xdf;
        ret |= write_reg(ctx, REG_15_ANALOG_PD, reg);
        ret |= read_reg(ctx, REG_1A_MONO_OUT_SEL, &reg); /**<  disable lout    */
        reg |= 0x20;
        ret |= write_reg(ctx, REG_1A_MONO_OUT_SEL, reg);
        reg &= 0xf7;
        ret |= write_reg(ctx, REG_1A_MONO_OUT_SEL, reg);
        ret |= write_reg(ctx, REG_1D_MIXER_GAIN, 0x02); /**<  mute speaker     */

        /**<  enable class d speaker output,select mixer output to speaker output */
        output = ((wm_dt_hw_codec_i2s_t *)ctx->hw)->codec_cfg.out_port;

        ret |= write_reg(ctx, REG_1E_SPEAKER, (output & WM_DRV_CODEC_I2S_OUTPORT_SPEAKER) ? 0xa0 : 0);
        ret |= es8374_set_voice_mute(ctx, false);
    }

    return ret;
}

static int es8374_init(wm_device_t *device, wm_drv_codec_i2s_cfg_t *cfg)
{
    int err;
    int ret                     = WM_ERR_SUCCESS;
    wm_drv_codec_i2s_ctx_t *ctx = NULL;
    wm_dt_hw_codec_i2s_t *hw    = (wm_dt_hw_codec_i2s_t *)device->hw;
    wm_dt_hw_i2s_t *i2s_hw      = NULL;
    uint8_t input               = 0;
    uint8_t output              = 0;

    if (!(device && es8374_check_cfg(cfg) == WM_ERR_SUCCESS)) {
        WM_DRV_CODEC_LOGE("bad param");
        return WM_ERR_INVALID_PARAM;
    }

    ctx              = (wm_drv_codec_i2s_ctx_t *)(device->drv);
    ctx->cfg         = *cfg;
    ctx->pri         = NULL;
    ctx->i2s_dev     = wm_dt_get_device_by_name(hw->i2s_device_name);
    ctx->hw          = hw;
    ctx->sample_rate = WM_8374_DEF_SMAPLE;
    ctx->cfg.mclk    = WM_8374_DEF_MCLK;
    ctx->power_pin   = WM_GPIO_NUM_MAX; //TODO use dt config gpio

    if (!ctx->i2s_dev) {
        WM_DRV_CODEC_LOGE("i2s dev fail");
        return WM_ERR_FAILED;
    }

    i2s_hw = (wm_dt_hw_i2s_t *)ctx->i2s_dev->hw;

    if (ctx->cfg.mclk == 0) {
        ctx->cfg.mclk = i2s_hw->i2s_cfg.mclk_hz;
    }

    err = es8374_init_ctrl(device, ctx);
    if (err != WM_ERR_SUCCESS) {
        WM_DRV_CODEC_LOGE("ctrl fail");
        return err;
    }

    input  = hw->codec_cfg.in_port;
    output = hw->codec_cfg.out_port;

    es8374_ctrl_setup_jack_pin(hw);

    ret |= es8374_stop_do(ctx);
    ret |= es8374_init_reg(ctx, (WM_ES8374_FMT_16BITS << 4) | WM_I2S_STD_I2S, WM_ES8374_MCLK_DIV_4, WM_ES8374_LCLK_DIV_256,
                           input, output);
    ret |= es8374_set_mic_gain(ctx, 21);
    ret |= es8374_set_df2se_pga(ctx, DF2SE_PGA_GAIN_EN);

    if (ret != 0) {
        WM_DRV_CODEC_LOGE("check init ret fail.ret=%d", ret);
        return WM_ERR_FAILED;
    }

    return ret;
}

static int es8374_deinit(wm_device_t *device)
{
    wm_drv_codec_i2s_ctx_t *ctx = (wm_drv_codec_i2s_ctx_t *)(device->drv);

    es8374_stop_do(ctx);
    write_reg(ctx, REG_00_RESET, 0x7F); /**< IC STOP */
    es8374_pa_power(ctx, false);

    return WM_ERR_SUCCESS;
}

static int es8374_set_speed(wm_drv_codec_i2s_ctx_t *ctx, uint32_t sample_rate_hz)
{
    int ret = 0;
    uint8_t reg_adc;
    uint8_t reg_dac;

    ret |= read_reg(ctx, REG_24_ADC_CTRL, &reg_adc);
    ret |= read_reg(ctx, REG_37_DAC_CTRL, &reg_dac);

    reg_adc &= 0xbf; /**< single speed */
    reg_dac &= 0x7f;

    ret |= write_reg(ctx, REG_24_ADC_CTRL, reg_adc);
    ret |= write_reg(ctx, REG_37_DAC_CTRL, reg_dac);

    return WM_ERR_SUCCESS;
}

static int es8374_set_format(wm_device_t *device, uint32_t sample_rate_hz, enum wm_i2s_bits bits, enum wm_i2s_chan_type channel)
{
    wm_drv_codec_i2s_ctx_t *ctx = (wm_drv_codec_i2s_ctx_t *)(device->drv);

    int ret       = 0;
    uint32_t mclk = 0;
    uint8_t reg   = 0;

    wm_drv_codec_i2s_bits_t codec_bits = 0;
    uint8_t pll_N;
    uint32_t pll_K;

    if (sample_rate_hz < 8000 || sample_rate_hz > 96000) {
        wm_log_error("bad sample =%d", sample_rate_hz);
        return WM_ERR_INVALID_PARAM;
    }

    if (sample_rate_hz <= 14000) {
        mclk = 3200000;
    } else if (sample_rate_hz <= 28000) {
        mclk = 5000000;
    } else if (sample_rate_hz <= 48000) {
        mclk = 10000000;
    } else if (sample_rate_hz <= 96000) {
        mclk = 20000000;
    } else {
        wm_log_error("bad sample");
        return WM_ERR_INVALID_PARAM;
    }

    if (bits == WM_I2S_BITS_16BIT) {
        codec_bits = WM_ES8374_FMT_16BITS;
    } else if (bits == WM_I2S_BITS_32BIT) {
        codec_bits = WM_ES8374_FMT_32BITS;
    } else {
        wm_log_error("bad bits");
        return WM_ERR_INVALID_PARAM;
    }

    if (!(channel > 0 && channel <= WM_I2S_CHAN_TYPE_STEREO)) {
        wm_log_error("bad channel");
        return WM_ERR_INVALID_PARAM;
    }

    wm_log_info("set %p,%d,%d,%d", ctx->i2s_dev, sample_rate_hz, bits, channel);

    ret = wm_drv_i2s_set_format(ctx->i2s_dev, sample_rate_hz, bits, channel);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("i2s set format fail,ret=%d", ret);
        return ret;
    }

    ret = wm_drv_i2s_set_mclk(ctx->i2s_dev, mclk);
    if (ret != WM_ERR_SUCCESS) {
        wm_log_error("i2s set mclk fail");
    }

    WM_ES8374_MDELAY(5);

    ctx->cfg.mclk    = mclk;
    ctx->sample_rate = sample_rate_hz;

    ret |= read_reg(ctx, REG_01_CLKM, &reg);

    reg &= 0x7f; /**< mclk not divide by 2  */

    ret |= write_reg(ctx, REG_01_CLKM, reg);

    es8374_set_speed(ctx, sample_rate_hz);
    es8374_get_pll_param(ctx, 8, &pll_N, &pll_K);

    WM_ES8374_MDELAY(5);

    ret |= write_reg(ctx, REG_0C_CLKM, (pll_K >> 16) & 0xff); /**< pll set:k                  */
    ret |= write_reg(ctx, REG_0D_CLKM, (pll_K >> 8) & 0xff);  /**< pll set:k                  */
    ret |= write_reg(ctx, REG_0E_CLKM, pll_K & 0xff);         /**< pll set:k                  */
    ret |= write_reg(ctx, REG_0B_CLKM, pll_N);                /**< pll set:n                  */

    ret |= es8374_cfg_fmt(ctx, ctx->cfg.std);
    ret |= es8374_set_bits_per_sample(ctx, codec_bits);

    return ret;
}

static int es8374_start(wm_device_t *device)
{
    int ret;
    wm_drv_codec_i2s_ctx_t *ctx = (wm_drv_codec_i2s_ctx_t *)(device->drv);

    if (ctx->is_start) {
        WM_DRV_CODEC_LOGE("start before");
        return WM_ERR_SUCCESS;
    }

    ret = es8374_start_do(ctx);

    es8374_pa_power(ctx, true);

    return ret;
}

static int es8374_stop(wm_device_t *device)
{
    int ret;
    wm_drv_codec_i2s_ctx_t *ctx = (wm_drv_codec_i2s_ctx_t *)(device->drv);

    if (!ctx->is_start) {
        WM_DRV_CODEC_LOGE("not start");
        return WM_ERR_SUCCESS;
    }

    es8374_pa_power(ctx, false);
    ret = es8374_stop_do(ctx);
    write_reg(ctx, REG_00_RESET, 0x7F); /**< IC STOP */

    return ret;
}

static int es8374_set_mute(wm_device_t *device, bool mute)
{
    return es8374_set_voice_mute((wm_drv_codec_i2s_ctx_t *)(device->drv), mute);
}

static int es8374_set_volume(wm_device_t *device, int vol)
{
    wm_drv_codec_i2s_ctx_t *ctx = (wm_drv_codec_i2s_ctx_t *)(device->drv);

    return es8374_set_volume_internal(ctx, vol);
}

static int es8374_set_mic_mute(wm_device_t *device, bool mute)
{
    wm_drv_codec_i2s_ctx_t *ctx = (wm_drv_codec_i2s_ctx_t *)(device->drv);
    wm_dt_hw_codec_i2s_t *hw    = (wm_dt_hw_codec_i2s_t *)device->hw;

    uint8_t input = (uint8_t)(hw->codec_cfg.in_port);

    if (mute) {
        input = 0; /**< input none, mute */
    }

    return es8374_config_adc_input(ctx, input);
}

static int es8374_set_mic_volume(wm_device_t *device, int vol)
{
    return es8374_set_mic_volume_internal((wm_drv_codec_i2s_ctx_t *)(device->drv), vol);
}

static int es8374_set_reg(wm_device_t *device, int reg, int value)
{
    if (!(reg >= 0 && reg <= 0x7F && value >= 0 && value <= 0xff)) {
        return WM_ERR_INVALID_PARAM;
    }
    return write_reg((wm_drv_codec_i2s_ctx_t *)(device->drv), reg, (uint8_t)value);
}

static int es8374_get_reg(wm_device_t *device, int reg, int *value)
{
    int err;
    uint8_t v;

    if (!(reg >= 0 && reg <= 0x7F && value)) {
        return WM_ERR_INVALID_PARAM;
    }

    err = read_reg((wm_drv_codec_i2s_ctx_t *)(device->drv), reg, &v);
    if (err == WM_ERR_SUCCESS) {
        *value = v;
    }
    return err;
}

static int es8374_dump(wm_device_t *device)
{
#define WM_ES8374_DUMP_REG_NUM 0x80

    wm_drv_codec_i2s_ctx_t *ctx = (wm_drv_codec_i2s_ctx_t *)(device->drv);

    for (int i = 0; i < WM_ES8374_DUMP_REG_NUM; i++) {
        uint8_t value = 0;
        int ret       = read_reg(ctx, i, &value);
        if (ret != WM_ERR_SUCCESS) {
            break;
        }
        wm_log_raw_info("0X%02X : 0X%02X\r\n", i, value);
    }
    return WM_ERR_SUCCESS;
}

const wm_drv_codec_i2s_ops_t wm_drv_codec_i2s_es8374_ops = {
    .init   = es8374_init,
    .deinit = es8374_deinit,

    .set_format = es8374_set_format,

    .start = es8374_start,
    .stop  = es8374_stop,

    .set_mute       = es8374_set_mute,
    .set_volume     = es8374_set_volume,
    .set_mic_mute   = es8374_set_mic_mute,
    .set_mic_volume = es8374_set_mic_volume,

    .set_reg = es8374_set_reg,
    .get_reg = es8374_get_reg,

    .dump = es8374_dump,
};

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
#include "wm_dt.h"
#include "wm_osal.h"
#include "wm_reg_op.h"
#include "wm_soc.h"
#include "wm_error.h"
#include "wm_drv_codec_i2s.h"
#include "wm_drv_i2c.h"
#include "wm_dt_hw.h"

#define WM_DRV_CODEC_DEBUG

#ifdef WM_DRV_CODEC_DEBUG
#include "wm_log.h"
#undef LOG_TAG
#define LOG_TAG                 "CODEC DRV"

#define WM_DRV_CODEC_PIN_OFFSET 0

#define WM_DRV_CODEC_LOG_D(...) wm_log_info(__VA_ARGS__)
#define WM_DRV_CODEC_LOG_I(...) wm_log_info(__VA_ARGS__)
#define WM_DRV_CODEC_LOG_W(...) wm_log_warn(__VA_ARGS__)
#define WM_DRV_CODEC_LOG_E(...) wm_log_error(__VA_ARGS__)

#define WM_DRV_CODEC_LOG_MARKER WM_DRV_CODEC_LOG_E("%s@%d", __func__, __LINE__)

#define WM_DRV_CODEC_ASSERT(v)  assert(v)

#else

#define WM_DRV_CODEC_PIN_OFFSET 0

#define WM_DRV_CODEC_LOG_D(...)
#define WM_DRV_CODEC_LOG_I(...)
#define WM_DRV_CODEC_LOG_E(...)
#define WM_DRV_CODEC_ASSERT(...)

#define WM_DRV_CODEC_LOG_MARKER

#define dump_cfgs(arg...) \
    do {                  \
    } while (0)

#define pulse(a, b)
#define pulse_init()
#endif

#define WM_DRV_CODEC_TIMEOUT     1000
#define WM_DRV_CODEC_I2C_SPEED   WM_I2C_SPEED_STANDARD
#define WM_DRV_CODEC_I2C_GI_MS   4

#define WM_DRV_CODEC_PRE_CHECK() (!device->drv || !((wm_drv_codec_data_t *)(device->drv))->ctx.lock)

#define WM_DRV_CODEC_TRY_LOCK() \
    (WM_ERR_SUCCESS == wm_os_internal_mutex_acquire(((wm_drv_codec_data_t *)(device->drv))->ctx.lock, WM_DRV_CODEC_TIMEOUT))

#define WM_DRV_CODEC_UNLOCK()        (WM_ERR_SUCCESS == wm_os_internal_mutex_release(((wm_drv_codec_data_t *)(device->drv))->ctx.lock))

#define WM_DRV_CODEC_OPS_CHECK(d, m) !!((wm_drv_codec_ops_t *)(d->ops)->m)

#define CLS_BITS(r, s, n)            ((r) &= ~(((1 << (n)) - 1) << (s)))

#define WM_DRV_CODEC_ES8374_READ_RET(data, addr, value)    \
    ret = wm_drv_codec_es8374_reg_read(data, addr, value); \
    if (WM_ERR_SUCCESS != ret) {                           \
        WM_DRV_CODEC_LOG_MARKER;                           \
        return ret;                                        \
    }

#define WM_DRV_CODEC_ES8374_WRITE_RET(data, addr, value)    \
    ret = wm_drv_codec_es8374_reg_write(data, addr, value); \
    if (WM_ERR_SUCCESS != ret) {                            \
        WM_DRV_CODEC_LOG_MARKER;                            \
        return ret;                                         \
    }

#define WM_DRV_CODEC_ES8374_AND_RET(data, addr, value)    \
    ret = wm_drv_codec_es8374_reg_and(data, addr, value); \
    if (WM_ERR_SUCCESS != ret) {                          \
        WM_DRV_CODEC_LOG_MARKER;                          \
        return ret;                                       \
    }

#define WM_DRV_CODEC_ES8374_OR_RET(data, addr, value)    \
    ret = wm_drv_codec_es8374_reg_or(data, addr, value); \
    if (WM_ERR_SUCCESS != ret) {                         \
        WM_DRV_CODEC_LOG_MARKER;                         \
        return ret;                                      \
    }

#define ES8374_MDELAY(ms)            wm_os_internal_time_delay_ms(ms)

/* ES8374 REG defines */
#define ES8374_RESET_ADDR            (0x00)
#define ES8374_CLK_MGM1_ADDR         (0x01)
#define ES8374_CLK_MGM2_ADDR         (0x02)
#define ES8374_PLL_MODE_EN_ADDR      (0x6F)
#define ES8374_PLL_MODE_SET_ADDR     (0x72)
#define ES8374_PLL_PDN_VCO_ADDR      (0x09)
#define ES8374_PLL_SET_ADDR          (0x0A)
#define ES8374_PLL_N_ADDR            (0x0B)
#define ES8374_PLL_K1_ADDR           (0x0C)
#define ES8374_PLL_K2_ADDR           (0x0D)
#define ES8374_PLL_K3_ADDR           (0x0E)
#define ES8374_I2S_CLK_ADDR          (0x0F)
#define ES8374_ADC_CTRL_ADDR         (0x24)
#define ES8374_DAC_CTRL_ADDR1        (0x36)
#define ES8374_ADC_CLK_DIV_ADDR      (0x05)
#define ES8374_ADC_LRCLK_DIV_HI_ADDR (0x06)
#define ES8374_ADC_LRCLK_DIV_LO_ADDR (0x07)
#define ES8374_I2S_ADC_PROTOCOL_ADDR (0x10)
#define ES8374_I2S_DAC_PROTOCOL_ADDR (0x11)
#define ES8374_TIMING_SET1_ADDR      (0x12)
#define ES8374_TIMING_SET2_ADDR      (0x13)
#define ES8374_POWER1                (0x14)
#define ES8374_POWER2                (0x15)
#define ES8374_MONOOUT_MIXER_SEL     (0x1A)
#define ES8374_MONOOUT_MIXER_GAIN    (0x1B)
#define ES8374_SPK_MIXER_SEL         (0x1C)
#define ES8374_SPK_MIXER_GAIN        (0x1D)
#define ES8374_SPEAKER_PROTECT_ADDR  (0x1F)
#define ES8374_SPEAKER_SEL_GAIN      (0x1E)
#define ES8374_LIN_PGA_ADDR          (0x21) //need set the used line to 0
#define ES8374_MONO_PGA_ADDR         (0x22)
#define ES8374_ADC_VOLUM_ADDR        (0x25)
#define ES8374_ALC_ADDR              (0x28)
#define ES8374_DAC_VOLUM_ADDR        (0x38)
#define ES8374_DAC_CTRL_ADDR2        (0x37)
#define ES8374_JACK_ADDR             (0x2C)
#define ES8374_DMIC_ADDR             (0x6D)
#define ES8374_UNKOWN71_ADDR         (0x71)
#define ES8374_UNKOWN73_ADDR         (0x73)

/* ES8374 REG Filed defines */
#define ES8374_ADC_VOLUME_DEF        (80)
#define ES8374_MCLK_CO_VALUE         (256)
#define ES8374_FIELD_SHIFT(v, s)     (uint8_t)((v) << (s))
#define ES8374_RESET_START           (0x3F)
#define ES8374_RESET_STOP            (0x03)
#define ES8374_RESET_IC_START        (0x80)
#define ES8374_CLOCK_MGM1_ON         (0x7F)
#define ES8374_CLOCK_MGM2_PLL_ON     (WM_BIT(3))
#define ES8374_CLOCK_MCLK_BYPASS     (~(WM_BIT(7)))
#define ES8374_CLOCK_MCLK_DIV2       (WM_BIT(7)) /* set to div2 if mclk src is bigger than 19.2MHz */
#define ES8374_PLL_MODE_EN_VALUE     (0xA0)
#define ES8374_PLL_MODE_SET_VALUE    (0x41)
#define ES8374_PLL_MODE_DSM_VALUE    (0x40)
#define ES8374_PLL_SET_VALUE         (0x8A)
#define ES8374_PLL_DIGITAL_ON        (WM_BIT(6))
#define ES8374_PLL_MCLK_THRESH_VALUE (19200000)
#define ES8374_SAMPLE_RATE_VIA_SPEED (48000)
#define ES8374_I2S_CLK_MASTER_VALUE  (WM_BIT(7))
#define ES8374_I2S_BCLK_INVERSE      (WM_BIT(5))
#define ES8374_I2S_BCLK_DIV_MASK     (0x1F)
#define ES8374_TIMING_SET1_VALUE     (0x30)
#define ES8374_TIMING_SET2_VALUE     (0x20)
#define ES8374_LINSEL_SHIFT_VALUE    (4)
#define ES8374_LINSEL_MASK           (0x03 << (ES8374_LINSEL_SHIFT_VALUE))
#define ES8374_LINSEL_LIN1_VALUE     (0x0 << (ES8374_LINSEL_SHIFT_VALUE))
#define ES8374_LINSEL_LIN2_VALUE     (0x1 << (ES8374_LINSEL_SHIFT_VALUE))
#define ES8374_ADC_DAC_CLK_DIV_VALUE (0x11)
#define ES8374_LIN_ON(s)             (0x04 | ((s) << 4))
#define ES8374_LIN_OFF(s)            (0x40 | ((s) << 4))
#define ES8374_LIN_GAIN_ON           (0x55)
#define ES8374_LIN_GAIN_OFF          (0xFF)
#define ES8374_LIN_SEL(data, on, off)                                                   \
    {                                                                                   \
        WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_LIN_PGA_ADDR, ES8374_LIN_OFF(off));  \
        WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_MONO_PGA_ADDR, ES8374_LIN_GAIN_OFF); \
        WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_LIN_PGA_ADDR, ES8374_LIN_ON(on));    \
        WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_MONO_PGA_ADDR, ES8374_LIN_GAIN_ON);  \
    }
#define ES8374_POWER1_ON            (0X8A)
#define ES8374_POWER2_ON            (0X40)

#define ES8374_VCO_MASK             (0x3)
#define ES8374_VCO_BYPASS           (0)
#define ES8374_VCO_DIV8             (1) /* by default */
#define ES8374_VCO_DIV4             (2)
#define ES8374_VCO_DIV2             (3)
#define ES8374_VCO_DIV_DEF_VALUE    (8)
#define ES8374_VCO_DIV_DEF_IDX      (1)

#define ES8374_I2S_FMT_16_VALUE     (3)
#define ES8374_I2S_FMT_18_VALUE     (2)
#define ES8374_I2S_FMT_20_VALUE     (1)
#define ES8374_I2S_FMT_24_VALUE     (0)
#define ES8374_I2S_FMT_32_VALUE     (4)
#define ES8374_I2S_FMT_SHIFT(v)     (v << 2)

#define ES8374_I2S_STD_I2S_VALUE    (0)
#define ES8374_I2S_STD_MSB_VALUE    (1)
#define ES8374_I2S_STD_PCM_VALUE    (3)
#define ES8374_I2S_STD_SHIFT(v)     (v << 0)
#define ES8374_I2S_STD_PCMA_VALUE   (0)
#define ES8374_I2S_STD_PCMB_VALUE   (1)
#define ES8374_I2S_STD_PCM_SHIFT(v) (v << 5)

#define ES8374_HANDLE_RET(data, handle)   \
    if (WM_ERR_SUCCESS != handle(data)) { \
        WM_DRV_CODEC_LOG_MARKER;          \
        return WM_ERR_FAILED;             \
    }

#define ES8374_READ_DUMP(data, addr, rbuf)                                   \
    if (WM_ERR_SUCCESS != wm_drv_codec_es8374_reg_read(data, addr, &rbuf)) { \
        WM_DRV_CODEC_LOG_MARKER;                                             \
        return WM_ERR_FAILED;                                                \
    } else {                                                                 \
        WM_DRV_CODEC_LOG_I("0x%02x = 0x%02x", addr, rbuf);                   \
    }

typedef struct {
    uint8_t k1;
    uint8_t k2;
    uint8_t k3;
    uint8_t n;
    uint32_t pll_mclk_hz; /* actual mclk for pll that may be div by 2 */
    uint32_t imclk;
    wm_dt_hw_codec_es8374_cfg_t *hw_cfg;
    wm_drv_i2c_config_t i2c_cfg;
    wm_drv_codec_i2s_cfg_t cfg;
} wm_drv_codec_priv_t;

typedef struct {
    wm_os_mutex_t *lock;
    wm_device_t *i2s_dev;  /**< i2s is initialized in codec driver */
    wm_device_t *i2c_dev;  /**< used for control codec, i2c should be initialized in upper layer */
    wm_device_t *gpio_dev; /**< used for notification to user, gpio should be initialized in upper layer */
    wm_drv_codec_priv_t priv;
} wm_drv_codec_ctx_t;

typedef struct {
    wm_drv_codec_ctx_t ctx;
} wm_drv_codec_data_t;

static int wm_drv_codec_es8374_start_playback(wm_drv_codec_data_t *data);

static int wm_drv_codec_es8374_reg_read(wm_drv_codec_data_t *data, uint8_t addr, uint8_t *rbuf)
{
    wm_drv_codec_ctx_t *ctx      = &data->ctx;
    wm_drv_i2c_config_t *i2c_cfg = &ctx->priv.i2c_cfg;

    ES8374_MDELAY(WM_DRV_CODEC_I2C_GI_MS);

    return wm_drv_i2c_read(ctx->i2c_dev, i2c_cfg, &addr, 1, rbuf, 1);
}

static int wm_drv_codec_es8374_reg_write(wm_drv_codec_data_t *data, uint8_t addr, uint8_t wbuf)
{
    wm_drv_codec_ctx_t *ctx      = &data->ctx;
    wm_drv_i2c_config_t *i2c_cfg = &ctx->priv.i2c_cfg;

    int ret = wm_drv_i2c_write(ctx->i2c_dev, i2c_cfg, &addr, 1, &wbuf, 1);

    ES8374_MDELAY(WM_DRV_CODEC_I2C_GI_MS);

    WM_DRV_CODEC_LOG_I("[0x%02x = 0x%02x] updated!", addr, wbuf);

    return ret;
}

static int wm_drv_codec_es8374_reg_and(wm_drv_codec_data_t *data, uint8_t addr, uint8_t wbuf)
{
    int ret;
    uint8_t rbuf = 0;

    ret = wm_drv_codec_es8374_reg_read(data, addr, &rbuf);
    if (WM_ERR_SUCCESS != ret) {
        WM_DRV_CODEC_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    rbuf &= wbuf;

    WM_DRV_CODEC_ES8374_WRITE_RET(data, addr, rbuf);

    return WM_ERR_SUCCESS;
}

static int wm_drv_codec_es8374_reg_or(wm_drv_codec_data_t *data, uint8_t addr, uint8_t wbuf)
{
    int ret;
    uint8_t rbuf = 0;

    ret = wm_drv_codec_es8374_reg_read(data, addr, &rbuf);
    if (WM_ERR_SUCCESS != ret) {
        WM_DRV_CODEC_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    rbuf |= wbuf;

    WM_DRV_CODEC_ES8374_WRITE_RET(data, addr, rbuf);

    return WM_ERR_SUCCESS;
}

static int wm_drv_codec_es8374_cal_pll_n_k(wm_drv_codec_data_t *data)
{
    wm_drv_codec_ctx_t *ctx     = &data->ctx;
    wm_drv_codec_priv_t *priv   = &ctx->priv;
    wm_drv_codec_i2s_cfg_t *cfg = &priv->cfg;

    uint32_t r;
    double N, pll_out, vco_out;

    /* XXX:
     * int(2^22 * (N - n) * 0.65735982222960)
     *
     * 2^22 * 0.65735982222960 = 2757166.93181690;
     * */
    double co = 2757166.9318169001984;

    priv->imclk = cfg->sample_rate_hz * ES8374_MCLK_CO_VALUE;

    pll_out = priv->imclk;

    vco_out = pll_out * ES8374_VCO_DIV_DEF_VALUE;

    N = vco_out / priv->pll_mclk_hz;
    /* WM_DRV_CODEC_LOG_I("vco:%lu, mclk:%lu, N:%d", (uint32_t)vco_out, priv->pll_mclk_hz, (uint32_t)N); */

    priv->n = (uint8_t)(N);

    r = (uint32_t)((N - priv->n) * co);

    priv->k3 = r & 0xFF;
    priv->k2 = (r >> 8) & 0xFF;
    priv->k1 = (r >> 16) & 0x3F;

    WM_DRV_CODEC_LOG_I("k1:0x%2x, k2:0x%2x, k3:0x%2x", priv->k1, priv->k2, priv->k3);

    return WM_ERR_SUCCESS;
}

static int wm_drv_codec_es8374_reset(wm_drv_codec_data_t *data)
{
    int ret;

    WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_RESET_ADDR, ES8374_RESET_START);

    WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_RESET_ADDR, ES8374_RESET_STOP);

    return WM_ERR_SUCCESS;
}

static int wm_drv_codec_es8374_init_clock_gate(wm_drv_codec_data_t *data)
{
    int ret;
    wm_drv_codec_i2s_cfg_t *cfg = &data->ctx.priv.cfg;

    WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_CLK_MGM1_ADDR, ES8374_CLOCK_MGM1_ON);

    /* TODO, XXX */
    if (cfg->mclk_hz >= ES8374_PLL_MCLK_THRESH_VALUE) {
        ret = wm_drv_codec_es8374_reg_or(data, ES8374_CLK_MGM1_ADDR, ES8374_FIELD_SHIFT(1, 7));
        if (WM_ERR_SUCCESS != ret) {
            WM_DRV_CODEC_LOG_MARKER;
            return ret;
        }

        data->ctx.priv.pll_mclk_hz = cfg->mclk_hz >> 1;
    } else {
        data->ctx.priv.pll_mclk_hz = cfg->mclk_hz;
    }

    return WM_ERR_SUCCESS;
}

static int wm_drv_codec_es8374_init_clock(wm_drv_codec_data_t *data)
{
    int ret;

    WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_PLL_MODE_EN_ADDR, ES8374_PLL_MODE_EN_VALUE);

    WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_PLL_MODE_SET_ADDR, ES8374_PLL_MODE_SET_VALUE);

    WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_PLL_PDN_VCO_ADDR, ES8374_VCO_DIV_DEF_IDX);

    wm_drv_codec_es8374_cal_pll_n_k(data);

    WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_PLL_K1_ADDR, data->ctx.priv.k1);

    WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_PLL_K2_ADDR, data->ctx.priv.k2);

    WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_PLL_K3_ADDR, data->ctx.priv.k3);

    WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_PLL_SET_ADDR, ES8374_PLL_SET_VALUE);

    WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_PLL_N_ADDR, data->ctx.priv.n);

    WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_PLL_PDN_VCO_ADDR, ES8374_VCO_DIV_DEF_IDX | ES8374_PLL_MODE_DSM_VALUE);

    /* Enable PLL to generate IMCLK based on the MCLK */
    WM_DRV_CODEC_ES8374_OR_RET(data, ES8374_CLK_MGM2_ADDR, ES8374_CLOCK_MGM2_PLL_ON);

    return WM_ERR_SUCCESS;
}

static int wm_drv_codec_es8374_set_dac_adc_clk_div(wm_drv_codec_data_t *data)
{
    int ret;

    wm_drv_codec_ctx_t *ctx     = &data->ctx;
    wm_drv_codec_priv_t *priv   = &ctx->priv;
    wm_drv_codec_i2s_cfg_t *cfg = &priv->cfg;

    /* DACLRCLK DIV settings */
    uint16_t daclrck_div = priv->imclk / cfg->sample_rate_hz;

    uint8_t daclrck_div_lo = daclrck_div & 0xFF;

    uint8_t daclrck_div_hi = (daclrck_div >> 8) & 0xF;

    uint8_t daclrck_div_hi_old = 0;

    ret = wm_drv_codec_es8374_reg_read(data, ES8374_ADC_LRCLK_DIV_HI_ADDR, &daclrck_div_hi_old);
    if (WM_ERR_SUCCESS != ret) {
        WM_DRV_CODEC_LOG_MARKER;
        return ret;
    }

    CLS_BITS(daclrck_div_hi_old, 0, 4);
    daclrck_div_hi |= daclrck_div_hi_old;

    WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_ADC_LRCLK_DIV_LO_ADDR, daclrck_div_lo);

    WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_ADC_LRCLK_DIV_HI_ADDR, daclrck_div_hi);

    WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_ADC_CLK_DIV_ADDR, ES8374_ADC_DAC_CLK_DIV_VALUE);

    WM_DRV_CODEC_LOG_I("adc lrhi[%d], lrlo[%d]", daclrck_div_hi, daclrck_div_lo);

    return WM_ERR_SUCCESS;
}

static int wm_drv_codec_es8374_i2s_clk(wm_drv_codec_data_t *data)
{
    int ret;
    uint8_t bclk_div = 0, bclk_div_idx = 1, value = 0;

    uint32_t bitrates = 0;

    wm_drv_codec_ctx_t *ctx     = &data->ctx;
    wm_drv_codec_priv_t *priv   = &ctx->priv;
    wm_drv_codec_i2s_cfg_t *cfg = &priv->cfg;

    /* XXX: ES8374 support mono only, stereo should multiple 2 */
    switch (cfg->fmt) {
        case WM_DRV_CODEC_I2S_FMT_16BIT:
        {
            bitrates = cfg->sample_rate_hz * 16;
            break;
        }
        case WM_DRV_CODEC_I2S_FMT_18BIT:
        {
            bitrates = cfg->sample_rate_hz * 18;
            break;
        }
        case WM_DRV_CODEC_I2S_FMT_20BIT:
        {
            bitrates = cfg->sample_rate_hz * 20;
            break;
        }
        case WM_DRV_CODEC_I2S_FMT_24BIT:
        {
            bitrates = cfg->sample_rate_hz * 24;
            break;
        }
        case WM_DRV_CODEC_I2S_FMT_32BIT:
        {
            bitrates = cfg->sample_rate_hz * 32;
            break;
        }
        default:
        {
            WM_DRV_CODEC_LOG_MARKER;
            return WM_ERR_NO_SUPPORT;
        }
    }

    bitrates *= cfg->ctype;

    if ((ctx->priv.imclk % bitrates) != 0) {
        WM_DRV_CODEC_LOG_MARKER;
        return WM_ERR_NO_SUPPORT;
    }

    bclk_div = ctx->priv.imclk / bitrates;

    switch (bclk_div) {
        case 1 ... 18:
        {
            bclk_div_idx = bclk_div;
            break;
        }
        case 20:
        {
            bclk_div_idx = 19;
            break;
        }
        case 22:
        {
            bclk_div_idx = 20;
            break;
        }
        case 24:
        {
            bclk_div_idx = 21;
            break;
        }
        case 25:
        {
            bclk_div_idx = 22;
            break;
        }
        case 30:
        {
            bclk_div_idx = 23;
            break;
        }
        case 32:
        {
            bclk_div_idx = 24;
            break;
        }
        case 33:
        {
            bclk_div_idx = 25;
            break;
        }
        case 34:
        {
            bclk_div_idx = 26;
            break;
        }
        case 36:
        {
            bclk_div_idx = 27;
            break;
        }
        case 44:
        {
            bclk_div_idx = 28;
            break;
        }
        case 48:
        {
            bclk_div_idx = 29;
            break;
        }
        case 66:
        {
            bclk_div_idx = 30;
            break;
        }
        case 72:
        {
            bclk_div_idx = 31;
            break;
        }
        default:
        {
            WM_DRV_CODEC_LOG_MARKER;
            return WM_ERR_FAILED;
        }
    }

    WM_DRV_CODEC_LOG_I("bclk_div:%d, idx:%d", bclk_div, bclk_div_idx);

    if (WM_DRV_CODEC_I2S_CLK_MASTER == cfg->i2s_mode) {
        value = (ES8374_FIELD_SHIFT(1, 7) | bclk_div_idx);
    } else {
        value = (bclk_div_idx);
    }

    WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_I2S_CLK_ADDR, value);

    return WM_ERR_SUCCESS;
}

static int wm_drv_codec_es8374_i2s_protocol(wm_drv_codec_data_t *data, uint8_t addr)
{
    int ret;
    uint8_t value;
    uint8_t adcwl = 0, adcformat = 0, pcmb = 0;

    wm_drv_codec_ctx_t *ctx     = &data->ctx;
    wm_drv_codec_priv_t *priv   = &ctx->priv;
    wm_drv_codec_i2s_cfg_t *cfg = &priv->cfg;

    switch (cfg->fmt) {
        case WM_DRV_CODEC_I2S_FMT_16BIT:
        {
            adcwl = ES8374_I2S_FMT_16_VALUE;
            break;
        }
        case WM_DRV_CODEC_I2S_FMT_18BIT:
        {
            adcwl = ES8374_I2S_FMT_18_VALUE;
            break;
        }
        case WM_DRV_CODEC_I2S_FMT_20BIT:
        {
            adcwl = ES8374_I2S_FMT_20_VALUE;
            break;
        }
        case WM_DRV_CODEC_I2S_FMT_24BIT:
        {
            adcwl = ES8374_I2S_FMT_24_VALUE;
            break;
        }
        case WM_DRV_CODEC_I2S_FMT_32BIT:
        {
            adcwl = ES8374_I2S_FMT_32_VALUE;
            break;
        }
        default:
        {
            WM_DRV_CODEC_LOG_MARKER;
            return WM_ERR_FAILED;
        }
    }

    switch (cfg->std) {
        case WM_DRV_CODEC_I2S_STD_I2S:
        {
            adcformat = ES8374_I2S_STD_I2S_VALUE;
            break;
        }
        case WM_DRV_CODEC_I2S_STD_MSB:
        {
            adcformat = ES8374_I2S_STD_MSB_VALUE;
            break;
        }
        case WM_DRV_CODEC_I2S_STD_DSPA:
        {
            adcformat = ES8374_I2S_STD_PCM_VALUE;
            break;
        }
        case WM_DRV_CODEC_I2S_STD_DSPB:
        {
            adcformat = ES8374_I2S_STD_PCM_VALUE;
            pcmb      = 1;
            break;
        }
        default:
        {
            WM_DRV_CODEC_LOG_MARKER;
            return WM_ERR_FAILED;
        }
    }

    value = ES8374_I2S_FMT_SHIFT(adcwl) | ES8374_I2S_STD_SHIFT(adcformat) | ES8374_I2S_STD_PCM_SHIFT(pcmb);

    WM_DRV_CODEC_ES8374_WRITE_RET(data, addr, value);

    return WM_ERR_SUCCESS;
}

static int wm_drv_codec_es8374_i2s_adc_protocol(wm_drv_codec_data_t *data)
{
    return wm_drv_codec_es8374_i2s_protocol(data, ES8374_I2S_ADC_PROTOCOL_ADDR);
}

static int wm_drv_codec_es8374_i2s_dac_protocol(wm_drv_codec_data_t *data)
{
    return wm_drv_codec_es8374_i2s_protocol(data, ES8374_I2S_DAC_PROTOCOL_ADDR);
}

static int wm_drv_codec_es8374_i2s_adc_speed_init(wm_drv_codec_data_t *data)
{
    int ret;
    uint8_t value               = 0;
    wm_drv_codec_ctx_t *ctx     = &data->ctx;
    wm_drv_codec_priv_t *priv   = &ctx->priv;
    wm_drv_codec_i2s_cfg_t *cfg = &priv->cfg;

    /* XXX:
     * Fs normally ranges from 8 kHz to 48 kHz, and in double speed mode,
     * Fs normally range from 64 kHz to 96 kHz
     * */

    WM_DRV_CODEC_ES8374_READ_RET(data, ES8374_ADC_CTRL_ADDR, &value);

    value |= WM_BIT(3);

    if (cfg->sample_rate_hz > ES8374_SAMPLE_RATE_VIA_SPEED) {
        value |= WM_BIT(6);
    } else {
        value &= ~WM_BIT(6);
    }

    WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_ADC_CTRL_ADDR, value);

    return WM_ERR_SUCCESS;
}

static int wm_drv_codec_es8374_i2s_adc_set_mute(wm_drv_codec_data_t *data, bool mute)
{
    int ret;
    uint8_t value = 0;

    if (mute) {
        value |= WM_BIT(5);
        WM_DRV_CODEC_ES8374_OR_RET(data, ES8374_ADC_CTRL_ADDR, value);
    } else {
        value &= ~WM_BIT(5);
        WM_DRV_CODEC_ES8374_AND_RET(data, ES8374_ADC_CTRL_ADDR, value);
    }

    return WM_ERR_SUCCESS;
}

static int wm_drv_codec_es8374_i2s_dac_speed_init(wm_drv_codec_data_t *data)
{
    int ret;
    uint8_t value               = 0;
    wm_drv_codec_ctx_t *ctx     = &data->ctx;
    wm_drv_codec_priv_t *priv   = &ctx->priv;
    wm_drv_codec_i2s_cfg_t *cfg = &priv->cfg;

    WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_DAC_CTRL_ADDR1, 0x40);

    ES8374_MDELAY(WM_DRV_CODEC_I2C_GI_MS);

    WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_DAC_CTRL_ADDR1, 0x00);

    /* XXX:
     * Fs normally ranges from 8 kHz to 48 kHz, and in double speed mode,
     * Fs normally range from 64 kHz to 96 kHz
     * */
    if (cfg->sample_rate_hz > ES8374_SAMPLE_RATE_VIA_SPEED) {
        value |= WM_BIT(7);
        WM_DRV_CODEC_ES8374_OR_RET(data, ES8374_DAC_CTRL_ADDR2, value);
    } else {
        value &= ~WM_BIT(7);
        WM_DRV_CODEC_ES8374_AND_RET(data, ES8374_DAC_CTRL_ADDR2, value);
    }

    return WM_ERR_SUCCESS;
}

static int wm_drv_codec_es8374_i2s_dac_get_mute(wm_drv_codec_data_t *data, bool *mute)
{
    uint8_t value = 0;
    int ret       = wm_drv_codec_es8374_reg_read(data, ES8374_DAC_VOLUM_ADDR, &value);
    if (WM_ERR_SUCCESS != ret) {
        return WM_ERR_FAILED;
    }

    *mute = !!(value & WM_BIT(5));

    return WM_ERR_SUCCESS;
}

static int wm_drv_codec_es8374_i2s_dac_set_mute(wm_drv_codec_data_t *data, bool mute)
{
    int ret;
    uint8_t value = 0;

    if (mute) {
        value |= WM_BIT(5);
        WM_DRV_CODEC_ES8374_OR_RET(data, ES8374_DAC_CTRL_ADDR1, value);
    } else {
        value &= ~WM_BIT(5);
        WM_DRV_CODEC_ES8374_AND_RET(data, ES8374_DAC_CTRL_ADDR1, value);
    }

    return WM_ERR_SUCCESS;
}

static int wm_drv_codec_es8374_set_timing(wm_drv_codec_data_t *data)
{
    int ret;

    WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_TIMING_SET1_ADDR, ES8374_TIMING_SET1_VALUE);

    WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_TIMING_SET2_ADDR, ES8374_TIMING_SET2_VALUE);

    return WM_ERR_SUCCESS;
}

static int wm_drv_codec_es8374_set_lin_gain(wm_drv_codec_data_t *data)
{
    int ret;
    uint8_t on = 1, off = 2;
    wm_drv_codec_ctx_t *ctx   = &data->ctx;
    wm_drv_codec_priv_t *priv = &ctx->priv;

    wm_dt_hw_codec_es8374_cfg_t *hw_cfg = priv->hw_cfg;

    if (hw_cfg->lin1 && hw_cfg->rin1) {
        on  = 1;
        off = 2;
    } else if (hw_cfg->lin2 && hw_cfg->rin2) {
        on  = 2;
        off = 1;
    } else {
        WM_DRV_CODEC_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    ES8374_LIN_SEL(data, on, off);

    return WM_ERR_SUCCESS;
}

#if 0
static int wm_drv_codec_es8374_set_autojack(wm_drv_codec_data_t *data)
{
    int ret;
    uint8_t value               = ((uint8_t)(WM_BIT(7) & 0xFF));
    wm_drv_codec_ctx_t *ctx     = &data->ctx;
    wm_drv_codec_priv_t *priv   = &ctx->priv;
    wm_drv_codec_i2s_cfg_t *cfg = &priv->cfg;

    if (cfg->jack_switch) {
        WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_JACK_ADDR, value);
    }

    return WM_ERR_SUCCESS;
}
#endif

static int wm_drv_codec_es8374_ic_start(wm_drv_codec_data_t *data)
{
    int ret;

    WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_RESET_ADDR, ES8374_RESET_IC_START);

    ES8374_MDELAY(WM_DRV_CODEC_I2C_GI_MS);

    WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_POWER1, ES8374_POWER1_ON);
    WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_POWER2, ES8374_POWER2_ON);

    return WM_ERR_SUCCESS;
}

static int wm_drv_codec_es8374_volume2db(wm_drv_codec_data_t *data, int *volume)
{
    if (*volume > 96 || *volume < 0) {
        WM_DRV_CODEC_LOG_E("Error: volume < 0 or > 96 !");
        return WM_ERR_INVALID_PARAM;
    }

    *volume = 192 - (*volume * 2);

    return WM_ERR_SUCCESS;
}

static int wm_drv_codec_es8374_db2volume(wm_drv_codec_data_t *data, uint8_t *volume)
{
    int ret;
    uint8_t db = 0;

    ret = wm_drv_codec_es8374_reg_read(data, ES8374_DAC_VOLUM_ADDR, &db);
    if (WM_ERR_SUCCESS != ret) {
        WM_DRV_CODEC_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    *volume = (192 - db) >> 1;

    return WM_ERR_SUCCESS;
}

static int wm_drv_codec_es8374_set_adc_volume(wm_drv_codec_data_t *data, int volume)
{
    int ret;

    wm_drv_codec_es8374_volume2db(data, &volume);

    WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_ADC_VOLUM_ADDR, volume);

    return WM_ERR_SUCCESS;
}

static int wm_drv_codec_es8374_set_dac_volume(wm_drv_codec_data_t *data, int volume)
{
    int ret;

    wm_drv_codec_es8374_volume2db(data, &volume);

    WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_DAC_VOLUM_ADDR, volume);

    return WM_ERR_SUCCESS;
}

static int wm_drv_codec_es8374_init_do(wm_drv_codec_data_t *data)
{
    int ret;

    ES8374_HANDLE_RET(data, wm_drv_codec_es8374_reset);
    ES8374_HANDLE_RET(data, wm_drv_codec_es8374_init_clock_gate);
    ES8374_HANDLE_RET(data, wm_drv_codec_es8374_init_clock);
    ES8374_HANDLE_RET(data, wm_drv_codec_es8374_i2s_clk);
    ES8374_HANDLE_RET(data, wm_drv_codec_es8374_set_dac_adc_clk_div);

    ES8374_HANDLE_RET(data, wm_drv_codec_es8374_i2s_adc_speed_init);
    ES8374_HANDLE_RET(data, wm_drv_codec_es8374_i2s_dac_speed_init);
    ES8374_HANDLE_RET(data, wm_drv_codec_es8374_set_timing);
    ES8374_HANDLE_RET(data, wm_drv_codec_es8374_set_lin_gain);

    ES8374_HANDLE_RET(data, wm_drv_codec_es8374_ic_start);
    ES8374_HANDLE_RET(data, wm_drv_codec_es8374_i2s_adc_protocol);
    ES8374_HANDLE_RET(data, wm_drv_codec_es8374_i2s_dac_protocol);

    WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_MONOOUT_MIXER_GAIN, 0x19);
    WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_SPK_MIXER_SEL, 0x90);
    WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_SPK_MIXER_GAIN, 0x09);
    WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_SPEAKER_PROTECT_ADDR, 0x00);
    WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_ALC_ADDR, 0x00);
    WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_DMIC_ADDR, 0x60);
    WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_PLL_MODE_EN_ADDR, 0xA0);
    WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_UNKOWN71_ADDR, 0x05);
    WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_UNKOWN73_ADDR, 0x00);
    WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_UNKOWN71_ADDR, 0x05);
    WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_UNKOWN73_ADDR, 0x70);

    //auto mute for monoout and spk
    WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_DAC_CTRL_ADDR2, 0x30);

    return WM_ERR_SUCCESS;
}

static int wm_drv_codec_es8374_prev_ops(wm_device_t *device)
{
    if (WM_DRV_CODEC_PRE_CHECK()) {
        return WM_ERR_FAILED;
    }

    if (!WM_DRV_CODEC_TRY_LOCK()) {
        return WM_ERR_FAILED;
    }

    return WM_ERR_SUCCESS;
}

static int wm_drv_codec_es8374_post_ops(wm_device_t *device)
{
    if (WM_DRV_CODEC_PRE_CHECK()) {
        WM_DRV_CODEC_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    if (!WM_DRV_CODEC_UNLOCK()) {
        WM_DRV_CODEC_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    return WM_ERR_SUCCESS;
}

static int wm_drv_codec_es8374_start_record(wm_drv_codec_data_t *data)
{
    int ret = wm_drv_codec_es8374_i2s_adc_set_mute(data, false);

    return ret == WM_ERR_SUCCESS ? wm_drv_codec_es8374_set_adc_volume(data, ES8374_ADC_VOLUME_DEF) : ret;
}

static int wm_drv_codec_es8374_start_playback(wm_drv_codec_data_t *data)
{
    int ret;

    wm_drv_codec_ctx_t *ctx     = &data->ctx;
    wm_drv_codec_priv_t *priv   = &ctx->priv;
    wm_drv_codec_i2s_cfg_t *cfg = &priv->cfg;

    if (cfg->intf == WM_DRV_CODEC_I2S_OUTPUT_MONOOUT || cfg->intf == WM_DRV_CODEC_I2S_OUTPUT_BOTH) {
        WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_MONOOUT_MIXER_SEL, 0xA0);
    }

    if (cfg->intf == WM_DRV_CODEC_I2S_OUTPUT_SPKOUT || cfg->intf == WM_DRV_CODEC_I2S_OUTPUT_BOTH) {
        WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_SPK_MIXER_SEL, 0x90);
        WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_SPEAKER_SEL_GAIN, 0xA0);
    }

    return wm_drv_codec_es8374_set_dac_volume(data, 96);
}

static int wm_drv_codec_es8374_start(wm_device_t *device)
{
    //int ret;

    wm_drv_codec_data_t *data   = device->drv;
    wm_drv_codec_ctx_t *ctx     = &data->ctx;
    wm_drv_codec_priv_t *priv   = &ctx->priv;
    wm_drv_codec_i2s_cfg_t *cfg = &priv->cfg;

    if (device == NULL || data == NULL) {
        WM_DRV_CODEC_LOG_MARKER;
        return WM_ERR_NOT_ALLOWED;
    }

    if (cfg->codec_mode == WM_DRV_CODEC_I2S_MODE_CODEC) {
        if (WM_ERR_SUCCESS != wm_drv_codec_es8374_start_record(data)) {
            return WM_ERR_FAILED;
        }

        return wm_drv_codec_es8374_start_playback(data);
    }

    if (cfg->codec_mode == WM_DRV_CODEC_I2S_MODE_RECORD) {
        return wm_drv_codec_es8374_start_record(data);
    }

    if (cfg->codec_mode == WM_DRV_CODEC_I2S_MODE_PLAY_BACK) {
        return wm_drv_codec_es8374_start_playback(data);
    }

    return WM_ERR_SUCCESS;
}

static int wm_drv_codec_es8374_stop(wm_device_t *device)
{
    int ret;
    uint8_t value;

    wm_drv_codec_data_t *data = device->drv;

    if (device == NULL || data == NULL) {
        WM_DRV_CODEC_LOG_MARKER;
        return WM_ERR_NOT_ALLOWED;
    }

    /* ADC mute */
    value = ((uint8_t)(WM_BIT(5) & 0xFF));
    WM_DRV_CODEC_ES8374_OR_RET(data, ES8374_ADC_CTRL_ADDR, value);

    /* disable mixer to monoout */
    WM_DRV_CODEC_ES8374_AND_RET(data, ES8374_MONOOUT_MIXER_SEL, ~0x60);

    /* disable classD */
    WM_DRV_CODEC_ES8374_AND_RET(data, ES8374_SPEAKER_SEL_GAIN, 0x00);

    /* disable DA to SPK mixer */
    WM_DRV_CODEC_ES8374_AND_RET(data, ES8374_SPK_MIXER_SEL, 0x00);

    return WM_ERR_SUCCESS;
}

static int wm_drv_codec_es8374_init(wm_device_t *device, wm_drv_codec_i2s_cfg_t *cfg)
{
    wm_dt_hw_codec_i2s_t *hw = device->hw;

    wm_drv_codec_data_t *data = NULL;
    wm_drv_codec_ctx_t *ctx   = NULL;

    if (cfg == NULL || device->hw == NULL) {
        WM_DRV_CODEC_LOG_MARKER;
        return WM_ERR_INVALID_PARAM;
    }

    if (hw->es8374_cfg.i2c == false) {
        WM_DRV_CODEC_LOG_MARKER;
        return WM_ERR_NO_SUPPORT;
    }

    if (cfg->ctype < WM_DRV_CODEC_I2S_CHAN_MONO || cfg->ctype > WM_DRV_CODEC_I2S_CHAN_STEREO) {
        WM_DRV_CODEC_LOG_MARKER;
        return WM_ERR_NO_SUPPORT;
    }

    /* ES8374 support stereo channel only */
    if (cfg->ctype == WM_DRV_CODEC_I2S_CHAN_MONO) {
        WM_DRV_CODEC_LOG_E("ES8374 should work with stereo channel!");
        return WM_ERR_NO_SUPPORT;
    }

    if (cfg->fmt < WM_DRV_CODEC_I2S_FMT_8BIT) {
        return WM_ERR_NO_SUPPORT;
    }

    if (cfg->intf == WM_DRV_CODEC_I2S_OUTPUT_MONOOUT || cfg->intf == WM_DRV_CODEC_I2S_OUTPUT_BOTH) {
        if (!hw->es8374_cfg.monoout) {
            WM_DRV_CODEC_LOG_E("Don't support output from monoout by HW board!");
            return WM_ERR_NO_SUPPORT;
        }
    }

    if (cfg->intf == WM_DRV_CODEC_I2S_OUTPUT_SPKOUT || cfg->intf == WM_DRV_CODEC_I2S_OUTPUT_BOTH) {
        if (!hw->es8374_cfg.spkout) {
            WM_DRV_CODEC_LOG_E("Don't support output from speaker by HW board!");
            return WM_ERR_NO_SUPPORT;
        }
    }

    /* allocate memeory for driver data */
    if ((data = wm_os_internal_calloc(1, sizeof(*data))) == NULL) {
        WM_DRV_CODEC_LOG_MARKER;
        return WM_ERR_NO_MEM;
    }

    device->drv = data;
    ctx         = &data->ctx;

    /* mutext create */
    if (WM_ERR_SUCCESS != wm_os_internal_mutex_create(&ctx->lock)) {
        WM_DRV_CODEC_LOG_MARKER;
        goto fail_create_mutex;
    }

    ctx->priv.hw_cfg = &hw->es8374_cfg;
    ctx->priv.cfg    = *cfg;

    ctx->priv.i2c_cfg.addr     = hw->es8374_cfg.address;
    ctx->priv.i2c_cfg.speed_hz = WM_DRV_CODEC_I2C_SPEED;

    /* check i2c device if has been initialized in upper layer */
    ctx->i2c_dev = wm_dt_get_device_by_name(hw->i2c_device_name);
    if (ctx->i2c_dev != NULL && ctx->i2c_dev->state == WM_DEV_ST_INITED) {
        WM_DRV_CODEC_LOG_MARKER;
        goto fail_get_i2c;
    }

    ctx->i2c_dev = wm_drv_i2c_init(hw->i2c_device_name);
    if (ctx->i2c_dev == NULL || ctx->i2c_dev->state != WM_DEV_ST_INITED) {
        WM_DRV_CODEC_LOG_MARKER;
        goto fail_get_i2c;
    }

    /* TODO: check if gpio is initialized */

    if (WM_ERR_SUCCESS != wm_drv_codec_es8374_init_do(data)) {
        WM_DRV_CODEC_LOG_MARKER;
        goto fail_es8374_init;
    }

    return WM_ERR_SUCCESS;

fail_es8374_init:
    wm_drv_i2c_deinit(ctx->i2c_dev);

fail_get_i2c:
    wm_os_internal_mutex_delete(ctx->lock);

fail_create_mutex:
    wm_os_internal_free(data);

    memset(&ctx->priv, 0x0, sizeof(ctx->priv));

    device->drv = NULL;

    return WM_ERR_FAILED;
}

int wm_drv_codec_es8374_deinit(wm_device_t *device)
{
    int ret;
    wm_drv_codec_data_t *data = NULL;
    wm_drv_codec_ctx_t *ctx   = NULL;

    if (device == NULL) {
        return WM_ERR_FAILED;
    }

    data = device->drv;
    ctx  = &data->ctx;

    WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_RESET_ADDR, ES8374_RESET_START);

    WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_MONOOUT_MIXER_SEL, 0x00);

    WM_DRV_CODEC_ES8374_WRITE_RET(data, ES8374_SPEAKER_SEL_GAIN, 0x00);

    if (ctx->i2c_dev) {
        if (WM_ERR_SUCCESS != wm_drv_i2c_deinit(ctx->i2c_dev)) {
            WM_DRV_CODEC_LOG_MARKER;
            return WM_ERR_FAILED;
        }
    }

    wm_os_internal_mutex_release(ctx->lock);
    wm_os_internal_mutex_delete(ctx->lock);

    ctx->lock = NULL;

    wm_os_internal_free(device->drv);

    device->drv = NULL;

    return WM_ERR_SUCCESS;
}

int wm_drv_codec_es8374_ioctl(wm_device_t *device, wm_drv_codec_i2s_ioctl_arg_t *arg)
{
    wm_drv_codec_data_t *data = device->drv;

    if (device == NULL || arg == NULL) {
        return WM_ERR_FAILED;
    }

    switch (arg->cmd) {
        case WM_DRV_CODEC_I2S_CMD_GET_VOL:
        {
            return wm_drv_codec_es8374_db2volume(data, &arg->u.reg.val);
        }
        case WM_DRV_CODEC_I2S_CMD_SET_VOL:
        {
            return wm_drv_codec_es8374_set_dac_volume(data, arg->u.reg.val);
        }
        case WM_DRV_CODEC_I2S_CMD_GET_MUTE:
        {
            return wm_drv_codec_es8374_i2s_dac_get_mute(data, &arg->u.mute);
        }
        case WM_DRV_CODEC_I2S_CMD_SET_MUTE:
        {
            wm_drv_codec_es8374_i2s_dac_set_mute(data, arg->u.mute);
            break;
        }
        case WM_DRV_CODEC_I2S_CMD_READ_REG:
        {
            return wm_drv_codec_es8374_reg_read(data, arg->u.reg.addr, &arg->u.reg.val);
        }
        case WM_DRV_CODEC_I2S_CMD_WRITE_REG:
        {
            return wm_drv_codec_es8374_reg_write(data, arg->u.reg.addr, arg->u.reg.val);
        }
        default:
        {
            return WM_ERR_INVALID_PARAM;
        }
    }

    return WM_ERR_SUCCESS;
}

int wm_drv_codec_es8374_reg_dumps(wm_device_t *device)
{
    uint8_t rbuf;

    wm_drv_codec_data_t *data = device->drv;

    if (device == NULL) {
        WM_DRV_CODEC_LOG_MARKER;
        return WM_ERR_FAILED;
    }

    WM_DRV_CODEC_LOG_I("\n");
    WM_DRV_CODEC_LOG_I("addr = value");

    ES8374_READ_DUMP(data, ES8374_RESET_ADDR, rbuf);
    ES8374_READ_DUMP(data, ES8374_CLK_MGM1_ADDR, rbuf);
    ES8374_READ_DUMP(data, ES8374_CLK_MGM2_ADDR, rbuf);
    ES8374_READ_DUMP(data, ES8374_PLL_MODE_EN_ADDR, rbuf);
    ES8374_READ_DUMP(data, ES8374_PLL_MODE_SET_ADDR, rbuf);
    ES8374_READ_DUMP(data, ES8374_PLL_PDN_VCO_ADDR, rbuf);
    ES8374_READ_DUMP(data, ES8374_PLL_SET_ADDR, rbuf);
    ES8374_READ_DUMP(data, ES8374_PLL_N_ADDR, rbuf);
    ES8374_READ_DUMP(data, ES8374_PLL_K1_ADDR, rbuf);
    ES8374_READ_DUMP(data, ES8374_PLL_K2_ADDR, rbuf);
    ES8374_READ_DUMP(data, ES8374_PLL_K3_ADDR, rbuf);
    ES8374_READ_DUMP(data, ES8374_I2S_CLK_ADDR, rbuf);
    ES8374_READ_DUMP(data, ES8374_ADC_CTRL_ADDR, rbuf);
    ES8374_READ_DUMP(data, ES8374_DAC_CTRL_ADDR1, rbuf);
    ES8374_READ_DUMP(data, ES8374_DAC_CTRL_ADDR2, rbuf);
    ES8374_READ_DUMP(data, ES8374_ADC_CLK_DIV_ADDR, rbuf);
    ES8374_READ_DUMP(data, ES8374_ADC_LRCLK_DIV_HI_ADDR, rbuf);
    ES8374_READ_DUMP(data, ES8374_ADC_LRCLK_DIV_LO_ADDR, rbuf);
    ES8374_READ_DUMP(data, ES8374_I2S_ADC_PROTOCOL_ADDR, rbuf);
    ES8374_READ_DUMP(data, ES8374_I2S_DAC_PROTOCOL_ADDR, rbuf);
    ES8374_READ_DUMP(data, ES8374_TIMING_SET1_ADDR, rbuf);
    ES8374_READ_DUMP(data, ES8374_TIMING_SET2_ADDR, rbuf);
    ES8374_READ_DUMP(data, ES8374_POWER1, rbuf);
    ES8374_READ_DUMP(data, ES8374_POWER2, rbuf);
    ES8374_READ_DUMP(data, ES8374_MONOOUT_MIXER_SEL, rbuf);
    ES8374_READ_DUMP(data, ES8374_MONOOUT_MIXER_GAIN, rbuf);
    ES8374_READ_DUMP(data, ES8374_SPK_MIXER_SEL, rbuf);
    ES8374_READ_DUMP(data, ES8374_SPK_MIXER_GAIN, rbuf);
    ES8374_READ_DUMP(data, ES8374_SPEAKER_SEL_GAIN, rbuf);
    ES8374_READ_DUMP(data, ES8374_LIN_PGA_ADDR, rbuf);
    ES8374_READ_DUMP(data, ES8374_MONO_PGA_ADDR, rbuf);
    ES8374_READ_DUMP(data, ES8374_ADC_VOLUM_ADDR, rbuf);
    ES8374_READ_DUMP(data, ES8374_DAC_VOLUM_ADDR, rbuf);
    ES8374_READ_DUMP(data, ES8374_JACK_ADDR, rbuf);

    return WM_ERR_SUCCESS;
}

const wm_drv_codec_i2s_ops_t wm_drv_codec_i2s_es8374_ops = {
    .init     = wm_drv_codec_es8374_init,
    .deinit   = wm_drv_codec_es8374_deinit,
    .start    = wm_drv_codec_es8374_start,
    .stop     = wm_drv_codec_es8374_stop,
    .ioctl    = wm_drv_codec_es8374_ioctl,
    .prev_ops = wm_drv_codec_es8374_prev_ops,
    .post_ops = wm_drv_codec_es8374_post_ops,
    .dumps    = wm_drv_codec_es8374_reg_dumps,
};

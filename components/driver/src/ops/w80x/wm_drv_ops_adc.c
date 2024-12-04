/**
 * @file wm_drv_ops_adc.c
 *
 * @brief Adc ops Driver Module
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

#include <string.h>
#include <math.h>
#include "wm_types.h"
#include "wm_soc_cfgs.h"
#include "wm_error.h"
#include "wm_osal.h"
#include "wm_ft_param.h"

#include "wm_hal_adc.h"
#include "wm_drv_adc.h"
#include "wm_drv_dma.h"
#include "wm_drv_rcc.h"
#include "wm_drv_irq.h"
#include "wm_drv_rcc.h"
#include "wm_dt_hw.h"
#include "wm_dt.h"

#define LOG_TAG "adc"
#include "wm_log.h"

static int wm_w800_drv_adc_init(wm_device_t *dev);
static int wm_w800_drv_adc_deinit(wm_device_t *dev);
static int wm_w800_drv_adc_set_cfg(wm_device_t *dev, const wm_drv_adc_cfg_t *cfg);
static int wm_w800_drv_adc_register_handle(wm_device_t *dev, wm_adc_intr_type_t intr_type, wm_drv_adc_callback_t adc_callback,
                                           void *user_data);
static int wm_w800_drv_adc_oneshot(wm_device_t *dev, wm_adc_channel_t adc_channel, int32_t *result);
static int wm_w800_drv_adc_polling(wm_device_t *dev, wm_adc_channel_t adc_channel, int *buf, uint16_t count, uint32_t timeout);
static int wm_w800_drv_adc_start_it(wm_device_t *dev, wm_adc_channel_t adc_channel);
static int wm_w800_drv_adc_stop_it(wm_device_t *dev, wm_adc_channel_t adc_channel);
static int wm_w800_drv_adc_read_temp(wm_device_t *dev, int *temperature_val);
static int wm_w800_drv_adc_read_voltage(wm_device_t *dev, int *voltage);
static int wm_w800_drv_adc_cal_voltage(wm_device_t *dev, int vol);

#define WM_ADC_CHANNEL_IS_VAILD(channel)                                                          \
    (channel == WM_ADC_CHANNEL_0 || channel == WM_ADC_CHANNEL_1 || channel == WM_ADC_CHANNEL_2 || \
     channel == WM_ADC_CHANNEL_3 || channel == WM_ADC_CHANNEL_0_1_DIFF_INPUT || channel == WM_ADC_CHANNEL_2_3_DIFF_INPUT)

#define WM_DRV_ADC_LOCK(lock)                                                            \
    do {                                                                                 \
        if (!lock) {                                                                     \
            wm_log_error("mutex create err!");                                           \
            return WM_ERR_FAILED;                                                        \
        }                                                                                \
        if (WM_ERR_SUCCESS != wm_os_internal_mutex_acquire(lock, WM_OS_WAIT_TIME_MAX)) { \
            wm_log_error("mutex acquire err!");                                          \
            return WM_ERR_FAILED;                                                        \
        }                                                                                \
    } while (0);

#define WM_DRV_ADC_UNLOCK(lock)             \
    do {                                    \
        if (!lock) {                        \
            return WM_ERR_NO_INITED;        \
        }                                   \
        wm_os_internal_mutex_release(lock); \
    } while (0);

#define WM_DRV_ADC_DESTORY_LOCK(lock)      \
    do {                                   \
        if (!lock) {                       \
            return WM_ERR_NO_INITED;       \
        }                                  \
        wm_os_internal_mutex_delete(lock); \
    } while (0);

typedef enum {
    WM_ADC_STATE_IDLE = 0,
    WM_ADC_STATE_RUNNING,
} wm_adc_state_t;

typedef struct {
    wm_os_mutex_t *mutex;
    wm_adc_state_t state;
    wm_device_t *clock_dev;
    wm_device_t *dma_dev;
    wm_drv_adc_cfg_t *cfg;
} wm_drv_adc_ctx_t;

typedef struct {
    wm_hal_adc_dev_t adc_hal;
    wm_drv_adc_ctx_t adc_drv;
} wm_drv_adc_data_t;

typedef struct {
    int (*init)(wm_device_t *dev);
    int (*deinit)(wm_device_t *dev);
    int (*set_cfg)(wm_device_t *dev, const wm_drv_adc_cfg_t *cfg);
    int (*register_handle)(wm_device_t *dev, wm_adc_intr_type_t intr_type, wm_drv_adc_callback_t adc_callback, void *user_data);
    int (*oneshot)(wm_device_t *dev, wm_adc_channel_t adc_channel, int32_t *result);
    int (*polling)(wm_device_t *dev, wm_adc_channel_t adc_channel, int *buf, uint16_t count, uint32_t timeout);
    int (*start_dma)(wm_device_t *dev, wm_adc_channel_t adc_channel, int *result, uint32_t size);
    int (*start_intr)(wm_device_t *dev, wm_adc_channel_t adc_channel);
    int (*stop_intr)(wm_device_t *dev, wm_adc_channel_t adc_channel);
    int (*read_temp)(wm_device_t *dev, int *temperature_val);
    int (*read_voltage)(wm_device_t *dev, int *voltage);
    int (*cal_voltage)(wm_device_t *dev, int vol);
} wm_drv_adc_ops_t;

static void adc_gauss_solve(int n, double A[], double x[], double b[])
{
    int i, j, k, r;
    double max;

    for (k = 0; k < n - 1; k++) {
        max = fabs(A[k * n + k]); /*find maxmum*/
        r   = k;

        for (i = k + 1; i < n - 1; i++) {
            if (max < fabs(A[i * n + i])) {
                max = fabs(A[i * n + i]);
                r   = i;
            }
        }

        if (r != k) {
            for (i = 0; i < n; i++) /*change array:A[k]&A[r] */
            {
                max          = A[k * n + i];
                A[k * n + i] = A[r * n + i];
                A[r * n + i] = max;
            }
        }
        max  = b[k]; /*change array:b[k]&b[r]     */
        b[k] = b[r];
        b[r] = max;
        for (i = k + 1; i < n; i++) {
            for (j = k + 1; j < n; j++) {
                A[i * n + j] -= A[i * n + k] * A[k * n + j] / A[k * n + k];
            }
            b[i] -= A[i * n + k] * b[k] / A[k * n + k];
        }
    }

    for (i = n - 1; i >= 0; x[i] /= A[i * n + i], i--) {
        for (j = i + 1, x[i] = b[i]; j < n; j++) {
            x[i] -= A[i * n + j] * x[j];
        }
    }
}

static void adc_polyfit(int n, double x[], double y[], int poly_n, double a[])
{
    int i, j;
    double *tempx, *tempy, *sumxx, *sumxy, *ata;

    tempx = wm_os_internal_calloc(n, sizeof(double));
    sumxx = wm_os_internal_calloc(poly_n * 2 + 1, sizeof(double));
    tempy = wm_os_internal_calloc(n, sizeof(double));
    sumxy = wm_os_internal_calloc(poly_n + 1, sizeof(double));
    ata   = wm_os_internal_calloc((poly_n + 1) * (poly_n + 1), sizeof(double));

    if (tempx && sumxx && tempy && sumxy && ata) {
        for (i = 0; i < n; i++) {
            tempx[i] = 1;
            tempy[i] = y[i];
        }

        for (i = 0; i < 2 * poly_n + 1; i++) {
            for (sumxx[i] = 0, j = 0; j < n; j++) {
                sumxx[i] += tempx[j];
                tempx[j] *= x[j];
            }
        }

        for (i = 0; i < poly_n + 1; i++) {
            for (sumxy[i] = 0, j = 0; j < n; j++) {
                sumxy[i] += tempy[j];
                tempy[j] *= x[j];
            }
        }

        for (i = 0; i < poly_n + 1; i++) {
            for (j = 0; j < poly_n + 1; j++) {
                ata[i * (poly_n + 1) + j] = sumxx[i + j];
            }
        }

        adc_gauss_solve(poly_n + 1, ata, a, sumxy);
    }

    if (tempx) {
        wm_os_internal_free(tempx);
    }

    if (sumxx) {
        wm_os_internal_free(sumxx);
    }

    if (tempy) {
        wm_os_internal_free(tempy);
    }

    if (sumxy) {
        wm_os_internal_free(sumxy);
    }

    if (ata) {
        wm_os_internal_free(ata);
    }
}

static int adc_polyfit_init(wm_adc_polyfit_param_t *polyfit_param)
{
    wm_ft_adc_cal_t adc_cal = { 0 };
    /*function f(x) = ax + b*/
    float a = 0.0;
    float b = 0.0;
    int i;
    double x[8] = { 0.0 };
    double y[8] = { 0.0 };

    polyfit_param->poly_n = 0;
    if (WM_ERR_SUCCESS == wm_ft_get_adc_cal_param(&adc_cal)) {
        if ((adc_cal.valid_cnt == 4) || (adc_cal.valid_cnt == 2) || (adc_cal.valid_cnt == 3)) {
            //memcpy((char *)&a, (char *)&adc_cal.units[ADC_CAL_K_POS], 4);
            //memcpy((char *)&b, (char *)&adc_cal.units[ADC_CAL_B_POS], 4);
            a = adc_cal.a;
            b = adc_cal.b;
            if ((a > 1.0) && (a < 1.3) && (b < -1000.0)) { /*new calibration*/
                polyfit_param->poly_n = 1;
                polyfit_param->a[1]   = a;
                polyfit_param->a[0]   = b;
            } else { /*old calibration*/
                for (i = 0; i < adc_cal.valid_cnt; i++) {
                    x[i] = (double)adc_cal.units[i].real_val;
                    y[i] = (double)adc_cal.units[i].ref_val;
                }

                polyfit_param->poly_n = 1;
                adc_polyfit(adc_cal.valid_cnt, x, y, 1, polyfit_param->a);

                if (b < -1000.0) {
                    polyfit_param->a[0] = b;
                }
            }

            return WM_ERR_SUCCESS;
        }
    }

    return WM_ERR_FAILED;
}

static int wm_w800_drv_adc_init(wm_device_t *dev)
{
    int err                     = 0;
    uint8_t i                   = 0;
    wm_dt_hw_adc_t *hw          = NULL;
    wm_drv_adc_data_t *drv_data = NULL;
    wm_device_t *dma_dev        = NULL;
    wm_dma_data_t *dma_data     = NULL;

    if (!dev->drv) {
        hw = (wm_dt_hw_adc_t *)dev->hw;

        dev->drv = (wm_drv_adc_data_t *)wm_os_internal_malloc(sizeof(wm_drv_adc_data_t));
        if (!dev->drv) {
            wm_log_error("adc init alloc err");
            return WM_ERR_NO_MEM;
        }
        memset(dev->drv, 0x00, sizeof(wm_drv_adc_data_t));

        drv_data = (wm_drv_adc_data_t *)dev->drv;

        // lock
        if (WM_ERR_SUCCESS != wm_os_internal_mutex_create(&drv_data->adc_drv.mutex)) {
            wm_log_error("mutex create err");
            wm_os_internal_free(drv_data);
            dev->drv = NULL;
            return WM_ERR_FAILED;
        }

        // get dma dev
        dma_dev = wm_dt_get_device_by_name(hw->dma_device_name);
        if (!dma_dev) {
            wm_log_error("adc dt don't have dma");
        } else {
            dma_data                  = (wm_dma_data_t *)dma_dev->drv;
            drv_data->adc_drv.dma_dev = dma_dev;
            drv_data->adc_hal.dma     = &dma_data->hal_dev;
        }
        drv_data->adc_hal.reg_base = (wm_adc_reg_t *)(hw->reg_base);
        drv_data->adc_hal.irq_num  = hw->irq_cfg.irq_num;

        // pin cfg
        for (i = 0; i < hw->pin_cfg_count; i++) {
            if (WM_ERR_SUCCESS != (err = wm_drv_gpio_iomux_func_sel(hw->pin_cfg[i].pin_num, hw->pin_cfg[i].pin_mux))) {
                wm_log_error("gpio cfg err %d !", err);
                wm_os_internal_free(drv_data);
                dev->drv = NULL;
                return err;
            }
        }

        // adc cfg
        drv_data->adc_drv.cfg = (wm_drv_adc_cfg_t *)wm_os_internal_malloc(sizeof(wm_drv_adc_cfg_t) +
                                                                          hw->adc_cfg_count * sizeof(wm_dt_hw_adc_cfg_t));
        if (!drv_data->adc_drv.cfg) {
            wm_log_error("adc alloc cfg err");
            wm_os_internal_free(drv_data);
            dev->drv = NULL;
            return WM_ERR_NO_MEM;
        }
        memset(drv_data->adc_drv.cfg, 0x00, sizeof(wm_drv_adc_cfg_t) + hw->adc_cfg_count * sizeof(wm_dt_hw_adc_cfg_t));
        drv_data->adc_drv.cfg->adc_channel_count = hw->adc_cfg_count;
        memcpy(&drv_data->adc_drv.cfg->cfg[0], hw->adc_cfg, hw->adc_cfg_count * sizeof(wm_dt_hw_adc_cfg_t));

        adc_polyfit_init(&drv_data->adc_hal.polyfit_param);
        wm_drv_clock_enable(wm_dt_get_device_by_name(hw->rcc_device_name), WM_RCC_SD_ADC_GATE_EN);

        drv_data->adc_drv.state = WM_ADC_STATE_IDLE;
        dev->state              = WM_DEV_ST_INITED;

        return WM_ERR_SUCCESS;
    } else {
        return WM_ERR_ALREADY_INITED;
    }
}

static int wm_w800_drv_adc_deinit(wm_device_t *dev)
{
    if (!dev) {
        return WM_ERR_INVALID_PARAM;
    }

    int err                     = 0;
    wm_drv_adc_data_t *adc_data = NULL;
    wm_dt_hw_adc_t *hw          = NULL;

    adc_data = (wm_drv_adc_data_t *)dev->drv;
    hw       = (wm_dt_hw_adc_t *)dev->hw;

    if (adc_data) {
        WM_DRV_ADC_LOCK(adc_data->adc_drv.mutex);
        if (WM_ERR_SUCCESS != (err = wm_hal_adc_deinit(&adc_data->adc_hal))) {
            WM_DRV_ADC_UNLOCK(adc_data->adc_drv.mutex);
            wm_log_error("adc deinit err %d ", err);
            return err;
        }

        if (adc_data->adc_drv.cfg) {
            wm_os_internal_free(adc_data->adc_drv.cfg);
        }
        wm_drv_clock_disable(wm_dt_get_device_by_name(hw->rcc_device_name), WM_RCC_SD_ADC_GATE_EN);

        WM_DRV_ADC_UNLOCK(adc_data->adc_drv.mutex);
        wm_os_internal_mutex_delete(adc_data->adc_drv.mutex);
        wm_os_internal_free(adc_data);
        dev->drv   = NULL;
        dev->state = WM_DEV_ST_UNINIT;

        return WM_ERR_SUCCESS;
    }

    return WM_ERR_NO_INITED;
}

static int wm_w800_drv_adc_set_cfg(wm_device_t *dev, const wm_drv_adc_cfg_t *cfg)
{
    uint16_t i                  = 0;
    wm_drv_adc_data_t *adc_data = NULL;

    if (dev && cfg && cfg->adc_channel_count && cfg->adc_channel_count <= WM_ADC_MAX_CHANNEL_COUNT) {
        adc_data = (wm_drv_adc_data_t *)dev->drv;

        if (adc_data) {
            WM_DRV_ADC_LOCK(adc_data->adc_drv.mutex);
            if (adc_data) {
                // check cfg
                for (i = 0; i < cfg->adc_channel_count; i++) {
                    if (!WM_ADC_CHANNEL_IS_VAILD(cfg->cfg[i].adc_channel) || cfg->cfg[i].pga_gain1 > WM_ADC_GAIN1_LEVEL_5 ||
                        cfg->cfg[i].pga_gain2 > WM_ADC_GAIN2_LEVEL_3) {
                        WM_DRV_ADC_UNLOCK(adc_data->adc_drv.mutex);
                        return WM_ERR_INVALID_PARAM;
                    }
                }

                if (WM_ADC_STATE_IDLE != adc_data->adc_drv.state) {
                    wm_log_error("adc dev not idle");
                    WM_DRV_ADC_UNLOCK(adc_data->adc_drv.mutex);
                    return WM_ERR_NOT_ALLOWED;
                }

                if (adc_data->adc_drv.cfg) {
                    wm_os_internal_free(adc_data->adc_drv.cfg);
                    adc_data->adc_drv.cfg = NULL;
                }

                adc_data->adc_drv.cfg = (wm_drv_adc_cfg_t *)wm_os_internal_malloc(
                    sizeof(wm_drv_adc_cfg_t) + cfg->adc_channel_count * sizeof(wm_dt_hw_adc_cfg_t));
                if (!adc_data->adc_drv.cfg) {
                    wm_log_error("adc alloc cfg err");
                    WM_DRV_ADC_UNLOCK(adc_data->adc_drv.mutex);
                    return WM_ERR_NO_MEM;
                }
                memset(adc_data->adc_drv.cfg, 0x00,
                       sizeof(wm_drv_adc_cfg_t) + cfg->adc_channel_count * sizeof(wm_dt_hw_adc_cfg_t));

                memcpy(adc_data->adc_drv.cfg, cfg,
                       sizeof(wm_drv_adc_cfg_t) + cfg->adc_channel_count * sizeof(wm_dt_hw_adc_cfg_t));
                WM_DRV_ADC_UNLOCK(adc_data->adc_drv.mutex);

                return WM_ERR_SUCCESS;
            }
            WM_DRV_ADC_UNLOCK(adc_data->adc_drv.mutex);
        }
    }

    return WM_ERR_FAILED;
}

static int wm_w800_drv_adc_register_handle(wm_device_t *dev, wm_adc_intr_type_t intr_type, wm_drv_adc_callback_t adc_callback,
                                           void *user_data)
{
    if (!dev || !adc_callback || intr_type >= WM_ADC_INTR_TYPE_MAX || intr_type < WM_ADC_INTR_TYPE_ADC) {
        return WM_ERR_INVALID_PARAM;
    }

    wm_drv_adc_data_t *adc_data = NULL;
    int err                     = 0;

    if (dev) {
        adc_data = (wm_drv_adc_data_t *)dev->drv;

        if (adc_data) {
            WM_DRV_ADC_LOCK(adc_data->adc_drv.mutex);

            if (adc_data) {
                err = wm_hal_adc_register_callback(&adc_data->adc_hal, intr_type, adc_callback, user_data);
                WM_DRV_ADC_UNLOCK(adc_data->adc_drv.mutex);
                return err;
            }

            WM_DRV_ADC_UNLOCK(adc_data->adc_drv.mutex);
        }
    }

    return WM_ERR_FAILED;
}

static int wm_w800_drv_adc_oneshot(wm_device_t *dev, wm_adc_channel_t adc_channel, int32_t *result)
{
    if (!dev || !WM_ADC_CHANNEL_IS_VAILD(adc_channel) || !result) {
        return WM_ERR_INVALID_PARAM;
    }

    int err                     = WM_ERR_FAILED;
    wm_drv_adc_data_t *adc_data = NULL;
    wm_hal_adc_config_t cfg     = { 0 };

    adc_data = (wm_drv_adc_data_t *)dev->drv;

    if (adc_data) {
        if (WM_ADC_STATE_IDLE != adc_data->adc_drv.state) {
            wm_log_error("adc dev not idle");
            return WM_ERR_NOT_ALLOWED;
        }

        WM_DRV_ADC_LOCK(adc_data->adc_drv.mutex);
        if (adc_data && adc_data->adc_drv.cfg) {
            for (int i = 0; i < adc_data->adc_drv.cfg->adc_channel_count; i++) {
                if (adc_data->adc_drv.cfg->cfg[i].adc_channel == adc_channel) {
                    cfg.adc_channel = adc_data->adc_drv.cfg->cfg[i].adc_channel;
                    cfg.pga_gain1   = WM_ADC_GAIN1_LEVEL_0; //adc_data->adc_drv.cfg->cfg[i].pga_gain1;
                    cfg.pga_gain2   = WM_ADC_GAIN2_LEVEL_0; //adc_data->adc_drv.cfg->cfg[i].pga_gain2;
                    cfg.adc_cmp     = adc_data->adc_drv.cfg->cfg[i].adc_cmp;
                    cfg.cmp_data    = adc_data->adc_drv.cfg->cfg[i].cmp_data;
                    cfg.cmp_pol     = adc_data->adc_drv.cfg->cfg[i].cmp_pol;
                    if (WM_ERR_SUCCESS != (err = wm_hal_adc_init(&adc_data->adc_hal, &cfg))) {
                        wm_log_error("adc init err %d", err);
                    } else {
                        *result = wm_hal_adc_oneshot(&adc_data->adc_hal, adc_channel);
                    }
                    break;
                }
            }
        }
        WM_DRV_ADC_UNLOCK(adc_data->adc_drv.mutex);
    }

    return err;
}

int wm_w800_drv_adc_polling(wm_device_t *dev, wm_adc_channel_t adc_channel, int *buf, uint16_t count, uint32_t timeout)
{
    if (!dev || !WM_ADC_CHANNEL_IS_VAILD(adc_channel) || !buf) {
        return WM_ERR_INVALID_PARAM;
    }

    int err                     = WM_ERR_FAILED;
    wm_drv_adc_data_t *adc_data = NULL;
    wm_hal_adc_config_t cfg     = { 0 };

    adc_data = (wm_drv_adc_data_t *)dev->drv;

    if (adc_data) {
        if (WM_ADC_STATE_IDLE != adc_data->adc_drv.state) {
            wm_log_error("adc dev not idle");
            return WM_ERR_NOT_ALLOWED;
        }

        WM_DRV_ADC_LOCK(adc_data->adc_drv.mutex);
        if (adc_data && adc_data->adc_drv.cfg) {
            for (int i = 0; i < adc_data->adc_drv.cfg->adc_channel_count; i++) {
                if (adc_data->adc_drv.cfg->cfg[i].adc_channel == adc_channel) {
                    cfg.adc_channel = adc_data->adc_drv.cfg->cfg[i].adc_channel;
                    cfg.pga_gain1   = WM_ADC_GAIN1_LEVEL_0; //adc_data->adc_drv.cfg->cfg[i].pga_gain1;
                    cfg.pga_gain2   = WM_ADC_GAIN2_LEVEL_0; //adc_data->adc_drv.cfg->cfg[i].pga_gain2;
                    cfg.adc_cmp     = adc_data->adc_drv.cfg->cfg[i].adc_cmp;
                    cfg.cmp_data    = adc_data->adc_drv.cfg->cfg[i].cmp_data;
                    cfg.cmp_pol     = adc_data->adc_drv.cfg->cfg[i].cmp_pol;
                    if (WM_ERR_SUCCESS != (err = wm_hal_adc_init(&adc_data->adc_hal, &cfg))) {
                        wm_log_error("adc init err %d", err);
                    } else {
                        err = wm_hal_adc_polling(&adc_data->adc_hal, (uint32_t *)buf, count, timeout);
                    }
                    break;
                }
            }
        }
        WM_DRV_ADC_UNLOCK(adc_data->adc_drv.mutex);
    }

    return err;
}

static int wm_w800_drv_adc_start_dma(wm_device_t *dev, wm_adc_channel_t adc_channel, int *result, uint32_t size)
{
    if (!dev || !WM_ADC_CHANNEL_IS_VAILD(adc_channel)) {
        return WM_ERR_INVALID_PARAM;
    }

    int err                     = WM_ERR_FAILED;
    wm_drv_adc_data_t *adc_data = NULL;
    wm_hal_adc_config_t cfg     = { 0 };

    adc_data = (wm_drv_adc_data_t *)dev->drv;
    if (adc_data) {
        WM_DRV_ADC_LOCK(adc_data->adc_drv.mutex);
        if (adc_data && adc_data->adc_drv.cfg) {
            for (int i = 0; i < adc_data->adc_drv.cfg->adc_channel_count; i++) {
                if (adc_data->adc_drv.cfg->cfg[i].adc_channel == adc_channel) {
                    cfg.adc_channel = adc_data->adc_drv.cfg->cfg[i].adc_channel;
                    cfg.pga_gain1   = WM_ADC_GAIN1_LEVEL_0; //adc_data->adc_drv.cfg->cfg[i].pga_gain1;
                    cfg.pga_gain2   = WM_ADC_GAIN2_LEVEL_0; //adc_data->adc_drv.cfg->cfg[i].pga_gain2;
                    cfg.adc_cmp     = adc_data->adc_drv.cfg->cfg[i].adc_cmp;
                    cfg.cmp_data    = adc_data->adc_drv.cfg->cfg[i].cmp_data;
                    cfg.cmp_pol     = adc_data->adc_drv.cfg->cfg[i].cmp_pol;
                    if (WM_ERR_SUCCESS != (err = wm_hal_adc_init(&adc_data->adc_hal, &cfg))) {
                        wm_log_error("adc init err %d", err);
                    } else {
                        if (WM_ERR_SUCCESS ==
                            (err = wm_drv_dma_request_ch(adc_data->adc_drv.dma_dev, &adc_data->adc_hal.dma_ch, 0))) {
                            err = wm_hal_adc_start_dma(&adc_data->adc_hal, result, size, adc_data->adc_hal.dma_ch);
                        } else {
                            wm_log_error("adc dma channel req err %d", err);
                        }
                    }

                    break;
                }
            }
        }
        WM_DRV_ADC_UNLOCK(adc_data->adc_drv.mutex);
    }

    return err;
}

static int wm_w800_drv_adc_start_it(wm_device_t *dev, wm_adc_channel_t adc_channel)
{
    if (!dev || !WM_ADC_CHANNEL_IS_VAILD(adc_channel)) {
        return WM_ERR_INVALID_PARAM;
    }
    int err                     = WM_ERR_FAILED;
    wm_drv_adc_data_t *adc_data = NULL;
    wm_hal_adc_config_t cfg     = { 0 };

    adc_data = (wm_drv_adc_data_t *)dev->drv;

    if (adc_data) {
        if (WM_ADC_STATE_IDLE != adc_data->adc_drv.state) {
            wm_log_error("adc dev not idle");
            return WM_ERR_NOT_ALLOWED;
        }

        if (adc_data && adc_data->adc_drv.cfg) {
            WM_DRV_ADC_LOCK(adc_data->adc_drv.mutex);
            for (int i = 0; i < adc_data->adc_drv.cfg->adc_channel_count; i++) {
                if (adc_data->adc_drv.cfg->cfg[i].adc_channel == adc_channel) {
                    if (adc_data->adc_hal.irq_callback[WM_ADC_INTR_TYPE_ADC].callback) {
                        cfg.adc_channel = adc_data->adc_drv.cfg->cfg[i].adc_channel;
                        cfg.pga_gain1   = WM_ADC_GAIN1_LEVEL_0; //adc_data->adc_drv.cfg->cfg[i].pga_gain1;
                        cfg.pga_gain2   = WM_ADC_GAIN2_LEVEL_0; //adc_data->adc_drv.cfg->cfg[i].pga_gain2;
                        cfg.adc_cmp     = adc_data->adc_drv.cfg->cfg[i].adc_cmp;
                        cfg.cmp_data    = adc_data->adc_drv.cfg->cfg[i].cmp_data;
                        cfg.cmp_pol     = adc_data->adc_drv.cfg->cfg[i].cmp_pol;

                        if (WM_ERR_SUCCESS != (err = wm_hal_adc_init(&adc_data->adc_hal, &cfg))) {
                            wm_log_error("adc init err %d", err);
                        } else {
                            err                     = wm_hal_adc_start_it(&adc_data->adc_hal);
                            adc_data->adc_drv.state = WM_ADC_STATE_RUNNING;
                        }
                    }

                    break;
                }
            }
            WM_DRV_ADC_UNLOCK(adc_data->adc_drv.mutex);
        }
    }

    return err;
}

static int wm_w800_drv_adc_stop_it(wm_device_t *dev, wm_adc_channel_t adc_channel)
{
    if (!dev || !WM_ADC_CHANNEL_IS_VAILD(adc_channel)) {
        return WM_ERR_INVALID_PARAM;
    }

    int err                     = WM_ERR_FAILED;
    wm_drv_adc_data_t *adc_data = NULL;

    adc_data = (wm_drv_adc_data_t *)dev->drv;

    if (adc_data) {
        if (WM_ADC_STATE_RUNNING != adc_data->adc_drv.state) {
            wm_log_error("adc dev not running, no need stop");
            return WM_ERR_NOT_ALLOWED;
        }

        WM_DRV_ADC_LOCK(adc_data->adc_drv.mutex);
        if (adc_data) {
            err                     = wm_hal_adc_stop_it(&adc_data->adc_hal);
            adc_data->adc_drv.state = WM_ADC_STATE_IDLE;
        }
        WM_DRV_ADC_UNLOCK(adc_data->adc_drv.mutex);
    }

    return err;
}

static int wm_w800_drv_adc_read_temp(wm_device_t *dev, int *temperature_val)
{
    if (!dev || !temperature_val) {
        return WM_ERR_INVALID_PARAM;
    }

    int err                     = 0;
    wm_drv_adc_data_t *adc_data = NULL;

    adc_data = (wm_drv_adc_data_t *)dev->drv;

    if (adc_data) {
        if (WM_ADC_STATE_IDLE != adc_data->adc_drv.state) {
            wm_log_error("adc dev not idle");
            return WM_ERR_NOT_ALLOWED;
        }

        WM_DRV_ADC_LOCK(adc_data->adc_drv.mutex);
        if (adc_data) {
            err = wm_hal_thermal_sensor_read_temperature(&adc_data->adc_hal, temperature_val);
            WM_DRV_ADC_UNLOCK(adc_data->adc_drv.mutex);
            return err;
        }
        WM_DRV_ADC_UNLOCK(adc_data->adc_drv.mutex);
    }

    return WM_ERR_FAILED;
}

static int wm_w800_drv_adc_read_voltage(wm_device_t *dev, int *voltage)
{
    if (!dev || !voltage) {
        return WM_ERR_INVALID_PARAM;
    }

    int err                     = 0;
    wm_drv_adc_data_t *adc_data = NULL;

    adc_data = (wm_drv_adc_data_t *)dev->drv;

    if (adc_data) {
        if (WM_ADC_STATE_IDLE != adc_data->adc_drv.state) {
            wm_log_error("adc dev not idle");
            return WM_ERR_NOT_ALLOWED;
        }

        WM_DRV_ADC_LOCK(adc_data->adc_drv.mutex);
        if (adc_data) {
            err = wm_hal_adc_chip_voltage_read_vdd(&adc_data->adc_hal, voltage);
            WM_DRV_ADC_UNLOCK(adc_data->adc_drv.mutex);
            return err;
        }
        WM_DRV_ADC_UNLOCK(adc_data->adc_drv.mutex);
    }

    return WM_ERR_FAILED;
}

static int wm_w800_drv_adc_cal_voltage(wm_device_t *dev, int vol)
{
    if (!dev) {
        return WM_ERR_INVALID_PARAM;
    }

    int err                     = WM_ERR_FAILED;
    wm_drv_adc_data_t *adc_data = NULL;

    adc_data = (wm_drv_adc_data_t *)dev->drv;

    if (adc_data) {
        WM_ADC_GET_RESULT(vol);
        WM_ADC_SIGNED_TO_UNSIGNED(vol);

        if (adc_data) {
            err = wm_hal_adc_cal_voltage(&adc_data->adc_hal, vol);
        }
    }

    return err;
}

const wm_drv_adc_ops_t wm_drv_adc_ops = {
    .init            = wm_w800_drv_adc_init,
    .deinit          = wm_w800_drv_adc_deinit,
    .set_cfg         = wm_w800_drv_adc_set_cfg,
    .register_handle = wm_w800_drv_adc_register_handle,
    .oneshot         = wm_w800_drv_adc_oneshot,
    .polling         = wm_w800_drv_adc_polling,
    .start_dma       = wm_w800_drv_adc_start_dma,
    .start_intr      = wm_w800_drv_adc_start_it,
    .stop_intr       = wm_w800_drv_adc_stop_it,
    .read_temp       = wm_w800_drv_adc_read_temp,
    .read_voltage    = wm_w800_drv_adc_read_voltage,
    .cal_voltage     = wm_w800_drv_adc_cal_voltage,
};

/**
 * @file wm_drv_ops_spi_master.h
 *
 * @brief SPI Master Module
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

#ifndef __WM_DRV_OPS_SPI_MASTER_H__
#define __WM_DRV_OPS_SPI_MASTER_H__

#include "wmsdk_config.h"
#include "wm_types.h"
#include "wm_osal.h"
#include "wm_dt_hw.h"
#include "wm_drv_spi_master.h"

typedef struct {
    bool is_init;
    wm_os_mutex_t *lock;
    wm_device_t *rcc_dev;
    wm_device_t *dma_dev;
    wm_hal_spim_dev_t hal_dev;
    wm_os_sem_t *sync_async_sem; //use to notice async done
    void *priv;
} wm_drv_spim_ctx_t;
typedef wm_drv_spim_ctx_t wm_drv_spis_ctx_t;

typedef struct {
    wm_spim_callback_t callback;
    void *usr_callback_data;
    bool trx_with_dma;
    wm_dt_hw_spim_dev_cfg_t config;
    bool xfer_continue;
    wm_os_sem_t *xfer_sem; //use to transceive tx/rx length over hw capibilty in one time
    uint8_t transceive_flag;
} spim_drv_priv_t;

typedef struct {
    int (*init)(wm_device_t *dev);
    int (*deinit)(wm_device_t *dev);
    int (*transceive_sync)(wm_device_t *dev, const wm_dt_hw_spim_dev_cfg_t *config, spim_transceive_t *desc,
                           uint32_t ms_to_wait);
    int (*transceive_async)(wm_device_t *dev, const wm_dt_hw_spim_dev_cfg_t *config, spim_transceive_t *desc,
                            wm_spim_callback_t callback, void *usr_data);
} wm_drv_spim_ops_t;
typedef wm_drv_spim_ops_t wm_drv_spis_ops_t;

#endif

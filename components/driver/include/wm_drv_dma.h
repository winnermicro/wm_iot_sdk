/**
 * @file wm_drv_dma.h
 *
 * @brief dma driver layer header file
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

#ifndef __WM_DRV_DMA_H__
#define __WM_DRV_DMA_H__

#include "wm_soc_cfgs.h"
#include "wm_osal.h"
#include "wm_types.h"
#include "wm_dt.h"
#include "wm_hal_dma.h"

#ifdef __cplusplus
extern "C" {
#endif

/// dma driver utils

/**
 * @brief DMA malloc
 */
#define WM_DRV_DMA_MALLOC                 wm_os_internal_malloc

/**
 * @brief DMA free
 */
#define WM_DRV_DMA_FREE                   wm_os_internal_free

/**
 * @brief DMA len bytes
 */
#define WM_DRV_DMA_LEN_IN_BYTES(t)        sizeof(t)

/**
 * @brief DMA memset
 */
#define WM_DRV_DMA_MEM_ZERO(target, size) memset(target, 0, size)

/**
 * @brief DMA lock struct
 */
#define WM_DRV_DMA_LOCK_T                 wm_os_mutex_t

/**
 * @brief DMA create lock
 */
#define WM_DRV_DMA_CREATE_LOCK            wm_os_internal_mutex_create

/**
 * @brief DMA delete lock
 */
#define WM_DRV_DMA_DEL_LOCK               wm_os_internal_mutex_delete

/**
 * @brief DMA acquire lock
 */
#define WM_DRV_DMA_GET_LOCK               wm_os_internal_mutex_acquire

/**
 * @brief DMA release lock
 */
#define WM_DRV_DMA_REL_LOCK               wm_os_internal_mutex_release

/**
 * @brief DMA first desc
 */
#define WM_DRV_DMA_FIRST_DESC             (true)

/// TODO, WM_HAL_DMA_UT define in other space
#define WM_DRV_DMA_UT
#ifdef WM_DRV_DMA_UT
#define WM_DRV_DMA_STATIC
#else
#define WM_DRV_DMA_STATIC static
#endif

/**
 * @enum wm_drv_dma_status_e
 * @brief dma drv operation status
 * TODO, refine after wm error code design final, merge with dma hal
 */
typedef enum wm_drv_dma_status_e {
    WM_DRV_DMA_STATUS_ERROR         = -6,
    WM_DRV_DMA_STATUS_TIMEOUT       = WM_ERR_TIMEOUT,
    WM_DRV_DMA_STATUS_BUSY          = WM_ERR_BUSY,
    WM_DRV_DMA_STATUS_RESOURCE_LACK = -3,
    WM_DRV_DMA_STATUS_NO_MEM        = WM_ERR_NO_MEM,
    WM_DRV_DMA_STATUS_FAILED        = WM_ERR_FAILED,
    WM_DRV_DMA_STATUS_SUCCESS       = WM_ERR_SUCCESS
} wm_drv_dma_status_t;

/**
 * @enum wm_drv_dma_reload_e
 * @brief drv dma auto reload control
 */
typedef enum wm_drv_dma_reload_e {
    WM_DRV_DMA_RELOAD_DISABLE = 0x0,
    WM_DRV_DMA_RELOAD_ENABLE,
    WM_DRV_DMA_RELOAD_MAX
} wm_drv_dma_reload_t;

/**
 * @enum wm_drv_dma_addr_mode_e
 * @brief drv dma addr inc mode
 */
typedef enum wm_drv_dma_addr_mode_e {
    WM_DRV_DMA_ADDR_FIXED = 0x0,
    WM_DRV_DMA_ADDR_INC,
    //TBD, delete rsvd in driver
    WM_DRV_DMA_ADDR_RSVD,
    WM_DRV_DMA_ADDR_WARP,
    WM_DRV_DMA_ADDR_MAX
} wm_drv_dma_addr_mode_t;

/**
 * @enum wm_drv_dma_ch_int_en_e
 * @brief drv dma ch interrupt control
 */
typedef enum wm_drv_dma_ch_int_en_e {
    WM_DRV_DMA_CH_INT_DISABLE = 0x0,
    WM_DRV_DMA_CH_INT_ENABLE,
    WM_DRV_DMA_CH_INT_MAX
} wm_drv_dma_ch_int_en_t;

/**
 * @enum wm_drv_dma_ch_e
 * @brief drv dma ch
 */
typedef enum wm_drv_dma_ch_e {
    WM_DRV_DMA_CH_0   = WM_DMA_CH_0,
    WM_DRV_DMA_CH_1   = WM_DMA_CH_1,
    WM_DRV_DMA_CH_2   = WM_DMA_CH_2,
    WM_DRV_DMA_CH_3   = WM_DMA_CH_3,
    WM_DRV_DMA_CH_4   = WM_DMA_CH_4,
    WM_DRV_DMA_CH_5   = WM_DMA_CH_5,
    WM_DRV_DMA_CH_6   = WM_DMA_CH_6,
    WM_DRV_DMA_CH_7   = WM_DMA_CH_7,
    WM_DRV_DMA_CH_MAX = WM_DMA_CH_MAX,
} wm_drv_dma_ch_t;

/**
 * @enum wm_drv_dma_mode_e
 * @brief drv dma mode
 */
typedef enum wm_drv_dma_mode_e {
    WM_DRV_DMA_NORMAL_MODE = 0x0,
    WM_DRV_DMA_WARP_MODE,
    WM_DRV_DMA_LIST_MODE,
    WM_DRV_DMA_MODE_MAX
} wm_drv_dma_mode_t;

/**
 * @enum wm_drv_dma_req_src_e
 * @brief drv dma request source
 */
typedef enum wm_drv_dma_req_src_e {
    WM_DRV_DMA_PWM_CAP0_REQ = 0x0,
    WM_DRV_DMA_PWM_CAP1_REQ,
    WM_DRV_DMA_SPI_RX_REQ,
    WM_DRV_DMA_SPI_TX_REQ,
    WM_DRV_DMA_ADC_CH0_REQ,
    WM_DRV_DMA_ADC_CH1_REQ,
    WM_DRV_DMA_ADC_CH2_REQ,
    WM_DRV_DMA_ADC_CH3_REQ,
    WM_DRV_DMA_I2S_RX_REQ,
    WM_DRV_DMA_I2S_TX_REQ,
    WM_DRV_DMA_SDIO_HOST_REQ,
    WM_DRV_DMA_CH_UART0_TX_REQ,
    WM_DRV_DMA_CH_UART0_RX_REQ,
    WM_DRV_DMA_CH_UART1_TX_REQ,
    WM_DRV_DMA_CH_UART1_RX_REQ,
    WM_DRV_DMA_CH_UART2_TX_REQ,
    WM_DRV_DMA_CH_UART2_RX_REQ,
    WM_DRV_DMA_CH_UART3_TX_REQ,
    WM_DRV_DMA_CH_UART3_RX_REQ,
    WM_DRV_DMA_CH_UART4_TX_REQ,
    WM_DRV_DMA_CH_UART4_RX_REQ,
    WM_DRV_DMA_CH_UART5_TX_REQ,
    WM_DRV_DMA_CH_UART5_RX_REQ,
    //TODO, more
    WM_DRV_DMA_REQ_MAX
} wm_drv_dma_req_src_t;

/**
 * @typedef wm_drv_dma_ch_callback_t
 * @brief dma drv operation done ch callback
 */
typedef void (*wm_drv_dma_ch_callback_t)(wm_dma_ch_t ch, uint32_t sts, void *user_data);

/**
 * @typedef wm_drv_dma_callback_t
 * @brief dma drv operation done callback
 */
typedef void (*wm_drv_dma_callback_t)(uint32_t sts, void *user_data);

/**
 * @defgroup WM_DMA_Structs WM DMA Structs
 * @brief WinnerMicro DMA Structs
 */

/**
 * @typedef wm_drv_dma_ch_cb_info_t
 * @brief dma user ch callback information
 */
typedef struct wm_drv_dma_ch_cb_info_s {
    void *cb_priv;
    wm_drv_dma_ch_callback_t cb;
} wm_drv_dma_ch_cb_info_t;

/**
 * @struct wm_drv_dma_cb_info_t
 * @brief dma user callback information
 */
typedef struct wm_drv_dma_cb_info_s {
    void *cb_priv;
    wm_drv_dma_callback_t cb;
} wm_drv_dma_cb_info_t;

/**
 * @struct wm_drv_dma_ctrl_t
 * @brief dma drv control information
 */
typedef struct wm_drv_dma_ctrl_s {
    /* dma drv ctrl information DW0 */
    uint32_t len           : 16; /* [15:0]  total transfer len in bytes */
    uint32_t ch            : 4;  /* [19:16] wm_drv_dma_ch_t */
    uint32_t src_inc_mode  : 2;  /* [27:26] wm_drv_dma_addr_mode_t */
    uint32_t dest_inc_mode : 2;  /* [29:28] wm_drv_dma_addr_mode_t */
    uint32_t int_en        : 1;  /* [30]    dma done interrupt enable/disable */
    uint32_t auto_reload   : 1;  /* [31]    wm_drv_dma_reload_t */

    /* dma drv ctrl information DW1 */
    uint32_t dma_mode      : 2; /* [1:0]   wm_drv_dma_mode_t */
    uint32_t req_src       : 6; /* [7:2]   wm_drv_dma_req_src_t */

    /* dma drv ctrl information DW2 */
    uint32_t src_warp_len  : 16; /* [15:0]  src buf warp len */
    uint32_t dest_warp_len : 16; /* [31:16] dest buf warp len */
} wm_drv_dma_ctrl_t;

/**
 * @struct wm_drv_dma_desc_t
 * @brief dma drv config desc
 */
typedef struct wm_drv_dma_desc_s {
    uint32_t src;
    uint32_t dest;
    wm_drv_dma_ctrl_t ctrl;
    struct wm_drv_dma_desc_s *next;
} wm_drv_dma_desc_t;

/**
 * @struct wm_drv_dma_xfer_cfg_t
 * @brief dma drv transfer config, this for junior user
 */
typedef struct wm_drv_dma_xfer_cfg_s {
    uint32_t src;
    uint32_t dest;
    uint32_t len         : 16; /* [15:0]  total transfer len in bytes */
    uint32_t req_src     : 6;  /* [25:20] wm_drv_dma_req_src_t */
    uint32_t auto_reload : 1;  /* [26]    wm_drv_dma_reload_t */
    uint32_t reserved    : 5;  /* [31:27] reserved */
} wm_drv_dma_xfer_cfg_t;

/**
 * @struct wm_drv_dma_irq_priv_t
 * @brief dma drv irq info
 */
typedef struct wm_drv_dma_irq_priv_s {
    void *dev;
    //TBD
} wm_drv_dma_irq_priv_t;

/**
 * @struct wm_drv_dma_ctx_t
 * @brief dma driver context
 */
typedef struct wm_drv_dma_ctx_s {
    WM_DRV_DMA_LOCK_T *lock;
    wm_device_t *clock_dev;
    wm_drv_dma_irq_priv_t irq_priv;
    wm_drv_dma_ch_cb_info_t user_ch_cb[WM_DRV_DMA_CH_MAX];
    void *desc[WM_DRV_DMA_CH_MAX];
} wm_drv_dma_ctx_t;

/**
 * @struct wm_dma_data_t
 * @brief dma data
 */
typedef struct wm_dma_data_s {
    wm_hal_dma_dev_t hal_dev;
    wm_drv_dma_ctx_t drv_ctx;
} wm_dma_data_t;

/**
 * @defgroup WM_DMA_APIs WM DRV DMA APIs
 * @brief WinnerMicro DRV DMA APIs
 */

/**
 * @addtogroup WM_DMA_APIs
 * @{
 */

/**
 * @brief          dma drv init
 * @param[in]      dma_name
 * @return         dma device reference
 */
wm_device_t *wm_drv_dma_init(const char *dma_name);

/**
 * @brief          dma drv deinit
 * @param[in]      dev device reference
 * @return         deinit operation status
 */
wm_drv_dma_status_t wm_drv_dma_deinit(wm_device_t *dev);

/**
 * @brief          request dma channel
 * @param[in]      dev device reference
 * @param[out]     ch channel that alloced by dma drv
 * @param[in]      wait_time channel request timeout
 * @return         dma channel request operation status
 */
wm_drv_dma_status_t wm_drv_dma_request_ch(wm_device_t *dev, uint8_t *ch, uint32_t wait_time);

/**
 * @brief          release dma channel
 * @param[in]      dev device reference
 * @param[in]      ch channel that need release, please make sure to use wm_drv_dma_request_ch to apply. If not applied, there may be exceptions
 * @param[in]      wait_time channel release timeout
 * @return         dma channel release operation status
 */
wm_drv_dma_status_t wm_drv_dma_release_ch(wm_device_t *dev, uint8_t ch, uint32_t wait_time);

/**
 * @brief          register dma done callback for user
 * @param[in]      dma_dev device reference
 * @param[in]      ch channel, please make sure to use wm_drv_dma_request_ch to apply. If not applied, there may be exceptions
 * @param[in]      dma_callback done user callback
 * @param[in]      user_data data
 * @return         callback register operation status
 */
wm_drv_dma_status_t wm_drv_dma_register_callback(wm_device_t *dma_dev, wm_dma_ch_t ch, wm_drv_dma_ch_callback_t dma_callback,
                                                 void *user_data);

/**
 * @brief          unregister dma done callback for user
 * @param[in]      dma_dev device reference
 * @param[in]      ch channel, please make sure to use wm_drv_dma_request_ch to apply. If not applied, there may be exceptions
 * @return         callback unregister operation status
 */
wm_drv_dma_status_t wm_drv_dma_unregister_callback(wm_device_t *dma_dev, wm_dma_ch_t ch);

/**
 * @brief          config transfer param to dma, support mormal mode/list mode/warp mode/poolling mode/interrupt mode
 * @param[in]      dma_dev device reference
 * @param[in]      dma_desc config
 * @return         dma config operation status
 */
wm_drv_dma_status_t wm_drv_dma_config(wm_device_t *dma_dev, wm_drv_dma_desc_t *dma_desc);

/**
 * @brief          start dma transfer
 * @param[in]      dma_dev device reference
 * @param[in]      ch channel to start, please make sure to use wm_drv_dma_request_ch to apply. If not applied, there may be exceptions
 * @return         dma start operation status
 */
wm_drv_dma_status_t wm_drv_dma_start(wm_device_t *dma_dev, wm_dma_ch_t ch);

/**
 * @brief          stop dma transfer
 * @param[in]      dma_dev device reference
 * @param[in]      ch channel to stop, please make sure to use wm_drv_dma_request_ch to apply. If not applied, there may be exceptions
 * @return         dma stop operation status
 */
wm_drv_dma_status_t wm_drv_dma_stop(wm_device_t *dma_dev, wm_dma_ch_t ch);

/**
 * @brief          get dma status
 * @param[in]      dma_dev device reference
 * @param[in]      ch channel to get status, please make sure to use wm_drv_dma_request_ch to apply. If not applied, there may be exceptions
 * @param[out]     dma_sts status
 * @return         get dma status operation status
 */
wm_drv_dma_status_t wm_drv_dma_get_status(wm_device_t *dma_dev, wm_dma_ch_t ch, wm_dma_sts_info_t *dma_sts);

/**
 * @brief          config and start dma
 * @param[in]      dma_dev device reference
 * @param[in]      ch channel that need release, please make sure to use wm_drv_dma_request_ch to apply. If not applied, there may be exceptions
 * @param[in]      dma_xfer xfer config 
 * @return         dma transfer operation status
 */
wm_drv_dma_status_t wm_drv_dma_transfer(wm_device_t *dma_dev, wm_dma_ch_t ch, wm_drv_dma_xfer_cfg_t *dma_xfer);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_DRV_DMA_H__ */

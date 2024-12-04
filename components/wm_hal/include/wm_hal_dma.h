/**
 * @file wm_hal_dma.h
 *
 * @brief dma hardware abstraction layer header file
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

#ifndef __WM_HAL_DMA_H__
#define __WM_HAL_DMA_H__

#include <string.h>
#include "wm_osal.h"
#include "wm_types.h"
#include "wm_list.h"
#include "wm_soc_cfgs.h"
#include "wm_hal_irq.h"
#include "wm_ll_dma.h"

#ifdef __cplusplus
extern "C" {
#endif

/// hal dma macro
#define WM_HAL_DMA_LIST                           struct dl_list
#define WM_HAL_DMA_LIST_INIT(list)                dl_list_init(list)
#define WM_HAL_DMA_LIST_PUSH(list, listh)         dl_list_add_tail(list, listh)
#define WM_HAL_DMA_LIST_FIRST(list, type, member) dl_list_first(list, type, member)
#define WM_HAL_DMA_LIST_DEL(list)                 dl_list_del(list)
#define WM_HAL_DMA_MEM_ZERO(target, size)         memset(target, 0, size)

/// TODO, WM_HAL_DMA_UT define in other space
#define WM_HAL_DMA_UT
#ifdef WM_HAL_DMA_UT
#define WM_HAL_DMA_STATIC
#else
#define WM_HAL_DMA_STATIC static
#endif

#define WM_HAL_DMA_ENTER_CRITIAL wm_hal_irq_set_critical()
#define WM_HAL_DMA_EXIT_CRITIAL  wm_hal_irq_release_critical()

/**
 * @brief dma hal operation status
 * TODO, refine after wm error code design final
 */
typedef enum wm_hal_dma_status_e {
    WM_HAL_DMA_STATUS_ERROR             = -4,
    WM_HAL_DMA_STATUS_RESOURCE_LACK     = -3,
    WM_HAL_DMA_STATUS_BUSY              = WM_ERR_BUSY,
    WM_HAL_DMA_STATUS_INVALID_PARAMETER = WM_ERR_INVALID_PARAM,
    WM_HAL_DMA_STATUS_FAILED            = WM_ERR_FAILED,
    WM_HAL_DMA_STATUS_SUCCESS           = WM_ERR_SUCCESS
} wm_hal_dma_status_t;

/**
 * @brief dma hal operation done callback
 */
typedef void (*wm_hal_dma_callback_t)(wm_dma_ch_t ch, uint32_t sts, void *user_data);

/**
 * @brief dma warp len info
 */
typedef struct wm_hal_dma_warp_info_s {
    uint16_t src_warp_len;
    uint16_t dest_warp_len;
} wm_hal_dma_warp_info_t;

/**
 * @brief dma control nonchain information
 */
typedef union {
    struct {
        uint32_t auto_reload   : 1;  /* [0]     wm_dma_reload_t */
        uint32_t src_addr_inc  : 2;  /* [2:1]   wm_dma_addr_mode_t */
        uint32_t dest_addr_inc : 2;  /* [4:3]   wm_dma_addr_mode_t */
        uint32_t data_unit     : 2;  /* [6:5]   wm_dma_xfer_unit_t */
        uint32_t burst_size    : 1;  /* [7]     wm_dma_burst_unit_t */
        uint32_t len           : 16; /* [23:8]  total transfer len */
        uint32_t reserved_24   : 8;  /* [31:24] reserved */
    };
    uint32_t val;
} wm_hal_dma_nonchain_ctrl_t;

/**
 * @brief dma chain control information
 */
typedef union {
    struct {
        uint32_t src_addr_inc  : 2;  /* [1:0]   wm_dma_addr_mode_t */
        uint32_t dest_addr_inc : 2;  /* [3:2]   wm_dma_addr_mode_t */
        uint32_t data_unit     : 2;  /* [5:4]   wm_dma_xfer_unit_t */
        uint32_t burst_size    : 1;  /* [6]     wm_dma_burst_unit_t */
        uint32_t len           : 16; /* [22:7]  total transfer len */
        uint32_t reserved_25   : 9;  /* [31:23] reserved */
    };
    uint32_t val;
} wm_hal_dma_chain_ctrl_t;

/**
 * @brief dma control information for list mode or non-list mode
 */
typedef union {
    wm_hal_dma_nonchain_ctrl_t nonchain;
    wm_hal_dma_chain_ctrl_t chain;
} wm_hal_dma_ctrl_t;

/**
 * @brief dma exntend control information
 */
typedef union {
    struct {
        uint32_t ch            : 4;  /* [3:0]   wm_dma_ch_t */
        uint32_t int_en        : 1;  /* [4]     ch interrupt enable/disable */
        uint32_t int_type      : 2;  /* [6:5]   wm_dma_int_type_t */
        uint32_t mode          : 1;  /* [7]     wm_dma_mode_t */
        uint32_t req_sel       : 4;  /* [11:8]  wm_dma_req_sel_t */
        uint32_t chain_mode    : 1;  /* [12]    wm_dma_chain_mode_ctrl_t */
        uint32_t uart_dma_ch   : 3;  /* [15:13] wm_dma_ch_uart_sel_t */
        uint32_t warp_mode_en  : 1;  /* [16]    wm_dma_warp_mode_ctrl_t */
        uint32_t chain_mode_en : 1;  /* [17]    wm_dma_chain_mode_ctrl_t */
        uint32_t reserved_18   : 14; /* [31:18] reserved */
    };
    uint32_t val;
} wm_hal_dma_extend_ctrl_t;

/**
 * @brief dma hal config desc, compatible with list mode
 *
 *  | 1st desc |  used by DMA HW in list mode or DMA HAL in non-list mode
 *  |          |
 *  | 2nd desc |  used by DMA HAL in non-list mode
 */
typedef struct wm_hal_dma_desc_s {
    /* 1st desc */
    uint32_t vld;
    wm_hal_dma_ctrl_t ctrl;
    uint32_t src_addr;
    uint32_t dest_addr;
    struct wm_hal_dma_desc_s *next;
    /* 2nd desc */
    wm_hal_dma_extend_ctrl_t extend_ctrl;
    wm_hal_dma_warp_info_t warp_info;
} wm_hal_dma_desc_t;

/**
 * @brief dma hal partial config desc that may change in each transfer
 */
typedef struct wm_hal_dma_cfg_partial_desc_s {
    uint32_t src_addr;
    uint32_t dest_addr;
    uint16_t len;
    wm_dma_ch_t ch;
} wm_hal_dma_cfg_partial_desc_t;

/**
 * @brief dma user callback information
 */
typedef struct wm_hal_dma_cb_info_s {
    void *cb_priv;
    wm_hal_dma_callback_t cb;
} wm_hal_dma_cb_info_t;

/**
 * @brief dma ch list
 */
typedef struct wm_hal_dma_ch_list_s {
    WM_HAL_DMA_LIST list;
    uint32_t rsvd;
    uint8_t ch;
} wm_hal_dma_ch_list_t;

/**
 * @brief dma ch use information
 */
typedef union {
    uint32_t ch;
    uint32_t ch_map;
} wm_hal_dma_ch_use_t;

/**
 * @brief dma irq info
 */
typedef struct wm_hal_dma_irq_priv_s {
    void *dev;
    wm_hal_dma_ch_use_t ch_use;
    uint8_t ch_irq;
} wm_hal_dma_irq_priv_t;

/**
 * @brief dma device data
 */
typedef struct wm_hal_dma_dev_s {
    wm_dma_reg_t *reg_base;
    uint32_t irq_num;
    wm_hal_dma_irq_priv_t *irq_priv[WM_DMA_CH_IRQ_MAX];
    wm_hal_dma_cb_info_t dma_user_cb[WM_DMA_CH_MAX];
    WM_HAL_DMA_LIST ch_list;
    /* used for dma ch alloc/release method 3 */
    //volatile uint32_t dma_ch;
    wm_hal_dma_desc_t *desc_list;
} wm_hal_dma_dev_t;

/**
 * @brief          dma hal init
 * @param[in]      dma_dev device reference
 * @return         init operation status
 */
wm_hal_dma_status_t wm_hal_dma_init(wm_hal_dma_dev_t *dma_dev);

/**
 * @brief          dma hal deinit
 * @param[in]      dma_dev device reference
 * @return         deinit operation status
 */
wm_hal_dma_status_t wm_hal_dma_deinit(wm_hal_dma_dev_t *dma_dev);

/**
 * @brief          register dma done callback for user
 * @param[in]      dma_dev device reference
 * @param[in]      ch channel
 * @param[in]      dma_callback done user callback
 * @param[in]      user_data data
 * @return         callback register operation status
 */
wm_hal_dma_status_t wm_hal_dma_register_callback(wm_hal_dma_dev_t *dma_dev, wm_dma_ch_t ch, wm_hal_dma_callback_t dma_callback,
                                                 void *user_data);

/**
 * @brief          unregister dma done callback for user
 * @param[in]      dma_dev device reference
 * @param[in]      ch channel
 * @return         callback unregister operation status
 */
wm_hal_dma_status_t wm_hal_dma_unregister_callback(wm_hal_dma_dev_t *dma_dev, wm_dma_ch_t ch);

/**
 * @brief          config transfer param to dma, support single mode/list mode/warp mode/pooling mode/interrupt mode
 * @param[in]      dma_dev device reference
 * @param[in]      dma_desc config
 * @return         dma config operation status
 */
wm_hal_dma_status_t wm_hal_dma_config(wm_hal_dma_dev_t *dma_dev, wm_hal_dma_desc_t *dma_desc);

/**
 * @brief          config partial transfer param to dma
 * @param[in]      dma_dev device reference
 * @param[in]      reload_desc partial config
 * @return         dma partail config operation status
 */
wm_hal_dma_status_t wm_hal_dma_config_partial(wm_hal_dma_dev_t *dma_dev, wm_hal_dma_cfg_partial_desc_t *reload_desc);

/**
 * @brief          start dma transfer
 * @param[in]      dma_dev device reference
 * @param[in]      ch channel to start
 * @return         dma start operation status
 */
wm_hal_dma_status_t wm_hal_dma_start(wm_hal_dma_dev_t *dma_dev, wm_dma_ch_t ch);

/**
 * @brief          stop dma transfer
 * @param[in]      dma_dev device reference
 * @param[in]      ch channel to stop
 * @return         dma stop operation status
 */
wm_hal_dma_status_t wm_hal_dma_stop(wm_hal_dma_dev_t *dma_dev, wm_dma_ch_t ch);

/**
 * @brief          get dma status
 * @param[in]      dma_dev device reference
 * @param[in]      ch channel to get status
 * @param[out]     dma_sts status info
 * @return         get dma status operation status
 */
wm_hal_dma_status_t wm_hal_dma_get_status(wm_hal_dma_dev_t *dma_dev, wm_dma_ch_t ch, wm_dma_sts_info_t *dma_sts);

/**
 * @brief          request dma channel
 * @param[in]      dma_dev device reference
 * @param[out]     ch channel that alloced by dma hal
 * @return         dma channel request operation status
 */
wm_hal_dma_status_t wm_hal_dma_request_ch(wm_hal_dma_dev_t *dma_dev, uint8_t *ch);

/**
 * @brief          release dma channel
 * @param[in]      dma_dev device reference
 * @param[out]     ch channel that alloced by dma hal
 * @return         dma channel release operation status
 */
wm_hal_dma_status_t wm_hal_dma_release_ch(wm_hal_dma_dev_t *dma_dev, uint8_t ch);

/**
 * @brief          dma isr entry
 * @param[in]      irq channel irq numner
 * @param[out]     priv channel priv data
 * @return         void
 */
void wm_hal_dma_isr_entry(wm_irq_no_t irq, void *priv);

/**
 * @brief          clear one dma request
 * @param[in]      dma_dev device reference
 * @param[in]      req_sel request id
 * @return         dma channel release operation status
 */
wm_hal_dma_status_t wm_hal_dma_clr_req(wm_hal_dma_dev_t *dma_dev, wm_dma_req_sel_t req_sel);

#ifdef __cplusplus
}
#endif

#endif /* __WM_HAL_DMA_H__ */

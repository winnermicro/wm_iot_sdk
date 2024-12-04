/**
 * @file wm_drv_dma_m2m_demo.h
 *
 * @brief dma DRV API memory to memory transfer demo main
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
#ifndef __WM_DRV_DMA_M2M_DEMO__
#define __WM_DRV_DMA_M2M_DEMO__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define DRV_DMA_M2M_EXAMPLE_XFER_LEN      (0x60)
#define DRV_DMA_M2M_EXAMPLE_XFER_LIST_LEN (0x2)
#define DRV_DMA_M2M_EXAMPLE_DMA_LIST_VLD  (0x1f)
#define DRV_DMA_M2M_EXAMPLE_MEMCMP        memcmp
#define DRV_DMA_M2M_EXAMPLE_MEMSET        memset
#define DRV_DMA_M2M_EXAMPLE_DUMP_CNT_LIST DRV_DMA_M2M_EXAMPLE_XFER_LIST_LEN
#define DRV_DMA_M2M_EXAMPLE_DUMP_CNT      (DRV_DMA_M2M_EXAMPLE_XFER_LIST_LEN - 1)
#define DRV_DMA_M2M_EXAMPLE_DMA_NAME      "dma"
#define DRV_DMA_M2M_EXAMPLE_TO_US         (portMAX_DELAY)

/**
 * @brief dma m2m example dma async status
 */
typedef enum wm_drv_dma_aysnc_sts_e {
    DRV_DMA_M2M_EXAMPLE_DMA_AYSNC_DONE = 0x0,
    /// TODO
    DRV_DMA_M2M_EXAMPLE_DMA_AYSNC_MAX
} wm_drv_dma_aysnc_sts_t;

/**
 * @brief dma m2m example id
 */
typedef enum wm_drv_dma_m2m_demo_id {
    DRV_DMA_M2M_EXAMPLE_INT_ASYNC_NONLIST = 0x0,
    DRV_DMA_M2M_EXAMPLE_SYNC_NONLIST,
    DRV_DMA_M2M_EXAMPLE_INT_ASYNC_WARP_NONlIST,
    DRV_DMA_M2M_EXAMPLE_INT_ASYNC_LIST
} wm_drv_dma_m2m_demo_id_t;

void wm_drv_dma_m2m_demo(void *p);

#endif /* __WM_DRV_DMA_M2M_DEMO__ */
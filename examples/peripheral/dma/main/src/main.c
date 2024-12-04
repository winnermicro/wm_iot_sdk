/**
 * @file main.c
 *
 * @brief dma HAL API memory to memory transfer demo main
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

#include <stdio.h>
#include "wmsdk_config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wm_drv_dma_m2m_demo.h"

#define WM_DRV_DMA_DEMO_TASK_STACK (2048)
#define WM_DRV_DMA_DEMO_TASK_PRIO  (configMAX_PRIORITIES - 8)

TaskHandle_t wm_drv_dma_demo_task_handle = NULL;
int main(void)
{
    xTaskCreate(wm_drv_dma_m2m_demo, "wm_drv_dma_demo", WM_DRV_DMA_DEMO_TASK_STACK, NULL, WM_DRV_DMA_DEMO_TASK_PRIO,
                &wm_drv_dma_demo_task_handle);
    return 0;
}

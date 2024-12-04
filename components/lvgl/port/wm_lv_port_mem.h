/**
 * @file wm_lv_port_mem.h
 *
 * @brief LVGL Customer Memory Porting Layer
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

#ifndef __WM_LV_PORT_CUST_MEM_H__
#define __WM_LV_PORT_CUST_MEM_H__

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "wm_heap.h"

/*********************
 *      DEFINES
 *********************/

/* as LVGL's Kconfig not include the customer memory functions redirection, 
 * so create a new cusetomer header file to redefine them.
 */

#if defined(CONFIG_LV_MEM_CUSTOM) && defined(CONFIG_COMPONENT_DRIVER_PSRAM_ENABLED)

#ifdef CONFIG_HEAP_USE_TRACING
#define wm_lv_psram_malloc(size)       wm_heap_caps_alloc_tracing(size, WM_HEAP_CAP_SPIRAM, __FILE__, __LINE__)
#define wm_lv_psram_realloc(ptr, size) wm_heap_caps_realloc_tracing(ptr, size, WM_HEAP_CAP_SPIRAM, __FILE__, __LINE__)
#else
#define wm_lv_psram_malloc(size)       wm_heap_caps_alloc(size, WM_HEAP_CAP_SPIRAM)
#define wm_lv_psram_realloc(ptr, size) wm_heap_caps_realloc(ptr, size, WM_HEAP_CAP_SPIRAM)
#endif
#define wm_lv_psram_free(ptr)        wm_heap_caps_free(ptr)

/* set LVGL memory configuration options */
#define CONFIG_LV_MEM_CUSTOM_ALLOC   wm_lv_psram_malloc
#define CONFIG_LV_MEM_CUSTOM_FREE    wm_lv_psram_free
#define CONFIG_LV_MEM_CUSTOM_REALLOC wm_lv_psram_realloc

#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*__WM_LV_PORT_CUST_MEM_H__*/
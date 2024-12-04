/**
 * @file wm_gpio_reg_struct.h
 *
 * @brief gpio registers struct Module
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

#ifndef __WM_GPIO_REG_STRUCT_H__
#define __WM_GPIO_REG_STRUCT_H__

#include <wm_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief gpio register layout
 * Notice: must 32bit for each member
 */
typedef struct {
    volatile uint32_t data;    /**< WM_GPIO_BASE_ADDR + 0x00, GPIO Data I/O Value */
    volatile uint32_t data_en; /**< WM_GPIO_BASE_ADDR + 0x04, GPIO Data W/R Enable */
    volatile uint32_t dir;     /**< WM_GPIO_BASE_ADDR + 0x08, Set GPIO Pin Dir */
    volatile uint32_t pull_en; /**< WM_GPIO_BASE_ADDR + 0x0C, Pull_up enable */
    volatile uint32_t af_sel;  /**< WM_GPIO_BASE_ADDR + 0x10, Set GPIO Pin AF */
    volatile uint32_t sf_s1;   /**< WM_GPIO_BASE_ADDR + 0x14, Select One of AF With SF_S1 */
    volatile uint32_t af_s0;   /**< WM_GPIO_BASE_ADDR + 0x18, Select One of AF With AF_S0 */
    volatile uint32_t dn_ena;  /**< WM_GPIO_BASE_ADDR + 0x1C, Pull_down enable */
    volatile uint32_t is;      /**< WM_GPIO_BASE_ADDR + 0x20, Interupt triggle mode select */
    volatile uint32_t ibe;     /**< WM_GPIO_BASE_ADDR + 0x24, single or double Edge select */
    volatile uint32_t iev;     /**< WM_GPIO_BASE_ADDR + 0x28, low/high level or falling/rising edge select */
    volatile uint32_t ie;      /**< WM_GPIO_BASE_ADDR + 0x2C, Irq enable */
    volatile uint32_t ris;     /**< WM_GPIO_BASE_ADDR + 0x30, irq flag */
    volatile uint32_t mis;     /**< WM_GPIO_BASE_ADDR + 0x34, irq flag */
    volatile uint32_t ic;      /**< WM_GPIO_BASE_ADDR + 0x38, Clr irq flag */
} wm_gpio_reg_t;

#ifdef __cplusplus
}
#endif

#endif /* __WM_GPIO_REG_STRUCT_H__ */

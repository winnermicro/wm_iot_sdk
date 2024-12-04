
/**
 * @file wm_hal_psram.h
 *
 * @brief hal psram master Module
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

#ifndef __WM_HAL_PSRAM_H__
#define __WM_HAL_PSRAM_H__

#include "wm_types.h"
#include "wm_soc_cfgs.h"
#include "wm_ll_psram.h"
#include "wm_hal_dma.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @enum wm_hal_psram_mode_t
 * @brief PSRAM mode
 */
typedef enum {
    WM_HAL_PSRAM_MODE_SPI = 0, /**< PSRAM WITH SPI          */
    WM_HAL_PSRAM_MODE_QSPI,    /**< PSRAM WITH QSPI         */
    WM_HAL_PSRAM_MODE_MAX,
} wm_hal_psram_mode_t;

/**
 * @enum wm_hal_psram_cmd_t
 * @brief PSRAM ioctrl command type
 */
typedef enum {
    WM_HAL_PSRAM_CMD_SET_MODE = 0, /**< SET PSRAM as SPI or QSPI      */
    WM_HAL_PSRAM_CMD_ENABLE_HSM,   /**< Enable PSRAM half sleep mode  */
    WM_HAL_PSRAM_CMD_DISABLE_HSM,  /**< Disable PSRAM half sleep mode */
    WM_HAL_PSRAM_CMD_MAX,
} wm_hal_psram_cmd_t;

/**
 * @struct wm_hal_psram_dma_t
 * @brief dma information
 */
typedef struct {
    wm_hal_dma_dev_t *dma_dev;
    uint8_t dma_ch;
} wm_hal_psram_dma_t;

/**
 * @struct wm_hal_psram_dev_t
 * @brief Hal psram device type
 */
typedef struct {
    wm_psram_reg_t *reg; /**< register base address                         */
    uint32_t clock_hz;   /**< pSRAM clock set                               */
    uint32_t ref_clock;  /**< reference clock,default CPU ,unit Hz          */
    uint8_t quad;        /**< 0:spi, 1: qspi                                */
    uint8_t status;      /**< internal status,set to 0 at initialization    */
    void *callback;      /**< callback ,called when DMA transfer done       */
    void *user_data;     /**< user data, pass to user when callback calling */
    uint32_t size;       /**< the whole psram size                          */
} wm_hal_psram_dev_t;

/**
 * @typedef wm_hal_psram_callback_t
 * @brief hal psram callback type,use for dma copy done
 *
 * @param device device type, @ref wm_hal_psram_dev_t
 * @param user_data user data from wm_hal_psram_register_callback
 */
typedef void (*wm_hal_psram_callback_t)(wm_hal_psram_dev_t *device, void *user_data);

/**
  * @brief  hal psram initialize
  *
  * @param[in] dev  The psram device struct, @ref wm_hal_psram_dev_t
  * @return
  *    - WM_ERR_SUCCESS : initialize OK
  *    - WM_ERR_FAILED : initialize FAIL
  *    - WM_ERR_INVALID_PARAM: bad param
  */
int wm_hal_psram_init(wm_hal_psram_dev_t *dev);

/**
  * @brief  hal psram deinitialize
  *
  * @param[in] dev  The psram device struct, @ref wm_hal_psram_dev_t
  * @return
  *    - WM_ERR_SUCCESS : deinitialize OK
  *    - WM_ERR_INVALID_PARAM: bad param
  */
int wm_hal_psram_deinit(wm_hal_psram_dev_t *dev);

/**
  * @brief  register call back
  *
  * @param[in] dev  The psram device struct, @ref wm_hal_psram_dev_t
  * @param[in] callback  The psram device struct, wm_hal_psram_callback_t
  * @param[in] user_data  userdata , Pass it to the user from the callback function
  * @return
  *    - WM_ERR_SUCCESS : OK
  *    - WM_ERR_INVALID_PARAM: bad param
  */
int wm_hal_psram_register_callback(wm_hal_psram_dev_t *dev, wm_hal_psram_callback_t callback, void *user_data);

/**
  * @brief  register call back
  *
  * @param[in] dev  The psram device struct, @ref wm_hal_psram_dev_t
  * @param[in] cmd  The command type
  * @param[in] param The param for the command
  * @param[in] arg The argument for the command
  * @return
  *    - WM_ERR_SUCCESS : OK
  *    - WM_ERR_NO_SUPPORT : not support command
  *    - WM_ERR_INVALID_PARAM: bad param
  */
int wm_hal_psram_ioctrl(wm_hal_psram_dev_t *dev, int cmd, uint32_t param, uint32_t *arg);

/**
  * @brief  psram get size
  *
  * @param[in] dev  The psram controler device, @arg wm_hal_psram_dev_t
  * @param[out] size  The whole psram size.
  * @return
  *    - WM_ERR_SUCCESS : read ok
  *    - WM_ERR_INVALID_PARAM: bad param
  * @warning auto dectect the psram size, support 1M,2M,4M,8M, the max size
  * is set detect size is defined by WM_HAL_PSRAM_MAX_DETECT_SIZE
  */
int wm_hal_psram_get_size(wm_hal_psram_dev_t *dev, uint32_t *size);

/**
  * @brief  memcpy with DMA or CPU
  *
  * @param[in] dev  The psram device struct, @ref wm_hal_psram_dev_t
  * @param[in] dst  The command type, destination address
  * @param[in] src  param , source address
  * @param[in] num  arg , copy size
  * @param[in] config  DMA configuration , @ref wm_hal_psram_dma_t
  * @return
  *    - WM_ERR_SUCCESS : OK
  *    - WM_ERR_NO_SUPPORT : not support command
  *    - WM_ERR_INVALID_PARAM: bad param
  */
int wm_hal_psram_memcpy(wm_hal_psram_dev_t *dev, void *dst, const void *src, size_t num, wm_hal_psram_dma_t *config);

#ifdef __cplusplus
}
#endif

#endif

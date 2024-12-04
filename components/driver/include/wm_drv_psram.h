/**
 * @file wm_drv_psram.h
 *
 * @brief driver psram master Module
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

#ifndef __WM_DRV_PSRAM_H__
#define __WM_DRV_PSRAM_H__

#include "wm_types.h"
#include "wm_error.h"
#include "wm_attr.h"
#include "wm_soc_cfgs.h"
#include "wm_dt.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_PSRAM_Enums WM PSRAM Enums
 * @brief WinnerMicro PSRAM Enums
 */

/**
 * @addtogroup WM_PSRAM_Enums
 * @{
 */

/**
 * @enum wm_drv_psram_mode_t
 * @brief PSRAM mode
 */
typedef enum {
    WM_DRV_PSRAM_MODE_SPI = 0, /**< PSRAM WITH SPI          */
    WM_DRV_PSRAM_MODE_QSPI,    /**< PSRAM WITH QSPI         */
    WM_DRV_PSRAM_MODE_MAX,
} wm_drv_psram_mode_t;

/**
 * @enum wm_drv_psram_cmd_t
 * @brief PSRAM ioctrl command type
 */
typedef enum {
    WM_DRV_PSRAM_CMD_SET_MODE = 0, /**< SET PSRAM as SPI or QSPI      */
    WM_DRV_PSRAM_CMD_ENABLE_HSM,   /**< Enable PSRAM half sleep mode  */
    WM_DRV_PSRAM_CMD_DISABLE_HSM,  /**< Disable PSRAM half sleep mode */
    WM_DRV_PSRAM_CMD_MAX,
} wm_drv_psram_cmd_t;
/**
 * @}
 */

/**
 * @typedef wm_drv_psram_callback_t
 * @brief drv psram callback type,use for dma copy done
 *
 * @param device device type, @ref wm_device_t
 * @param user_data user data from wm_hal_psram_register_callback
 */
typedef void (*wm_drv_psram_callback_t)(wm_device_t *device, void *user_data);

/**
 * @defgroup WM_DRV_PSRAM_APIs WM DRV PSRAM TYPEs
 * @brief WinnerMicro DRV PSRAM TYPEs
 */

/**
 * @addtogroup WM_DRV_PSRAM_APIs
 * @{
 */

/**
  * @brief  psram drvier initialize
  *
  * @param[in] device_name  The psram controler name, defined in device table
  * @return
  *    - !0 : psram controler handle, return previous handle if called twice
  *    - NULL: fail
  */
wm_device_t *wm_drv_psram_init(const char *device_name);

/**
  * @brief  psram drvier deinitialize
  *
  * @param[in] device  The psram controler device, @ref wm_device_t*
  * @return
  *    - WM_ERR_SUCCESS : deinitialize ok
  *    - WM_ERR_INVALID_PARAM: bad param
  */
int wm_drv_psram_deinit(wm_device_t *device);

/**
  * @brief  register a callback function for the PSRAM device
  *
  * @param[in] device  The psram device struct, @ref wm_device_t*
  * @param[in] callback  The callback function to register
  * @param[in] user_data The user data
  * 
  * @return
  *    - WM_ERR_SUCCESS : OK
  *    - WM_ERR_FAILED : FAILED
  *    - WM_ERR_INVALID_PARAM: bad param
  */

int wm_drv_psram_register_callback(wm_device_t *device, wm_drv_psram_callback_t callback, void *user_data);

/**
  * @brief  ioctrl
  *
  * @param[in] device  The psram device struct, @ref wm_device_t*
  * @param[in] cmd  The command type
  * @param[in] param  The param for the command
  * @param[in] arg  The argument for the command
  * @return
  *    - WM_ERR_SUCCESS : OK
  *    - WM_ERR_NO_SUPPORT : not support command
  *    - WM_ERR_INVALID_PARAM: bad param
  * @note cmd use as follow
  *   [cmd = WM_DRV_PSRAM_CMD_SET_MODE, param = WM_DRV_PSRAM_MODE_SPI]: SPI mode
  *   [cmd = WM_DRV_PSRAM_CMD_SET_MODE, param = WM_DRV_PSRAM_MODE_QSPI]: QSPI mode
  *   [cmd = WM_DRV_PSRAM_CMD_ENABLE_HSM ]: enable half sleep mode
  *   [cmd = WM_DRV_PSRAM_CMD_DISABLE_HSM ]: disable half sleep mode
  */
int wm_drv_psram_ioctrl(wm_device_t *device, int cmd, uint32_t param, uint32_t *arg);

/**
  * @brief  psram drvier write api
  *
  * @param[in] device  The psram device struct, @ref wm_device_t*
  * @param[out] size  The whole psram size, Use the size configured in device table
  * @return
  *    - WM_ERR_SUCCESS : OK
  *    - WM_ERR_INVALID_PARAM: bad param
  */
int wm_drv_psram_get_size(wm_device_t *device, uint32_t *size);

/**
  * @brief  psram memcpy by DMA
  *
  * @param[in] device  The psram controler device, @ref wm_device_t*
  * @param[in] dst  The Destination address
  * @param[out] src  The source address
  * @param[in] size  The data size
  * @return
  *    - WM_ERR_SUCCESS : OK
  *    - WM_ERR_FAILED : FAILED
  *    - WM_ERR_BUSY: The previous copy has not been completed
  *    - WM_ERR_INVALID_PARAM: bad param
  *
  * @note Calling this interface does not necessarily use DMA copy.
  * There are three conditions for using DMA
  *   (1) Src, dst address 4-byte alignment
  *   (2) The copy length is not less than PSRAM_MIN_DMA_SIZE,default is 1024
  *   (3) The device table configures the psram device to use DMA. When API calls,
  *       an idle DMA transmission channel is applied
  *   CPU copy:
  *         step1: memcpy
  *         step2: calling callback nodify copy done
  *         step3: return
  *   DMA copy:
  *         step1: start DMA
  *         step2: return
  *         step3: calling callback notify DMA copy done
  *
  * @warning DMA and CPU access PSRAM through SPI pins, so they cannot access PSRAM at the same time.
  * The interface will return WM_ERR_BUSY if the last access is not over.
  */
int wm_drv_psram_memcpy_dma(wm_device_t *device, void *dst, const void *src, uint32_t size);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_DRV_PSRAM_H__ */

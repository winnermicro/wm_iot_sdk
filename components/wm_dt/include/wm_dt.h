/**
 * @file wm_dt.h
 *
 * @brief Device Table Module
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

#ifndef __WM_DT_H__
#define __WM_DT_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WM_DT_Enumerations WM DT Enumerations
 * @brief WinnerMicro DT Enumerations
 */

/**
 * @addtogroup WM_DT_Enumerations
 * @{
 */

/**
 * @brief device status
 */
typedef enum device_status_enumeration {
    WM_DEV_ST_UNINIT = 0x0,
    WM_DEV_ST_INITING,
    WM_DEV_ST_INITED,
    WM_DEV_ST_UNKNOWN
} wm_device_status_t;

/**
 * @}
 */

/**
 * @defgroup WM_DT_Structures WM DT Structures
 * @brief WinnerMicro DT Structures
 */

/**
 * @addtogroup WM_DT_Structures
 * @{
 */

/**
 * @brief device structure
 *
 * Structure of the device entity
 */
typedef struct device_structure {
    char *name; /**< device name */

    void *hw; /**< hardware info, ref wm_dt_hw_xxx_t */

    void *ops; /**< device operation interface */
    void *drv; /**< driver context data */

    wm_device_status_t state; /**< device state */

    void *priv; /**< user private data */
} wm_device_t;

/**
 * @}
 */

/**
 * @defgroup WM_DT_Type_Definitions WM DT Type Definitions
 * @brief WinnerMicro DT Type Definitions
 */

/**
 * @defgroup WM_DT_Functions WM DT Functions
 * @brief WinnerMicro DT Functions
 */

/**
 * @addtogroup WM_DT_Functions
 * @{
 */

/**
 * @brief     Get the device
 *
 * @param[in] device_name  device name
 *
 * @return
 *    - NULL  : failed
 *    - others: succeed
 */
wm_device_t *wm_dt_get_device_by_name(const char *device_name);

/**
 * @brief     Set the device table name
 *
 * @param[in] default_name  device table name
 *
 * @return
 *    - WM_ERR_SUCCESS : succeed
 *    - others         : failed
 *
 * @warning It must be invoked before any use of wm_dt_get_device_by_name,
 *          and will fail if invoked after wm_dt_get_device_by_name.
 */
int wm_dt_set_device_table_name(const char *default_name);

/**
 * @brief     Free the device
 *
 * @param[in] device_name  device name
 *
 * @return
 *    - WM_ERR_SUCCESS : succeed
 *    - others         : failed
 */
int wm_dt_free_device_by_name(const char *device_name);

/**
 * @brief     Free the device
 *
 * @param[in] device  device pointer
 *
 * @return
 *    - WM_ERR_SUCCESS : succeed
 *    - others         : failed
 */
int wm_dt_free_device(wm_device_t *device);

/**
 * @brief     Free all devices
 *
 * @return
 *    - WM_ERR_SUCCESS : succeed
 *    - others         : failed
 */
int wm_dt_free_all_device(void);

/**
 * @brief     dump in the default serial
 *
 * @return
 *    - WM_ERR_SUCCESS : succeed
 *    - others         : failed
 */
int wm_dt_dump_all_device(void);

/**
 * @brief     auto init system device
 *
 * @return
 *    - WM_ERR_SUCCESS : succeed
 *    - others         : failed
 */
int wm_dt_auto_init_device(void);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __WM_DT_H__ */

/**
 * @file cis.h
 *
 * @brief CIS initialization
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

#ifndef __CIS_H__
#define __CIS_H__

#include "wm_drv_sdio_slave.h"

/**
 * @brief Initialize CIS registers
 *
 * @param[in] dev Pointer to the device structure of the SDIO slave.
 *
 * @return
 *    - WM_ERR_SUCCESS if the initialization succeeds.
 *    - WM_ERR_INVALID_PARAM if the device pointer is NULL.
 *    - Other error codes may be returned to reflect different failure conditions.
 */
int wm_sdio_slave_cis_init(wm_device_t *dev);

#endif

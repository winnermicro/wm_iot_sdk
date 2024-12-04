/**
 * @file cis.c
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

#include "cis.h"

#define LOG_TAG "example"
#include "wm_log.h"

/**
 * @brief Function 0 CIS register position enumeration
 */
typedef enum {
    WM_SDIO_SLAVE_F0_FUNCID_POS       = 0x00, /**< Function 0 FUNCID position */
    WM_SDIO_SLAVE_F0_FUNCE_POS        = 0x04, /**< Function 0 FUNCE position */
    WM_SDIO_SLAVE_F0_FUNCE_MAXBLK_POS = 0x08, /**< Function 0 FUNCE MAXBLK position */
    WM_SDIO_SLAVE_F0_MANFID_MID_POS   = 0x0C, /**< Function 0 MANFID MID position */
    WM_SDIO_SLAVE_F0_END_POS          = 0x10, /**< Function 0 END position */
    WM_SDIO_SLAVE_F0_MAX_POS
} wm_sdio_slave_f0_pos_t;

/**
 * @brief Function 1 CIS register position enumeration
 */
typedef enum {
    WM_SDIO_SLAVE_F1_FUNCID_POS         = 0x00, /**< Function 1 FUNCID position */
    WM_SDIO_SLAVE_F1_FUNCE_POS          = 0x04, /**< Function 1 FUNCE position */
    WM_SDIO_SLAVE_F1_FUNCE_VER_POS      = 0x08, /**< Function 1 FUNCE VER position */
    WM_SDIO_SLAVE_F1_FUNCE_NSN_POS      = 0x0C, /**< Function 1 FUNCE NSN position */
    WM_SDIO_SLAVE_F1_FUNCE_CSASIZE_POS  = 0x10, /**< Function 1 FUNCE CSASIZE position */
    WM_SDIO_SLAVE_F1_FUNCE_OCR_POS      = 0x14, /**< Function 1 FUNCE OCR position */
    WM_SDIO_SLAVE_F1_FUNCE_MINPWR_POS   = 0x18, /**< Function 1 FUNCE MINPWR position */
    WM_SDIO_SLAVE_F1_FUNCE_STANDBY_POS  = 0x1C, /**< Function 1 FUNCE STANDBY position */
    WM_SDIO_SLAVE_F1_FUNCE_OPTBW_POS    = 0x20, /**< Function 1 FUNCE OPTBW position */
    WM_SDIO_SLAVE_F1_FUNCE_NTIMEOUT_POS = 0x24, /**< Function 1 FUNCE NTIMEOUT position */
    WM_SDIO_SLAVE_F1_FUNCE_AVGPWR_POS   = 0x28, /**< Function 1 FUNCE AVGPWR position */
    WM_SDIO_SLAVE_F1_FUNCE_MAXPWR_POS   = 0x2C, /**< Function 1 FUNCE MAXPWR position */
    WM_SDIO_SLAVE_F1_END_POS            = 0x30, /**< Function 1 END position */
    WM_SDIO_SLAVE_F1_MAX_POS
} wm_sdio_slave_f1_pos_t;

/**
 * @brief Register configuration structure for SDIO slave device
 *
 * This structure defines the configuration parameters for a single SDIO slave register.
 * It contains the function number, register address and value to be configured.
 */
typedef struct {
    wm_hal_sdio_slave_func_t func; /**< SDIO slave function number (0 or 1) */
    uint32_t addr;                 /**< Register address offset */
    uint32_t value;                /**< Value to be written to the register */
} reg_config_t;

/**
 * @brief Default register configuration for SDIO slave device
 *
 * @note This configuration serves as a reference template. Users should adjust the settings according to their specific SDIO card specifications and requirements.
 */
static const reg_config_t fn_regs[] = {
    { WM_DRV_SDIO_SLAVE_FUNC_0, WM_SDIO_SLAVE_F0_FUNCID_POS,         0x000C0221 }, /* CISTPL_FUNCID tuple for SDIO card id */
    { WM_DRV_SDIO_SLAVE_FUNC_0, WM_SDIO_SLAVE_F0_FUNCE_POS,
     0x00000422                                                                 }, /* Function extension, supports 800 block size bytes */
    { WM_DRV_SDIO_SLAVE_FUNC_0, WM_SDIO_SLAVE_F0_FUNCE_MAXBLK_POS,   0x04203208 }, /* 25 Mb/Sec (0x32) */
    { WM_DRV_SDIO_SLAVE_FUNC_0, WM_SDIO_SLAVE_F0_MANFID_MID_POS,     0x53470296 }, /* Manufacturer device code, starts with 0x20 */
    { WM_DRV_SDIO_SLAVE_FUNC_0, WM_SDIO_SLAVE_F0_END_POS,            0xffffffff }, /* End of Function 0 */

    { WM_DRV_SDIO_SLAVE_FUNC_1, WM_SDIO_SLAVE_F1_FUNCID_POS,         0x000C0221 }, /* CISTPL_FUNCID tuple for SDIO card id */
    { WM_DRV_SDIO_SLAVE_FUNC_1, WM_SDIO_SLAVE_F1_FUNCE_POS,
     0x01012a22                                                                 }, /* Function extension, bit[31-24]=0x01: supports low power and CLK stop */
    { WM_DRV_SDIO_SLAVE_FUNC_1, WM_SDIO_SLAVE_F1_FUNCE_VER_POS,
     0x00000020                                                                 }, /* 0x20 indicates SDIO 2.0 support, 32-bit serial number */
    { WM_DRV_SDIO_SLAVE_FUNC_1, WM_SDIO_SLAVE_F1_FUNCE_NSN_POS,      0x00000000 }, /* Available CSA space 32-bit, 0x00000000 bytes */
    { WM_DRV_SDIO_SLAVE_FUNC_1, WM_SDIO_SLAVE_F1_FUNCE_CSASIZE_POS,
     0x08000300                                                                 }, /* bit[15-8]=0x03: CSA read-only, 800 block size */
    { WM_DRV_SDIO_SLAVE_FUNC_1, WM_SDIO_SLAVE_F1_FUNCE_OCR_POS,      0x00FF8000 }, /* Operating Condition Register */
    { WM_DRV_SDIO_SLAVE_FUNC_1, WM_SDIO_SLAVE_F1_FUNCE_MINPWR_POS,
     0x010f0a08                                                                 }, /* Power: 8mA nominal, 10mA avg, 16mA max, 1mA min */
    { WM_DRV_SDIO_SLAVE_FUNC_1, WM_SDIO_SLAVE_F1_FUNCE_STANDBY_POS,  0x00000101 }, /* Standby power settings */
    { WM_DRV_SDIO_SLAVE_FUNC_1, WM_SDIO_SLAVE_F1_FUNCE_OPTBW_POS,    0x00000000 }, /* Optional bus width */
    { WM_DRV_SDIO_SLAVE_FUNC_1, WM_SDIO_SLAVE_F1_FUNCE_NTIMEOUT_POS, 0x00000000 }, /* Timeout value */
    { WM_DRV_SDIO_SLAVE_FUNC_1, WM_SDIO_SLAVE_F1_FUNCE_AVGPWR_POS,   0x00000000 }, /* Average power */
    { WM_DRV_SDIO_SLAVE_FUNC_1, WM_SDIO_SLAVE_F1_FUNCE_MAXPWR_POS,   0x00000000 }, /* Maximum power */
    { WM_DRV_SDIO_SLAVE_FUNC_1, WM_SDIO_SLAVE_F1_END_POS,            0x000000ff }  /* End of Function 1 */
};

int wm_sdio_slave_cis_init(wm_device_t *dev)
{
    int ret = WM_ERR_SUCCESS;

    if (dev == NULL) {
        wm_log_error("Invalid device pointer");
        return WM_ERR_INVALID_PARAM;
    }

    for (int i = 0; i < sizeof(fn_regs) / sizeof(fn_regs[0]); i++) {
        ret = wm_drv_sdio_slave_write_cis_reg(dev, fn_regs[i].func, fn_regs[i].addr, fn_regs[i].value);
        if (ret != WM_ERR_SUCCESS) {
            wm_log_error("Failed to write register func=%d addr=0x%x", fn_regs[i].func, fn_regs[i].addr);
            return ret;
        }
    }

    return ret;
}

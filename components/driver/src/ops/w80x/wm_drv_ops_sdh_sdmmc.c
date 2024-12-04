/**
 * @file wm_drv_ops_sdh_sdmmc.c
 *
 * @brief DRV_OPS_SDH_SDMMC Module
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

#include "wm_drv_sdh_sdmmc.h"
#include "wm_drv_rcc.h"
#include "wm_dt_hw.h"
#include "wm_drv_gpio.h"

/**
 * @brief Enable debug logs for SDH_SDMMC driver
 */
#define WM_DRV_SDH_SDMMC_DEBUG 0
#if WM_DRV_SDH_SDMMC_DEBUG
#define LOG_TAG "DRV SDMMC"
#include "wm_log.h"
#define WM_DRV_SDH_SDMMC_LOG_D         wm_log_debug
#define WM_DRV_SDH_SDMMC_LOG_I         wm_log_info
#define WM_DRV_SDH_SDMMC_LOG_W         wm_log_warn
#define WM_DRV_SDH_SDMMC_LOG_E         wm_log_error
#define WM_DRV_SDH_SDMMC_LOG_DUMP      wm_log_dump
#define WM_DRV_SDH_SDMMC_FUNC_EXIT(fn) WM_DRV_SDH_SDMMC_LOG_D("%s exit", fn)
#define WM_DRV_SDH_SDMMC_FUNC_NAME     __func__
#else
#define WM_DRV_SDH_SDMMC_LOG_D(...)
#define WM_DRV_SDH_SDMMC_LOG_I(...)
#define WM_DRV_SDH_SDMMC_LOG_W(...)
#define WM_DRV_SDH_SDMMC_LOG_E(...)
#define WM_DRV_SDH_SDMMC_LOG_DUMP(...)
#define WM_DRV_SDH_SDMMC_FUNC_EXIT(fn)
#endif

/**
 * @brief Lock the SDH_SDMMC driver for thread-safe operations
 */
#define WM_DRV_SDH_SDMMC_LOCK(lock)                                                      \
    do {                                                                                 \
        if (!lock) {                                                                     \
            if (WM_ERR_SUCCESS != wm_os_internal_mutex_create(&lock)) {                  \
                WM_DRV_SDH_SDMMC_LOG_E("mutex create err!");                             \
                return WM_ERR_FAILED;                                                    \
            }                                                                            \
        }                                                                                \
        if (WM_ERR_SUCCESS != wm_os_internal_mutex_acquire(lock, WM_OS_WAIT_TIME_MAX)) { \
            WM_DRV_SDH_SDMMC_LOG_E("mutex acquire err!");                                \
            return WM_ERR_FAILED;                                                        \
        }                                                                                \
    } while (0);

/**
 * @brief Unlock the SDH_SDMMC driver after thread-safe operations
 */
#define WM_DRV_SDH_SDMMC_UNLOCK(lock)       \
    do {                                    \
        if (!lock) {                        \
            return WM_ERR_NO_INITED;        \
        }                                   \
        wm_os_internal_mutex_release(lock); \
    } while (0);

/**
 * @brief Timeout definitions for various SDMMC operations
 */
#define WM_DRV_OPS_SDH_SDMMC_CMD_TIMEOUT   (10)
#define WM_DRV_OPS_SDH_SDMMC_BLOCK_TIMEOUT (300)
#define WM_DRV_OPS_SDH_SDMMC_READY_TIMEOUT (300)

static int wm_drv_ops_sdh_sdmmc_init(wm_device_t *dev);
static int wm_drv_ops_sdh_sdmmc_deinit(wm_device_t *dev);
static int wm_drv_ops_sdh_sdmmc_read_blocks(wm_device_t *dev, uint8_t *buf, uint32_t start_block, uint32_t num_blocks);
static int wm_drv_ops_sdh_sdmmc_write_blocks(wm_device_t *dev, const uint8_t *buf, uint32_t start_block, uint32_t num_blocks);
static int wm_drv_ops_sdh_sdmmc_get_card_info(wm_device_t *dev, wm_drv_sdh_sdmmc_card_info_t *card_info);

/**
 * @brief SDH_SDMMC driver operations structure
 */
typedef struct {
    int (*init)(wm_device_t *dev);   /**< Pointer to the initialization function */
    int (*deinit)(wm_device_t *dev); /**< Pointer to the deinitialization function */
    int (*read_blocks)(wm_device_t *dev, uint8_t *buf, uint32_t start_block,
                       uint32_t num_blocks); /**< Pointer to the read blocks function */
    int (*write_blocks)(wm_device_t *dev, const uint8_t *buf, uint32_t start_block,
                        uint32_t num_blocks); /**< Pointer to the write blocks function */
    int (*get_card_info)(wm_device_t *dev,
                         wm_drv_sdh_sdmmc_card_info_t *card_info); /**< Pointer to the get card info function */
} wm_drv_sdh_sdmmc_ops_t;

/**
 * @brief Structure representing the device context for SDH_SDMMC driver
 */
typedef struct {
    wm_os_mutex_t *mutex;                   /**< Mutex for thread-safe access */
    wm_device_t *clock_dev;                 /**< Pointer to a clock device */
    wm_device_t *dma_dev;                   /**< Pointer to a dma device */
    wm_drv_sdh_sdmmc_card_info_t card_info; /**< Card information */
    wm_hal_sdh_evt_t evt;                   /**< Event type */
    wm_os_sem_t *evt_sem;                   /**< mutex for event  */
} wm_drv_sdh_sdmmc_dev_ctx_t;

/**
 * @brief SDH_SDMMC driver data structure
 */
typedef struct {
    wm_hal_sdh_dev_t hal_sdh_dev;       /**< HAL device structure */
    wm_drv_sdh_sdmmc_dev_ctx_t drv_ctx; /**< Driver context for the SDH_SDMMC driver */
} wm_drv_sdh_sdmmc_drv_t;

static int wm_drv_sdh_sdmmc_pin_cfg(wm_device_t *dev)
{
    int ret                      = WM_ERR_SUCCESS;
    wm_dt_hw_sdh_t *hw_sdh_sdmmc = NULL;

    hw_sdh_sdmmc = (wm_dt_hw_sdh_t *)dev->hw;

    for (uint8_t i = 0; i < hw_sdh_sdmmc->pin_cfg_count; i++) {
        ret = wm_drv_gpio_iomux_func_sel(hw_sdh_sdmmc->pin_cfg[i].pin_num, hw_sdh_sdmmc->pin_cfg[i].pin_mux);
        if (ret != WM_ERR_SUCCESS) {
            return ret;
        }
    }

    return ret;
}

static int wm_drv_sdh_sdmmc_card_init(wm_device_t *dev)
{
    int ret                          = WM_ERR_SUCCESS;
    wm_drv_sdh_sdmmc_drv_t *drv_data = NULL;
    wm_hal_sdh_xfer_desc_t xfer      = { 0 };
    uint8_t i                        = 0;
    uint8_t retry_count              = 24;
    uint16_t delay_ms                = 0;

    if (!dev->drv) {
        return WM_ERR_INVALID_PARAM;
    }

    drv_data = (wm_drv_sdh_sdmmc_drv_t *)dev->drv;

    /*
     * CMD 0  Reset Card
     * CMD 8  Get voltage (Only 2.0 Card response to this)
     * CMD 55  Indicate Next Command are Application specific
     * ACMD 41 Get Voltage windows
     * CMD 2  CID reg
     * CMD 3  Get RCA
     */
    for (i = 0; i < retry_count; i++) {
        /* CMD0 */
        memset(&xfer, 0, sizeof(wm_hal_sdh_xfer_desc_t));
        xfer.port_mode                          = WM_HAL_SDH_PORT_MODE_MMC;
        xfer.sdio_xfer_desc.sd_cmd.cmd_id       = WM_HAL_SDH_CMD_NUM_0;
        xfer.sdio_xfer_desc.sd_cmd.cmd_rsp_type = WM_HAL_SDH_CMD_NO_RSP;
        xfer.sdio_xfer_desc.sd_cmd.cmd_timeout  = WM_DRV_OPS_SDH_SDMMC_CMD_TIMEOUT;
        ret                                     = wm_hal_sdh_xfer_polling(&drv_data->hal_sdh_dev, &xfer);
        if (ret != WM_ERR_SUCCESS) {
            WM_DRV_SDH_SDMMC_LOG_E("CMD0 err");
            continue;
        }

        /* CMD8 */
        memset(&xfer, 0, sizeof(wm_hal_sdh_xfer_desc_t));
        xfer.port_mode                          = WM_HAL_SDH_PORT_MODE_MMC;
        xfer.sdio_xfer_desc.sd_cmd.cmd_id       = WM_HAL_SDH_CMD_NUM_8;
        xfer.sdio_xfer_desc.sd_cmd.cmd_arg      = 0x1AA;
        xfer.sdio_xfer_desc.sd_cmd.cmd_rsp_type = WM_HAL_SDH_CMD_WAIT_RSP;
        xfer.sdio_xfer_desc.sd_cmd.cmd_rsp_len  = 2;
        xfer.sdio_xfer_desc.sd_cmd.cmd_timeout  = WM_DRV_OPS_SDH_SDMMC_CMD_TIMEOUT;
        ret                                     = wm_hal_sdh_xfer_polling(&drv_data->hal_sdh_dev, &xfer);
        if (ret != WM_ERR_SUCCESS) {
            WM_DRV_SDH_SDMMC_LOG_E("CMD8 err");
            continue;
        }
        WM_DRV_SDH_SDMMC_LOG_DUMP(WM_LOG_LEVEL_INFO, "CMD8 resp: ", 4, xfer.sdio_xfer_desc.sd_cmd.cmd_rsp,
                                  xfer.sdio_xfer_desc.sd_cmd.cmd_rsp_len << 2);
        if ((xfer.sdio_xfer_desc.sd_cmd.cmd_rsp[0] == 0x1AA) &&
            ((xfer.sdio_xfer_desc.sd_cmd.cmd_rsp[1] & 0xFF) == xfer.sdio_xfer_desc.sd_cmd.cmd_id)) {
            break;
        }
    }
    if (i >= retry_count) {
        drv_data->drv_ctx.card_info.type = WM_DRV_SDH_SDMMC_CARD_TYPE_SD;
    }

    for (i = 0; i < retry_count; i++) {
        /* CMD55 */
        memset(&xfer, 0, sizeof(wm_hal_sdh_xfer_desc_t));
        xfer.port_mode                          = WM_HAL_SDH_PORT_MODE_MMC;
        xfer.sdio_xfer_desc.sd_cmd.cmd_id       = WM_HAL_SDH_CMD_NUM_55;
        xfer.sdio_xfer_desc.sd_cmd.cmd_arg      = 0;
        xfer.sdio_xfer_desc.sd_cmd.cmd_rsp_type = WM_HAL_SDH_CMD_WAIT_RSP;
        xfer.sdio_xfer_desc.sd_cmd.cmd_rsp_len  = 2;
        xfer.sdio_xfer_desc.sd_cmd.cmd_timeout  = WM_DRV_OPS_SDH_SDMMC_CMD_TIMEOUT;
        ret                                     = wm_hal_sdh_xfer_polling(&drv_data->hal_sdh_dev, &xfer);
        if (ret != WM_ERR_SUCCESS) {
            WM_DRV_SDH_SDMMC_LOG_E("CMD55 err");
            continue;
        }
        WM_DRV_SDH_SDMMC_LOG_DUMP(WM_LOG_LEVEL_INFO, "CMD55 resp: ", 4, xfer.sdio_xfer_desc.sd_cmd.cmd_rsp,
                                  xfer.sdio_xfer_desc.sd_cmd.cmd_rsp_len << 2);
        if (xfer.sdio_xfer_desc.sd_cmd.cmd_rsp[1] != xfer.sdio_xfer_desc.sd_cmd.cmd_id) {
            WM_DRV_SDH_SDMMC_LOG_E("CMD55 resp err");
            continue;
        }

        /* ACMD41 */
        memset(&xfer, 0, sizeof(wm_hal_sdh_xfer_desc_t));
        xfer.port_mode                          = WM_HAL_SDH_PORT_MODE_MMC;
        xfer.sdio_xfer_desc.sd_cmd.cmd_id       = WM_HAL_SDH_CMD_NUM_41;
        xfer.sdio_xfer_desc.sd_cmd.cmd_arg      = 0xC0100000;
        xfer.sdio_xfer_desc.sd_cmd.cmd_rsp_type = WM_HAL_SDH_CMD_WAIT_RSP;
        xfer.sdio_xfer_desc.sd_cmd.cmd_rsp_len  = 2;
        xfer.sdio_xfer_desc.sd_cmd.cmd_timeout  = WM_DRV_OPS_SDH_SDMMC_CMD_TIMEOUT;
        ret                                     = wm_hal_sdh_xfer_polling(&drv_data->hal_sdh_dev, &xfer);
        if (ret != WM_ERR_SUCCESS) {
            WM_DRV_SDH_SDMMC_LOG_E("ACMD41 err");
            continue;
        }
        WM_DRV_SDH_SDMMC_LOG_DUMP(WM_LOG_LEVEL_INFO, "ACMD41 resp: ", 4, xfer.sdio_xfer_desc.sd_cmd.cmd_rsp,
                                  xfer.sdio_xfer_desc.sd_cmd.cmd_rsp_len << 2);
        if ((xfer.sdio_xfer_desc.sd_cmd.cmd_rsp[1] & 0xFF) != 0x3F) {
            WM_DRV_SDH_SDMMC_LOG_E("ACMD41 resp1 err");
            continue;
        }
        if ((xfer.sdio_xfer_desc.sd_cmd.cmd_rsp[0] >> 31) & 0x01) {
            if (drv_data->drv_ctx.card_info.type == 0) {
                if ((xfer.sdio_xfer_desc.sd_cmd.cmd_rsp[0] >> 30) & 0x02) {
                    drv_data->drv_ctx.card_info.type = WM_DRV_SDH_SDMMC_CARD_TYPE_SDSC;
                } else if ((xfer.sdio_xfer_desc.sd_cmd.cmd_rsp[0] >> 30) & 0x03) {
                    drv_data->drv_ctx.card_info.type = WM_DRV_SDH_SDMMC_CARD_TYPE_SDHC;
                }
            }
            WM_DRV_SDH_SDMMC_LOG_D("Card is ready, type:%d", drv_data->drv_ctx.card_info.type);
            break;
        } else {
            WM_DRV_SDH_SDMMC_LOG_I("Card is not ready");
            delay_ms = 1 << i;
            if (delay_ms >= 256) {
                delay_ms = 256;
            }
            wm_os_internal_time_delay_ms(delay_ms);
        }
    }
    if (i >= retry_count) {
        if (ret != WM_ERR_SUCCESS) {
            return ret;
        } else {
            return WM_ERR_FAILED;
        }
    }

    /* CMD2 */
    memset(&xfer, 0, sizeof(wm_hal_sdh_xfer_desc_t));
    xfer.port_mode                          = WM_HAL_SDH_PORT_MODE_MMC;
    xfer.sdio_xfer_desc.sd_cmd.cmd_id       = WM_HAL_SDH_CMD_NUM_2;
    xfer.sdio_xfer_desc.sd_cmd.cmd_arg      = 0;
    xfer.sdio_xfer_desc.sd_cmd.cmd_rsp_type = WM_HAL_SDH_CMD_WAIT_RSP;
    xfer.sdio_xfer_desc.sd_cmd.cmd_rsp_len  = 4;
    xfer.sdio_xfer_desc.sd_cmd.cmd_timeout  = WM_DRV_OPS_SDH_SDMMC_CMD_TIMEOUT;
    ret                                     = wm_hal_sdh_xfer_polling(&drv_data->hal_sdh_dev, &xfer);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_SDH_SDMMC_LOG_E("CMD2 err");
        return ret;
    }
    WM_DRV_SDH_SDMMC_LOG_DUMP(WM_LOG_LEVEL_INFO, "CMD2 resp: ", 4, xfer.sdio_xfer_desc.sd_cmd.cmd_rsp,
                              xfer.sdio_xfer_desc.sd_cmd.cmd_rsp_len << 2);
    if ((xfer.sdio_xfer_desc.sd_cmd.cmd_rsp[3] >> 24) != 0x3F) {
        WM_DRV_SDH_SDMMC_LOG_E("CMD2 resp err");
        return WM_ERR_FAILED;
    }

    /* CMD3 */
    memset(&xfer, 0, sizeof(wm_hal_sdh_xfer_desc_t));
    xfer.port_mode                          = WM_HAL_SDH_PORT_MODE_MMC;
    xfer.sdio_xfer_desc.sd_cmd.cmd_id       = WM_HAL_SDH_CMD_NUM_3;
    xfer.sdio_xfer_desc.sd_cmd.cmd_arg      = 0;
    xfer.sdio_xfer_desc.sd_cmd.cmd_rsp_type = WM_HAL_SDH_CMD_WAIT_RSP;
    xfer.sdio_xfer_desc.sd_cmd.cmd_rsp_len  = 2;
    xfer.sdio_xfer_desc.sd_cmd.cmd_timeout  = WM_DRV_OPS_SDH_SDMMC_CMD_TIMEOUT;
    ret                                     = wm_hal_sdh_xfer_polling(&drv_data->hal_sdh_dev, &xfer);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_SDH_SDMMC_LOG_E("CMD3 err");
        return ret;
    }
    WM_DRV_SDH_SDMMC_LOG_DUMP(WM_LOG_LEVEL_INFO, "CMD3 resp: ", 4, xfer.sdio_xfer_desc.sd_cmd.cmd_rsp,
                              xfer.sdio_xfer_desc.sd_cmd.cmd_rsp_len << 2);
    if ((xfer.sdio_xfer_desc.sd_cmd.cmd_rsp[1] & 0xFF) != xfer.sdio_xfer_desc.sd_cmd.cmd_id) {
        WM_DRV_SDH_SDMMC_LOG_E("CMD3 resp err");
        return WM_ERR_FAILED;
    }
    drv_data->drv_ctx.card_info.rca = xfer.sdio_xfer_desc.sd_cmd.cmd_rsp[0] >> 16;
    WM_DRV_SDH_SDMMC_LOG_D("RCA = 0x%X", drv_data->drv_ctx.card_info.rca);

    return ret;
}

static int wm_drv_sdh_sdmmc_card_get_csd(wm_device_t *dev)
{
    int ret                          = WM_ERR_SUCCESS;
    wm_drv_sdh_sdmmc_drv_t *drv_data = NULL;
    wm_hal_sdh_xfer_desc_t xfer      = { 0 };
    uint8_t adjust_resp[16]          = { 0 };
    uint32_t capacity                = 0;
    uint16_t n                       = 0;
    uint32_t csize                   = 0;

    if (!dev->drv) {
        return WM_ERR_INVALID_PARAM;
    }

    drv_data = (wm_drv_sdh_sdmmc_drv_t *)dev->drv;

    /*
     * CMD 9 SEND_CSD
     */
    /* CMD9 */
    memset(&xfer, 0, sizeof(wm_hal_sdh_xfer_desc_t));
    xfer.port_mode                          = WM_HAL_SDH_PORT_MODE_MMC;
    xfer.sdio_xfer_desc.sd_cmd.cmd_id       = WM_HAL_SDH_CMD_NUM_9;
    xfer.sdio_xfer_desc.sd_cmd.cmd_arg      = (drv_data->drv_ctx.card_info.rca << 16);
    xfer.sdio_xfer_desc.sd_cmd.cmd_rsp_type = WM_HAL_SDH_CMD_WAIT_RSP;
    xfer.sdio_xfer_desc.sd_cmd.cmd_rsp_len  = 4;
    xfer.sdio_xfer_desc.sd_cmd.cmd_timeout  = WM_DRV_OPS_SDH_SDMMC_CMD_TIMEOUT;
    ret                                     = wm_hal_sdh_xfer_polling(&drv_data->hal_sdh_dev, &xfer);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_SDH_SDMMC_LOG_E("CMD9 err");
        return ret;
    }
    WM_DRV_SDH_SDMMC_LOG_DUMP(WM_LOG_LEVEL_INFO, "CMD9 resp: ", 4, xfer.sdio_xfer_desc.sd_cmd.cmd_rsp,
                              xfer.sdio_xfer_desc.sd_cmd.cmd_rsp_len << 2);
    if ((xfer.sdio_xfer_desc.sd_cmd.cmd_rsp[3] >> 24) != 0x3F) {
        WM_DRV_SDH_SDMMC_LOG_E("CMD9 resp err");
        return WM_ERR_FAILED;
    }

    for (uint8_t i = 0; i < xfer.sdio_xfer_desc.sd_cmd.cmd_rsp_len; i++) {
        adjust_resp[(i << 2) + 0] = (xfer.sdio_xfer_desc.sd_cmd.cmd_rsp[3 - i] >> 24) & 0xFF;
        adjust_resp[(i << 2) + 1] = (xfer.sdio_xfer_desc.sd_cmd.cmd_rsp[3 - i] >> 16) & 0xFF;
        adjust_resp[(i << 2) + 2] = (xfer.sdio_xfer_desc.sd_cmd.cmd_rsp[3 - i] >> 8) & 0xFF;
        adjust_resp[(i << 2) + 3] = (xfer.sdio_xfer_desc.sd_cmd.cmd_rsp[3 - i] >> 0) & 0xFF;
    }

    if ((adjust_resp[1] & 0x40) == 0x40) {
        drv_data->drv_ctx.card_info.csd_ver = WM_DRV_SDH_SDMMC_CARD_CSD_VER_2;
        csize    = adjust_resp[10] + ((uint32_t)adjust_resp[9] << 8) + ((uint32_t)(adjust_resp[8] & 63) << 16) + 1;
        capacity = csize << 9;
        drv_data->drv_ctx.card_info.capacity   = (long long)capacity * 1024;
        drv_data->drv_ctx.card_info.block_size = WM_SDH_BLOCK_SIZE;
    } else {
        drv_data->drv_ctx.card_info.csd_ver = WM_DRV_SDH_SDMMC_CARD_CSD_VER_1;
        n        = (adjust_resp[6] & 0x0F) + ((adjust_resp[11] & 0x80) >> 7) + ((adjust_resp[10] & 0x03) << 1) + 2;
        csize    = (adjust_resp[9] >> 6) + ((uint16_t)adjust_resp[8] << 2) + ((uint16_t)(adjust_resp[7] & 0x03) << 10) + 1;
        capacity = (uint32_t)csize << (n - 10);
        drv_data->drv_ctx.card_info.capacity   = (long long)capacity * 1024;
        drv_data->drv_ctx.card_info.block_size = WM_SDH_BLOCK_SIZE;
    }

    WM_DRV_SDH_SDMMC_LOG_D("card_info.capacity: %lld", drv_data->drv_ctx.card_info.capacity);
    WM_DRV_SDH_SDMMC_LOG_D("card_info.block_size: %d", drv_data->drv_ctx.card_info.block_size);
    WM_DRV_SDH_SDMMC_LOG_D("card_info.csd_ver: %d", drv_data->drv_ctx.card_info.csd_ver);
    WM_DRV_SDH_SDMMC_LOG_D("card_info.type: %d", drv_data->drv_ctx.card_info.type);
    WM_DRV_SDH_SDMMC_LOG_D("card_info.rca: %08X", drv_data->drv_ctx.card_info.rca);

    return ret;
}

static int wm_drv_sdh_sdmmc_card_get_status(wm_device_t *dev, uint32_t *state)
{
    int ret                          = WM_ERR_SUCCESS;
    wm_drv_sdh_sdmmc_drv_t *drv_data = NULL;
    wm_hal_sdh_xfer_desc_t xfer      = { 0 };

    if (!dev->drv) {
        return WM_ERR_INVALID_PARAM;
    }

    drv_data = (wm_drv_sdh_sdmmc_drv_t *)dev->drv;

    /*
     * CMD 13 SEND_STATUS
     */
    /* CMD13 */
    memset(&xfer, 0, sizeof(wm_hal_sdh_xfer_desc_t));
    xfer.port_mode                          = WM_HAL_SDH_PORT_MODE_MMC;
    xfer.sdio_xfer_desc.sd_cmd.cmd_id       = WM_HAL_SDH_CMD_NUM_13;
    xfer.sdio_xfer_desc.sd_cmd.cmd_arg      = (drv_data->drv_ctx.card_info.rca << 16);
    xfer.sdio_xfer_desc.sd_cmd.cmd_rsp_type = WM_HAL_SDH_CMD_WAIT_RSP;
    xfer.sdio_xfer_desc.sd_cmd.cmd_rsp_len  = 4;
    xfer.sdio_xfer_desc.sd_cmd.cmd_timeout  = WM_DRV_OPS_SDH_SDMMC_CMD_TIMEOUT;
    ret                                     = wm_hal_sdh_xfer_polling(&drv_data->hal_sdh_dev, &xfer);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_SDH_SDMMC_LOG_E("CMD13 err");
        return ret;
    }
    WM_DRV_SDH_SDMMC_LOG_DUMP(WM_LOG_LEVEL_INFO, "CMD13 resp: ", 4, xfer.sdio_xfer_desc.sd_cmd.cmd_rsp,
                              xfer.sdio_xfer_desc.sd_cmd.cmd_rsp_len << 2);
    if ((xfer.sdio_xfer_desc.sd_cmd.cmd_rsp[1] & 0xFF) != xfer.sdio_xfer_desc.sd_cmd.cmd_id) {
        WM_DRV_SDH_SDMMC_LOG_E("CMD13 resp err");
        return WM_ERR_FAILED;
    }
    if (state) {
        *state = xfer.sdio_xfer_desc.sd_cmd.cmd_rsp[0];
    }

    WM_DRV_SDH_SDMMC_LOG_I("current_state %d, error_state %d\n", (xfer.sdio_xfer_desc.sd_cmd.cmd_rsp[0] >> 9) & 0xF,
                           (xfer.sdio_xfer_desc.sd_cmd.cmd_rsp[0] >> 19) & 0x1);

    return ret;
}

static int wm_drv_sdh_sdmmc_card_sel(wm_device_t *dev)
{
    int ret                          = WM_ERR_SUCCESS;
    wm_drv_sdh_sdmmc_drv_t *drv_data = NULL;
    wm_hal_sdh_xfer_desc_t xfer      = { 0 };

    if (!dev->drv) {
        return WM_ERR_INVALID_PARAM;
    }

    drv_data = (wm_drv_sdh_sdmmc_drv_t *)dev->drv;

    /*
     * CMD 7 Select/deselect card
     */
    /* CMD7 */
    memset(&xfer, 0, sizeof(wm_hal_sdh_xfer_desc_t));
    xfer.port_mode                          = WM_HAL_SDH_PORT_MODE_MMC;
    xfer.sdio_xfer_desc.sd_cmd.cmd_id       = WM_HAL_SDH_CMD_NUM_7;
    xfer.sdio_xfer_desc.sd_cmd.cmd_arg      = (drv_data->drv_ctx.card_info.rca << 16);
    xfer.sdio_xfer_desc.sd_cmd.cmd_rsp_type = WM_HAL_SDH_CMD_WAIT_RSP;
    xfer.sdio_xfer_desc.sd_cmd.cmd_rsp_len  = 2;
    xfer.sdio_xfer_desc.sd_cmd.cmd_timeout  = WM_DRV_OPS_SDH_SDMMC_CMD_TIMEOUT;
    ret                                     = wm_hal_sdh_xfer_polling(&drv_data->hal_sdh_dev, &xfer);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_SDH_SDMMC_LOG_E("CMD7 err");
        return ret;
    }
    WM_DRV_SDH_SDMMC_LOG_DUMP(WM_LOG_LEVEL_INFO, "CMD7 resp: ", 4, xfer.sdio_xfer_desc.sd_cmd.cmd_rsp,
                              xfer.sdio_xfer_desc.sd_cmd.cmd_rsp_len << 2);
    if ((xfer.sdio_xfer_desc.sd_cmd.cmd_rsp[1] & 0xFF) != xfer.sdio_xfer_desc.sd_cmd.cmd_id) {
        WM_DRV_SDH_SDMMC_LOG_E("CMD7 resp err");
        return WM_ERR_FAILED;
    }

    return ret;
}

static int wm_drv_sdh_sdmmc_card_set_bus_width(wm_device_t *dev)
{
    int ret                          = WM_ERR_SUCCESS;
    wm_dt_hw_sdh_t *hw_sdh_sdmmc     = NULL;
    wm_drv_sdh_sdmmc_drv_t *drv_data = NULL;
    wm_hal_sdh_xfer_desc_t xfer      = { 0 };

    if (!dev->drv) {
        return WM_ERR_INVALID_PARAM;
    }

    hw_sdh_sdmmc = (wm_dt_hw_sdh_t *)dev->hw;
    drv_data     = (wm_drv_sdh_sdmmc_drv_t *)dev->drv;

    /*
     * CMD 55  Indicate Next Command are Application specific
     * ACMD 6 Set the data bus width
     */
    /* CMD55 */
    memset(&xfer, 0, sizeof(wm_hal_sdh_xfer_desc_t));
    xfer.port_mode                          = WM_HAL_SDH_PORT_MODE_MMC;
    xfer.sdio_xfer_desc.sd_cmd.cmd_id       = WM_HAL_SDH_CMD_NUM_55;
    xfer.sdio_xfer_desc.sd_cmd.cmd_arg      = (drv_data->drv_ctx.card_info.rca << 16);
    xfer.sdio_xfer_desc.sd_cmd.cmd_rsp_type = WM_HAL_SDH_CMD_WAIT_RSP;
    xfer.sdio_xfer_desc.sd_cmd.cmd_rsp_len  = 2;
    xfer.sdio_xfer_desc.sd_cmd.cmd_timeout  = WM_DRV_OPS_SDH_SDMMC_CMD_TIMEOUT;
    ret                                     = wm_hal_sdh_xfer_polling(&drv_data->hal_sdh_dev, &xfer);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_SDH_SDMMC_LOG_E("CMD55 err");
        return ret;
    }
    WM_DRV_SDH_SDMMC_LOG_DUMP(WM_LOG_LEVEL_INFO, "CMD55 resp: ", 4, xfer.sdio_xfer_desc.sd_cmd.cmd_rsp,
                              xfer.sdio_xfer_desc.sd_cmd.cmd_rsp_len << 2);
    if (xfer.sdio_xfer_desc.sd_cmd.cmd_rsp[1] != xfer.sdio_xfer_desc.sd_cmd.cmd_id) {
        WM_DRV_SDH_SDMMC_LOG_E("CMD55 resp err");
        return ret;
    }

    /* ACMD6 */
    memset(&xfer, 0, sizeof(wm_hal_sdh_xfer_desc_t));
    xfer.port_mode                    = WM_HAL_SDH_PORT_MODE_MMC;
    xfer.sdio_xfer_desc.sd_cmd.cmd_id = WM_HAL_SDH_CMD_NUM_6;
    if (hw_sdh_sdmmc->sdh_cfg.bus_width == WM_SDH_BUS_WIDTH_1BIT) {
        xfer.sdio_xfer_desc.sd_cmd.cmd_arg = 0x00;
    } else if (hw_sdh_sdmmc->sdh_cfg.bus_width == WM_SDH_BUS_WIDTH_4BITS) {
        xfer.sdio_xfer_desc.sd_cmd.cmd_arg = 0x02;
    }
    xfer.sdio_xfer_desc.sd_cmd.cmd_rsp_type = WM_HAL_SDH_CMD_WAIT_RSP;
    xfer.sdio_xfer_desc.sd_cmd.cmd_rsp_len  = 2;
    xfer.sdio_xfer_desc.sd_cmd.cmd_timeout  = WM_DRV_OPS_SDH_SDMMC_CMD_TIMEOUT;
    ret                                     = wm_hal_sdh_xfer_polling(&drv_data->hal_sdh_dev, &xfer);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_SDH_SDMMC_LOG_E("ACMD6 err");
        return ret;
    }
    WM_DRV_SDH_SDMMC_LOG_DUMP(WM_LOG_LEVEL_INFO, "ACMD6 resp: ", 4, xfer.sdio_xfer_desc.sd_cmd.cmd_rsp,
                              xfer.sdio_xfer_desc.sd_cmd.cmd_rsp_len << 2);
    if (xfer.sdio_xfer_desc.sd_cmd.cmd_rsp[1] != xfer.sdio_xfer_desc.sd_cmd.cmd_id) {
        WM_DRV_SDH_SDMMC_LOG_E("ACMD6 resp err");
        return ret;
    }

    return ret;
}

static int wm_drv_sdh_sdmmc_set_block_len(wm_device_t *dev, uint32_t block_len)
{
    int ret                          = WM_ERR_SUCCESS;
    wm_drv_sdh_sdmmc_drv_t *drv_data = NULL;
    wm_hal_sdh_xfer_desc_t xfer      = { 0 };

    if (!dev->drv) {
        return WM_ERR_INVALID_PARAM;
    }

    drv_data = (wm_drv_sdh_sdmmc_drv_t *)dev->drv;

    /*
     * CMD 16 Select/deselect card
     */
    /* CMD16 */
    memset(&xfer, 0, sizeof(wm_hal_sdh_xfer_desc_t));
    xfer.port_mode                          = WM_HAL_SDH_PORT_MODE_MMC;
    xfer.sdio_xfer_desc.sd_cmd.cmd_id       = WM_HAL_SDH_CMD_NUM_16;
    xfer.sdio_xfer_desc.sd_cmd.cmd_arg      = block_len;
    xfer.sdio_xfer_desc.sd_cmd.cmd_rsp_type = WM_HAL_SDH_CMD_WAIT_RSP;
    xfer.sdio_xfer_desc.sd_cmd.cmd_rsp_len  = 2;
    xfer.sdio_xfer_desc.sd_cmd.cmd_timeout  = WM_DRV_OPS_SDH_SDMMC_CMD_TIMEOUT;
    ret                                     = wm_hal_sdh_xfer_polling(&drv_data->hal_sdh_dev, &xfer);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_SDH_SDMMC_LOG_E("CMD16 err");
        return ret;
    }
    WM_DRV_SDH_SDMMC_LOG_DUMP(WM_LOG_LEVEL_INFO, "CMD16 resp: ", 4, xfer.sdio_xfer_desc.sd_cmd.cmd_rsp,
                              xfer.sdio_xfer_desc.sd_cmd.cmd_rsp_len << 2);
    if ((xfer.sdio_xfer_desc.sd_cmd.cmd_rsp[1] & 0xFF) != xfer.sdio_xfer_desc.sd_cmd.cmd_id) {
        WM_DRV_SDH_SDMMC_LOG_E("CMD16 resp err");
        return WM_ERR_FAILED;
    }

    return ret;
}

static int wm_drv_sdh_sdmmc_card_stop_trans(wm_device_t *dev)
{
    int ret                          = WM_ERR_SUCCESS;
    wm_drv_sdh_sdmmc_drv_t *drv_data = NULL;
    wm_hal_sdh_xfer_desc_t xfer      = { 0 };

    if (!dev->drv) {
        return WM_ERR_INVALID_PARAM;
    }

    drv_data = (wm_drv_sdh_sdmmc_drv_t *)dev->drv;

    /*
     * CMD 12 STOP_TRANSMISSION
     */
    /* CMD12 */
    memset(&xfer, 0, sizeof(wm_hal_sdh_xfer_desc_t));
    xfer.port_mode                          = WM_HAL_SDH_PORT_MODE_MMC;
    xfer.sdio_xfer_desc.sd_cmd.cmd_id       = WM_HAL_SDH_CMD_NUM_12;
    xfer.sdio_xfer_desc.sd_cmd.cmd_arg      = 0;
    xfer.sdio_xfer_desc.sd_cmd.cmd_rsp_type = WM_HAL_SDH_CMD_WAIT_RSP;
    xfer.sdio_xfer_desc.sd_cmd.cmd_rsp_len  = 2;
    xfer.sdio_xfer_desc.sd_cmd.cmd_timeout  = WM_DRV_OPS_SDH_SDMMC_CMD_TIMEOUT;
    ret                                     = wm_hal_sdh_xfer_polling(&drv_data->hal_sdh_dev, &xfer);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_SDH_SDMMC_LOG_E("CMD12 err");
        return ret;
    }
    WM_DRV_SDH_SDMMC_LOG_DUMP(WM_LOG_LEVEL_INFO, "CMD12 resp: ", 4, xfer.sdio_xfer_desc.sd_cmd.cmd_rsp,
                              xfer.sdio_xfer_desc.sd_cmd.cmd_rsp_len << 2);
    if ((xfer.sdio_xfer_desc.sd_cmd.cmd_rsp[1] & 0xFF) != xfer.sdio_xfer_desc.sd_cmd.cmd_id) {
        WM_DRV_SDH_SDMMC_LOG_E("CMD12 resp err");
        return WM_ERR_FAILED;
    }

    return ret;
}

static void wm_drv_sdh_sdmmc_callback(void *hal_dev, wm_hal_sdh_evt_t *evt, void *user_data)
{
    wm_drv_sdh_sdmmc_drv_t *drv_data = (wm_drv_sdh_sdmmc_drv_t *)user_data;

    WM_DRV_SDH_SDMMC_LOG_D("sdmmc callback: %d", *evt);
    drv_data->drv_ctx.evt = *evt;
    wm_os_internal_sem_release(drv_data->drv_ctx.evt_sem);
}

static int wm_drv_sdh_sdmmc_wait_ready(wm_device_t *dev, uint32_t num_blocks)
{
    int ret                          = WM_ERR_SUCCESS;
    wm_drv_sdh_sdmmc_drv_t *drv_data = NULL;
    uint32_t start_time              = 0;
    uint32_t delta_ms                = 0;
    uint32_t card_state              = 0;
    uint8_t current_state            = 0;
    uint8_t error_state              = 0;
    wm_os_status_t os_status         = WM_OS_STATUS_SUCCESS;

    drv_data = (wm_drv_sdh_sdmmc_drv_t *)dev->drv;

    /* Waiting for transmission event */
    os_status = wm_os_internal_sem_acquire(drv_data->drv_ctx.evt_sem, WM_DRV_OPS_SDH_SDMMC_BLOCK_TIMEOUT);
    if (os_status == WM_OS_STATUS_SUCCESS) {
        if ((drv_data->drv_ctx.evt.type == WM_HAL_SDH_EVT_READ_DONE) ||
            (drv_data->drv_ctx.evt.type == WM_HAL_SDH_EVT_WRITE_DONE)) {
            ret = WM_ERR_SUCCESS;
        } else if ((drv_data->drv_ctx.evt.type == WM_HAL_SDH_EVT_READ_ERR) ||
                   (drv_data->drv_ctx.evt.type == WM_HAL_SDH_EVT_WRITE_ERR)) {
            ret = WM_ERR_FAILED;
        }
    } else {
        ret = WM_ERR_TIMEOUT;
    }

    if (num_blocks > 1) {
        ret += wm_drv_sdh_sdmmc_card_stop_trans(dev);
        if (ret != WM_ERR_SUCCESS) {
            return ret;
        }
    }

    /*waiting for card to trans state*/
    start_time = wm_os_internal_get_time_ms();
    do {
        ret += wm_drv_sdh_sdmmc_card_get_status(dev, &card_state);
        if (ret != WM_ERR_SUCCESS) {
            break;
        }
        current_state = (card_state >> 9) & 0xF;
        error_state   = (card_state >> 19) & 0x1;
        if (error_state) {
            ret = WM_ERR_FAILED;
            break;
        }
        delta_ms = wm_os_internal_get_time_ms() - start_time;
        if (delta_ms > WM_DRV_OPS_SDH_SDMMC_READY_TIMEOUT) {
            WM_DRV_SDH_SDMMC_LOG_E("Waiting ready timeout");
            ret = WM_ERR_TIMEOUT;
            break;
        }
    } while (current_state != 4);

    drv_data->drv_ctx.evt.type = WM_HAL_SDH_EVT_MAX;

    return ret;
}

static int wm_drv_ops_sdh_sdmmc_init(wm_device_t *dev)
{
    int ret                          = WM_ERR_SUCCESS;
    wm_drv_sdh_sdmmc_drv_t *drv_data = NULL;
    wm_dt_hw_sdh_t *hw_sdh_sdmmc     = NULL;
    wm_dma_data_t *dma_data          = NULL;
    wm_drv_clock_ctx_t *rcc_drv_ctx  = NULL;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state == WM_DEV_ST_INITED) {
        return WM_ERR_ALREADY_INITED;
    }

    drv_data = (wm_drv_sdh_sdmmc_drv_t *)wm_os_internal_malloc(sizeof(wm_drv_sdh_sdmmc_drv_t));
    if (!drv_data) {
        ret = WM_ERR_NO_MEM;
        goto exit;
    }
    memset(drv_data, 0x00, sizeof(wm_drv_sdh_sdmmc_drv_t));

    WM_DRV_SDH_SDMMC_LOCK(drv_data->drv_ctx.mutex);

    hw_sdh_sdmmc                = (wm_dt_hw_sdh_t *)dev->hw;
    drv_data->drv_ctx.clock_dev = wm_dt_get_device_by_name(hw_sdh_sdmmc->rcc_device_name);
    if (drv_data->drv_ctx.clock_dev == NULL) {
        wm_os_internal_free(drv_data);
        drv_data = NULL;
        ret      = WM_ERR_FAILED;
        goto exit;
    }
    drv_data->hal_sdh_dev.reg_base     = (wm_sdh_reg_t *)(hw_sdh_sdmmc->reg_base);
    drv_data->hal_sdh_dev.irq_num      = hw_sdh_sdmmc->irq_cfg.irq_num;
    drv_data->hal_sdh_dev.irq_priority = hw_sdh_sdmmc->irq_cfg.irq_priority;
    drv_data->hal_sdh_dev.fhclk        = wm_drv_rcc_get_config_clock(drv_data->drv_ctx.clock_dev, WM_RCC_TYPE_CPU);

    drv_data->hal_sdh_dev.sdh_cfg.clock_hz  = hw_sdh_sdmmc->sdh_cfg.clock_hz;
    drv_data->hal_sdh_dev.sdh_cfg.xfer_mode = WM_HAL_SDH_XFER_MODE_SPEED_HIGH;
    drv_data->hal_sdh_dev.sdh_cfg.bus_width = hw_sdh_sdmmc->sdh_cfg.bus_width;
    drv_data->hal_sdh_dev.sdh_cfg.port_mode = WM_HAL_SDH_PORT_MODE_MMC;
    drv_data->hal_sdh_dev.sdh_cfg.sd_mode   = WM_HAL_SDH_SD_MODE_SDMMC;

    drv_data->drv_ctx.dma_dev = wm_dt_get_device_by_name(hw_sdh_sdmmc->dma_device_name);
    if (drv_data->drv_ctx.dma_dev == NULL) {
        wm_os_internal_free(drv_data);
        drv_data = NULL;
        ret      = WM_ERR_FAILED;
        goto exit;
    }
    dma_data                          = (wm_dma_data_t *)drv_data->drv_ctx.dma_dev->drv;
    drv_data->hal_sdh_dev.dma_hal_dev = &dma_data->hal_dev;

    rcc_drv_ctx                       = (wm_drv_clock_ctx_t *)drv_data->drv_ctx.clock_dev->drv;
    drv_data->hal_sdh_dev.rcc_hal_dev = &rcc_drv_ctx->hal_dev;
    drv_data->drv_ctx.evt.type        = WM_HAL_SDH_EVT_MAX;

    dev->drv = drv_data;

    ret = wm_drv_clock_enable(drv_data->drv_ctx.clock_dev, WM_RCC_SDIOM_GATE_EN);
    if (ret != WM_ERR_SUCCESS) {
        goto exit;
    }

    ret = wm_hal_sdh_init(&drv_data->hal_sdh_dev);
    if (ret != WM_ERR_SUCCESS) {
        goto exit;
    }

    ret = wm_drv_sdh_sdmmc_pin_cfg(dev);
    if (ret != WM_ERR_SUCCESS) {
        goto exit;
    }

    ret = wm_drv_sdh_sdmmc_card_init(dev);
    if (ret != WM_ERR_SUCCESS) {
        goto exit;
    }

    ret = wm_drv_sdh_sdmmc_card_get_csd(dev);
    if (ret != WM_ERR_SUCCESS) {
        goto exit;
    }

    ret = wm_drv_sdh_sdmmc_card_get_status(dev, NULL);
    if (ret != WM_ERR_SUCCESS) {
        goto exit;
    }

    ret = wm_drv_sdh_sdmmc_card_sel(dev);
    if (ret != WM_ERR_SUCCESS) {
        goto exit;
    }

    ret = wm_drv_sdh_sdmmc_card_set_bus_width(dev);
    if (ret != WM_ERR_SUCCESS) {
        goto exit;
    }

    ret = wm_drv_sdh_sdmmc_set_block_len(dev, WM_SDH_BLOCK_SIZE);
    if (ret != WM_ERR_SUCCESS) {
        goto exit;
    }

    ret = wm_hal_sdh_tx_register_callback(&drv_data->hal_sdh_dev, wm_drv_sdh_sdmmc_callback, drv_data);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_SDH_SDMMC_LOG_E("register tx callback err");
        goto exit;
    }

    ret = wm_hal_sdh_rx_register_callback(&drv_data->hal_sdh_dev, wm_drv_sdh_sdmmc_callback, drv_data);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_SDH_SDMMC_LOG_E("register rx callback err");
        goto exit;
    }

    if (wm_os_internal_sem_create(&drv_data->drv_ctx.evt_sem, 0) != WM_OS_STATUS_SUCCESS) {
        ret = WM_ERR_NO_MEM;
        goto exit;
    }

    dev->state = WM_DEV_ST_INITED;
    WM_DRV_SDH_SDMMC_LOG_D("wm_drv_ops_sdh_sdmmc_init successfully");

exit:
    WM_DRV_SDH_SDMMC_UNLOCK(drv_data->drv_ctx.mutex);
    if (ret != WM_ERR_SUCCESS && dev->drv != NULL) {
        wm_os_internal_free(dev->drv);
        dev->drv = NULL;
    }

    return ret;
}

static int wm_drv_ops_sdh_sdmmc_deinit(wm_device_t *dev)
{
    int ret                          = WM_ERR_SUCCESS;
    wm_drv_sdh_sdmmc_drv_t *drv_data = NULL;
    wm_dt_hw_sdh_t *hw_sdh_sdmmc     = NULL;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    hw_sdh_sdmmc = (wm_dt_hw_sdh_t *)dev->hw;
    drv_data     = (wm_drv_sdh_sdmmc_drv_t *)dev->drv;

    WM_DRV_SDH_SDMMC_LOCK(drv_data->drv_ctx.mutex);

    for (uint8_t i = 0; i < hw_sdh_sdmmc->pin_cfg_count; i++) {
        ret = wm_drv_gpio_iomux_func_sel(hw_sdh_sdmmc->pin_cfg[i].pin_num, WM_GPIO_IOMUX_FUN5);
        if (ret != WM_ERR_SUCCESS) {
            goto exit;
        }
    }

    ret = wm_hal_sdh_deinit(&drv_data->hal_sdh_dev);
    if (ret != WM_ERR_SUCCESS) {
        goto exit;
    }

    ret = wm_drv_clock_disable(drv_data->drv_ctx.clock_dev, WM_RCC_SDIOM_GATE_EN);
    if (ret != WM_ERR_SUCCESS) {
        goto exit;
    }

    if (drv_data->drv_ctx.evt_sem) {
        wm_os_internal_sem_delete(drv_data->drv_ctx.evt_sem);
    }

    if (dev->drv != NULL) {
        wm_os_internal_free(dev->drv);
        dev->drv = NULL;
    }

    dev->state = WM_DEV_ST_UNINIT;

exit:
    WM_DRV_SDH_SDMMC_UNLOCK(drv_data->drv_ctx.mutex);

    return ret;
}

static int wm_drv_ops_sdh_sdmmc_read_blocks(wm_device_t *dev, uint8_t *buf, uint32_t start_block, uint32_t num_blocks)
{
    int ret                          = WM_ERR_SUCCESS;
    wm_drv_sdh_sdmmc_drv_t *drv_data = NULL;
    wm_hal_sdh_xfer_desc_t xfer      = { 0 };
    wm_drv_dma_status_t dma_status   = 0;

    if (dev == NULL || buf == NULL || num_blocks == 0 || num_blocks > WM_DRV_SDH_SDMMC_MAX_NUM_BLOCKS) {
        return WM_ERR_INVALID_PARAM;
    }

    drv_data = (wm_drv_sdh_sdmmc_drv_t *)dev->drv;
    if (drv_data == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (start_block + num_blocks <= 0 ||
        start_block + num_blocks > (uint32_t)(drv_data->drv_ctx.card_info.capacity / drv_data->drv_ctx.card_info.block_size)) {
        return WM_ERR_INVALID_PARAM;
    }

    WM_DRV_SDH_SDMMC_LOCK(drv_data->drv_ctx.mutex);

    xfer.sdio_xfer_desc.mode = WM_HAL_SDH_SDMMC_RX;

    /* CMD */
    xfer.port_mode = WM_HAL_SDH_PORT_MODE_MMC;
    if (num_blocks == 1) {
        xfer.sdio_xfer_desc.sd_cmd.cmd_id = WM_HAL_SDH_CMD_NUM_17;
    } else if (num_blocks > 1) {
        xfer.sdio_xfer_desc.sd_cmd.cmd_id = WM_HAL_SDH_CMD_NUM_18;
    }

    if (drv_data->drv_ctx.card_info.csd_ver == WM_DRV_SDH_SDMMC_CARD_CSD_VER_1) {
        xfer.sdio_xfer_desc.sd_cmd.cmd_arg = start_block * WM_SDH_BLOCK_SIZE;
    } else {
        xfer.sdio_xfer_desc.sd_cmd.cmd_arg = start_block;
    }
    xfer.sdio_xfer_desc.sd_cmd.cmd_rsp_type = WM_HAL_SDH_CMD_WAIT_RSP;
    xfer.sdio_xfer_desc.sd_cmd.cmd_rsp_len  = 2;
    xfer.sdio_xfer_desc.sd_cmd.cmd_timeout  = WM_DRV_OPS_SDH_SDMMC_CMD_TIMEOUT;

    /* DATA */
    xfer.sdio_xfer_desc.sd_data.rx.buf     = (void *)buf;
    xfer.sdio_xfer_desc.sd_data.rx.len     = num_blocks * WM_SDH_BLOCK_SIZE;
    xfer.sdio_xfer_desc.sd_data.rx.timeout = WM_DRV_OPS_SDH_SDMMC_BLOCK_TIMEOUT;

    dma_status = wm_drv_dma_request_ch(drv_data->drv_ctx.dma_dev, &drv_data->hal_sdh_dev.dma_ch, 0);
    if (dma_status != WM_DRV_DMA_STATUS_SUCCESS) {
        goto exit;
    }

    ret = wm_hal_sdh_xfer_dma(&drv_data->hal_sdh_dev, &xfer);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_SDH_SDMMC_LOG_E("read blocks err");
        goto exit;
    }

    if ((xfer.sdio_xfer_desc.sd_cmd.cmd_rsp[1] & 0xFF) != xfer.sdio_xfer_desc.sd_cmd.cmd_id) {
        WM_DRV_SDH_SDMMC_LOG_E("CMD%d resp err", xfer.sdio_xfer_desc.sd_cmd.cmd_id);
        ret = WM_ERR_FAILED;
        goto exit;
    }

    ret = wm_drv_sdh_sdmmc_wait_ready(dev, num_blocks);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_SDH_SDMMC_LOG_E("sdmmc wait ready err");
        goto exit;
    }

exit:
    if (dma_status == WM_DRV_DMA_STATUS_SUCCESS) {
        ret += (int)wm_drv_dma_release_ch(drv_data->drv_ctx.dma_dev, drv_data->hal_sdh_dev.dma_ch, WM_OS_WAIT_TIME_MAX);
    }

    WM_DRV_SDH_SDMMC_UNLOCK(drv_data->drv_ctx.mutex);

    return ret;
}
static int wm_drv_ops_sdh_sdmmc_write_blocks(wm_device_t *dev, const uint8_t *buf, uint32_t start_block, uint32_t num_blocks)
{
    int ret                          = WM_ERR_SUCCESS;
    wm_drv_sdh_sdmmc_drv_t *drv_data = NULL;
    wm_hal_sdh_xfer_desc_t xfer      = { 0 };
    wm_drv_dma_status_t dma_status   = 0;

    if (dev == NULL || buf == NULL || num_blocks == 0 || num_blocks > WM_DRV_SDH_SDMMC_MAX_NUM_BLOCKS) {
        return WM_ERR_INVALID_PARAM;
    }

    drv_data = (wm_drv_sdh_sdmmc_drv_t *)dev->drv;
    if (drv_data == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (start_block + num_blocks <= 0 ||
        start_block + num_blocks > (uint32_t)(drv_data->drv_ctx.card_info.capacity / drv_data->drv_ctx.card_info.block_size)) {
        return WM_ERR_INVALID_PARAM;
    }

    WM_DRV_SDH_SDMMC_LOCK(drv_data->drv_ctx.mutex);

    xfer.sdio_xfer_desc.mode = WM_HAL_SDH_SDMMC_TX;

    /* CMD */
    xfer.port_mode = WM_HAL_SDH_PORT_MODE_MMC;
    if (num_blocks == 1) {
        xfer.sdio_xfer_desc.sd_cmd.cmd_id = WM_HAL_SDH_CMD_NUM_24;
    } else if (num_blocks > 1) {
        xfer.sdio_xfer_desc.sd_cmd.cmd_id = WM_HAL_SDH_CMD_NUM_25;
    }

    if (drv_data->drv_ctx.card_info.csd_ver == WM_DRV_SDH_SDMMC_CARD_CSD_VER_1) {
        xfer.sdio_xfer_desc.sd_cmd.cmd_arg = start_block * WM_SDH_BLOCK_SIZE;
    } else {
        xfer.sdio_xfer_desc.sd_cmd.cmd_arg = start_block;
    }
    xfer.sdio_xfer_desc.sd_cmd.cmd_rsp_type = WM_HAL_SDH_CMD_WAIT_RSP;
    xfer.sdio_xfer_desc.sd_cmd.cmd_rsp_len  = 2;
    xfer.sdio_xfer_desc.sd_cmd.cmd_timeout  = WM_DRV_OPS_SDH_SDMMC_CMD_TIMEOUT;

    /* DATA */
    xfer.sdio_xfer_desc.sd_data.tx.buf     = (void *)buf;
    xfer.sdio_xfer_desc.sd_data.tx.len     = num_blocks * WM_SDH_BLOCK_SIZE;
    xfer.sdio_xfer_desc.sd_data.tx.timeout = WM_DRV_OPS_SDH_SDMMC_BLOCK_TIMEOUT;

    dma_status = wm_drv_dma_request_ch(drv_data->drv_ctx.dma_dev, &drv_data->hal_sdh_dev.dma_ch, 0);
    if (dma_status != WM_DRV_DMA_STATUS_SUCCESS) {
        goto exit;
    }

    ret = wm_hal_sdh_xfer_dma(&drv_data->hal_sdh_dev, &xfer);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_SDH_SDMMC_LOG_E("read blocks err");
        goto exit;
    }

    if ((xfer.sdio_xfer_desc.sd_cmd.cmd_rsp[1] & 0xFF) != xfer.sdio_xfer_desc.sd_cmd.cmd_id) {
        WM_DRV_SDH_SDMMC_LOG_E("CMD%d resp err", xfer.sdio_xfer_desc.sd_cmd.cmd_id);
        ret = WM_ERR_FAILED;
        goto exit;
    }

    ret = wm_drv_sdh_sdmmc_wait_ready(dev, num_blocks);
    if (ret != WM_ERR_SUCCESS) {
        WM_DRV_SDH_SDMMC_LOG_E("sdmmc wait ready err");
        goto exit;
    }

exit:
    if (dma_status == WM_DRV_DMA_STATUS_SUCCESS) {
        ret += (int)wm_drv_dma_release_ch(drv_data->drv_ctx.dma_dev, drv_data->hal_sdh_dev.dma_ch, WM_OS_WAIT_TIME_MAX);
    }

    WM_DRV_SDH_SDMMC_UNLOCK(drv_data->drv_ctx.mutex);

    return ret;
}

static int wm_drv_ops_sdh_sdmmc_get_card_info(wm_device_t *dev, wm_drv_sdh_sdmmc_card_info_t *card_info)
{
    int ret                          = WM_ERR_SUCCESS;
    wm_drv_sdh_sdmmc_drv_t *drv_data = NULL;

    if (dev == NULL || card_info == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    drv_data = (wm_drv_sdh_sdmmc_drv_t *)dev->drv;
    if (drv_data == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    WM_DRV_SDH_SDMMC_LOCK(drv_data->drv_ctx.mutex);
    ret = wm_drv_sdh_sdmmc_card_get_status(dev, NULL);
    if (ret == WM_ERR_SUCCESS) {
        *card_info = drv_data->drv_ctx.card_info;
    }
    WM_DRV_SDH_SDMMC_UNLOCK(drv_data->drv_ctx.mutex);

    return ret;
}

/**
 * @brief Constant definition for the SDH_SDMMC driver operations
 */
const wm_drv_sdh_sdmmc_ops_t wm_drv_sdh_sdmmc_ops = {
    .init          = wm_drv_ops_sdh_sdmmc_init,
    .deinit        = wm_drv_ops_sdh_sdmmc_deinit,
    .read_blocks   = wm_drv_ops_sdh_sdmmc_read_blocks,
    .write_blocks  = wm_drv_ops_sdh_sdmmc_write_blocks,
    .get_card_info = wm_drv_ops_sdh_sdmmc_get_card_info,
};

/**
 * @file wm_drv_ops_tft_lcd_spi.h
 *
 * @brief DRV OPS COMMON Module for SPI TFT LCD
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

#ifndef __WM_DRV_OPS_TFT_LCD_SPI_H__
#define __WM_DRV_OPS_TFT_LCD_SPI_H__

#include "wm_types.h"
#include "wm_error.h"
#include "wm_attr.h"
#include "wm_dt_hw.h"
#include "wm_drv_ops_tft_lcd.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable debug logs for TFT LCD driver
 */
#define LOG_TAG "DRV TFT_LCD"
#include "wm_log.h"

#define WM_DRV_TFT_LCD_DEBUG 0
#if WM_DRV_TFT_LCD_DEBUG
#define WM_DRV_TFT_LCD_LOG_D         wm_log_debug
#define WM_DRV_TFT_LCD_LOG_I         wm_log_info
#define WM_DRV_TFT_LCD_LOG_W         wm_log_warn
#define WM_DRV_TFT_LCD_LOG_E         wm_log_error
#define WM_DRV_TFT_LCD_LOG_DUMP      wm_log_dump
#define WM_DRV_TFT_LCD_FUNC_EXIT(fn) WM_DRV_TFT_LCD_LOG_D("%s exit", fn)
#define WM_DRV_TFT_LCD_FUNC_NAME     __func__
#else
#define WM_DRV_TFT_LCD_LOG_D(...)
#define WM_DRV_TFT_LCD_LOG_I(...)
#define WM_DRV_TFT_LCD_LOG_W(...)
#define WM_DRV_TFT_LCD_LOG_E(...)
#define WM_DRV_TFT_LCD_LOG_DUMP(...)
#define WM_DRV_TFT_LCD_FUNC_EXIT(fn)
#endif

/**
 * @brief Check param and return r
 */
#define WM_DRV_LCD_TFT_CHECK_PARAM_RET(p, r)       \
    if (!(p)) {                                    \
        wm_log_error("%s,%d", __FILE__, __LINE__); \
        return (r);                                \
    }

/**
 * @brief Check param and return WM_ERR_INVALID_PARAM
 */
#define WM_DRV_LCD_TFT_CHECK_PARAM(p)              \
    if (!(p)) {                                    \
        wm_log_error("%s,%d", __FILE__, __LINE__); \
        return WM_ERR_INVALID_PARAM;               \
    }

#define WM_DRV_LCD_TFT_CHECK_RETURN(r)                        \
    if ((r) != WM_ERR_SUCCESS) {                              \
        wm_log_error("%s,%d,ret(%d)", __FILE__, __LINE__, r); \
        return r;                                             \
    }

#define WM_DRV_LCD_TFT_CHECK_GOTO(r, tag)                     \
    if ((r) != WM_ERR_SUCCESS) {                              \
        wm_log_error("%s,%d,ret(%d)", __FILE__, __LINE__, r); \
        goto tag;                                             \
    }

#define WM_DRV_TFT_LCD_LOCK(lock)                                                        \
    do {                                                                                 \
        if (!lock) {                                                                     \
            if (WM_ERR_SUCCESS != wm_os_internal_mutex_create(&lock)) {                  \
                wm_log_error("mutex create err!");                                       \
                return WM_ERR_FAILED;                                                    \
            }                                                                            \
        }                                                                                \
        if (WM_ERR_SUCCESS != wm_os_internal_mutex_acquire(lock, WM_OS_WAIT_TIME_MAX)) { \
            wm_log_error("mutex acquire err!");                                          \
            return WM_ERR_FAILED;                                                        \
        }                                                                                \
    } while (0);

#define WM_DRV_TFT_LCD_UNLOCK(lock)         \
    do {                                    \
        if (!lock) {                        \
            return WM_ERR_NO_INITED;        \
        }                                   \
        wm_os_internal_mutex_release(lock); \
    } while (0);

#define LCD_XFER_BYTES_TIMEOUT_MS 1000 //uint:ms

/* The command header length definiation in command table which using TLV format
 * Header = Len + Delay + Cmd_Type
 */
#define LCD_CMD_SEQ_HEADER_LEN    (3)

/**
 * @brief TFT LCD driver structure encapsulates driver-specific data and operations related to the TFT LCD.
 *
 * This structure includes the device structure and the driver context structure.
 * It is used to store and manage the runtime state of the driver and the hardware interface.
 */

typedef struct {
    uint16_t x_resolution; /**< lcd panel width, number of pixels on x-axis */
    uint16_t y_resolution; /**< lcd panel height, number of pixels on y-axis */
    uint8_t rotation;      /**< indicates the rotation index, @ref wm_lcd_rotate_t */
} wm_tft_lcd_cfg_t;

typedef struct {
    wm_os_mutex_t *mutex;             /**< Mutex for thread-safe access to the LCD device */
    wm_device_t *spi_dev;             /**< The pointer of hardware controller which be used by TFT LCD device */
    wm_dt_hw_spim_dev_cfg_t *spi_cfg; /**< The SPI configuration pointer indicates to the hw structure spi_cfg */
    wm_tft_lcd_cfg_t panel_cfg;       /**< The LCD configuration which is clone from the hw structure panel_cfg */
    wm_lcd_tx_callback_t tx_done_cb;  /**< Callback function for LCD transmission completion */
    void *usr_data;                   /**< Callback function argument for LCD transmission completion */
    bool dma_sending;                 /**< Indicates if DMA is running by LCD module, to avoid Async and Async conflicts */
} wm_drv_tft_lcd_spi_t;

int lcd_io_backlight_on(wm_dt_hw_tft_lcd_spi_t *hw);
int lcd_io_backlight_off(wm_dt_hw_tft_lcd_spi_t *hw);
int lcd_io_dcx_data_start(wm_dt_hw_tft_lcd_spi_t *hw);
int lcd_io_dcx_cmd_start(wm_dt_hw_tft_lcd_spi_t *hw);
int lcd_io_dcx_stop(wm_dt_hw_tft_lcd_spi_t *hw);
int lcd_io_reset(wm_dt_hw_tft_lcd_spi_t *hw);

void lcd_spim_callback(int result, void *data);
int lcd_send_bytes_with_sdhspi(wm_device_t *dev, uint8_t *buf, int length);
int lcd_send_data(wm_device_t *dev, uint8_t *buf, int length);
int lcd_send_command(wm_device_t *dev, uint16_t cmd, wm_lcd_cmd_type_t cmd_type);
int lcd_init_cmd(wm_device_t *dev, const uint8_t *cmd_table, int size);

#ifdef __cplusplus
}
#endif

#endif /* __WM_DRV_OPS_TFT_LCD_SPI_H__ */

/**
 * @file wm_drv_ops_seg_lcd.c
 *
 * @brief DRV_OPS_SEG_LCD Module
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

#include "wm_drv_seg_lcd.h"
#include "wm_drv_rcc.h"
#include "wm_dt_hw.h"

#define LOG_TAG "drv_seg_lcd"
#include "wm_log.h"

#define WM_DRV_SEG_LCD_LOCK(lock)                                                        \
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

#define WM_DRV_SEG_LCD_UNLOCK(lock)         \
    do {                                    \
        if (!lock) {                        \
            return WM_ERR_NO_INITED;        \
        }                                   \
        wm_os_internal_mutex_release(lock); \
    } while (0);

static int wm_drv_ops_seg_lcd_init(wm_device_t *dev);
static int wm_drv_ops_seg_lcd_deinit(wm_device_t *dev);
static int wm_drv_ops_seg_lcd_clear(wm_device_t *dev);
static int wm_drv_ops_seg_lcd_display(wm_device_t *dev, uint8_t com_id, uint32_t seg_mask, uint32_t seg_data);

typedef struct {
    int (*init)(wm_device_t *dev);   /**< Pointer to the initialization function */
    int (*deinit)(wm_device_t *dev); /**< Pointer to the deinitialization function */
    int (*clear)(wm_device_t *dev);  /**< Pointer to the clear display function */
    int (*display)(wm_device_t *dev, uint8_t com_id, uint32_t seg_mask,
                   uint32_t seg_data); /**< Pointer to the display function */
} wm_drv_seg_lcd_ops_t;

/**
 * Represents the device context structure for the segment LCD driver.
 * This structure is used internally by the driver to maintain context-specific
 * information necessary for the proper functioning of the segment LCD.
 * It includes a mutex for thread-safe operations.
 */
typedef struct {
    wm_os_mutex_t *mutex;   /**< Mutex for thread-safe access to the segment LCD device */
    wm_device_t *clock_dev; /**< Pointer to a clock device, used for timing control */
} wm_drv_seg_lcd_dev_ctx_t;

/**
 * @brief Segment LCD driver structure that encapsulates driver-specific data and operations related to the hardware segment LCD.
 *
 * This structure includes the hardware abstraction layer (HAL) device structure and the driver context structure.
 * It is used to store and manage the runtime state of the driver and the hardware interface.
 */
typedef struct {
    wm_hal_seg_lcd_dev_t hal_dev;     /**< HAL device structure */
    wm_drv_seg_lcd_dev_ctx_t drv_ctx; /**< Driver context for thread safety */
} wm_drv_seg_lcd_drv_t;

static int wm_drv_ops_seg_lcd_init(wm_device_t *dev)
{
    int ret                        = WM_ERR_SUCCESS;
    wm_drv_seg_lcd_drv_t *drv_data = NULL;
    wm_dt_hw_seg_lcd_t *hw_seg_lcd = NULL;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state == WM_DEV_ST_INITED) {
        return WM_ERR_ALREADY_INITED;
    }

    drv_data = (wm_drv_seg_lcd_drv_t *)wm_os_internal_malloc(sizeof(wm_drv_seg_lcd_drv_t));
    if (!drv_data) {
        return WM_ERR_NO_MEM;
    }
    memset(drv_data, 0x00, sizeof(wm_drv_seg_lcd_drv_t));

    hw_seg_lcd                               = (wm_dt_hw_seg_lcd_t *)dev->hw;
    drv_data->hal_dev.seg_lcd_reg            = (wm_seg_lcd_reg_t *)(hw_seg_lcd->reg_base);
    drv_data->hal_dev.seg_lcd_cfg.duty_sel   = hw_seg_lcd->seg_lcd_cfg.duty_sel;
    drv_data->hal_dev.seg_lcd_cfg.vlcd_cc    = hw_seg_lcd->seg_lcd_cfg.vlcd_cc;
    drv_data->hal_dev.seg_lcd_cfg.bias       = hw_seg_lcd->seg_lcd_cfg.bias;
    drv_data->hal_dev.seg_lcd_cfg.hd         = hw_seg_lcd->seg_lcd_cfg.hd;
    drv_data->hal_dev.seg_lcd_cfg.frame_freq = hw_seg_lcd->seg_lcd_cfg.frame_freq;
    drv_data->hal_dev.seg_lcd_cfg.com_num    = hw_seg_lcd->seg_lcd_cfg.com_num;
    drv_data->drv_ctx.clock_dev              = wm_dt_get_device_by_name(hw_seg_lcd->rcc_device_name);
    dev->drv                                 = drv_data;

    WM_DRV_SEG_LCD_LOCK(drv_data->drv_ctx.mutex);

    ret = wm_drv_clock_enable(drv_data->drv_ctx.clock_dev, WM_RCC_LCD_GATE_EN);
    if (ret != WM_ERR_SUCCESS) {
        goto exit;
    }

    ret = wm_hal_seg_lcd_init(&drv_data->hal_dev);
    if (ret != WM_ERR_SUCCESS) {
        goto exit;
    }
    ret = wm_hal_seg_lcd_clear(&drv_data->hal_dev);
    if (ret != WM_ERR_SUCCESS) {
        goto exit;
    }

    dev->state = WM_DEV_ST_INITED;

exit:
    WM_DRV_SEG_LCD_UNLOCK(drv_data->drv_ctx.mutex);
    if (ret != WM_ERR_SUCCESS && dev->drv != NULL) {
        wm_os_internal_free(dev->drv);
        dev->drv = NULL;
    }

    return ret;
}

static int wm_drv_ops_seg_lcd_deinit(wm_device_t *dev)
{
    int ret                        = WM_ERR_SUCCESS;
    wm_drv_seg_lcd_drv_t *drv_data = NULL;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    if (dev->state != WM_DEV_ST_INITED) {
        return WM_ERR_NO_INITED;
    }

    drv_data = (wm_drv_seg_lcd_drv_t *)dev->drv;

    WM_DRV_SEG_LCD_LOCK(drv_data->drv_ctx.mutex);
    wm_hal_seg_lcd_deinit(&drv_data->hal_dev);
    WM_DRV_SEG_LCD_UNLOCK(drv_data->drv_ctx.mutex);

    ret = wm_drv_clock_disable(drv_data->drv_ctx.clock_dev, WM_RCC_LCD_GATE_EN);
    if (ret != WM_ERR_SUCCESS) {
        return ret;
    }

    if (dev->drv != NULL) {
        wm_os_internal_free(dev->drv);
        dev->drv = NULL;
    }

    dev->state = WM_DEV_ST_UNINIT;

    return ret;
}

static int wm_drv_ops_seg_lcd_clear(wm_device_t *dev)
{
    int ret                        = WM_ERR_SUCCESS;
    wm_drv_seg_lcd_drv_t *drv_data = NULL;

    if (dev == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    drv_data = (wm_drv_seg_lcd_drv_t *)dev->drv;
    if (drv_data == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    WM_DRV_SEG_LCD_LOCK(drv_data->drv_ctx.mutex);
    ret = wm_hal_seg_lcd_clear(&drv_data->hal_dev);
    WM_DRV_SEG_LCD_UNLOCK(drv_data->drv_ctx.mutex);

    return ret;
}

static int wm_drv_ops_seg_lcd_display(wm_device_t *dev, uint8_t com_id, uint32_t seg_mask, uint32_t seg_data)
{
    int ret                        = WM_ERR_SUCCESS;
    wm_drv_seg_lcd_drv_t *drv_data = NULL;

    if (dev == NULL || com_id >= WM_SEG_LCD_COM_MAX || seg_mask == 0) {
        return WM_ERR_INVALID_PARAM;
    }

    if ((~seg_mask & seg_data) != 0) {
        return WM_ERR_INVALID_PARAM;
    }

    drv_data = (wm_drv_seg_lcd_drv_t *)dev->drv;
    if (drv_data == NULL) {
        return WM_ERR_INVALID_PARAM;
    }

    WM_DRV_SEG_LCD_LOCK(drv_data->drv_ctx.mutex);
    ret = wm_hal_seg_lcd_write(&drv_data->hal_dev, com_id, seg_mask, seg_data);
    WM_DRV_SEG_LCD_UNLOCK(drv_data->drv_ctx.mutex);

    return ret;
}

const wm_drv_seg_lcd_ops_t wm_drv_seg_lcd_ops = {
    .init    = wm_drv_ops_seg_lcd_init,
    .deinit  = wm_drv_ops_seg_lcd_deinit,
    .clear   = wm_drv_ops_seg_lcd_clear,
    .display = wm_drv_ops_seg_lcd_display,
};
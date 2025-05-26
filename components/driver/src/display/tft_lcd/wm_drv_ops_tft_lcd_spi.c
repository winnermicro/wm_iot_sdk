/**
 * @file wm_drv_ops_tft_lcd_spi.c
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

#include "wm_drv_tft_lcd.h"
#include "wm_dt.h"
#include "wm_dt_hw.h"

#include "wm_drv_gpio.h"
#include "wm_utils.h"
#include "wm_drv_sdh_spi.h"
#include "wm_drv_ops_tft_lcd_spi.h"

/*only the content which length >=128 can be considered to be transmitted by DMA, 
 *and DMA only transmit the part that is 4-byte aligned x*/
#define TFT_LCD_DMA_TRIGGER_THRESHOLD (128)

/*wait the previous data dma transfer done.
 *not optimize this function, otherwise system may aways loop here*/
__attribute__((optimize("O0"))) static void _waiting_data_tx_done(wm_device_t *dev)
{
    wm_drv_tft_lcd_spi_t *drv = (wm_drv_tft_lcd_spi_t *)dev->drv;

    //loop to avoid conflict if async and sync operation both existing
    while (drv->dma_sending == true) {
    }
}

int lcd_io_backlight_on(wm_dt_hw_tft_lcd_spi_t *hw)
{
    int ret = WM_ERR_FAILED;

    WM_DRV_TFT_LCD_LOG_D("lcd_back_on");
    ret = wm_drv_gpio_data_set(hw->io_lcd_led);
    WM_DRV_LCD_TFT_CHECK_RETURN(ret);

    return ret;
}

int lcd_io_backlight_off(wm_dt_hw_tft_lcd_spi_t *hw)
{
    int ret = WM_ERR_FAILED;

    WM_DRV_TFT_LCD_LOG_D("lcd_back_off");
    ret = wm_drv_gpio_data_reset(hw->io_lcd_led);
    WM_DRV_LCD_TFT_CHECK_RETURN(ret);

    return ret;
}

int lcd_io_dcx_data_start(wm_dt_hw_tft_lcd_spi_t *hw)
{
    int ret = WM_ERR_FAILED;

    ret = wm_drv_gpio_data_set(hw->io_lcd_dcx);
    WM_DRV_LCD_TFT_CHECK_RETURN(ret);

    return ret;
}

int lcd_io_dcx_cmd_start(wm_dt_hw_tft_lcd_spi_t *hw)
{
    int ret = WM_ERR_FAILED;

    ret = wm_drv_gpio_data_reset(hw->io_lcd_dcx);
    WM_DRV_LCD_TFT_CHECK_RETURN(ret);

    return ret;
}

int lcd_io_dcx_stop(wm_dt_hw_tft_lcd_spi_t *hw)
{
    int ret = WM_ERR_FAILED;

    ret = wm_drv_gpio_data_set(hw->io_lcd_dcx);
    WM_DRV_LCD_TFT_CHECK_RETURN(ret);

    return ret;
}

int lcd_io_reset(wm_dt_hw_tft_lcd_spi_t *hw)
{
    int ret = WM_ERR_FAILED;

    ret = wm_drv_gpio_data_set(hw->io_lcd_reset);
    WM_DRV_LCD_TFT_CHECK_RETURN(ret);

    /*TODO: clarify the time requirement from datasheet */
    wm_os_internal_time_delay_ms(100);
    ret = wm_drv_gpio_data_reset(hw->io_lcd_reset);
    WM_DRV_LCD_TFT_CHECK_RETURN(ret);

    wm_os_internal_time_delay_ms(200);
    ret = wm_drv_gpio_data_set(hw->io_lcd_reset);
    WM_DRV_LCD_TFT_CHECK_RETURN(ret);

    wm_os_internal_time_delay_ms(120);
    return ret;
}

void lcd_spim_callback(int result, void *lcd_drv)
{
    wm_drv_tft_lcd_spi_t *drv = NULL;

    drv = (wm_drv_tft_lcd_spi_t *)lcd_drv;
    if (!drv) {
        WM_DRV_TFT_LCD_LOG_E("lcd cb param invalid");
    }

    drv->dma_sending = false;
    if (drv->tx_done_cb) {
        drv->tx_done_cb(result, drv->usr_data);
    }
}

int lcd_send_bytes_with_sdhspi(wm_device_t *dev, uint8_t *buf, int length)
{
    int ret                   = WM_ERR_SUCCESS;
    wm_drv_tft_lcd_spi_t *drv = (wm_drv_tft_lcd_spi_t *)dev->drv;

    spim_transceive_t desc = { 0 };
    desc.tx_buf            = buf;
    desc.tx_len            = length;

    if (buf == NULL || length == 0) {
        return WM_ERR_INVALID_PARAM;
    }

    //DMA transfer may not yield performance benefits for data lengths under 32 bytes due to initialization overheads
    if ((length < TFT_LCD_DMA_TRIGGER_THRESHOLD) || (drv->tx_done_cb == NULL)) {
        ret = wm_drv_sdh_spi_transceive_sync(drv->spi_dev, drv->spi_cfg, &desc, LCD_XFER_BYTES_TIMEOUT_MS);
        WM_DRV_LCD_TFT_CHECK_RETURN(ret);
    } else {
        int index                = 0;
        wm_data_align_t table[3] = { 0 };

        //deal with the scenario the buffer start address or buffer total length not 4 byte alignment
        ret = wm_data_align_parser(buf, length, table);
        //WM_DRV_TFT_LCD_LOG_D("buf=%p, total len=%d", buf, length);
        //WM_DRV_TFT_LCD_LOG_D("[0]addr=%p, size=%d, flag=%d", table[0].addr, table[0].size, table[0].flag);
        //WM_DRV_TFT_LCD_LOG_D("[1]addr=%p, size=%d, flag=%d", table[1].addr, table[1].size, table[1].flag);
        //WM_DRV_TFT_LCD_LOG_D("[2]addr=%p, size=%d, flag=%d", table[2].addr, table[2].size, table[2].flag);

        for (index = 0; index < 3; index++) {
            desc.tx_buf = table[index].addr;
            desc.tx_len = table[index].size;

            /*wait the previous data dma transfer done*/
            _waiting_data_tx_done(dev);
            WM_DRV_TFT_LCD_LOG_D("id=%d, buf=%p, size=%d, flag=%d, buf[0]=0x%x, buf[1]=0x%x", index, table[index].addr,
                                 table[index].size, table[index].flag, *(table[index].addr), *(table[index].addr + 1));
            if (table[index].flag == WM_DATA_ALIGN_TYPE_DMA) {
                drv->dma_sending = true;

                ret = wm_drv_sdh_spi_transceive_async(drv->spi_dev, drv->spi_cfg, &desc, lcd_spim_callback, drv);
                if (ret != WM_ERR_SUCCESS) {
                    /* release the resource to avoid dead lock or system halt */
                    WM_DRV_TFT_LCD_LOG_E("async failed,ret=%d", ret);
                    drv->dma_sending = false;
                    if (drv->tx_done_cb) {
                        drv->tx_done_cb(ret, drv->usr_data);
                    }
                }
                WM_DRV_LCD_TFT_CHECK_RETURN(ret);
            } else if (table[index].flag == WM_DATA_ALIGN_TYPE_CPU) {
                ret = wm_drv_sdh_spi_transceive_sync(drv->spi_dev, drv->spi_cfg, &desc, LCD_XFER_BYTES_TIMEOUT_MS);
                WM_DRV_LCD_TFT_CHECK_RETURN(ret);
            }
        }
    }
    return ret;
}

int lcd_send_data(wm_device_t *dev, uint8_t *buf, int length)
{
    int ret                    = WM_ERR_FAILED;
    wm_dt_hw_tft_lcd_spi_t *hw = (wm_dt_hw_tft_lcd_spi_t *)dev->hw;

    /*need the previous data dma transfer done*/
    _waiting_data_tx_done(dev);

    ret = lcd_io_dcx_data_start(hw);
    WM_DRV_LCD_TFT_CHECK_RETURN(ret);

    ret = lcd_send_bytes_with_sdhspi(dev, buf, length);
    WM_DRV_LCD_TFT_CHECK_RETURN(ret);

    return ret;
}

int lcd_send_command(wm_device_t *dev, uint16_t cmd, wm_lcd_cmd_type_t cmd_type)
{
    int ret                    = WM_ERR_FAILED;
    wm_dt_hw_tft_lcd_spi_t *hw = (wm_dt_hw_tft_lcd_spi_t *)dev->hw;

    if (cmd_type >= LCD_CMD_TYPE_MAX) {
        return ret;
    }

    /*wait the previous data dma transfer done*/
    _waiting_data_tx_done(dev);

    WM_DRV_TFT_LCD_LOG_D("lcd_send_command, 0x%x, type(%d)", cmd, cmd_type);
    ret = lcd_io_dcx_cmd_start(hw);
    WM_DRV_LCD_TFT_CHECK_RETURN(ret);

    ret = lcd_send_bytes_with_sdhspi(dev, (uint8_t *)&cmd, WM_BIT(cmd_type));
    WM_DRV_LCD_TFT_CHECK_RETURN(ret);

    return ret;
}

int lcd_init_cmd(wm_device_t *dev, const uint8_t *cmd_table, int size)
{
    int ret                    = WM_ERR_FAILED;
    uint8_t cmd_data_len       = 0;
    uint16_t cmd               = 0;
    uint8_t delay              = 0;
    const uint8_t *data_buf    = NULL;
    const uint8_t *pos         = cmd_table;
    wm_lcd_cmd_type_t cmd_type = LCD_CMD_TYPE_8BIT;

    WM_DRV_LCD_TFT_CHECK_PARAM(pos);

    while (*pos != 0) {
        if (pos > cmd_table + size) {
            WM_DRV_TFT_LCD_LOG_E("exceed the max cmd_table size");
            break;
        }

        cmd_data_len = *pos;
        delay        = *(pos + 1);
        cmd_type     = *(pos + 2);

        if (cmd_type == LCD_CMD_TYPE_16BIT) {
            cmd = ((uint16_t)pos[LCD_CMD_SEQ_HEADER_LEN] << 8) | pos[LCD_CMD_SEQ_HEADER_LEN + 1];
        } else if (cmd_type == LCD_CMD_TYPE_8BIT) {
            cmd = *(pos + LCD_CMD_SEQ_HEADER_LEN);
        } else {
            WM_DRV_TFT_LCD_LOG_E("incorrect cmd table");
            break;
        }

        // Send the command bytes
        ret = lcd_send_command(dev, cmd, cmd_type);
        WM_DRV_LCD_TFT_CHECK_RETURN(ret);

        if (cmd_data_len >= 2) {
            data_buf = pos + LCD_CMD_SEQ_HEADER_LEN + WM_BIT(cmd_type);
            // Send the data bytes following the command
            ret = lcd_send_data(dev, (uint8_t *)data_buf, cmd_data_len - 1);
            WM_DRV_LCD_TFT_CHECK_RETURN(ret);
        }

        WM_DRV_TFT_LCD_LOG_D("lcd_init_cmd cmd=0x%x, cmd_type=0x%x, delay=0x%x, cmd_data_len=0x%x, data=0x%x, dataaddr=%p \n",
                             cmd, cmd_type, delay, cmd_data_len, *data_buf, data_buf);

        // Move to the next command
        pos += cmd_data_len + LCD_CMD_SEQ_HEADER_LEN;

        if (delay > 0) {
            WM_DRV_TFT_LCD_LOG_D("delay(0x%x)ms", delay);
            wm_os_internal_time_delay_ms(delay);
        }
    }

    return ret;
}

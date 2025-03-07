/**
 * @file main.c
 *
 * @brief I2S DRV demo Module
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

#include <stdint.h>
#include <string.h>
#include "wm_osal.h"
#include "wmsdk_config.h"
#include "wm_list.h"
#include "wm_error.h"
#include "wm_types.h"
#include "wm_drv_i2s.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "wm_log.h"
#undef LOG_TAG
#define LOG_TAG                "example_i2s"

#define WM_APP_I2S_RX_PKT_SIZE 1024
#define WM_APP_I2S_TX_PKT_SIZE 1024

#define WM_APP_I2S_RX_PKT_NUM  6
#define WM_APP_I2S_TX_PKT_NUM  6

/**
 * @brief I2S configuration.
 *
 * @note
 *  - two board communication, you need modify g_cfg.mode , one use master, one use slave
 */
static wm_drv_i2s_cfg_t g_cfg = {
    .mode        = WM_I2S_MODE_MASTER,
    .dir         = WM_I2S_DIR_INOUT,
    .std         = WM_I2S_STD_I2S,
    .xtype       = WM_I2S_XFER_DMA,
    .rx_pkt_size = WM_APP_I2S_RX_PKT_SIZE,
    .rx_pkt_num  = WM_APP_I2S_RX_PKT_NUM,
    .tx_pkt_num  = WM_APP_I2S_TX_PKT_NUM,
};

static wm_drv_i2s_format_t g_fmt = {
    .bits           = WM_I2S_BITS_16BIT,
    .ctype          = WM_I2S_CHAN_TYPE_STEREO,
    .sample_rate_hz = WM_DRV_I2S_SP_48000,
};

static QueueHandle_t i2s_rx_queue = NULL;

static uint64_t g_send_pkts = 0;
static uint64_t g_recv_pkts = 0;

static int write_callback(wm_device_t *dev, wm_drv_i2s_event_t *event)
{
    if (event->len > 0) {
        g_send_pkts++;
        if (g_send_pkts % 100 == 0) {
            wm_log_debug("write:%llu", g_send_pkts);
        }
    }
    return WM_ERR_SUCCESS;
}

static void recv_data_process(uint8_t *data, int len)
{
    g_recv_pkts++;
    if (g_recv_pkts % 100 == 0) {
        wm_log_debug("recv:%llu", g_recv_pkts);
    }
}

static int read_callback(wm_device_t *dev, wm_drv_i2s_event_t *event)
{
    portBASE_TYPE pxHigherPriorityTaskWoken = pdFALSE;

    /*
    Send the RX data to other task to proceess.

    The data in the callback is not processed directly because the callback is in the interrupt context.
    It will block the whole system and lost data if too much things is proceessed in the interrupt.
    */
    xQueueSendFromISR(i2s_rx_queue, event, &pxHigherPriorityTaskWoken);

    if ((pdTRUE == pxHigherPriorityTaskWoken)) {
        /*wake up the higher task after iterrupt end*/
        portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
    }

    return WM_ERR_SUCCESS;
}

static uint8_t *write_data_init(void)
{
    uint8_t *pbuf = NULL;
    int i;

    /* malloc write buffer*/
    pbuf = malloc(WM_APP_I2S_TX_PKT_SIZE * g_cfg.tx_pkt_num);
    if (!pbuf) {
        wm_log_error("malloc send buf fail, size=%d", WM_APP_I2S_TX_PKT_SIZE);
        return pbuf;
    }

    /*fill incremental uint8_t integer*/
    for (i = 0; i < g_cfg.tx_pkt_num * WM_APP_I2S_TX_PKT_SIZE; i++) {
        pbuf[i] = (uint8_t)i;
    }

    return pbuf;
}

static void write_task(void *param)
{
    int err;
    wm_device_t *i2s_dev = param;
    int tx_buf_index     = 0;
    uint8_t *pbuf        = NULL;

    /*Init write buffer and data*/
    pbuf = write_data_init();
    if (!pbuf) {
        wm_log_error("init write data fail");
        goto END;
    }

    /*Register TX done event callback*/
    wm_drv_i2s_register_write_cb(i2s_dev, write_callback);

    wm_log_info("start write data loop");

    while (1) {
        /*write to the pair side*/
        while (1) {
            err = wm_drv_i2s_write_async(i2s_dev, pbuf + (tx_buf_index * WM_APP_I2S_TX_PKT_SIZE), WM_APP_I2S_TX_PKT_SIZE);
            if (err == WM_ERR_NO_MEM) {
                /*All the DMA desc is using, wait a moment.*/
                vTaskDelay(pdMS_TO_TICKS(5));
            } else if (err == WM_ERR_SUCCESS) {
                /*OK, free pbuf at tx done call back*/
                tx_buf_index++;
                tx_buf_index %= g_cfg.tx_pkt_num;
                break;
            } else {
                /*error*/
                wm_log_error("write err=%d,exit!", err);
                break;
            }
        }
    }

    /*write stop*/
    wm_drv_i2s_write_stop(i2s_dev);
    wm_drv_i2s_register_write_cb(i2s_dev, NULL);

END:
    if (pbuf) {
        free(pbuf);
    }
    vTaskDelete(NULL);
}

static void read_task(void *param)
{
    wm_drv_i2s_event_t event;
    wm_device_t *i2s_dev = param;

    uint8_t *recv_buf = NULL;
    int i;

    /*malloc buffer for data receive*/
    recv_buf = malloc(g_cfg.rx_pkt_size * g_cfg.rx_pkt_num);
    if (!recv_buf) {
        goto END;
    }

    /*create queue for data process*/
    i2s_rx_queue = xQueueCreate(WM_APP_I2S_RX_PKT_NUM, sizeof(wm_drv_i2s_event_t));
    if (!i2s_rx_queue) {
        goto END;
    }

    /*register data received callback*/
    wm_drv_i2s_register_read_cb(i2s_dev, read_callback);

    wm_log_info("start read data");

    /*append buffer to receive*/
    for (i = 0; i < g_cfg.rx_pkt_num; i++) {
        wm_drv_i2s_read_async(i2s_dev, recv_buf + i * g_cfg.rx_pkt_size, g_cfg.rx_pkt_size);
    }

    while (1) {
        if (xQueueReceive(i2s_rx_queue, &event, 1000) == pdPASS) {
            if (event.type == WM_DRV_I2S_EVENT_RX_READY) {
                if (event.buf && event.len > 0) {
                    /*proc receive data*/
                    recv_data_process(event.buf, event.len);
                }
                /*append buffer to receive*/
                wm_drv_i2s_read_async(i2s_dev, event.buf, g_cfg.rx_pkt_size);
            } else if (event.type == WM_DRV_I2S_EVENT_RX_DONE) {
                wm_log_info("RX done");
            }
        }
    }

    /*stop read*/
    wm_drv_i2s_read_stop(i2s_dev);
    wm_drv_i2s_register_read_cb(i2s_dev, NULL);

END:

    /*free receive buffer*/
    if (recv_buf) {
        free(recv_buf);
    }

    /*free receive event queue*/
    if (i2s_rx_queue) {
        vQueueDelete(i2s_rx_queue);
        i2s_rx_queue = NULL;
    }

    /*delete RX task*/
    vTaskDelete(NULL);
}

int main(void)
{
    wm_device_t *i2s_dev = NULL;

    wm_log_info("I2S example start.");

    /*Init I2S driver*/
    i2s_dev = wm_drv_i2s_init("i2s", &g_cfg);
    if (i2s_dev == NULL) {
        wm_log_error("I2S driver init Failed!");
        return WM_ERR_FAILED;
    } else {
        wm_log_info("start OK, mode=%d,dir=%d,std=%d,fmt=%d,ctype=%d,rx_pkt_size=%u,rx_pkt_num=%u,tx_pkt_num=%u", g_cfg.mode,
                    g_cfg.dir, g_cfg.std, g_fmt.bits, g_fmt.ctype, g_cfg.rx_pkt_size, g_cfg.rx_pkt_num, g_cfg.tx_pkt_num);
    }

    if (wm_drv_i2s_set_format(i2s_dev, g_fmt.sample_rate_hz, g_fmt.bits, g_fmt.ctype) != WM_ERR_SUCCESS) {
        wm_log_error("I2S set format fail.");
        return WM_ERR_FAILED;
    }

    /*Start I2S read task*/
    if (g_cfg.dir == WM_I2S_DIR_IN || g_cfg.dir == WM_I2S_DIR_INOUT) {
        xTaskCreate(read_task, "i2s_rx", 1024, i2s_dev, 5, NULL);
    }

    /*Start I2S write task*/
    if (g_cfg.dir == WM_I2S_DIR_OUT || g_cfg.dir == WM_I2S_DIR_INOUT) {
        xTaskCreate(write_task, "i2s_tx", 1024, i2s_dev, 4, NULL);
    }

    return WM_ERR_SUCCESS;
}

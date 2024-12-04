/**
 * @file wm_drv_dma_m2m_dmeo.c
 *
 * @brief dma DRV API memory to memory transfer demo
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

#include "wmsdk_config.h"
#include "wm_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "wm_irq.h"
#include "wm_log.h"
#include "wm_drv_dma.h"
#include "wm_drv_dma_m2m_demo.h"

#undef LOG_TAG
#define LOG_TAG                        "DMA DRV M2M DEMO"
#define DRV_DMA_M2M_EXAMPLE_DUMP_WIDTH 0xf
#define DRV_DMA_M2M_EXAMPLE_LOG_INFO   wm_log_info
#define DRV_DMA_M2M_EXAMPLE_DUMP       wm_log_dump
#define WM_DMA_TIME_DELAY              10

uint8_t dma_src_data[DRV_DMA_M2M_EXAMPLE_XFER_LIST_LEN][DRV_DMA_M2M_EXAMPLE_XFER_LEN] = { 0x0 };
uint8_t dma_dest_buf[DRV_DMA_M2M_EXAMPLE_XFER_LIST_LEN][DRV_DMA_M2M_EXAMPLE_XFER_LEN] = { 0x0 };
uint8_t dma_dest_buf_ref[DRV_DMA_M2M_EXAMPLE_XFER_LEN]                                = { 0x0 };
wm_drv_dma_desc_t dma_desc[DRV_DMA_M2M_EXAMPLE_XFER_LIST_LEN]                         = { 0x0 };
static uint32_t dma_async_sts                                                         = 0x0;

void wm_drv_dma_example_ch_callback(wm_dma_ch_t ch, uint32_t sts, void *user_data)
{
    uint32_t magic  = *((uint32_t *)user_data);
    uint8_t *ck_buf = &dma_dest_buf[0][0];

    //TODO, just example for warp mode compare distinguish
    if (magic == DRV_DMA_M2M_EXAMPLE_INT_ASYNC_WARP_NONlIST) {
        if (!DRV_DMA_M2M_EXAMPLE_MEMCMP(&dma_dest_buf_ref[0], ck_buf, DRV_DMA_M2M_EXAMPLE_XFER_LEN)) {
            ;
        } else {
            DRV_DMA_M2M_EXAMPLE_LOG_INFO("dma m2m xfer demo finish with fail, contxt missmatch");
        }
    } else if (!DRV_DMA_M2M_EXAMPLE_MEMCMP(&dma_src_data[0][0], ck_buf, DRV_DMA_M2M_EXAMPLE_XFER_LEN)) {
        ;
    } else {
        DRV_DMA_M2M_EXAMPLE_LOG_INFO("dma m2m xfer demo finish with fail, contxt missmatch");
    }

    /* sem/mutex or just flag for example*/
    dma_async_sts++;
}

static void wm_drv_dma_example_m2m_interrupt_non_list(void)
{
    static wm_device_t *dma_dev       = NULL;
    wm_drv_dma_desc_t dma_example_cfg = {
        .src  = (uint32_t)&dma_src_data[0][0],
        .dest = (uint32_t)&dma_dest_buf[0][0],
    };
    static uint32_t magic = (uint32_t)DRV_DMA_M2M_EXAMPLE_INT_ASYNC_NONLIST;
    uint8_t dma_ch        = 0;

    /* TBD, src data value can random gen */
    DRV_DMA_M2M_EXAMPLE_MEMSET(&dma_src_data[0][0], 0x3a, sizeof(dma_src_data));
    DRV_DMA_M2M_EXAMPLE_MEMSET(&dma_dest_buf[0][0], 0x0, sizeof(dma_dest_buf));
    DRV_DMA_M2M_EXAMPLE_DUMP(WM_LOG_LEVEL_INFO, "[before dma]dma_m2m src data:", DRV_DMA_M2M_EXAMPLE_DUMP_WIDTH,
                             (void *)&dma_src_data[0][0], DRV_DMA_M2M_EXAMPLE_XFER_LEN);
    DRV_DMA_M2M_EXAMPLE_DUMP(WM_LOG_LEVEL_INFO, "[before dma]dma_m2m dest buf:", DRV_DMA_M2M_EXAMPLE_DUMP_WIDTH,
                             (void *)&dma_dest_buf[0][0], DRV_DMA_M2M_EXAMPLE_XFER_LEN);
    DRV_DMA_M2M_EXAMPLE_LOG_INFO("------start dma example: dma demo, dma interrupt without list mode ------");
    dma_dev = wm_drv_dma_init(DRV_DMA_M2M_EXAMPLE_DMA_NAME);
    wm_drv_dma_request_ch(dma_dev, &dma_ch, DRV_DMA_M2M_EXAMPLE_TO_US);
    wm_drv_dma_register_callback(dma_dev, dma_ch, wm_drv_dma_example_ch_callback, &magic);
    /* build dma desc */
    dma_example_cfg.ctrl.len           = DRV_DMA_M2M_EXAMPLE_XFER_LEN;
    dma_example_cfg.ctrl.ch            = dma_ch;
    dma_example_cfg.ctrl.src_inc_mode  = WM_DRV_DMA_ADDR_INC;
    dma_example_cfg.ctrl.dest_inc_mode = WM_DRV_DMA_ADDR_INC;
    dma_example_cfg.ctrl.int_en        = WM_DRV_DMA_CH_INT_ENABLE;
    dma_example_cfg.ctrl.auto_reload   = WM_DRV_DMA_RELOAD_DISABLE;
    dma_example_cfg.ctrl.dma_mode      = WM_DRV_DMA_NORMAL_MODE;
    //dma_example_cfg.ctrl.req_src = m2m not use;
    //dma_example_cfg.ctrl.src_warp_len = normal mode not use;
    //dma_example_cfg.ctrl.dest_warp_len = normal mode not use;
    dma_example_cfg.next = NULL;

    wm_log_info("src  0x%x", dma_example_cfg.src);
    wm_log_info("dest  0x%x", dma_example_cfg.dest);
    wm_log_info("len  %d", dma_example_cfg.ctrl.len);
    wm_log_info("ch  %d", dma_example_cfg.ctrl.ch);
    wm_log_info("src_inc_mode  %d", dma_example_cfg.ctrl.src_inc_mode);
    wm_log_info("dest_inc_mode  %d", dma_example_cfg.ctrl.dest_inc_mode);
    wm_log_info("int_en  %d", dma_example_cfg.ctrl.int_en);
    wm_log_info("auto_reload  %d", dma_example_cfg.ctrl.auto_reload);
    wm_log_info("dma_mode  %d", dma_example_cfg.ctrl.dma_mode);
    wm_log_info("src_warp_len  %d", dma_example_cfg.ctrl.src_warp_len);
    wm_log_info("dest_warp_len  %d", dma_example_cfg.ctrl.dest_warp_len);

    wm_drv_dma_config(dma_dev, &dma_example_cfg);
    dma_async_sts = 0;
    wm_drv_dma_start(dma_dev, dma_ch);

    while (!dma_async_sts) {
        wm_os_internal_time_delay_ms(100);
    }
    DRV_DMA_M2M_EXAMPLE_DUMP(WM_LOG_LEVEL_INFO, "[after dma]dma_m2m src data:", DRV_DMA_M2M_EXAMPLE_DUMP_WIDTH,
                             (void *)&dma_src_data[0][0], DRV_DMA_M2M_EXAMPLE_XFER_LEN);
    DRV_DMA_M2M_EXAMPLE_DUMP(WM_LOG_LEVEL_INFO, "[after dma]dma_m2m dest buf:", DRV_DMA_M2M_EXAMPLE_DUMP_WIDTH,
                             (void *)&dma_dest_buf[0][0], DRV_DMA_M2M_EXAMPLE_XFER_LEN);
    wm_drv_dma_stop(dma_dev, dma_ch);
    wm_drv_dma_deinit(dma_dev);
}

static void wm_drv_dma_example_m2m_polling_non_list(void)
{
    static wm_device_t *dma_dev       = NULL;
    wm_drv_dma_desc_t dma_example_cfg = {
        .src  = (uint32_t)&dma_src_data[0][0],
        .dest = (uint32_t)&dma_dest_buf[0][0],
    };

    wm_dma_sts_info_t dma_sts = {
        .sts      = WM_DMA_RUNNIG,
        .xfer_cnt = 0,
    };
    uint8_t dma_ch = 0;

    /* TBD, src data value can random gen */
    DRV_DMA_M2M_EXAMPLE_MEMSET(&dma_src_data[0][0], 0x3a, DRV_DMA_M2M_EXAMPLE_XFER_LEN);
    DRV_DMA_M2M_EXAMPLE_MEMSET(&dma_dest_buf[0][0], 0x0, DRV_DMA_M2M_EXAMPLE_XFER_LEN);
    DRV_DMA_M2M_EXAMPLE_DUMP(WM_LOG_LEVEL_INFO, "[before dma]dma_m2m src data:", DRV_DMA_M2M_EXAMPLE_DUMP_WIDTH,
                             (void *)&dma_src_data[0][0], DRV_DMA_M2M_EXAMPLE_XFER_LEN);
    DRV_DMA_M2M_EXAMPLE_DUMP(WM_LOG_LEVEL_INFO, "[before dma]dma_m2m dest buf:", DRV_DMA_M2M_EXAMPLE_DUMP_WIDTH,
                             (void *)&dma_dest_buf[0][0], DRV_DMA_M2M_EXAMPLE_XFER_LEN);
    DRV_DMA_M2M_EXAMPLE_LOG_INFO("------start dma example: dma demo, dma polling without list mode ------");
    dma_dev = wm_drv_dma_init(DRV_DMA_M2M_EXAMPLE_DMA_NAME);
    wm_drv_dma_request_ch(dma_dev, &dma_ch, DRV_DMA_M2M_EXAMPLE_TO_US);
    //wm_drv_dma_register_callback(dma_dev, dma_ch, wm_drv_dma_example_ch_callback, &magic);
    /* build dma desc */
    dma_example_cfg.ctrl.len           = DRV_DMA_M2M_EXAMPLE_XFER_LEN;
    dma_example_cfg.ctrl.ch            = dma_ch;
    dma_example_cfg.ctrl.src_inc_mode  = WM_DRV_DMA_ADDR_INC;
    dma_example_cfg.ctrl.dest_inc_mode = WM_DRV_DMA_ADDR_INC;
    dma_example_cfg.ctrl.int_en        = WM_DRV_DMA_CH_INT_DISABLE;
    dma_example_cfg.ctrl.auto_reload   = WM_DRV_DMA_RELOAD_DISABLE;
    dma_example_cfg.ctrl.dma_mode      = WM_DRV_DMA_NORMAL_MODE;
    //dma_example_cfg.ctrl.req_src = m2m not use;
    //dma_example_cfg.ctrl.src_warp_len = normal mode not use;
    //dma_example_cfg.ctrl.dest_warp_len = normal mode not use;
    dma_example_cfg.next = NULL;
    wm_drv_dma_config(dma_dev, &dma_example_cfg);
    wm_drv_dma_start(dma_dev, dma_ch);

    do {
        if (dma_sts.sts == WM_DMA_IDLE) {
            DRV_DMA_M2M_EXAMPLE_LOG_INFO("dma move to idle");
            break;
        }
    } while (wm_drv_dma_get_status(dma_dev, dma_ch, &dma_sts));

    DRV_DMA_M2M_EXAMPLE_DUMP(WM_LOG_LEVEL_INFO, "[after dma]dma_m2m src data:", DRV_DMA_M2M_EXAMPLE_DUMP_WIDTH,
                             (void *)&dma_src_data[0][0], DRV_DMA_M2M_EXAMPLE_XFER_LEN);
    DRV_DMA_M2M_EXAMPLE_DUMP(WM_LOG_LEVEL_INFO, "[after dma]dma_m2m dest buf:", DRV_DMA_M2M_EXAMPLE_DUMP_WIDTH,
                             (void *)&dma_dest_buf[0][0], DRV_DMA_M2M_EXAMPLE_XFER_LEN);
    if (!DRV_DMA_M2M_EXAMPLE_MEMCMP((void *)dma_example_cfg.src, (void *)dma_example_cfg.dest, DRV_DMA_M2M_EXAMPLE_XFER_LEN)) {
        DRV_DMA_M2M_EXAMPLE_LOG_INFO("dma m2m xfer demo finish with success");
    } else {
        DRV_DMA_M2M_EXAMPLE_LOG_INFO("dma m2m xfer demo finish with fail, contxt missmatch");
    }

    wm_drv_dma_stop(dma_dev, dma_ch);
    wm_drv_dma_deinit(dma_dev);
}

static void wm_drv_dma_example_m2m_interrupt_warp(void)
{
    static wm_device_t *dma_dev       = NULL;
    wm_drv_dma_desc_t dma_example_cfg = {
        .src  = (uint32_t)&dma_src_data[0][0],
        .dest = (uint32_t)&dma_dest_buf[0][0],
    };
    static uint32_t magic = (uint32_t)DRV_DMA_M2M_EXAMPLE_INT_ASYNC_WARP_NONlIST;
    uint8_t dma_ch        = 0;

    /* TBD, src data value can random gen */
    DRV_DMA_M2M_EXAMPLE_MEMSET(&dma_src_data[0][0], 0x0, sizeof(dma_src_data));
    DRV_DMA_M2M_EXAMPLE_MEMSET(&dma_src_data[0][0], 0x3a, DRV_DMA_M2M_EXAMPLE_XFER_LEN / 4);
    DRV_DMA_M2M_EXAMPLE_MEMSET(&dma_src_data[0][0] + DRV_DMA_M2M_EXAMPLE_XFER_LEN / 4, 0xee,
                               sizeof(dma_src_data) - DRV_DMA_M2M_EXAMPLE_XFER_LEN / 4);
    DRV_DMA_M2M_EXAMPLE_MEMSET(&dma_dest_buf[0][0], 0x0, sizeof(dma_dest_buf));
    DRV_DMA_M2M_EXAMPLE_MEMSET(&dma_dest_buf_ref[0], 0x0, sizeof(dma_dest_buf_ref));
    DRV_DMA_M2M_EXAMPLE_MEMSET(&dma_dest_buf_ref[0], 0xee, sizeof(dma_dest_buf_ref) / 4);
    DRV_DMA_M2M_EXAMPLE_DUMP(WM_LOG_LEVEL_INFO, "[before dma]dma_m2m src data:", DRV_DMA_M2M_EXAMPLE_DUMP_WIDTH,
                             (void *)&dma_src_data[0][0], DRV_DMA_M2M_EXAMPLE_XFER_LEN);
    DRV_DMA_M2M_EXAMPLE_DUMP(WM_LOG_LEVEL_INFO, "[before dma]dma_m2m dest buf:", DRV_DMA_M2M_EXAMPLE_DUMP_WIDTH,
                             (void *)&dma_dest_buf[0][0], DRV_DMA_M2M_EXAMPLE_XFER_LEN);
    DRV_DMA_M2M_EXAMPLE_DUMP(WM_LOG_LEVEL_INFO, "[before dma]dma_m2m dest buf ref:", DRV_DMA_M2M_EXAMPLE_DUMP_WIDTH,
                             (void *)&dma_dest_buf_ref[0], DRV_DMA_M2M_EXAMPLE_XFER_LEN);
    DRV_DMA_M2M_EXAMPLE_LOG_INFO("------start dma example: dma demo, dma interrupt with warp mode ------");
    dma_dev = wm_drv_dma_init(DRV_DMA_M2M_EXAMPLE_DMA_NAME);
    wm_drv_dma_request_ch(dma_dev, &dma_ch, DRV_DMA_M2M_EXAMPLE_TO_US);
    wm_drv_dma_register_callback(dma_dev, dma_ch, wm_drv_dma_example_ch_callback, &magic);
    /* build dma desc */
    dma_example_cfg.ctrl.len           = DRV_DMA_M2M_EXAMPLE_XFER_LEN;
    dma_example_cfg.ctrl.ch            = dma_ch;
    dma_example_cfg.ctrl.src_inc_mode  = WM_DRV_DMA_ADDR_WARP;
    dma_example_cfg.ctrl.dest_inc_mode = WM_DRV_DMA_ADDR_WARP;
    dma_example_cfg.ctrl.int_en        = WM_DRV_DMA_CH_INT_ENABLE;
    dma_example_cfg.ctrl.auto_reload   = WM_DRV_DMA_RELOAD_DISABLE;
    dma_example_cfg.ctrl.dma_mode      = WM_DRV_DMA_WARP_MODE;
    //dma_example_cfg.ctrl.req_src = m2m not use;
    dma_example_cfg.ctrl.src_warp_len  = DRV_DMA_M2M_EXAMPLE_XFER_LEN / 1;
    dma_example_cfg.ctrl.dest_warp_len = DRV_DMA_M2M_EXAMPLE_XFER_LEN / 4;
    dma_example_cfg.next               = NULL;
    wm_drv_dma_config(dma_dev, &dma_example_cfg);
    dma_async_sts = 0;
    wm_drv_dma_start(dma_dev, dma_ch);

    while (!dma_async_sts) {
        wm_os_internal_time_delay_ms(100);
    }

    DRV_DMA_M2M_EXAMPLE_DUMP(WM_LOG_LEVEL_INFO, "[after dma]dma_m2m src data:", DRV_DMA_M2M_EXAMPLE_DUMP_WIDTH,
                             (void *)&dma_src_data[0][0], DRV_DMA_M2M_EXAMPLE_XFER_LEN);
    DRV_DMA_M2M_EXAMPLE_DUMP(WM_LOG_LEVEL_INFO, "[after dma]dma_m2m dest buf:", DRV_DMA_M2M_EXAMPLE_DUMP_WIDTH,
                             (void *)&dma_dest_buf[0][0], DRV_DMA_M2M_EXAMPLE_XFER_LEN);

    wm_drv_dma_stop(dma_dev, dma_ch);
    wm_drv_dma_deinit(dma_dev);
}

static void wm_drv_dma_example_m2m_transfer(void)
{
    static wm_device_t *dma_dev           = NULL;
    wm_drv_dma_xfer_cfg_t dma_example_cfg = { 0 };
    static uint32_t magic                 = (uint32_t)DRV_DMA_M2M_EXAMPLE_INT_ASYNC_NONLIST;
    uint8_t dma_ch                        = 0;

    /* TBD, src data value can random gen */
    DRV_DMA_M2M_EXAMPLE_MEMSET(&dma_src_data[0][0], 0x3a, sizeof(dma_src_data));
    DRV_DMA_M2M_EXAMPLE_MEMSET(&dma_dest_buf[0][0], 0x0, sizeof(dma_dest_buf));
    DRV_DMA_M2M_EXAMPLE_DUMP(WM_LOG_LEVEL_INFO, "[before dma]dma_m2m src data:", DRV_DMA_M2M_EXAMPLE_DUMP_WIDTH,
                             (void *)&dma_src_data[0][0], DRV_DMA_M2M_EXAMPLE_XFER_LEN);
    DRV_DMA_M2M_EXAMPLE_DUMP(WM_LOG_LEVEL_INFO, "[before dma]dma_m2m dest buf:", DRV_DMA_M2M_EXAMPLE_DUMP_WIDTH,
                             (void *)&dma_dest_buf[0][0], DRV_DMA_M2M_EXAMPLE_XFER_LEN);
    DRV_DMA_M2M_EXAMPLE_LOG_INFO("------start dma example: dma demo, dma interrupt without list mode transfer api------");
    dma_dev = wm_drv_dma_init(DRV_DMA_M2M_EXAMPLE_DMA_NAME);
    wm_drv_dma_request_ch(dma_dev, &dma_ch, DRV_DMA_M2M_EXAMPLE_TO_US);
    wm_drv_dma_register_callback(dma_dev, dma_ch, wm_drv_dma_example_ch_callback, &magic);
    /* build dma desc */
    dma_example_cfg.src = (uint32_t)&dma_src_data[0][0], dma_example_cfg.dest = (uint32_t)&dma_dest_buf[0][0],
    dma_example_cfg.len         = DRV_DMA_M2M_EXAMPLE_XFER_LEN;
    dma_example_cfg.auto_reload = WM_DRV_DMA_RELOAD_DISABLE;
    dma_async_sts               = 0;
    wm_drv_dma_transfer(dma_dev, dma_ch, &dma_example_cfg);

    while (!dma_async_sts) {
        wm_os_internal_time_delay_ms(100);
    }

    DRV_DMA_M2M_EXAMPLE_DUMP(WM_LOG_LEVEL_INFO, "[after dma]dma_m2m src data:", DRV_DMA_M2M_EXAMPLE_DUMP_WIDTH,
                             (void *)&dma_src_data[0][0], DRV_DMA_M2M_EXAMPLE_XFER_LEN);
    DRV_DMA_M2M_EXAMPLE_DUMP(WM_LOG_LEVEL_INFO, "[after dma]dma_m2m dest buf:", DRV_DMA_M2M_EXAMPLE_DUMP_WIDTH,
                             (void *)&dma_dest_buf[0][0], DRV_DMA_M2M_EXAMPLE_XFER_LEN);

    wm_drv_dma_stop(dma_dev, dma_ch);
    wm_drv_dma_deinit(dma_dev);
}

void wm_drv_dma_m2m_demo(void *p)
{
    wm_device_t *dev = wm_drv_dma_init("dma");
    if (dev) {
        wm_log_info("drv dma mem <-----> mem transfer demo, with different dma feature combinations\n");
        uint32_t count = 0;

        while (1) {
            /* dma 1st demo - pass */
            wm_drv_dma_example_m2m_interrupt_non_list();

            /* dma 2nd demo - pass  */
            wm_drv_dma_example_m2m_polling_non_list();

            /* dma 3rd demo - pass */
            wm_drv_dma_example_m2m_interrupt_warp();

            /* dma 4th demo - pass */
            wm_drv_dma_example_m2m_transfer();

            wm_log_info("dma demo done! count %d ", count++);
            vTaskDelay(pdMS_TO_TICKS(1000));
        }

    } else {
        wm_log_error("dma dev error");
    }
    vTaskDelete(NULL);
}

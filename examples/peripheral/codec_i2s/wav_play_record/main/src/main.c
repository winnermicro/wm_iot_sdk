/**
 * @file main.c
 *
 * @brief I2S_CODEC demo
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
#include <ctype.h>
#include "wm_list.h"
#include "wm_soc_cfgs.h"
#include "wmsdk_config.h"
#include "wm_error.h"
#include "wm_osal.h"
#include "wm_types.h"
#include "wm_task_config.h"
#include "wm_drv_i2s.h"
#include "wm_drv_codec_i2s.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "wm_diskio.h"
#include "ff.h"

#include "wm_wav.h"

#define LOG_TAG "app_i2s_codec"
#include "wm_log.h"

enum {
    WM_APP_CODEC_WORK_TYPE_PLAYBACK            = 1, /**< play wav file in sdcard   */
    WM_APP_CODEC_WORK_TYPE_RECORD              = 2, /**< record wav file to sdcard */
    WM_APP_CODEC_WORK_TYPE_PLAYBACK_AND_RECORD = 3, /**< play wav file in sdcard and record wav file to sdcard */
};

/**< example work mode */
#define WM_APP_CODEC_WORK_TYPE (WM_APP_CODEC_WORK_TYPE_PLAYBACK_AND_RECORD)

/**< Play file name. */
#define WM_PLAYBCCK_WAV_FILE   "16B_16K.WAV"

/**< record file name. */
#define WM_RECORD_WAV_FILE     "record.wav"

#define WM_I2C_DEV_NAME        "i2c"
#define WM_I2S_DEV_NAME        "i2s"
#define WM_CODEC_I2S_DEV_NAME  "es8374"

#define WM_APP_I2S_TX_PKT_SIZE 1200
#define WM_APP_I2S_RX_PKT_SIZE 1200
#define WM_APP_I2S_TX_PKT_NUM  6
#define WM_APP_I2S_RX_PKT_NUM  6

/**< Play and record task status. */
typedef enum {
    WM_APP_I2S_TASK_IDEL   = 0,
    WM_APP_I2S_TASK_RUNING = 1,
    WM_APP_I2S_TASK_BUSY   = 2,
} wm_codec_i2s_task_state_t;

static wm_device_t *g_codec = NULL; /**< codec device handle */
static wm_device_t *g_i2s   = NULL; /**< i2s device handle   */
static FATFS *g_fs          = NULL; /**< FAT fs handle       */

static QueueHandle_t i2s_rx_queue = NULL; /**< rx event queue */
static QueueHandle_t i2s_tx_queue = NULL; /**< tx event queue */

static wm_codec_i2s_task_state_t g_play_status   = WM_APP_I2S_TASK_IDEL;
static wm_codec_i2s_task_state_t g_record_status = WM_APP_I2S_TASK_IDEL;

/**< SD card mount base path */
static char g_base_path[8];
static wm_drv_i2s_format_t g_play_fmt;
static int g_work_type = WM_APP_CODEC_WORK_TYPE;

static int write_callback(wm_device_t *dev, wm_drv_i2s_event_t *event)
{
    portBASE_TYPE pxHigherPriorityTaskWoken = pdFALSE;

    if (event->len > 0) {
        /*
        Send the TX event to other task to proceess.

        The callback is in the interrupt context.
        It will block the whole system and lost data if too much things is proceessed in the interrupt.
        */
        xQueueSendFromISR(i2s_tx_queue, event, &pxHigherPriorityTaskWoken);

        if ((pdTRUE == pxHigherPriorityTaskWoken)) {
            /*wake up the higher task after iterrupt end*/
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
        }
    }
    return WM_ERR_SUCCESS;
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

static void resample_to_16bits(uint8_t *pbuf, int len, int bits)
{
    uint8_t *p8s = NULL;
    uint8_t *p8d = NULL;
    int16_t *p16 = NULL;
    uint8_t a, b;

    if (bits == 8) {
        p8s = pbuf + (len - 1);
        p16 = ((int16_t *)pbuf) + (len - 1);

        for (; p8s >= pbuf;) {
            *p16 = (((int16_t)(*p8s)) - 128) * 256;
            p8s--;
            p16--;
        }
    }
    if (bits == 24) {
        p8s = pbuf;
        p8d = pbuf;

        for (; len > 0;) {
            a      = p8s[1];
            b      = p8s[2];
            p8d[0] = a;
            p8d[1] = b;
            len -= 3;
            p8s += 3;
            p8d += 2;
        }
    }
}

static int mount_sdcard(void)
{
    FRESULT res;

    if (!g_fs) {
        g_fs = calloc(1, sizeof(*g_fs));
        if (!g_fs) {
            return WM_ERR_FAILED;
        }
    }

    sprintf(g_base_path, "%d:", WM_DISKIO_DRIVER_NUM_SDMMC);
    res = f_mount(g_fs, g_base_path, 1);
    if (res != FR_OK) {
        wm_log_error("mount sdcard fail. res=%d", res);
        free(g_fs);
        g_fs = NULL;
        return WM_ERR_FAILED;
    } else {
        wm_log_info("mount sdcard to %s, fs_type=%d", g_base_path, g_fs->fs_type);
        return WM_ERR_SUCCESS;
    }
}

static int unmout_sdcard(void)
{
    FRESULT res;

    if (g_fs != NULL) {
        res = f_mount(g_fs, g_base_path, 0);
        if (res != FR_OK) {
            wm_log_error("Failed to unmount file system.");
        }
        g_fs = NULL;
        return WM_ERR_SUCCESS;
    } else {
        wm_log_error("not mount before");
        return WM_ERR_FAILED;
    }
}

static int read_wav_and_write_to_i2s(void *f_wav, uint8_t *prx_buf_cur, int bits)
{
    int err;
    int len;
    int max_read_len = WM_APP_I2S_TX_PKT_SIZE;

    if (bits == 8) {
        max_read_len /= 2;
    }

    len = wm_wav_read_pull(f_wav, prx_buf_cur, max_read_len);
    if (len <= 0) {
        wm_log_error("read end");
        return WM_ERR_FAILED;
    }

    if (bits == 8 || bits == 24) {
        resample_to_16bits(prx_buf_cur, len, bits);

        if (bits == 8) {
            len *= 2;
        } else {
            len = len * 2 / 3;
        }
    }

    err = wm_drv_i2s_write_async(g_i2s, prx_buf_cur, len);

    return err;
}

static int codec_i2s_init(void)
{
    int err;

    wm_drv_i2s_cfg_t i2s_cfg = {
        .mode        = WM_I2S_MODE_MASTER,
        .dir         = WM_I2S_DIR_INOUT,
        .std         = WM_I2S_STD_I2S,
        .xtype       = WM_I2S_XFER_DMA,
        .rx_pkt_size = WM_APP_I2S_RX_PKT_SIZE,
        .rx_pkt_num  = WM_APP_I2S_RX_PKT_NUM,
        .tx_pkt_num  = WM_APP_I2S_TX_PKT_NUM,
    };

    wm_drv_codec_i2s_cfg_t codec_cfg = {
        .mode = WM_I2S_MODE_SLAVE,
        .std  = WM_I2S_STD_I2S,
        .type = WM_DRV_CODEC_I2S_MODE_INOUT,
    };

    if (g_work_type == WM_APP_CODEC_WORK_TYPE_PLAYBACK) {
        i2s_cfg.dir    = WM_I2S_DIR_OUT;
        codec_cfg.type = WM_DRV_CODEC_I2S_MODE_OUT;
    } else if (g_work_type == WM_APP_CODEC_WORK_TYPE_RECORD) {
        i2s_cfg.dir    = WM_I2S_DIR_IN;
        codec_cfg.type = WM_DRV_CODEC_I2S_MODE_IN;
    } else if (g_work_type == WM_APP_CODEC_WORK_TYPE_PLAYBACK_AND_RECORD) {
        i2s_cfg.dir    = WM_I2S_DIR_INOUT;
        codec_cfg.type = WM_DRV_CODEC_I2S_MODE_INOUT;
    } else {
        wm_log_error("bad work type %d", g_work_type);
        return WM_ERR_FAILED;
    }

    /* codec use i2s type, so initialize it*/
    wm_drv_i2c_init(WM_I2C_DEV_NAME);

    /* init codec */
    g_codec = wm_drv_codec_i2s_init(WM_CODEC_I2S_DEV_NAME, &codec_cfg);
    if (!g_codec) {
        wm_log_error("init %s fail.", WM_CODEC_I2S_DEV_NAME);
        return WM_ERR_FAILED;
    }

    /* start codec */
    err = wm_drv_codec_i2s_start(g_codec);
    if (err != WM_ERR_SUCCESS) {
        wm_log_error("start %s fail.", WM_CODEC_I2S_DEV_NAME);
        return WM_ERR_FAILED;
    }

    /* init i2s */
    g_i2s = wm_drv_i2s_init(WM_I2S_DEV_NAME, &i2s_cfg);
    if (!g_i2s) {
        wm_log_error("init %s fail.", WM_I2S_DEV_NAME);
        return WM_ERR_FAILED;
    } else {
        wm_log_info("codec " WM_CODEC_I2S_DEV_NAME "start OK, mode=%d,dir=%d,std=%d,rx_pkt_size=%u,rx_pkt_num=%u,tx_pkt_num=%u",
                    i2s_cfg.mode, i2s_cfg.dir, i2s_cfg.std, i2s_cfg.rx_pkt_size, i2s_cfg.rx_pkt_num, i2s_cfg.tx_pkt_num);
    }

    return WM_ERR_SUCCESS;
}

static void playback_task(void *parameters)
{
    int err;
    uint8_t *pbuf = NULL;
    void *f_wav   = NULL;
    wm_wav_info_t wav_info;
    wm_drv_i2s_event_t event;
    char pathfile[48];
    int tx_buf_index = 0;
    enum wm_i2s_bits bits;

    snprintf(pathfile, sizeof(pathfile), "%s%s", g_base_path, (char *)parameters);

    /*open wav file*/
    f_wav = wm_wav_read_open(pathfile, &wav_info);
    if (!f_wav) {
        wm_log_error("Failed to open %s", pathfile);
        goto END;
    }

    /*malloc write buffer for read pcm data from wav file*/
    pbuf = malloc(WM_APP_I2S_TX_PKT_SIZE * WM_APP_I2S_TX_PKT_NUM);
    if (!pbuf) {
        wm_log_error("malloc send buf fail, size=%d", WM_APP_I2S_TX_PKT_SIZE);
        goto END;
    }

    /*Register TX done event callback*/
    wm_drv_i2s_register_write_cb(g_i2s, write_callback);

    /*set playback volume*/
    err = wm_drv_codec_i2s_set_volume(g_codec, 80);
    if (err != WM_ERR_SUCCESS) {
        wm_log_error("set volume fail");
        goto END;
    }

    /*set playback unmute*/
    err = wm_drv_codec_i2s_set_mute(g_codec, false);
    if (err != WM_ERR_SUCCESS) {
        wm_log_error("set mute fail");
        goto END;
    }

    /* driver only support 16 bits and 32 bits*/
    bits = (wav_info.bitsWidth == 32 ? WM_I2S_BITS_32BIT : WM_I2S_BITS_16BIT);

    g_play_fmt.sample_rate_hz = wav_info.eSampleRate;
    g_play_fmt.bits           = bits;

    if (wav_info.channels == 2) {
        g_play_fmt.ctype = WM_I2S_CHAN_TYPE_STEREO;
    } else if (wav_info.channels == 1) {
        g_play_fmt.ctype = WM_I2S_CHAN_TYPE_MONO_LEFT;
    } else {
        wm_log_error("bad volume channel, num=%d", wav_info.channels);
        goto END;
    }

    /* set pcm data format*/
    err = wm_drv_codec_i2s_set_format(g_codec, wav_info.eSampleRate, bits, g_play_fmt.ctype);
    if (err != WM_ERR_SUCCESS) {
        wm_log_error("codec set format fail");
        goto END;
    }

    /* start codec i2s*/
    err = wm_drv_codec_i2s_start(g_codec);
    if (err != WM_ERR_SUCCESS) {
        wm_log_error("codec start fail");
        goto END;
    }

    /*create queue for write data end event*/
    i2s_tx_queue = xQueueCreate(WM_APP_I2S_TX_PKT_NUM, sizeof(wm_drv_i2s_event_t));
    if (!i2s_tx_queue) {
        wm_log_error("create queue fail.");
        goto END;
    }

    wm_log_info("start play");

    g_play_status = WM_APP_I2S_TASK_RUNING;

    /*start write data to i2s*/
    for (int i = 0; i < WM_APP_I2S_TX_PKT_NUM; i++) {
        if (read_wav_and_write_to_i2s(f_wav, pbuf + (tx_buf_index * WM_APP_I2S_TX_PKT_SIZE), wav_info.bitsWidth) ==
            WM_ERR_SUCCESS) {
            tx_buf_index++;
            tx_buf_index %= WM_APP_I2S_TX_PKT_NUM;
        }
    }

    while (g_play_status == WM_APP_I2S_TASK_RUNING) {
        /*wait write done event*/
        if (xQueueReceive(i2s_tx_queue, &event, 1000) == pdPASS) {
            /*write data to i2s*/
            if (read_wav_and_write_to_i2s(f_wav, pbuf + (tx_buf_index * WM_APP_I2S_TX_PKT_SIZE), wav_info.bitsWidth) ==
                WM_ERR_SUCCESS) {
                tx_buf_index++;
                tx_buf_index %= WM_APP_I2S_TX_PKT_NUM;
            } else {
                /*error*/
                wm_log_error("write err=%d,exit!", err);
                break;
            }
        }
    }

    /*write stop*/
    wm_drv_i2s_write_stop(g_i2s);
    wm_drv_i2s_register_write_cb(g_i2s, NULL);

    wm_log_info("");

END:
    /*cose wave file*/
    if (f_wav) {
        wm_wav_read_close(f_wav);
    }

    /*free write buffer*/
    if (pbuf) {
        free(pbuf);
    }

    /*free receive event queue*/
    if (i2s_tx_queue) {
        vQueueDelete(i2s_tx_queue);
        i2s_tx_queue = NULL;
    }

    g_play_status = WM_APP_I2S_TASK_IDEL;

    wm_log_info("play end.");

    vTaskDelete(NULL);
}

static void record_task(void *parameters)
{
    int i;
    int err;

    uint8_t *pbuf = NULL;
    void *f_wav   = NULL;

    /**< use same format to playback */
    wm_wav_info_t wav_info = {
        .eAencType   = WM_CODE_LPCM,
        .eSampleRate = g_play_fmt.sample_rate_hz,
        .bitsWidth   = g_play_fmt.bits,
        .channels    = (g_play_fmt.ctype == WM_I2S_CHAN_TYPE_STEREO ? 2 : 1),
    };

    char pathfile[48];
    wm_drv_i2s_event_t event;
    uint32_t total_size = 0;

    g_record_status = WM_APP_I2S_TASK_RUNING;

    snprintf(pathfile, sizeof(pathfile), "%s%s", g_base_path, WM_RECORD_WAV_FILE);

    /**< open wav file to write record pcm data */
    f_wav = wm_wav_write_open(pathfile, &wav_info);
    if (!f_wav) {
        wm_log_error("Failed to open %s", pathfile);
        goto END;
    }

    /*malloc read buffer*/
    pbuf = malloc(WM_APP_I2S_RX_PKT_SIZE * WM_APP_I2S_RX_PKT_NUM);
    if (!pbuf) {
        wm_log_error("malloc recv buf fail, size=%d", WM_APP_I2S_RX_PKT_SIZE * WM_APP_I2S_RX_PKT_NUM);
        goto END;
    }

    /*create queue for read event*/
    i2s_rx_queue = xQueueCreate(WM_APP_I2S_RX_PKT_NUM, sizeof(wm_drv_i2s_event_t));
    if (!i2s_rx_queue) {
        wm_log_error("create queue fail.");
        goto END;
    }

    /*Register rx done event callback*/
    wm_drv_i2s_register_read_cb(g_i2s, read_callback);

    /*unmute mic*/
    err = wm_drv_codec_i2s_set_mic_mute(g_codec, false);
    if (err != WM_ERR_SUCCESS) {
        wm_log_error("set mic mute fail");
        goto END;
    }

    /*set mic volume*/
    err = wm_drv_codec_i2s_set_mic_volume(g_codec, 100);
    if (err != WM_ERR_SUCCESS) {
        wm_log_error("set mic volume fail");
        goto END;
    }

    /*start codec*/
    err = wm_drv_codec_i2s_start(g_codec);
    if (err != WM_ERR_SUCCESS) {
        wm_log_error("codec start fail");
        goto END;
    }

    wm_log_info("start record");

    /*append buffer to receive*/
    for (i = 0; i < WM_APP_I2S_RX_PKT_NUM; i++) {
        wm_drv_i2s_read_async(g_i2s, pbuf + i * WM_APP_I2S_RX_PKT_SIZE, WM_APP_I2S_RX_PKT_SIZE);
    }

    while (g_record_status == WM_APP_I2S_TASK_RUNING) {
        /*receive record data ready event*/
        if (xQueueReceive(i2s_rx_queue, &event, 1000) == pdPASS) {
            if (event.type == WM_DRV_I2S_EVENT_RX_READY) {
                if (event.buf && event.len > 0) {
                    /*write receive data to wav file*/
                    wm_wav_write_put(f_wav, event.buf, event.len);
                    total_size += event.len;
                    if (total_size > (5 * 1024 * 1024)) {
                        wm_log_info("up to 5M, stop");
                        break;
                    }
                }
                /*append buffer to receive*/
                wm_drv_i2s_read_async(g_i2s, event.buf, WM_APP_I2S_RX_PKT_SIZE);
            } else if (event.type == WM_DRV_I2S_EVENT_RX_DONE) {
                wm_log_info("RX done");
            }
        }
    }

    /*stop read*/
    wm_drv_i2s_read_stop(g_i2s);
    wm_drv_i2s_register_read_cb(g_i2s, NULL);

END:
    /*close write file*/
    if (f_wav) {
        wm_wav_write_close(f_wav);
    }

    /*free receive buffer*/
    if (pbuf) {
        free(pbuf);
    }

    /*free receive event queue*/
    if (i2s_rx_queue) {
        vQueueDelete(i2s_rx_queue);
        i2s_rx_queue = NULL;
    }

    g_record_status = WM_APP_I2S_TASK_IDEL;

    wm_log_info("record end. size=%u", total_size);

    vTaskDelete(NULL);
}

int main(void)
{
    int err;

    wm_log_info("start wav play record example.");

    /*mount SD card, some SD cards write slowly, which may cause incomplete recording data */
    err = mount_sdcard();
    if (err != WM_ERR_SUCCESS) {
        return WM_ERR_FAILED;
    }

    /*codec and i2s init*/
    err = codec_i2s_init();
    if (err != WM_ERR_SUCCESS) {
        return WM_ERR_FAILED;
    }

    if (g_work_type == WM_APP_CODEC_WORK_TYPE_PLAYBACK_AND_RECORD || g_work_type == WM_APP_CODEC_WORK_TYPE_PLAYBACK) {
        /*start playback task*/
        xTaskCreate(playback_task, "player", 1024, WM_PLAYBCCK_WAV_FILE, 5, NULL);
    }

    if (g_work_type == WM_APP_CODEC_WORK_TYPE_PLAYBACK_AND_RECORD) {
        /*wait playback running, so the record task use same sample,bits and channels to the playback task*/
        while (g_play_status != WM_APP_I2S_TASK_RUNING) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }

    if (g_work_type == WM_APP_CODEC_WORK_TYPE_PLAYBACK_AND_RECORD || g_work_type == WM_APP_CODEC_WORK_TYPE_RECORD) {
        /*start record task*/
        xTaskCreate(record_task, "record", 1024, NULL, 4, NULL);
    }

    while (g_play_status != WM_APP_I2S_TASK_IDEL || g_record_status != WM_APP_I2S_TASK_IDEL) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    unmout_sdcard();

    wm_log_info("end wav play record.");

    return 0;
}

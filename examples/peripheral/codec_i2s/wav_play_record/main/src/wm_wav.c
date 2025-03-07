#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "wmsdk_config.h"

#include "wm_wav.h"
#include "ff.h"

#define LOG_TAG "wav"
#include "wm_log.h"

typedef struct {
    signed short wFormatTag;
    signed short wChannels;
    unsigned int dwSamplesPerSec;
    unsigned int dwAvgBytesPerSec;
    signed short wBlockAlign;
    signed short wBitsPerSample;

} wm_wav_format_t;

typedef struct {
    char chRIFF[4];
    unsigned int dwRIFFLen;
    char chWAVE[4];
    char chFMT[4];
    unsigned int dwFMTLen;
    wm_wav_format_t wave;
    char chDATA[4];
    unsigned int dwDATALen;

} wm_wav_file_header_t;

typedef struct {
    char item_name[4];
    unsigned int item_len;
} wm_wav_desc_item_t;

typedef struct {
    FIL fil;
    FIL *fp;
    uint32_t len;
    wm_wav_info_t wave_info;

} wm_wav_rw_info_t;

static int wm_wav_addWaveHeader(wm_wav_file_header_t *tWavHead, wm_wav_code_t code, int channels, int sample_rate,
                                int bitsWidth, int raw_len)
{
    memset(tWavHead, 0, sizeof(*tWavHead));

    tWavHead->chRIFF[0] = 'R';
    tWavHead->chRIFF[1] = 'I';
    tWavHead->chRIFF[2] = 'F';
    tWavHead->chRIFF[3] = 'F';

    tWavHead->chWAVE[0] = 'W';
    tWavHead->chWAVE[1] = 'A';
    tWavHead->chWAVE[2] = 'V';
    tWavHead->chWAVE[3] = 'E';

    tWavHead->chFMT[0] = 'f';
    tWavHead->chFMT[1] = 'm';
    tWavHead->chFMT[2] = 't';
    tWavHead->chFMT[3] = 0x20;
    tWavHead->dwFMTLen = 0x10;

    if (channels == 1) {
        tWavHead->wave.wChannels = 0x01;
    } else {
        tWavHead->wave.wChannels = 0x02;
    }

    tWavHead->wave.wFormatTag      = 0x1;
    tWavHead->wave.wBitsPerSample  = bitsWidth;
    tWavHead->wave.dwSamplesPerSec = sample_rate;
    tWavHead->wave.dwAvgBytesPerSec =
        (tWavHead->wave.wBitsPerSample * tWavHead->wave.dwSamplesPerSec * tWavHead->wave.wChannels) / 8;
    tWavHead->wave.wBlockAlign = 1024;

    tWavHead->chDATA[0] = 'd';
    tWavHead->chDATA[1] = 'a';
    tWavHead->chDATA[2] = 't';
    tWavHead->chDATA[3] = 'a';
    tWavHead->dwDATALen = raw_len;
    tWavHead->dwRIFFLen = raw_len + sizeof(wm_wav_file_header_t) - 8;

    return 0;
}

void *wm_wav_write_open(const char *file, wm_wav_info_t *wave_info)
{
    wm_wav_rw_info_t *p = NULL;
    wm_wav_file_header_t fileHeader;
    FRESULT res;
    UINT bw;

    if (!(file && wave_info)) {
        return NULL;
    }

    p = calloc(1, sizeof(*p));
    if (!p) {
        return NULL;
    }

    p->fp        = &p->fil;
    p->wave_info = *wave_info;

    res = f_open(p->fp, file, FA_CREATE_ALWAYS | FA_WRITE);
    if (res != FR_OK) {
        wm_log_error("Failed to open file for writing: %d", res);
        free(p);
        return NULL;
    }

    wm_wav_addWaveHeader(&fileHeader, p->wave_info.eAencType, p->wave_info.channels, p->wave_info.eSampleRate,
                         p->wave_info.bitsWidth, 0);

    res = f_write(p->fp, &fileHeader, sizeof(fileHeader), &bw);
    if (res != FR_OK) {
        f_close(p->fp);
        free(p);
        p = NULL;
    }

    return p;
}

int wm_wav_write_put(void *handle, void *buf, int size)
{
    wm_wav_rw_info_t *p = handle;
    int write_len       = 0;
    FRESULT res;
    UINT bw;

    if (p && buf && size > 0) {
        uint8_t *ptmp = buf;
        int len;

        while (size > 0) {
            len = (size > (1 * 8192) ? (1 * 8192) : size);

            res = f_write(p->fp, ptmp, len, &bw);

            if (res == FR_OK) {
                write_len = bw;
                p->len += write_len;

                ptmp += len;
                size -= len;
            } else {
                break;
            }
        }
    }

    return write_len;
}

void wm_wav_write_close(void *handle)
{
    wm_wav_rw_info_t *p = handle;
    wm_wav_file_header_t fileHeader;
    UINT bw;

    if (p) {
        if (p->len > 0) {
            wm_wav_addWaveHeader(&fileHeader, p->wave_info.eAencType, p->wave_info.channels, p->wave_info.eSampleRate,
                                 p->wave_info.bitsWidth, p->len);
            f_lseek(p->fp, 0);
            f_write(p->fp, &fileHeader, sizeof(fileHeader), &bw);
        }

        f_close(p->fp);

        free(p);
    }
}

void *wm_wav_read_open(const char *file, wm_wav_info_t *wave_info)
{
    wm_wav_rw_info_t *p = NULL;
    wm_wav_file_header_t wave_head;
    wm_wav_desc_item_t item;
    FRESULT res;
    UINT br;
    int offset = 0;

    if (!(file && wave_info)) {
        return NULL;
    }

    p = calloc(1, sizeof(*p));
    if (!p) {
        return NULL;
    }

    p->fp = &p->fil;

    res = f_open(p->fp, file, FA_READ);
    if (res != FR_OK) {
        wm_log_error("Fail open %s", file);
        free(p);
        return NULL;
    }

    res = f_read(p->fp, &wave_head, sizeof(wave_head) - sizeof(wm_wav_desc_item_t), &br);
    if (res != FR_OK || br == 0) {
        wm_log_error("Failed to read from file: %d", res);
        f_close(p->fp);
        free(p);
        return NULL;
    }

    offset = sizeof(wave_head) - sizeof(wm_wav_desc_item_t);

    p->wave_info.eAencType   = wave_head.wave.wFormatTag;
    p->wave_info.channels    = wave_head.wave.wChannels;
    p->wave_info.bitsWidth   = wave_head.wave.wBitsPerSample;
    p->wave_info.eSampleRate = wave_head.wave.dwSamplesPerSec;

    *wave_info = p->wave_info;

    wm_log_info("read fmt len=%u", wave_head.dwFMTLen);

    if (wave_head.dwFMTLen > sizeof(wm_wav_format_t)) {
        offset = 12 + (8 + wave_head.dwFMTLen);
        f_lseek(p->fp, offset); /*skip riff and fmt*/
    }

    /*read items*/
    while (1) {
        res = f_read(p->fp, &item, sizeof(item), &br);
        if (res != FR_OK || br == 0) {
            wm_log_error("Failed to read from file: %d", res);
            f_close(p->fp);
            free(p);
            return NULL;
        }

        wm_log_info("read %.*s,len=%u", 4, item.item_name, item.item_len);

        if (!strncmp(item.item_name, "data", 4)) {
            /*read to raw data, end*/
            break;
        }

        /*goto next item*/
        offset += sizeof(item) + item.item_len;
        f_lseek(p->fp, offset); /*skip riff and fmt*/
    }

    wm_log_info("open %s: code=%d,chs=%d,bits=%d,eSampleRate=%d,offset=%d", file, p->wave_info.eAencType,
                wave_head.wave.wChannels, wave_head.wave.wBitsPerSample, wave_head.wave.dwSamplesPerSec, offset);

    return p;
}

int wm_wav_read_pull(void *handle, unsigned char buf[], int size)
{
    FRESULT res;
    wm_wav_rw_info_t *p = handle;
    int read_len        = 0;

    if (p && p->fp) {
        res = f_read(p->fp, buf, size, (UINT *)&read_len);
        if (res != FR_OK) {
            read_len = -1;
        }
    }

    return read_len;
}

int wm_wav_read_close(void *handle)
{
    wm_wav_rw_info_t *p = handle;
    int read_len        = 0;

    if (p) {
        if (p->fp) {
            f_close(p->fp);
            p->fp = NULL;
        }
        free(p);
    }

    return read_len;
}

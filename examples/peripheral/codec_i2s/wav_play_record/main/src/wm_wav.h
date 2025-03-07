
#ifndef __WM_WAV_H__
#define __WM_WAV_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    WM_CODE_LPCM   = 0x01,
    WM_CODE_G711A  = 0x06,
    WM_CODE_G711U  = 0x07,
    WM_CODE_G726   = 0x045,
    WM_CODE_UNKOWN = 0xff,
} wm_wav_code_t;

typedef struct {
    wm_wav_code_t eAencType;
    int eSampleRate;
    int bitsWidth;
    int channels;
} wm_wav_info_t;

void *wm_wav_write_open(const char *file, wm_wav_info_t *wave_info);

int wm_wav_write_put(void *handle, void *buf, int size);

void wm_wav_write_close(void *handle);

void *wm_wav_read_open(const char *file, wm_wav_info_t *wave_info);

int wm_wav_read_pull(void *handle, unsigned char buf[], int size);

int wm_wav_read_close(void *handle);

#ifdef __cplusplus
}
#endif

#endif

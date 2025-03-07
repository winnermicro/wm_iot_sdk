# I2S Write

## Functional Overview

Once this application is launched, it performs the following operations:

1. mount SD card

2. Initialize Codec and I2S

3. Start the wav file playback task

(1) Open wav file；

(2) Prepare I2S to write buffer;

(3) Read the data to the buffer. If it is not 16 bits or 32 bits, resample to 16 bits;

(4) Write it out through I2S cycle and transmit it to CODEC module；

(5) End the task when the play is completed.

4. Start recording and end recording after recording to 10M.

(1) Open wav file;

(2) Prepare I2S read data buffer;

(3) Write the data read from I2S to the wav file；

(4) When recording to 5M, end the recording task.

## Environmental Requirements

1. The demo uses an EVB board
2. The default GPIO used is configured in the device table

1. Demo pin link requirement

```
┌─────────────────┐           ┌──────────────────────────┐
│    w80x         │           │          codec 8374      │
│                 │           │                          │
│      I2S_MCLK   │──────────►│ MCLK                     │
│                 │           │                          │
│     I2S_BCLK    ├──────────►│ BCLK                     |
│                 │           │                          │
│    I2S_LRCLK    ├──────────►│ LRCLK                    |
│                 │           │                          │
│        I2S_DI   ├──────────►│ ASDOUT                   │
│                 │           │                          │
│        I2S_DO   │◄──────────┤ DSDIN                    │
│                 │           │                          │
│        PA1      ├──────────►│ CCLK                     │
│                 │           │                          │
│        PA4      ├──────────►│ CDATA                    │
│                 │           │                          │
│        GND      ├───────────┤ GND                      │
└─────────────────┘           └──────────────────────────┘
```

2. Demo pin configuration

Different development boards may have different chip models, so you need to configure the pins in the device table to match the borad.

3. other configurations

In the example, you need to use SD card and FAT file system, as well as I2C, I2S module, The configurations of these modules have been turned on  in the file prj.config.

The SD card shoud use FAT file system, and a wav file named 16B_16K.wav need be placed in the card root directory. For the specific working mode and file name, refer to the following three configurations in source file main.c

```
/**< example work mode */
#define WM_APP_CODEC_WORK_TYPE (WM_APP_CODEC_WORK_TYPE_PLAYBACK_AND_RECORD)

/**< Play file name. */
#define WM_PLAYBCCK_WAV_FILE   "16B_16K.WAV"

/**< record file name. */
#define WM_RECORD_WAV_FILE     "record.wav"

```

## Compilation and Flashing

Example location: `examples/peripheral/codec_i2s/wav_play_record`

For compilation, flashing, and other operations, please refer to: [Quick Start](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## Running result

Successfully running will output the following logs

The log output level can be configure to Debug, so the  logs with prefix [D] can be output. These logs are convenient for debugging, but will affect data transmission.

```
[I] (5) main: ver: 2.3-alpha.1 build at Dec 24 2024 15:12:57
[I] (5) main: boot reason 0
[I] (5) main: heap size 272536(266.15KB)
[I] (6) main: flash size 0x200000(2MB)
[I] (8) app_i2s_codec: start wav play record example.
[I] (8) diskio_sdmmc: Initializing sdmmc device...
[I] (15) app_i2s_codec: mount sdcard to 0:, fs_type=3
[I] (107) es8374: set mic vol=100,reg=0
[I] (155) es8374: set out vol=0,reg=192
[I] (173) es8374: set adc in
[I] (201) es8374: setup gpio
[I] (201) es8374: set gpio 44=1,err = 0
[I] (201) app_i2s_codec: codec es8374start OK, mode=0,dir=2,std=0,rx_pkt_size=1200,rx_pkt_num=6,tx_pkt_num=6
[I] (206) wav: read fmt len=16
[I] (206) wav: read LIST,len=108
[I] (206) wav: read data,len=5766232
[I] (206) wav: open 0:16B_16K.WAV: code=1,chs=2,bits=16,eSampleRate=16000,offset=152
[I] (206) es8374: set out vol=80,reg=38
[I] (213) es8374: set 0x20005db0,16000,16,3
[I] (261) es8374: set adc in
[I] (289) es8374: setup gpio
[I] (289) es8374: set gpio 44=1,err = 0
[I] (289) app_i2s_codec: start play
[I] (324) es8374: set mic vol=100,reg=0
[I] (334) es8374: set adc in
[I] (362) es8374: setup gpio
[I] (362) es8374: set gpio 44=1,err = 0
[I] (362) app_i2s_codec: start record
[I] (80759) app_i2s_codec: up to 5M, stop
[I] (80772) app_i2s_codec: record end. size=5244000
[E] (88745) app_i2s_codec: read end
[E] (88745) app_i2s_codec: write err=0,exit!
[I] (88745) app_i2s_codec:
[I] (88745) app_i2s_codec: play end.
[I] (88751) app_i2s_codec: end wav play record.

```

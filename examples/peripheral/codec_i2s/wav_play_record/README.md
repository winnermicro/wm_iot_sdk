# I2S write

## 功能概述

此应用程序启动后执行下面几个操作：

1. mount SD card

2. 初始化 Codec 和 I2S

3. 启动 wav 文件播放 Task

    （1） 打开wav文件

    （2） 准备 I2S 写入 buffer；

    （3） 读出数据到 buffer, 如果不是 16 位或者 32 位，重采样到 16 位；

    （4） 通过 I2S 循环写出，传输给 CODEC 模块；

    （5） 播放完成则结束播放 Task。

4. 启动录音，录到 10M 大小后，结束录制。

    （1） 打开 wav 文件；

    （2） 准备 I2S 读数据 buffer；

    （3） 把从 I2S 读到的数写入到 wav 文件中；

    （4） 录制到 5M 大小时，结束录音的 Task。



## 环境要求

1. Demo pin脚使用要求：

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

2. Demo pin 脚配置，不同开发板，芯片型号可能不一样，需要在设备表中配置 pin 脚一致。

3. 其他配置

example需要使用 SD card 和 FAT 文件系统, 还需要 I2C , I2S模块, prj.config中已经打开了这些模块。

SD card 为 FAT 文件系统，在卡根目录下放 16B_16K.WAV 的 wav 文件。具体工作模式和文件参照 main.c 中下面 3 个配置


```
/**< example work mode */
#define WM_APP_CODEC_WORK_TYPE (WM_APP_CODEC_WORK_TYPE_PLAYBACK_AND_RECORD)

/**< Play file name. */
#define WM_PLAYBCCK_WAV_FILE   "16B_16K.WAV"

/**< record file name. */
#define WM_RECORD_WAV_FILE     "record.wav"

```

## 编译和烧录

示例位置：`examples/peripheral/codec_i2s/wav_play_record`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 运行结果

成功运行将输出如下日志 （ Log 输出级别调整为 Debug 才能输出 [D] 的日志，这些日志方便调试，但会影响数据传输。）

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
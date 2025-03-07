# I2S Write

## Functional Overview

Once this application is launched, it performs the following operations:

1. Initialize I2S controller drive

2. Start the task of receiving data

(1) Register the callback function to receive data. In the callback function, send the data to RX task for processing;

(2) Prepare the receive buffer and add the receive data buffer;

(3) Receive and process data circularly;

(4) End processing, stop receiving, cancel callback, and release the receiving buffer;

3. Start the task of sending data

(1) Register the sending data callback function, which is in the callback function;

(2) Prepare to send the data buffer, and initialize it to the increasing uint8_t integer data;

(3) Transmit data circularly;

(4) End processing, stop sending, cancel callback, and release data buffer。

## Environmental Requirements

1. The demo uses an EVB board
2. The default GPIO used is configured in the device table

1. Demo pin link requirement

```
┌─────────────────┐           ┌──────────────────────────┐
│    w80x         │           │          w80x            │
│                 │           │                          │
│                 │           │                          │
│     I2S_BCLK    ├──────────►│I2S_BCLK                  |
│                 │           │                          │
│    I2S_RLCLK    ├──────────►│I2S_RLCLK                 |
│                 │           │                          │
│        I2S_DI   ├──────────►│I2S_DO                    │
│                 │           │                          │
│        I2S_DO   │◄──────────┤I2S_DI                    │
│                 │           │                          │
│                 │           │                          │
│              GND├───────────┤GND                       │
└─────────────────┘           └──────────────────────────┘
```

2. Demo pin configuration

Different development boards may have different chip models, so you need to configure the pins in the device table to match the borad.

3. I2S configuration in source code

The default configuration in the main.c is Mstar mode. If two devices communicate, one of them needs to be set to slave mode

## Compilation and Flashing

Example location: `examples/peripheral/i2s`

For compilation, flashing, and other operations, please refer to: [Quick Start](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## Running result

Successfully running will output the following logs

The log output level can be configure to Debug, so the  logs with prefix [D] can be output. These logs are convenient for debugging, but will affect data transmission.

```
[D] (0) dt: add device rcc
[D] (0) dt: add device pmu
[D] (0) dt: add device iflash
[D] (1) ptb: partition table base 0xe000
[D] (1) dt: add device hash
[D] (1) ft: ft_0 base 0, ft_1 base 1000
[D] (1) ft: ft init too
[D] (2) dt: add device crc
[D] (2) ft: ft init 1


[I] (7) main: ver: 2.1.4dev build at Nov 25 2024 14:48:30
[I] (7) main: boot reason 0
[I] (7) main: heap size 272504(266.12KB)
[I] (7) main: flash size 0x200000(2MB)
[I] (8) example_i2s: I2S example start.
[D] (8) dt: add device i2s
[D] (8) DRV_I2S: clock:160000000, bitrate:1536000, mclk:80000000, extal:0, mdiv:2, bdiv:104
[D] (9) DRV_I2S: cfg use tx
[D] (9) DRV_I2S: cfg use rx
[I] (9) example_i2s: start OK, mode=0,dir=2,std=0,fmt=1,ctype=2,rx_pkt_size=1024,rx_pkt_num=6,tx_pkt_num=6
[I] (9) example_i2s: start read data
[D] (9) HAL_I2S: rx dma start
[I] (10) example_i2s: start write data loop
[D] (10) HAL_I2S: tx dma start
[I] (22) example_i2s: RX done
[D] (84) HAL_I2S: rx dma start
[D] (148) HAL_I2S: rx dma start
[D] (212) HAL_I2S: rx dma start
[D] (275) HAL_I2S: rx dma start
[D] (339) HAL_I2S: rx dma start
[D] (1064) example_i2s: recv:100
[D] (1074) example_i2s: write:100
[D] (2127) example_i2s: recv:200
[D] (2137) example_i2s: write:200
[D] (3189) example_i2s: recv:300
[D] (3200) example_i2s: write:300
[D] (4252) example_i2s: recv:400
[D] (4263) example_i2s: write:400

```

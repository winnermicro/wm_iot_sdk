# CRC DEMO

## Overview

Initiate a FreeRTOS task that runs the CRC demo once.

The CRC demo will iterate through the CRC types supported by the W800 and determine whether to perform reverse processing.

## Requirements

The board must have a W80x chip.

## Building and Flashing

Example Location： `examples/peripheral/crc`

For compilation, burning, and more information, see: [Quick Start Guide](https://doc.winnermicro.net/w800/en/2.2-beta.2/get_started/index.html)

## Running Result

Upon successful execution, the following logs will be output

```
D/dt              [0.000] add device iflash
D/ptb             [0.002] partition table base 0xe000
D/ft              [0.002] ft_0 base 0, ft_1 base 1000
D/ft              [0.002] ft init 1
D/main            [0.004] boot reason 0
D/main            [0.004] heap size 273144(266.74KB)
D/main            [0.004] flash size 0x200000(2MB)
I/test            [0.004] crc demo.
D/dt              [0.006] add device crc
I/test            [0.008] WM_GPSEC_CRC8 normal value:0x00000096
I/test            [0.010] WM_GPSEC_CRC8 INPUT_REFLECT value:0x00000003
I/test            [0.010] WM_GPSEC_CRC8 OUTPUT_REFLECT value:0x00000069
I/test            [0.010] WM_GPSEC_CRC8 OUT_IN_REVERSE value:0x000000C0
I/test            [0.012] WM_GPSEC_CRC16_MODBUS normal value:0x00002B74
I/test            [0.012] WM_GPSEC_CRC16_MODBUS INPUT_REFLECT value:0x0000F384
I/test            [0.012] WM_GPSEC_CRC16_MODBUS OUTPUT_REFLECT value:0x00002ED4
I/test            [0.014] WM_GPSEC_CRC16_MODBUS OUT_IN_REVERSE value:0x000021CF
I/test            [0.014] WM_GPSEC_CRC16_CCITT normal value:0x0000B685
I/test            [0.014] WM_GPSEC_CRC16_CCITT INPUT_REFLECT value:0x0000964E
I/test            [0.030] WM_GPSEC_CRC16_CCITT OUTPUT_REFLECT value:0x0000A16D
I/test            [0.030] WM_GPSEC_CRC16_CCITT OUT_IN_REVERSE value:0x00007269
I/test            [0.030] WM_GPSEC_CRC32 normal value:0x109EAC83
I/test            [0.054] WM_GPSEC_CRC32 INPUT_REFLECT value:0x4B1CAC0D
I/test            [0.054] WM_GPSEC_CRC32 OUTPUT_REFLECT value:0xC1357908
I/test            [0.054] WM_GPSEC_CRC32 OUT_IN_REVERSE value:0xB03538D2
I/test            [0.076] crc demo.
I/test            [1.076] crc demo.
I/test            [2.076] crc demo.
I/test            [3.076] crc demo.
I/test            [4.076] crc demo.
I/test            [5.076] crc demo.
```
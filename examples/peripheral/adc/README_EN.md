# ADC DEMO

## Overview

After starting this application, perform the following actions:

1. Initialize the ADC
2. Start a thread to read the internal temperature, chip voltage, and data from channel 0 of the chip within

## Environmental Requirements

Please note that there is a corresponding relationship between ADC channels and IO. When configuring, please refer to the definition in wm_adc_channel_t

## Building and Flashing

Example Location： `examples/peripheral/adc`

For operations such as compiling and burning, please refer to: [Quick Start Guide](https://doc.winnermicro.net/w800/en/2.2-beta.2/get_started/index.html)

## Running Result

The following logs is displayed after successful operation

```
[I] (21) adc_example: chip temp 40.205
[I] (46) adc_example: chip voltage 3233mv
[I] (50) adc_example: result oneshot 817mv
[I] (93) adc_example: result 0 356mv
[I] (94) adc_example: result 1 97mv
[I] (94) adc_example: result 2 -17mv
[I] (94) adc_example: result 3 37mv
[I] (94) adc_example: result 4 179mv
[I] (94) adc_example: result 5 339mv
[I] (94) adc_example: result 6 454mv
[I] (94) adc_example: result 7 465mv
[I] (94) adc_example: result 8 306mv
[I] (94) adc_example: result 9 58mv
[I] (94) adc_example: result 10 -17mv
[I] (94) adc_example: result 11 -17mv
[I] (94) adc_example: result 12 -16mv
[I] (94) adc_example: result 13 61mv
[I] (95) adc_example: result 14 208mv
[I] (95) adc_example: result 15 387mv
[I] (95) adc_example: result 16 478mv
[I] (95) adc_example: result 17 437mv
[I] (95) adc_example: result 18 244mv
[I] (95) adc_example: result 19 0mv
```
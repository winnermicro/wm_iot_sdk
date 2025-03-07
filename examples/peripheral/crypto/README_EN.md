# CRYP demo

## Overview

Initiate a FreeRTOS task to run the cryp demo once. 

The cryp demo iterates through the cryptography (cryp) types supported by the W800, as well as the submodes supported by each cryp type.

## Requirements

The board which have a w80x chip.

## Building and Flashing

Example Location： `examples/peripheral/cryp`

For operations such as compiling and burning, please refer to: [get started](https://doc.winnermicro.net/w800/en/latest/get_started/index.html)

## Running Result

The following logs are displayed after successful operation

```
D/dt              [0.000] add device iflash
D/ptb             [0.002] partition table base 0xe000
D/ft              [0.002] ft_0 base 0, ft_1 base 1000
D/ft              [0.002] ft init 1
D/main            [0.004] boot reason 0
D/main            [0.004] heap size 273144(266.74KB)
D/main            [0.004] flash size 0x200000(2MB)
I/test            [0.004] cryp demo.
D/dt              [0.006] add device cryp
I/test            [0.006] rc4 test success
I/test            [0.006] aes ecb test success
I/test            [0.010] aes cbc test success
I/test            [0.016] aes ctr test success
I/test            [0.016] des ecb test success
I/test            [0.018] des cbc test success
I/test            [0.018] 3des ecb test success
I/test            [0.018] 3des cbc test success
I/test            [0.020] cryp demo.
I/test            [1.020] cryp demo.
I/test            [2.020] cryp demo.
I/test            [3.020] cryp demo.
I/test            [4.020] cryp demo.
I/test            [5.020] cryp demo.
```
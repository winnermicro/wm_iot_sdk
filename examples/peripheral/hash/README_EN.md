# HASH DEMO

## Overview

Initiate a FreeRTOS task to run a hash demo once. 

The hash demo demonstrates how to calculate the hash value of data using MD5 and SHA1 algorithms, and then compares the calculated hash value with the expected hash value that has been precomputed.

## Requirements

The board which have a W80x chip.

## Building and Flashing

Example Location： `examples/peripheral/hash`

For compilation, flashing, and other operations, please refer to: [Quick Start Guide](https://doc.winnermicro.net/w800/en/latest/get_started/index.html)

## Running Result

Upon successful execution, the following logs will be output.

```
D/dt              [0.000] add device iflash
D/ptb             [0.002] partition table base 0xe000
D/ft              [0.002] ft_0 base 0, ft_1 base 1000
D/ft              [0.002] ft init 1
D/main            [0.004] boot reason 0
D/main            [0.004] heap size 273144(266.74KB)
D/main            [0.004] flash size 0x200000(2MB)
I/test            [0.004] hash demo.
D/dt              [0.006] add device hash
I/test            [0.006] md5 test success
I/test            [0.006] sha1 test success
I/test            [0.006] hash demo.
I/test            [1.008] hash demo.
I/test            [2.008] hash demo.
I/test            [3.008] hash demo.
I/test            [4.008] hash demo.
I/test            [5.008] hash demo.
I/test            [6.008] hash demo.
```
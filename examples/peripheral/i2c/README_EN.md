# I2C Write,Read

## Overview

After the application starts, perform the following operations:
1. Initialize I2C controller drive
2. Write a 16 byte string from EEPROM internal offset 0
3. Read a page of 16 bytes from EEPROM internal offset 0 and print it out

## Requirements

1. Demo uses EVB board, EEPROM model is NV24C02, and I2C address on this board is 0x52; When using other models, note that the address and other parameters should be modified to match.
2. I2C API reads and writes EEPROM. How to use I2C API. When actually reading and writing EEPROM, it is more convenient to use the device driver API of EEPROM.

## Building and Flashing

Example location: `examples/peripheral/i2c`
Other example`examples/peripheral/eeprom`

For operations such as compiling and burning, please refer to: [Quick Start Guide] (../../../../get_started/index. html # build function)

## Running Result

The following log is displayed after successful operation

```
D/dt              [0.002] add device iflash
D/ptb             [0.002] partition table base 0xe000
D/ft              [0.002] ft_0 base 0, ft_1 base 1000
D/ft              [0.004] ft init too
D/ft              [0.004] ft init 1
D/main            [0.004] boot reason 0
D/main            [0.004] heap size 273160(266.76KB)
D/main            [0.006] flash size 0x200000(2MB)
I/example_i2c     [0.006] i2c write/read example.

D/dt              [0.006] add device i2c0
I/example_i2c     [0.008] write end,ret=0
I/example_i2c     [0.028] read end,ret=0
I/example_i2c     [0.028] read OK:0123456789abc
```

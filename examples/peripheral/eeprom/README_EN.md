# EEPROM Write,Read

## Overview

After the application starts, perform the following operations:
1. Initialize I2C controller drive and EEPROM device drive
2. Write a string to the EEPROM internal offset of 5
3. Read a string from the EEPROM internal offset of 5

## Requirements

1.The demo utilizes an EVB Board with an EEPROM model of NV24C02, which has an I2C address of 0x52 on this board; When using other boards or EEPROM models, pay attention to modifying the configuration in the device table.

## Building and Flashing

Example location: `examples/peripheral/eeprom`
Other example`examples/peripheral/i2c`

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
I/example_eeprom  [0.006] eeprom write/read example.

D/dt              [0.006] add device i2c0
D/dt              [0.008] add device eeprom0
I/example_eeprom  [0.016] write end,len=24,ret=0
I/example_eeprom  [0.016] read end,ret=0
I/example_eeprom  [0.018] read OK:0123456789abcdefghijklmn
```

# RCC set cpu clock

## Overview

After the application starts, perform the following 2 operations:
1. Initialize RCC module
2. set CPU clock as 120MHz

## Requirements

1. None

## Building and Flashing

Sample location: `examples/peripheral/rcc`
For operations such as compiling and burning, please refer to: [Quick Start Guide](../../../../../get_started/index. html # build function)

## Running Result

The following log is displayed after successful operation

```
D/dt              [0.002] add device iflash
D/ptb             [0.002] partition table base 0xe000
D/ft              [0.002] ft_0 base 0, ft_1 base 1000
D/ft              [0.002] ft init too
D/ft              [0.004] ft init 1
D/main            [0.004] boot reason 0
D/main            [0.004] heap size 273160(266.76KB)
D/main            [0.004] flash size 0x200000(2MB)
I/exam_rcc        [0.006] rcc set cpu clock start.

set cpu clock as 120Mhz pass
```

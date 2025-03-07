# RNG Demo

## Overview

After starting，RNG (Random Number Generator) demo runs once.

The demo sequentially generates 5 sets of 32-bit random numbers, 10 sets of 32-bit random numbers.

## Requirements

The board which has a W80x chip.

## Building and flashing

Example Location： `examples/peripheral/rng`

For operations such as compiling and burning, please refer to: [get started](https://doc.winnermicro.net/w800/en/latest/get_started/index.html)

## Running Result

Upon successful execution, the following logs will be output.

```
D/dt              [0.000] add device iflash
D/ptb             [0.002] partition table base 0xe000
D/ft              [0.002] ft_0 base 0, ft_1 base 1000
D/ft              [0.002] ft init 1
D/main            [0.004] boot reason 0
D/main            [0.004] heap size 272736(266.34KB)
D/main            [0.004] flash size 0x200000(2MB)
I/test            [0.006] rng demo.
D/dt              [0.006] add device rng
RNG out:0x0000002F 0x0000005E 0x000000BD 0x0000017B 0x000002F6 
RNG out:0x308EBA23 0x611D7447 0xC23AE88E 0x8475D11C 0x08EBA239 0x11D74472 0x23AE88E5 0x475D11CA 0x8EBA2395 0x1D74472B 
I/test            [0.008] rng demo.
I/test            [1.008] rng demo.
I/test            [2.008] rng demo.
I/test            [3.008] rng demo.
```
# Flash Demo

## Overview

1. Initialize flash
2. Read and write flash data by call driver API


## Environmental Requirement

In advance, connect an external Flash memory. This example demonstrates connecting an external Flash memory through SPI, which requires prior wiring using the SPI interface.


## Building and Flashing

code path：`wm_iot_sdk/examples/peripheral/flash/external_flash`

For operations such as compiling and burning, please refer to: [Quick Start Guide](https://doc.winnermicro.net/w800/en/latest/get_started/index.html)

## Running Result

```
start example write and read external flash


********start write without erase example*********
write data is: abcdefghijklmnopqrst
read data is: abcdefghijklmnopqrst
without erase example pass


*************start write with erase example*************
write data is: zyxwvutsrqponmlkjihg
read data is: zyxwvutsrqponmlkjihg
with erase example pass

```
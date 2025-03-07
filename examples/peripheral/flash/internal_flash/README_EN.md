# Flash Demo

## Overview

1. Initialize flash
2. Get user region from partition table
3. Read and Write flash data by call driver API


## Environmental Requirement

None


## Building and Flashing

Code path：`wm_iot_sdk/examples/peripheral/flash/internal_flash`

For operations such as compiling and burning, please refer to: [Quick Start Guide](https://doc.winnermicro.net/w800/en/latest/get_started/index.html)

## Running Result

```
start example write and read internal flash


********start write without erase example*********
write data is: abcdefghijklmnopqrst
read data is: abcdefghijklmnopqrst
write without erase example pass


*************start write with erase example*************
write data is: zyxwvutsrqponmlkjihg
read data is: zyxwvutsrqponmlkjihg
write with erase example pass

```
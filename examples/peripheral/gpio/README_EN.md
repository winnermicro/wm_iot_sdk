# GPIO Demo

## Overview

After starting this application, perform the following actions:

1. Initialize the gpio
2. Configure the gpio, including GPIO input/output and interrupt configuration, and describe IO configurations that are not defined in the device table
3. Use interrupt triggering to observe device log output

## Environmental Requirements

Please note that gpio should be configured using devcie tree as a device, but due to the characteristics of iomux, IO configuration for pins is also provided

## Building and Flashing

Example Location： `examples/peripheral/gpio`

For operations such as compiling and burning, please refer to: [Quick Start Guide](https://doc.winnermicro.net/w800/en/latest/get_started/index.html)

## Running Result

The following log is displayed after successful operation

```
D/dt              [0.090] add device gpio
I/gpio            [0.094] WM_GPIO_DEFINE3 data value high
I/gpio            [1.098] WM_GPIO_DEFINE3 data value low
I/gpio            [2.102] gpio init ok
I/gpio            [19.178] GPIO[4] intr, val: 1
I/gpio            [21.214] GPIO[4] intr, val: 0
```
# Touch Sensor Interrupt

## Overview

This example demonstrates the use of a touch sensor interrupt function.

## Requirements

Touch sensor keys are connected in the following table:

| IO Number | IO Nmae   | Touch Number  |
| :-------- | :-------: | :------------ |
| 16        | PB0       | 3             |
| 17        | PB0       | 4             |
| 18        | PB0       | 5             |
| 12        | PA12      | CMOD          |
| 14        | PA14      | CMC           |

## Building and Flashing

Example Location： `examples/peripheral/touch_sensor/touch_interrupt`

For compilation, burning, and more, please refer to: [Quick Start Guide](https://doc.winnermicro.net/w800/en/latest/get_started/index.html)


## Running Result

Upon successful execution, the following logs will be output

```
D/dt              [0.038] add device touch_sensor
D/dt              [0.042] add device gpio
touch sensor 5 activated
touch sensor 5 activated
touch sensor 5 activated
touch sensor 5 activated
touch sensor 5 activated
touch sensor 5 activated
touch sensor 5 activated
touch sensor 3 activated
touch sensor 3 activated
touch sensor 3 activated
touch sensor 3 activated
touch sensor 3 activated
touch sensor 3 activated
touch sensor 3 activated
touch sensor 3 activated
touch sensor 3 activated
touch sensor 3 activated
touch sensor 4 activated
touch sensor 4 activated
touch sensor 4 activated
touch sensor 4 activated
```
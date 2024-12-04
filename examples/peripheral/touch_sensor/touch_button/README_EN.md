# Touch Sensor Button

## Overview

This example demonstrates the use of a touch sensor for key short and long presses.

A touch held for less than 500 milliseconds and released is a short press, and a touch held for more than 3 seconds is a long press.

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

Example Location： `examples/peripheral/touch_sensor/touch_button`

For compilation, burning, and more, please refer to: [Quick Start Guide](https://doc.winnermicro.net/w800/en/2.2-beta.2/get_started/index.html)


## Running Result

Successfully running will output the following logs

```
D/dt              [0.038] add device touch_sensor
D/dt              [0.042] add device gpio
I/touch           [0.046] touch button init ok
I/touch           [7.318] key5 pressed
I/touch           [11.326] key5 pressed
I/touch           [14.768] key5 long pressed
I/touch           [99.176] key4 pressed
I/touch           [99.754] key3 pressed
```
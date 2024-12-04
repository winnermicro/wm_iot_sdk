# Touch Sensor Poll Read

## Overview

This example demonstrates the use of a touch sensor for polling reads.

## Requirements

Touch sensor keys are connected in the following table:

| IO Number | IO Nmae   | Touch Number  |
| :-------- | :-------: | :------------ |
| 16        | PB0       | 3             |
| 17        | PB0       | 4             |
| 18        | PB0       | 5             |
| 12        | PA12      | CMOD          |
| 14        | PA14      | CMC           |

## Building and flashing

Example Location： `examples/peripheral/touch_sensor/touch_poll`

For compilation, burning, and more, please refer to: [Quick Start Guide](https://doc.winnermicro.net/w800/en/2.2-beta.2/get_started/index.html)


## Running result

Upon successful execution, the following logs will be output

```
D/dt              [0.038] add device touch_sensor
D/dt              [0.042] add device gpio
Touch Sensor read, the output format is:
Touch sensor num:[raw data]

T3: [1975] T4: [2112] T5: [2520]
T3: [1980] T4: [2163] T5: [2468]
T3: [1959] T4: [2123] T5: [2506]
T3: [1978] T4: [2155] T5: [2463]
T3: [1948] T4: [2139] T5: [2505]
T3: [1994] T4: [2154] T5: [2442]
T3: [1968] T4: [2155] T5: [2516]
T3: [1982] T4: [2145] T5: [2459]
T3: [1975] T4: [2144] T5: [2528]
T3: [1970] T4: [2154] T5: [2525]
T3: [1996] T4: [2144] T5: [2526]
T3: [1973] T4: [2152] T5: [2533]
T3: [1995] T4: [2161] T5: [2519]
T3: [1981] T4: [2140] T5: [2526]
T3: [1939] T4: [2156] T5: [2512]
T3: [1978] T4: [2138] T5: [2507]
```
# Wdt Read

## Overview

After starting this application, perform the following actions:

1. Initialize wdt, the current initialization value of wdt is configured as 10s in wm_dt_hw.c
2. Start the thread.Within the thread, perform a watchdog feeding operation in the thread, and read the cycle value and remaining value of the watchdog timer.

## Environmental requirements

The configuration of the watchdog has been completed in the device tree. If you need to make changes, you can modify the corresponding devcie tree

## Building and flashing

Example Location： `examples/peripheral/wdt`

For compilation, burning, and more, please refer to: [Quick Start Guide](https://doc.winnermicro.net/w800/en/2.2-beta.2/get_started/index.html)

## Running Result

Upon successful execution, the following logs will be output.

```
D/dt              [0.038] add device wdt
I/wdt_test        [0.042] wdt feed
I/wdt_test        [0.046] wdt counter value 400000000 reamining time 399998672
I/wdt_test        [0.550] wdt feed
I/wdt_test        [0.554] wdt counter value 400000000 reamining time 399999252
```
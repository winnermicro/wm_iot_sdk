# Timer Read

## Overview

After starting this application, perform the following actions:

1. Initialize timer0, timer1, timer2
2. Start the timer and observe its execution

## Environmental Requirements

Please note that w800 supports 6 timers, and the device name in the device tree is timer0, timer1, timer2, timer3, timer4, timer5， Please register as needed

## Building and Flashing

Example Location： `examples/peripheral/timer`

For compilation, burning, and more, please refer to: [Quick Start Guide](https://doc.winnermicro.net/w800/en/latest/get_started/index.html)

## Running Result

Upon successful execution, the following logs will be output

```
D/dt              [0.038] add device timer0
D/dt              [0.042] add device timer1
D/dt              [0.046] add device timer2
I/timer_test      [2.052] timer1 TRIGGERED
I/timer_test      [3.052] timer2 TRIGGERED
I/timer_test      [4.052] timer1 TRIGGERED
I/timer_test      [6.052] timer1 TRIGGERED
I/timer_test      [6.052] timer2 TRIGGERED
I/timer_test      [8.046] timer1 TRIGGERED
I/timer_test      [9.046] timer2 TRIGGERED
I/timer_test      [10.046] timer0 TRIGGERED
I/timer_test      [10.046] timer1 TRIGGERED
I/timer_test      [10.050] all timer stop
```
# PWM Output

## Overview

Upon startup, this application performs the following operations:

1. Initialize the PWM controller driver.
2. Bind and initialize GPIO 28 (PB12) for PWM channel 0.
3. Initialize PWM channel 0 with specific parameters for independent output mode.
4. Start the independent output of PWM channel 0.
5. Observe whether the LED is lit.
6. The PWM will continuously output a waveform with a periodically changing duty cycle, which means the LED light will gradually dim and then become brighter.

## Environment Requirements

1. Demo uses an EVB board
2. Board with onboard LED bind with the configured GPIO

## Compilation and Flashing

Example location: `examples/peripheral/pwm`

For compilation and flashing instructions, refer to: [Quick Start Guide](https://doc.winnermicro.net/w800/en/2.2-beta.2/get_started/index.html)

## Running Result

The following log is displayed after successful operation

```
[D] (0) dt: add device pmu
[D] (0) dt: add device rcc
[D] (0) dt: add device dma
[D] (2) dt: add device gpio
[D] (6) dt: add device iflash
[D] (6) ptb: partition table base 0xe000
[D] (6) ft: ft_0 base 0, ft_1 base 1000
[D] (6) ft: ft init too
[D] (6) ft: ft init 1
[D] (236) main: boot reason 0
[D] (236) main: heap size 273400(266.99KB)
[D] (238) main: flash size 0x200000(2MB)
[D] (318) dt: add device pwm
[I] (318) PWM DRV: pwm channel initialize successfully!
[I] (318) PWM HAL: Find period[199], clkdiv[20] for freq[10000]!
[I] (318) PWM: The pin GPIO[28] will continuously output a PWM waveform with a periodically varying duty cycle.
```

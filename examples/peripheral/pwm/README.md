# PWM Output

## 功能概述

此应用程序启动后执行下面几个操作：

1. 初始化 PWM 控制器驱动
2. 为 PWM channel 0 绑定并初始化 GPIO 28 (PB12)
3. 以特定参数初始化 PWM channel 0 为独立输出模式
4. 启动 PWM channel 0 独立输出
5. 观察 LED 是否点亮
6. PWM 将会持续输出周期变化占空比的波形，即 LED 灯将逐渐暗再变亮

## 环境要求

1. Demo 使用 EVB 板子
2. 对应 GPIO 挂载 LED 灯

## 编译和烧录

示例位置：`examples/peripheral/pwm`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 运行结果

成功运行将输出如下日志

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

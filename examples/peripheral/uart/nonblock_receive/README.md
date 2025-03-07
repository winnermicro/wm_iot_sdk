# Uart nonblock_receive

## 功能概述

此应用程序启动后执行下面4个操作：

1. 初始化串口1
2. 创建消息队列
3. 注册回调函数接收串口数据接收状态回调，回调中把状态消息发送到task中去处理；
4. 循环接收消息，收到消息后从驱动中获取到接收数据的指针和当前块长度，根据指针和长度把数据写回到串口1，释放数据指针。

Uart使用这种方式接收数据，不会阻塞task, 可以在其他模块的task中使用。由于直接拿接收buffer中的数据指针进行处理， 无需拷贝，更有效率。

## 环境要求

1. 引脚连接：

   硬件配置 存储在 Device Table 中，包含 UART 设备的引脚定义。

   Device Table 的定义文件是 wm_dt_hw.c，但具体可能会因为此工程所引用的设备表文件不同而由差异。

   下图为 W800 的 GPIO 序号 ， 对应的是 EVB 开发板的示例， 实际使用应该根据硬件配置Device Table， 根据实际硬件使用链接。

   | GPIO_NUM | 引脚编号 | 连接描述  | 链接电脑 |
   | -------- |-------- | --------- | --------|
   | WM_GPIO_NUM_22 | PB6   | TX   |   RX     |
   | WM_GPIO_NUM_23 | PB7   | RX   |   TX     |


## 编译和烧录

示例位置：`examples/peripheral/uart/block_read_write`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
D/dt              [0.002] add device iflash
D/ptb             [0.002] partition table base 0xe000
D/ft              [0.002] ft_0 base 0, ft_1 base 1000
D/ft              [0.004] ft init too
D/ft              [0.004] ft init 1
D/main            [0.004] boot reason 0
D/main            [0.004] heap size 273152(266.75KB)
D/main            [0.006] flash size 0x200000(2MB)
I/exam_uart       [0.006] uart noblock receive demo start.

D/dt              [0.006] add device uart1
```

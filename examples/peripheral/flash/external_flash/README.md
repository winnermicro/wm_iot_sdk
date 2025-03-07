# flash示例

## 功能概述

此应用程序启动后执行下面2个操作：

1. 初始化flash
2. 调用flash接口读写数据


## 环境要求
提前接入外部flash，本示例演示的是通过SPI外接flash，需提前利用SPI接线方式接入flash


## 编译和烧录

示例位置：`wm_iot_sdk/examples/peripheral/flash/external_flash`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
start example write and read external flash


********start write without erase example*********
write data is: abcdefghijklmnopqrst
read data is: abcdefghijklmnopqrst
without erase example pass


*************start write with erase example*************
write data is: zyxwvutsrqponmlkjihg
read data is: zyxwvutsrqponmlkjihg
with erase example pass

```
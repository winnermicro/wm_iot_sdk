# flash示例

## 功能概述

此应用程序启动后执行下面2个操作：

1. 初始化flash
2. 获取分区表中的用户区域
3. 调用 flash 接口读写用户区域数据


## 环境要求
无


## 编译和烧录

示例位置：`wm_iot_sdk/examples/peripheral/flash/internal_flash`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
start example write and read internal flash


********start write without erase example*********
write data is: abcdefghijklmnopqrst
read data is: abcdefghijklmnopqrst
write without erase example pass


*************start write with erase example*************
write data is: zyxwvutsrqponmlkjihg
read data is: zyxwvutsrqponmlkjihg
write with erase example pass

```
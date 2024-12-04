# SDIO Slave

## 功能概述

此应用程序是使用 `WM IoT SDK` 进行 SDIO 从机通信的示例。它演示了如何使用 `WM IoT SDK` 中的 `wm_drv_sdio_slave` 驱动 API 进行主机和从机之间的数据传输。示例包括同步和异步数据传输,以及命令和数据的接收处理。

## 环境要求

需要支持 SDIO 从机功能的硬件环境,以及一个 SDIO 主机设备。

- W80x 开发板
- 带有 SDIO 主机接口的设备(如另一个 MCU 或开发板)
- 连接线缆

CIS (Card Information Structure) 定义了卡的一些信息结构，CIS 包含公共 CIS (Function 0) 和 Function 1 端口各自的 CIS，`cis.c` 文件中定义的 `fn_regs` 仅仅是 CIS 配置参考，用户可根据自己的 SDIO 卡类型进行修改。

## 编译和烧录

示例位置：`examples\peripheral\sdio_slave`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
[I] (4) main: ver: 2.1.3dev build at Nov  7 2024 10:57:19
[I] (4) main: boot reason 0
[I] (4) main: heap size 272952(266.55KB)
[I] (5) main: flash size 0x200000(2MB)
[I] (6) example: SDIO slave demo start
[I] (6) example: Initializing sdio slave device...
[I] (7) example: Successfully initialized sdio slave device
[I] (7) example: Waiting for rx events...
```
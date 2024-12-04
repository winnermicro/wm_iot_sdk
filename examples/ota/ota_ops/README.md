# OTA_OPS

## 功能概述

此应用程序是使用 `WM IoT SDK` 使用 `wm_ota_ops` API 接口进行设备固件升级功能的全面示例。它演示了如何处理 OTA 更新，包括初始化 OTA 操作、读取固件镜像数据、写入数据到设备的 OTA 分区、以及在完成后重启设备。基于 `wm_ota_ops` API 接口可以实现各自升级方案和对接多个 IoT 平台，以下是 OTA 整体架构：

![OTA Architecture](./figure/ota_architecture.svg)

## 环境要求

1. `main/src/atcmd_ota.img` 为嵌入到代码中的升级固件，来源于 `examples/atcmd` 编译生成的 OTA 升级固件，若需要修改成其他固件可参考[构建系统-文件嵌入到代码中](https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/component_guides/build_system.html#id5)
2. 该工程使用到的分区表必须包含 `app_ota` 分区，分区表可参考[分区表机制](https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/component_guides/partition_table.html)

## 编译和烧录

示例位置：`examples\ota\ota_ops`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
[I] (2) main: ver: 2.0.0rc2 build at Sep 11 2024 00:45:45
....................................................................................................
[I] (2856) example: This example succeeds in running.
[I] (2856) ota_ops: ota reboot, please wait for the restart...
start unxz, please wait...
...........................................
unxz succeed
[E] (2) nvs: scan item crc err in 0,status=fc,key=wm_sta_mac,do GC
[I] (108) main: ver: 2.0.0rc2 build at Sep 10 2024 17:54:07
[I] (308) atcmd: set default wifi config
[I] (368) atcmd: set default tcpip config
[I] (432) atcmd: atcmd init finished.
```

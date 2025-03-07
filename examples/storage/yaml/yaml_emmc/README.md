# libyaml

## 功能概述

本示例演示了如何使用 fatfs 调用 libyaml 解析 yaml 文件。

## 环境要求

需要外接 EMMC 卡。

## 编译和烧录

示例位置：`examples/storage/yaml/yaml_emmc`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
diskio_sdmmc: Initializing sdmmc device...
yaml_emmc_example: File system mount success, fs_type: 3
yaml_emmc_example: localhost, abc
```
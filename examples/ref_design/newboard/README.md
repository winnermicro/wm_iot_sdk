# New Board Reference Project

## 功能概述

此工程演示如何基于一个新的硬件开发板，进行 Device Table 的配置 及 Partition Table 的配置, 协助开发者在全新的工程中快速
梳理开发步骤，完成基本工程的创建。

## 环境要求

可用联盛德的官方开发板，也可用开发者自己基于联盛德SoC模组搭建的开发板

## 开发步骤说明

### 1:了解硬件设计需求

   向硬件工程师或开发板供应商 获取开发板原理图或硬件需求说明，软件工程师据此可清楚知道,在此项目中:

   1.1. 所用的 SoC 型号是什么? 该 SoC 所含的 Memory 及 内置/外置 Flash， Interrupt, Timer， IO 等硬件资源有多少?

   对应的芯片手册资料可参考 [SoC指南-芯片手册](https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/soc_guides/index.html)

   1.2. 所搭配的外设有哪些，比如：UART, SPI-Master, I2S 等？需要用到 SoC 的多少引脚？ 此项目对每个外设的性能要求及对应的参数配置要求？
        可对比当前所选型 SoC 的 pinmux 来确认引脚是否符合 项目需求。
        
   文档可参考 [SoC-PINMUX](https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/component_guides/pinmux.html)

### 2:梳理项目整体的软件功能需求

   据此，软件工程师可清楚知道,此项目中:

   2.1. 有多少待开发的功能，每个功能大概 需要用到多少 Memory, Flash 等硬件存储资源。

   2.2. 项目软件的整体框架, 需要创建多少任务，各任务的优先级 和 stack, heap 资源需求，各任务间的通信机制选择和协作关系等。

### 3:软硬件需求项确认后，创建新的 SDK 工程

   这里有两种途径:

   3.1. 直接在 SDK 中，选择一个基准工程，比如 'hello_world' 或 此处的 'newboard' 工程，将其拷贝到 example 的其他目录 并重命名为自己的工程名，比如 'project_a'
        需要注意的是，工程名请用英文字母，可以加数字，但不能包含空格或其他语言文字。

   文档可参考 [构建系统-快速创建工程](https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/component_guides/build_system.html)

   3.2. 基于 vscode IDE 环境创建工程

   文档可参考 [IDE-创建工程](https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/get_started/ide.html)

### 4:创建新工程对应的设备表 (Device Table)

   虽然在 SDK 的 components/wm_dt 目录中包含有 默认的 wm_dt_hw.c, wm_dt_hw.h 文件，但这些文件内容包含了SDK所有原生支持的控制器设备及外部设备的设备表定义。
   在开发者自己的项目中，不见得里面所定义的所有设备都会被用到，且用到的设备也不一定会沿用其相同的配置参数 (比如:不同项目的引脚选用会不一样)。

   通过 1.2 的调查，开发者基本上应该可以确定，是否新工程也需要创建其独有的 Device Table。
   如果需要，则可参考本示例进行创建。
   
   文档可参考 [设备表](https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/component_guides/device_table.html)
   
### 5：创建新工程对应的分区表 (Partition Table)

   虽然在 SDK 的 components/partition_table/config 目录中包含有 默认的 partition_table_large.csv, partition_table_normal.csv, partition_table_with_ota.csv 分区表文件，
   这些文件可以由用户在 menuconfig 界面中选择，但这些分区表文件 仅包含基础分区。
   在开发者自己的项目中，有可能会需要项目特有的分区，方便配合业务的实现，比如:存储配置文件，音频文件，密钥，工厂参数等。

   通过 2.1 的调查，开发者基本上应该可以确定，是否新工程也需要创建其独有的 分区，并单独管理其分区表文件。
   如果需要，则可参考本示例进行创建。
   
   文档可参考 [分区表](https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/component_guides/partition_table.html)

### 6:基于业务需求，配置功能开关

   SDK 采用组件架构，各组件都有1个或多个 Kconfig 功能开关进行管理，很多功能开关是默认关闭的。

   项目所需要的 功能开关，不建议 直接在 各组件目录下的 Kconfig 文件中去打开， 而建议到新工程的 prj.config 文件中 去添加所需功能开关的定义。
   这样可以 避免对此 SDK 中其他工程的影响。

   修改后可通过 menuconfig 界面，查看此工程 所需要的各项 功能开关是否被正确设定。
   
   文档可参考 [Kconfig配置](https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/component_guides/kconfig.html)

   需要注意的是，设备相关的功能开关 会与 设备表中的配置有关。 打开相关设备功能时，原生设备表的内容也需要复制到新工厂的设备表中，否则会产生编译错误。
   

### 7:创建自己的任务和组件，进行业务开发

   创建自己的应用任务时，需要考虑到对其他任务的影响，所以需要查询系统其他任务的优先级整体分配情况来定应用任务优先级。

   可参考 [开发助手](https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/component_guides/dev_asst.html)


## 编译和烧录

示例位置：`examples/ref_design/newboard`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/get_started/index.html)

## 运行结果

成功运行将输出如下日志,  其中partition table info 会打印出此工程目录下存储的 partition_table_custom.csv 内容

```
partition table info:
  name             offset     size       flag
  ft               0x00000000 0x00002000 0x00
  bootloader       0x00002000 0x0000c000 0x00
  partition_table  0x0000e000 0x00001000 0x00
  app              0x0000f000 0x001d1000 0x00
  user             0x001e0000 0x00002000 0x00
  nvs              0x001f0000 0x00008000 0x00
[I] (4) main: Ver: 0.8.1 build at Sep  3 2024 21:16:57
[I] (4) ref new board: heap free size=271296(bytes)
[I] (1004) ref new board: heap free size=275552(bytes)
[I] (2004) ref new board: heap free size=275552(bytes)
[I] (3004) ref new board: heap free size=275552(bytes)
[I] (4004) ref new board: heap free size=275552(bytes)
```
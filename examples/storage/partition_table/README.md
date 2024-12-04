# Partition Table

## 功能概述
此应用程序启动后执行3个操作，初始化分区表，分区查找，打印全部分区。

## 环境要求

1. 按需求配置WM_PARTITION_TABLE_TYPE，默认配置的是components/partition_table/partition_table_large.csv


## 编译和烧录

示例位置：`examples/storage/partition_table`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
D/dt              [0.002] add device iflash
D/ptb             [0.002] partition table base 0xe000
D/ft              [0.002] ft_0 base 0, ft_1 base 1000
D/ft              [0.004] ft init too
D/ft              [0.004] ft init 1
D/main            [0.004] boot reason 0
D/main            [0.004] heap size 273160(266.76KB)
D/main            [0.004] flash size 0x200000(2MB)
I/exam_pt         [0.006] This example succeeds in running.
D/ptb             [0.006] partition table base 0xe000
I/exam_pt         [0.006] Get nvs OK,offset=0x001f1000,size=0x8000

partition table info:
  name             offset     size       flag
  ft               0x00000000 0x00002000 0x00
  bootloader       0x00002000 0x0000c000 0x00
  partition_table  0x0000e000 0x00001000 0x00
  device_table     0x0000f000 0x00001000 0x00
  app              0x00010000 0x001e1000 0x00
  nvs              0x001f1000 0x00008000 0x00
```

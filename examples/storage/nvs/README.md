# NVS

## 功能概述

此应用程序启动后执行下面5个操作：

1. 字符串读写
2. 二进制数据读写
3. 浮点数读写
4. 8位整数读写
5. 使用迭代器遍历全部数据

## 环境要求

1. 需要配置COMPONENT_NVS_ENABLED为enable,默认是enable状态
2. 分区表中存在nvs分区，分区表中默认已经存在nvs分区


## 编译和烧录

示例位置：`examples/storage/nvs`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
[I] (5) main: ver: 2.1.3dev build at Nov  6 2024 14:39:57
[I] (10) main: boot reason 0
[I] (13) main: heap size 272712(266.32KB)
[I] (17) main: flash size 0x200000(2MB)
[I] (20) exam_nvs: NVS example succeeds in running.
[I] (25) exam_nvs: open nvs, group=user err=0
[I] (30) exam_nvs: Read str:123456

[I] (33) exam_nvs: dump [binary]
0x0000   01 02 03 04 05                                    .....
[I] (42) exam_nvs: Read float=1.234500

[I] (46) exam_nvs: Read I8=-120

[I] (49) exam_nvs: Start iterator.
[I] (52) exam_nvs: key_str,len=7
[I] (55) exam_nvs: key_bin,len=5
[I] (58) exam_nvs: key_float,len=8
[I] (61) exam_nvs: key_I8,len=1
[I] (64) exam_nvs: NVS example end.

```

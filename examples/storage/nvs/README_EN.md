# NVS

## Overview

After the application starts, perform the following 5 operations:
1. String reading and writing
2. Binary data reading and writing
3. Floating point number reading and writing
4. 8-bit integer reading and writing
5. Use iterator to traverse all data

## Requirements

1. You need to configure COMPONENT_NVS_ENABLED to enable, and the default is enable
2. There is an nvs partition in the partition table. By default, an nvs partition already exists in the partition table

## Building and flashing

Sample location: ` examples/storage/nvs`

For operations such as compiling and burning, please refer to: [Quick Start Guide](https://doc.winnermicro.net/w800/en/latest/get_started/index.html)

## Running Result

Upon successful execution, the following logs will be output.

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

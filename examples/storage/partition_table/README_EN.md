# Partition Table

## Overview

After the application starts, it performs three operations, initializing the partition table, searching partitions, and printing all partitions.

## Requirements

1. Configure WM_PARTITION_TABLE_TYPE as required. The default configuration is components/partition_table/partition_table_large.csv


## Building and Flashing

Example location: ` examples/storage/partition_table`

For operations such as compiling and burning, please refer to: [Quick Start Guide](../../../../get_started/index. html # build function)

## Running Results

Upon successful execution, the following logs will be output.

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

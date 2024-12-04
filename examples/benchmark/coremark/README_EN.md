# CoreMark

## Overview

CoreMark is used to test processor performance.

## Requirements

None.

## Building and Flashing

Example Location： `examples/benchmark/coremark`

For compiling, burning, and others, see: [Quick Start Guide](https://doc.winnermicro.net/w800/en/2.2-beta.2/get_started/index.html)

## Running Result

If it runs successfully, it will output the following logs.

```
2K performance run parameters for coremark.
CoreMark Size    : 666
Total ticks      : 10318000
Total time (secs): 10.318010
Iterations/Sec   : 639.658209
Iterations       : 6600
Compiler version : GCC6.3.0
Compiler flags   :
Memory location  : STACK
seedcrc          : 0xe9f5
[0]crclist       : 0xe714
[0]crcmatrix     : 0x1fd7
[0]crcstate      : 0x8e3a
[0]crcfinal      : 0x33ff
Correct operation validated. See readme.txt for run and reporting rules.
CoreMark 1.0 : 639.658209 / GCC6.3.0  / STACK
Score (Coremarks/MHz):   2.67
```
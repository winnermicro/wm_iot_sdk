# SDIO Slave

## Overview

This application is an example of SDIO slave communication using the `WM IoT SDK`. It demonstrates how to use the `wm_drv_sdio_slave` driver APIs from the `WM IoT SDK` for data transfer between host and slave. The example includes both synchronous and asynchronous data transfers, as well as command and data reception handling.

## Requirements

Hardware environment that supports SDIO slave functionality and an SDIO host device are required.

- W80x development board
- Host device with SDIO master interface (e.g. another MCU or development board)
- Connection cables

CIS (Card Information Structure) defines the information structure of the card. CIS contains both common CIS (Function 0) and Function 1 port's respective CIS. The `fn_regs` defined in the `cis.c` file is just a CIS configuration reference, users can modify it according to their own SDIO card type.

## Building and Fashing

Example Location： `examples\peripheral\sdio_slave`

For compilation, flashing, and other operations, please refer to: [Quick Start Guide](https://doc.winnermicro.net/w800/en/latest/get_started/index.html)


## Running Result

The following log is displayed after successful operation

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

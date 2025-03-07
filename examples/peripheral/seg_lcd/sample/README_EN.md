# SEG_LCD-Sample

## Overview
This application is a comprehensive example of the SEG_LCD segmented display function using the WM IoT SDK. It demonstrates how to initialize the SEG_LCD device, perform a series of display operations, including repeatedly displaying all segments and weekdays, and finally clear the display and deinitialize the device.

## Requirements

1. Segmented Display Requirements:

   - Manufacturer: Shenzhen Genyu Technology Co., Ltd.
   - Model: GY-H70248 （Sample）
   - Can be replaced with any other segmented display

2. Pin Connections:

   | Pin Number | Connection Description | GY-H70248 PIN （Sample） |
   | ---------- | --------------------- | ------------- |
   | PB25       | COM0                  | 1 and 37      |
   | PB21       | COM1                  | 2 and 38      |
   | PB22       | COM2                  | 3 and 39      |
   | PB27       | COM3                  | 4             |
   | PB26       | SEG01                 | 5             |
   | PB24       | SEG02                 | 6             |
   | PA7        | SEG03                 | 7             |
   | PA8        | SEG04                 | 8             |
   | PA9        | SEG05                 | 9             |
   | PA10       | SEG06                 | 10            |
   | PA11       | SEG07                 | 11            |
   | PA12       | SEG08                 | 12            |
   | PA13       | SEG09                 | 13            |
   | PA14       | SEG10                 | 14            |
   | PA15       | SEG11                 | 15            |
   | PBO        | SEG12                 | 16            |
   | PB1        | SEG13                 | 17            |
   | PB2        | SEG14                 | 18            |
   | PB3        | SEG15                 | 19            |
   | PB4        | SEG16                 | 20            |
   | PB5        | SEG17                 | 21            |
   | PB6        | SEG18                 | 22            |
   | PB7        | SEG19                 | 23            |
   | PB8        | SEG20                 | 24            |
   | PB9        | SEG21                 | 25            |
   | PB10       | SEG22                 | 26            |
   | PB11       | SEG23                 | 27            |
   | PB12       | SEG24                 | 28            |
   | PB13       | SEG25                 | 29            |
   | PB14       | SEG26                 | 30            |
   | PB15       | SEG27                 | 31            |
   | PB16       | SEG28                 | 32            |
   | PB17       | SEG29                 | 33            |
   | PB18       | SEG30                 | 34            |
   | PA6        | SEG31                 | 35            |

## Building and Flashing

Example Location： `examples\peripheral\seg_lcd\sample\`

For compilation, flashing, and other operations, please refer to: [Quick Start Guide](https://doc.winnermicro.net/w800/en/latest/get_started/index.html)


## Running Result

The following log is displayed after successful operation
```
[I] (10) main: ver: 2.2-beta.2 build at Dec 10 2024 09:33:51
[I] (10) main: boot reason 0
[I] (11) main: heap size 272648(266.26KB)
[I] (11) main: flash size 0x200000(2MB)
[I] (13) example: seg_lcd display demo start.
[I] (13) example: Initializing segment LCD device...
[I] (15) example: Starting segment LCD display/clear cycle...
[I] (10131) example: Deinitializing segment LCD device.
[I] (10132) example: This example succeeds in running.
```

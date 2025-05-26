# SPI Master Soft (GPIO Simulation) Asynchronous Transceive Example

## Functional Overview

After the application is launched, the following operations will be executed in sequence:

1. **Initialization of SPI Master Soft (GPIO Simulation)**
   The SPI master functionality is implemented using GPIO pins through software simulation. This method does not require a hardware SPI module but may be affected by the CPU clock frequency, resulting in deviations between the actual operating frequency and the preset value.

2. **Calling the SPI Master Soft (GPIO Simulation) Asynchronous Transceive Interface**
   256 bytes of data are sent, while simultaneously receiving 256 bytes of data. The asynchronous transceive mechanism allows the program to continue executing other tasks during data transmission, thereby improving system efficiency.

**Note**:
Due to the limitations of GPIO simulation, the actual operating frequency may deviate from the preset value, and the maximum frequency is affected by the CPU clock. If higher accuracy of frequency or faster rates are required, it is recommended to use a hardware SPI Master.

## Environment Requirements

1. **Hardware Connection**

  The pin configurations are stored in the Device Table, where users can define and query them by device name.
  The default configuration of the GPIO numbers for the W800 and their corresponding SPI Master interface connections is shown in the table below. The actual configuration should refer to the Device Table. The default configuration is stored in the "device_table.toml" or "wm_dt_hw.c" under the selected chip type. For specific usage, please refer to the Device Table section.

| GPIO Number | Pin Number | Connection Description |
|-------------|------------|------------------------|
| WM_GPIO_NUM_27 | PB11 | CS |
| WM_GPIO_NUM_26 | PB10 | MOSI |
| WM_GPIO_NUM_25 | PB9 | MISO |
| WM_GPIO_NUM_24 | PB8 | CLOCK |

- Ensure that the SPI master pins are correctly connected to the slave device.
- The SPI mode in this example is Mode 0 (CPOL = 0, CPHA = 0), with a clock frequency of 2 MHz.

2. **Slave Device Preparation**

   The slave end must be started in advance, be in a receive-waiting state, and have data prepared for transmission.
## Compilation and Flashing

1. **Example Location**

   The example code is located at `examples/peripheral/spi_master_soft/transceive_async`.

2. **Compilation and Flashing Operations**

   Please refer to the [Quick Start Guide](https://doc.winnermicro.net/w800/en/latest/get_started/index.html).

## Operation Results

Upon successful execution, the program will output the following log:

```
[I] (2) main: ver: 2.3-beta.2 build at Apr  2 2025 09:37:38
[I] (2) main: boot reason 0
[I] (2) main: heap size 272608(266.22KB)
[I] (3) main: flash size 0x200000(2MB)
[I] (4) exam_spim_soft: spi master async transceive start.
[I] (30) exam_spim_soft: enter example_spim_async_callback, result:0
[I] (30) exam_spim_soft: rx data as below
[I] (30) exam_spim_soft: dump [rx data]
0x0000   31 32 33 34 35 36 37 38  39 30 31 31 32 33 34 35  1234567890112345
0x0010   36 37 38 39 30 31 32 33  34 35 36 37 38 39 30 31  6789012345678901
0x0020   32 33 34 35 36 37 38 39  30 31 32 33 34 35 36 37  2345678901234567
0x0030   38 39 30 31 32 33 34 35  36 37 38 39 30 31 32 33  8901234567890123
0x0040   34 35 36 37 38 39 30 31  32 33 34 35 36 37 38 39  4567890123456789
0x0050   30 31 32 33 34 35 36 37  38 39 30 31 32 33 34 35  0123456789012345
0x0060   36 37 38 39 30 31 32 33  34 35 36 37 38 39 30 31  6789012345678901
0x0070   32 33 34 35 36 37 38 39  30 31 32 33 34 35 36 37  2345678901234567
0x0080   38 39 30 31 32 33 34 35  36 37 38 39 30 31 32 33  8901234567890123
0x0090   34 35 36 37 38 39 30 31  32 33 34 35 36 37 38 39  4567890123456789
0x00a0   30 31 32 33 34 35 36 37  38 39 30 31 32 33 34 35  0123456789012345
0x00b0   36 37 38 39 30 31 32 33  34 35 36 37 38 39 30 31  6789012345678901
0x00c0   32 33 34 35 36 37 38 39  30 31 32 33 34 35 36 37  2345678901234567
0x00d0   38 39 30 31 32 33 34 35  36 37 38 39 30 31 32 33  8901234567890123
0x00e0   34 35 36 37 38 39 30 31  32 33 34 35 36 37 38 39  4567890123456789
0x00f0   30 31 32 33 34 35 36 37  38 39 30 31 32 33 34 35  0123456789012345
```

The received data is related to the data prepared by the slave device.

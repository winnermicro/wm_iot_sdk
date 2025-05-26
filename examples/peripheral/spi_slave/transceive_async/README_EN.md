# SPI Slave Asynchronous Transceiver Example

## Function Overview

After this application starts, it performs the following two operations:

1. Initialize SPI slave
2. Call SPI slave asynchronous transceiver interface, send 256 bytes while receiving 256 bytes of data

## Environmental Requirements

1. The master device and SPI slave pins need to be connected in advance
2. The slave side needs to be started in advance, waiting to receive and prepare the data to be sent
3. PIN_CS: GPIO20, PIN_CLK: GPIO17, PIN_MISO: GPIO16, PIN_MOSI: GPIO7, mode 0


## Compile and Download

Example location: `examples/peripheral/spi_slave/transceive_async`

For compilation, flashing, and other operations, please refer to: [Quick Start Guide](https://doc.winnermicro.net/w800/en/latest/get_started/index.html)

## Running Result

The following log will be displayed after successful operation:

```
[I] (3) main: ver: 2.4-alpha.2 build at Apr 24 2025 11:40:57
[I] (4) main: boot reason 0
[I] (4) main: heap size 272512(266.12KB)
[I] (5) main: flash size 0x200000(2MB)
[I] (5) exam_spis: spi slave async transecie start.
[I] (466) exam_spis: enter example_spis_aysnc_callback, result:0
[I] (467) exam_spis: rx data as below
[I] (467) exam_spis: dump [rx data]
0x0000   78 56 34 12 78 56 34 12  78 56 34 12 78 56 34 12  xV4.xV4.xV4.xV4.
0x0010   78 56 34 12 78 56 34 12  78 56 34 12 78 56 34 12  xV4.xV4.xV4.xV4.
0x0020   78 56 34 12 78 56 34 12  78 56 34 12 78 56 34 12  xV4.xV4.xV4.xV4.
0x0030   78 56 34 12 78 56 34 12  78 56 34 12 78 56 34 12  xV4.xV4.xV4.xV4.
0x0040   78 56 34 12 78 56 34 12  78 56 34 12 78 56 34 12  xV4.xV4.xV4.xV4.
0x0050   78 56 34 12 78 56 34 12  78 56 34 12 78 56 34 12  xV4.xV4.xV4.xV4.
0x0060   78 56 34 12 78 56 34 12  78 56 34 12 78 56 34 12  xV4.xV4.xV4.xV4.
0x0070   78 56 34 12 78 56 34 12  78 56 34 12 78 56 34 12  xV4.xV4.xV4.xV4.
0x0080   78 56 34 12 78 56 34 12  78 56 34 12 78 56 34 12  xV4.xV4.xV4.xV4.
0x0090   78 56 34 12 78 56 34 12  78 56 34 12 78 56 34 12  xV4.xV4.xV4.xV4.
0x00a0   78 56 34 12 78 56 34 12  78 56 34 12 78 56 34 12  xV4.xV4.xV4.xV4.
0x00b0   78 56 34 12 78 56 34 12  78 56 34 12 78 56 34 12  xV4.xV4.xV4.xV4.
0x00c0   78 56 34 12 78 56 34 12  78 56 34 12 78 56 34 12  xV4.xV4.xV4.xV4.
0x00d0   78 56 34 12 78 56 34 12  78 56 34 12 78 56 34 12  xV4.xV4.xV4.xV4.
0x00e0   78 56 34 12 78 56 34 12  78 56 34 12 78 56 34 12  xV4.xV4.xV4.xV4.
0x00f0   78 56 34 12 78 56 34 12  78 56 34 12 78 56 34 12  xV4.xV4.xV4.xV4.
[I] (492) exam_spis: This example succeeds in running.
```

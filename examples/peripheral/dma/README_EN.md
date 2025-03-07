# DMA Demo

## Overview

After starting this application, perform the following actions:

1. Initialize DMA
2. Start the thread and perform m2m DMA related operations within the thread, including interrupt normal mode, loop mode, and non interrupt blocking mode
3. The thread will continue to execute in a loop

## Environmental Requirements

The DMA configuration has been completed in the device tree. If you need to make any changes, you can modify the corresponding device tree settings.

## Building and Flashing

Example Location： `examples/peripheral/dma`

For operations such as compiling and burning, please refer to: [Quick Start Guide](https://doc.winnermicro.net/w800/en/latest/get_started/index.html)

## Running Result

The following log is displayed after successful operation

```
[I] (48) DMA DRV M2M DEMO: ------start dma example: dma demo, dma interrupt without list mode ------
[I] (70) DMA DRV M2M DEMO: src  0x200015a0
[I] (70) DMA DRV M2M DEMO: dest  0x20001470
[I] (70) DMA DRV M2M DEMO: len  96
[I] (70) DMA DRV M2M DEMO: ch  0
[I] (70) DMA DRV M2M DEMO: src_inc_mode  1
[I] (92) DMA DRV M2M DEMO: dest_inc_mode  1
[I] (92) DMA DRV M2M DEMO: int_en  1
[I] (92) DMA DRV M2M DEMO: auto_reload  0
[I] (92) DMA DRV M2M DEMO: dma_mode  0
[I] (92) DMA DRV M2M DEMO: src_warp_len  0
[I] (92) DMA DRV M2M DEMO: dest_warp_len  0
[I] (214) DMA DRV M2M DEMO: dump [[after dma]dma_m2m src data:]
0x0000   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x000f   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x001e   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x002d   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x003c   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x004b   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x005a   3a 3a 3a 3a 3a 3a                              ::::::
[I] (216) DMA DRV M2M DEMO: dump [[after dma]dma_m2m dest buf:]
0x0000   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x000f   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x001e   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x002d   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x003c   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x004b   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x005a   3a 3a 3a 3a 3a 3a                              ::::::
[I] (241) DMA DRV M2M DEMO: dump [[before dma]dma_m2m src data:]
0x0000   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x000f   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x001e   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x002d   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x003c   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x004b   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x005a   3a 3a 3a 3a 3a 3a                              ::::::
[I] (285) DMA DRV M2M DEMO: dump [[before dma]dma_m2m dest buf:]
0x0000   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00  ...............
0x000f   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00  ...............
0x001e   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00  ...............
0x002d   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00  ...............
0x003c   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00  ...............
0x004b   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00  ...............
0x005a   00 00 00 00 00 00                              ......
[I] (329) DMA DRV M2M DEMO: ------start dma example: dma demo, dma polling without list mode ------
[I] (351) DMA DRV M2M DEMO: dump [[after dma]dma_m2m src data:]
0x0000   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x000f   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x001e   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x002d   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x003c   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x004b   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x005a   3a 3a 3a 3a 3a 3a                              ::::::
[I] (397) DMA DRV M2M DEMO: dump [[after dma]dma_m2m dest buf:]
0x0000   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x000f   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x001e   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x002d   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x003c   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x004b   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x005a   3a 3a 3a 3a 3a 3a                              ::::::
[I] (441) DMA DRV M2M DEMO: dma m2m xfer demo finish with success
[I] (441) DMA DRV M2M DEMO: dump [[before dma]dma_m2m src data:]
0x0000   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x000f   3a 3a 3a 3a 3a 3a 3a 3a  3a ee ee ee ee ee ee  :::::::::......
0x001e   ee ee ee ee ee ee ee ee  ee ee ee ee ee ee ee  ...............
0x002d   ee ee ee ee ee ee ee ee  ee ee ee ee ee ee ee  ...............
0x003c   ee ee ee ee ee ee ee ee  ee ee ee ee ee ee ee  ...............
0x004b   ee ee ee ee ee ee ee ee  ee ee ee ee ee ee ee  ...............
0x005a   ee ee ee ee ee ee                              ......
[I] (486) DMA DRV M2M DEMO: dump [[before dma]dma_m2m dest buf:]
0x0000   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00  ...............
0x000f   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00  ...............
0x001e   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00  ...............
0x002d   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00  ...............
0x003c   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00  ...............
0x004b   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00  ...............
0x005a   00 00 00 00 00 00                              ......
[I] (553) DMA DRV M2M DEMO: dump [[before dma]dma_m2m dest buf ref:]
0x0000   ee ee ee ee ee ee ee ee  ee ee ee ee ee ee ee  ...............
0x000f   ee ee ee ee ee ee ee ee  ee 00 00 00 00 00 00  ...............
0x001e   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00  ...............
0x002d   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00  ...............
0x003c   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00  ...............
0x004b   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00  ...............
0x005a   00 00 00 00 00 00                              ......
[I] (598) DMA DRV M2M DEMO: ------start dma example: dma demo, dma interrupt with warp mode ------
[I] (698) DMA DRV M2M DEMO: dump [[after dma]dma_m2m src data:]
0x0000   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x000f   3a 3a 3a 3a 3a 3a 3a 3a  3a ee ee ee ee ee ee  :::::::::......
0x001e   ee ee ee ee ee ee ee ee  ee ee ee ee ee ee ee  ...............
0x002d   ee ee ee ee ee ee ee ee  ee ee ee ee ee ee ee  ...............
0x003c   ee ee ee ee ee ee ee ee  ee ee ee ee ee ee ee  ...............
0x004b   ee ee ee ee ee ee ee ee  ee ee ee ee ee ee ee  ...............
0x005a   ee ee ee ee ee ee                              ......
[I] (700) DMA DRV M2M DEMO: dump [[after dma]dma_m2m dest buf:]
0x0000   ee ee ee ee ee ee ee ee  ee ee ee ee ee ee ee  ...............
0x000f   ee ee ee ee ee ee ee ee  ee 00 00 00 00 00 00  ...............
0x001e   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00  ...............
0x002d   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00  ...............
0x003c   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00  ...............
0x004b   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00  ...............
0x005a   00 00 00 00 00 00                              ......
[I] (725) DMA DRV M2M DEMO: dump [[before dma]dma_m2m src data:]
0x0000   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x000f   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x001e   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x002d   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x003c   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x004b   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x005a   3a 3a 3a 3a 3a 3a                              ::::::
[I] (769) DMA DRV M2M DEMO: dump [[before dma]dma_m2m dest buf:]
0x0000   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00  ...............
0x000f   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00  ...............
0x001e   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00  ...............
0x002d   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00  ...............
0x003c   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00  ...............
0x004b   00 00 00 00 00 00 00 00  00 00 00 00 00 00 00  ...............
0x005a   00 00 00 00 00 00                              ......
[I] (813) DMA DRV M2M DEMO: ------start dma example: dma demo, dma interrupt without list mode transfer api------
[I] (935) DMA DRV M2M DEMO: dump [[after dma]dma_m2m src data:]
0x0000   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x000f   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x001e   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x002d   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x003c   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x004b   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x005a   3a 3a 3a 3a 3a 3a                              ::::::
[I] (937) DMA DRV M2M DEMO: dump [[after dma]dma_m2m dest buf:]
0x0000   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x000f   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x001e   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x002d   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x003c   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x004b   3a 3a 3a 3a 3a 3a 3a 3a  3a 3a 3a 3a 3a 3a 3a  :::::::::::::::
0x005a   3a 3a 3a 3a 3a 3a                              ::::::
[I] (962) DMA DRV M2M DEMO: dma demo done! count 0 
```
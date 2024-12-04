.. _pinmux:

Pinmux
=============

SoC(System on Chip) control peripherals and interact with data through IO pins. The W80X series of SoC come in different packages, offering varying numbers of IO pins. An example is provided below with the pinout diagram of the W800:

.. figure:: ../../_static/component-guides/driver/w800_io.svg
    :align: center
    :alt: w800_io

Considering the relationship between chip package area and cost, the number of chip pins is limited as much as possible. 
To support more functions, some pins are multiplexed for multiple purposes, which is also known as pinmux, as introduced in the current chapter.

IO pinmux definition is crucial in project development but can easily lead to confusion. When obtaining a new platform or initiating new device functions, it is often necessary to first configure the pinmux parameters in the SDK based on the project's functional requirements and in combination with the hardware engineer's schematic.
This is a prerequisite for the proper operation of related device drivers. To avoid confusion, attention can be paid to the following aspects:

.. note::
    1.Ensure that the chip specification book referred to is consistent with the SoC selected for the project.
   
    2.Be cautious that the IO configured in the device table does not exceed the support range of the selected SoC. It is recommended to use pin enumeration definitions when configuring pins so that the compilation system can check for validity.
    
    3.Be aware of whether the IO configured in the device table is redundantly defined in multiple devices.
    
    4.Verify whether the IO attribute configurations in the device table are correct (such as pullup/pulldown, input/output direction, etc.).
    
    5.Check whether the newly added application software in the project has issues similar to those mentioned in points 2, 3, and 4.
    
    In the future, we will improve and provide ways to assist users in detecting these errors in advance.

The following tables are more intuitive Pinmux Tables compiled based on the chip specification books of the W80X series SoC.

The pinmux function multiplexing relationships for W800 and W805 chips
------------------------------------------------------------------------

.. raw:: html

   <table class="docutils align-default">
   <thead>
   <tr class="row-odd">
   <th class="head wm-small-padding2"><p>GPIO_NUM</p></th>
   <th class="head wm-small-padding2"><p>PadName</p></th>
   <th class="head wm-small-padding2"><p>FUN1</p></th>
   <th class="head wm-small-padding2"><p>FUN2</p></th>
   <th class="head wm-small-padding2"><p>FUN3</p></th>
   <th class="head wm-small-padding2"><p>FUN4</p></th>
   <th class="head wm-small-padding2"><p>FUN5</p></th>
   <th class="head wm-small-padding2"><p>FUN6</p></th>
   <th class="head wm-small-padding2"><p>FUN7</p></th>
   </tr>
   </thead>
   <tbody>
   <tr class="row-even">
   <td class="wm-small-padding2"><p>WM_GPIO_NUM_0</p></td>
   <td class="wm-small-padding2"><p>PA0</p></td>
   <td class="wm-small-padding2"><p>I2S_MCLK</p></td>
   <td class="wm-small-padding2"><p>L-SPI_CS</p></td>
   <td class="wm-small-padding2"><p>PWM2</p></td>
   <td class="wm-small-padding2"><p>I2S_DO</p></td>
   <td class="wm-small-padding2"><p>GPIO</p></td>
   <td class="wm-small-padding2"></td>
   <td class="wm-small-padding2"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding2"><p>WM_GPIO_NUM_1</p></td>
   <td class="wm-small-padding2"><p>PA1</p></td>
   <td class="wm-small-padding2"><p>JTAG_CK</p></td>
   <td class="wm-small-padding2"><p>I2C_SCL</p></td>
   <td class="wm-small-padding2"><p>PWM3</p></td>
   <td class="wm-small-padding2"><p>I2S_LRCLK</p></td>
   <td class="wm-small-padding2"><p>GPIO</p></td>
   <td class="wm-small-padding2"><p>ADC_0</p></td>
   <td class="wm-small-padding2"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding2"><p>WM_GPIO_NUM_4</p></td>
   <td class="wm-small-padding2"><p>PA4</p></td>
   <td class="wm-small-padding2"><p>JTAG_SWO</p></td>
   <td class="wm-small-padding2"><p>I2C_SDA</p></td>
   <td class="wm-small-padding2"><p>PWM4</p></td>
   <td class="wm-small-padding2"><p>I2S_BCLK</p></td>
   <td class="wm-small-padding2"><p>GPIO</p></td>
   <td class="wm-small-padding2"><p>ADC_1</p></td>
   <td class="wm-small-padding2"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding2"><p>WM_GPIO_NUM_7</p></td>
   <td class="wm-small-padding2"><p>PA7</p></td>
   <td class="wm-small-padding2"><p>PWM4</p></td>
   <td class="wm-small-padding2"><p>L-SPI_MOSI</p></td>
   <td class="wm-small-padding2"><p>I2S_MCLK</p></td>
   <td class="wm-small-padding2"><p>I2S_DI</p></td>
   <td class="wm-small-padding2"><p>GPIO</p></td>
   <td class="wm-small-padding2"></td>
   <td class="wm-small-padding2"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding2"><p>WM_GPIO_NUM_16</p></td>
   <td class="wm-small-padding2"><p>PB0</p></td>
   <td class="wm-small-padding2"><p>PWM0</p></td>
   <td class="wm-small-padding2"><p>L-SPI_MISO</p></td>
   <td class="wm-small-padding2"><p>UART3_TX</p></td>
   <td class="wm-small-padding2"><p>PSRAM_CK</p></td>
   <td class="wm-small-padding2"><p>GPIO</p></td>
   <td class="wm-small-padding2"></td>
   <td class="wm-small-padding2"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding2"><p>WM_GPIO_NUM_17</p></td>
   <td class="wm-small-padding2"><p>PB1</p></td>
   <td class="wm-small-padding2"><p>PWM1</p></td>
   <td class="wm-small-padding2"><p>L-SPI_CK</p></td>
   <td class="wm-small-padding2"><p>UART3_RX</p></td>
   <td class="wm-small-padding2"><p>PSRAM_CS</p></td>
   <td class="wm-small-padding2"><p>GPIO</p></td>
   <td class="wm-small-padding2"></td>
   <td class="wm-small-padding2"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding2"><p>WM_GPIO_NUM_18</p></td>
   <td class="wm-small-padding2"><p>PB2</p></td>
   <td class="wm-small-padding2"><p>PWM2</p></td>
   <td class="wm-small-padding2"><p>L-SPI_CK</p></td>
   <td class="wm-small-padding2"><p>UART2_TX</p></td>
   <td class="wm-small-padding2"><p>PSRAM_DAT0</p></td>
   <td class="wm-small-padding2"><p>GPIO</p></td>
   <td class="wm-small-padding2"></td>
   <td class="wm-small-padding2"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding2"><p>WM_GPIO_NUM_19</p></td>
   <td class="wm-small-padding2"><p>PB3</p></td>
   <td class="wm-small-padding2"><p>PWM3</p></td>
   <td class="wm-small-padding2"><p>L-SPI_MISO</p></td>
   <td class="wm-small-padding2"><p>UART2_RX</p></td>
   <td class="wm-small-padding2"><p>PSRAM_DAT1</p></td>
   <td class="wm-small-padding2"><p>GPIO</p></td>
   <td class="wm-small-padding2"></td>
   <td class="wm-small-padding2"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding2"><p>WM_GPIO_NUM_20</p></td>
   <td class="wm-small-padding2"><p>PB4</p></td>
   <td class="wm-small-padding2"><p>L-SPI_CS</p></td>
   <td class="wm-small-padding2"><p>UART2_RTS</p></td>
   <td class="wm-small-padding2"><p>UART4_TX</p></td>
   <td class="wm-small-padding2"><p>PSRAM_DAT2</p></td>
   <td class="wm-small-padding2"><p>GPIO</p></td>
   <td class="wm-small-padding2"></td>
   <td class="wm-small-padding2"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding2"><p>WM_GPIO_NUM_21</p></td>
   <td class="wm-small-padding2"><p>PB5</p></td>
   <td class="wm-small-padding2"><p>L-SPI_MOSI</p></td>
   <td class="wm-small-padding2"><p>UART2_CTS</p></td>
   <td class="wm-small-padding2"><p>UART4_RX</p></td>
   <td class="wm-small-padding2"><p>PSRAM_DAT3</p></td>
   <td class="wm-small-padding2"><p>GPIO</p></td>
   <td class="wm-small-padding2"></td>
   <td class="wm-small-padding2"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding2"><p>WM_GPIO_NUM_22</p></td>
   <td class="wm-small-padding2"><p>PB6</p></td>
   <td class="wm-small-padding2"><p>UART1_TX</p></td>
   <td class="wm-small-padding2"><p>MMC_CLK</p></td>
   <td class="wm-small-padding2"><p>H-SPI_CK</p></td>
   <td class="wm-small-padding2"><p>SDIO_SLAVE_CK</p></td>
   <td class="wm-small-padding2"><p>GPIO</p></td>
   <td class="wm-small-padding2"></td>
   <td class="wm-small-padding2"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding2"><p>WM_GPIO_NUM_23</p></td>
   <td class="wm-small-padding2"><p>PB7</p></td>
   <td class="wm-small-padding2"><p>UART1_RX</p></td>
   <td class="wm-small-padding2"><p>MMC_CMD</p></td>
   <td class="wm-small-padding2"><p>H-SPI_INT</p></td>
   <td class="wm-small-padding2"><p>SDIO_SLAVE_CMD</p></td>
   <td class="wm-small-padding2"><p>GPIO</p></td>
   <td class="wm-small-padding2"></td>
   <td class="wm-small-padding2"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding2"><p>WM_GPIO_NUM_24</p></td>
   <td class="wm-small-padding2"><p>PB8</p></td>
   <td class="wm-small-padding2"><p>I2S_BCLK</p></td>
   <td class="wm-small-padding2"><p>MMC_DAT0</p></td>
   <td class="wm-small-padding2"><p>PWM_BREAK</p></td>
   <td class="wm-small-padding2"><p>SDIO_SLAVE_D0</p></td>
   <td class="wm-small-padding2"><p>GPIO</p></td>
   <td class="wm-small-padding2"></td>
   <td class="wm-small-padding2"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding2"><p>WM_GPIO_NUM_25</p></td>
   <td class="wm-small-padding2"><p>PB9</p></td>
   <td class="wm-small-padding2"><p>I2S_LRCLK</p></td>
   <td class="wm-small-padding2"><p>MMC_DAT1</p></td>
   <td class="wm-small-padding2"><p>H-SPI_CS</p></td>
   <td class="wm-small-padding2"><p>SDIO_SLAVE_D1</p></td>
   <td class="wm-small-padding2"><p>GPIO</p></td>
   <td class="wm-small-padding2"></td>
   <td class="wm-small-padding2"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding2"><p>WM_GPIO_NUM_26</p></td>
   <td class="wm-small-padding2"><p>PB10</p></td>
   <td class="wm-small-padding2"><p>I2S_DI</p></td>
   <td class="wm-small-padding2"><p>MMC_DAT2</p></td>
   <td class="wm-small-padding2"><p>H-SPI_DI</p></td>
   <td class="wm-small-padding2"><p>SDIO_SLAVE_D2</p></td>
   <td class="wm-small-padding2"><p>GPIO</p></td>
   <td class="wm-small-padding2"></td>
   <td class="wm-small-padding2"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding2"><p>WM_GPIO_NUM_27</p></td>
   <td class="wm-small-padding2"><p>PB11</p></td>
   <td class="wm-small-padding2"><p>I2S_D0</p></td>
   <td class="wm-small-padding2"><p>MMC_DAT3</p></td>
   <td class="wm-small-padding2"><p>H-SPI_DO</p></td>
   <td class="wm-small-padding2"><p>SDIO_SLAVE_D3</p></td>
   <td class="wm-small-padding2"><p>GPIO</p></td>
   <td class="wm-small-padding2"></td>
   <td class="wm-small-padding2"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding2"><p>WM_GPIO_NUM_35</p></td>
   <td class="wm-small-padding2"><p>PB19</p></td>
   <td class="wm-small-padding2"><p>UART0_TX</p></td>
   <td class="wm-small-padding2"><p>PWM0</p></td>
   <td class="wm-small-padding2"><p>UART1_RTS</p></td>
   <td class="wm-small-padding2"><p>I2C_SDA</p></td>
   <td class="wm-small-padding2"><p>GPIO</p></td>
   <td class="wm-small-padding2"></td>
   <td class="wm-small-padding2"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding2"><p>WM_GPIO_NUM_36</p></td>
   <td class="wm-small-padding2"><p>PB20</p></td>
   <td class="wm-small-padding2"><p>UART0_RX</p></td>
   <td class="wm-small-padding2"><p>PWM1</p></td>
   <td class="wm-small-padding2"><p>UART1_CTS</p></td>
   <td class="wm-small-padding2"><p>I2C_SCL</p></td>
   <td class="wm-small-padding2"><p>GPIO</p></td>
   <td class="wm-small-padding2"></td>
   <td class="wm-small-padding2"></td>
   </tr>
   </tbody>
   </table>


The pinmux function multiplexing relationships for W801S chip
------------------------------------------------------------------
.. raw:: html

   <table class="docutils align-default">
   <thead>
   <tr class="row-odd">
   <th class="head wm-small-padding"><p class="wm-small-font">GPIO_NUM</p></th>
   <th class="head wm-small-padding"><p class="wm-small-font">PadName</p></th>
   <th class="head wm-small-padding"><p class="wm-small-font">FUN1</p></th>
   <th class="head wm-small-padding"><p class="wm-small-font">FUN2</p></th>
   <th class="head wm-small-padding"><p class="wm-small-font">FUN3</p></th>
   <th class="head wm-small-padding"><p class="wm-small-font">FUN4</p></th>
   <th class="head wm-small-padding"><p class="wm-small-font">FUN5</p></th>
   <th class="head wm-small-padding"><p class="wm-small-font">FUN6</p></th>
   <th class="head wm-small-padding"><p class="wm-small-font">FUN7</p></th>
   </tr>
   </thead>
   <tbody>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_0</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PA0</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_MCLK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_CS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM2</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_DO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_1</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PA1</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">JTAG_CK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2C_SCL</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM3</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_LRCLK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">ADC_0</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_2</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PA2</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART1_RTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART2_TX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM0</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART3_RTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">ADC_3</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_3</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PA3</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART1_CTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART2_RX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM1</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART3_CTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">ADC_2</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_4</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PA4</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">JTAG_SWO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2C_SDA</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM4</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_BCLK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">ADC_1</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_5</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PA5</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART3_TX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART2_RTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM_BREAK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART4_RTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_6</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PA6</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART3_RX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART2_CTS</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART4_CTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG31</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_7</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PA7</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM4</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_MOSI</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_MCLK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_DI</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG03</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_8</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PA8</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM_BREAK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART4_TX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART5_TX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_BCLK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG04</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_9</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PA9</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">MMC_CLK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART4_RX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART5_RX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_LRCLK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG05</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">Touch1</p></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_10</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PA10</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">MMC_CMD</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART4_RTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM0</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_DO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG06</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">Touch2</p></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_11</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PA11</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">MMC_DAT0</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART4_CTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM1</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_DI</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG07</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_12</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PA12</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">MMC_DAT1</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART5_TX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM2</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG08</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">CMOD</p></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_13</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PA13</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">MMC_DAT2</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART5_RX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM3</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG09</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_14</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PA14</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">MMC_DAT3</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART5_CTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM4</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG10</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">CDC</p></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_15</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PA15</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PSRAM_CK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART5_RTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM_BREAK</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG11</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_16</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB0</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM0</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_MISO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART3_TX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PSRAM_CK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG12</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">Touch3</p></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_17</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB1</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM1</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_CK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART3_RX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PSRAM_CS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG13</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">Touch4</p></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_18</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB2</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM2</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_CK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART2_TX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PSRAM_DAT0</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG14</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">Touch5</p></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_19</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB3</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM3</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_MISO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART2_RX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PSRAM_DAT1</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG15</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">Touch6</p></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_20</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB4</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_CS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART2_RTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART4_TX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PSRAM_DAT2</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG16</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">Touch7</p></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_21</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB5</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_MOSI</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART2_CTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART4_RX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PSRAM_DAT3</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG17</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">Touch8</p></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_22</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB6</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART1_TX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">MMC_CLK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">H-SPI_CK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">SDIO_SLAVE_CK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG18</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">Touch9</p></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_23</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB7</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART1_RX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">MMC_CMD</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">H-SPI_INT</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">SDIO_SLAVE_CMD</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG19</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">Touch10</p></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_24</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB8</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_BCLK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">MMC_DAT0</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM_BREAK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">SDIO_SLAVE_D0</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG20</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">Touch11</p></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_25</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB9</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_LRCLK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">MMC_DAT1</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">H-SPI_CS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">SDIO_SLAVE_D1</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG21</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">Touch12</p></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_26</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB10</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_DI</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">MMC_DAT2</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">H-SPI_DI</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">SDIO_SLAVE_D2</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG22</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_27</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB11</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_D0</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">MMC_DAT3</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">H-SPI_DO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">SDIO_SLAVE_D3</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG23</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_28</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB12</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">H-SPI_CK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM0</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART5_CTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_BCLK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG24</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_29</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB13</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">H-SPI_INT</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM1</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART5_RTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_LRCLK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG25</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_30</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB14</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">HSPI_CS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM2</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_CS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_DO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG26</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_31</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB15</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">HSPI_DI</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM3</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_CK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_DI</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG27</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_32</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB16</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">HSPI_DO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM4</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_MISO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART1_RX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG28</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_33</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB17</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART5_RX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM_BREAK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_MOSI</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_MCLK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG29</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_34</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB18</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART5_TX</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG30</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_35</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB19</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART0_TX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM0</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART1_RTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2C_SDA</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_36</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB20</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART0_RX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM1</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART1_CTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2C_SCL</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_37</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB21</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART0_RTS</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_COM1</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_38</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB22</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART0_CTS</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_COM2</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_40</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB24</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_CK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM2</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG2</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_41</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB25</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_MISO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM3</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_COM0</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_42</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB26</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_MOSI</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM4</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG1</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_43</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB27</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PSRAM_CS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART0_TX</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_COM3</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   </tbody>
   </table>
 
 
The pinmux function multiplexing relationships for W802 chip
------------------------------------------------------------------------

.. raw:: html

   <table class="docutils align-default">
   <thead>
   <tr class="row-odd">
   <th class="head wm-small-padding"><p class="wm-small-font">GPIO_NUM</p></th>
   <th class="head wm-small-padding"><p class="wm-small-font">PadName</p></th>
   <th class="head wm-small-padding"><p class="wm-small-font">FUN1</p></th>
   <th class="head wm-small-padding"><p class="wm-small-font">FUN2</p></th>
   <th class="head wm-small-padding"><p class="wm-small-font">FUN3</p></th>
   <th class="head wm-small-padding"><p class="wm-small-font">FUN4</p></th>
   <th class="head wm-small-padding"><p class="wm-small-font">FUN5</p></th>
   <th class="head wm-small-padding"><p class="wm-small-font">FUN6</p></th>
   <th class="head wm-small-padding"><p class="wm-small-font">FUN7</p></th>
   </tr>
   </thead>
   <tbody>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_0</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PA0</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_MCLK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_CS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM2</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_DO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_1</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PA1</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">JTAG_CK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2C_SCL</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM3</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_LRCLK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">ADC_0</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_2</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PA2</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART1_RTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART2_TX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM0</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART3_RTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">ADC_3</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_3</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PA3</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART1_CTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART2_RX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM1</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART3_CTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">ADC_2</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_4</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PA4</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">JTAG_SWO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2C_SDA</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM4</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_BCLK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">ADC_1</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_7</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PA7</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM4</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_MOSI</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_MCLK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_DI</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_9</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PA9</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">MMC_CLK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART4_RX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART5_RX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_LRCLK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">Touch1</p></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_10</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PA10</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">MMC_CMD</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART4_RTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM0</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_DO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">Touch2</p></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_12</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PA12</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">MMC_DAT1</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART5_TX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM2</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">CMOD</p></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_14</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PA14</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">MMC_DAT3</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART5_CTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM4</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">CDC</p></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_16</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB0</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM0</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_MISO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART3_TX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PSRAM_CK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">Touch3</p></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_17</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB1</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM1</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_CK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART3_RX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PSRAM_CS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">Touch4</p></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_18</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB2</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM2</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_CK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART2_TX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PSRAM_DAT0</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">Touch5</p></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_19</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB3</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM3</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_MISO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART2_RX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PSRAM_DAT1</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">Touch6</p></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_20</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB4</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_CS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART2_RTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART4_TX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PSRAM_DAT2</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">Touch7</p></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_21</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB5</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_MOSI</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART2_CTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART4_RX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PSRAM_DAT3</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">Touch8</p></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_22</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB6</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART1_TX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">MMC_CLK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">H-SPI_CK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">SDIO_SLAVE_CK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">Touch9</p></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_23</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB7</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART1_RX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">MMC_CMD</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">H-SPI_INT</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">SDIO_SLAVE_CMD</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">Touch10</p></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_24</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB8</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_BCLK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">MMC_DAT0</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM_BREAK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">SDIO_SLAVE_D0</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">Touch11</p></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_25</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB9</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_LRCLK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">MMC_DAT1</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">H-SPI_CS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">SDIO_SLAVE_D1</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">Touch12</p></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_26</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB10</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_DI</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">MMC_DAT2</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">H-SPI_DI</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">SDIO_SLAVE_D2</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_27</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB11</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_D0</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">MMC_DAT3</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">H-SPI_DO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">SDIO_SLAVE_D3</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_28</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB12</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">H-SPI_CK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM0</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART5_CTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_BCLK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_29</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB13</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">H-SPI_INT</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM1</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART5_RTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_LRCLK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_30</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB14</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">HSPI_CS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM2</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_CS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_DO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_31</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB15</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">HSPI_DI</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM3</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_CK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_DI</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_32</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB16</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">HSPI_DO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM4</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_MISO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART1_RX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_33</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB17</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART5_RX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM_BREAK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_MOSI</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_MCLK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_34</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB18</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART5_TX</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_35</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB19</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART0_TX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM0</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART1_RTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2C_SDA</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_36</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB20</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART0_RX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM1</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART1_CTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2C_SCL</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_37</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB21</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART0_RTS</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_38</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB22</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART0_CTS</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_40</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB24</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_CK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM2</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_41</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB25</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_MISO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM3</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_42</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB26</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_MOSI</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM4</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_45</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB29</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PSRAM_DAT1_1</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART0_RTS_1</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">Touch15</p></td>
   </tr>
   </tbody>
   </table>


The pinmux function multiplexing relationships for W803 chip
------------------------------------------------------------------

.. raw:: html

   <table class="docutils align-default">
   <thead>
   <tr class="row-odd">
   <th class="head wm-small-padding3"><p>GPIO_NUM</p></th>
   <th class="head wm-small-padding3"><p>PadName</p></th>
   <th class="head wm-small-padding3"><p>FUN1</p></th>
   <th class="head wm-small-padding3"><p>FUN2</p></th>
   <th class="head wm-small-padding3"><p>FUN3</p></th>
   <th class="head wm-small-padding3"><p>FUN4</p></th>
   <th class="head wm-small-padding3"><p>FUN5</p></th>
   <th class="head wm-small-padding3"><p>FUN6</p></th>
   <th class="head wm-small-padding3"><p>FUN7</p></th>
   </tr>
   </thead>
   <tbody>
   <tr class="row-even">
   <td class="wm-small-padding3"><p>WM_GPIO_NUM_0</p></td>
   <td class="wm-small-padding3"><p>PA0</p></td>
   <td class="wm-small-padding3"><p>I2S_MCLK</p></td>
   <td class="wm-small-padding3"><p>L-SPI_CS</p></td>
   <td class="wm-small-padding3"><p>PWM2</p></td>
   <td class="wm-small-padding3"><p>I2S_DO</p></td>
   <td class="wm-small-padding3"><p>GPIO</p></td>
   <td class="wm-small-padding3"></td>
   <td class="wm-small-padding3"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding3"><p>WM_GPIO_NUM_1</p></td>
   <td class="wm-small-padding3"><p>PA1</p></td>
   <td class="wm-small-padding3"><p>JTAG_CK</p></td>
   <td class="wm-small-padding3"><p>I2C_SCL</p></td>
   <td class="wm-small-padding3"><p>PWM3</p></td>
   <td class="wm-small-padding3"><p>I2S_LRCLK</p></td>
   <td class="wm-small-padding3"><p>GPIO</p></td>
   <td class="wm-small-padding3"><p>ADC_0</p></td>
   <td class="wm-small-padding3"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding3"><p>WM_GPIO_NUM_4</p></td>
   <td class="wm-small-padding3"><p>PA4</p></td>
   <td class="wm-small-padding3"><p>JTAG_SWO</p></td>
   <td class="wm-small-padding3"><p>I2C_SDA</p></td>
   <td class="wm-small-padding3"><p>PWM4</p></td>
   <td class="wm-small-padding3"><p>I2S_BCLK</p></td>
   <td class="wm-small-padding3"><p>GPIO</p></td>
   <td class="wm-small-padding3"><p>ADC_1</p></td>
   <td class="wm-small-padding3"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding3"><p>WM_GPIO_NUM_7</p></td>
   <td class="wm-small-padding3"><p>PA7</p></td>
   <td class="wm-small-padding3"><p>PWM4</p></td>
   <td class="wm-small-padding3"><p>L-SPI_MOSI</p></td>
   <td class="wm-small-padding3"><p>I2S_MCLK</p></td>
   <td class="wm-small-padding3"><p>I2S_DI</p></td>
   <td class="wm-small-padding3"><p>GPIO</p></td>
   <td class="wm-small-padding3"></td>
   <td class="wm-small-padding3"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding3"><p>WM_GPIO_NUM_12</p></td>
   <td class="wm-small-padding3"><p>PA12</p></td>
   <td class="wm-small-padding3"></td>
   <td class="wm-small-padding3"><p>UART5_TX</p></td>
   <td class="wm-small-padding3"><p>PWM2</p></td>
   <td class="wm-small-padding3"></td>
   <td class="wm-small-padding3"><p>GPIO</p></td>
   <td class="wm-small-padding3"></td>
   <td class="wm-small-padding3"><p>CMOD</p></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding3"><p>WM_GPIO_NUM_14</p></td>
   <td class="wm-small-padding3"><p>PA14</p></td>
   <td class="wm-small-padding3"></td>
   <td class="wm-small-padding3"><p>UART5_CTS</p></td>
   <td class="wm-small-padding3"><p>PWM4</p></td>
   <td class="wm-small-padding3"></td>
   <td class="wm-small-padding3"><p>GPIO</p></td>
   <td class="wm-small-padding3"></td>
   <td class="wm-small-padding3"><p>CDC</p></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding3"><p>WM_GPIO_NUM_16</p></td>
   <td class="wm-small-padding3"><p>PB0</p></td>
   <td class="wm-small-padding3"><p>PWM0</p></td>
   <td class="wm-small-padding3"><p>L-SPI_MISO</p></td>
   <td class="wm-small-padding3"><p>UART3_TX</p></td>
   <td class="wm-small-padding3"><p>PSRAM_CK</p></td>
   <td class="wm-small-padding3"><p>GPIO</p></td>
   <td class="wm-small-padding3"></td>
   <td class="wm-small-padding3"><p>Touch3</p></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding3"><p>WM_GPIO_NUM_17</p></td>
   <td class="wm-small-padding3"><p>PB1</p></td>
   <td class="wm-small-padding3"><p>PWM1</p></td>
   <td class="wm-small-padding3"><p>L-SPI_CK</p></td>
   <td class="wm-small-padding3"><p>UART3_RX</p></td>
   <td class="wm-small-padding3"><p>PSRAM_CS</p></td>
   <td class="wm-small-padding3"><p>GPIO</p></td>
   <td class="wm-small-padding3"></td>
   <td class="wm-small-padding3"><p>Touch4</p></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding3"><p>WM_GPIO_NUM_18</p></td>
   <td class="wm-small-padding3"><p>PB2</p></td>
   <td class="wm-small-padding3"><p>PWM2</p></td>
   <td class="wm-small-padding3"><p>L-SPI_CK</p></td>
   <td class="wm-small-padding3"><p>UART2_TX</p></td>
   <td class="wm-small-padding3"><p>PSRAM_DAT0</p></td>
   <td class="wm-small-padding3"><p>GPIO</p></td>
   <td class="wm-small-padding3"></td>
   <td class="wm-small-padding3"><p>Touch5</p></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding3"><p>WM_GPIO_NUM_19</p></td>
   <td class="wm-small-padding3"><p>PB3</p></td>
   <td class="wm-small-padding3"><p>PWM3</p></td>
   <td class="wm-small-padding3"><p>L-SPI_MISO</p></td>
   <td class="wm-small-padding3"><p>UART2_RX</p></td>
   <td class="wm-small-padding3"><p>PSRAM_DAT1</p></td>
   <td class="wm-small-padding3"><p>GPIO</p></td>
   <td class="wm-small-padding3"></td>
   <td class="wm-small-padding3"><p>Touch6</p></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding3"><p>WM_GPIO_NUM_20</p></td>
   <td class="wm-small-padding3"><p>PB4</p></td>
   <td class="wm-small-padding3"><p>L-SPI_CS</p></td>
   <td class="wm-small-padding3"><p>UART2_RTS</p></td>
   <td class="wm-small-padding3"><p>UART4_TX</p></td>
   <td class="wm-small-padding3"><p>PSRAM_DAT2</p></td>
   <td class="wm-small-padding3"><p>GPIO</p></td>
   <td class="wm-small-padding3"></td>
   <td class="wm-small-padding3"><p>Touch7</p></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding3"><p>WM_GPIO_NUM_21</p></td>
   <td class="wm-small-padding3"><p>PB5</p></td>
   <td class="wm-small-padding3"><p>L-SPI_MOSI</p></td>
   <td class="wm-small-padding3"><p>UART2_CTS</p></td>
   <td class="wm-small-padding3"><p>UART4_RX</p></td>
   <td class="wm-small-padding3"><p>PSRAM_DAT3</p></td>
   <td class="wm-small-padding3"><p>GPIO</p></td>
   <td class="wm-small-padding3"></td>
   <td class="wm-small-padding3"><p>Touch8</p></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding3"><p>WM_GPIO_NUM_22</p></td>
   <td class="wm-small-padding3"><p>PB6</p></td>
   <td class="wm-small-padding3"><p>UART1_TX</p></td>
   <td class="wm-small-padding3"><p>MMC_CLK</p></td>
   <td class="wm-small-padding3"><p>H-SPI_CK</p></td>
   <td class="wm-small-padding3"><p>SDIO_SLAVE_CK</p></td>
   <td class="wm-small-padding3"><p>GPIO</p></td>
   <td class="wm-small-padding3"></td>
   <td class="wm-small-padding3"><p>Touch9</p></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding3"><p>WM_GPIO_NUM_23</p></td>
   <td class="wm-small-padding3"><p>PB7</p></td>
   <td class="wm-small-padding3"><p>UART1_RX</p></td>
   <td class="wm-small-padding3"><p>MMC_CMD</p></td>
   <td class="wm-small-padding3"><p>H-SPI_INT</p></td>
   <td class="wm-small-padding3"><p>SDIO_SLAVE_CMD</p></td>
   <td class="wm-small-padding3"><p>GPIO</p></td>
   <td class="wm-small-padding3"></td>
   <td class="wm-small-padding3"><p>Touch10</p></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding3"><p>WM_GPIO_NUM_24</p></td>
   <td class="wm-small-padding3"><p>PB8</p></td>
   <td class="wm-small-padding3"><p>I2S_BCLK</p></td>
   <td class="wm-small-padding3"><p>MMC_DAT0</p></td>
   <td class="wm-small-padding3"><p>PWM_BREAK</p></td>
   <td class="wm-small-padding3"><p>SDIO_SLAVE_D0</p></td>
   <td class="wm-small-padding3"><p>GPIO</p></td>
   <td class="wm-small-padding3"></td>
   <td class="wm-small-padding3"><p>Touch11</p></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding3"><p>WM_GPIO_NUM_25</p></td>
   <td class="wm-small-padding3"><p>PB9</p></td>
   <td class="wm-small-padding3"><p>I2S_LRCLK</p></td>
   <td class="wm-small-padding3"><p>MMC_DAT1</p></td>
   <td class="wm-small-padding3"><p>H-SPI_CS</p></td>
   <td class="wm-small-padding3"><p>SDIO_SLAVE_D1</p></td>
   <td class="wm-small-padding3"><p>GPIO</p></td>
   <td class="wm-small-padding3"></td>
   <td class="wm-small-padding3"><p>Touch12</p></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding3"><p>WM_GPIO_NUM_26</p></td>
   <td class="wm-small-padding3"><p>PB10</p></td>
   <td class="wm-small-padding3"><p>I2S_DI</p></td>
   <td class="wm-small-padding3"><p>MMC_DAT2</p></td>
   <td class="wm-small-padding3"><p>H-SPI_DI</p></td>
   <td class="wm-small-padding3"><p>SDIO_SLAVE_D2</p></td>
   <td class="wm-small-padding3"><p>GPIO</p></td>
   <td class="wm-small-padding3"></td>
   <td class="wm-small-padding3"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding3"><p>WM_GPIO_NUM_27</p></td>
   <td class="wm-small-padding3"><p>PB11</p></td>
   <td class="wm-small-padding3"><p>I2S_D0</p></td>
   <td class="wm-small-padding3"><p>MMC_DAT3</p></td>
   <td class="wm-small-padding3"><p>H-SPI_DO</p></td>
   <td class="wm-small-padding3"><p>SDIO_SLAVE_D3</p></td>
   <td class="wm-small-padding3"><p>GPIO</p></td>
   <td class="wm-small-padding3"></td>
   <td class="wm-small-padding3"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding3"><p>WM_GPIO_NUM_35</p></td>
   <td class="wm-small-padding3"><p>PB19</p></td>
   <td class="wm-small-padding3"><p>UART0_TX</p></td>
   <td class="wm-small-padding3"><p>PWM0</p></td>
   <td class="wm-small-padding3"><p>UART1_RTS</p></td>
   <td class="wm-small-padding3"><p>I2C_SDA</p></td>
   <td class="wm-small-padding3"><p>GPIO</p></td>
   <td class="wm-small-padding3"></td>
   <td class="wm-small-padding3"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding3"><p>WM_GPIO_NUM_36</p></td>
   <td class="wm-small-padding3"><p>PB20</p></td>
   <td class="wm-small-padding3"><p>UART0_RX</p></td>
   <td class="wm-small-padding3"><p>PWM1</p></td>
   <td class="wm-small-padding3"><p>UART1_CTS</p></td>
   <td class="wm-small-padding3"><p>I2C_SCL</p></td>
   <td class="wm-small-padding3"><p>GPIO</p></td>
   <td class="wm-small-padding3"></td>
   <td class="wm-small-padding3"></td>
   </tr>
   </tbody>
   </table>

The pinmux function multiplexing relationships for W806 chip
------------------------------------------------------------------------

.. raw:: html

   <table class="docutils align-default">
   <thead>
   <tr class="row-odd">
   <th class="head wm-small-padding"><p class="wm-small-font">GPIO_NUM</p></th>
   <th class="head wm-small-padding"><p class="wm-small-font">PadName</p></th>
   <th class="head wm-small-padding"><p class="wm-small-font">FUN1</p></th>
   <th class="head wm-small-padding"><p class="wm-small-font">FUN2</p></th>
   <th class="head wm-small-padding"><p class="wm-small-font">FUN3</p></th>
   <th class="head wm-small-padding"><p class="wm-small-font">FUN4</p></th>
   <th class="head wm-small-padding"><p class="wm-small-font">FUN5</p></th>
   <th class="head wm-small-padding"><p class="wm-small-font">FUN6</p></th>
   <th class="head wm-small-padding"><p class="wm-small-font">FUN7</p></th>
   </tr>
   </thead>
   <tbody>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_0</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PA0</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_MCLK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_CS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM2</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_DO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_1</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PA1</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">JTAG_CK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2C_SCL</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM3</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_LRCLK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">ADC_0</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_2</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PA2</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART1_RTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART2_TX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM0</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART3_RTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">ADC_3</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_3</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PA3</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART1_CTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART2_RX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM1</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART3_CTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">ADC_2</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_4</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PA4</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">JTAG_SWO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2C_SDA</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM4</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_BCLK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">ADC_1</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_5</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PA5</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART3_TX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART2_RTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM_BREAK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART4_RTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_6</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PA6</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART3_RX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART2_CTS</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART4_CTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG31</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_7</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PA7</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM4</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_MOSI</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_MCLK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_DI</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG03</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_8</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PA8</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM_BREAK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART4_TX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART5_TX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_BCLK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG04</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_9</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PA9</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">MMC_CLK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART4_RX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART5_RX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_LRCLK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG05</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_10</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PA10</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">MMC_CMD</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART4_RTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM0</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_DO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG06</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_11</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PA11</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">MMC_DAT0</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART4_CTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM1</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_DI</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG07</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_12</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PA12</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">MMC_DAT1</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART5_TX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM2</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG08</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_13</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PA13</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">MMC_DAT2</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART5_RX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM3</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG09</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_14</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PA14</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">MMC_DAT3</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART5_CTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM4</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG10</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_15</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PA15</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PSRAM_CK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART5_RTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM_BREAK</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG11</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_16</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB0</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM0</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_MISO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART3_TX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PSRAM_CK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG12</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_17</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB1</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM1</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_CK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART3_RX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PSRAM_CS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG13</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_18</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB2</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM2</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_CK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART2_TX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PSRAM_DAT0</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG14</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_19</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB3</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM3</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_MISO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART2_RX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PSRAM_DAT1</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG15</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_20</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB4</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_CS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART2_RTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART4_TX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PSRAM_DAT2</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG16</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_21</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB5</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_MOSI</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART2_CTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART4_RX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PSRAM_DAT3</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG17</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_22</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB6</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART1_TX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">MMC_CLK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">H-SPI_CK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">SDIO_SLAVE_CK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG18</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_23</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB7</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART1_RX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">MMC_CMD</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">H-SPI_INT</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">SDIO_SLAVE_CMD</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG19</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_24</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB8</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_BCLK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">MMC_DAT0</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM_BREAK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">SDIO_SLAVE_D0</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG20</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_25</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB9</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_LRCLK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">MMC_DAT1</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">H-SPI_CS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">SDIO_SLAVE_D1</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG21</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_26</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB10</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_DI</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">MMC_DAT2</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">H-SPI_DI</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">SDIO_SLAVE_D2</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG22</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_27</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB11</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_D0</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">MMC_DAT3</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">H-SPI_DO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">SDIO_SLAVE_D3</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG23</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_28</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB12</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">H-SPI_CK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM0</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART5_CTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_BCLK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG24</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_29</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB13</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">H-SPI_INT</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM1</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART5_RTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_LRCLK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG25</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_30</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB14</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">HSPI_CS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM2</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_CS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_DO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG26</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_31</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB15</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">HSPI_DI</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM3</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_CK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_DI</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG27</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_32</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB16</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">HSPI_DO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM4</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_MISO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART1_RX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG28</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_33</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB17</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART5_RX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM_BREAK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_MOSI</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2S_MCLK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG29</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_34</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB18</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART5_TX</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG30</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_35</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB19</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART0_TX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM0</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART1_RTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2C_SDA</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_36</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB20</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART0_RX</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM1</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART1_CTS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">I2C_SCL</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_37</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB21</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART0_RTS</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_COM1</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_38</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB22</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART0_CTS</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_COM2</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_40</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB24</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_CK</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM2</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG2</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_41</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB25</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_MISO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM3</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_COM0</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-even">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_42</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB26</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">L-SPI_MOSI</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PWM4</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_SEG1</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   <tr class="row-odd">
   <td class="wm-small-padding"><p class="wm-small-font">WM_GPIO_NUM_43</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PB27</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">PSRAM_CS</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">UART0_TX</p></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"></td>
   <td class="wm-small-padding"><p class="wm-small-font">GPIO</p></td>
   <td class="wm-small-padding"><p class="wm-small-font">LCD_COM3</p></td>
   <td class="wm-small-padding"></td>
   </tr>
   </tbody>
   </table>


The requirements for setting the attributes of pins for IO function
-----------------------------------------------------------------------
By configuring pinmux, the IO can be selected to perform different functions, and some of these functions have specific requirements for the attributes of the pins. 
The following table summarizes the specific IO attribute requirements for each IO function of the W80X SoC:


=================  ===========================  ========================  ========================
IO Function        Sub-Function Requirements        Pullmode                   Direction
=================  ===========================  ========================  ========================
LCD_SEG                                             FLOAT                     INPUT
LCD_COM                                             FLOAT                     INPUT
ADC                                                 FLOAT                     INPUT
Touch                                               FLOAT                     INPUT
I2C_SDA                                             PULLUP                    Irrelevant
I2C_SCL                                             PULLUP                    Irrelevant
PWM_BREAK           High Level Trigger              PULLDOWN                  Irrelevant
PWM_BREAK           Low Level Trigger               PULLUP                    Irrelevant
UART RX                                             PULLUP                    Irrelevant
GPIO                Input                           PULLUP/DOWN/FLOAT         INPUT
GPIO                Output                          Irrelevant                OUTPUT
I2S_LRCLK           Sequence L->R                   PULLUP                    Irrelevant
I2S_LRCLK           Sequence R->L                   PULLDOWN                  Irrelevant
I2S BCLK/DI/DO                                      Irrelevant                Irrelevant
SDIO                                                Irrelevant                Irrelevant
HSPI                                                Irrelevant                Irrelevant
SIM                                                 Irrelevant                Irrelevant
PWM                                                 Irrelevant                Irrelevant
MMC                                                 Irrelevant                Irrelevant
Uart TX/RTS/CTS                                     Irrelevant                Irrelevant
JTAG                                                Irrelevant                Irrelevant
=================  ===========================  ========================  ========================

For example, WM_GPIO_NUM_1 is configured as FUN6, which corresponds to the ADC function. 
Additionally, it is necessary to set the IO attribute to FLOAT and configure the GPIO Direction as INPUT.

.. note::
   1.Upon power-on, all IOs on the W80X series SoC are defaulted to a Floating, Input state.

   2.When configuring an IO to FUN6 or FUN7, this IO is set to an analog function. Under such conditions, the Pullmode of this IO should not be set to pullup, as it may cause damage to the chip in some environments.

   3.For PWM applications, attention should be paid to IO configuration. Each channel should only have one pin function configured to maintain a single output path. If multiple pins are set to the same PWM channel (for example, both GPIO2 and GPIO10 are set to PWM0), all these pins will output the PWM0 waveform.
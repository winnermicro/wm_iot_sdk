.. _pinmux:

Pinmux
=============

SoC 通过 IO 引脚进行外设控制和数据交互，W80X 系列的 SOC 包含有不同封装，引出不同数量的 IO, 举例下图为 W800 的引脚图：

.. figure:: ../../_static/component-guides/driver/w800_io.svg
    :align: center
    :alt: w800_io

考虑到芯片封装面积及成本关系，芯片的引脚数量会做尽可能的做限制，而为了多支持一些功能，一些引脚会被复用成多种功能，也即
当前章节所介绍的 pinmux 引脚复用。

IO pinmux 定义是项目开发非常重要，但很容易混乱的地方。拿到新的平台或启动新的设备功能，往往要先依据项目功能需求，结合
硬件工程师的原理图，在SDK中配置好 pinmux 参数，这是相关设备驱动能正常运行的前提。
为了避免混乱，可以注意如下一些地方：

.. note::
    1.注意 所参考的 芯片规格书 务必与项目所选型的 SoC 一致。

    2.注意 设备表 中所配置的 IO 是否超出 了所选择 SOC 的支持的范围。建议配置引脚时使用引脚枚举定义，以便编译系统能够检查是否合法。

    3.注意 设备表 中所配置的 IO 是否在多个 device 中被重复定义。

    4.注意 设备表 中对 IO 的属性配置是否正确 (如 pullup/pulldown, input/output方向 等)。  

    5.注意 项目新增加的应用软件中 是否存在上述 2，3，4 的问题。  

    未来我们会完善并提供一些途径协助用户 预先检测出这些错误。

如下几个表格是 依据 W80X 系列 SoC 的芯片规格书 整理出的 更加直观的 Pinmux Table.

W800, W805 芯片的 pinmux 功能复用关系
-----------------------------------------

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


W801S 芯片的 pinmux 功能复用关系
------------------------------------

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

W802 芯片的 pinmux 功能复用关系
------------------------------------

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


W803 芯片的 pinmux 功能复用关系
------------------------------------

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

W806 芯片的 pinmux 功能复用关系
------------------------------------

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


IO function 对 pin 的属性设定要求
---------------------------------

IO 通过 pinmux 配置 选择成 不同的function， 部分 function 对 pin 的属性设定会有要求，
如下表格 汇总了 W80X SoC 各个 IO functioin 的具体 IO 属性要求：

=================  ========================  ========================  ========================
IO Function         子功能需求                 Pullmode                  Direction
=================  ========================  ========================  ========================
LCD_SEG                                       FLOAT                     INPUT
LCD_COM                                       FLOAT                     INPUT
ADC                                           FLOAT                     INPUT
Touch                                         FLOAT                     INPUT
I2C_SDA                                       PULLUP                    无关
I2C_SCL                                       PULLUP                    无关
PWM_BREAK           高电平触发                 PULLDOWN                  无关
PWM_BREAK           低电平触发                 PULLUP                    无关
UART RX                                       PULLUP                    无关
GPIO                Input                     PULLUP/DOWN/FLOAT         INPUT
GPIO                Output                    无关                       OUTPUT
I2S_LRCLK           顺序 L->R                 PULLUP                     无关
I2S_LRCLK           顺序 R->L                 PULLDOWN                   无关
I2S BCLK/DI/DO                                无关                       无关
SDIO                                          无关                       无关
HSPI                                          无关                       无关
SIM                                           无关                       无关
PWM                                           无关                       无关
MMC                                           无关                       无关
Uart TX/RTS/CTS                               无关                       无关
JTAG                                          无关                       无关
=================  ========================  ========================  ========================

比如：WM_GPIO_NUM_1 被配置成 FUN6， 即 ADC 功能，也需要配合将 IO 属性配置成 FLOAT, 将 GPIO Direction 配置成 INPUT。

.. note::
    1.W80X 系列 SoC 上电后，所有 IO 默认为 Floating, Input 状态。

    2.IO 配置成 FUN6, FUN7 时，是将此 IO 设定成模拟功能。这种条件下 该 IO 的 Pullmode 不要设定 pullup , 否则可能在一些环境中造成芯片损坏。

    3.PWM 应用层需要注意 IO 配置，1个 channel 仅1个 pin func 配置，保持仅一路输出，若多个 pin 设定成同1个 PWM Channel (如：GPIO2, GPIO10 都设定成 PWM0), 这些 pin 都会有PWM0波形的输出。

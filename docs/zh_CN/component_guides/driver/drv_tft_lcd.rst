.. _tft_lcd:

TFT_LCD
=============

简介
-------------

薄膜晶体管液晶显示器 (Thin-Film Transistor Liquid Crystal Display，简称 TFT-LCD) 是一种利用液晶材料的电光效应来显示图像的显示器。
TFT-LCD 技术是目前广泛应用于电视、电脑显示器、手机、平板电脑等设备中的显示技术。

TFT-LCD 显示器由多层结构组成，包括背光层、偏振层、彩色滤光片、液晶层、薄膜晶体管阵列以及前面板等，其工作原理是，背光层发出光线，通过偏振层后，光线的偏振方向被改变。
液晶层中的液晶分子在电场的作用下改变排列，进而改变光线的偏振状态。薄膜晶体管阵列控制每个像素点上的电压，从而控制液晶分子的排列，实现图像的显示。

Display 模块分层结构
----------------------

WM IOT SDK 的 Display 模块分为4个功能层:

  1. 应用层 (用户基于中间层，或直接基于驱动层开发的应用)。
  2. 中间层 (集成 LVGL 实现点，线等功能)。
  3. 驱动层 (用于各屏的设备驱动实现)。
  4. 通信层 (如 SDIO，SPI，I2C 等，W80X 不支持 RGB，I80，MIPI 接口)。


驱动层 功能列表
-------------------

- 支持 LCD 参数配置 (主要通过 Device Table 进行，包含 SPI Freqrency，屏引脚配置等)。
- 支持画图功能 (包含绘图窗口位置及图像数据的传送)。
- 支持画图内容的异步和同步发送。
- 支持屏幕旋转功能 (单纯控制屏的 x,y 轴顺序)。
- 支持屏幕配置参数获取功能 (主要从 Device Table 中获取)。
- 支持屏幕的命令发送 (应用端可发送屏幕的特殊命令)。
- 支持屏幕的背光控制。


驱动使用方法
^^^^^^^^^^^^^^^^

目前驱动层支持多款 TFT LCD 屏，比如：NV3041A 和 ST7735 驱动的屏幕。 
W80X Chip 可通过 SDIO 的 SPI 模式 向屏高速通信发送图像数据。
驱动层提供了统一的 API 接口，方便使用者控制屏幕。
如下是部分场景的使用方法介绍。


显示图片 (CPU Polling)
"""""""""""""""""""""""
起始条件：

- 参考 tft_lcd_polling example 的 readme 说明,将屏和开发板的硬件引脚连线好。 `Polling 示例说明 <../../examples_reference/examples/peripheral/tft_lcd/tft_lcd_polling/README.html>`_。
- 配置 device table 中对应的设备参数，包含 SDIO Controller 和 TFT LCD 的设备表参数。
- 准备好要显示的图片内容， 在当前 example 范例中 通过图像转换工具(如：lvgl Image Converter) 将图片转成 RGB565 格式的 const 数组， 编译到固件后烧录到 flash。

API 调用时序:

- 调用 ``wm_drv_sdh_spi_init`` 初始化 SDIO Controller Driver
- 调用 ``wm_drv_tft_lcd_init`` 初始化 TFT LCD Device Driver
- 调用 ``wm_drv_tft_lcd_set_backlight`` 打开TFT LCD的背光
- 调用 ``malloc`` 申请一块临时 buffer ``buf1`` (可选项)
- 调用 ``wm_drv_tft_lcd_draw_bitmap`` 在此函数所设定的窗口中发送 ``buf1`` 中的图片内容到 TFT LCD 的GRAM

结果：

- TFT LCD 屏上会显示画面


.. note:: 

    - ``buf1`` 为可选项，若图片存储在 Flash 才需要，因为 DMA 不能直接访问 Flash
    - ``buf1`` 受限于硬件SRAM大小，可能暂存不了一整张图片，此时可以切成多个小块进行存储和传输
      
      如：480 x 272 分辨率的图片，若采用 RGB565 Pixel Format, 其数据量为 480 x 272 x 2 = 261120 字节。
      此场景可将图片切成4块，应用层申请 ``buf1`` 大小为 65280 字节逐块的缓存和发送

    - 所使用的图片格式要与 设备驱动中 预设的 pixel-format 保持一致
      
       + 默认 NV3041A 的 TFT LCD 驱动中，所支持的 Color Mode 为 RGB565， 具体设定在 ST7735 驱动的初始化序列中 ``st7735_init_seq[]``。
       + 默认 ST7735 的 TFT LCD 驱动中，所支持的 Color Mode 为 RGB565， 具体设定在 NV3041A 驱动的初始化序列中 ``nv3041a_init_seq[]``。


显示图片 (DMA)
"""""""""""""""""""""""

起始条件：

- 参考 tft_lcd_dma example 的 readme 说明,将屏和开发板的硬件引脚连线好。 `DMA 示例说明 <../../examples_reference/examples/peripheral/tft_lcd/tft_lcd_dma/README.html>`_。
- 配置 device table 中对应的设备参数，包含 SDIO Controller 和 TFT LCD 的设备表参数。
- 准备好要显示的图片内容， 在当前 example 范例中 通过图像转换工具(如：lvgl Image Converter) 将图片转成 RGB565 格式的 const 数组， 编译到固件后烧录到 flash。

API 调用时序:

- 创建一个 sempahore, 比如： ``lcd_demo_sem``
- 调用 ``wm_drv_sdh_spi_init`` 来初始化 SDIO Controller Driver
- 调用 ``wm_drv_tft_lcd_init`` 来初始化 TFT LCD Device Driver
- 调用 ``wm_drv_tft_lcd_set_backlight`` 打开TFT LCD的背光
- 调用 ``malloc`` 申请一块临时 buffer ``buf1`` (可选项，若图片存储与 Flash 才需要，因为 DMA 不能直接访问 Flash )
- 调用 ``wm_drv_tft_lcd_register_tx_callback`` 注册 tx done callback，在上次数据传输完成后释放 ``lcd_demo_sem``
- 调用 ``xSemaphoreTake`` 去获取 ``lcd_demo_sem``，未获取到时，当前 task 会进入阻塞状态
- 调用 ``wm_drv_tft_lcd_draw_bitmap`` 在此函数所设定的窗口中发送 ``buf1`` 中的图片内容到 TFT LCD 的GRAM

结果：

- TFT LCD 屏上显示画面


.. note:: 

    - ``buf1`` 为可选项，若图片存储在 Flash 才需要，因为 DMA 不能直接访问 Flash
    - ``buf1`` 受限于硬件SRAM大小，可能暂存不了一整张图片，此时可以切多块存储和传输
      
      如：480x272 分辨率的图片，若采用 RGB565 Pixel Format，其数据量为 480 x 272 x 2 = 261120 字节。
      此场景可将图片切成4块，应用层申请 ``buf1`` 大小为 65280 字节 逐块的缓存和发送

    - 所使用的图片格式应该要与 device table 中预设的 pixel-format 保持一致
    - 此DMA显示方案相较于CPU Polling方式, 可以提高CPU利用率
    - 当注册了tx done callback， ``wm_drv_tft_lcd_draw_bitmap`` 内部会默认考虑走DMA发送,但也有例外
      
      1. 发送数据总长度小于 32 Byte，因为此长度用 DMA 发送效率提升不大
      2. 发送数据的起始地址 非 4byte alignment地址，也即非4的整数倍
      3. 发送数据的总长度 非 4byte alignment地址,也即非4的整数倍

    - 遇到 2，3 的情况， ``wm_drv_tft_lcd_draw_bitmap`` 内部会把此段数据进行分段发送，
      满足 4 Byte align 的数据段用DMA发送，剩余的用 CPU Polling方式发送


应用实例
""""""""""""""""""

 :ref:`examples/peripheral/tft_lcd<peripheral_example>`

API 参考
------------------

  查找 TFT LCD 相关API请参考：
    
  :ref:`label_api_tft_lcd`


TFT LCD 设备驱动的添加方法
------------------------------

下图为 TFT LCD 设备驱动框架

.. figure:: ../../../_static/component-guides/driver/tft_lcd_arch.svg
    :align: center
    :alt: TFT LCD 设备驱动分层架构图


若要添加新的 SPI TFT LCD 设备 的设备驱动，建议保持当前分层结构，大部分情况下只需要修改黄色相关层级即可。

第一步：在设备表中创建设备信息

.. code-block::

    const static wm_dt_hw_tft_lcd_spi_t dt_hw_nv3041a = {
        .init_cfg = { .init_level = 0, .init_priority = 0 },
        .spi_cfg = {
            .mode = 0,
            .freq = 60000000,               /* Hz clock */
            .pin_cs = {
                .pin_num = WM_GPIO_NUM_27,  /**< CS - PB11 */
                .pin_mux = WM_GPIO_IOMUX_FUN5,
            },
        },
        .io_lcd_reset = WM_GPIO_NUM_26,     /**< RST - PB10 */
        .io_lcd_led = WM_GPIO_NUM_32,       /**< LED - PB16 */
        .io_lcd_dcx = WM_GPIO_NUM_25,       /**< DCX - PB9 */
        .io_lcd_te = WM_GPIO_NUM_24,        /**< TE  - PB8 */
        .spi_device_name = "sdspi",
        .gpio_device_name="gpio",
    };

.. note:: 

   - 设备表的修改方法 可参考相关章节 :ref:`pinmux`  以及  :ref:`lable_device_table`
   - 设备表中的 ``io_lcd_**`` 配置须严格匹配 你所使用开发板所选择的 IO
   - 若 ``io_lcd_te`` 不支持，可以赋值成 ``WM_GPIO_NUM_MAX``


第二步：创建新设备的 header 文件

在该文件中主要存放2种数据：

 - 新 TFT LCD 设备的 命令集，用宏定义方式呈现 (可选)
 - 新 TFT LCD 设备的 初始化命令表

初始化命令表范例如下：

.. code-block::

  const uint8_t nv3041a_init_seq[] = {
    //Format: len , delay, cmd_type,  cmd, data ... 
    //0x03, 0x00, LCD_CMD_TYPE_16BIT, 0x12, 0x34, 0xBE, // Example:16bit command "0x3412" be used
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xFF, 0xA5,
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xE7, 0x10, // TE_output_en
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x35, 0x01, // TE_interface_en 01
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x36, 0x00,
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x3A, 0x01, // 01---565，00---666 (color mode)
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x40, 0x01, // 01:IPS/00:TN
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x44, 0x15, // VBP 21
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x45, 0x15, // VFP 21
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x7D, 0x03, // vdds_trim[2:0]
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xC1, 0xBB, // avdd_clp_en avdd_clp[1:0] avcl_clp_en avcl_clp[1:0] 0xbb 88 a2
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xC2, 0x05, // vgl_clp_en vgl_clp[2:0]
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xC3, 0x10, // vgl_clp_en vgl_clp[2:0]
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xC6, 0x3E, // avdd_ratio_sel avcl_ratio_sel vgh_ratio_sel[1:0] vgl_ratio_sel[1:0] 35
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xC7, 0x25, // mv_clk_sel[1:0] avdd_clk_sel[1:0] avcl_clk_sel[1:0] 2e
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xC8, 0x11, // VGL_CLK_sel
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x7A, 0x5F, // user_vgsp  4f:0.8V 3f:1.04V 5f
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x6F, 0x44, // user_gvdd  1C:5.61 5f 53 2a 3a
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x78, 0x70, // user_gvcl  50:-3.22 75 58 66
    ..
    0x0 , //End Byte
  };


.. note:: 

    - 命令初始化表 是为了统一管理 TFT LCD 设备的各个初始化时序命令而设计
    - 命令初始化表 的格式 务必遵循当前设计要求，即 
      ``len(1Byte)+delay(1Byte)+cmd type(1Byte)+cmd(1~2 Byte)+data(len-cmd实际长度)``

      + 其中 ``len = cmd + data``， 当解析函数遇到 ``len`` = 0 的行, 即认为是结束符而退出执行。
      + 当 ``cmd type`` 为 ``LCD_CMD_TYPE_8BIT`` 时， ``cmd`` 为 1个Byte。
      + 当 ``cmd type`` 为 ``LCD_CMD_TYPE_16BIT`` 时， ``cmd`` 为 2个Byte。
      + ``delay`` 的单位为毫秒(ms) 。

    - 每个 TFT LCD 设备的初始化命令和顺序可能存在差异，须参考 LCD 设备厂商的规格书或驱动代码来设定。
    - 命令初始化表 的解析函数放在了 ``lcd_init_cmd()`` 中，新的 LCD 设备驱动可以沿用。
  

第三步：创建新设备的 驱动主体文件

因为有 第一步配置硬件，第二步配置初始化列表，且已有封装了底层 io 的操作接口， 新设备的驱动主体文件变得更加简单。
对于新设备主体驱动可以拷贝一份既有 LCD 设备的驱动文件，如 ``wm_drv_ops_nv3041a_spi.c`` 和 ``wm_drv_ops_nv3041a_spi.h`` 并重命名。
检查里面所实现的 ``Ops`` 函数逻辑是否 与新设备预期行为 存在差异的地方。

一般来讲主要改动如下几个地方即可：

- ``NV3041_LCD_CMD_`` 和 ``NV3041A_CFG_MADCTL_`` 前缀 所定义的命令替换。
- ``wm_drv_ops_nv3041a`` 前缀 所定义的 函数名替换。
- ``lcd_init_cmd()`` 中引用新的命令初始化表。


第四步： 配置文件完善

这里有两个配置文件, 新的屏幕驱动添加后，也需要添加信息到这些文件中。用于在编译时能通过 menuconfig UI 选择到新添加的 LCD Device。

- 文件1： ``wm_drv_tft_lcd_cfg.h``
- 
  + 该文件用于管理所有 LCD Device 的 ``Device Name`` 以及 ``Resolution`` ， ``Rotation`` 等信息。
  + 其中 ``Device Name`` 务必和 设备表中所选择屏的设备 ``Device Name`` 内容一致。
  + 文件1 会依据 文件2 中的配置 以及 用户通过 ``menuconfig`` 所选择的 LCD Device 来确定 生效的 LCD Device 及其基本信息。

- 文件2： ``components\driver\Kconfig``
  
  + 该文件用于管理所有 LCD Device 的 Compile Option。


.. code-block::

    //file: wm_drv_tft_lcd_cfg.h
    //This file provides a comprehensive summary of basic information related to LCD devices.
    //  Special notice:
    //  - The device names listed herein must precisely match the definitions in the device table.
    //  - When a new LCD device is added, it is essential to add new options to the COMPONENT_DRIVER_TFT_LCD_OPTIONS 
    //    managed by menuconfig, which is located in components/driver/kconfig.

    // List all lcd device's device name
    #define DEV_NAME_NV3041A_SPI "nv3041a_spi"
    #define DEV_NAME_ST7735_SPI  "st7735_spi"


    //List all lcd device's resoultion and rotation info
    #if defined(CONFIG_COMPONENT_DRIVER_LCD_NV3041A_SPI)
    #define WM_CFG_TFT_LCD_DEVICE_NAME  DEV_NAME_NV3041A_SPI
    #define WM_CFG_TFT_LCD_X_RESOLUTION 480  //Horizontal display resolution in LCD spec,independent of WM_CFG_TFT_LCD_ROTATION.
    #define WM_CFG_TFT_LCD_Y_RESOLUTION 272  //Vertical display resolution in LCD spec, independent of WM_CFG_TFT_LCD_ROTATION.
    #define WM_CFG_TFT_LCD_ROTATION     0    //The rotation to be used.
    #define WM_CFG_TFT_LCD_PIXEL_WIDTH  2    //The pixel width to be used, RGB565(2 Bytes).

    #elif defined(CONFIG_COMPONENT_DRIVER_LCD_ST7735_SPI)
    #define WM_CFG_TFT_LCD_DEVICE_NAME  DEV_NAME_ST7735_SPI
    #define WM_CFG_TFT_LCD_X_RESOLUTION 128
    #define WM_CFG_TFT_LCD_Y_RESOLUTION 160
    #define WM_CFG_TFT_LCD_ROTATION     0
    #define WM_CFG_TFT_LCD_PIXEL_WIDTH  2
    #endif

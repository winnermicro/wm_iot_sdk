.. _lvgl_gui:

LVGL
=============

简介
-------------

LVGL (轻量级和通用图形库)是一个免费和开源的图形库，它提供了创建嵌入式GUI所需的一切，具有易于使用的图形元素，美丽的视觉效果和低内存占用。

功能列表
-------------

- 丰富的模块化图形组件：按钮 (buttons)、图表 (charts)、列表 (lists)、滑动条 (sliders)、图片 (images) 等
- 高级的图形引擎：具有动画、抗锯齿、透明度、平滑滚动、图层混合等效果
- 支持多语言，采用 UTF-8 编码
- 具有类似 CSS 样式的完全可定制图形元素
- 与硬件无关：可与任何微控制器或显示器配合使用
- 可扩展：能够在内存很小的情况下运行（64KB 闪存，16KB 内存）
- 即使具有高级图形效果，也可进行单帧缓冲操作
- 用 C 语言编写以实现最大兼容性（与 C++ 兼容）
- 模拟器可在没有嵌入式硬件的情况下在 PC 上开始嵌入式图形用户界面设计
- 支持与 MicroPython 绑定
- 教程、示例、主题可用于快速图形用户界面设计
- 文档可在线获取以及以 PDF 形式提供
- 在 MIT 许可下免费和开源


配置要求
^^^^^^^^^^^^^^^^^^^

- 时钟速度： 建议大于 16MHz
- Flash/ROM： 基本的组件需要大于 64KB (建议大于 180KB)
- RAM：
  
  + 静态 RAM：根据使用的功能和对象类型，约为 2KB
  + 栈空间: 大于 2KB (建议大于 8KB)
  + 动态数据（堆）: 如果使用多个对象，大于 2KB (建议大于 48KB)
  + 显示缓冲区：大于 “水平分辨率” 像素（建议大于 10 倍 “水平分辨率”）
  + 在MCU或外部显示控制器中有一个帧缓冲区
  
- C99 或更新版本的编译器


LVGL资源
^^^^^^^^^^^^^^^^^^^

- 在线文档： `官方介绍 <https://docs.lvgl.io/master/intro/introduction.html>`_。
- 在线图片转换工具： `LVGL Image Converter <https://lvgl.io/tools/imageconverter>`_。
- UI编辑工具： Gui Guider， Squareline 等


LVGL 的示例
-------------

LVGL自带有很多示例， 在 WM IOT SDK 中可以通过 Kconfig 编译选项选择，在屏幕上展示相关 Demo 示例。

.. figure:: ../../_static/component-guides/driver/lvgl_demo.svg
    :align: center
    :alt: LVGL 示例


LVGL on WM IOT SDK
--------------------

这里有一些 WM IOT SDK LVGL 的设计和移植信息，帮忙用户在WM IOT SDK 和相关硬件平台中快速使用LVGL。

1. LVGL Example 工程路径： ``wm_iot_sdk/examples/tft_lcd/tft_lcd_lvgl``。
   
2. LVGL 编译选项并未 采用 ``lv_conf.h`` , 而采用 ``Kconfig`` 格式存储，通过 ``menuconfig`` 或 ``guiconfi`` 进行配置。

   + 可参考 `示例说明 <../examples_reference/examples/peripheral/tft_lcd/tft_lcd_lvgl/README.html>`_。

3. 目前 LVGL 适配的是 TFT LCD 屏。

   + TFT LCD 屏的选择也是在 Kconfig 中， 其选择在编译时会自动导入 ``wm_drv_tft_lcd_cfg.h``  和 ``lvgl_user_config.h``，进而联动在 TFT LCD 屏驱动 和 LVGL 中生效。

4. LVGL 选项中的 ``Color Depth`` 需要匹配 TFT LCD 屏驱动中 的 Color Mode。
   
   + 默认 NV3041A 的 TFT LCD 驱动中，所支持的 Color Mode 为 RGB565， 具体设定在 ST7735 驱动的初始化序列中 ``st7735_init_seq[]``。
   + 默认  ST7735 的 TFT LCD 驱动中，所支持的 Color Mode 为 RGB565， 具体设定在 NV3041A 驱动的初始化序列中 ``nv3041a_init_seq[]``。

.. figure:: ../../_static/component-guides/driver/lvgl_demo_kconfig.svg
    :align: center
    :alt: LVGL Kconfig 界面

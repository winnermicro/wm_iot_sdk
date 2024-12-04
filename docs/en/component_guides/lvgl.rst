.. _lvgl_gui:

LVGL
=============

Introduction
-------------

LVGL (Lightweight and Versatile Graphics Library) is a free and open-source graphics library. It provides everything needed to create an embedded GUI. It has easy-to-use graphical elements, beautiful visual effects, and low memory usage.

Feature List
-------------

- Rich modular graphical components: buttons, charts, lists, sliders, images, etc.
- Advanced graphics engine: with effects such as animations, anti-aliasing, transparency, smooth scrolling, layer blending, etc.
- Supports multiple languages and uses UTF-8 encoding.
- Fully customizable graphical elements with CSS-like styles.
- Hardware-independent: can be used with any microcontroller or display.
- Extensible: can run with very little memory (64KB flash, 16KB RAM).
- Can operate with single frame buffer even with advanced graphical effects.
- Written in C language for maximum compatibility (compatible with C++).
- Simulator allows starting embedded GUI design on a PC without embedded hardware.
- Supports binding with MicroPython.
- Tutorials, examples, and themes are available for rapid GUI design.
- Documentation is available online and in PDF format.
- Free and open source under the MIT license.


Configuration Requirement
^^^^^^^^^^^^^^^^^^^^^^^^^^

- Clock speed: Recommended to be greater than 16MHz.
- Flash/ROM: Basic components require more than 64KB (recommended to be more than 180KB).
- RAM：
  
  + Static RAM: Approximately 2KB depending on the functions and object types used.
  + Stack space: Greater than 2KB (recommended to be greater than 8KB).
  + Dynamic data (heap): If multiple objects are used, greater than 2KB (recommended to be greater than 48KB).
  + Display buffer: Greater than "horizontal resolution" pixels (recommended to be greater than 10 times "horizontal resolution").
  + Have a frame buffer in the MCU or external display controller.
  
- Compiler of C99 or newer version


LVGL Resource
^^^^^^^^^^^^^^^^^^^

- Online documentation： `Official Webage <https://docs.lvgl.io/master/intro/introduction.html>`_.
- Online image conversion tool： `LVGL Image Converter <https://lvgl.io/tools/imageconverter>`_.
- UI Design Tools： Gui Guider， Squareline，etc.


LVGL Demo
-------------

LVGL have many official examples. In the WM IOT SDK, relevant Demo examples can be selected through Kconfig compilation options and displayed on the screen.

.. figure:: ../../_static/component-guides/driver/lvgl_demo.svg
    :align: center
    :alt: LVGL Demo


LVGL on WM IOT SDK
--------------------

Here are some design and porting information of WM IOT SDK LVGL to help users quickly use LVGL in WM IOT SDK and related hardware platforms.

1. LVGL Example project path： ``wm_iot_sdk/examples/tft_lcd/tft_lcd_lvgl``.
   
2. The LVGL compilation option does not use ``lv_conf.h`` , but is stored in the ``Kconfig`` ，and configured through ``menuconfig`` or ``guiconfi`` .

   + Refer to `Project Introduction <../examples_reference/examples/peripheral/tft_lcd/tft_lcd_lvgl/README_EN.html>`_.

3. Currently, LVGL is only adapted to TFT LCD screens in WM IOT SDK.

   + The selection of TFT LCD screens is also in ``Kconfig``. During compilation, it will automatically import ``wm_drv_tft_lcd_cfg.h`` and ``lvgl_user_config.h``, and then take effect in the TFT LCD screen driver and LVGL.
  
4. The ``Color Depth`` in the LVGL option needs to match the ``Color Mode`` in the TFT LCD screen driver.
   
   + In the default TFT LCD driver of NV3041A, the supported Color Mode is RGB565. The specific setting is in the initialization sequence of the ST7735 driver ``st7735_init_seq[]``.
   + In the default TFT LCD driver of ST7735, the supported Color Mode is RGB565. The specific setting is in the initialization sequence of the NV3041A driver ``nv3041a_init_seq[]``.

.. figure:: ../../_static/component-guides/driver/lvgl_demo_kconfig.svg
    :align: center
    :alt: LVGL Kconfig UI

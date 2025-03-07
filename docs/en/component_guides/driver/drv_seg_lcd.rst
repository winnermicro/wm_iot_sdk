.. _seg_lcd:

SEG_LCD
=============

Introduction
--------------

The segment LCD, also known as a segmented Liquid Crystal Display (LCD), is a common type of liquid crystal display screen that consists of multiple independent segments or "Zones," each capable of independently controlling the display of different characters or graphics.

Feature List
------------------

This module is divided into two levels of functionality: the bottom layer is the driver-level functions, the the upper layer is the GDC0689 device driver functions implemented based on the driver layer.

Driver Layer Function List
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Supports LCD screens with a maximum of 8 COM X 32 SEG COM/SEG configuration;
- Supports various refresh modes: Static, 1/2, 1/3, 1/4, 1/5, 1/6, 1/7, 1/8 duty;
- Supports various bias voltages: Static, 1/2, 1/3, 1/4 bias;
- LCD refresh rate can be dynamically configured.

GDC0689 Device Driver Layer Function List
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Digital display: 6-digit integer or decimal display
- Time display: Hour and minute display
- Unit display: "mV" (millivolt), "N" (Newton), "m" (meter), "kg" (kilogram), "g" (gram), "mm" (millimeter), με (microstrain), "kPa" (kilopascal), "MPa" (megapascal), "℃" (Celsius)
- Icon display: Service icons, power level icons, signal level icons
- Display all elements
- Clear display

Function Overview
-----------------------

This module primarily displays time,numbers, units, and icons on the GDC0689 segmented LCD screen.

Hardware Configuration
---------------------------

seg_lcd Configuration (pre-configured)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

| Configured via Device Tree (DT) modifications.

Main Functions
----------------

SEG_LCD Driver Layer Functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Starting Conditions:

- Initialize the seg_lcd driver before use

Related APIs:

- Call ``wm_drv_seg_lcd_init`` to initialize the seg_lcd driver
- Call ``wm_drv_seg_lcd_register_table`` to register the seg_lcd display table
- Call ``wm_drv_seg_lcd_display_seg`` to control the display state of specified segments
- Call ``wm_drv_seg_lcd_clear`` to clear all display content
- Call ``wm_drv_seg_lcd_deinit`` to deinitialize the seg_lcd driver

GDC0689 Display Time Function
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Starting Conditions:

- Initialize gdc0689 before use

Relevant Timing APIs:

- Call ``wm_gdc0689_init`` to initialize gdc0689
- Call ``wm_gdc0689_display_time`` to display the current time on the segmented LCD

Result:

- The current time is displayed on the segmented LCD

GDC0689 Display Numeric Function
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Starting Conditions:

- Initialize gdc0689 before use

Relevant Timing APIs:

- Call ``wm_gdc0689_init`` to initialize gdc0689
- Call ``wm_gdc0689_display_integer`` to display integer values on the segmented LCD
- Call ``wm_gdc0689_display_decimal`` to display decimal values on the segmented LCD

Result:

- Numbers are displayed on the segmented LCD

GDC0689 Display Unit Function
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Starting Conditions:

- Initialize gdc0689 before use

Relevant Sequence APIs:

- Call ``wm_gdc0689_init`` to initialize gdc0689
- Call ``wm_gdc0689_display_unit`` to display measurement units on the segmented LCD

Result:

- Measurement Units are displayed on the segmented LCD

GDC0689 Display Icon Function
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Starting Conditions:

- Initialize gdc0689 before use

Relevant Sequence APIs:

- Call ``wm_gdc0689_init`` to initialize gdc0689
- Call ``wm_gdc0689_display_service_icon`` to display the service icon on the segmented LCD
- Call ``wm_gdc0689_display_battery_level`` to display battery level on the segmented LCD
- Call ``wm_gdc0689_display_signal_level`` to display signal strength on the segmented LCD

Result:

- Icons are displayed on the segmented LCD

Notes
-------------

1. DT configuration needs to be set according to the actual parameters of the segmented LCD;

2. If using a non-GDC0689 segmented LCD, you need to refer to ``examples/peripheral/seg_lcd/sample`` based on the seg_lcd driver and complete the corresponding content display in combination with the segmented LCD datasheet:

   * Step 1. Configure the seg_lcd pins and related parameters through :ref:`lable_device_table`
   * Step 2. Call ``wm_drv_seg_lcd_init("seg_lcd");`` to initialize the seg_lcd driver
   * Step 3. Define the seg_lcd display table ``g_seg_lcd_table`` according to the screen's datasheet
   * Step 4. Call ``wm_drv_seg_lcd_register_table(dev, g_seg_lcd_table, sizeof(g_seg_lcd_table) / sizeof(g_seg_lcd_table[0]));`` to register the seg_lcd display table
   * Step 5. Call ``wm_drv_seg_lcd_display_seg`` to encapsulate display functions for various content, you can refer to the implementation in ``components\driver\src\display\seg_lcd\wm_gdc0689.c``

Application Example
------------------------

 :ref:`examples/peripheral/seg_lcd<peripheral_example>`

API Reference
------------------

| The API is divided into two parts: one part is the seg_lcd controller driver layer API, and the other part is the GDC0689 device driver API implemented based on the seg_lcd controller driver layer.

| :ref:`label_api_seg_lcd`
| :ref:`label_api_gdc0689`

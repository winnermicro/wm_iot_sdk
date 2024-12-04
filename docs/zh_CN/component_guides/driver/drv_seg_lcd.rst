.. _seg_lcd:

SEG_LCD
=============

简介
-------------

断码屏，又称为分段式 LCD(Liquid Crystal Display)，是一种常见的液晶显示屏，它由多个独立的小段或“段”组成，每个段可以独立控制显示不同的字符或图形。

功能列表
-------------

该模块分为两个层次的功能，底层是 driver 层的功能，上层是基于 driver 层实现的 GDC0689 device driver 功能。

driver 层功能列表
^^^^^^^^^^^^^^^^^^^

- 支持最大 8 COM X32 SEG 的 COM/SEG 类型 LCD 屏；
- 支持 Static, 1/2, 1/3, 1/4, 1/5, 1/6, 1/7, 1/8 duty 各种刷新模式；
- 支持 Static, 1/2, 1/3, 1/4 bias 等各种偏置电压。
- LCD 刷新频率可以动态配置；

GDC0689 device driver 层功能列表
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- 数字显示: 6 位整数或小数显示
- 时间显示：时分显示
- 单位显示："mV"（毫伏）、"N"（牛顿）、"m"（米）、"kg"（千克）、"g"（克）、"mm"（毫米）、με（微应变）、"kPa"（千帕斯卡）、"MPa"（兆帕斯卡）、"℃"（摄氏度）
- 图标显示：服务图标，电源等级图标，信号等级图标
- 显示所有元素
- 清除显示


功能概述
-------------

该模块主要在型号为 GDC0689 断码屏上显示时间、数字、单位、图标。


硬件配置
-------------

seg_lcd 配置（已配置）
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

| 通过修改 DT 进行配置。

gdc0689 配置（已配置）
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

| 通过修改 DT 进行配置。

主要功能
-------------

显示时间功能
^^^^^^^^^^^^^

起始条件：

- 使用前初始化 gdc0689

相关时序 API:

- 调用 ``wm_gdc0689_init`` 来初始化 gdc0689
- 调用 ``wm_gdc0689_display_time`` 在断码屏上显示当前时间


结果：

- 断码屏上显示时间

显示数字功能
^^^^^^^^^^^^^

起始条件：

- 使用前初始化 gdc0689

相关时序 API:

- 调用 ``wm_gdc0689_init`` 来初始化 gdc0689
- 调用 ``wm_gdc0689_display_integer`` 在断码屏上显示整数值
- 调用 ``wm_gdc0689_display_decimal`` 在断码屏上显示小数值

结果：

- 断码屏上显示数字

显示单位功能
^^^^^^^^^^^^^

起始条件：

- 使用前初始化 gdc0689

相关时序 API:

- 调用 ``wm_gdc0689_init`` 来初始化 gdc0689
- 调用 ``wm_gdc0689_display_unit`` 在断码屏上显示测量单位

结果：

- 断码屏上显示单位

显示图标功能
^^^^^^^^^^^^^

起始条件：

- 使用前初始化 gdc0689

相关时序 API:

- 调用 ``wm_gdc0689_init`` 来初始化 gdc0689
- 调用 ``wm_gdc0689_display_service_icon`` 在断码屏上显示服务图标
- 调用 ``wm_gdc0689_display_battery_level`` 在断码屏上显示电池电量
- 调用 ``wm_gdc0689_display_signal_level`` 在断码屏上显示信号强度

结果：

- 断码屏上显示图标

注意事项
-------------

| 1. DT 配置需按照实际断码屏实际参数进行设置;
| 2. 若使用非 GDC0689 断码屏，需基于 seg_lcd driver 参考 gdc0689 device driver ，并结合断码屏数据手册完成对应的 device driver;

应用实例
-------------

 :ref:`examples/peripheral/seg_lcd<peripheral_example>`

API 参考
------------------

| API 分为两部分 ，一部分是 seg_lcd controller driver 层 API ，另一部分是基于 seg_lcd controller driver 层实现的 GDC0689 device driver API。

| :ref:`label_api_seg_lcd`
| :ref:`label_api_gdc0689`
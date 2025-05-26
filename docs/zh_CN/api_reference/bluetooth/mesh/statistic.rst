.. _bluetooth_mesh_stat:

帧统计信息
###############

帧统计 API 允许监视不同接口上接收的帧的数量，以及计划的和成功的传输和中继尝试的数量。

该 API 帮助用户估计广告商配置参数的效率和设备的扫描能力。监控参数的数量可以很容易地通过客户值进行扩展。

应用程序可以随时读取和清理统计信息。

API 参考
===============

头文件
-----------

- wm_iot_sdk/components/bluetooth/include/bluetooth/mesh/statistic.h
- This header file can be included with:

.. code-block:: c
   :emphasize-lines: 1

   #include "bluetooth/mesh/statistic.h"

.. doxygengroup:: bt_mesh_stat

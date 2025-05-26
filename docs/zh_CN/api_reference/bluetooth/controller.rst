.. _bt_ctrl:

蓝牙控制器
####################

wm-iot-sdk 蓝牙控制器包含了基带管理、链路控制、链路管理及HCI接口的实现，支持蓝牙4.2标准。wm-iot-sdk 蓝牙控制器以库的形式展现，用户通过提供的api访问。

头文件
-----------

- wm_iot_sdk/components/bluetooth/include/bluetooth/controller.h
- This header file can be included with:

.. code-block:: c
   :emphasize-lines: 1

   #include "bluetooth/controller.h"

API 参考
-----------

.. doxygengroup:: bt_ctrl

.. _bluetooth_mesh_dfu_cli:

固件更新客户端
######################

固件更新客户端负责通过网状网络分发固件更新。固件更新客户端使用 :ref:`bluetooth_mesh_blob_cli` 作为其传输的传输。

头文件
===============

- wm_iot_sdk/components/bluetooth/include/bluetooth/mesh/dfu_cli.h
- This header file can be included with:

.. code-block:: c
   :emphasize-lines: 1

   #include "bluetooth/mesh/dfu_cli.h"

API 参考
===============

.. doxygengroup:: bt_mesh_dfu_cli
   :project: wm-iot-sdk-apis
   :members:

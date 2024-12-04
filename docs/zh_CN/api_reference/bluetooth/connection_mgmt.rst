.. _bluetooth_connection_mgmt:

连接管理
#####################

Bluetooth 协议栈使用一个名为 :c:struct:`bt_conn` 表示与其他设备的连接。此结构的内部结构不向应用程序公开，获取内部结构信息（例如远程地址）可以使用
:c:func:`bt_conn_get_info` API。 连接对象依赖于引用计数，并且应用程序应使用 :c:func:`bt_conn_ref` API 保存连接对象。因为这可以确保对象保持有效（即使连接将断开）。类似地 :c:func:`bt_conn_unref` API 用于释放该连接对象。

应用程序可以通过注册 :c:struct:`bt_conn_cb` 结构，使用: :c:macro:`bt_conn_cb_register` 或 :c:macro:`BT_CONN_CB_DEFINE` APIs。 
此结构作为应用程序为连接和断开事件以及其他事件如安全级别的更改或连接参数回调的参数传入回调函数。
当蓝牙充当中心设备时，应用程序将通过 :c:func:`bt_conn_le_create` API创建该结构体。

API参考
*************

.. doxygengroup:: bt_conn

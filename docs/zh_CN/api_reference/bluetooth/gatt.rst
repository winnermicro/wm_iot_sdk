.. _bt_gatt:


通用属性配置文件（GATT）
################################

GATT 层管理服务数据库，为服务注册提供 API 和属性声明。

可以使用 :c:func:`bt_gatt_service_register()` API 注册服务，它采用 :c:struct:`bt_gatt_service` 结构，该结构提供服务包含的属性。助手宏 :c:macro:`BT_GATT_SERVICE()` 可以用于声明服务。

可以使用 :c:func:`bt_gatt_attr` 结构或使用辅助对象宏之一：

    :c:macro:`BT_GATT_PRIMARY_SERVICE`
        声明主要服务。

    :c:macro:`BT_GATT_SECONDARY_SERVICE`
        声明辅助服务。

    :c:macro:`BT_GATT_INCLUDE_SERVICE`
        声明部分服务。

    :c:macro:`BT_GATT_CHARACTERISTIC`
        声明特征。

    :c:macro:`BT_GATT_DESCRIPTOR`
        声明描述符。

    :c:macro:`BT_GATT_ATTRIBUTE`
        声明属性。

    :c:macro:`BT_GATT_CCC`
        声明客户端特征配置。

    :c:macro:`BT_GATT_CEP`
        声明特征扩展属性。

    :c:macro:`BT_GATT_CUD`
        声明特征用户格式。

每个属性都包含一个 ``uuid`` ，用于描述其类型，一个 ``read`` 回调、 ``write`` 回调和一组权限。读取和写入如果属性权限不允许回调各自的操作。

.. note::
   GATT 中不支持32位 UUID。应转换所有32位 UUID，当 UUID 包含在 ATT PDU 中时，为128位 UUID。

.. note::
   属性 ``read`` 和 ``write`` 回调直接从 RX 线程调用，因此，不建议在其中长时间阻塞。

可以使用 :c:func:`bt_gatt_notify` API 通知属性值更改，或者存在 :c:func:`bt_gatt_notify_cb` ，其中可以传递回调，以便在需要知道确切的时间时调用，数据已通过空中传输。指示由支持 :c:func:`bt_gatt_indicate` API。

可以使用配置选项 ``CONFIG_BT_GATT_CLIENT`` 启用客户端过程

可以使用启动发现过程 :c:func:`bt_gatt_discover` API，它接受 :c:struct:`bt_gatt_discover_params` 结构，描述发现。在设置 ``uuid`` 时，参数还用作筛选器将发现匹配的仅字段属性，而设置它到 NULL 允许发现所有属性。

.. note::
  不支持缓存发现的属性。

:c:func:`bt_gatt_Read` API 支持读取过程，该 API 采用 :c:struct:`bt_gatt_read_params` 结构作为参数。在参数1或可以设置更多的属性，尽管设置多个句柄需要选项 ``CONFIG_BT_GATT_READ_MULTIPLE`` 。

:c:func:`bt_gatt_Write` API 支持写入过程，并采用 :c:struct:`bt_gatt_write_params` 结构作为参数。如果写入操作不需要响应 :c:func:`bt_gatt_write_without_response` 或 :c:func:`bt_gatt_write_without_response_cb` API 可以与以后的工作类似于 :c:func:`bt_gatt_notify_cb` 。
可以使用启动对通知和指示的订阅 :c:func:`bt_gatt_subscribe` API，它需要 :c:struct:`bt_gatt_subscripte_params` 作为参数。对的多个订阅支持相同的属性，因此可以有多个 ``notify`` 回调为同一属性触发。可以使用删除订阅 :c:func:`bt_gatt_unsubscribe` API。

.. note::
  删除订阅时 ``notify``  用数据调用回调
  设置为 NULL。

头文件
-----------

- wm_iot_sdk/components/bluetooth/include/bluetooth/gatt.h
- This header file can be included with:

.. code-block:: c
   :emphasize-lines: 1

   #include "bluetooth/gatt.h"

API 参考
-----------

.. doxygengroup:: bt_gatt

.. doxygengroup:: bt_gatt_server

.. doxygengroup:: bt_gatt_client

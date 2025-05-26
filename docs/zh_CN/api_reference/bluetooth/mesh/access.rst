.. _bluetooth_mesh_access:

访问层
############

访问层是应用程序与 Bluetooth Mesh 网络的接口。访问层提供了将节点行为划分为元素和模型的机制，这些元素和模型由应用程序实现。

mesh 模型
===========

网格节点的功能由模型表示。模型实现节点支持的单个行为，如灯光、传感器或节温器。网格模型被分组为 *元素* ，每个元素都是分配了自己的单播地址，并且只能包含每种类型的模型。
通常，每个元素表示网格的单个方面节点行为。例如，包含传感器、两个灯和电源插座将此功能扩展到四个元素，每个元素元素实例化的单个方面所需的所有模型支持的行为。

节点的元素和模型结构在节点组合数据中指定，该数据在初始化期间传递给 :c:func:`bt_mesh_init` 。蓝牙 SIG 定义了一组基础模型（请参见
:ref:`bluetooth_mesh_models`）以及一组用于实现公共中的行为 `Bluetooth Mesh Model Specification
<https://www.bluetooth.com/specifications/mesh-specifications/>`_  ABluetooth SIG 未指定的所有型号都是供应商型号，并且必须绑定到公司 ID。

网格模型具有多个参数，可以通过网格堆栈的初始化或使用
:ref:`bluetooth_mesh_models_cfg_srv`:

操作码列表
===========

操作码列表包含模型可以接收的所有消息操作码，以及可接受的最小有效负载长度和将它们传递到的回调。模型可以支持任何数量的操作码，但每个操作码只能由每个元素中的一个模型列出。

完整的操作码列表必须传递给组合数据中的模型结构，并且不能在运行时更改。操作码列表的末尾由特殊的 :c:macro:`BT_MESH_MODEL_OP_end` 条目确定。此条目必须始终存在于操作码列表中，除非列表为空。在这种情况下，应使用 :c:macro:`BT_MESH_MODEL_NO_OPS` 来代替正确的操作码列表定义。

AppKey 列表 
===========

AppKey 列表包含模型可以接收的所有应用程序密钥上的消息。只有使用 AppKey 列表中的应用程序密钥加密的消息才会传递给模型。

每个型号可以持有的最大支持应用程序密钥数是通过 ``CONFIG_BT_MESH_model_KEY_COUNT`` 配置选项配置的。AppKey 列表的内容由管理
:ref:`bluetooth_mesh_models_cfg_srv`.

订阅列表
=================

模型将处理寻址到其元素的单播地址的所有消息（假定所使用的应用程序密钥存在于 AppKey 列表中）。此外，该模型将处理寻址到其订阅列表中的任何组或虚拟地址的数据包。这允许节点使用单个消息寻址整个网状网络中的多个节点。

每个型号可以容纳的订阅列表中支持的最大地址数是用 ``CONFIG_BT_MESH_model_GROUP_COUNT`` 配置的配置选项。订阅列表的内容由管理 :ref:`bluetooth_mesh_models_cfg_srv`.

模型发布
=================

模型可以通过两种方式发送信息：

* 通过在 :c:struct:`bt_mesh_msg_ctx` 中指定一组消息参数，并调用 :c:func:`bt_mesh_model_send`。
* 通过设置 :c:struct:`bt_mesh_model_pub` 结构并调用 :c:func:`bt_mesh_model_publish` 。

使用 :c:func:`bt_mesh_model_publish` 发布消息时，模型将使用由配置的发布参数 :ref:`bluetooth_mesh_models_cfg_srv` 。这是发送未经提示的模型消息的推荐方式，因为它将选择消息参数的责任传递给网络管理员，网络管理员可能比单个节点更了解网状网络。

为了使用发布参数支持发布，模型必须为发布分配数据包缓冲区，并将其传递给 :c:member:`bt_mesh_model_pub.msg` 。配置服务器也可以设置周期发布消息的发布。
为了支持这一点，模型必须填充 :c:member:`bt_mesh_model_pub.update` 回调。 :c:member:`bt_mesh_model_pub.update` 回调将在
消息被发布，允许模型改变有效载荷以反映其当前状态。

通过将 :c:member:`bt_mesh_model_pub.retr_update` 设置为1， 该模型可以配置每次重新传输时触发的 :c:member:`btmeshmodelpub.update` 回调。 例如，这可以由以下模型使用：
使用Delay参数，该参数可以针对每次重新传输进行调整。 :c:func:`bt_mesh_model_pub_is_retransmission` 函数可以是用于区分首次发布和重新传输。
:c:macro:`BT_MESH_PUB_MSG_OTAL` 和 :c:macro:`BT_MESCH_PUB_MSG_NUM` 宏可用于返回一个发布间隔内的传输总数和重传次数。

扩展型号
===============

Bluetooth Mesh 规范允许网格模型彼此扩展。当一个模型扩展另一个模型时，它继承该模型的功能，
并且扩展可以用于从简单的模型中构造复杂的模型，利用现有的模型功能来避免定义新的操作码。
模型可以从任何元素扩展任何数量的模型。当一个模型在同一个元素中扩展另一个模型时，
这两个模型将共享订阅列表。网格堆栈通过将两个模型的订阅列表合并为一个列表来实现这一点，
并结合模型可以拥有的订阅总数。模型可以扩展扩展其他模型的模型，从而创建“扩展树”。
扩展树中的所有模型都为其跨越的每个元素共享一个订阅列表。

模型扩展是通过在 :c:func:`bt_mesh_model_extend` 初始化， 一个模型只能由另一个模型扩展，并且扩展不能是循环的。请注意，节点状态的绑定以及模型之间的其他关系必须由模型实现来定义。

模型扩展概念在访问层数据包处理中增加了一些开销，并且必须显式启用
``CONFIG_BT_MESH_MODEL_EEXTENSIONS`` 是否有任何效果。

模型数据存储
==================

网格模型可能具有与需要持久存储的每个模型实例相关联的数据，访问 API 提供了一种利用内部模型实例编码方案存储该数据的机制。 
模型可以通过调用 :c:func:`bt_mesh_model_data_store` 。 下次设备重新启动时能够读取数据，
必须填充模型的 :c:member:`btmeshmodelcb.settingsset` 回调。 当在持久存储中找到模型特定的数据时，
将调用该回调。 模型可以通过调用传递为的 ``read_cb`` 来检索数据回调的参数。有关详细信息，请参阅 ``settings_api`` 模块文档。

当模型数据频繁更改时，在每次更改时存储它可能会导致闪存磨损增加。为了减少磨损，
模型可以通过调用 :c:func:`bt_mesh_model_data_store_schedule` 来延迟数据存储。
堆栈将使用由 ``CONFIG_BT_MESH_STORE_TIMEOUT`` 选择。工作项正在运行时， 
堆栈将调用 :c:member:`btmeshmodelcb.pending_store`
请求存储数据的每个模型的回调，然后，模型可以调用 :c:func:`bt_mesh_model_data_store` 来存储数据。

如果启用了 ``CONFIG_BT_MESH_SETTINGS_WORKQ`` ，回调这个
:c:member:`bt_mesh_model_cb.pending_store` 从专用的线程。这允许堆栈处理其他传入和传出消息
存储模型数据时。当需要存储大量数据时，建议使用此选项和 :c:func:`bt_mesh_model_data_store_schedule` 函数。

组成数据
================

合成数据提供关于网格设备的信息。
设备的组成数据保存关于设备、它支持的型号和其他功能。组成
数据被拆分为不同的页面，其中每个页面包含特定的功能有关设备的信息。为了访问此信息，用户
可以使用 :ref:`bluetooth_mesh_models_cfg_srv` 模型或 :ref:`bluetooth_mesh_lcd_srv` 模型。

成分数据页0
-----------------------

成分数据页面 0 提供关于设备的基本信息，对于所有网状设备都是必需的。它包含图元和模型组成、支持的功能和制造商信息。

成分数据第1页
-----------------------

成分数据页面 1 提供关于模型之间关系的信息，对于所有网状设备都是必需的。 模型可以扩展和/或对应于一个或多个模型。 模型可以通过调用 :c:func:`bt_mesh_model_extend` 来扩展另一个模型，
或通过调用 :c:func:`bt_mesh_model_communicate` 来对应于另一个模型。 ``CONFIG_BT_MESH_MODEL_EXTENSION_LIST_SIZE`` 指定设备上的组合中可以存储多少模型关系， 并且该数字应该反映 :c:func:`bt_mesh_model_extend` 和 :c:func:`bt_mesh_model` 对应调用的数量。

成分数据第2页
-----------------------

成分数据页面 2 提供支持的网格轮廓的信息。Mesh 配置文件规范定义了希望支持特定蓝牙 SIG 定义的配置文件的设备的产品要求。当前支持的配置文件可以在中的第3.12节中找到 `Bluetooth SIG Assigned Numbers
<https://www.bluetooth.com/specifications/assigned-numbers/uri-scheme-name-string-mapping/>`_.
成分数据页面 2 仅对于声称支持一个或多个网格配置文件的设备是必需的。

成分数据第128、129和130页
---------------------------------------

合成数据页128、129和130分别镜像合成数据页0、1和2。当固件更新后合成数据将更改时，它们用于表示镜像页面的新内容。 请参见 :ref:`bluetooth_mesh_dfu_srv_comp_data_and_models_metadata` 以了解详细信息。

延迟消息
==================

可延迟消息功能通过 Kconfig 选项启用 ``CONFIG_BT_MESH_ACCESS_DELAYABLE_MSG`` 。
这是一种可选功能，用于实现模型在对接收到的消息（也称为响应消息）的响应中传输的消息的规范建议。

响应消息的发送应具有以下随机延迟：

* 如果接收的消息被发送到单播地址，则介于20到50毫秒之间。

* 如果发送了接收的消息，则介于20到500毫秒之间到组或虚拟地址。

如果设置了 :c:member:`bt_mesh_msg_ctx.rnd_delay` 标志，则会触发可延迟消息功能。可延迟消息功能将消息存储在本地存储器中，同时等待随机延迟到期。

如果传输层没有足够的内存在随机延迟到期时发送消息，则消息将再延迟10毫秒。如果传输层由于任何其他原因无法发送消息，可延迟消息功能将引发带有传输层错误代码的 :c:member:`bt_mesh_send_cb.start` 回调。

如果可延迟消息功能找不到足够的可用内存来存储传入消息，则它将以接近到期的延迟发送消息以释放内存。

挂起或重置网格堆栈时，将删除尚未发送的消息，并引发 :c:member:`bt_mesh_send_cb.start` 回调，并返回错误代码。

可延迟发布
======================

在以下情况下，可延迟发布功能实现消息发布延迟的规范建议：

* Bluetooth Mesh 堆栈启动时或由 :c:member:`bt_Mesh_model_publish` 函数触发发布时，介于20到500毫秒之间。
* 对于定期发布的消息，介于20到50毫秒之间。

此功能是可选的，并通过 ``CONFIG_BT_MESH_DELAYABLE_PUBLICATION``  Kconfig 选项启用。启用后，每个模型都可以通过将 :c:member:`bt_mesh_model_pub.delayable` 位字段相应地设置为 ``1`` 或 ``0`` 来启用或禁用可延迟发布，该位字段可以随时更改。

API 参考
======================

头文件
-----------

- wm_iot_sdk/components/bluetooth/include/bluetooth/mesh/access.h
- This header file can be included with:

.. code-block:: c
   :emphasize-lines: 1

   #include "bluetooth/mesh/access.h"

.. doxygengroup:: bt_mesh_access

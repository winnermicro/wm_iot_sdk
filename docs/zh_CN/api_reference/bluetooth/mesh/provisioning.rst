.. _bluetooth_mesh_provisioning:

预配置
############

配置是将设备添加到网状网络的过程。它需要两个设备以下角色运行:

* *供应器* 表示网络所有者，并负责将新节点添加到网状网络。
* 被调配者是通过调配过程添加到网络的设备。在配置过程开始之前，被调配者是未配置的设备。

WM IoT SDK Bluetooth Mesh 堆栈中的 资源调配模块既支持被供应者角色的广告和 GATT 设置承载器，也支持供应器角色的高级配置承载。

资源调配过程
************************

必须先配置所有 Bluetooth Mesh 节点，然后才能参与 Bluetooth Mesh 网络。配置 API 提供设备成为配置的 Mesh 节点所需的所有功能。资源调配是一个五步过程，涉及以下步骤：

* 信标
* 邀请
* 公钥交换
* 身份验证
* 预配置数据传输

信标
=========

要启动配置过程，未配置的设备必须首先开始广播未配置的信标。这使得它对附近的供应器可见，
可以启动供应。要指示需要配置设备，请调用 :c:func:`bt_mesh_prov_enable` 。设备使用设备 UUID 和 
``OOB information`` 字段开始广播未配置的 ``prov`` ，如传递给 :c:func:`bt_mesh_init` 的 prov 参数中所指定的那样。
此外，可以指定统一资源标识符（URI），其可以将配置器指向某些带外信息的位置，例如设备的公钥或认证值数据库。
URI 在单独的信标中进行公告，未配置的信标包含 URI 哈希，以将两者绑定在一起。


统一资源标识符
---------------------------

统一资源标识符应遵循蓝牙核心规范补充中规定的格式。URI 必须以 URI 方案开始，该方案编码为单个 utf-8 数据点，或者以特殊的 ``none`` 方案开始，编码为 ``0x01`` 。可用方案列在 `Bluetooth website
<https://www.bluetooth.com/specifications/assigned-numbers/>`_.

编码 URI 的示例：
 
.. list-table:: URI encoding examples
 
  * - URI
    - Encoded
  * - ``http://example.com``
    - ``\x16//example.com``
  * - ``https://www.zephyrproject.org/``
    - ``\x17//www.zephyrproject.org/``
  * - ``just a string``
    - ``\x01just a string``

正在设置邀请
=======================

调配器通过发送调配邀请来启动调配过程。邀请会提示被调配者使用 Health Server :ref:`bluetooth_mesh_models_Health_srv_attention` （如果可用）提醒自己注意。

未配置的设备通过提供其功能的列表（包括支持的带外身份验证方法和算法）来自动响应邀请。

公钥交换
===================

在供应过程开始之前，供应器和未经验证的设备交换带内或带外（OOB）公钥。

带内公钥交换是配置过程的一部分，始终由未配置的设备和配置程序支持。

如果应用程序希望通过 OOB 支持公钥交换，则需要为网格堆栈提供公钥和私钥。
未配置的设备将在其功能中反映这一点。供应器通过任何可用的 OOB 机制获得公钥（例如，设备可以公布包含公钥的分组，
或者可以将其编码为打印在设备包装上的 QR 码）。请注意，即使未配置的设备已为带外交换指定公钥，
如果配置程序无法通过 OOB 机制检索公钥，则它也可以选择在带内交换公钥。在这种情况下，网格堆栈将为每个配置过程生成一个新的密钥对。

要在未配置的设备端启用 OOB 公钥支持，需要启用 ``CONFIG_BT_MESH_PROV_OOB_public_key`` 。
在启动配置过程之前，应用程序必须通过初始化指向 :c:member:`bt_mesh_prov.public_key_be` 和 :c:member:`bt_mesh_prov.private_key_be` 的指针来提供公钥和私钥。密钥需要以大端字节顺序提供。

要提供通过 OOB 获得的设备公钥，请在供应器端调用 :c:func:`bt_mesh_prov_remote_pub_key_set` 。

身份验证
==============

在初始交换之后，供应器选择带外（OOB）身份验证方法。这允许用户确认供应器连接到的设备实际上是他们想要的设备，而不是恶意的第三方。

Provisioning API 支持被调配者的以下身份验证方法：

* **静态OOB：** 将身份验证值分配给生产中的设备，供应器可以以特定于应用程序的方式查询该值。
* **输入OOB：** 用户输入身份验证值。中列出了可用的输入操作 :c:enum:`bt_mesh_input_action_t`.
* **输出OOB：** 向用户显示身份验证值。中列出了可用的输出操作 :c:enum:`bt_mesh_output_action_t`.

应用程序必须为 :c:struct:`bt_mesh_prov` 中支持的身份验证方法提供回调，并在 :c:member:`bt_mesh_prov.output_actions` 和 :c:member:`bt_mesh_prov.input_actions` 中启用支持的操作。

选择输出 OOB 操作时，应在调用输出回调时向用户提供身份验证值，并保持该值，直到调用 :c:member:`bt_mesh_prov.input_complete` 或 :c:member:`bt_mesh_prov.complete` 回调。
如果 ``blink`` ， ``beep`` 或者 ``vibrate`` ，则应在延迟三秒或更长时间后重复该序列。

选择输入 OOB 操作时，当应用程序收到 :c:member:`bt_mesh_prov.Input` 回调时，应提示用户。应通过 :c:member:`bt_mesh_input_string ()` 或 :c:member:`bt_mesh_input_number()` 将用户响应反馈给配置 API。如果在60秒内未记录用户响应，则中止配置过程。

如果供应者希望强制 OOB 身份验证，则必须使用 ``BT_MESH_ECDH_P256_HMAC_SHA256_AES_CCM`` 算法

数据传输
=============

成功验证设备后，配置程序传输配置数据：

* 单播地址
* 网络密钥
* IV 指数
* 网络标志

  * 密钥更新
  * IV 更新

此外，为节点生成设备密钥。所有这些数据都由网格堆栈存储，并调用配置 :c:member:`bt_mesh_prov.complete` 回调。

设置安全性
*********************

根据公钥交换机制和身份验证方法的选择，配置过程可以是安全的，也可以是不安全的。

2021年5月24日，ANSI `disclosed <https://kb.cert.org/vuls/id/799380>`_ Bluetooth Mesh 配置协议中的一组漏洞，
展示了如何在模拟和MITM攻击中利用闪烁、振动、推送、扭曲和输入/输出数字 OOB 方法提供的低熵。
作为响应，Bluetooth SIG 在 BluetoothMesh Profile Specification v1.0.1 
`erratum 16350 <https://www.bluetooth.org/docman/handlers/DownloadDoc.ashx?doc_id=516072>`_ 中将这些OOB方法重新分类为不安全的，
因为 AuthValue 可能是实时强制的。为了确保安全配置，应用程序应该使用静态 OOB 值和 OOB 公钥传输。

API 参考
===============

头文件
---------------

- wm_iot_sdk/components/bluetooth/include/bluetooth/mesh/main.h
- This header file can be included with:

.. code-block:: c
   :emphasize-lines: 1

   #include "bluetooth/mesh/main.h"

.. doxygengroup:: bt_mesh_prov

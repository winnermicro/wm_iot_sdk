
开发助手
===========


概述
---------------

开发助手用于辅助用户基于 WM IOT SDK 进行项目开发，该章节涉及代码组织结构参考，任务属性定义管理，堆栈信息查询等理解。

1. 代码组织结构
---------------

.. figure:: ../../_static/component-guides/dev_asst/sdk_code_structure_overview.svg
    :align: center
    :alt: code sturcture

用户在实际开发过程中需要注意 WM SDK 已经提供了常用的通用定义以及组件以供参考

2. 任务属性定义
---------------

代码路径： components/wm_system/include/wm_task_config.h

.. figure:: ../../_static/component-guides/dev_asst/sdk_code_structure_task_config.svg
    :align: center
    :alt: task configure


3. 交互式功能
---------------

开发助手具有如下交互功能：

3.1 AT 模块
^^^^^^^^^^^^^^^^^

若需要使用 AT 首先需要在 menuconfig 中使能 AT 模块以及使用到的子功能

.. figure:: ../../_static/component-guides/dev_asst/at_command_menuconfig1.svg
    :align: center
    :alt: AT 使能菜单一

然后再根据需求选择对应的 AT 命令集

.. figure:: ../../_static/component-guides/dev_asst/at_command_menuconfig2.svg
    :align: center
    :alt: AT 使能菜单二

更多信息可参考 AT 命令列表： :doc:`AT <AT/index>`

3.2 CLI 模块
^^^^^^^^^^^^^^^^^

若需要使用 CLI 首先需要在 menuconfig 中使能 CLI 模块以及使用到的子功能

.. figure:: ../../_static/component-guides/dev_asst/cli_command_menuconfig1.svg
    :align: center
    :alt: CLI 使能菜单一

然后再根据需求选择对应的 CLI 命令集

.. figure:: ../../_static/component-guides/dev_asst/cli_command_menuconfig2.svg
    :align: center
    :alt: CLI 使能菜单二

CLI 交互默认通过 UART0 (即日志输出口)，查看当前系统支持的 CLI 命令可使用 help 命令，如图

.. figure:: ../../_static/component-guides/dev_asst/cli_command_help_overview.svg
    :align: center
    :alt: CLI help

例如可以使用命令 task 列出当前系统所有任务以及其资源开销，如图

.. figure:: ../../_static/component-guides/dev_asst/cli_command_task_overview.svg
    :align: center
    :alt: CLI task

新增命令信息可参考 CLI 命令： :doc:`CLI <cli>`


4. 存储管理
---------------

WM IOT SDK 包含多种存储机制，比如

- NVStore：用于存储小数据量，如： 蓝牙 和 wifi 连线参数，用户配置参数等。其存储位置是 Flash :ref:`NVStore <nvs>`。
- FT Region：用于存储芯片出厂的参数，比如：RF校准值，ADC 校准值，MAC 地址等。 
              其存储位置是 Flash 或 OTP (one time program)，视不同芯片的硬件设计而定。
              原则上该区域的各参数仅能写一次，写完后会被 lock 住，物理上不能再写。
- File Systems：用于存储大数据内容，如：音频，图片等。并由多种文件系统机制(FATFS, LittleFS...)做管理, 其存储位置可能是 SD 卡 或 Flash。


虽然各存储机制的使命明确，但实际使用时，仍会存在一些容易混淆的场景。

比如：MAC 地址存储及获取 可能同时在NVStore 及 FT Region中都有存储， 而开发者可能有自己的读写策略。
这里介绍下 WM IOT SDK 的 MAC 管理机制

4.1 MAC 地址管理
^^^^^^^^^^^^^^^^^

 WM IOT SDK 的 MAC 管理机制 遵循如下原则

 - 提供统一的 MAC Get/Set API 给用户或其他模块使用。 API 被声明成weak属性，用户可在自己的工程中重新实现覆盖。
 - MAC 地址存储方式和种类存在差异
  
   + NVStore 中存储 包含 Wifi Station,  WiFi SoftAP, BT 这3种不同类型的 MAC 地址。
   + FT Region 中仅存储 包含 WiFi , BT 两种类型 MAC 地址。
   + 代码中 还存在有一个 Default MAC，在 NVStore, FT Region 内容都不可靠的场景下，用于MAC地址导入。
  
 - 原生 MAC API 的读操作流程：
  
   + 上电时，若 NVStore 中存在有效 MAC 地址，则未来 Get MAC 所获取到的就是 NVStore。
   + 若 NVStore 中的 MAC 地址无效，则 Get MAC 所获取到的值 会从 FT Region 中取出，并做 MAC 地址转换。
   + 若 FT Region 中的 MAC 地址也无效， 则 Get MAC 所获取到的值 会从代码 Default MAC 中取出，并做 MAC 地址转换。

 - 原生 MAC API 的写操作流程：

   + 仅可以写到 NVStore中。
   + 若NVStore Disable, Set MAC 会返回错误。
  
 - 默认系统行为：
  
   + 系统上电后，会检查是否 NVStore 中有有效 MAC 地址存储。
   + 若没有有效 MAC 地址，则调用统一 Get MAC API 或获取或产生MAC地址。
   + 获取到的 MAC 地址，会被存储回 NVStore 中。
  


.. figure:: ../../_static/component-guides/driver/wm_mac_addr_flow_chart.svg
    :align: center
    :alt: WM IOT SDK MAC Address Load Flow


统一的 MAC Get/Set API：

- 这两个API 为Weak属性，用户有自己的 MAC 管理机制，可以在自己的工程中，比如工程里的 main.c 中，重新实现这两个函数。
- 在编译链接时，链接器会直接链接用户实现的新函数。

.. code-block::

    ATTRIBUTE_WEAK int wm_sys_set_mac_addr(wm_mac_type_t mac_type, uint8_t *mac_addr, uint8_t mac_addr_len);
    ATTRIBUTE_WEAK int wm_sys_get_mac_addr(wm_mac_type_t mac_type, uint8_t *mac_addr, uint8_t mac_addr_len);

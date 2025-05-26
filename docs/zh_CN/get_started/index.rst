***********
快速入门
***********

本文档旨在指导用户搭建硬件开发的软件环境，通过一个简单的示例展示如何使用 WM IoT SDK 配置菜单，并编译、下载固件至开发板等步骤。

概述
============

WM IoT SDK 对 SoC 芯片支持以下功能：

.. only:: w800

    * 2.4 GHz Wi-Fi
    * 低功耗蓝牙
    * 高性能 32 位 CK804 单核处理器
    * 多种外设
    * 内置安全硬件

SoC 采用 40 nm 工艺制程，具有最佳的功耗性能、射频性能、稳定性、通用性和可靠性，适用于各种应用场景和不同功耗需求。

WinnerMicro 为用户提供完整的软、硬件资源，进行硬件设备的开发。其中，WM IoT SDK 旨在协助用户快速开发物联网 (IoT) 应用，可满足用户对 Wi-Fi、蓝牙、低功耗等方面的要求。

准备工作
=============

硬件
--------------

* 一款开发板
* USB 数据线
* 电脑（Windows 或 Linux）

.. only:: w800

    以下是 W800 官方开发板，点击链接可了解更多硬件信息。

    .. toctree::
       :maxdepth: 1

       W800-Arduino <w800_arduino>

       W803-Pico <w803_pico>

       HLK-W800 <hlk_w800>

       HLK-W806 <hlk_w806>

软件
---------

如需使用 WM IoT SDK，请安装以下软件：

    * 搭建编译环境，用于编译固件代码；
    * 获取 `WM IoT SDK <https://doc.winnermicro.net/download/version>`_ 软件开发框架源码。

.. _compiling-environment:

搭建编译环境
==============

**目前提供两种编译环境：**

- 命令行编译（ :ref:`SHELL<shell-compiling-environment>` 编译）
- 集成工具编译（ :ref:`IDE<ide>` 编译）

.. hint::

    由于不同用户使用的操作系统可能不同，于是我们针对市面上常用的几种操作系统（:ref:`Windows 系统<windows-system>` 和 :ref:`Linux 系统<linux-system>`）分别提供了搭建环境的步骤，用户可以根据自己的实际情况任选其一即可。


.. _shell-compiling-environment:

搭建命令行编译环境
----------------------

.. toctree::
   :maxdepth: 1

   windows
   linux

使用 IDE 搭建编译环境
---------------------------

.. toctree::
   :maxdepth: 1

   ide

.. _build-function:

编译第一个工程
==================

**选择一个工程**

如果已经安装好 WM IoT SDK，请在命令提示行中切换到工程目录，如

::

    cd examples
    cd hello_world

需注意这是没有使用集成开发环境（IDE）的操作示例，WM IoT SDK 和工程存放路径不允许带空格等特殊字符。

第一次使用工程时，请检查工程目录下是否已经存在 **build** 文件夹，如果存在，那么在编译前删掉它，
否则可能因为 build 文件夹中已有的构建信息导致本次编译失败。
对于从其它地方拷贝来的工程，请务必要检查此项。

**设置芯片类型**

::

    wm.py set-soc <type>

当前默认的芯片类型为 W800，当不执行设置芯片类型时，默认使用 W800 的配置。

可执行 ``wm.py set-soc --list-socs`` 查看当前支持的所有芯片类型，也可以在 ``menuconfig`` 的 ``SoC configuration`` 菜单中选择芯片类型。

需注意此操作将清除并初始化项目之前的编译和配置。


**配置工程编译参数**

::

    wm.py menuconfig

或

::

    wm.py guiconfig

工程中的所有配置都有默认值，所以可以跳过配置工程步骤，不影响后续的编译和使用。

**配置工程设备表**

::

    wm.py devconfig

设备表存储了当前 SoC 的硬件配置参数，各 SoC 拥有默认配置，所以可以跳过配置设备表步骤，不会影响后续的编译和使用。


**编译工程**

::

    wm.py build


**烧录到设备**

运行以下命令可以进行烧录，PORT 是当前开发板连接的串口。

::

    wm.py flash -p PORT

其中的 PORT 需替换为系统中实际串口的名字（比如 Windows 下的 COM1，或者 Linux 下的 /dev/ttyUSB0）。
如果省略 -p 选项，wm.py flash 会尝试使用第一个可用的串口进行烧写。
如果想擦除整块 flash 然后重新烧写，请运行 ``wm.py flash -e -p PORT`` 。
默认完成烧录之后会自动重启设备，如果想在烧录之后不自动重启设备，请运行 ``wm.py flash -m -p PORT`` 。

如果想使用图形化烧录工具，请选择使用 `Upgrade Tools 工具 <http://isme.fun/?log=blog&id=34>`_ 烧录固件到设备，
编译得到的固件在工程目录的 `build` 文件夹中，固件介绍请参阅 :ref:`固件格式<firmware_format>`。

**观察串口运行信息**

您可以使用 ``wm.py monitor -p PORT`` 命令，实时显示设备串口输出的工程运行信息。
注意，不要忘记将 PORT 替换为您的串口名称，串口名称规则同上小结 `<烧录到设备>` 所述。

``wm.py monitor`` 只提供最基础的串口信息显示功能，启动后您可使用快捷键 ``Ctrl+C`` 终止捕获设备串口信息。

当然，您也可以选择其它串口工具观察设备输出信息，WM IoT SDK 并不限定所用的串口工具。


.. tip::

    您也可以运行以下命令，一次性执行编译、烧录和观察串口信息：

    ``wm.py flash monitor -p PORT``



卸载编译环境
=================

如果您是 Windows 用户，并且使用的是 ``WM IoT SDK 集成包`` 搭建的编译环境，则可以在 ``设置 - 应用 - 应用和功能``（或 ``控制面板 - 程序 - 卸载程序``）运行卸载向导来卸载 WM IoT SDK 编译环境。

如果您使用的是 ``VS Code`` 搭建的编译环境，可以在 VS Code 左侧的插件列表里选择已安装的 WM-IDE 插件，在跳转的页面中就有卸载选项，点击卸载即可。

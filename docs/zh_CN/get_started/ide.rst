
.. _ide:

VS Code
==================
**目前提供以下 IDE 可供用户使用：**

:ref:`VS Code<ide>`

**环境要求**

- VS Code 需要 V1.70.0 及以上版本, 否则插件无法安装成功

.. _vscode_windows:

**Windows 环境下使用**

1. 安装 VS Code

访问 Visual Studio Code 的 `官方网站 <https://code.visualstudio.com/>`__ 下载并正确安装。

2. 打开 VS Code 之后点击最左侧的扩展，然后搜索 ``WM-IDE`` 进行安装

.. figure:: ../../_static/get_started/ide/wm_vscode_ide_plugin.svg
   :align: center

3. 为了更好的使用 VS Code 进行开发，建议再安装如下扩展：

- Microsoft C/C++ extensions

- Microsoft Serial Monitor Extension

4. 打开 ``WM-IDE`` 插件

点击左侧功能面板 WinnerMicro 图标进入插件主界面，在该界面上可以进行环境配置、创建工程等操作。

.. figure:: ../../_static/get_started/ide/wm_vscode_ide_ui.svg
   :align: center

5. 配置 ``WM-IDE`` 插件

在使用 WM-IDE 前必须先进行配置，在配置前需要获取 `WM IoT SDK <https://doc.winnermicro.net/download/version>`_ 软件开发框架源码。

完成下图的这些配置，包括 ``WM IoT SDK Path`` 和 ``WM Dev Suite Path`` 这两项配置

.. figure:: ../../_static/get_started/ide/wm_vscode_ide_win_config1.svg
   :align: center

其中 ``WM IoT SDK Path``:

::

  WM IoT SDK 源码下载存放路径\wm_iot_sdk

其中 ``WM Dev Suite Path``:

若已经参考 :ref:`Windows 搭建命令行编译环境<windows-system>` 下载安装了集成开发环境，可直接进行配置

::

  集成开发环境安装路径\winnermicro\wmclics

若没有安装 :ref:`Windows 搭建命令行编译环境<windows-system>` 集成开发环境，可点击 ``Install`` 并选择安装路径进行自动安装配置

.. figure:: ../../_static/get_started/ide/wm_vscode_ide_win_config_install.svg
   :align: center

若不希望采用集成开发环境的方式，还可以点击 ``Advance`` 进行高级配置,单独配置每一项

.. figure:: ../../_static/get_started/ide/wm_vscode_ide_win_config2.svg
   :align: center

配置完成需点击 ``Save``

6. 创建工程

有三种方式创建工程：

- Creat Empty Project: 创建一个空的工程，创建后可在该工程添加相应功能形成自己的项目
- Creat Hello World Project: 创建一个最简单的打印 `Hello World` 工程，可快速体验
- Import Project: 导入 WM IoT SDK examples 目录下的各种示例工程，可快速掌握相关功能的使用

.. figure:: ../../_static/get_started/ide/wm_vscode_ide_creat_project.svg
   :align: center

在创建工程界面输入工程保存位置以及工程名之后，就会生成工程。

7. 对创建的工程进行配置、编译、烧录、运行等操作

选择任一工程文件，如 main.c 文件，点击 main.c 然后右键:

.. figure:: ../../_static/get_started/ide/wm_vscode_ide_build.svg
   :align: center

根据上述图片步骤:

  1、选择 `menuconfig` 打开设置菜单进行项目配置

  2、选择 `build` 编译工程

  3、选择 `flash` 烧录固件

也可以在底部的状态栏中的 WM IoT SDK 菜单中选择对应命令进行操作。

成功烧录固件后，可点击 `串口监行器` -> `更改端口号(与芯片端口号一致)` -> `更改波特率` -> `运行` 查看运行结果。

.. figure:: ../../_static/get_started/ide/wm_vscode_ide_log.svg
   :align: center

.. hint::

   * 同时也可以无需执行第六步创建工程，直接打开 wm_iot_sdk 右键 examples 下的工程进行配置、编译、烧录、运行等操作。

**Linux 环境下使用**

使用方法和步骤完全同 :ref:`Windows 环境下使用<vscode_windows>`, 以下重点描述有差异步骤：

1. 安装 VS Code

在Linux 系统上更 **推荐** 使用类似 ``apt`` 、``yum``、``dnf`` 等之类包管理工具自动安装相关工具。以下是在 Ubuntu 系统上使用 apt 命令的示例:

::

   sudo apt update
   sudo apt install code

5. 配置 ``WM-IDE`` 插件

在配置前需要获取 `WM IoT SDK <https://doc.winnermicro.net/download/version>`_ 软件开发框架源码，并参考 :ref:`Linux 搭建命令行编译环境<linux-system>` 下载安装开发编译环境。

完成下图的这些配置，配置的路径需要根据实际安装路径进行配置。

.. figure:: ../../_static/get_started/ide/wm_vscode_ide_linux_config.svg
   :align: center

.. hint::

   * 连接开发板，需要安装对应 USB 转串口（比如：CH34X）驱动。
   * 串口无法打开时，可能是串口权限问题，尝试通过 sudo gpasswd --add 用户名 dialout 将用户添加到 dialout 组，以获得串口访问权限，或者使用 sudo chmod 777 /dev/ttyUSB0 （以实际使用串口为准）来给予所有用户读写权限。

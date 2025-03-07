.. _cli:

CLI
=======

概述
--------

CLI 是一种通过命令行界面与设备或芯片进行交互的方式，用户可以通过输入特定的命令来执行各种操作。



基本原理
----------

CLI 的核心是开源项目 `Letter-shell <https://github.com/NevermindZZT/letter-shell>`_ ，Letter-shell 是一个 C 语言编写的可以嵌入在程序中的 shell，可以通过命令行调用，运行程序中的函数。主要的功能有：

- 支持 **tab** 键自动补全；
- 支持 **help** 显示命令用法；
- 命令处理函数同 Linux 命令行函数；
- 添加方便，可随意放置；
- 支持功能和部分命令裁剪；
- 支持选择任意串口 (默认使用 UART0，波特率 115200，数据位 8位，停止位 1位，无奇偶校验，无流控)

定义CLI命令
-------------

使用 ``WM_CLI_CMD_DEFINE`` 定义一条命令，其用法如下：
::
    
    WM_CLI_CMD_DEFINE(cmd, handler, description, usage)

其中 ``cmd`` 为命令名称、 ``handler`` 为命令处理函数、 ``description`` 为命令描述、 ``usage`` 为命令详细说明，如果想裁剪固件体积，可以通过 ``menuconfig`` 关闭 ``usage`` 功能。

若想在命令处理函数 ``handler`` 中输出打印信息，可使用 ``int wm_cli_printf(const char *fmt, ...)`` 函数进行输出，用法和 printf 函数完全一致。

使用示例
-----------

添加一个名为 ``test`` 的命令
::
  
    static void cmd_test(int argc, char *argv[])
    {
        /* do somethings... */
        
        wm_cli_printf("somethings\r\n");
    }

    WM_CLI_CMD_DEFINE(test, cmd_test, Display test information, test -- Display test information)

其处理函数为 ``cmd_test`` ， 执行 ``help`` 命令可以看到其描述信息 ``description`` 为Display test information，执行 ``help test`` 命令可以看到其详细用法 ``usage`` 为test -- Display test information。
处理函数中， ``argc``  表示参数数量， ``argv`` 是一个参数指针数组，指向传递给函数的参数字符串，如：argv[0] 是命令的名称，argv[1] 是第一个参数，argv[2] 是第二个参数。

使用介绍
-----------

输入命令后，按回车键会执行命令，按退格键可以删除输入的字符，使用键盘方向键中的左右键可以改变光标位置，
使用键盘方向键中的上下键进行历史命令选择，输入某个命令的首字符开始的字符后可按 tab 键自动补全，也可直接按 tab 键显示全部的 CLI 命令。

当前 CLI 仅支持最基本的字母、数字、回车、退格、方向按键，不支持如 ctrl + c 等特殊功能和组合按键等。

当遇到不熟悉的命令时，可以通过下面的方式查看帮助：
    - 输入 ``help`` 后回车，会显示全部的命令，并在命令后面显示其简单帮助信息。
    - 输入 ``help command`` 后回车，会显示 `command` 命令对应的详细帮助信息，该信息可被裁剪。
    - 可根据命令自己提供的帮助参数查看，如 ``command --help``、``command -h``、``command help``，该信息由具体命令提供。


使用 CLI 时，必须要选择一款支持 shell 终端功能的串口工具，如 `PuTTY <https://putty.org/>`_、`XSHELL <https://www.xshell.com/zh/xshell>`_、`SecureCRT <https://www.vandyke.com/products/securecrt>`_ 等。

.. attention::
    像 sscom、xcom 等这类嵌入式中常见的串口工具，他们统统不支持完整的 shell 终端特性，使用它们的话并不能完善的体验 CLI 特性。
    这类工具也包括 `Upgrade Tools 工具 <http://isme.fun/?log=blog&id=34>`_、VS Code 中安装的 Serial Monitor 插件。

以使用 PuTTY 为例，

直接打开某个串口可按如下步骤使用：

.. figure:: ../../_static/component-guides/cli/putty_open.png
    :align: center
    :alt: 使用 PuTTY 打开串口

先保存某个串口配置，之后加载配置打开串口使用：

.. figure:: ../../_static/component-guides/cli/putty_save.png
    :align: center
    :alt: 使用 PuTTY 保存串口配置

.. figure:: ../../_static/component-guides/cli/putty_load.png
    :align: center
    :alt: 使用 PuTTY 载入串口配置

打开某个串口时的效果如下：

.. figure:: ../../_static/component-guides/cli/putty_main.png
    :align: center
    :alt: 使用 PuTTY 操作串口

CLI 的 meunconfig 配置
------------------------

.. list-table::
   :widths: 45 50 25 
   :header-rows: 0
   :align: center

   * - 配置名称
     - 配置描述
     - 默认值

   * - CONFIG_COMPONENT_CLI_ENABLED
     - 是否启用 CLI 组件
     - N

   * - CONFIG_CLI_LINE_SIZE 
     - 配置单行输入大小
     - 64 

   * - CONFIG_CLI_CMD_PARAS_NUM
     - 配置命令行输入参数个数
     - 8
    
   * - CONFIG_CLI_CMD_HISTORY_NUM
     - 配置历史命令个数
     - 5

   * - CONFIG_CLI_ENABLE_LONG_USAGE
     - 配置使用 CLI 的端口
     - Uart0

   
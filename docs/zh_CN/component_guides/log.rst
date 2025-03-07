
.. _wm_log:

LOG
=============

简介
-------------

    LOG（日志）主要用于开发过程中的调试信息输出。


功能列表
------------
    1. :ref:`LOG分级 <log_level>`：LOG 信息分等级输出。
    2. :ref:`LOG输出 <log_output>`：LOG 输出。
    3. :ref:`printf <printf>`：原始 printf 输出。
    4. :ref:`Dump <dump>`：16 进制方式 dump 输出。
    5. :ref:`颜色配置 <color>`：log 颜色配置。
    6. :ref:`Direct <direct>`：直接输出。
    7. :ref:`串口选择 <port>`：串口选择。


LOG 功能概述
---------------

    LOG 包括 2 个部分，一是标准 C stdio.h 中定义的 printf, fprintf, fputs, puts, putchar 函数， 这个部分在 libc_port.c 中有 overwrite 并对接到串口上。

    另一个部分是功能更强大的 wm_log 模块,代码位于 components/wm_log ，提供了分级，颜色，时间戳等更多的功能。


主要功能
---------------

.. _log_level:

LOG 等级
^^^^^^^^^^^^^^^

    LOG 共有 6 个级别， ``none`` 、 ``error`` 、 ``warn`` 、 ``info`` 、 ``debug`` 、 ``verbose``，其定义如下：

    .. code:: c

        typedef enum {
            WM_LOG_LEVEL_NONE,    /**< No log output        */
            WM_LOG_LEVEL_ERROR,   /**< Critical errors      */
            WM_LOG_LEVEL_WARN,    /**< warnings             */
            WM_LOG_LEVEL_INFO,    /**< Information messages */
            WM_LOG_LEVEL_DEBUG,   /**< Debug informations   */
            WM_LOG_LEVEL_VERBOSE, /**< Extra information which is not necessary */
        } wm_log_level_t;



    模块中默认等级是 INFO 级别，只有调用 error, warn, info 对应的宏能输出 LOG 信息。 debug, verbose 的宏不能输出。如果需要输出 debug, verbose的信息，需要配置输出等级。

    CONFIG_LOG_DEFAULT_LEVEL 宏是编译前配置的，可以在 menuconfig 中进行配置。运行中也可以调用 wm_log_set_level 接口进行配置。

.. _log_output:

LOG 输出
^^^^^^^^^^^^^^^

    **1. 格式化输出**

    LOG 格式化输出一般使用如下几个宏：

    wm_log_error

    wm_log_warn

    wm_log_info

    wm_log_debug

    wm_log_verbose

    其参数和 printf 的参数一样，等级从 1 到 5 。当等级设置为 1 时，只有等级 1 对应的宏 wm_log_error 才会输出 LOG 信息。
    等级设置为 0， 所有 LOG 都不输出，等级设置为 5，所有 LOG 都输出。

    **2. 原始信息输出**

    下面这个宏输出原始信息，类似 printf 函数的输出， 不带时间戳等信息。

    wm_log_raw

    **3. 格式**

    下面是 LOG 输出的格式，最前面中括号内是等级，第二部分小括号内是时间戳，从开机开始的毫秒数， 第 3 列是 LOG TAG , 用于区分
    不同模块输出的信息。

    .. code::

        [E] (5017810) at_log: test wm_log_error
        [W] (5017810) at_log: test wm_log_warn
        [I] (5017810) at_log: test wm_log_info
        [D] (5017810) at_log: test wm_log_debug
        [W] (5017810) at_log: test wm_log_verbose


    **3. LOG_TAG**

    wm_log.h 中定义了默认的 LOG_TAG 为 "NO_TAG" ，如下所示：

        .. code::c

            #if !defined(LOG_TAG)
                #define LOG_TAG "NO_TAG"
            #endif

    在自己模块的 .c 文件中，包含 wm_log.h 之前可以定义 LOG_TAG 为其他字符串。例如：

        .. code::c

            #define LOG_TAG "my_module"
            #include "wm_log.h"

    .. warning::
        不要在对外的头文件中定义 LOG_TAG ，这样容易引起重复定义。


.. _printf:

printf
^^^^^^^^^^^^^^^

    printf 函数也可以使用，但没有等级，时间戳等控制，也无法通过配置去关闭，所以不建议使用。



.. _dump:

Dump
^^^^^^^^^^^^^^^

    Dump 输出调用 wm_log_dump , 如下所示：


    .. code::c

        wm_log_dump(WM_LOG_LEVEL_INFO, "test", 16, buf, sizeof(buf));



.. _color:

颜色
^^^^^^^^^^^^^^^

    LOG 模块默认没有打开颜色功能，可以在 menuconfig 中打开，配置项为 CONFIG_LOG_USE_COLOR 。
    打开后，每条 LOG 前后会加上颜色代码，不同 LOG 级别的颜色不同，在支持颜色的串口工具上显示更美观。


.. _Direct:

Direct 输出模式
^^^^^^^^^^^^^^^

    目前 LOG 的底层是调用串口驱动去输出信息，在 driver 层串口驱动初始化前 ，LOG 信息直接写到串口的硬件 fifo 中输出，
    在 driver 层串口驱动初始化后，会把格式化好的 LOG 信息输出到串口驱动的 TX Buffer 中，后面驱动会在中断处理时把 Buffer 中
    的数据发送到硬件 fifo 中。

    Direct 输出模式就是强制把数据直接发送到串口硬件 fifo 中，不通过串口驱动的软件 Buffer 缓冲。

    Direct 输出模式可以在 menuconfig 中配置， 配置项为 CONFIG_LOG_WRITE_DIRECT。


.. _port:

串口端口
^^^^^^^^^^^^^^^

    串口端口可以在 menuconfig 中选择，默认使用串口 0 ，可以直接在 prj.config 文件中配置，如需配置到串口 1 ，可以配置为：

    CONFIG_LOG_UART_PRINT_USE_UART1=y




应用实例
---------

    参考示例如下：

    .. code:: c

        #define LOG_TAG "test"
        #include "wm_log.h"
        int main(void)
        {
            uint8_t buf[66];
            for (int i = 0; i < sizeof(buf); i++) {
                buf[i] = i;
            }
            wm_log_error("test wm_log_error");
            wm_log_warn("test wm_log_warn");
            wm_log_info("test wm_log_info");
            wm_log_debug("test wm_log_debug");
            wm_log_verbose("test wm_log_verbose");
            wm_log_raw(WM_LOG_LEVEL_INFO, "test wm_log_raw\n");
            wm_log_dump(WM_LOG_LEVEL_INFO, "test", 16, buf, sizeof(buf));
            return 0;
        }



    运行结果：

    ::

        [I] (2) main: ver: 2.0.0rc2 build at Sep 12 2024 17:15:18
        [E] (2) test: test wm_log_error
        [W] (2) test: test wm_log_warn
        [I] (2) test: test wm_log_info
        test wm_log_raw
        start dump [test]
        0x0000   00 01 02 03 04 05 06 07  08 09 0a 0b 0c 0d 0e 0f  ................
        0x0010   10 11 12 13 14 15 16 17  18 19 1a 1b 1c 1d 1e 1f  ................
        0x0020   20 21 22 23 24 25 26 27  28 29 2a 2b 2c 2d 2e 2f   !"#$%&'()*+,-./
        0x0030   30 31 32 33 34 35 36 37  38 39 3a 3b 3c 3d 3e 3f  0123456789:;<=>?
        0x0040   40 41                                             @A



使用限制
---------

    ``wm_log_xx`` 有一个限制，第一个参数必须为常量字符串或者能替换成常量字符串的宏。例如下面这个是错误的：

    .. code:: c

        const char* msg = "123";
        wm_log_info(msg);

    应该这样使用：

    .. code:: c

        const char* msg = "123";
        wm_log_info("%s",msg);


    .. warning::

        LOG, AT, CLI 都在使用 Uart 作为输出，他们使用的 Uart Port 口都可以在 menuconfig 中配置，
        要注意调整时 AT 和 CLI 不能使用同一个 Uart Port 口。LOG 即可以和 AT 同一个口，也可以和CLI
        同一个口。默认配置中，LOG,CLI 都用 Uart0, AT 用 Uart1。

    .. warning::

        wm_printf_direct 函数直接把 LOG 阻塞方式输出到  Uart 硬件 fifo 中 ，像 printf 一样不受 Log 级别限制，
        建议仅在中断关闭的场景或者 OS 已经不能正常调用的场景下调试使用。

    .. note::

        浮点数格式化中，支持 %f 和 %lf , 不支持 %e, %E, %g, %G。另外，整数格式化不支持 %zu, %zi, %zd。


LOG 的 meunconfig 配置
--------------------------

主要配置如下：

.. list-table::
   :widths: 45 50 25 
   :header-rows: 0
   :align: center

   * - 配置名称
     - 配置描述
     - 默认值

   * - CONFIG_LOG_PRINT_UART_MODE 
     - 配置输出端口
     - Uart0

   * - CONFIG_LOG_DEFAULT_LEVEL
     - 配置输出级别
     - INFO 级别

   * - CONFIG_LOG_USE_COLOR
     - 配置输出 LOG 支持颜色
     - N


API 参考
---------

    查找 LOG 相关 API 请参考:

    :ref:`label-api-log`
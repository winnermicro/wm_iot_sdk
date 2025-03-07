
.. |Equipment-Name| replace:: w800
.. _debug:

调试方法
==============

本文将指导使用 |Equipment-Name| 的调试方法：

1.  :ref:`添加调试信息<log>` 
2.  :ref:`使用调试器在线调试 <cklink>` 
3.  :ref:`分析异常产生后的现场 <ramdump>` 
4.  :ref:`获取重启原因<bootreason>`

.. _log:

:ref:`添加调试信息 <debug>` 
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**使用 wm_log_xx 打印 log 信息：**
------------------------------------------------------------------

``wm_log_xx`` 函数在 ``wm_log.h`` 头文件里声明，它有 6 种 log 级别供用户根据实际需要选择： ``none``，``error``，
``warn``， ``info``， ``debug``， ``verbose``，也提供断言接口、十六进制 dump 接口以及原始信息输出接口。
并支持按不同的级别将信息以不同的颜色进行标识显示（需串口工具支持颜色功能）。

``wm_log_xx`` 使用方法和 C 库函数 ``printf`` 相似，它不仅支持 ``printf`` 的大部分功能，使用 ``wm_log_xx`` 系列
接口时，除了原始信息输出接口外，都会在末尾自动添加换行符，减轻开发者重复输入工作量。

``wm_log_xx`` 有一个限制，第一个参数必须为常量字符串或者能替换成常量字符串的宏。例如下面这个是错误的：

.. code:: c

  const char* msg = "123";
  wm_log_info(msg);

应该这样使用：

.. code:: c

  const char* msg = "123";
  wm_log_info("%s",msg);


在使用时，可以通过 menuconfig 进行配置，通过 ``Components configuration`` ---> ``log`` ---> 
``default log level`` 选择 log 级别，也可通过 api 在程序中进行动态控制。
  
在 cli 中，查看 log 信息时还可以通过命令设置显示级别，如设置 debug 级别的信息：

::

    log debug

另外，对于需要异步打印的用户，wm_log 也支持以任务方式进行打印。需要通过 menuconfig 启用 log task 打印，然后调整合适的 log 任务优先级和 log 缓存队列大小。但需注意过高的 log 任务优先级可能导致失去异步意义，过小的缓存队列可能导致丢失部分打印信息。

打印信息默认使用 uart0 输出。如果用户想对此进行选择，那么以设置输出串口为 uart1 为例，操作如下：

.. only:: w800

    进入 ``menuconfig``，选择 ``Components configuration`` 并进入 ``Log`` 组件进行配置
    就可以选择使用对应的串口，甚至可以选择不打印。


.. _cklink:

:ref:`使用调试器在线调试 <debug>` 
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

在线调试需要使用 cklink 调试器，下载并安装 debug server 软件。cklink 和 debug server 详细用法请查看 `T-Head Debugger Server User Guide <../../../../download/debug_server/T-Head_Debugger_Server_User_Guide_ZH-CN.pdf>`_。

.. important::

    对于调试 XIP 方式运行的代码，cklink 调试器只支持最多 **2** 个硬件断点。
    为了让调试方便，Debugger Server 会将超过 2 个后的断点，
    以擦除 flash 中原代码，重新插入新的代码替换的方式来实现了更多的断点，正常情况下调试完毕它会还原代码。
    所以在使用 cklink 调试 XIP 方式运行的代码时，当断点数超过 2 个后，就会存在丢失 flash 上原有数据的风险。

.. only:: w800

w800 使用 cklink 调试器的接线方式如下表

   .. list-table:: 
      :widths: 25 25 25
      :header-rows: 0
      :align: center

      * - w800
        - cklink
        - 说明

      * - PIN_NUM_1（PA1）
        - TCK
        - 必须连接

      * - PIN_NUM_4（PA4）
        - TMS
        - 必须连接

      * - GND
        - GND
        - 必须连接

      * - RST（复位脚）
        - TRST
        - 可选连接（推荐接上硬复位稳定）

      * - 3V3
        - 3V3
        - 可选连接


其中 gdb 已经包含在编译工具链中不需要额外安装。

**gdb 命令行：**
------------------------------------------

在 |Equipment-Name| 连接 cklink 调试器后，先运行 debugserver ，然后

使用编译命令行输入：

::

    wm.py gdb

启动 gdb 进入调试模式，gdb 更多操作请参考 `T-HEAD CPU 调试技巧 <../../../../download/debug_server/T-HEAD_CPU_Debugging_Tips_CN.pdf>`_。

此外 WM IoT SDK 提供了 4 个调试命令，它们的作用分别是：

- ``wmbt``   : 查看当前FreeRTOS 中所有 task 状态。
- ``wmst``   : 根据 ``wmbt`` 中显示的 task index 将cpu上下文切换到该 task 调用栈。
- ``wmbk``   : 将cpu上下文恢复为第一次进入调试器时的 task 调用栈。
- ``wmheap`` : 查看当前堆内存分配情况。

.. _another-debug:

**IDE调试：**
------------------------------------------

.. toctree::
   :maxdepth: 1

   ide_debug

.. _ramdump:

:ref:`分析异常产生后的现场 <debug>` 
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

WM IoT SDK 提供了不需要调试器就可以分析异常产生后的现场的方法。

**查看死机原因和当前地址 （按照异常提示操作查看）：**
-------------------------------------------------------------

当死机或异常发生时 |Equipment-Name| 会将错误信息当前地址以及异常名称打印出来。

以程序运行除 0 异常为例，|Equipment-Name| 会打印如下异常信息：

::

    CPU Exception : 3
    Exception name: Divide by zero

    r0 : 0x00000000 r1 : 0x0000000a r2 : 0x0000000a r3 : 0x00000000
    r4 : 0x04040404 r5 : 0x05050505 r6 : 0x06060606 r7 : 0x07070707
    r8 : 0x20006000 r9 : 0x09090909 r10: 0x10101010 r11: 0x11111111
    r12: 0x12121212 r13: 0x13131313 r14: 0x20005ffc r15: 0x0800cc0e

    psr: 0x80000340
    pc : 0x0800cc12

    Show pc info by run: csky-elfabiv2-addr2line -p -f -e ./build/hello_world.elf -a 0x0800cc12 
    0x0800cc0e



接下来只需在编译命令行运行异常信息中的提示命令：

::

    csky-elfabiv2-addr2line -p -f -e ./build/hello_world.elf -a 0x0800cc12
    0x0800cc0e 

这时候可以看到错误发生的位置。

::

    0x0800cc12: test_task at winnermicro-iot-os/examples/hello_world/main/src/main.c:9
    0x0800cc0e: test_task at winnermicro-iot-os/examples/hello_world/main/src/main.c:9


简单问题可以使用这种方法，遇到复杂问题时推荐使用下面这种方法。

**ramdump分析：**
-------------------------------------------------------------

ramdump分析主要包括两个步骤：

1. 使用 ``wm.py ramdump –p port`` 导出现场
   
发生错误或异常时不复位系统，使用编译命令行输入：

::

    wm.py ramdump -p port

其中 ``port`` 为端口号，只有一个串口时可不填写该参数。执行完成后就可以使用下面的命令。

2. 使用 ``wm.py debug`` 分析导出的 ramdump

导出 ramdump 后输入 ``wm.py debug`` 进入 debug 模式，接下来的调试方法与 gdb 命令行在线调试完全一样，此处不再介绍。

以重复释放内存为例，|Equipment-Name| 会立马打印如下异常信息：

::
    
    CPU Exception : 16
    Exception name: Trap instruction

    r0 : 0x0000004d r1 : 0x0000000a r2 : 0x0000000a r3 : 0x0000004d
    r4 : 0x20005fc0 r5 : 0x05050505 r6 : 0x06060606 r7 : 0x07070707
    r8 : 0x20005fd0 r9 : 0x09090909 r10: 0x10101010 r11: 0x11111111
    r12: 0x0800b9bc r13: 0x20005f9c r14: 0x20005fbc r15: 0x0801a7d4

    psr: 0x80000340
    pc : 0x0801a7d4

    Show pc info by run: csky-elfabiv2-addr2line -p -f -e ./build/hello_world.elf -a 0x0801a7d4 0x0801a7d4

这个时候我们使用 ``wm.py ramdump –p port`` 导出异常现场，再使用 ``wm.py debug`` 进入
gdb 调试，使用 ``bt`` 命令。可以看到如下信息：

::

    (cskygdb) bt
    #0  0x0801a7d4 in heap_free (pv=0x200060b0) at winnermicro-iot-os/components/heap/src/wm_heap.c:245
    #1  0x0801ab74 in wm_heap_caps_free (p=0x200060b0) at winnermicro-iot-os/components/heap/src/wm_heap.c:513
    #2  0x08018456 in free (ptr=0x200060b0) at winnermicro-iot-os/components/wm_hal/src/wm_hal_mem.c:43
    #3  0x0800cc32 in test_task (parameters=0x0) at winnermicro-iot-os/examples/hello_world/main/src/main.c:11
    #4  0xfffffffe in ?? ()

显而易见，在 ``main.c`` 中的 ``test_task`` 函数的第 11 行，调用 ``free`` 函数触发了异常。

.. hint:: 
    
    * 当前 ramdump 分析功能不支持查看外设寄存器（register）中的数据。

.. _bootreason:

:ref:`获取重启原因 <debug>`
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    当系统重启时，若希望能获取到重启原因，可以调用 SDK 提供的 reboot reason APIs.
    API 包含设定和获取两种操作。
    在系统内部已经 在一些场景中预先调用了 ``wm_set_reboot_reason()`` 设定 reboot reason， 比如：软件 reboot, 系统 Exception, Watch Dog Timeout 的地方。

::

    typedef enum {
        WM_REBOOT_REASON_POWER_ON    = 0, /**< power on or reset button */
        WM_REBOOT_REASON_STANDBY     = 1, /**< chip standby */
        WM_REBOOT_REASON_EXCEPTION   = 2, /**< exception reset */
        WM_REBOOT_REASON_WDG_TIMEOUT = 3, /**< watchdog timeout */
        WM_REBOOT_REASON_ACTIVE      = 4, /**< user active reset */
        WM_REBOOT_REASON_MAX              // No comment needed for the max value
    } wm_reboot_reason_t;

    /**
      * @brief     Set reboot reason code
      *
      * @param     reason : reason code
      *
      * @return  None
      */
    void wm_set_reboot_reason(wm_reboot_reason_t reason);

    /**
      * @brief  Get reboot reason code
      *
      * @return wm_reboot_reason_t
      */
    wm_reboot_reason_t wm_get_reboot_reason(void);

用户可在程序初始阶段，例如在用户的应用 Application Task 开启时，调用 ``wm_get_reboot_reason()`` 获取并打印 reboot 原因。

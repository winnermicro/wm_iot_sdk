.. _heap:

堆内存管理
=============

概述
--------

堆（Heap）是动态分配内存的区域，用于存储程序运行时动态分配的内存空间。

堆内存
--------

栈 (stack) 和堆 (heap) 的区别
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

WM IoT SDK 应用程序使用常见的计算机架构模式：由程序控制流动态分配的内存（即 **栈**）、由函数调用动态分配的内存（即 **堆**）以及在编译时分配的 **静态内存**。

由于 WM IoT SDK 是一个多线程 RTOS 环境，因此每个 RTOS 任务都有自己的栈，这些栈默认在创建任务时从堆中分配。


W800 包含多种类型的 RAM：

- SRAM
- DRAM
- PSRAM

可以基于属性的内存分配器允许应用程序按不同目的进行堆分配。


多数情况下，可直接使用 C 标准函数库中的 ``malloc()``  和 ``free()`` 函数实现堆分配。为充分利用各种内存类型及其特性，WM IoT SDK 还具有基于内存属性的堆内存分配器。
分配具有特定属性的内存，可使用
::

    wm_heap_caps_alloc()

内存属性
^^^^^^^^^^^^^^

编程时提供属性：

- WM_HEAP_CAP_DEFAULT，默认属性，指明这种属性的内存在使用时并不关心其来源，可能为 SRAM、DRAM、PSRAM 之一。
- WM_HEAP_CAP_INTERNAL，片内内存，指明这种属性的内存在使用时只能是片上的内存，可能为 SRAM 和 DRAM 之一。
- WM_HEAP_CAP_SPIRAM，片外内存，指明这种属性的内存在使用时只能是 PSRAM。
- WM_HEAP_CAP_EXEC，可执指令内存，指明这种属性的内存在使用时只能是 SRAM。
- WM_HEAP_CAP_SHARED，和高速外设可共享使用的内存，指明这种属性的内存在使用时只能是 DRAM。

当申请的指定属性的内存不足时，执行 ``wm_heap_caps_alloc`` 会返回 ``NULL`` 。


要获取所有 RAM 堆的剩余空间大小，请调用：
::
   
    wm_heap_get_free_size()

调用 malloc() 时，WM IoT SDK 内部调用 ``wm_heap_caps_alloc`` 函数，使用属性 ``WM_HEAP_CAP_DEFAULT`` 分配内存。
内存重新分配调用 ``wm_heap_caps_realloc`` 函数。

分配的内存可以通过调用 ``wm_heap_caps_free`` 函数释放。

显示堆内存统计信息可以调用 ``wm_heap_print_stats`` 函数。
调试时，显示堆内存分配信息调用 ``wm_heap_print_tracing`` 函数。

当前堆内存分配算法采用 `FreeRTOS heap4 <https://www.freertos.org/Documentation/02-Kernel/02-Kernel-features/09-Memory-management/01-Memory-management#heap_4c>`_ 方案，所有从堆内存分配的内存，都为 ``8 字节对齐``。释放内存时会自动合并相邻空闲内存，具备一定的内存碎片整理能力。

RAM 说明
~~~~~~~~~~~~~

W800 的内存分成三块：160Kbyte SRAM、 128Kbyte DRAM 和 最大 8Mbyte 的 PSRAM：

.. table::
    :widths: 25 25 25 25 25 25
    :align: center

    +--------------------+-----------------------+---------------------+---------------------+----------+-------------------------------------------------------------------------------------+
    |       内存块       |          功能         |       起始地址      |        终止地址     |    大小  |    属性                                                                             |
    +====================+=======================+=====================+=====================+==========+=====================================================================================+
    |      160 Kbyte     |          SRAM         |      0x20000000     |      0x20027FFF     |  160Kbyte|  WM_HEAP_CAP_DEFAULT | WM_HEAP_CAP_INTERNAL | WM_HEAP_CAP_EXEC                      |
    +--------------------+-----------------------+---------------------+---------------------+----------+-------------------------------------------------------------------------------------+
    |      128 Kbyte     |          DRAM         |      0x20028000     |      0x20047FFF     |  128Kbyte|  WM_HEAP_CAP_DEFAULT | WM_HEAP_CAP_INTERNAL | WM_HEAP_CAP_SHARED                    |
    +--------------------+-----------------------+---------------------+---------------------+----------+-------------------------------------------------------------------------------------+
    |      <=8 Mbyte     |         PSRAM         |      0x30000000     |      0x307FFFFF     |    8Mbyte|  WM_HEAP_CAP_SPIRAM                                                                 |
    +--------------------+-----------------------+---------------------+---------------------+----------+-------------------------------------------------------------------------------------+

.. tip::
   - 只申请 160KB 的 SRAM，则调用 ``wm_heap_caps_alloc(size, WM_HEAP_CAP_EXEC)``
   - 只申请 128KB 的 DRAM，则调用 ``wm_heap_caps_alloc(size, WM_HEAP_CAP_SHARED)``
   - 只申请 PSRAM，则调用 ``wm_heap_caps_alloc(size, WM_HEAP_CAP_SPIRAM)``
   - 只申请片上内存，则调用 ``wm_heap_caps_alloc(size, WM_HEAP_CAP_INTERNAL)``
   - 如果不关心从哪申请内存，则调用 ``wm_heap_caps_alloc(size, WM_HEAP_CAP_DEFAULT)``，这和使用 ``malloc`` 是一样的效果
   - 若内存不足时，调用上述所有分配接口将返回 NULL 表示分配失败
   - 当前 SDK 提供的 ``HEAP`` 管理与 FreeRTOS 提供的 ``heap_4`` 类似，具备内存碎片回收机制，但若频繁分配和释放不同大小的内存块时，仍有可能出现内存碎片
   - 在 W80X 平台上 pSRAM 的内存属性不能声明成 ``WM_HEAP_CAP_DEFAULT`` ， 原因是 CPU 与 DMA 同时访问会造成总线冲突，进而引起系统宕机。pSRAM 具体用法可参考 :ref:`pSRAM<drv_psram>`


堆内存的 meunconfig 配置
---------------------------------

主要配置如下：

.. list-table::
   :widths: 45 50 25 
   :header-rows: 0
   :align: center

   * - 配置名称
     - 配置描述
     - 默认值

   * - CONFIG_HEAP_USE_ASSERT
     - 是否启用堆断言检查
     - Y 

   * - CONFIG_HEAP_USE_TRACING
     - 是否启用堆分配记录调试功能
     - Y


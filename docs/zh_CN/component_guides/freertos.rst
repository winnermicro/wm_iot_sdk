.. _freertos:

FreeRTOS
===============

概述
---------------

FreeRTOS 是一个开源的 RTOS（实时操作系统），它以组件的形式集成到 WM IoT SDK 中，当前 WM IoT SDK 中所有的程序都是基于该系统运行的。

配置
---------------

内核配置
^^^^^^^^^^^^^^^^^

原生 FreeRTOS 要求移植工具和应用程序通过在 **FreeRTOSConfig.h** 头文件中添加各种 ``#define config...`` 宏定义来配置内核。
原生 FreeRTOS 支持一系列内核配置选项，允许启用或禁用各种内核的功能。

- 常用的内核配置选项：

  + CONFIG_FREERTOS_HZ：FreeRTOS 的时钟 TICK 频率。

  + CONFIG_FREERTOS_TIMER_TASK_PRIORITY：FreeRTOS 的定时器任务优先级。

  + CONFIG_FREERTOS_TIMER_TASK_STACK_DEPTH：FreeRTOS 的定时器任务栈大小。


使用 FreeRTOS
---------------

应用程序入口点
^^^^^^^^^^^^^^^^^^

在 WM IoT SDK 中使用 FreeRTOS 时用户无需考虑在哪调用 ``vTaskStartScheduler()`` 和 ``vTaskEndScheduler()``，
WM IoT SDK 会自动在系统初始化流程中调用它们启动 FreeRTOS，
但用户需要定义一个 ``main`` 函数作为用户应用程序的入口点，``main`` 函数在 WM IoT SDK 启动时被自动调用。

- 一般而言，用户程序都是从 ``main`` 函数开始编写，当前提供的示例也都如此设计。

- main 函数可以在任何时候返回，也可以创建和启动其它任务，使用时不存在任何限制。

后台任务
^^^^^^^^^^^^^^^^^^

在启动过程中，WM IoT SDK 和 FreeRTOS 会自动创建多个在后台运行的任务，必定会创建的任务下表所示。

.. list-table::
   :widths: 25 45 25 25 
   :header-rows: 0
   :align: center

   * - 任务名称
     - 描述
     - 任务栈大小
     - 优先级

   * - FreeRTOS 空闲任务 (IDLE)
     - FreeRTOS 自动创建的空闲任务。
     - CONFIG_FREERTOS_IDLE_TASK_STACKSIZE，默认 256
     - 0

   * - FreeRTOS 定时器任务 (Tmr Svc)
     - FreeRTOS 自动创建的定时器服务任务
     - CONFIG_FREERTOS_TIMER_TASK_STACK_DEPTH，默认 512
     - CONFIG_FREERTOS_TIMER_TASK_PRIORITY，默认 15

   * - 系统启动初始化任务 (main)
     - WM IoT SDK 创建系统启动初始化任务，在 main 返回时会自我删除
     - 4096
     - 1

.. attention::
   - 如果应用程序使用了其他 WM IoT SDK 功能（如 Wi-Fi 或蓝牙），那么这些功能可能会在上表的任务之外创建自己的任务。
   - WM IoT SDK 组件中创建的任务可以在 ``components\wm_system\include\wm_task_config.h`` 中查看其定义的任务优先级和任务栈大小，不建议用户修改和直接使用该文件里的配置。

附加功能
---------------

WM IoT SDK 还为 FreeRTOS 提供了一些补充功能，如 TICKLESS 和 IDLE 钩子。

FreeRTOS 堆
---------------

WM IoT SDK 并没有直接使用 FreeRTOS 自带的堆实现，而是根据 heap4 的机制实现了自己的堆内存分配算法。
WM IoT SDK 中将 FreeRTOS 内存分配或释放函数（如 ``pvPortMalloc()`` 和 ``pvPortFree()``）映射到 WM IoT SDK 堆 API（即 ``wm_heap_caps_alloc()`` 和 ``wm_heap_caps_free()``）。

WM IoT SDK 中 FreeRTOS 所使用的内存为片上高速内存，不支持片外内存，可参考 :ref:`堆内存管理<heap>`
.. _freertos:

FreeRTOS
===============

Overview
---------------

FreeRTOS is an open source RTOS (Real-Time Operating System) that is integrated into the WM IoT SDK in the form of components, and on which all programs in the current WM IoT SDK run.

Configuration
---------------

Kernel Configuration
^^^^^^^^^^^^^^^^^^^^^^

Native FreeRTOS requires porting tools and applications by adding various ``#define config...`` to **FreeRTOSConfig.h** headers. Macro definition to configure the kernel
Native FreeRTOS supports a range of kernel configuration options that allow functionalities of various kernels to be enabled or disabled.

- Commonly used kernel configuration options:

  + CONFIG_FREERTOS_HZ: FreeRTOS clock TICK frequency.

  + CONFIG_FREERTOS_TIMER_TASK_PRIORITY: Timer task priority of FreeRTOS.

  + CONFIG_FREERTOS_TIMER_TASK_STACK_DEPTH: Timer task stack size of FreeRTOS.


Using FreeRTOS
---------------

Application entry point
^^^^^^^^^^^^^^^^^^^^^^^^^

When using FreeRTOS in WM IoT SDK, users don't need to consider where to call ``vTaskStartScheduler()`` and ``vTaskEndScheduler()``,
The WM IoT SDK automatically calls them in the system initialisation flow to start FreeRTOS.
However, the user needs to define a ``main`` function as the entry point of the user application, and the ``main`` function is automatically called when the WM IoT SDK starts.

- Generally, user applications are written from the ``main`` function, and the examples provided so far are designed as such.

- The main function can be returned to at any time, and other tasks can be created and started without any restrictions on its use.

Background Tasks
^^^^^^^^^^^^^^^^^^

During startup, the WM IoT SDK and FreeRTOS automatically create several tasks that run in the background, the tasks that are bound to be created are shown in the table below.

.. list-table::
   :widths: 25 45 25 25
   :header-rows: 0
   :align: center

   * - Task name
     - Description
     - Task Stack Size
     - Priority

   * - FreeRTOS Idle Tasks (IDLE)
     - Idle task created automatically by FreeRTOS.
     - CONFIG_FREERTOS_IDLE_TASK_STACKSIZE, default 256
     - 0

   * - FreeRTOS Timer Task (Tmr Svc)
     - Timer service task created automatically by FreeRTOS
     - CONFIG_FREERTOS_TIMER_TASK_STACK_DEPTH, default 512
     - CONFIG_FREERTOS_TIMER_TASK_PRIORITY, default 15

   * - System boot initialisation task (main)
     - The WM IoT SDK creates a system startup initialisation task, which deletes itself when main returns.
     - 4096
     - 1

.. attention:: 
   - If the application uses other WM IoT SDK features, such as Wi-Fi or Bluetooth, then these features may create their own tasks in addition to those listed above.
   - The tasks created in the WM IoT SDK component can be viewed in the ``components\wm_system\include\wm_task_config.h`` its defined task priority and task stack size. It is not recommended that users modify and directly use the configuration in this file.

Additional Features
---------------------

The WM IoT SDK also provides some additional features for FreeRTOS, such as TICKLESS and IDLE hooks.

FreeRTOS Stack
---------------

The WM IoT SDK does not directly use the FreeRTOS heap implementation, but implements its own heap memory allocation algorithm based on the heap4 mechanism.
he WM IoT SDK maps FreeRTOS memory allocation or release functions (such as ``pvPortMalloc()`` and ``pvPortFree()`` )to the WM IoT SDK heap API (i.e. ``wm_heap_caps_alloc()`` and ``wm_heap_caps_free()``).

The memory used by FreeRTOS in the WM IoT SDK is high-speed memory on chip and does not support off-chip memory, see :ref:`heap memory management <heap>`

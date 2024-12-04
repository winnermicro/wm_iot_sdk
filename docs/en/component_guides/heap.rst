.. _heap:

Heap Memory Management
=========================

Overview
------------

The heap is an area for dynamic memory allocation, used to store memory space allocated dynamically during program execution.

Heap Memory
------------

Difference Between Stack and Heap
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

WM IoT SDK applications utilize the common computer architecture paradigm: Memory dynamically allocated by the program control flow (i.e., the **stack**),  Memory dynamically allocated by function calls (i.e., the **heap**), and **static memory** allocated at during compilation.

Since the WM IoT SDK operates in a multithreaded RTOS environment, each RTOS task has its own stack, which is allocated from the heap by default when the task is created.

.. only:: w800

The W800  incorporates multiple types of RAM:

- SRAM
- DRAM
- PSRAM

A memory allocator based on attributes allows applications to allocate heap memory for different purposes.

In most cases, heap allocation can be achieved directly using the standard C library functions ``malloc()``  and ``free()`` . 
To make full use of the various types of memory and their characteristics, the WM IoT SDK also includes an attribute-based heap memory allocator.
To allocate memory with specific attributes, use:
::

    wm_heap_caps_alloc()

Memory Attributes
^^^^^^^^^^^^^^^^^^^^

When programming, the following attributes are provided:

-WM_HEAP_CAP_DEFAULT, the default attribute, indicating that the memory of this type does not care about its source when in use and may be one of SRAM, DRAM, or PSRAM.

-WM_HEAP_CAP_INTERNAL, on-chip memory, indicating that the memory of this type can only be on-chip memory when in use and may be one of SRAM and DRAM.

-WM_HEAP_CAP_SPIRAM, off-chip memory, indicating that the memory of this type can only be PSRAM when in use.

-WM_HEAP_CAP_EXEC, executable memory, indicating that the memory of this type can only be SRAM when in use.

-WM_HEAP_CAP_SHARED, memory that can be shared with high-speed peripherals, indicating that the memory of this type can only be DRAM when in use.

When the requested memory with specific attributes is insufficient,  ``wm_heap_caps_alloc`` will return ``NULL`` 

To get the remaining space size of all RAM heaps, call:
::
   
    wm_heap_get_free_size()

When calling malloc(), the WM IoT SDK internally calls the ``wm_heap_caps_alloc`` function, using the attribute ``WM_HEAP_CAP_DEFAULT`` to allocate memory.
Memory reallocation calls the ``wm_heap_caps_realloc`` function.

Allocated memory can be freed by calling the ``wm_heap_caps_free``  function. 

To display heap memory statistics, call the ``wm_heap_print_stats`` function.For debugging, display heap memory allocation information by calling the ``wm_heap_print_tracing`` function.

The current heap memory allocation algorithm adopts the  `FreeRTOS heap4 <https://www.freertos.org/Documentation/02-Kernel/02-Kernel-features/09-Memory-management/01-Memory-management#heap_4c>`_  scheme, where all memory allocated from the heap is ``8-byte aligned``. When memory is freed, adjacent free blocks are automatically merged, providing a certain level of memory fragmentation management capability.

RAM  Description
~~~~~~~~~~~~~~~~~~~

.. only:: w800

    The W800's memory is divided into three blocks: 160 Kbyte SRAM, 128 Kbyte DRAM, and up to 8M PSRAM:

    +--------------------+-----------------------+---------------------+---------------------+----------+-------------------------------------------------------------------------------------+
    |       Memory Block |         Function      |  Starting Address   |  Ending Address     |    Size  |    Attributes                                                                       |
    +====================+=======================+=====================+=====================+==========+=====================================================================================+
    |      160 Kbyte     |          SRAM         |      0x20000000     |      0x20027FFF     |  160Kbyte|  WM_HEAP_CAP_DEFAULT | WM_HEAP_CAP_INTERNAL | WM_HEAP_CAP_EXEC                      |
    +--------------------+-----------------------+---------------------+---------------------+----------+-------------------------------------------------------------------------------------+
    |      128 Kbyte     |          DRAM         |      0x20028000     |      0x20047FFF     |  128Kbyte|  WM_HEAP_CAP_DEFAULT | WM_HEAP_CAP_INTERNAL | WM_HEAP_CAP_SHARED                    |
    +--------------------+-----------------------+---------------------+---------------------+----------+-------------------------------------------------------------------------------------+
    |      <=8 Mbyte     |          PSRAM        |      0x30000000     |      0x307FFFFF     |    8Mbyte|  WM_HEAP_CAP_SPIRAM                                                                 |
    +--------------------+-----------------------+---------------------+---------------------+----------+-------------------------------------------------------------------------------------+
	
.. tip::
   - To allocate only 160KB of SRAM, call ``wm_heap_caps_alloc(size, WM_HEAP_CAP_EXEC)``.
   - To allocate only 128KB of DRAM, call ``wm_heap_caps_alloc(size, WM_HEAP_CAP_SHARED)``.
   - To allocate only PSRAM, call ``wm_heap_caps_alloc(size, WM_HEAP_CAP_SPIRAM)``.
   - To allocate only on-chip memory, call ``wm_heap_caps_alloc(size, WM_HEAP_CAP_INTERNAL)``.
   - If you do not care where the memory is allocated from, call ``wm_heap_caps_alloc(size, WM_HEAP_CAP_DEFAULT)``, which has the same effect as using malloc.
   - When memory is insufficient, calling any of the above allocation interfaces will return NULL to indicate allocation failure.
   - The HEAP management provided by the ``WM IOT SDK`` is similar to ``heap_4`` provided by FreeRTOS, equipped with a memory fragmentation recovery mechanism. However, when memory blocks of different sizes are frequently allocated and released, memory fragmentation may still occur.
   - On the W80X platform, the memory attribute of pSRAM cannot be declared as ``WM_HEAP_CAP_DEFAULT``. The reason is that simultaneous access by the CPU and DMA will cause a bus conflict, which will further lead to a system crash. For the specific usage of pSRAM, please refer to :ref:`pSRAM<drv_psram>`.
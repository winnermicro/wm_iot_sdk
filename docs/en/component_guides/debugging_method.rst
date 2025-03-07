
.. |Equipment-Name| replace:: W800
.. _debug:

Debugging Methods
====================

This document will guide you through the debugging methods for |Equipment-Name| :

1.  :ref:`Adding Debugging Information <log>` 
2.  :ref:`Using a Debugger for Online Debugging <cklink>` 
3.  :ref:`Analyzing the Scene After an Exception Occurs <ramdump>` 
4.  :ref:`Obtaining the Reason for Reboot <bootreason>`

.. _log:

:ref:`Adding Debugging Information <debug>` 
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Using wm_log_xx to Print Log Information:**
------------------------------------------------------------------

The ``wm_log_xx`` function is declared in the ``wm_log.h`` header file and provides 6 log levels for users to choose from based on their needs: ``error`` ,
``warn``, ``info``, ``debug``, ``verbose``. It also offers assertion interfaces, hexadecimal dump interfaces, and raw information output interfaces. Additionally, it supports displaying information in different colors according to different levels (requires serial port tools with color support).

The usage of ``wm_log_xx`` is similar to the C library function ``printf`` .Not only does it support most of the functions of ``printf`` , but when using the ``wm_log_xx`` series of interfaces, besides the raw information output port, a newline character is automatically appended at the end, reducing the developers' workload of repeatedly inputting it.

There is a restriction for  ``wm_log_xx`` : the first parameter must be a constant string or a macro that can be replaced with a constant string. For example, the following is incorrect:
  
.. code:: c

  const char* msg = "123";
  wm_log_info(msg);

It should be used like this:

.. code:: c

  const char* msg = "123";
  wm_log_info("%s",msg);

During use, it can be configured through menuconfig, by selecting the log level through  ``Components configuration`` ---> ``log`` --->  ``default log level``, or it can be dynamically controlled within the program through APIs.

In the CLI, when viewing log information, you can also set the display level through commands, such as setting the information level to debug:

::

    log debug
	
Additionally, for users who need asynchronous printing, wm_log also supports printing in task mode. You need to enable log task printing through menuconfig and then adjust the appropriate log task priority and log buffer queue size. 
However, note that excessively high log task priority may negate the purpose of asynchronous printing, and an excessively small buffer queue may result in some print information being lost.	
	
By default, print information is output using uart0. If users want to make a selection, taking setting the output serial port to uart1 as an example, the operation is as follows:

.. only:: w800

    Enter ``menuconfig``，select ``Components configuration`` and enter ``Log`` component for configuration to select the corresponding serial port or even choose not to print.


.. _cklink:

	
:ref:`Using a Debugger for Online Debugging <debug>` 
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Online debugging requires the use of a cklink debugger. Download and install the debug server software. For detailed usage of cklink and the debug server, please refer to the `T-Head Debugger Server User Guide <../../../../download/debug_server/T-Head_Debugger_Server_User_Guide_EN-US.pdf>`_.

.. important::

    For debugging code running in XIP mode, cklink Debugger only supports up to **2** hardware breakpoints.
    For debugging convenience, Debugger Server will replace the breakpoints after more than 2 breakpoints by erasing the original code in the flash and inserting new code to replace it.
    To make debugging easier, Debugger Server will realize more breakpoints by erasing the original code in flash and inserting new code to replace it, and it will restore the code after debugging under normal circumstances.
    Therefore, when using cklink to debug the code running in XIP mode, when the number of breakpoints exceeds 2, there is a risk of losing the original data on the flash.

.. only:: w800

The wiring method for using the cklink debugger with w800 is shown in the following table:

   .. list-table:: 
      :widths: 25 25 25
      :header-rows: 0
      :align: center

      * - w800
        - cklink
        - Description

      * - PIN_NUM_1(PA1)s
        - TCK
        - Must be connected

      * - PIN_NUM_4(PA4)
        - TMS
        - Must be connected

      * - GND
        - GND
        - Must be connected

      * - RST(Reset pin)
        - TRST
        - Optional (Recommended for stable hardware reset)

      * - 3V3
        - 3V3
        - Optional


GDB is already included in the build toolchain and does not need to be installed separately.

**GDB Command Line:**
--------------------------------

After connecting the  |Equipment-Name| to the cklink debugger, start by running the debug server.Then,

use the following command in the compile command line:

::

    wm.py gdb

This will start GDB and enter debugging mode. For more GDB operations, please refer to `T-HEAD CPU Debugging Tips <../../../../download/debug_server/T-HEAD_CPU_Debugging_Tips_CN.pdf>`_.

Additionally, the WM IoT SDK provides 4 debugging commands, which have the following functions:

- ``wmbt``   : View the status of all tasks in FreeRTOS.
- ``wmst``   : Switch the CPU context to the call stack of the task displayed by ``wmbt`` based on the task index.
- ``wmbk``   : Restore the CPU context to the task call stack at the first entry to the debugger.
- ``wmheap`` : View the current heap memory allocation status.

.. _another-debug:

**IDE Debugging Guide ：**
--------------------------------------
.. toctree::
   :maxdepth: 1

   ide_debug

.. _ramdump:

:ref:`Analyzing the Scene After an Exception <debug>` 
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The WM IoT SDK provides a method to analyze the scene after an exception without requiring a debugger.

**Checking the Cause of a Crash and the Current Address (Following the Exception Prompt):**
-----------------------------------------------------------------------------------------------

When a crash or exception occurs, The |Equipment-Name| will print the error information, the current address, and the exception name.

Taking a divide-by-zero exception during program execution as an example, |Equipment-Name| will print the following exception information:

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



Next, simply run the command prompted in the exception information in the compilation command line:

::

    csky-elfabiv2-addr2line -p -f -e ./build/hello_world.elf -a 0x0800cc12
    0x0800cc0e 

At this point, you can see where the error occurred:

::

    0x0800cc12: test_task at winnermicro-iot-os/examples/hello_world/main/src/main.c:9
    0x0800cc0e: test_task at winnermicro-iot-os/examples/hello_world/main/src/main.c:9


This method can be used for simple problems. For more complex issues, it is recommended to use the following method.

**Ramdump Analysis:**
-------------------------------------------------------------

Ramdump analysis mainly includes two steps:

1. Use ``wm.py ramdump –p port`` to export the scene.
   
When an error or exception occurs, do not reset the system. Enter the following command in the compile command line:

::

    wm.py ramdump -p port

Where ``port`` is the port number. If there is only one serial port, this parameter can be omitted. Once completed, you can use the following command.。

2. Use ``wm.py debug`` to analyze the exported RAM dump

After exporting the ramdump,  input ``wm.py debug`` to enter debug mode. The subsequent debugging method is the same as online debugging through the GDB command line, so it is not introduced here.

Taking double-free memory as an example, |Equipment-Name| will immediately print the following exception information:

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

At this point, we use ``wm.py ramdump -p port`` to export the exception site, then use ``wwm.py debug`` to enter gdb debugging, and use the ``bt`` command. You will see the following information:

::

    (cskygdb) bt
    #0  0x0801a7d4 in heap_free (pv=0x200060b0) at winnermicro-iot-os/components/heap/src/wm_heap.c:245
    #1  0x0801ab74 in wm_heap_caps_free (p=0x200060b0) at winnermicro-iot-os/components/heap/src/wm_heap.c:513
    #2  0x08018456 in free (ptr=0x200060b0) at winnermicro-iot-os/components/wm_hal/src/wm_hal_mem.c:43
    #3  0x0800cc32 in test_task (parameters=0x0) at winnermicro-iot-os/examples/hello_world/main/src/main.c:11
    #4  0xfffffffe in ?? ()

It is clear that in the ``test_task`` function in ``main.c`` , an exception was triggered by calling the ``free`` function at line 11. 

.. hint::

    * The current ramdump analysis function does not support viewing data in peripheral registers.

.. _bootreason:

:ref:`Obtaining the Reason for Reboot <debug>`
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

When the system reboots, if you want to obtain the reboot reason, you can call the reboot reason APIs provided by the SDK. The API includes both setting and getting operations. The system has already called ``wm_set_reboot_reason()``  in some scenarios to set the reboot reason, such as software reboot, system exception, and watchdog timeout.


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

Users can call ``wm_get_reboot_reason()`` during the initial stage of the program, for instance, when the user's Application Task is started, to obtain and print the reboot reason.
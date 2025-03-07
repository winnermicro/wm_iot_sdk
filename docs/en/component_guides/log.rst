
.. _wm_log:

Log
=============

Introduction
-------------

    Log is primarily used for debugging information output during the development process.


Feature List
------------
    1. :ref:`Log Level <log_level>`: Outputs LOG information at different levels.
    2. :ref:`Log Output <log_output>`: LOG Output.
    3. :ref:`printf <printf>`: Raw printf output.
    4. :ref:`Dump <dump>`: Hexadecimal dump output.
    5. :ref:`Color settings <color>`: Log color settings.
    6. :ref:`Direct <direct>`: Direct output.
    7. :ref:`Serial port selection <port>`: Serial port selection.


Log Function Overview
---------------------

    LOG includes two parts: one is the printf, fprintf, fputs, puts, and putchar functions defined in the standard C header stdio.h, this section is overwrite in libc_port.c and connected to the serial port.

    The other part is the more powerful wm_log module, with the code located in components/wm_log, which provides additional features such as log levels, colors, timestamps, and more.


Main function
-------------

.. _log_level:

Log level
^^^^^^^^^^^

    Log has a total of 6 levels, ``none`` 、 ``error`` 、 ``warn`` 、 ``info`` 、 ``debug`` 、 ``verbose``, its definition is as follows:

    .. code:: c

        typedef enum {
            WM_LOG_LEVEL_NONE,    /**< No log output        */
            WM_LOG_LEVEL_ERROR,   /**< Critical errors      */
            WM_LOG_LEVEL_WARN,    /**< warnings             */
            WM_LOG_LEVEL_INFO,    /**< Information messages */
            WM_LOG_LEVEL_DEBUG,   /**< Debug informations   */
            WM_LOG_LEVEL_VERBOSE, /**< Extra information which is not necessary */
        } wm_log_level_t;



    The default log level in the module is INFO, and only the macros corresponding to error, warn, and info can output log information. The macros for debug and verbose cannot output logs. To output debug or verbose information, you need to configure the log level.

    The CONFIG_LOG_DEFAULT_LEVEL macro is configured before compilation and can be set in menuconfig. It can also be configured at runtime by calling the wm_log_set_level interface.

.. _log_output:

Log Output
^^^^^^^^^^^^

    **1. Formatted Output**

    Formatted output for LOG generally uses the following macros:

    wm_log_error

    wm_log_warn

    wm_log_info

    wm_log_debug

    wm_log_verbose

    Its parameters are the same as those of printf, with levels ranging from 1 to 5. When the level is set to 1, only the macro wm_log_error corresponding to level 1 will output the LOG information.
    When the level is set to 0, no Log will be output. When the level is set to 5, all LOGs will be output.

    **2. Raw output**

    The following macro outputs raw information, similar to the output of the printf function, without timestamps or other additional information.

    wm_log_raw

    **3. Format**

    The following is the format of the LOG output: the first part in square brackets is the level, the second part in parentheses is the timestamp, in milliseconds since boot, and the third column is the LOG TAG, 
    used to distinguish information from different modules.

    .. code::

        [E] (5017810) at_log: test wm_log_error
        [W] (5017810) at_log: test wm_log_warn
        [I] (5017810) at_log: test wm_log_info
        [D] (5017810) at_log: test wm_log_debug
        [W] (5017810) at_log: test wm_log_verbose


    **3. LOG_TAG**

    In wm_log.h, the default LOG_TAG is defined as 'NO_TAG', as shown below:

        .. code::c

            #if !defined(LOG_TAG)
                #define LOG_TAG "NO_TAG"
            #endif

    In your module's .c file, you can define LOG_TAG as a different string before including wm_log.h. For example:

        .. code::c

            #define LOG_TAG "my_module"
            #include "wm_log.h"

    .. warning::
        Do not define LOG_TAG in external header files, as this can lead to redefinition.


.. _printf:

printf
^^^^^^

    The printf function can also be used, but it lacks control over levels, timestamps, and cannot be disabled through configuration, so it is not recommended.



.. _dump:

Dump
^^^^

    Dump output calls wm_log_dump, as shown below:


    .. code::c

        wm_log_dump(WM_LOG_LEVEL_INFO, "test", 16, buf, sizeof(buf));



.. _color:

Color
^^^^^

    The LOG module does not have the color feature enabled by default. It can be enabled in menuconfig, with the configuration option CONFIG_LOG_USE_COLOR.
    After enabling it, each LOG will be prefixed and suffixed with color codes. Different LOG levels will have different colors, making the display more visually appealing on serial tools that support color.


.. _Direct:

Direct output mode
^^^^^^^^^^^^^^^^^^

    Currently, the LOG is output by calling the serial driver at the lower level. Before the serial driver is initialized at the driver layer, the LOG information is directly written to the hardware FIFO of the serial port for output,
    After the serial driver is initialized at the driver layer, the formatted LOG information is output to the TX buffer of the serial driver. 
    Later, the driver will send the data from the buffer to the hardware FIFO during interrupt handling.

    The Direct output mode forces the data to be sent directly to the serial hardware FIFO, bypassing the serial driver's software buffer.

    The Direct output mode can be configured in menuconfig, with the configuration option being CONFIG_LOG_WRITE_DIRECT.


.. _port:

Serial port
^^^^^^^^^^^

    The serial port can be selected in menuconfig, with serial port 0 being the default. It can also be configured directly in the prj.config file. To configure it to serial port 1, you can set it as follows:

    CONFIG_LOG_UART_PRINT_USE_UART1=y




Application example
-------------------

    The reference example is as follows:

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



    Execution result:

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



Usage restrictions
------------------

    ``wm_log_xx`` has a limitation: the first argument must be a constant string or a macro that can be replaced with a constant string. For example, the following is incorrect:

    .. code:: c

        const char* msg = "123";
        wm_log_info(msg);

    It should be used as follows:

    .. code:: c

        const char* msg = "123";
        wm_log_info("%s",msg);


    .. warning::

        LOG, AT, and CLI all use UART for output, and the UART ports they use can be configured in menuconfig.
        Note that when making adjustments, AT and CLI cannot use the same UART port.  LOG can use the same port as AT, or the same port as CLI.
        In the default configuration, LOG and CLI both use UART0, while AT uses UART1.

    .. warning::

        The wm_printf_direct function directly outputs the LOG to the UART hardware FIFO in a blocking mode, and like printf, it is not restricted by the log level.
        It is recommended to use this only in scenarios where interrupts are disabled or the operating system is unable to function properly.

    .. note::

        In floating point number format, %f and %lf are supported, but %e, %E, %g, %G are not. In addition, integer formatting does not support %zu, %zi, %zd.


LOG meunconfig configuration
------------------------------

The main configurations are as follows:

.. list-table::
  :widths: 45 50 25 
  :header-rows: 0
  :align: center

  * - Configuration Name
    - Config Discriptors
    - Default Values

  * - CONFIG_LOG_PRINT_UART_MODE 
    - Configuring output ports
    - Uart0

  * - CONFIG_LOG_DEFAULT_LEVEL
    - Configuration output level
    - INFO level

  * - CONFIG_LOG_USE_COLOR
    - Configure the output LOG to support colors
    - N


API Reference
-------------

    For LOG-related API, please refer to:

    :ref:`label-api-log`
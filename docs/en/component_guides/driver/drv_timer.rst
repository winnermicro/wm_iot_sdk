
.. _timer:

TIMER
=============

Introduction
---------------
The Timer is a hardware module used for precise time control and event counting. In embedded systems, timers can be used to generate precise time delays, count events, generate timed interrupts, and other functions. They are essential tools for implementing real-time operating system clocks, PWM (Pulse Width Modulation) signal generation, pulse counting, and event timing.

The W800 chip supports multiple timers, each of which can independently configure parameters such as count period and auto-reload. The flexibility and high precision of the timer module make it widely used in various embedded applications.

Function List
---------------

1. :ref:`Initialization <timer_init>` — Initializes the timer driver.
2. :ref:`Set Count Period <timer_set_period>` — Sets the timer's count period.
3. :ref:`Set Auto-Reload <timer_set_auto_reload>` — Configures the timer's auto-reload mode.
4. :ref:`Start Timer <timer_start>` — Starts the timer.
5. :ref:`Stop Timer <timer_stop>` — Stops the timer.
6. :ref:`Read Counter Value <timer_get_counter>` — Reads the current count value of the timer.
7. :ref:`Register Interrupt Callback <timer_register_callback>` — Registers a timer interrupt callback function.

Function Overview
--------------------------

**Setting and Getting Count Values**: Forces the timer to start counting from the beginning and how to obtain the counter value at any time.

**Multi-Channel Support**: The W800 supports up to 6 independent timer channels.

**Flexible Parameter Configuration**: The Timer supports configuring parameters such as clock source, countting period, and auto-reload.

**High Precision**: The Timer provides high-precision time control capabilities, suitable for various precise timing applications.

**Interrupts and Callbacks**: The timer supports interrupts and callback functions, which can trigger interrupts when the timer overflows or reaches a specific count value.

Main Functions
------------------

.. _timer_init:

Initializing the Timer
^^^^^^^^^^^^^^^^^^^^^^^^^
Before using the timer, you need to call the ``wm_drv_timer_init()`` function to allocate resources for the timer, using the ``wm_device_t`` structure to receive the timer device identifier. Example:

.. code:: c

    wm_device_t *timer_dev;
    timer_dev = wm_drv_timer_init("timer0");

The first parameter specifies the device name, which is defined in the device table, ranging from "timer0" to "timer5".

.. warning:: After initializing the timer, if ``wm_drv_timer_deinit()`` is not called, calling ``wm_drv_timer_init()`` again will return ``NULL``.

.. _timer_set_period:

Setting the Counting Period
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Call the ``wm_drv_timer_set_period()`` function to set the counting period of the timer, in microseconds. Example:

.. code:: c

    wm_device_t *timer_dev;
    timer_dev = wm_drv_timer_init("timer0");

    wm_drv_timer_set_period(timer_dev, 1000); 

The first parameter is a pointer to the timer device, of type ``wm_device_t*``.

The second parameter is an unsigned 32-bit integer representing the counting period of the timer, in microseconds. In this example, 1000 means the timer's count period is 1000 microseconds (i.e., 1 millisecond).

.. _timer_set_auto_reload:

Setting Automatic Reload
^^^^^^^^^^^^^^^^^^^^^^^^^^

Call the function ``wm_drv_timer_set_auto_reload()`` to configure the automatic reload mode of the timer. Example:

.. code:: c

    wm_device_t *timer_dev;
    timer_dev = wm_drv_timer_init("timer0");

    wm_drv_timer_set_auto_reload(timer_dev, true); 

The first parameter is a pointer to the timer device, of type ``wm_device_t*``.

The second parameter is the auto-reload option, of type ``bool``. Setting it to true enables the auto-reload function, and setting it to false disables the auto-reload function.

.. _timer_start:

Starting the Timer
^^^^^^^^^^^^^^^^^^^^

Call the function ``wm_drv_timer_start()`` to start the timer. Example:

.. code:: c

    wm_device_t *timer_dev;
    timer_dev = wm_drv_timer_init("timer0");

    wm_drv_timer_cfg_t timer_cfg = {
    .unit = WM_HAL_TIMER_UNIT_US,
    .period = 1000,
    .auto_reload = true
        };
    wm_drv_timer_start(timer_dev, timer_cfg);

The first parameter is a pointer to the timer device, of type ``wm_device_t*``.

The second parameter is used to configure the relevant parameters of the timer, of type ``wm_drv_timer_cfg_t``. Among them, ``unit`` specifies the timer's unit, of type wm_hal_timer_unit_t. It is set to WM_HAL_TIMER_UNIT_US, indicating that the timer's unit is microseconds (us). ``period`` specifies the timer's period, of type uint32_t. Here, it is set to 1000, indicating a period of 1000 microseconds (1 millisecond). ``auto_reload`` sets whether the timer auto-reloads, of type bool. Here, it is set to true, indicating that the timer will automatically reload and start counting again when the period ends.

.. _timer_stop:

Stopping the Timer
^^^^^^^^^^^^^^^^^^^^

Call the function ``wm_drv_timer_stop()`` to stop the timer. Example:

.. code:: c

    wm_device_t *timer_dev;
    timer_dev = wm_drv_timer_init("timer0");

    wm_drv_timer_stop(timer_dev);

The parameter is a pointer to the timer device, of type ``wm_device_t*``.

.. _timer_get_counter:

Reading Counter Value
^^^^^^^^^^^^^^^^^^^^^^^

Call the function ``wm_drv_timer_get_counter()`` to read the current count value of the timer. Example:

.. code:: c

    wm_device_t *timer_dev;
    timer_dev = wm_drv_timer_init("timer0");

    uint32_t counter;
    wm_drv_timer_get_counter(timer_dev, &counter);

The first parameter is a pointer to the timer device, of type ``wm_device_t*``.

The second parameter is a pointer to a ``uint32_t`` type, used to store the current count value of the timer.

.. _timer_register_callback:

Registering  an Interrupt Callback
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Call the function ``wm_drv_timer_register_callback()`` to register a callback function for timer interrupts. Example:

.. code:: c

    wm_device_t *timer_dev;
    timer_dev = wm_drv_timer_init("timer0");

    void timer_callback(void *arg) {
        printf("Timer interrupt triggered!\n");
    }

    wm_drv_timer_register_callback(timer_dev, timer_callback, NULL);

The first parameter is a pointer to the timer device, of type ``wm_device_t*``.

The second parameter is the callback function, of type ``wm_drv_timer_callback_t``. This function is called when the timer interrupt is triggered.

The third parameter is private data passed to the callback function, of type ``void *``.

Application Example
---------------------------
For basic examples of using timers, refer to: examples/peripheral/timer

API Reference
-------------------
For related TIMER APIs, refer to:

:ref:`label_api_timer`
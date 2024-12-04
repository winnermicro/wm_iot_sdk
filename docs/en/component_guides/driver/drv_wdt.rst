
.. _wdt:

WDT
=============

Introduction
----------------
WDT (Watchdog Timer) is a timer circuit that typically features an input known as "kicking the dog" or "servicing the dog," and an output connected to the RST terminal of the MCU. When the MCU is operating normally, it outputs a signal to the "kicking the dog" input at regular intervals to reset the WDT. If the WDT is not serviced within the specified time (usually when the program runs into an abnormal state), the WDT will timeout and send a reset signal to the MCU, causing it to reset and preventing it from freezing. The role of the watchdog timer is to prevent the program from entering an infinite loop or running abnormally.

Function List
----------------
1. :ref:`Initialize <wdt_init>` - Initialize the watchdog driver.
2. :ref:`Start WDT <wdt_start>` - Start the watchdog.
3. :ref:`Stop WDT <wdt_stop>` - Stop the watchdog.
4. :ref:`Set WDT Period <set_wdt_counter>` - Set the watchdog period.
5. :ref:`Get WDT Period <get_wdt_counter>` - Get the watchdog period.
6. :ref:`Get WDT Remaining Time <get_wdt_remain>` - Get the watchdog remaining time.
7. :ref:`Feed the Dog <wdt_feed>` - Feed the watchdog.

Feature Overview
-------------------
**Setting and Getting Period Values**: Set and get the watchdog period value,which takes effect immediately upon setting, clears the previous count value, and restarts.

**Getting Remaining Time**: Obtain the remaining time of the watchdog timer to observe its behavior.

**High Precision**: The watchdog timer possesses high-precision time control capabilities.

**Interrupt**: The timer supports interrupts.and When the watchdog is triggered, it sets the reboot reason and resets the system.

Main Features
---------------

.. _wdt_init:

Initialize the Watchdog Timer
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Before using the timer, you need to call the ``wm_drv_wdt_init()`` function to allocate resources for the watchdog timer. The ``wm_device_t`` structure  is used to receive the timer device identifier. Example:

.. code:: c

       wm_device_t *wdt_dev;
       wdt_dev = wm_drv_wdt_init("wdt");

The first parameter specifies the device name.

.. warning:: After the watchdog is initialized, if ``wm_drv_wdt_deinit()`` is not called, calling ``wm_drv_wdt_init()`` again will return ``NULL``.

.. _wdt_start:

Starting the Watchdog Timer
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The function ``wm_drv_wdt_start()`` is called to start the watchdog timer. Example:

.. code:: c

      wm_device_t *wdt_dev;
      wdt_dev = wm_drv_wdt_init("wdt");
        
      wm_drv_wdt_start(wdt_dev);
        
The first parameter is a pointer to the timer device of type ``wm_device_t*``.

.. _wdt_stop:

Stopping the Watchdog Timer
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    The function ``wm_drv_wdt_stop()`` is called to stop the watchdog timer . Example:

    .. code:: c

        wm_device_t *wdt_dev;
        wdt_dev = wm_drv_wdt_init("wdt");
        
        wm_drv_wdt_stop(wdt_dev);
        
    The first parameter is a pointer to the timer device of type ``wm_device_t*``.

.. _set_wdt_counter:

Setting the Watchdog Timer Period
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The function ``wm_drv_wdt_set_counter_value()``  is called to set the watchdog timer period in microseconds. Example:

.. code:: c

      wm_device_t *wdt_dev;
      wdt_dev = wm_drv_wdt_init("wdt");

      wm_drv_wdt_set_counter_value(wdt_dev, 1000 * 1000 * 10); 
    
The first parameter is a pointer to the timer device of type ``wm_device_t*``.

The second parameter is an unsigned 32-bit integer representing the watchdog timer period in microseconds. In this example, the watchdog period is set to 10 seconds.

.. _get_wdt_counter:

Getting the Watchdog Timer Period
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The function ``wm_drv_wdt_get_counter_value()``  is called to get the watchdog timer period value. Example:

.. code:: c

      wm_device_t *wdt_dev;
      int counter_value = 0;
        
      wdt_dev = wm_drv_wdt_init("wdt");
      wm_drv_wdt_get_counter_value(wdt_dev, &counter_value); 

The first parameter is a pointer to the watchdog timer device of type ``wm_device_t*``.

The second parameter is the watchdog period value to be retrieved, of type ``int``.

.. _get_wdt_remain:

Getting the Watchdog Timer Remaining Time
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    The function ``wm_drv_wdt_get_remaining_time()`` is called to get the watchdog timer's remaining time value. Example:

    .. code:: c

        wm_device_t *wdt_dev;
        int remain = 0;
        
        wdt_dev = wm_drv_wdt_init("wdt");
        wm_drv_wdt_get_remaining_time(wdt_dev, &remain); 

    The first parameter is a pointer to the watchdog timer device of type ``wm_device_t*``.

    The second parameter is the remaining time value to be retrieved, of type ``int``.

.. _wdt_feed:

Feeding the Watchdog Timer
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The function ``wm_drv_wdt_feed()``  is called to "feed" the watchdog timer. Example:

.. code:: c

     wm_device_t *wdt_dev;
     wdt_dev = wm_drv_wdt_init("wdt");
        
     wm_drv_wdt_feed(wdt_dev);
        
The first parameter is a pointer to the timer device of type ``wm_device_t*``.

Application Example
----------------------
For a basic example of using the WDT, please refer to: examples/peripheral/wdt

API Reference
-------------------
For WDT-related API references, please refer to:

:ref:`label_api_wdt`
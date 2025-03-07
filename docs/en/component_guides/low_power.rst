
Low Power Modes
================

Overview
-----------

In an embedded IoT application field where standby performance is important, 
this section aims to introduce the low-power mode of the WM IoT SDK.

Low Power Mode
----------------

There are currently a total of four low-power modes:

- No sleep

- Light Sleep

- Deep Sleep

- System Standby

The corresponding code is:

.. code:: C

    typedef enum {
        WM_PM_MODE_NO_SLEEP,
        WM_PM_MODE_LIGHT_SLEEP,
        WM_PM_MODE_DEEP_SLEEP,
        WM_PM_MODE_STANDBY,
    } wm_pm_mode_t;


model comparison
--------------------

    ====================== ======================== ======================================== ================================ ================================
    Mode                   CPU                      Peripheral Status                        WiFi                             BLE
    ====================== ======================== ======================================== ================================ ================================
    no sleep               normal work              normal work                              normal work                      normal work
    light sleep            sleep                    configurable to work or sleep on demand  normal work, can stay connected  normal work, can stay connected
    deep sleep             sleep                    sleep                                    normal work, can stay connected  normal work, can stay connected
    system standby         sleep                    sleep                                    sleep, not staying connected     sleep, not staying connected
    ====================== ======================== ======================================== ================================ ================================


The power consumption of these four low-power modes are, 
in descending order: No Sleep > Light Sleep > Deep Sleep > System Standby.


Tickless Energy Saving Mode
------------------------------

Currently FreeRTOS tickless power saving mode is used for low power consumption, 
you can modify the conditions for entering tickless power saving mode according to the actual usage situation.

This can be done by changing ``Components configuration → FreeRTOS → FreeRTOS tickless idle time`` in menuconfig.

.. only:: w800

    When using **Deep Sleep**, you need to change the value of **FreeRTOS tickless idle time** to 9 or above for the deep sleep to take effect.


WiFi power saving mode
-----------------------

When in actual use, if the WiFi function is also used, you need to configure the WiFi power saving mode as well.
WiFi power saving only applies to station mode, which turns off RF, PHY and BBP in sleep state to reduce power consumption.
If enabled, the station will periodically switch between active and sleep state after connecting to AP successfully, and the station can maintain connection with AP.

WiFi power saving is configured using the interface ``wm_wifi_set_ps`` with the following configurable options:

 - none: no power saving, can work at full power.

 - Minimum modem: In modem minimum sleep mode, the station wakes up every DTIM to receive beacon frames. 

   Broadcast data is transmitted after the DTIM and will not be lost, this is the default mode.

 - Maximum modem: In maximum modem sleep mode, the station will wake up every listen_interval to receive beacon frames.

   Since the station may be dormant during DTIM time, broadcast data may be lost. 

   The longer the listen_interval, the more power is saved, but broadcast data is more likely to be lost.


Caveat
--------

 - In light sleep mode, ``Timer5`` is used as the wake-up source by default. If this timer is already used by other applications, the light power saving mode is disabled. 

   If you want to change it to another timer, you can change it manually in ``menuconfig`` .

 - In deep sleep mode, the CPU and each peripheral clock gating will be turned off, and this mode cannot be selected if the peripheral needs to keep working continuously (e.g. PWM).

 - In deep sleep mode, the default timer provided by PMU will be used to wake up automatically, although W80X also provides special IO wake up, but due to the hardware limitation of W80X, it is not possible to check the actual sleep state.

   Although W80X also provides special IO wakeup, because of the hardware limitation of W80X, the actual sleep time cannot be queried, so the tick timing of freertos may be inaccurate if the IO is used to wake up manually after sleep,

   in order to ensure tick timing, it is a good idea to set the callback function ``wm_pm_get_slept_callback`` to inform the W80X of the actual sleep time when waking up the device using wakeup IO..

 - When the system is woken up after sleep in standby mode, the system will start running from the beginning, and the data in the memory will be zero.


Examples of Low Power Consumption
-------------------------------------

    For a basic example of using Low Power, please refer to: :ref:`examples/low_power<low_power_example>`


Low Power API Reference
-------------------------------

    To find low power related APIs see: :ref:`label_api_low_power`

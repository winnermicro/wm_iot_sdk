.. _drv_rtc:

RTC
=============

Introduction
---------------
The Real-Time Clock (RTC) is a special timing device that is typically designed for low-power operation. It can provide users with time and calendar functions. Additionally, the RTC also offers an alarm function, which can be used to wake up the device.

Function List
---------------
1. :ref:`RTC Initialization <rtc_init>`
2. :ref:`RTC Calendar <rtc_calendar>`
3. :ref:`RTC Alarm <rtc_alarm>`

Function Overview
-------------------
 **Calendar Function**: The RTC module supports setting and querying the calendar, including: year, month, day, hour, minute, and second.

  **Alarm Function**： The RTC module supports configuring alarms and registering alarm callbacks; when the alarm goes off, it will notify the user through the callback.


Main Functions
-----------------

.. _rtc_init:

RTC Initialization
^^^^^^^^^^^^^^^^^^^^

Before using the RTC, you need to initialize it by calling ``wm_drv_rtc_init("rtc")`` .

If the RTC has already been initialized by another module, you can call ``wm_dt_get_device_by_name("rtc")`` to query the RTC device pointer.

.. code:: c

    wm_device_t *rtc_dev;

    /** Initialize the RTC */
    rtc_dev = wm_drv_rtc_init("rtc");

    /** Query the RTC device pointer */
    rtc_dev = wm_dt_get_device_by_name("rtc");

.. _rtc_calendar:


RTC Calendar Function
^^^^^^^^^^^^^^^^^^^^^^^^

The RTC provides calendar query and setting functions. The WM800 supports calendar functions within the range of 2000-01-01 00:00:00 to 2127-12-31 23:59:59. The current time should be configured when the device is powered on for the first time.

**Setting the RTC Calendar**

.. code:: c

    wm_device_t *rtc_dev;
    struct tm tm = { 0 };

    /** The RTC has been initialized */

    /** Query the RTC device pointer */
    rtc_dev = wm_dt_get_device_by_name("rtc");

    /** Set the calendar time to: 2024-07-01 12:00:00 */
    tm.tm_year = 124;  // Years since 1900
    tm.tm_mon  = 6;    // 0-based month (July)
    tm.tm_mday = 1;
    tm.tm_hour = 12;
    tm.tm_min  = 0;
    tm.tm_sec  = 0;

    /** Set the RTC calendar */
    wm_drv_rtc_set_time(rtc_dev, &tm);

**Querying the RTC Calendar**

.. code:: c

    char buffer[32];
    wm_device_t *rtc_dev;
    struct tm tm = { 0 };

    /** The RTC has been initialized */

    /** Query the RTC device pointer */
    rtc_dev = wm_dt_get_device_by_name("rtc");

    /** Query the RTC calendar */
    wm_drv_rtc_get_time(rtc_dev, &tm);

    /** Serialize the retrieved calendar, depends on the C standard library <time.h> */
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);

    /** Display the current time */
    printf("%s\n", buffer);

.. _rtc_alarm:

RTC Alarm Function
^^^^^^^^^^^^^^^^^^^^^

**Query RTC Alarm ID**

Depending on the device model, the RTC can provide one or multiple alarms for user to use. Pay attention to checking the function's return value.

Note: The W800 has only one RTC alarm.

.. code:: c

    wm_device_t *rtc_dev;
    int id;

    /** The RTC has been initialized */

    /** Query the RTC device pointer */
    rtc_dev = wm_dt_get_device_by_name("rtc");

    /** Query the available RTC alarm */
    wm_drv_rtc_get_alarm_available_id(rtc_dev, &id);

**Query RTC Alarm Mask**

The RTC provides a query  for the alarm mask. The query result can be used to set up repeating alarms based on the types defined  in ``wm_rtc_alarm_mask_e``.

Note: The W800 currently does not  support repeating alarms, so the queried mask should be 0.

.. code:: c

    wm_device_t *rtc_dev;
    int mask;

    /** The RTC has been initialized */

    /** Query the RTC device pointer */
    rtc_dev = wm_dt_get_device_by_name("rtc");

    /** Query the alarm mask for ID 0 */
    wm_drv_rtc_get_alarm_mask(rtc_dev, 0, &mask);

**Registering RTC Alarm Callback**

Users can register an alarm callback with the RTC. When the alarm expires, the RTC will callback the user-registered function.
Additionally, you can register a private pointer form their application with the RTC. When the RTC calls the user's callback, it will pass this pointer to the user's callback for use.

.. code:: c

    /** User-defined callback function */
    static void wm_rtc_alarm_callback_handle(void *user_data) {
        /** RTC alarm has expired */
    }

    /** Register the RTC alarm callback */
    wm_device_t *rtc_dev;

    /** The RTC has been initialized */

    /** Query the RTC device pointer */
    rtc_dev = wm_dt_get_device_by_name("rtc");

    wm_drv_rtc_register_alarm_callback(rtc_dev, 0, wm_rtc_alarm_callback_handle, NULL);

**Setting the RTC Alarm**

Users can set the RTC alarm time. When the alarm expires, the device will trigger the RTC alarm interrupt and invoke the user- registered callback.

.. code:: c

    struct tm tm = { 0 };
    wm_device_t *rtc_dev;

    /** The RTC has been initialized */

    /** Query the RTC device pointer */
    rtc_dev = wm_dt_get_device_by_name("rtc");

    /** Set the alarm to expire on 2024-08-01 12:00:00 */
    tm.tm_year = 124;  // Years since 1900
    tm.tm_mon  = 7;    // 0-based month (August)
    tm.tm_mday = 1;
    tm.tm_hour = 12;
    tm.tm_min  = 0;
    tm.tm_sec  = 0;

    wm_drv_rtc_set_alarm(rtc_dev, 0, 0, &tm);

**Canceling the RTC Alarm**

Based on user business logic, the RTC also provides an interface to cancel the RTC alarm.

.. code:: c

    struct tm tm = { 0 };
    wm_device_t *rtc_dev;

    /** The RTC has been initialized */

    /** Query the RTC device pointer */
    rtc_dev = wm_dt_get_device_by_name("rtc");

    /** Cancel the RTC alarm */
    wm_drv_rtc_abort_alarm(rtc_dev, 0);

Application Example
---------------------
For a basic example of using ADC, please refer to: examples/peripheral/rtc

API Reference
---------------

    :ref:`label_api_rtc`

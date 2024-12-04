
.. _drv_rtc:

RTC
=============

简介
-------------

    实时时钟（RTC，Real-Time Clock）是一种特殊的计时设备，其通常设计用于低功耗运行，可以为用户提供时间和日历功能。同时 RTC 也提供闹钟功能，可用于唤醒设备。

功能列表
-------------
    1. :ref:`RTC 初始化 <rtc_init>`
    2. :ref:`RTC 日历 <rtc_calendar>`
    3. :ref:`RTC 闹钟 <rtc_alarm>`

功能概述
-------------

    **日历功能：** RTC 模块支持设置、查询日历，包含：年、月、日、时、分、秒。

    **闹钟功能：** RTC 模块支持配置闹钟，以及注册闹钟回调；当闹钟到期时，将通过回调通知用户。

主要功能
-------------

.. _rtc_init:

RTC 初始化
^^^^^^^^^^^^^^^^

   使用 RTC 时前需调用 ``wm_drv_rtc_init("rtc")`` 初始化 RTC。

   若 RTC 已被其他模块初始化，可以调用 ``wm_dt_get_device_by_name("rtc")`` 查询 RTC 的 device 指针

    .. code:: c

        wm_device_t *rtc_dev;

        /** 初始化 RTC */
        rtc_dev = wm_drv_rtc_init("rtc");

        /** 查询 RTC device 指针 */
        rtc_dev = wm_dt_get_device_by_name("rtc");

.. _rtc_calendar:

RTC 日历功能
^^^^^^^^^^^^^^^^

    RTC 提供日历的查询和设置功能，WM800 支持 2000-01-01 00:00:00 ~ 2127-12-31 23:59:59 范围内的日历功能。设备首次上电时，应配置好当前时间。

    **设置 RTC 日历**

    .. code:: c

        wm_device_t *rtc_dev;
        struct tm tm = { 0 };

        /** RTC 已被初始化 */

        /** 查询 RTC device 指针 */
        rtc_dev = wm_dt_get_device_by_name("rtc");

        /** 设置日历时间为： 2024-07-01 12:00:00 */
        tm.tm_year = 124;
        tm.tm_mon  = 6;
        tm.tm_mday = 1;
        tm.tm_hour = 12;
        tm.tm_min  = 0;
        tm.tm_sec  = 0;

        /** 设置 RTC 日历 */
        wm_drv_rtc_set_time(rtc_dev, &tm);

    **查询 RTC 日历**

    .. code:: c

        char buffer[32];
        wm_device_t *rtc_dev;
        struct tm tm = { 0 };

        /** RTC 已被初始化 */

        /** 查询 RTC device 指针 */
        rtc_dev = wm_dt_get_device_by_name("rtc");

        /** 查询 RTC 日历 */
        wm_drv_rtc_get_time(rtc_dev, &tm);

        /** 序列化获取到的日历，依赖 C 标准库 <time.h> */
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);

        /** 显示当前时间 */
        printf("%s\n", buffer);

.. _rtc_alarm:

RTC 闹钟功能
^^^^^^^^^^^^^^^^

    **查询 RTC 闹钟 ID**

    RTC 根据不同的设备型号，可提供 1 个或多个 RTC 闹钟供用户使用，注意判断函数返回值。

    注： W800 只有 1 个 RTC 闹钟。

    .. code:: c

        wm_device_t *rtc_dev;
        int id;

        /** RTC 已被初始化 */

        /** 查询 RTC device 指针 */
        rtc_dev = wm_dt_get_device_by_name("rtc");

        /** 查询 RTC 空闲闹钟 */
        wm_drv_rtc_get_alarm_available_id(rtc_dev, &id);


    **查询 RTC 闹钟掩码**

    RTC 提供闹钟掩码的查询，查询的结果可以根据 wm_rtc_alarm_mask_e 中的类型来使用 RTC 重复闹钟。

    注： W800 暂不支持 RTC 重复闹钟，查询到的掩码应该为 0。

    .. code:: c

        wm_device_t *rtc_dev;
        int mask;

        /** RTC 已被初始化 */

        /** 查询 RTC device 指针 */
        rtc_dev = wm_dt_get_device_by_name("rtc");

        /** 查询 ID 为 0 的闹钟掩码 */
        wm_drv_rtc_get_alarm_available_id(rtc_dev, 0, &mask);

    **注册 RTC 闹钟回调**

    用户可以向 RTC 注册闹钟回调，在闹钟到期后，RTC 回调用户注册的 callback。
    另外，也可以将应用程序的私有指针注册到 RTC，RTC 在调用用户的 callback 时，把该指针传递给用户的 callback 使用。

    .. code:: c

        /** 用户自己的 callback 回调函数 */
        static void wm_rtc_alarm_callback_handle(void *user_data) {
            /** RTC 闹钟已到期 */
        }

        /** 向 RTC 注册闹钟回调 */
        wm_device_t *rtc_dev;

        /** RTC 已被初始化 */

        /** 查询 RTC device 指针 */
        rtc_dev = wm_dt_get_device_by_name("rtc");

        wm_drv_rtc_register_alarm_callback(rtc_dev, 0, wm_rtc_alarm_callback_handle, NULL);



    **设置 RTC 闹钟**

    用户可以设置 RTC 闹钟时间，在闹钟到期后，设备会触发 RTC 闹钟中断，并回调用户注册的 callback。

    .. code:: c

        struct tm tm         = { 0 };
        wm_device_t *rtc_dev;

        /** RTC 已被初始化 */

        /** 查询 RTC device 指针 */
        rtc_dev = wm_dt_get_device_by_name("rtc");

        /** 设置闹钟在 2024-08-01 12:00:00 到期*/
        tm.tm_year = 124;
        tm.tm_mon  = 7;
        tm.tm_mday = 1;
        tm.tm_hour = 12;
        tm.tm_min  = 0;
        tm.tm_sec  = 0;

        wm_drv_rtc_set_alarm(rtc_dev, 0, 0, &tm);

    **取消 RTC 闹钟**

    根据用户业务逻辑，RTC 也提供了取消 RTC 闹钟接口。

    .. code:: c

        struct tm tm         = { 0 };
        wm_device_t *rtc_dev;

        /** RTC 已被初始化 */

        /** 查询 RTC device 指针 */
        rtc_dev = wm_dt_get_device_by_name("rtc");

        /** 取消 RTC 闹钟 */
        wm_drv_rtc_abort_alarm(rtc_dev, 0);


应用实例
-------------
    使用 RTC 基本示例请参照 :ref:`examples/peripheral/rtc<peripheral_example>`

API参考
-------------

    :ref:`label_api_rtc`

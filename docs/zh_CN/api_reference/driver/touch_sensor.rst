========================
Touch Sensor API 参考
========================

该模块分为两个部分 API, 包括 Touch Sensor 和 Touch Button

Touch Sensor
==================

Touch Sensor 头文件
-----------------------

- wm_iot_sdk/components/driver/include/wm_drv_touch_sensor.h
- This header file can be included with:

.. code-block:: c
   :emphasize-lines: 1

   #include "wm_drv_touch_sensor.h"

Touch Sensor 类型定义
-----------------------
.. doxygengroup:: WM_DRV_TOUCH_SENSOR_TYPEs
    :project: wm-iot-sdk-apis
    :content-only:

Touch Sensor 接口
-----------------------
.. doxygengroup:: WM_DRV_TOUCH_SENSOR_APIs
    :project: wm-iot-sdk-apis
    :content-only:


.. _label_api_touch_button:

Touch Button
==================

Touch Button 头文件
-----------------------

- wm_iot_sdk/components/driver/include/wm_drv_touch_button.h
- This header file can be included with:

.. code-block:: c
   :emphasize-lines: 1

   #include "wm_drv_touch_button.h"

Touch Button 类型定义
-----------------------
.. doxygengroup:: WM_DRV_TOUCH_BUTTON_TYPEs
    :project: wm-iot-sdk-apis
    :content-only:

Touch Button 接口
-----------------------
.. doxygengroup:: WM_DRV_TOUCH_BUTTON_APIs
    :project: wm-iot-sdk-apis
    :content-only:

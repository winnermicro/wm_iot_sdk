.. _touch_sensor:

Touch Sensor
==============

Introduction
--------------

The touch sensor system consists of a protective cover layer, touch electrodes, an insulating substrate, and wiring. The protective cover layer is located on the top, while the insulating substrate is equipped with electrodes and wiring. The touch cover layer will cause changes in capacitance, and based on these changes, it determines whether the touch is valid.



Function List
---------------

  1. :ref:`Initialize <touch_sensor_init>` - Initialize the touch sensor driver.
  2. :ref:`Set Threshold <touch_sensor_set_threshold>` - Set the threshold for the touch sensor.
  3. :ref:`Get Threshold <touch_sensor_get_threshold>` - Retrieve the threshold of the touch sensor.
  4. :ref:`Read Count <touch_sensor_read_countnum>` - Read the count value of the touch sensor.
  5. :ref:`Start Sensor <touch_sensor_start>` - Start the touch sensor.
  6. :ref:`Stop Sensor <touch_sensor_stop>` - Stop the touch sensor.
  7. :ref:`Register Callback <touch_sensor_register_callback>` - Register a callback function for the touch sensor.
  8. :ref:`De-initialization <touch_sensor_deinit>`——De-initializes the touch sensor driver.

Feature Overview
-------------------

Currently, only W801S, W802, W803, and W806 are integrated with touch sensor modules. During operation, they sequentially scan the status of each touch button at regular intervals, counting and recording the status of each button within a set time window. If the set threshold is exceeded, it is determined that the button has been touched, and application processing is performed through polling or interrupts.

The current software-adjustable parameter is only the touch threshold (adjustment range: 0-127), which can be adjusted according to the actual environment to obtain the most suitable value.

W801S and W806 have a total of 12 available TOUCH pins and two control pins, with the touch sensor pin relationships as shown in the following table.

.. list-table::
   :widths: 25 25 25 25 25
   :header-rows: 0
   :align: center

   * - Touch Sensor Number (Name)
     - Pin Number
     - Pin Name
     - Pin Multiplex Function
     - Pin Attribute

   * - TOUCH1
     - PIN_NUM_9
     - PA9
     - OPT7
     - float/input

   * - TOUCH2
     - PIN_NUM_10
     - PA10
     - OPT7
     - float/input
   
   * - TOUCH3
     - PIN_NUM_16
     - PB0
     - OPT7
     - float/input

   * - TOUCH4
     - PIN_NUM_17
     - PB1
     - OPT7
     - float/input

   * - TOUCH5
     - PIN_NUM_18
     - PB2
     - OPT7
     - float/input

   * - TOUCH6
     - PIN_NUM_19
     - PB3
     - OPT7
     - float/input

   * - TOUCH7
     - PIN_NUM_20
     - PB4
     - OPT7
     - float/input

   * - TOUCH8
     - PIN_NUM_21
     - PB5
     - OPT7
     - float/input

   * - TOUCH9
     - PIN_NUM_22
     - PB6
     - OPT7
     - float/input

   * - TOUCH10
     - PIN_NUM_23
     - PB7
     - OPT7
     - float/input

   * - TOUCH11
     - PIN_NUM_24
     - PB8
     - OPT7
     - float/input

   * - TOUCH12
     - PIN_NUM_25
     - PB9
     - OPT7
     - float/input

   * - CMOD
     - PIN_NUM_12
     - PA12
     - OPT7
     - float/input

   * - CDC
     - PIN_NUM_14
     - PA14
     - OPT7
     - float/input

.. only:: w802

W802 has a total of 13 available TOUCH pins and two control pins, with the touch sensor pin relationships as shown in the following table.
    

.. list-table::
   :widths: 25 25 25 25 25
   :header-rows: 0
   :align: center

   * - Touch Sensor Number (Name)
     - Pin Number
     - Pin Name
     - Pin Multiplex Function
     - Pin Attribute

   * - TOUCH1
     - PIN_NUM_9
     - PA9
     - OPT7
     - float/input

   * - TOUCH2
     - PIN_NUM_10
     - PA10
     - OPT7
     - float/input

   * - TOUCH3
     - PIN_NUM_16
     - PB0
     - OPT7
     - float/input

   * - TOUCH4
     - PIN_NUM_17
     - PB1
     - OPT7
     - float/input

   * - TOUCH5
     - PIN_NUM_18
     - PB2
     - OPT7
     - float/input

   * - TOUCH6
     - PIN_NUM_19
     - PB3
     - OPT7
     - float/input

   * - TOUCH7
     - PIN_NUM_20
     - PB4
     - OPT7
     - float/input

   * - TOUCH8
     - PIN_NUM_21
     - PB5
     - OPT7
     - float/input

   * - TOUCH9
     - PIN_NUM_22
     - PB6
     - OPT7
     - float/input

   * - TOUCH10
     - PIN_NUM_23
     - PB7
     - OPT7
     - float/input

   * - TOUCH11
     - PIN_NUM_24
     - PB8
     - OPT7
     - float/input

   * - TOUCH12
     - PIN_NUM_25
     - PB9
     - OPT7
     - float/input

   * - TOUCH15
     - PIN_NUM_45
     - PB29
     - OPT7
     - float/input

   * - CMOD
     - PIN_NUM_12
     - PA12
     - OPT7
     - float/input

   * - CDC
     - PIN_NUM_14
     - PA14
     - OPT7
     - float/input
.. only:: w803

 W803 has a total of 10 available TOUCH touch pins and two control pins. The relationship of the touch sensor pins is shown in the table below.

.. list-table::
   :widths: 25 25 25 25 25
   :header-rows: 0
   :align: center

   * - Touch Sensor Number (Name)
     - Pin Number
     - Pin Name
     - Pin Multiplex Function
     - Pin Attribute

   * - TOUCH3
     - PIN_NUM_16
     - PB0
     - OPT7
     - float/input

   * - TOUCH4
     - PIN_NUM_17
     - PB1
     - OPT7
     - float/input

   * - TOUCH5
     - PIN_NUM_18
     - PB2
     - OPT7
     - float/input

   * - TOUCH6
     - PIN_NUM_19
     - PB3
     - OPT7
     - float/input

   * - TOUCH7
     - PIN_NUM_20
     - PB4
     - OPT7
     - float/input

   * - TOUCH8
     - PIN_NUM_21
     - PB5
     - OPT7
     - float/input

   * - TOUCH9
     - PIN_NUM_22
     - PB6
     - OPT7
     - float/input

   * - TOUCH10
     - PIN_NUM_23
     - PB7
     - OPT7
     - float/input

   * - TOUCH11
     - PIN_NUM_24
     - PB8
     - OPT7
     - float/input

   * - TOUCH12
     - PIN_NUM_25
     - PB9
     - OPT7
     - float/input

   * - CMOD
     - PIN_NUM_12
     - PA12
     - OPT7
     - float/input

   * - CDC
     - PIN_NUM_14
     - PA14
     - OPT7
     - float/input



During the design of the touch sensor circuit, to achieve better touch performance, please consider the following factors and suggestions:

.. only:: w800

.. list-table::
   :widths: 25 25
   :header-rows: 0
   :align: center

   * - Influencing Factor
     - Recommended Design

   * - Touch panel thickness
     - Within 3mm

   * - Light guide pillar
     - Should not be too thick

   * - CMOD capacitor
     - 2nF - 10nF

   * - CDC capacitor
     - Floating

   * - Pin series resistor
     - If conduction interference is not considered, values can be 1kΩ - 10kΩ

        If conduction interference is considered, values are recommended to be 4.7kΩ - 10kΩ

   * - Touch and non-touch rate of change
     - Ensure at least 2.5%, preferably greater than 3%

   * - Gap between touch panel and PCB
     - Should not have gaps



Main Features
-----------------------

.. _touch_sensor_init:

Initializing the Touch Sensor
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To use the touch sensor, you need to call the ``wm_drv_touch_sensor_init()`` function to initialize the driver. Example:

.. code:: c

     wm_device_t *touch_dev;
     touch_dev = wm_drv_touch_sensor_init("touch_sensor");

.. _touch_sensor_set_threshold:

Setting the Threshold
^^^^^^^^^^^^^^^^^^^^^^^^^^^

To set the threshold of the touch sensor, you can call the ``wm_drv_touch_sensor_set_threshold()`` function. Example:

.. code:: c

     wm_device_t *touch_dev;
     touch_dev = wm_drv_touch_sensor_init("touch_sensor");

     wm_drv_touch_sensor_set_threshold(touch_dev, WM_TOUCH_SENSOR_NUM_1, 100);

.. _touch_sensor_get_threshold:

Getting the Threshold
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To retrieve the threshold value of the touch sensor,  you can call the ``wm_drv_touch_sensor_get_threshold()`` function. Example:

.. code:: c

     wm_device_t *touch_dev;
     touch_dev = wm_drv_touch_sensor_init("touch_sensor");

     uint32_t threshold;
     wm_drv_touch_sensor_get_threshold(touch_dev, WM_TOUCH_SENSOR_NUM_1, &threshold);

.. _touch_sensor_read_countnum:

Reading the Count
^^^^^^^^^^^^^^^^^^^

To read the count value of the touch sensor, you can call the ``wm_drv_touch_sensor_read_countnum()`` function. Example:

.. code:: c

    wm_device_t *touch_dev;
    touch_dev = wm_drv_touch_sensor_init("touch_sensor");

    uint32_t countnum;
    wm_drv_touch_sensor_read_countnum(touch_dev, 0, &countnum);

.. _touch_sensor_start:

Starting the Sensor
^^^^^^^^^^^^^^^^^^^^^^^^^^

To start the touch sensor, you can call the ``wm_drv_touch_sensor_start()`` function. Example:

.. code:: c

    wm_device_t *touch_dev;
    touch_dev = wm_drv_touch_sensor_init("touch_sensor");

    wm_drv_touch_sensor_start(touch_dev, WM_TOUCH_SENSOR_NUM_1);

.. _touch_sensor_stop:

Stopping the Sensor
^^^^^^^^^^^^^^^^^^^^^^^^

To stop the touch sensor, you can call the ``wm_drv_touch_sensor_stop()`` function. Example:

.. code:: c

    wm_device_t *touch_dev;
    touch_dev = wm_drv_touch_sensor_init("touch_sensor");

    wm_drv_touch_sensor_stop(touch_dev, WM_TOUCH_SENSOR_NUM_1);

.. _touch_sensor_register_callback:

Registering a Callback
^^^^^^^^^^^^^^^^^^^^^^^^^

To register a callback function for the touch sensor, you can call the ``wm_drv_touch_sensor_register_callback()`` function. Example:

.. code:: c

    wm_device_t *touch_dev;
    touch_dev = wm_drv_touch_sensor_init("touch_sensor");

    void touch_callback(wm_touch_sensor_num_t touch_num, void *priv)
    {
    }

    wm_drv_touch_sensor_register_callback(touch_dev, WM_TOUCH_SENSOR_NUM_1, touch_callback, NULL);

.. _touch_sensor_deinit:

Deinitialize the Touch Sensor
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

When the touch sensor is no longer needed, you can call the ``wm_drv_touch_sensor_deinit()`` function to deinitialize the driver and release resources. Example:

.. code:: c

    wm_device_t *touch_dev;
    touch_dev = wm_drv_touch_sensor_init("touch_sensor");

    wm_drv_touch_sensor_deinit(touch_dev);

Application Examples
------------------------

For basic examples of using the touch sensor, please refer to: examples/peripheral/touch_sensor

API Reference
---------------

- Touch Button :ref:`label_api_touch_button`
   

.. _adc:

ADC
=============

Introduction
---------------
An ADC (Analog-to-Digital Converter) is a hardware module that converts analog signals into digital signals. In embedded systems and microcontrollers, ADCs are used to convert sensor or other analog input signals into digital data for further processing and analysis.It should be noted that in the W80x series chips, the analog conversion circuit of the ADC requires approximately 2MS to generate one data.


Function  List
------------------

1. :ref:`Initialization <adc_init>` —— Initialize the ADC driver.
2. :ref:`Configuration of ADC <adc_cfg>` —— Configure ADC channels and parameters.
3. :ref:`Register Interrupt Callback <adc_register_callback>` —— Registers a callback function for timer interrupts.
4. :ref:`Get Single ADC Result <adc_oneshot>` —— Obtains the result of a single ADC conversion.
5. :ref:`Get Polling ADC Result <adc_polling>` —— Obtains the result of an ADC conversion in polling mode.
6. :ref:`Start ADC Interrupt <adc_start_it>` —— Start ADC interrupt mode.
7. :ref:`Stop ADC Interrupt <adc_stop_it>` —— Stop ADC interrupt mode.
8. :ref:`Read Internal Chip Temperature <adc_read_temperature>` —— Read the internal temperature of the chip.
9. :ref:`Read Chip Voltage <adc_read_vdd>` —— Read the supply voltage of the chip.
10. :ref:`Calculate Voltage Value <adc_cal_voltage>` —— Calculate the actual voltage value based on the ADC result.

Function Overview
---------------------

**Resource Management**: The W800 has 4 independent ADC input channels and 2 differential channels. Users can manage different channels according to specific needs.

**Flexible Parameter Configuration**: The ADC supports configuring the number of channels such as the number of channels and sampling gain to suit various application needs.

**High Precision**: The ADC has high-precision signal conversion capabilities, suitable for various precise measurement applications.

**Interrupts and Callbacks**: The ADC supports interrupts and callback functions, which can trigger callback functions upon completion of conversions, facilitating real-time data processing.

**Temperature Sensing**: Built-in temperature sensors allow for reading the internal temperature of the chip via the ADC.

**Voltage Detection**: The ADC can detect the chip's supply voltage, facilitating power management.

Main Features
-----------------------

.. _adc_init:

Initialize ADC
^^^^^^^^^^^^^^^^^^^^

Before using the ADC, you need to call the ``wm_drv_adc_init()`` function to allocate resources for the ADC, using the ``wm_device_t`` structure to receive the ADC device identifier. Example:

.. code:: c

   wm_device_t *adc_dev;
   adc_dev = wm_drv_adc_init("adc");

The first parameter specifies the device name, which is defined in the device table.

.. warning:: After initializing the ADC, if ``wm_drv_adc_deinit`` is not called, calling ``wm_drv_adc_init`` again will return ``NULL``.

.. _adc_cfg:

Configuring the ADC
^^^^^^^^^^^^^^^^^^^^

To configure the parameters of the ADC device, call the``wm_drv_adc_cfg()``  function. Example:

.. code:: c

   wm_device_t *adc_dev;
   adc_dev = wm_drv_adc_init("adc");

   wm_drv_adc_cfg_t adc_cfg;
   adc_cfg.adc_channel_count = 1;
   /* set cfg */
   ...

   wm_drv_adc_cfg(adc_dev, &adc_cfg);

The first parameter is a pointer to the ADC device, of type ``wm_device_t*``.

The second parameter is a pointer to a structure containing the ADC configuration parameters, of type ``wm_drv_adc_cfg_t``.

.. _adc_register_callback:

Registering a Callback
^^^^^^^^^^^^^^^^^^^^^^^^^^

Call the function ``wm_drv_adc_register_callback()`` to register an ADC interrupt callback function. The `adc_callback` is a callback function that will be invoked when an ADC interrupt is triggered. The `arg` is a parameter passed to the callback function, which is not used in this example. Here is an example:

.. code:: c

     wm_device_t *adc_dev;
     adc_dev = wm_drv_adc_init("adc");

     void adc_callback(void *arg) {
     printf("ADC interrupt triggered!\n");
     }

     wm_drv_adc_register_callback(adc_dev, WM_ADC_INTR_TYPE_ADC, adc_callback, NULL);

    

The first parameter is a pointer to the ADC device, of type ``wm_device_t*``.

The second parameter is the interrupt type, which indicates different types of ADC interrupts. In the example, ``WM_ADC_INTR_TYPE_ADC`` triggers an interrupt triggered when ADC conversion ends. It can also be set to ``WM_ADC_INTR_TYPE_DMA`` for DMA (Direct Memory Access) interrupts.
Note that the w800 does not currently support ADC DMA transfers. ``WM_ADC_INTR_TYPE_COMP`` represents a comparator interrupt. ``WM_ADC_INTR_TYPE_MAX`` is the maximum value of the enumeration type, used for boundary checking. The type is ``wm_adc_intr_type_t``.

The third parameter is the callback function, of type ``wm_drv_adc_callback_t``.

The fourth parameter is private data passed to the callback function, of type ``void *``.

.. _adc_oneshot:

Generating a Single Conversion Result
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Call the function ``wm_drv_adc_oneshot()`` to perform a single ADC conversion and return the result. Here is an example:

.. code:: c

        wm_device_t *adc_dev;
        adc_dev = wm_drv_adc_init("adc");

        int32_t result;
        wm_drv_adc_oneshot(adc_dev, 0, &result);

The first parameter is a pointer to the ADC device, of type ``wm_device_t*``.

The second parameter is the ADC channel number, of type ``wm_adc_channel_t``.

The third parameter is a pointer to an ``int32_t`` variable ``result``, used to store the result of the ADC conversion.

.. _adc_polling:

Generating Continuous Conversion Results
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Call the function ``wm_drv_adc_polling()`` to perform continuous ADC conversions and store the results in a buffer. Here is an example:

.. code:: c

        wm_device_t *adc_dev;
        adc_dev = wm_drv_adc_init("adc");

        int buf[10];
        wm_drv_adc_polling(adc_dev, 0, buf, 10, 1000);

The first parameter is a pointer to the ADC device, of type ``wm_device_t*``.

The second parameter is the ADC channel number, of type ``wm_adc_channel_t``.

The third parameter is a pointer to a buffer that stores the conversion results, of type ``int*``, used to store the polled ADC data.

The fourth parameter is the size of the buffer, of type ``uint16_t``.

The fifth parameter is the timeout value in milliseconds, specifying when to stop polling after exceeding this time , of type ``uint32_t``.

.. _adc_start_it:

Starting ADC Interrupt
^^^^^^^^^^^^^^^^^^^^^^^^^^

Call the function ``wm_drv_adc_start_it()`` to start the ADC interrupt, initiating conversions in interrupt mode. Here is an example:

.. code:: c
        
        wm_device_t *adc_dev;
        adc_dev = wm_drv_adc_init("adc");

        wm_drv_adc_start_it(adc_dev, 0);

The first parameter is a pointer to the ADC device, of type ``wm_device_t*``.

The second parameter is the ADC channel number, of type ``wm_adc_channel_t``.

.. warning:: Interrupt mode will output ADC collected data rapidly. Avoid time-consuming operations in the corresponding interrupt callback, and use ``wm_drv_adc_cal_voltage()`` to calculate the collected voltage.

.. _adc_stop_it:

Stopping ADC Interrupt
^^^^^^^^^^^^^^^^^^^^^^^^^

Call the function ``wm_drv_adc_stop_it()`` to stop the ADC interrupt, ending conversions in interrupt mode. Here is an example:

.. code:: c

        wm_device_t *adc_dev;
        adc_dev = wm_drv_adc_init("adc");
        
        wm_drv_adc_stop_it(adc_dev, 0);

The first parameter is a pointer to the ADC device, of type ``wm_device_t*``.

The second parameter is the ADC channel number, of type ``wm_adc_channel_t``.

.. _adc_read_temperature:

Reading Temperature
^^^^^^^^^^^^^^^^^^^^^^

Call the function ``wm_drv_thermal_sensor_read_temperature()`` to read the temperature value within the chip. Here is an example:

.. code:: c

        wm_device_t *adc_dev;
        adc_dev = wm_drv_adc_init("adc");

        int temperature_val;
        wm_drv_thermal_sensor_read_temperature(adc_dev, &temperature_val);

The first parameter is a pointer to the ADC device, of type ``wm_device_t*``.

The second parameter is a pointer to an integer that will store the read temperature value, of type ``int*``.

.. _adc_read_vdd:

Reading Voltage
^^^^^^^^^^^^^^^^^^^^

Call the function ``wm_drv_adc_chip_voltage_read_vdd()`` to read the supply voltage of the chip. Here is an example:

.. code:: c

        wm_device_t *adc_dev;
        adc_dev = wm_drv_adc_init("adc");

        uint32_t voltage;
        wm_drv_adc_chip_voltage_read_vdd(adc_dev, &voltage);

The first parameter is a pointer to the ADC device, of type ``wm_device_t*``.

The second parameter is a pointer to a variable that will store the read voltage value, of type ``uint32_t*``.

.. _adc_cal_voltage:

Calibrating Voltage
^^^^^^^^^^^^^^^^^^^^^^^

Call the function ``wm_drv_adc_cal_voltage()`` to calibrate the chip voltage. Here is an example:

.. code:: c

        wm_device_t *adc_dev;
        adc_dev = wm_drv_adc_init("adc");

        wm_drv_adc_cal_voltage(adc_dev, 3300);

The first parameter is a pointer to the ADC device, of type ``wm_device_t*``.

The second parameter is an integer representing the raw value obtained from the ADC interrupt, with 3300 set as the raw value in this example.

Application Example
--------------------------
For a basic example of using the ADC, refer to: examples/peripheral/adc

API Reference
------------------
For more information on ADC-related APIs, refer to:

:ref:`label_api_adc`
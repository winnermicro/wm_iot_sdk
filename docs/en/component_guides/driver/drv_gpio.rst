
.. _gpio:

GPIO
=============

Introduction
-------------

GPIO (General-Purpose Input/Output) is the abbreviation for General-Purpose Input/Output port. It allows for both output and input to be controlled via software. For input, the default voltage can be configured through pull mode. For output, writing to a register can set the pin to output a high or low potential.

Function List
---------------

   1. :ref:`Initialization <gpio_init>` — Initialize the driver.
   2. :ref:`Configure Direction <gpio_set_dir>` — Set the direction (input or output) of the GPIO pin.
   3. :ref:`Set Pull Mode <gpio_set_pullmode>` — Configure the GPIO pin's pull-up, pull-down, or floating mode.
   4. :ref:`Data Read <gpio_data_get>` — Read data from the GPIO pin.
   5. :ref:`Data Write <gpio_data_set>` — Write data to the GPIO pin, where 1 represents pulling high and 0 represents pulling low.
   6. :ref:`Pin Multiplexing <gpio_iomux_func_sel>` — Select the IOMUX pin multiplexing .
   7. :ref:`Enable Interrupt <gpio_enable_isr>` — Enable GPIO interrupts.
   8. :ref:`Set Interrupt Mode <gpio_set_intr_mode>` — Configure the GPIO interrupt mode.

Function Overview
------------------------
The W800 features 48 GPIO pins, which can be configured for multiplexing through the IO MUX. For specific multiplexing relationships, please refer to the PINMUX section. Among them, OPT1-OPT4 are configured for digital multiplexing, OPT5 is for GPIO, and OPT6-OPT7 are for analog multiplexing. It is important to note that when configured for analog multiplexing, the pin needs to be set to input floating mode.
    

Main Functions
-----------------------

.. _gpio_init:

Initializing GPIO
^^^^^^^^^^^^^^^^^^^^

    Before using GPIO, the ``wm_drv_gpio_init`` function must be called to initialize the GPIO device. Utilizing  ``wm_device_t``, the following is an example code snippet:

    .. code:: c

        wm_device_t *gpio_device;
        gpio_device = wm_drv_gpio_init("gpio");

    The parameter specifies the device name, which must match the name defined in the device table.

    .. warning:: After initializing GPIO, if ``wm_drv_gpio_deinit`` is not called, calling ``wm_drv_gpio_init`` again will return ``NULL``.

.. _gpio_set_dir:

Configuring Direction
---------------------

GPIO direction configuration is used to set the pin to either input or output mode. Use the ``wm_drv_gpio_set_dir`` function to set the pin direction. Example code is as follows:

.. code:: c

    wm_drv_gpio_set_dir(WM_GPIO_NUM_0, WM_GPIO_DIR_INPUT);
    wm_drv_gpio_set_dir(WM_GPIO_NUM_0, WM_GPIO_DIR_OUTPUT);

The first parameter is the pin number, and the second parameter is the direction, which can be either ``WM_GPIO_DIR_INPUT`` or ``WM_GPIO_DIR_OUTPUT``.

.. _gpio_set_pullmode:

Configuring Pull Mode
---------------------

    GPIO pull mode configuration is used to set the pin to pull-up, pull-down, or floating mode. Use the ``wm_drv_gpio_set_pullmode`` function to configure the pin pull mode. Here is an example code snippet:

    .. code:: c

        wm_drv_gpio_set_pullmode(WM_GPIO_NUM_0, WM_GPIO_PULL_UP);
        wm_drv_gpio_set_pullmode(WM_GPIO_NUM_0, WM_GPIO_PULL_DOWN);

    The first parameter is the pin number, and the second parameter is the pull mode, which can be ``WM_GPIO_PULL_UP``, ``WM_GPIO_PULL_DOWN``, or ``WM_GPIO_FLOAT``.

.. _gpio_data_get:

Reading Data
^^^^^^^^^^^^^^^^^^^^

Use the ``wm_drv_gpio_data_get`` function to read data from a GPIO pin. This function returns the current level of the pin. Example code is as follows:

.. code:: c

    int value = wm_drv_gpio_data_get(WM_GPIO_NUM_0);

    The parameter is the pin number.

.. _gpio_data_set:

Writing Data
^^^^^^^^^^^^^^^^^^^^

    Use the ``wm_drv_gpio_data_set`` function to write data to the GPIO pin,setting the pin to high level. Here is an example code snippet:

    .. code:: c

        wm_drv_gpio_data_set(WM_GPIO_NUM_0);

    Use the ``wm_drv_gpio_data_reset`` function to set the pin to low level. Example code is as follows:

.. code:: c

        wm_drv_gpio_data_reset(WM_GPIO_NUM_0);

The parameter is the pin number.

.. _gpio_iomux_func_sel:

Pin Multiplexing Selection
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To select the multiplexing function of a GPIO pin using the using the ``wm_drv_gpio_iomux_func_sel`` function,the example code is as follows:

.. code:: c

      wm_gpio_num_t pin = WM_GPIO_NUM_21;
      wm_gpio_pin_mux_t func = WM_GPIO_IOMUX_FUN5;

      wm_drv_gpio_iomux_func_sel(pin, func);

The first parameter is the pin number, and the second parameter is the function mode, which can be referenced in the table in the function overview or checked in ``wm_io_mux.h``.

.. _gpio_enable_isr:

Enabling Interrupt
^^^^^^^^^^^^^^^^^^^^

To enable interrupts for a GPIO pin using the ``wm_drv_gpio_enable_isr`` function, the example code is as follows:

.. code:: c

      wm_drv_gpio_enable_isr(WM_GPIO_NUM_21);

The parameter is the pin number.

.. _gpio_disable_isr:

Disabling Interrupts
^^^^^^^^^^^^^^^^^^^^^^^^^^

To disable interrupts for a GPIO pin using the ``wm_drv_gpio_disable_isr`` function, the example code is as follows::

.. code:: c

      wm_drv_gpio_disable_isr(WM_GPIO_NUM_21);

 The parameter is the pin number.

.. _gpio_set_intr_mode:

Setting Interrupt Mode
^^^^^^^^^^^^^^^^^^^^^^^^^^

Configuring the interrupt trigger mode for a GPIO pin using the ``wm_drv_gpio_set_intr_mode`` function involves selecting from options such as falling edge trigger, rising edge trigger, double edge trigger, low level trigger, and high level trigger. The example code is provided below:

.. code:: c

        wm_drv_gpio_set_intr_mode(WM_GPIO_NUM_21, WM_GPIO_IRQ_TRIG_HIGH_LEVEL);

The first parameter is the pin number, while the second parameter is the interrupt mode, which can be ``WM_GPIO_IRQ_TRIG_FALLING_EDGE``, ``WM_GPIO_IRQ_TRIG_RISING_EDGE``, ``WM_GPIO_IRQ_TRIG_DOUBLE_EDGE``, ``WM_GPIO_IRQ_TRIG_LOW_LEVEL``, or ``WM_GPIO_IRQ_TRIG_HIGH_LEVEL``.

Application Example
----------------------

For a basic example of GPIO using, please refer to: examples/peripheral/gpio

API Reference
---------------
To find GPIO-related APIs, please refer to:

  :ref:`label_api_gpio`
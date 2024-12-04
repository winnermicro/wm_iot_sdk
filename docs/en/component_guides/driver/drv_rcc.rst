.. _rcc:

RCC
=============

Introduction
---------------
RCC (Reset and Clock Control) is responsible for managing various clock sources and clock dividers, providing clock enablement to various peripherals.

Function List
---------------

- Supports enabling and disabling clocks for other modules, such as I2C, UART, SPI, DMA, etc.
- Supports resetting clocks of other modules.
- Supports frequency settings for CPU/WLAN/Peripheral, etc.
- The default CPU frequency and default WLAN frequency are defined in the Device table, and can also be modified through the RCC API after the system boots up.

Features Supported by RCC HW
------------------------------
.. only:: w800

   Please refer to the Clock and Reset Module section of the register manual.


Function Descriptions
------------------------

Clock Enable/Disable
^^^^^^^^^^^^^^^^^^^^^^^
**Relevant Sequence API:**

- Call ``wm_drv_clock_init`` to initialize the clock. If the device is already initialized, obtain the device pointer through the ``wm_dt_get_device_by_name`` function.
- Call ``wm_drv_clock_enable`` to enable the clock for the corresponding module.
- Call ``wm_drv_clock_disable`` to disable the clock for the corresponding module.

**Clock modules that support enable/disable:**

- I2C, UART, SPI, DMA, RF, TIMER, GPIO, SD_ADC, PWM, LCD, I2S, RSA, 
- GPSEC, SDIOM, QSRAM, BT, TOUCH

Clock Reset
^^^^^^^^^^^^^
**Relevant Sequence API:**

- Initialize the clock by calling ``wm_drv_clock_init``. If the device is already initialized,acquire the device pointer via ``wm_dt_get_device_by_name``.
- Reset the module clock by calling  ``wm_drv_clock_reset`` .

**Result:**

- After the reset is successful, the corresponding module returns to the initialized state. For example, if the SPI module is reset, the SPI module will return to its initialized state.

**Modules that can be reset:**

- BBP, MAC, SEC, SDIO_AHB, DMA, MEM_MNG, APB, I2C, UART, SPIM, SPIS
- RF, GPIO, TIMER, SAR_ADC, PWM, LCD, I2S, RSA, QPSEC, SDIOM, BT, FLASH, TOUCH

.. warning:: After the system is running, some modules cannot be reset again. Forcing a reset may cause the system to malfunction.


Clock Frequency Setting
^^^^^^^^^^^^^^^^^^^^^^^^^

- Setting the clock for the corresponding modules

**Relevant Sequence API:**

- Initialize the clock by calling ``wm_drv_clock_init``. If the device is already initialized, acquire the device pointer through the ``wm_dt_get_device_by_name`` function.
- Invoke ``wm_drv_rcc_config_clock`` to set the frequency for the specified clock module.
- Invoke ``wm_drv_rcc_get_config_clock`` to query the current clock frequency of a specified clock module.

**Modules with configurable clocks:**

- CPU, WLAN, SD_ADC, QFLASH, GPSEC, RSA, PERIPHERAL

.. warning:: Arbitrarily setting the clock may cause the system to malfunction. It is recommended to configure the frequency of related modules by referring to the clock tree.


Clock tree
^^^^^^^^^^^^^^^^^
The clock tree as below

.. only:: w800

    .. figure:: ../../../_static/component-guides/driver/W800_clock_tree.svg
        :align: center
        :scale: 100%
        :alt: SPI clock tree


    .. note::
        1: When the CPU frequency is lower than 40 MHz, to ensure that the CPU/APB clock ratio is a positive integer, the clock frequency of the WLAN must be fixed as one quarter of the CPU frequency, so that the ratio clock of the CPU and APB is 1.

        2: In low power mode, the STANDBY device can only use RC32K as the clock source. When in Sleep mode, you can choose the RC32 and 40M frequency division of 25 KHz as the clock source.

        3: When SDIO_SLAVE, HSPI, LSPI_SLAVE, or I2S_SLAVE serves as the slave device, the clock mainly comes from the master device.


Application Example
-------------------------

 For a basic example of RCC, please refer to :ref:`examples/peripheral/rcc<peripheral_example>`

| Clock initialization example

.. code:: C

    dev = wm_drv_clock_init("rcc");
    if (dev != NULL) {
        wm_drv_clock_disable(dev, WM_RCC_CLOCK_ALL);
        wm_drv_clock_enable(dev, WM_RCC_UART0_GATE_EN | WM_RCC_UART1_GATE_EN |
                   WM_RCC_RF_CFG_GATE_EN | WM_RCC_GPIO_GATE_EN |
                   WM_RCC_SD_ADC_GATE_EN | WM_RCC_TOUCH_GATE_EN); //enable UART, RF, GPIO, SD_ADC, TOUCH clocks
        wm_drv_rcc_config_clock(dev, WM_RCC_TYPE_CPU, 240);  //Configure CPU clock to 240MHz
    }

API Reference
---------------
:ref:`label_api_rcc_drver`
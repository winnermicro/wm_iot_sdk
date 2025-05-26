.. _lable_device_table:

Device Table
==============

Overview
-------------

The device table currently supports two usage methods:
    * Configuration using TOML files, which is the **recommended** method
    * Configuration using C language files, which is the traditional method

Users can choose either method, with TOML file configuration being the default.
The current device table configuration process is illustrated in the following figure:

.. figure:: ../../../_static/component-guides/device_table/wm_device_table_en.svg
    :align: center
    :alt: Device Table Configuration

Using TOML Files Configuration 
-----------------------------------

`TOML <https://toml.io/en>`_ files are configuration files in a text format that are semantically clear and easy to read, and they support adding comments, making them very user-friendly.

TOML files have a complete syntax specification, and the current syntax details can be found at:

.. toctree::
   :maxdepth: 1

   TOML v1.0.0-rc.2 <toml_spec_v1.0.0-rc.2>

The default location for the TOML configuration file used by the WM IoT SDK is ``components/wm_dt/config/device_table.toml``. If a ``device_table.toml`` file exists in the root directory of the project, the file in the project root directory will be used preferentially.

When you need to modify the configuration file, it is recommended not to modify the default ``device_table.toml`` file in the component directory directly. Instead, it is recommended to copy it to the project root directory and modify the ``device_table.toml`` file in the project root directory.

The ``device_table.toml`` file can be edited with any text editor, but some editors may require a plugin to support syntax highlighting, which will not be covered here.

When the build is initiated, the build system will first check if there is a ``device_table.toml`` file in the root directory of the project. If there is , it will copy this ``device_table.toml`` file as ``build/device_table/device_table.toml``, and  afterwards, it will **always** use the ``build/device_table/device_table.toml`` file as the current configuration. The scripts will use the ``build/device_table/device_table.toml`` file as the current configuration.The script will convert the ``build/device_table/device_table.toml`` file into C code for compilation, which will be in the ``build/device_table/`` path. 
If the file ``device_table.toml`` does not exist in the root directory of the project, choose to copy ``components/wm_dt/config/<soc>/device_table.toml`` to ``build/device_table/device_table. toml``, and the subsequent process will be the same as described above.

When development is complete, you can use the command ``wm.py saveconfig`` to save the currently tuned device table configuration to the ``device_table.toml`` file in the project root directory.

"Web-based Configuration System for Device Table
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To further simplify the configuration of the ``device_table.toml`` file, a web page system is provided. Through this system, users can check, enter, and modify settings directly on the web page, without needing to concern themselves with the TOML file specification or the device properties in the SDK.

The web page can be automatically opened with the command ``wm.py devconfig``.
Users in non-desktop environments will not be able to use this command and will instead need to edit  the ``device_table.toml`` file manually or directly in C code.

The content displayed in the web page is affected by the SoC type and driver options in the menuconfig menu, and may be different for different SoC types or truncated drivers.

.. note::
    The menuconfig system uses a keep-alive checking mechanism that automatically stops the menuconfig system after a page has been inactive for more than 5 seconds.
    When the browser loses focus, is minimized, or similar situations occur,, different systems and browsers may fail to keep alive due to their control policies.

TOML Configuration File Format
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The ``device_table.toml`` file used by the WM IoT SDK adopts of the following format (partial excerpt):

.. code:: toml

    # WM IoT SDK Device Table
    table_name = "default"

    [[dev]]
    dev_name = "uart0"
    reg_base = 0x40010600
    init_cfg = {init_level = "system", init_priority = 50}
    uart_cfg = {baudrate = 115200, parity = "none", stop_bits = 1, data_bits = 8, flow_ctrl = "none"}
    irq_cfg  = {irq_num = 16, irq_priority = 0}
    pin_cfg  = [{pin = 35, func = "fun1"}, #tx
                {pin = 36, func = "fun1"}, #rx
                {pin = 37, func = "fun1"}, #rts
                {pin = 38, func = "fun1"}] #cts
    dma_device = "dma"

    [[dev]]
    dev_name = "timer0"
    reg_base = 0x40011800
    init_cfg = {init_level = "app", init_priority = 50}
    irq_cfg  = {irq_num = 30, irq_priority = 0}

    [[dev]]
    dev_name = "seg_lcd"
    reg_base = 0x40011C00
    init_cfg = {init_level = "app", init_priority = 50}
    seg_lcd_cfg = {duty_sel = 3, vlcd_cc = 0, bias = 2, hd = 0, frame_freq = 60, com_num = 4}

    [[dev]]
    dev_name = "gdc0689"
    init_cfg = {init_level = "app", init_priority = 50}
    pin_cfg  = [{pin = 42, func = "fun6"},
                {pin = 40, func = "fun6"},
                {pin = 7,  func = "fun6"},
                {pin = 8,  func = "fun6"},
                {pin = 25, func = "fun6"},
                {pin = 26, func = "fun6"},
                {pin = 27, func = "fun6"},
                {pin = 28, func = "fun6"},
                {pin = 29, func = "fun6"}]
    seg_lcd_device = "seg_lcd"

Device Table Options Introduction
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Common information:

    * ``table_name`` specifies the name of the device table corresponding to this configuration file. This option is optional and takes a string value.

      This option is useful in scenarios where multiple device table configurations are used .It can be ignored in other cases.

    * ``[[dev]]`` indicates a  specific device configuration, each device starts with this marker.
    * ``dev_name`` indicates the name of the device and takes a string value.This name is used to identify the device in the code..
    * ``reg_base`` indicates the base address of the device's register and take a hexadecimal integer value.
    * ``irq_cfg`` indicates the interrupt information for device, which includes two member options:
        * ``irq_num`` indicates the interrupt number, taking a decimal integer value within the range of 0 - 31.
        * ``irq_priority`` indicates the interrupt priority, taking a decimal integer value within the range of 0 - 3. The higher the value, the higher the priority.
    * ``init_cfg`` indicates the initialization configuration of the device, containing two member options:
        * ``init_level`` indicates the initialization method, taking a string value,

          A value of ``system`` indicates that the device is initialized automatically by the system,

          A value of ``app`` indicates that the device is initialized by the user.
        * ``init_priority`` indicates the initialization priority, taking a decimal integer value within the range of 0 - 100.

          The higher the value, the higher the priority. Devices with the same priority are randomly initialized at that priority level.

    .. _label_pincfg:
    
    * ``pin_cfg`` Indicates the pin configuration information of the device, which contains four member options:
        * ``pin`` Indicates the pin numberand and is a required option. It takes a decimal integer value in the range of 0 to 45.
        * ``fun`` Indicates the pin multiplexing function and is a required option. It takes a string value and can be

           ``fun1``, ``fun2``, ``fun3``, ``fun4``, ``fun5``, ``fun6``, ``fun7``.

          Refer to :ref:`pinmux` for specific multiplexing relationships.
        * ``dir`` represents the input/output attribute of the pin and is an optional option. The default is the input attribute when omitted

           It can take values ``input`` and ``output`` to represent input and output respectively.
        * ``pupd`` Indicates the pull-up/pull-down attribute, optional of the pin and is an optional option. The default is the floating attribute when omitted. 

           It can take values  ``float``, ``pullup``, and ``pulldown``, which means hover, pullup, and pulldown respectively.

Optional options can be omitted from the configuration file. However, if it is filled in, the format must meet the requirements.

.. _label_back:

Device-specific information:

    * ``rcc_cfg`` indicates the RCC configuration information, with its specific options introduced in the :ref:`rcc_config_section` section.
    * ``gpio_cfg`` indicates the GPIO configuration information, with its specific options introduced in the :ref:`gpio_config_section` section.
    * ``seg_lcd_cfg`` indicates the segment LCD configuration information, with its specific options introduced in the :ref:`seglcd_config_section` section.
    * ``uart_cfg`` indicates the UART configuration information, with  its specific options introduced in the :ref:`uart_config_section` section.
    * ``i2c_cfg`` indicates the I2C configuration information, with its specific options introduced in the :ref:`i2c_config_section` section.
    * ``pmu_cfg`` indicates the PMU configuration information, with its specific options introduced in the :ref:`pmu_config_section` section.
    * ``touch_button_cfg`` indicates the touch button configuration information, with its specific options introduced in the :ref:`touch_button_config_section` section.
    * ``adc_cfg`` indicates the ADC configuration information, with its specific options introduced in the :ref:`adc_config_section` section.
    * ``eeprom_cfg`` indicates the EEPROM configuration information, with its specific options introduced in the :ref:`eeprom_config_section` section.
    * ``spi_cfg`` indicates the SPI configuration information, with its specific options introduced in the :ref:`spi_config_section` section.
    * ``i2s_cfg`` indicates the I2S configuration information, with its specific options introduced in the :ref:`i2s_config_section` section.
    * ``sdh_cfg`` indicates the SD/MMC configuration information, with its specific options introduced in the :ref:`sdmmc_config_section` section.
    * ``tftlcd_cfg`` indicates the TFT LCD configuration information, with its specific options introduced in the :ref:`tftlcd_config_section` section.
    * ``wdt_cfg`` indicates the watchdog configuration information, with its specific options introduced in the :ref:`wdt_config_section` section.
    * ``psram_cfg`` indicates the PSRAM configuration information, with its specific options introduced in the :ref:`psram_config_section` section.
    * ``flash_cfg`` indicates the internal and external Flash configuration information, with its specific options introduced in the :ref:`flash_config_section` section.

    * ``dma_device`` indicates that this device requires DMA and specifies the name of the DMA controller it uses, taking a string value.
        - Whether this item is present determines whether the DMA function is enabled. For example, if omitted in UART configuration, the UART driver will not enable the DMA function.
    * ``irq_device`` indicates that this device requires IRQ and specifies the name of the IRQ controller it uses, taking a string value.
    * ``rcc_device`` indicates that this device requires RCC and specifies the name of the RCC controller it uses, taking a string value.
    * ``seg_lcd_device`` indicates that this device requires a segment LCD and specifies the name of the segment LCD controller it uses, taking a string value.
    * ``pinmux_device`` indicates that this device requires a PINMUX and specifies the name of the PINMUX controller it uses, taking a string value.
    * ``touch_sensor_device`` indicates that this device requires a touch sensor and specifies the name of the touch sensor controller it uses, taking a string value.
    * ``i2c_device`` indicates that this device requires I2C and specifies the name of the I2C controller it uses, taking a string value.
    * ``spi_device`` indicates that this device requires SPI and specifies the name of the SPI controller it uses, taking a string value.
    * ``gpio_device`` indicates that this device requires GPIO and specifies the name of the GPIO controller it uses, taking a string value.

.. _rcc_config_section:

:ref:`RCC Configuration <label_back>`
+++++++++++++++++++++++++++++++++++++++++++++

For the RCC module, its configuration options in the configuration file under ``rcc_cfg`` include the following options:

    * ``type`` indicates the clock type for different modules in the system, possible values are ``peripheral``, ``wlan``, ``cpu``, ``adc``, ``qflash``, ``gpsec``, ``rsa``, ``apb``.
    * ``clock`` indicates the clock frequency for each bus or controller, taking a decimal integer value in MHz.
        - SD ADC clock: possible value is 1.
        - WLAN clock: possible value is 160 (only 160 MHz WiFi can work normally).
        - CPU clock: possible values are 240, 160, 120, 96, 80, 60, 48, 40, 32, 30, 24, 20, 16, 15, 12, 10, 8, 6, 5, 4, 3, 2.
        - QFLASH clock: possible value is 80.
        - GPSEC clock: possible value is 160.
        - RSA clock: possible value is 160.
        - APB clock: the maximum configurable value is 40, generally not recommended to configure lower.

.. _gpio_config_section:

:ref:`GPIO Configuration <label_back>`
+++++++++++++++++++++++++++++++++++++++++

For the GPIO module, its configuration options in the configuration file under ``gpio_cfg`` include the following options:

    * ``pin_num``, ``pin_mux``, ``pin_dir``, ``pin_pupd`` can refer to :ref:`pin_cfg <label_pincfg>`.
    * ``int_mode`` indicates the interrupt trigger mode for configuring the GPIO pin, possible values are ``falling_edge``, ``rising_edge``, ``double_edge``, ``low_level``, ``high_level``.

.. _seglcd_config_section:

:ref:`SEG LCD Configuration <label_back>`
++++++++++++++++++++++++++++++++++++++++++

For the segment LCD device, its configuration options in the configuration file under ``seg_lcd_cfg`` include the following options:

    * ``duty_sel`` indicates the duty cycle of the LCD display, possible values are fractions, such as ``static``, ``1/2``, ``1/3``, ``1/4``, ``1/5``, ``1/6``, ``1/7``, ``1/8``.
    * ``vlcd_cc`` indicates the voltage selection for the LCD display, taking a floating-point value with units, possible values are ``2.7v``, ``2.9v``, ``3.1v``, ``3.3v``.
    * ``bias`` indicates the bias voltage for the LCD, possible values are fractions, such as ``1/4``, ``1/2``, ``1/3``, ``static``.
    * ``hd`` indicates the high-level state of the LCD driver, taking a decimal integer value, possible values are ``low`` and ``high``.
    * ``frame_freq`` indicates the refresh frame frequency of the LCD, taking a decimal integer value.
    * ``com_num`` indicates the number of common terminals for the LCD, taking a decimal integer value, possible values are ``1``, ``2``, ``3``, ``4``, ``5``, ``6``, ``7``, ``8``.


.. _uart_config_section:

:ref:`UART Configuration <label_back>`
++++++++++++++++++++++++++++++++++++++++++

For a UART device, the configuration options ``uart_cfg`` in the configuration file contain five member options:

    * ``baudrate`` indicates the UART baud rate, taking a decimal integer value, possible values are ``600``, ``1200``, ``1800``, ``2400``, ``4800``, ``9600``, ``19200``, ``38400``, ``57600``, ``115200``, ``230400``, ``460800``, ``921600``, ``1000000``, ``1250000``, ``1500000``, ``2000000``.
    * ``parity`` indicates the UART parity, taking a string value, possible values are ``none``, ``even``, and ``odd``.
    * ``stop_bits`` indicates the UART stop bits, taking a decimal integer value, possible values are ``1`` and ``2``.
    * ``data_bits`` indicates the UART data bits, taking a decimal integer value, possible values are ``5``, ``6``, ``7``, and ``8``.
    * ``flow_ctrl`` indicates the UART flow control option, taking a string value, possible values are ``none``, ``rts``, ``cts``, and ``rts_cts``.

.. _i2c_config_section:

:ref:`I2C Configuration <label_back>`
++++++++++++++++++++++++++++++++++++++++++

For the I2C device, its configuration options in the configuration file under ``i2c_cfg`` include the following options:

    * ``max_clock`` indicates the maximum clock frequency of the I2C bus (unit: Hz), taking a decimal integer value.
    * ``addr_10_bits`` indicates whether the 10-bit I2C address mode is enabled, possible values are boolean: ``true``, ``false``. Only 7-bit addresses are supported on the SOC of WM800 series.

.. _pmu_config_section:

:ref:`PMU Configuration <label_back>`
++++++++++++++++++++++++++++++++++++++

For the PMU, its configuration options in the configuration file include the following option:

    * ``clk_src`` indicates the clock source selection, controlling the clock source for different modules in the system. Possible values are ``internal`` and ``external``.

        - internal: Uses an internal 32 KHz crystal oscillator as the clock source.
        - external: Uses an external 40 MHz crystal oscillator with a divided clock source.

.. _touch_button_config_section:

:ref:`Touch Button Configuration <label_back>`
+++++++++++++++++++++++++++++++++++++++++++++++++

For the touch button module, its configuration options in the configuration file under ``touch_button`` include the following two member options:

    * ``key_num`` indicates the touch button number, taking a decimal integer value, within the range of 1 - 12, 15.
    * ``threshold`` indicates the touch detection threshold, taking a decimal integer value, within the range of 0 - 127.

.. _adc_config_section:

:ref:`ADC Configuration <label_back>`
++++++++++++++++++++++++++++++++++++++

For the ADC device, its configuration options in the configuration file under ``adc_cfg`` include the following members:

    * ``adc_channel`` specifies the ADC channel to select the input signal for sampling. Possible values are ``0``, ``1``, ``2``, ``3``, ``8``, ``9``.

        - 0: Use ADC channel 0.
        - 1: Use ADC channel 1.
        - 2: Use ADC channel 2.
        - 3: Use ADC channel 3.
        - 8: Use the differential of ADC channel 0 and ADC channel 1.
        - 9: Use the differential of ADC channel 2 and ADC channel 3.

    * ``pga_gain1`` specifies the configuration for the first gain stage of the Programmable Gain Amplifier (PGA), in string format, with possible values of ``level0``, ``level1``, ``level2``, ``level3``, ``level4``, ``level5``.
    * ``pga_gain2`` specifies the configuration for the second gain stage of the PGA, in string format, with possible values of ``level0``, ``level1``, ``level2``, ``level3``.
    * ``adc_cmp`` specifies whether to enable or disable the ADC comparator function, possible boolean values: ``true``, ``false``.
    * ``cmp_data`` specifies the comparison value as a decimal integer, valid when ``adc_cmp`` is ``true``. It can be omitted when ``adc_cmp`` is ``false``.
    * ``cmp_pol`` specifies the interrupt trigger mode for the ADC comparator function, with possible decimal integer values of 0 or 1.

        - 0: Interrupt is triggered when the ADC result is greater than or equal to cmp_data.
        - 1: Interrupt is triggered when the ADC result is less than cmp_data.

.. _eeprom_config_section:

:ref:`EEPROM Configuration <label_back>`
+++++++++++++++++++++++++++++++++++++++++

For the EEPROM device, its configuration options in the configuration file include the following:

    * ``speed_hz`` specifies the I2C communication speed for the EEPROM, in Hertz (Hz), taking a decimal integer value.
    * ``size`` Total size of EEPROM, in bytes, taking a decimal integer value.
    * ``i2c_addr`` specifies the I2C address of the EEPROM, taking a hexadecimal integer value.
    * ``page_size`` specifies the size of the EEPROM pages, determining the number of bytes written in each operation, taking a decimal integer value.
    * ``addr_width`` specifies the width of the EEPROM address, taking a decimal integer value.
    * ``read_only`` specifies whether the EEPROM is read-only, possible boolean values: ``true``, ``false``.
    * ``max_write_time_ms`` sets the maximum write delay for EEPROM operations to ensure the write completes, in milliseconds (ms), taking a decimal integer value.
    * ``pin_cfg`` specifies the configuration for special pins (e.g., write protection), which can be omitted if not used, in the same format as :ref:`pin_cfg <label_pincfg>`.

.. _spi_config_section:

:ref:`SPI Configuration <label_back>`
+++++++++++++++++++++++++++++++++++++++

For devices using SPI, the configuration options in the configuration file under ``spi_cfg`` include the following:

    * ``mode`` specifies the SPI operating mode, controlling the clock polarity (CPOL) and phase (CPHA). Different peripherals may require different modes for proper communication, with possible values of ``0``, ``1``, ``2``, ``3``.

        - 0: CPOL=0, CPHA=0
        - 1: CPOL=0, CPHA=1
        - 2: CPOL=1, CPHA=0
        - 3: CPOL=1, CPHA=1

    * ``freq`` specifies the working frequency of the SPI device, in Hertz (Hz), taking a decimal integer value.
    * ``pin_cs`` specifies the configuration for the chip select pin of the device, with configuration information referenced from :ref:`pin_cfg <label_pincfg>`.

.. _i2s_config_section:

:ref:`I2S Configuration <label_back>`
+++++++++++++++++++++++++++++++++++++++

For I2S, the configuration options in the configuration file under ``i2s_cfg`` include the following:

    * ``extal_clock_en`` specifies whether to enable the external clock source, possible boolean values: ``true``, ``false``.
    * ``mclk_hz`` specifies the master clock frequency (MCLK) for the I2S interface, in Hertz (Hz), taking a decimal integer value.

.. _sdmmc_config_section:

:ref:`SD/MMC Configuration <label_back>`
++++++++++++++++++++++++++++++++++++++++++

For SD/MMC devices, their configuration options wtthin the configuration file under ``sdh_cfg`` include the following:

    * ``speed_hz`` specifies the clock frequency for SD/MMC, used to determine the data transfer speed, taking a decimal integer value in Hertz (Hz), supporting multiples of the CPU clock frequency such as 1/2, 1/4, 1/6, 1/8, 1/10, 1/12, 1/14, 1/16, etc.
    * ``bus_width`` specifies the SD/MMC bus width, taking a decimal integer value, with possible values of ``1`` or ``4``.

.. _tftlcd_config_section:

:ref:`TFT LCD Configuration <label_back>`
+++++++++++++++++++++++++++++++++++++++++

For TFT LCD, the configuration options include the following:

    * ``pin_lcd_reset`` specifies the reset pin for the TFT LCD screen, used to reset the display module. It takes a GPIO pin number, which can be any number from ``0`` to ``WM_GPIO_NUM_MAX-1``.
    * ``pin_lcd_dcx`` specifies the data/command select pin for the TFT LCD screen. It takes a GPIO pin number, which can be any number from ``0`` to ``WM_GPIO_NUM_MAX-1``.
    * ``pin_lcd_led`` specifies the backlight control pin for the TFT LCD screen. It takes a GPIO pin number, which can be any number from ``0`` to ``WM_GPIO_NUM_MAX-1``.

.. hint::
    During the build process, the build script ``wmdt.py`` will first convert ``device_table.toml`` into ``wm_dt_hw.c``. The ``pin_lcd_*`` entries in ``device_table.toml`` will be converted to ``io_lcd_*`` and stored in ``wm_dt_hw.c``.
    If ``pin_lcd_*`` is set to ``-1`` in ``device_table.toml``, it will be converted to ``io_lcd_* = WM_GPIO_NUM_MAX`` in ``wm_dt_hw.c``.
    When ``io_lcd_* = WM_GPIO_NUM_MAX``, the corresponding IO will be ignored during TFT LCD driver initialization, and IO initialization will not be performed for that pin.

.. _wdt_config_section:

:ref:`WDT Configuration <label_back>`
++++++++++++++++++++++++++++++++++++++

For the Watchdog Timer (WDT), the configuration options in the configuration file under ``wdt_cfg`` include the following:

    * ``counter_value`` specifies the watchdog timer count value, i.e., the timer cycle before a reset is triggered. It takes a decimal integer value in microseconds.

.. _psram_config_section:

:ref:`PSRAM Configuration <label_back>`
++++++++++++++++++++++++++++++++++++++++++

For a Pseudo-Static Random Access Memory (PSRAM), the configuration options in the configuration file under ``psram_cfg`` include the following:

    * ``qspi`` specifies whether to enable Quad SPI mode, possible boolean values: ``true``, ``false``.
    * ``clock_hz`` specifies the clock frequency for PSRAM, in Hertz (Hz), taking a decimal integer value. The value is a 3-15 division of the CPU's main frequency.

.. _flash_config_section:

:ref:`FLASH onfiguration <label_back>`
++++++++++++++++++++++++++++++++++++++++++

For a FLASH storage device, the configuration options in the configuration file under ``flash_cfg`` include the following:

    * ``quad_spi`` specifies whether to enable Quad SPI mode, possible boolean values: ``true``, ``false``.


Configuration using C Language Files
----------------------------------------

The C-style configuration can be enabled through the ``menuconfig`` menu under ``Components configuration  ---> Device Table  ---> Use c style configuration``. Once this option is enabled, the TOML configuration file is automatically disabled.

The default location for the  C-style configuration files currently used by the WM IoT SDK is ``components/wm_dt/config/wm_dt_hw.c`` and ``components/wm_dt/config/wm_dt_hw.h``.

When you need to modify the configuration file, it is not recommended to modify the default files in the component directory directly. Instead, it is recommended to copy a copy from the component directory to the project directory (e.g., copy to the ``main/dt`` directory), and then modify the ``CMakeLists.txt`` file in the ``main`` component of the project directory to add the new configuration files.

.. code:: cmake

    set(ADD_DT_C_FILES "dt/wm_dt_hw.c")
    set(ADD_DT_H_FILES "dt")

If you want to use multiple configuration files, you can set it as follows:

.. code:: cmake

    set(ADD_DT_C_FILES "dt/dt_config1.c"
                       "dt/dt_config2.c"
                       )

    set(ADD_DT_H_FILES "dt"
                       )

Please note that when adding configuration files to the project directory, the ``wm_dt_hw.h`` file name cannot be changed.
The ``.c`` file can be renamed arbitrarily.
For multiple configuration files, you need to change the internal name of the configuration in the file so that each configuration has a different internal name.
For example, ``dt_config1.c`` configuration internal name is ``my_config1``:

::

    WM_DT_TABLE_DEFINE(my_config1, (sizeof(dt_hw_table_entry) / sizeof(dt_hw_table_entry[0])), (void *)&dt_hw_table_entry[0]);

For example, `dt_config2.c` configures internal name is `my_config2`:

::

    WM_DT_TABLE_DEFINE(my_config2, (sizeof(dt_hw_table_entry) / sizeof(dt_hw_table_entry[0])), (void *)&dt_hw_table_entry[0]);


Format of C Language Configuration File 
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The C-style configuration files used by the WM IoT SDK consist of the following format (partial excerpt):

.. code:: c

    typedef struct {
        uint8_t init_level;
        uint8_t init_priority;
    } wm_dt_hw_init_cfg_t;
    
    typedef struct {
        uint8_t irq_num; /**< @ref wm_irq_no_t */
        uint8_t irq_priority;
    } wm_dt_hw_irq_cfg_t;
    
    typedef struct {
        uint8_t pin_num;  /**< @ref wm_gpio_num_t */
        uint8_t pin_mux;  /**< @ref wm_gpio_pin_mux_t */
        uint8_t pin_dir;  /**< @ref wm_gpio_dir_t */
        uint8_t pin_pupd; /**< @ref wm_gpio_pupd_t */
    } wm_dt_hw_pin_cfg_t;
    
    typedef struct {
        int baudrate;      /**< @ref wm_uart_baudrate_t */
        uint8_t parity;    /**< @ref wm_uart_parity_t */
        uint8_t stop_bits; /**< @ref wm_uart_stop_bits_t */
        uint8_t data_bits; /**< @ref wm_uart_data_bits_t */
        uint8_t flow_ctrl; /**< @ref wm_uart_flowctrl_t */
    } wm_dt_hw_uart_cfg_t;
    
    typedef struct {
        char *device_name;
        wm_dt_hw_init_cfg_t init_cfg;
        uint32_t reg_base;
        wm_dt_hw_irq_cfg_t irq_cfg;
        wm_dt_hw_uart_cfg_t uart_cfg;
        uint8_t pin_cfg_count;
        wm_dt_hw_pin_cfg_t *pin_cfg;
        char *dma_device_name;
    } wm_dt_hw_uart_t;
    
    typedef struct {
        char *device_name;
        wm_dt_hw_init_cfg_t init_cfg;
        uint32_t reg_base;
        wm_dt_hw_irq_cfg_t irq_cfg;
    } wm_dt_hw_timer_t;

.. code:: c

    const static wm_dt_hw_pin_cfg_t dt_hw_uart0_pin[4] = {
        [0] = { .pin_num = WM_GPIO_NUM_35, .pin_mux = WM_GPIO_IOMUX_FUN1, .pin_dir = WM_GPIO_DIR_INPUT, .pin_pupd = WM_GPIO_FLOAT },
        [1] = { .pin_num = WM_GPIO_NUM_36, .pin_mux = WM_GPIO_IOMUX_FUN1, .pin_dir = WM_GPIO_DIR_INPUT, .pin_pupd = WM_GPIO_FLOAT },
        [2] = { .pin_num = WM_GPIO_NUM_37, .pin_mux = WM_GPIO_IOMUX_FUN1, .pin_dir = WM_GPIO_DIR_INPUT, .pin_pupd = WM_GPIO_FLOAT },
        [3] = { .pin_num = WM_GPIO_NUM_38, .pin_mux = WM_GPIO_IOMUX_FUN1, .pin_dir = WM_GPIO_DIR_INPUT, .pin_pupd = WM_GPIO_FLOAT },
    };
    
    const static wm_dt_hw_uart_t dt_hw_uart0 = {
        .device_name     = "uart0",
        .init_cfg        = { .init_level = 0, .init_priority = 50 },
        .reg_base        = 0x40010600,
        .irq_cfg         = { .irq_num = WM_IRQ_UART0, .irq_priority = 0 },
        .uart_cfg        = { .baudrate  = WM_UART_BAUDRATE_B115200,
                            .parity    = WM_UART_PARITY_NONE,
                            .stop_bits = WM_UART_STOP_BIT_1,
                            .data_bits = WM_UART_DATA_BIT_8,
                            .flow_ctrl = WM_UART_FLOW_CTRL_DISABLE },
        .pin_cfg_count   = sizeof(dt_hw_uart0_pin) / sizeof(dt_hw_uart0_pin[0]),
        .pin_cfg         = (wm_dt_hw_pin_cfg_t *)dt_hw_uart0_pin,
        .dma_device_name = "dma",
    };
    
    const static wm_dt_hw_uart_t dt_hw_timer0 = {
        .device_name = "timer0",
        .init_cfg    = { .init_level = 0,         .init_priority = 50 },
        .reg_base    = 0x40011800,
        .irq_cfg     = { .irq_num = WM_IRQ_TIMER, .irq_priority = 0   },
    };
    
The options in the  C-style configuration files are intuitive and will not be further explained here.

.. note::
    Be sure to carefully configure ``pin_cfg`` to avoid definitions that exceed the supported range of the current SOC or definition conflicts.

    For example: the defined GPIO_NUM might be outside the range supported by the current SOC, or one pin might be defined by multiple devices.

    For more details, refer to :ref:`pinmux`.

Using the Device Table
------------------------

Generally, when using it, users only need to use ``wm_device_t *wm_dt_get_device_by_name(const char *device_name)`` to obtain the device from the device table for use.

.. code:: c

    typedef struct {
        char *name; /**< device name */
    
        void *hw; /**< hardware info, ref wm_dt_hw_xxx_t */
    
        void *ops; /**< device operation interface */
        void *drv; /**< driver context data */
    
        wm_device_status_t state; /**< device state */
    
        void *priv; /**< user private data */
    } wm_device_t;

The ``hw`` pointer points to the information in the configuration file and is used by each driver module itself.

For scenarios using multiple configuration files, they can be selected based on the preset name in the configuration file by using the ``int wm_dt_set_device_table_name(const char *default_name)`` interface in the code.

The call to the wm_dt_set_device_table_name interface needs to be executed as early in the process as possible, so it is recommended to use ``WM_COMPONEN_INIT_0`` to set up an initialization function, and then call wm_dt_set_device_table_name in this initialization function. COMPONEN_INIT_0, please refer to :ref:`label-components_autoinit` section.

Adding New Devices
----------------------

Using TOML Configuration Files
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Add new devices to wmdt.py
+++++++++++++++++++++++++++++

It is necessary to modify both the TOML configuration file and the ``tools/wm/wmdt.py`` file.

You can add the required options in the TOML file as needed, and then add the parsing code for the new device options in the ``wmdt.py`` file.

In ``wmdt.py``, you can modify:
    * Add the header files required by the hw structure in the ``wm_dt_hw_c_include`` variable, if any, otherwise no need to add.
    * Add common data structure definitions in the ``generate_common`` method, if any, otherwise no need to add.  
    * Add the parsing code for the new device in the ``generate_file`` method, referring to the ``i2s`` code.

.. code:: python

    def generate_i2s(self):  # Core function: Generate I2S configuration

        members = '''
        typedef struct {
            bool extal_clock_en;     /**< @ref wm_drv_i2s_ioctl_args_t */
            uint32_t mclk_hz;        /**< @ref wm_drv_i2s_ioctl_args_t */
        } wm_dt_hw_i2s_cfg_t;  /* Defines specific I2S configuration, not required if not needed */

        typedef struct {
            wm_dt_hw_init_cfg_t init_cfg;

            uint32_t reg_base;
            wm_dt_hw_irq_cfg_t irq_cfg;

            uint8_t pin_cfg_count;
            wm_dt_hw_pin_cfg_t *pin_cfg;

            wm_dt_hw_i2s_cfg_t i2s_cfg;

            char *dma_device_name;
            char *rcc_device_name;
        } wm_dt_hw_i2s_t;  /* This section will be added to the header file */
        '''
        base_name = "i2s"  # Defines the base name of the device for subsequent matching and code generation.

        self.update_dt_hw_h(members)  # Update the device header file by appending the hardware configuration structure.

        self.update_dt_hw_c_ops_def(base_name)  # Update device operation definitions by linking the base name and configuration.

        for item in self.config:
            dev_name = item["dev_name"]
            if base_name == dev_name:  # Loop through each device configuration in the toml file and generate corresponding C code content.

                self.update_dt_hw_c_pin_cfg(dev_name, item)  # Generate the pin_cfg structure in C code, optional.

                self.update_dt_table(dev_name, base_name, item)  # Update the device table by linking the device name with the ops.
                self.update_dt_hw_c_begin(dev_name)  # Begin generating device initialization code, mandatory.
                self.update_dt_hw_c_init_cfg(item)   # Generate the init_cfg section in the C structure, mandatory.
                self.update_dt_hw_c_irq_cfg(item)    # Generate the irq_cfg section in the C structure, optional.
                # For specific configurations, access the dictionary item ["i2s_cfg"] to dynamically generate the i2s_cfg part of the C structure, optional.
                self.wm_dt_hw_c = (
                    self.wm_dt_hw_c
                    + "    .i2s_cfg = { .extal_clock_en = "
                    + str(item["i2s_cfg"]["extal_clock_en"]).lower()
                    + ", .extal_clock_hz = "
                    + str(item["i2s_cfg"]["mclk_hz"])
                    + " },\n"
                )
                self.update_dt_hw_c_pin_cfg_link(dev_name)  # Link the pin_cfg in the C structure to a specific variable, optional.
                self.update_dt_hw_c_dma_device(item)        # Link the dma_device_name in the C structure to a specific name, optional.
                self.update_dt_hw_c_irq_device(item)        # Link the irq_device_name in the C structure to a specific name, optional.
                self.update_dt_hw_c_rcc_device(item)        # Link the rcc_device_name in the C structure to a specific name, optional.
                self.update_dt_hw_c_end()  # Complete the device initialization code generation, mandatory.
    
.. code:: python

    def generate_file(self):
        ......
        wmdt.generate_i2s()  # Add the processing code for the new device to generate_file
        ......
      
* For some common data structures, the script already has operation methods. You can refer to the handling of ``i2s`` without adding new code.

.. code:: python

    # Example: A common data structure update_dt_hw_c_rcc_device has been defined
    def update_dt_hw_c_rcc_device(self, item):
        if "rcc_device" in item:
            self.wm_dt_hw_c = self.wm_dt_hw_c + "    .rcc_device = \"" + item["rcc_device"] + "\",\n"

    # Directly call update_dt_hw_c_rcc_device in the configuration item
    self.update_dt_hw_c_rcc_device(item)
    
When the modification is complete, run the ``wm.py build`` command and the build system will automatically call the ``wmdt.py`` script to convert the toml file to a c file. If there are no errors, the parsed C configuration file will be generated in the ``build/device_table`` folder, if there are errors, you can follow the prompts to deal with them.

Adding a New Device to the Web Interface
++++++++++++++++++++++++++++++++++++++++++

The web configuration module uses the Flask framework, and the Jinja2 template can conveniently pass variables for device configuration. The source code is located in the ``tools/devconf`` path.
When adding a new device to the web interface, the following steps are required:

    - Modify the devconf.py script
    - Modify the config_helper.py script
    - Add an HTML page (optional)

Taking the addition of a new I2S device as an example:

In the ``config_helper.py`` configuration management class ``ConfigHelper``, you need to modify:

    * In the ``__init__`` function, add initialization information for the I2S device. The main tasks are:
        - Read the configuration from the toml file into the local configuration structure
        - Read the trimming configuration from the kconfig file

.. code:: python

        def __init__(self, *args):
            # Initialize I2S configuration dictionary, defining parameters and default values for the device
            self.i2s_config_data = {'chip_type':'W800', 'dev_name':'I2S', 'extal_clock_en':0, 'extal_clock_hz':0, 'initlevel':0, 'initpriority':0}

            # Traverse the config["dev"] list for each device, adding checks for I2S
                    for item in config["dev"]:
                        dev_name = item["dev_name"]
                        ...
                        # Add information for the I2S device where needed
                        elif dev_name == "i2s":
                            # Retrieve the current I2S device configuration, defaulting to i2s_config_data
                            config_data = self.i2s_config_data

                            # Enable or disable external clock based on i2s_cfg configuration from the file
                            if item["i2s_cfg"]["extal_clock_en"]:
                                config_data["extal_clock_en"] = 1  # Enable external clock
                            else:
                                config_data["extal_clock_en"] = 0  # Disable external clock

                            # Set the external clock frequency
                            config_data["extal_clock_hz"] = item["i2s_cfg"]["mclk_hz"]

                            # Configure clock pin (bclkpinnum) and left/right channel clock pins (lrclkpinnum)
                            config_data["bclkpinnum"] = item["pin_cfg"][0]["pin"]
                            config_data["lrclkpinnum"] = item["pin_cfg"][1]["pin"]

                            # Define known MCLK, DI, and DO pin numbers and their functions
                            mclk_pinnum = [0, 7, 33]
                            mclk_pinfun = ["fun1", "fun3", "fun4"]

                            di_pinnum = [7, 11, 26, 31]
                            di_pinfun = ["fun4", "fun4", "fun1", "fun4"]

                            do_pinnum = [0, 10, 27, 30]
                            do_pinfun = ["fun4", "fun4", "fun1", "fun4"]

                            # Disable MCLK, DI, and DO by default
                            config_data["enable_mclk"] = 0
                            config_data["enable_di"] = 0
                            config_data["enable_do"] = 0

                            # Traverse pin_cfg and enable the respective functions based on pin matching
                            for pin_cfg in item["pin_cfg"]:
                                # Enable MCLK if pin and function match
                                for i in range(0, len(mclk_pinnum)):
                                    if pin_cfg["pin"] == mclk_pinnum[i] and pin_cfg["fun"] == mclk_pinfun[i]:
                                        config_data["enable_mclk"] = 1  # Enable MCLK
                                        config_data["mclkpinnum"] = pin_cfg["pin"]  # Record MCLK pin
                                        break

                                # Enable DI if pin and function match
                                for i in range(0, len(di_pinnum)):
                                    if pin_cfg["pin"] == di_pinnum[i] and pin_cfg["fun"] == di_pinfun[i]:
                                        config_data["enable_di"] = 1  # Enable DI
                                        config_data["dipinnum"] = pin_cfg["pin"]  # Record DI pin
                                        break

                                # Enable DO if pin and function match
                                for i in range(0, len(do_pinnum)):
                                    if pin_cfg["pin"] == do_pinnum[i] and pin_cfg["fun"] == do_pinfun[i]:
                                        config_data["enable_do"] = 1  # Enable DO
                                        config_data["dopinnum"] = pin_cfg["pin"]  # Record DO pin
                                        break

                            # Load additional initialization configuration
                            self.load_init_cfg(item, config_data)

                            # Update I2S configuration data
                            self.i2s_config_data = config_data

            # Add I2S key-value pair in the default kconfig initialization
            kconfig = {......, "i2s":1, ......}
            with open(self.kconfig_file, "r", encoding="utf-8") as file:
                # Add the I2S key-value pair to the dynamically adjusted kconfig
                kconfig = {......, "i2s":0, ......}
                for line in file:
                    line = line.strip()
                # Add options for the selected I2S device
                    elif line == "CONFIG_COMPONENT_DRIVER_I2S_ENABLED=y":
                        kconfig["i2s"] = 1

                self.set_devices_kconfig(kconfig)
                self.i2s_config_data["chip_type"] = chip_type # Record SoC type as well

* Add to the ``set_devices_kconfig`` method:

.. code:: python

        # Add I2S kconfig item to update self.i2s_config_data
        def set_devices_kconfig(self, kconfig):
            self.i2s_config_data["kconfig"] = kconfig

* In the ``set_devices_pin`` method, set pin multiplexing. If the device does not involve pins, it can be omitted:

.. code:: python

            def set_devices_pin(self):
            self.pinmux = {}
            self.pinmux["chip_type"] = self.clock_config_data["chip_type"]
            kconfig = self.clock_config_data["kconfig"]
            self.pinmux["pinmux"] = [{} for i in range(46)]
            for item in self.config["dev"]:
                dev_name = item["dev_name"]
                ...
                # Add I2S item to set pin multiplexing based on the configuration in i2s_config_data
                elif dev_name == "i2s" and kconfig["i2s"]:
                    self.pinmux["pinmux"][item["pin_cfg"][0]["pin"]][dev_name] = "I2S_BCLK"
                    self.pinmux["pinmux"][item["pin_cfg"][1]["pin"]][dev_name] = "I2S_LRCLK"
                    if self.i2s_config_data["enable_mclk"]:
                        self.pinmux["pinmux"].append({dev_name:"I2S_MCLK"})
                    if self.i2s_config_data["enable_di"]:
                        self.pinmux["pinmux"].append({dev_name:"I2S_DI"})
                    if self.i2s_config_data["enable_do"]:
                        self.pinmux["pinmux"].append({dev_name:"I2S_DO"})

* Add the ``get_i2s_config`` method:

.. code:: python

        def get_i2s_config(self, dev_name):
            if dev_name == 'I2S':
                return self.i2s_config_data

* Add the ``set_i2s_config`` method:

.. code:: python

        def set_i2s_config(self, data):
        if data['dev_name'] == 'I2S':
            self.i2s_config_data = data

        if not hasattr(self, 'config'):
            return

        for item in self.config["dev"]:
            if item["dev_name"] == "i2s":
                config_data = self.i2s_config_data
                if config_data["extal_clock_en"]:
                    item["i2s_cfg"]["extal_clock_en"] = True
                else:
                    item["i2s_cfg"]["extal_clock_en"] = False
                item["i2s_cfg"]["mclk_hz"] = config_data["extal_clock_hz"]

                mclk_pinnum = [0, 7, 33]
                mclk_pinfun = ["fun1", "fun3", "fun4"]

                di_pinnum   = [7, 11, 26, 31]
                di_pinfun   = ["fun4", "fun4", "fun1", "fun4"]

                do_pinnum   = [0, 10, 27, 30]
                do_pinfun   = ["fun4", "fun4", "fun1", "fun4"]

                to_pop = []
                for pin_cfg in item["pin_cfg"]:
                    for i in range(0, len(mclk_pinnum)):
                        if pin_cfg["pin"] == mclk_pinnum[i] and pin_cfg["fun"] == mclk_pinfun[i]:
                            to_pop.append(item["pin_cfg"].index(pin_cfg))
                            break
                    for i in range(0, len(di_pinnum)):
                        if pin_cfg["pin"] == di_pinnum[i] and pin_cfg["fun"] == di_pinfun[i]:
                            to_pop.append(item["pin_cfg"].index(pin_cfg))
                            break
                    for i in range(0, len(do_pinnum)):
                        if pin_cfg["pin"] == do_pinnum[i] and pin_cfg["fun"] == do_pinfun[i]:
                            to_pop.append(item["pin_cfg"].index(pin_cfg))
                            break

                for i in reversed(to_pop):
                    item["pin_cfg"].pop(i)

                item["pin_cfg"][0]["pin"] = config_data["bclkpinnum"]
                if config_data["bclkpinnum"] == 4:
                    item["pin_cfg"][0]["fun"] = "fun4"
                elif config_data["bclkpinnum"] == 8:
                    item["pin_cfg"][0]["fun"] = "fun4"
                elif config_data["bclkpinnum"] == 24:
                    item["pin_cfg"][0]["fun"] = "fun1"
                else:#28
                    item["pin_cfg"][0]["fun"] = "fun4"
                item["pin_cfg"][1]["pin"] = config_data["lrclkpinnum"]
                if config_data["lrclkpinnum"] == 1:
                    item["pin_cfg"][1]["fun"] = "fun4"
                elif config_data["lrclkpinnum"] == 9:
                    item["pin_cfg"][1]["fun"] = "fun4"
                elif config_data["lrclkpinnum"] == 25:
                    item["pin_cfg"][1]["fun"] = "fun1"
                else:#29
                    item["pin_cfg"][1]["fun"] = "fun4"

                if config_data["enable_mclk"]:
                    if config_data["mclkpinnum"] == 0:
                        item["pin_cfg"].append({"pin":config_data["mclkpinnum"], "fun":"fun1"})
                    elif config_data["mclkpinnum"] == 7:
                        item["pin_cfg"].append({"pin":config_data["mclkpinnum"], "fun":"fun3"})
                    else:#33
                        item["pin_cfg"].append({"pin":config_data["mclkpinnum"], "fun":"fun4"})

                if config_data["enable_di"]:
                    if config_data["dipinnum"] == 7:
                        item["pin_cfg"].append({"pin":config_data["dipinnum"], "fun":"fun4"})
                    elif config_data["dipinnum"] == 11:
                        item["pin_cfg"].append({"pin":config_data["dipinnum"], "fun":"fun4"})
                    elif config_data["dipinnum"] == 26:
                        item["pin_cfg"].append({"pin":config_data["dipinnum"], "fun":"fun1"})
                    else:#31
                        item["pin_cfg"].append({"pin":config_data["dipinnum"], "fun":"fun4"})

                if config_data["enable_do"]:
                    if config_data["dopinnum"] == 0:
                        item["pin_cfg"].append({"pin":config_data["dopinnum"], "fun":"fun4"})
                    elif config_data["dopinnum"] == 10:
                        item["pin_cfg"].append({"pin":config_data["dopinnum"], "fun":"fun4"})
                    elif config_data["dopinnum"] == 27:
                        item["pin_cfg"].append({"pin":config_data["dopinnum"], "fun":"fun1"})
                    else:#30
                        item["pin_cfg"].append({"pin":config_data["dopinnum"], "fun":"fun4"})

                self.update_init_cfg(config_data, item)

                self.update_config_file() # Write local configuration back to the toml file
                break

* The configuration management tool class ``ConfigHelper`` in ``devconf.py`` needs to be modified:

    * Modify the ``wmdt_index`` method as follows:

.. code:: python

    def wmdt_index():
        method = request.method
        if method == 'POST':
            if request.form.get("dev_name2"):
                dev_name = request.form.get("dev_name2")
            ...
            # For POST request, add or update I2S device parameters configuration
            elif dev_name.startswith('I2S'):
                #data = {'initlevel':0, 'initpriority':0}
                data = configHelper.get_i2s_config(dev_name)
                data['initlevel'] = int(request.form.get("initlevel"))
                data['initpriority'] = int(request.form.get("initpriority"))
                data['extal_clock_en'] = int(request.form.get("extal_clock_en"))
                data['extal_clock_hz'] = int(request.form.get("extal_clock_hz"))
                try:
                    data['enable_mclk'] = int(request.form.get("enable_mclk"))
                except TypeError:
                    data['enable_mclk'] = 0
                try:
                    data['mclkpinnum'] = int(request.form.get("mclkpinnum"))
                except TypeError:
                    data['mclkpinnum'] = 0
                data['bclkpinnum'] = int(request.form.get("bclkpinnum"))
                data['lrclkpinnum'] = int(request.form.get("lrclkpinnum"))
                try:
                    data['enable_di'] = int(request.form.get("enable_di"))
                except TypeError:
                    data['enable_di'] = 0
                try:
                    data['dipinnum'] = int(request.form.get("dipinnum"))
                except TypeError:
                    data['dipinnum'] = 0
                try:
                    data['enable_do'] = int(request.form.get("enable_do"))
                except TypeError:
                    data['enable_do'] = 0
                try:
                    data['dopinnum'] = int(request.form.get("dopinnum"))
                except TypeError:
                    data['dopinnum'] = 0
                data['dev_name'] = dev_name
                configHelper.set_i2s_config(data)

        # For GET request, add data for I2S rendering
        elif dev_name.startswith('I2S'):
            return render_template('index.html', data=configHelper.get_i2s_config(dev_name))

* Modify and add to the html files in templates:
    - Modify the devices.html file to add device names to the left-side device list.
    - Modify the status.html file to add state control for new devices.
    - Add a new i2s.html file to add specific interface configuration options to I2S.
    - Modify the index.html file to include the i2s.html file.

After the modifications are complete, you can run the ``wm.py devconfig`` command in the project directory and configure the devices through the web page.


Using C Language Configuration Files
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

You can refer to the default configuration files and add the  hardware(hw) structure and operation(ops) structure for the new device by referring to the ``uart`` device.

After adding the device data structure, update the device table as follows:

.. code:: c

    const static struct wm_dt_table_entry dt_hw_table_entry[3] = {
        [0] = { .dev_name = "uart0",  .hw_addr = (void *)&dt_hw_uart0,  .ops_addr = (void *)&wm_drv_uart_ops  },
        [1] = { .dev_name = "uart1",  .hw_addr = (void *)&dt_hw_uart1,  .ops_addr = (void *)&wm_drv_uart_ops  },
        [2] = { .dev_name = "timer0", .hw_addr = (void *)&dt_hw_timer0, .ops_addr = (void *)&wm_drv_timer_ops },
    };
    
    WM_DT_TABLE_DEFINE(default, 3, (void *)&dt_hw_table_entry[0]);

When registering with the device table using ``WM_DT_TABLE_DEFINE(name, count, addr)``, you can achieve the purpose of setting multiple configuration files by filling in different names.

C Language Data Structure Rules
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Regardless of the configuration file method used, the C language data structure finally used must follow this convention for the hw structure of each device:

.. code:: c

    typedef struct {
        char *device_name;
        wm_dt_hw_init_cfg_t init_cfg;
        /* more ... */
    } wm_dt_hw_xxx_t;

That is, the first member of the hw structure must be ``init_cfg``.

The ``init_cfg`` represents the initialized configuration information for this device, which will take effect in future releases. It contains two member options:
        * ``init_level`` indicates the initialization method.

          A value of ``1`` indicates that the initialization of this device is done automatically by the system.

          A value of ``0`` indicates that the user should initialize the device by himself.
        * The value of ``init_priority`` indicates the initialization priority when the system completes the initialization of the device automatically, and the value is a decimal integer in the range of 0 - 100, the higher the value, the higher the priority.

          The higher the value, the higher the priority, the higher priority is initialized first. Devices with the same priority are randomly initialized at this priority level.

The ops structure for each device has the following conventions:

.. code:: c

    typedef struct {
        int (*init)(wm_device_t *dev);
        int (*deinit)(wm_device_t *dev);
        /* more ... */
    } wm_drv_xxx_ops_t;

That is, the first two members in the ops structure must be the ``init`` function and the ``deinit`` function.

API Reference
---------------

Refer to :ref:`label_api_device_table`

Device Table meunconfig configuration
-------------------------------------------------

The main configuration is as follows:

.. list-table::
  :widths: 45 50 25 
  :header-rows: 0
  :align: center

  * - Configuration name
    - Configuration description
    - Default values

  * - CONFIG_DT_USE_C_STYLE_CONF
    - Whether to use the C language file configuration
    - N
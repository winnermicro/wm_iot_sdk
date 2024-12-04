
.. _i2c:

****************************
I2C AT Command Set
****************************

-  :ref:`AT+DRVI2CINIT <cmd-DRVI2CINIT>`: Initialize the I2C Host Driver
-  :ref:`AT+DRVI2CRD <cmd-DRVI2CRD>`: Read data from the I2C device
-  :ref:`AT+DRVI2CWRDATA <cmd-DRVI2CWRDATA>`: Write data to the I2C device
-  :ref:`AT+DRVI2CWRBYTES <cmd-DRVI2CWRBYTES>`: Write up to 4 bytes of data to the I2C device


.. _cmd-DRVI2CINIT:

:ref:`AT+DRVI2CINIT <cmd-DRVI2CINIT>`: Initialize the I2C Host Driver
-------------------------------------------------------------------------------------------

Set command
^^^^^^^^^^^

**Command:**

::

    AT+DRVI2CINIT=<num>,<scl_io>,<sda_io>,<clock>

**Response:**

::

    OK

Parameter
^^^^^^^^^^

-  **<num>**: I2C port number, range: it can only be 0. If the following parameters are not set, AT will cancel the initialization of the I2C port.
-  **<scl_io>**: I2C clock pin.
-  **<sda_io>**: I2C data transfer pin.
-  **<clock>**: I2C Host mode clock frequency, Unit: Hz, Range: 100000 to 400000.


Description
^^^^^^^^^^^

-  [SCL, SDA] can be selected as [PA1, PA4],[PB20, PB19]. For the PA group, use the corresponding numbers directly. For the PB group, add 16 to the number in the name. However, note that PB20 and PB19 are typically used for UART0.
-  Frequency range: [100000, 400000] Hz.

Example
^^^^^^^

::

    AT+DRVI2CINIT=0,1,4,400000      // I2C device 0 uses PA1 and PA4; the clock frequency is 400 KHz.
    AT+DRVI2CINIT=0,36,35,400000    // I2C device 0 uses PB20 and PB19; the clock frequency is 400 KHz.
    AT+DRVI2CINIT=0                 // Cancel I2C initialization.



.. _cmd-DRVI2CRD:

:ref:`AT+DRVI2CRD <cmd-DRVI2CRD>`: Read data from the I2C device.
-------------------------------------------------------------------------------------------

Set command
^^^^^^^^^^^

**Command:**

::

    AT+DRVI2CRD=<num>,<address>,<length>

**Response:**

::

    +DRVI2CRD:<read data>
    OK

Parameter
^^^^^^^^^^

-  **<num>**: The I2C port number can only be 0.
-  **<address>**: I2C slave address, 7-bit address: 0 ~ 0x7F.
-  **<length>**: I2C data length, range: 1 ~ 2048.
-  **<read data>**: I2C read data.

Description
^^^^^^^^^^^

-  When reading data from a slave device, it is usually necessary to first write the offset address or register address within the slave device.

Example
^^^^^^^

::

    AT+DRVI2CRD=0,0x52,1 // To read a byte from a slave device at address 0x52 using I2C Host device 0, you need to issue a command based on the specific type of the device.

    For EEPROM, you can first write the offset address to be read, and then read the data. For example:
    AT+DRVI2CWRBYTES=0,0x52,1,0x10     // I2C0 writes the internal offset address 0x10 to the device at address 0x52.
    AT+DRVI2CRD=0,0x52,6               // I2C0 reads 6 bytes from the device at address 0x52, starting from the internal offset 0x10.



.. _cmd-DRVI2CWRDATA:

:ref:`AT+DRVI2CWRDATA <cmd-DRVI2CWRDATA>`: Write I2C data.
-------------------------------------------------------------------------------------------

Set command
^^^^^^^^^^^

**Command:**

::

    AT+DRVI2CWRDATA=<num>,<address>,<length>

**Response:**

::

    OK


After receiving the above response, enter the data to be written. When the data reaches the specified length in the parameter, the data transfer begins. If the data transfer is successful, it returns:

::

    OK


If the data transfer fails, it returns:

::

    ERROR


Parameter
^^^^^^^^^^

-  **<num>**: The I2C port number can only be 0.
-  **<address>**: I2C slave device address, 7-bit address: 0 ~ 0x7F;
-  **<length>**: I2C data length, range: 1 ~ 2048.

Description
^^^^^^^^^^^

-  When writing data to the device, it is usually necessary to prepend the data with the slave device's internal offset address or register address.

Example
^^^^^^^

::

    AT+DRVI2CWRDATA=0,0x52,10   // I2C0 writes 10 bytes to the slave device.
    30 31 32 33 34 35 36 37 38 39 //In passthrough mode, write the first 10 bytes of data. Note the 8-bit offset EEPROM, where the first byte 0x30 represents the offset address to write.

    For an 8-bit offset address EEPROM, the byte before the write command is considered the offset address, and the following bytes are considered data. The address and data cannot be written in two separate operations. For a 16-bit offset address EEPROM, 
    the first two bytes of the write operation are considered the address.

.. _cmd-DRVI2CWRBYTES:

:ref:`AT+DRVI2CWRBYTES <cmd-DRVI2CWRBYTES>`: Write I2C data not exceeding 4 bytes.
-------------------------------------------------------------------------------------------

Set command
^^^^^^^^^^^

**Command:**

::

    AT+DRVI2CWRBYTES=<num>,<address>,<length>,<data>

**Response:**

::

    OK


Parameter
^^^^^^^^^^

-  **<num>**: The I2C port number can only be 0.
-  **<address>**: I2C slave device address, 7-bit address: 0 ~ 0x7F.
-  **<length>**: The length of the I2C data to be written, range: 1 to 4 bytes.
-  **<data>**: The parameter <length> specifies the data, range: 0 to 0xFFFFFFFF.

Description
^^^^^^^^^^^

-  None 

Example
^^^^^^^

::

    AT+DRVI2CWRBYTES=0,0x52,3,0x103132     // I2C0 writes 3-byte data 0x103132 to the device with address 0x52.



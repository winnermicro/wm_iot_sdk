
.. _codec_i2s:

CODEC_I2S
=============

Introduction
--------------

    CODEC_I2S module is used to drive the external audio chip and complete audio recording, playing and other processing with I2S module.

    For example, the external connection of the Shunxin ES8374 chip and the external connection of the microphone, speakers and headphones
    can be used to record the sound and play the audio data from the speakers and headphones.


    .. figure:: ../../../_static/component-guides/driver/codec_module_h.svg
        :align: center
        :alt: CODEC_I2S Common use connection


Function List
--------------

    1. :ref:`Init <codec_i2s_init>`——Initialize CODEC driver

    2. :ref:`Deinit <codec_i2s_deinit>`——De initialize CODEC drive

    3. :ref:`Set format <codec_i2s_set_format>`——Set audio format

    4. :ref:`Start <codec_i2s_start>`——Start codec

    5. :ref:`Stop <codec_i2s_stop>`——Stop codec

    6. :ref:`Set mute <codec_i2s_set_mute>`——Enable/Disable output mute

    7. :ref:`Set volume <codec_i2s_set_volume>`——Set output volume level

    8. :ref:`Set mic mute <codec_i2s_set_mic_mute>`——Enable/Disable input mute

    9. :ref:`Set mic volume <codec_i2s_set_mic_volume>`——Set input volume level

    10. :ref:`Set reg <codec_i2s_set_reg>`——Write CODEC chip register directly

    11. :ref:`Get reg <codec_i2s_get_reg>`——Read CODEC chip register directly

Function Overview
------------------

    **Codec Chip** At present, the driver only supports the ES8374 audio chip. If you need to use other chips, you can refer to the document for porting.

    **Master and Slave** At present, the WM80X chip is generally the Master, ES8374 as Slave.

    **Tansfer Protocol** I2S and CODEC module use I2S protocol.

    **Work Mode** Support IN, OUT, IN + OUT work mode:

        IN : Record mode, audio form microphone,tansfer the audio data form CODEC to WM80X through I2S port;

        OUT : playback mode, WM80X tansfer the audio data to CODEC chip through I2S port. and the output to speaker and headphone;

        IN + OUT : Use the record and playback mode at the same time.

    **Sample Rate** Different CODEC chips have different capabilities

        ES8374 : Support 8K ~ 96K , 32-bit stereo only supports 8K ~ 48K .

    **Bit width** Support 16 bits 32 bits if use ES8374, User can resample audio data to 16 bits if the source sample is 8 bits or 24 bits, There is a example in .

    **Sound Channel** Support mono and stereo.

    **Mute** Support mute function.

    **volume level** Support volume setting.

    **Register W/R** It provides an interface for directly reading and writing CODEC chip registers.


.. _codec_i2s_set:

Configuration
-------------------


Device Table Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^
    CODEC_I2S needs to be configured according to the actual board and chip, hardware connection and CODEC chip model.

Compile Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^

    I2C, I2S, CODEC module needs

    CONFIG_COMPONENT_DRIVER_I2C_ENABLED=y

    CONFIG_COMPONENT_DRIVER_I2S_ENABLED=y

    CONFIG_COMPONENT_DRIVER_CODEC_ES8374_ENABLED=y

CODEC_I2C Address
----------------------

    When WM80X controls the CODEC chip, it uses the I2C protocol. For the I2C address of the CODEC chip,
    please refer to the manual and hardware connection The I2C driver interface uses a 7-bit address,
    excluding the read/write bit. When calling the read/write interface, the I2C bottom driver will move
    one bit to the left and add the read/write bit.


Main Functions
---------------

.. _codec_i2s_init:

Initialize CODEC_I2S
^^^^^^^^^^^^^^^^^^^^^^^^^

    Aall ``wm_drv_codec_i2s_init()`` before use other CODEC_I2S api , for example

    .. code:: c

        wm_device_t *codec_i2s_dev = wm_dt_get_device_by_name("es8374");
        wm_drv_codec_i2s_init("es8374");

.. _codec_i2s_deinit:

Deinit CODEC_I2S
^^^^^^^^^^^^^^^^^^

    Call ``wm_drv_codec_i2s_deinit()`` to release CODEC_I2S resources, for example

    .. code:: c

        wm_drv_codec_i2s_dedeinit(codec_i2s_dev);



.. _codec_i2s_set_format:

Set Audio Format
^^^^^^^^^^^^^^^^^^^^

    Call ``wm_drv_codec_i2s_set_format`` after initialize to set audio format.

    .. code:: c

        wm_drv_codec_i2s_set_format(codec_i2s_dev, 48000, 16, WM_I2S_CHAN_TYPE_STEREO);


    .. warning:: If you use both recording and playback, you must use the same format.

.. _codec_i2s_start:

Start
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    Call ``wm_drv_codec_i2s_start`` to start work.


    .. code:: c

        wm_drv_codec_i2s_start(codec_i2s_dev);


.. _codec_i2s_stop:

Stop
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    Call ``wm_drv_codec_i2s_stop`` to stop work.


    .. code:: c

        wm_drv_codec_i2s_stop(codec_i2s_dev);


.. _codec_i2s_set_mute:

Set Playback Mute
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    Call ``wm_drv_codec_i2s_set_mute`` to set playback mute.


    .. code:: c

        wm_drv_codec_i2s_set_mute(codec_i2s_dev);


.. _codec_i2s_set_mic_mute:

Set Record Mute
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    Call ``wm_drv_codec_i2s_set_mic_mute`` to set record mute.


    .. code:: c

        wm_drv_codec_i2s_set_mic_mute(codec_i2s_dev);


.. _codec_i2s_set_volume:

Set playback volume level
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    call ``wm_drv_codec_i2s_set_volume`` to set playback volume level, the volume is form  0 to 100.


    .. code:: c

        wm_drv_codec_i2s_set_volume(codec_i2s_dev,90);


.. _codec_i2s_set_mic_volume:

Set record volume level
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    Call ``wm_drv_codec_i2s_set_mic_volume`` to set record volume level, the volume is form  0 to 100.


    .. code:: c

        wm_drv_codec_i2s_set_mic_volume(codec_i2s_dev,90);

.. _codec_i2s_set_reg:

Write CODEC register
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    Call ``wm_drv_codec_i2s_set_reg`` to write CODEC register.


    .. code:: c

        wm_drv_codec_i2s_set_reg(codec_i2s_dev,0x38,20);


.. _codec_i2s_get_reg:

Read CODEC register
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    Call ``wm_drv_codec_i2s_get_reg`` to read CODEC register.


    .. code:: c

        int value;
        wm_drv_codec_i2s_get_reg(codec_i2s_dev,0x38,&value);


Notes
-------------

    .. note::
        The volume ranges from 0 to 100, maped linearly to the minimum and maximum db values of the specific codec.

        When the upper level volume changes, the specific codec's db value may not change because it's has less volume db levels.

        When db changes linearly, the corresponding voltage and power amplification are not linear, and the sound on
        the human ear is also not linear. To achieve the linear volume change on the sense, upper level users need
        to add a mapping table for conversion.


    .. warning::

        The VDDAC and VDDSPK voltages should be same to avoid the speaker make a popping voice.


Application Example
--------------------------

    For a basic example of using the CODEC_I2S module, please refer to :ref:`examples/peripheral/codec_i2s/wav_play_record<peripheral_example>`


API Reference
--------------------------
    To find CODEC_I2S related APIs, please refer to:

    :ref:`label_api_codec_i2s`




New CODEC_I2S driver development
=========================================


    The SDK already contains ES8374 drivers. For other audio chips, new driver codes need to be added.


    The CODEC_I2S driver has a unified interface, which is defined in "components/driver/include/codec_i2s/wm_drv_codc_i2s.h" .
    The function pointer defined in the Structure wm_drv_codc_i2s_ops_t , which is the interface needed to be implemented for the
    CODEC_I2S driver.

    The drive implementation of CODEC_I2S is placed under components/driver/src/codec_i2s/. The wm_drv_codc_i2s.c file implements
    the call from the unified interface to the specific driver instance. The association between the specific driver instance and
    the device is defined in the device table.

    .. figure:: ../../../_static/component-guides/driver/codec_module_s.svg
        :align: center
        :alt: CODEC_I2S

    The file wm_drv_codec_i2s_xxx. c in the figure is the driver to be implemented。


    STEP 1: Create Device information in Device Table

    The following is the device information of ES8374, which can be modified by reference

    .. code:: c

        const static wm_dt_hw_codec_i2s_t dt_hw_es8374 = {
            .init_cfg                 = { .init_level = 0, .init_priority = 49 },
            .i2s_device_name = "i2s",
            .i2c_device_name = "i2c",
            .codec_cfg = {
                .i2c_address = 0x10,        /**< codec i2c 7 bits address   */
                .in_port = 2,               /**< mic port2 input            */
                .out_port = 3,              /**< speaker and headphone use  */
                .jack_pin = WM_GPIO_NUM_MAX,
                .pa_pin = WM_GPIO_NUM_MAX,
                .max_gain = 0,           /**< VPA = 5.0V , VDAC = 3.3V , max_gain = 20 * log(5.0/3.3) = 0 dB */
            },
        };


    .. note::

        For the modification method of the device , refer to the relevant chapter Pinmux and the device list.

        codec_cfg in the device table must match the development board


    STEP 2: Create the new device driver

    The wm_drv_codec_i2s_es8374. c file is the driver of ES8374 series models The file internally declares the global variable
    wm_drv_codec_i2s_es8374_ops, which will be associated to the device in the device table. When operating on the device, the
    interface in the ops will be called.

    .. code:: c

        const wm_drv_codec_i2s_ops_t wm_drv_codec_i2s_xxx_ops = {
            .init   = xxx_init,
            .deinit = xxx_deinit,
            .set_format = xxx_set_format,
            .start = xxx_start,
            .stop  = xxx_stop,
            .set_mute       = xxx_set_mute,
            .set_volume     = xxx_set_volume,
            .set_mic_mute   = xxx_set_mic_mute,
            .set_mic_volume = xxx_set_mic_volume,
            .set_reg = xxx_set_reg,
            .get_reg = xxx_get_reg,
            .dump = xxx_dump,
        };

    STEP 3: Configuration

    In the Kconfig file under components/driver, add the CODEC module enabling option COMPONENT_DRIVER_CODEC_XXXX_ENABLED

    In the CMakeLists.txt file, add the newly added driver file to the compilation

    The following is the configuration of ES8374:

    .. code::

        config COMPONENT_DRIVER_CODEC_ES8374_ENABLED
            bool "CODEC ES8374"
            default n
            select COMPONENT_DRIVER_I2S_ENABLED
            select COMPONENT_DRIVER_I2C_ENABLED
            help
                Enable codec es8374 driver module.


    .. note:: The new driver needs to be associated with a certain device in the device table so that the device can use the new driver.

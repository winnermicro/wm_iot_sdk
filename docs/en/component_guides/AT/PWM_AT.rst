
.. _pwm:

****************************
PWM AT Command Set
****************************

-  :ref:`AT+DRVPWMINIT <cmd-DRVPWMINIT>`: Initialize the PWM module with a specified frequency and GPIO
-  :ref:`AT+DRVPWMDUTY <cmd-DRVPWMDUTY>`: Set duty cycle of a specified PWM channel
-  :ref:`AT+DRVPWMFADE <cmd-DRVPWMFADE>`: Set PWM fade

Introduction
--------------

PWM, or Pulse Width Modulation, is an analog signal control technique widely used in electronics and automation control. PWM controls the output of analog circuits by varying the pulse width to manipulate the signal's duty cycle.

.. _cmd-DRVPWMINIT:

:ref:`AT+DRVPWMINIT <cmd-DRVPWMINIT>`: Initialize the PWM Module with a Specified Frequency and GPIO
-----------------------------------------------------------------------------------------------------------------

Set Command
^^^^^^^^^^^^^


**Command:**

::

    AT+DRVPWMINIT=<freq>,<ch0_gpio>,[,...,<ch4_gpio>]

**Response:**

::

    OK

Parameters
^^^^^^^^^^^^^

-  **<freq>**: The frequency of PWM output, in Hz
-  **<ch0_gpio>**: The GPIO pin for PWM output

Description
^^^^^^^^^^^^^

-  The set frequency must satisfy the formula: `master_clock / (period + 1) / clockdiv`, so some frequencies that cannot be divided evenly may not be output.
-  Selectable values for ch0_gpio : 16, 35, 28, 2, 10
-  Selectable values for ch1_gpio :: 17, 36, 3, 11, 29
-  Selectable values for ch2_gpio :: 0, 18, 12, 30, 40
-  Selectable values for ch3_gpio :: 1, 19, 13, 31, 41
-  Selectable values for ch4_gpio :: 4,  7, 14, 32, 42
-  Frequency range: [3, 160K] HZ

Example
^^^^^^^^^^^^^

::

    // output 10KHz on pwm channel0
    AT+DRVPWMINIT=10000,16
    // output 10KHz on pwm channel0,1,2
    AT+DRVPWMINIT=10000,16,17,18 
    // output 10KHz on pwm channel1,2
    AT+DRVPWMINIT=10000,,17,18
    // output 10KHz on pwm channel2
    AT+DRVPWMINIT=10000,,,18 

.. _cmd-DRVPWMDUTY:

:ref:`AT+DRVPWMDUTY <cmd-DRVPWMDUTY>`: Set Duty Cycle of a Specified PWM Channel
-------------------------------------------------------------------------------------------

Set command
^^^^^^^^^^^^^


**Command:**

::

    AT+DRVPWMDUTY=<ch0 duty>[,...,<ch4 duty>]

**Response:**

::

    OK

Parameters
^^^^^^^^^^^^^

-  **<ch0 duty>**: Duty cycle of PWM output on channel0
-  **<ch1 duty>**: Duty cycle of PWM output on channel1
-  **<ch2 duty>**: Duty cycle of PWM output on channel2
-  **<ch3 duty>**: Duty cycle of PWM output on channel3
-  **<ch4 duty>**: Duty cycle of PWM output on channel4

Description
^^^^^^^^^^^^^

-  The set duty cycle will affect the frequency set during initialization.

Example
^^^^^^^^^^^

::

    // set the high-level cycle count to 10 in the PWM waveform output on pwm channel0
    AT+DRVPWMDUTY=10
    // set the high-level cycle count to 10,20,30 in the PWM waveform output on pwm channel0,1,2
    AT+DRVPWMDUTY=10,20,30
    // set the high-level cycle count to 20,30 in the PWM waveform output on pwm channel1,2
    AT+DRVPWMDUTY=,20,30
    // set the high-level cycle count to 30 in the PWM waveform output on pwm channel2
    AT+DRVPWMDUTY=,,30

.. _cmd-DRVPWMFADE:

:ref:`AT+DRVPWMFADE <cmd-DRVPWMFADE>`: Set Fade Mode for Specified PWM Channel
-------------------------------------------------------------------------------------------

Set Command
^^^^^^^^^^^^^
^^^^^^^

**Command:**

::

    AT+DRVPWMFADE=<ch0_target_duty>,<ch0_fade_time>[,...,<ch4_target_duty>,<ch4_fade_time>]

**Response:**

::

    OK

Parameters
^^^^^^^^^^^^^

-  **<ch0_target_duty>**: Target duty cycle of PWM output on channel0
-  **<ch0_fade_time>**: Time (ms) to change to the target duty cycle on channel0
-  **<ch1_target_duty>**: Target duty cycle of PWM output on channel1
-  **<ch1_fade_time>**: Time (ms) to change to the target duty cycle on channel1
-  **<ch2_target_duty>**: Target duty cycle of PWM output on channel2
-  **<ch2_fade_time>**: Time (ms) to change to the target duty cycle on channel2
-  **<ch3_target_duty>**: Target duty cycle of PWM output on channel3
-  **<ch3_fade_time>**: Time (ms) to change to the target duty cycle on channel3
-  **<ch4_target_duty>**: Target duty cycle of PWM output on channel4
-  **<ch4_fade_time>**: Time (ms) to change to the target duty cycle on channel4

Description
^^^^^^^^^^^^^

-   When the target duty cycle is greater than the current duty cycle, the duty cycle will increment; otherwise, it will decrement.

Example
^^^^^^^^^^^^^

::

    // change the duty cycle of the PWM waveform from the current duty value to 100 within 10 seconds
    AT+DRVPWMFADE=100,10000
    // pwm channel0,1,2 output PWM waveform with duty cycle changing from the current duty value to 100 within 10 seconds
    AT+DRVPWMFADE=100,10000,100,10000,100,10000
    // pwm channel1,2 output PWM waveform with duty cycle changing from the current duty value to 100 within 10 seconds
    AT+DRVPWMFADE=,,100,10000,100,10000
    // pwm channel2 output PWM waveform with duty cycle changing from the current duty value to 100 within 10 seconds
    AT+DRVPWMFADE=,,,,100,10000


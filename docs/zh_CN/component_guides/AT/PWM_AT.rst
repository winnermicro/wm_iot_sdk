
.. _pwm:

****************************
PWM AT 命令集
****************************

-  :ref:`AT+DRVPWMINIT <cmd-DRVPWMINIT>`: 以指定频率与 GPIO 初始化 PWM 模块
-  :ref:`AT+DRVPWMDUTY <cmd-DRVPWMDUTY>`: 设置指定 PWM channel 的占空比数
-  :ref:`AT+DRVPWMFADE <cmd-DRVPWMFADE>`: 设置 PWM 渐变

介绍
------

PWM，即脉冲宽度调制（Pulse Width Modulation），是一种模拟信号控制技术，广泛应用于电子学和自动化控制领域。PWM通过改变脉冲的宽度来控制信号的占空比，从而控制模拟电路的输出。

.. _cmd-DRVPWMINIT:

:ref:`AT+DRVPWMINIT <cmd-DRVPWMINIT>`: 以指定频率与 GPIO 初始化 PWM 模块
-------------------------------------------------------------------------------------------

设置命令
^^^^^^^^^^^

**命令：**

::

    AT+DRVPWMINIT=<freq>,<ch0_gpio>,[,...,<ch4_gpio>]

**响应：**

::

    OK

参数
^^^^^^^^^^

-  **<freq>**: PWM 输出的频率，单位 HZ
-  **<ch0_gpio>**: 输出 PWM 的 GPIO 引脚

说明
^^^^^

-  设定的频率需要满足公式: `master_clock / (period + 1) / clockdiv`,故部分无法被整除的频率无法输出
-  ch0_gpio 可选取值: 16, 35, 28, 2, 10
-  ch1_gpio 可选取值: 17, 36, 3, 11, 29
-  ch2_gpio 可选取值: 0, 18, 12, 30, 40
-  ch3_gpio 可选取值: 1, 19, 13, 31, 41
-  ch4_gpio 可选取值: 4,  7, 14, 32, 42
-  频率取值范围 [3, 160K] HZ

示例
^^^^

::

    // pwm channel0 输出 10KHz
    AT+DRVPWMINIT=10000,16
    // pwm channel0,1,2 输出 10KHz
    AT+DRVPWMINIT=10000,16,17,18 
    // pwm channel1,2 输出 10KHz
    AT+DRVPWMINIT=10000,,17,18 
    // pwm channel2 输出 10KHz
    AT+DRVPWMINIT=10000,,,18

.. _cmd-DRVPWMDUTY:

:ref:`AT+DRVPWMDUTY <cmd-DRVPWMDUTY>`: 设置指定 PWM channel 的占空比
-------------------------------------------------------------------------------------------

设置命令
^^^^^^^^^^^

**命令：**

::

    AT+DRVPWMDUTY=<ch0 duty>[,...,<ch4 duty>]

**响应：**

::

    OK

参数
^^^^^^^^^^

-  **<ch0 duty>**: channel0 输出的 PWM 占空比数
-  **<ch1 duty>**: channel1 输出的 PWM 占空比数
-  **<ch2 duty>**: channel2 输出的 PWM 占空比数
-  **<ch3 duty>**: channel3 输出的 PWM 占空比数
-  **<ch4 duty>**: channel4 输出的 PWM 占空比数

说明
^^^^^

-  设置的占空比数 duty 会影响初始化时设置的频率

示例
^^^^

::

    // pwm channel0 输出 PWM 波形中的高电平 cycle 数为 10
    AT+DRVPWMDUTY=10
    // pwm channel0,1,2 输出 PWM 波形中的高电平 cycle 数为 10,20,30
    AT+DRVPWMDUTY=10,20,30
    // pwm channel1,2 输出 PWM 波形中的高电平 cycle 数为 20,30
    AT+DRVPWMDUTY=,20,30
    // pwm channel2 输出 PWM 波形中的高电平 cycle 数为 30
    AT+DRVPWMDUTY=,,30

.. _cmd-DRVPWMFADE:

:ref:`AT+DRVPWMFADE <cmd-DRVPWMFADE>`: 设置指定 PWM channel 的渐变模式
-------------------------------------------------------------------------------------------

设置命令
^^^^^^^^^^^

**命令：**

::

    AT+DRVPWMFADE=<ch0_target_duty>,<ch0_fade_time>[,...,<ch4_target_duty>,<ch4_fade_time>]

**响应：**

::

    OK

参数
^^^^^^^^^^

-  **<ch0_target_duty>**: channel0 最终输出的PWM占空比数
-  **<ch0_fade_time>**: channel0 变化至最终占空比数时所需时间(ms)
-  **<ch1_target_duty>**: channel1 最终输出的PWM占空比数
-  **<ch1_fade_time>**: channel1 变化至最终占空比数时所需时间(ms)
-  **<ch2_target_duty>**: channel2 最终输出的PWM占空比数
-  **<ch2_fade_time>**: channel2 变化至最终占空比数时所需时间(ms)
-  **<ch3_target_duty>**: channel2 最终输出的PWM占空比数
-  **<ch3_fade_time>**: channel2 变化至最终占空比数时所需时间(ms)
-  **<ch4_target_duty>**: channel3 最终输出的PWM占空比数
-  **<ch4_fade_time>**: channel3 变化至最终占空比数时所需时间(ms)

说明
^^^^^

-  当 target duty 大于当前duty值时，duty 值递增变化，反之递减

示例
^^^^

::

    // pwm channel0 输出pwm波形的占空比从当前duty值在 10 秒内变化至 100
    AT+DRVPWMFADE=100,10000
    // pwm channel0,1,2 输出pwm波形的占空比从当前duty值在 10 秒内变化至 100
    AT+DRVPWMFADE=100,10000,100,10000,100,10000
    // pwm channel1,2 输出pwm波形的占空比从当前duty值在 10 秒内变化至 100
    AT+DRVPWMFADE=,,100,10000,100,10000
    // pwm channel2 输出pwm波形的占空比从当前duty值在 10 秒内变化至 100
    AT+DRVPWMFADE=,,,,100,10000


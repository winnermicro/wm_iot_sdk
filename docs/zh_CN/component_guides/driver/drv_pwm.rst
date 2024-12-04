.. _pwm_drv:

PWM Driver
=============

简介
-------------

| 脉宽调制(Pulse Width Modulation, PWM)作为一种控制技术，在物联网(IoT)领域中的应用正日益广泛。PWM 通过调节脉冲信号的占空比来控制电压或功率，是一种高效的模拟信号控制方式。
| PWM已广泛应用于智能家居、工业自动化、医疗设备、智能农业等多个领域。

- 智能家居：PWM技术在智能照明系统中用于调节灯光的亮度和色温，提高能源效率。同时，在智能音响设备中，PWM用于控制音量和音质，以提供更好的用户体验。
- 工业自动化：在工业领域，PWM广泛应用于电机控制系统，精确调节电机转速和扭矩，提升生产线的效率和精度。
- 医疗设备：PWM技术在医疗设备中的应用主要体现在呼吸机、注射泵等设备中，通过精确控制输出参数，确保设备的稳定和可靠运行。
- 智能农业：在智能农业中，PWM用于控制灌溉系统的水流量、施肥系统的肥料浓度等，助力精准农业的发展。

功能列表
-------------

- 支持2通道输入信号捕获(PWM0/PWM4)。
- 支持5通道PWM信号生成。
- 支持5通道制动功能。

功能概述
-------------

.. figure:: ../../../_static/component-guides/driver/pwm.svg
    :align: center
    :alt: pwm流程图

输出模式
^^^^^^^^^^^^^

- **独立输出**：每个通道独立输出PWM波形

  .. figure:: ../../../_static/component-guides/driver/pwm_indp.svg
      :align: center
      :alt: pwm 独立输出

- **双通道同步输出**：ch0+ch1 或 ch2+ch3 同步输出

  .. figure:: ../../../_static/component-guides/driver/pwm_2sync.svg
      :align: center
      :alt: pwm 双通道同步输出

- **全通道同步输出**：所有通道同步输出

  .. figure:: ../../../_static/component-guides/driver/pwmallsync.svg
      :align: center
      :alt: pwm 全通道同步输出

- **双通道互补输出**：ch0+ch1 或 ch2+ch3 输出互补波形

  .. figure:: ../../../_static/component-guides/driver/pwm_2complement.svg
      :align: center
      :alt: pwm 双通道互补输出

制动模式
^^^^^^^^^^^^^

根据GPIO的配置可以实现如下两种制动模式：

- 当制动输入GPIO检测到低电平时，相应通道输出指定的高/低电平
- 当制动输入GPIO检测到高电平时，相应通道输出指定的高/低电平
- 制动的输入信号电平由 GPIO 初始化配置时决定，若上拉则检测到低电平触发，反之同理。

  .. figure:: ../../../_static/component-guides/driver/pwm_brake.svg
      :align: center
      :alt: pwm 制动

.. note::
   若需要触发时（拉高制动触发引脚）输出PWM波形，需要指定period与clkdiv，按照文档中公式输出特定的频率，设置为0为电平模式

.. note::
   PWM 输出行为基于周期和占空比条件（autoload = true，inverse = false）。实际输出参考如下表格

.. list-table:: PWM Output Behavior
   :header-rows: 1

   * - period
     - duty
     - period_num == 0
     - period_num > 0
   * - period = 0
     - duty > period
     - output high level
     - output high level
   * - period > 0
     - duty >= period
     - output high level
     - output a high level pulse
   * - period = 0
     - duty = 0
     - output high level
     - output high level
   * - period > duty
     - duty >= 0
     - output PWM
     - output period_num PWM

捕获模式
^^^^^^^^^^^^^

从ch0或ch4通道监测输入信号，采样范围为：3 ~16K。

  .. figure:: ../../../_static/component-guides/driver/pwm_capture.svg
      :align: center
      :alt: pwm 捕获

.. note::
    捕获模式的period与clkdiv参数设置得更大，以便采样低频率；反之，设置为较小的值以采样高频率。



功能配置
-------------

PWM通道配置
^^^^^^^^^^^^^

| 配置通道调用函数 ``wm_drv_pwm_channel_init``。
| 通道的配置需向通道配置函数传递包括设置参数设置的结构体 ``wm_drv_pwm_channel_cfg_t``。
| 此时调用函数 ``wm_drv_pwm_channels_start`` 可启动已配置pwm通道，调用函数 ``wm_drv_pwm_channels_stop`` 停止已配置pwm通道。

PWM频率配置
^^^^^^^^^^^^^

PWM频率是在1秒钟内，信号从高电平到低电平再回到高电平的次数，也就是一秒钟有多少个PWM周期。我们提供有两种方法改变PWM频率

- 调用 ``wm_drv_pwm_set_channel_freq`` 设置指定PWM通道频率
- 调用 ``wm_drv_pwm_set_channel_period_clkdiv`` 根据给定的周期和时钟分频器值设置PWM通道的频率
- 调用 ``wm_drv_pwm_get_channel_freq``  获取指定PWM通道的当前频率

.. note::
    频率设置策略：
    
    1. 频率必须在 [WM_DRV_PWM_FREQ_MIN(3Hz), WM_DRV_PWM_FREQ_MAX(160KHz)] 范围内
    
    2. 首先尝试使用当前的 period 值，通过调整 clkdiv 实现目标频率
       
       频率计算公式: freq = master_clock / (period + 1) / clkdiv
    
       其中：
       
       - master_clock 为 PWM 时钟频率 (通常为 40MHz)
       
       - period 保持当前配置值不变
       
       - clkdiv 会自动计算，范围为 [1, 65535]
	   
    3. 如果无法通过调整 clkdiv 实现目标频率，函数会尝试同时调整 period 和 clkdiv 来实现目标频率
    
    4. 如果无法计算出合适的 period 和 clkdiv，则设置失败

PWM占空比（duty）配置
^^^^^^^^^^^^^^^^^^^^^^

PWM占空比是一个脉冲周期内，高电平的时间与整个周期时间的比例，通过调节占空比就可以调节脉冲宽度，占空周期数，范围从[0到255]（默认：40）。

- 调用 ``wm_drv_pwm_set_channel_duty`` 设置PWM通道的占空
- 调用 ``wm_drv_pwm_get_channel_duty`` 获取PWM通道的占空比

.. note::
    参数设置10表示10%，不支持小数形式
.. note::
    占空比 = (duty + 1) / (period + 1)

PWM周期（period）配置
^^^^^^^^^^^^^^^^^^^^^^

| PWM周期是一个脉冲信号的时间，每个周期的周期数，范围从[0到255]（默认：199）。
| 周期数为0时禁用此功能，大于0的值在指定周期数后停止输出。

- 调用 ``wm_drv_pwm_set_channel_period_num`` 设置指定PWM通道在产生中断前要生成的周期数
- 调用 ``wm_drv_pwm_get_channel_period_num`` 获取指定PWM通道的周期数中断设置

PWM时钟分频（clkdiv）配置
^^^^^^^^^^^^^^^^^^^^^^^^^^

时钟分频表示每秒钟时钟信号的周期数。

- 调用 ``wm_drv_pwm_get_channel_period_clkdiv`` 获取PWM通道的时钟分频器值

.. note::
    clkdiv为0与1表示不分频



.. note::
    周期受频率影响


主要功能
-------------

输出功能
^^^^^^^^^^^^^

起始条件：

- 定义并初始化 ``wm_drv_pwm_channel_cfg_t`` 结构体。设置PWM通道的基本参数，选择输出模式，初始化PWM通道

相关时序API：

- 调用 ``wm_drv_pwm_init`` 来初始化PWM设备，获取设备指针
- 使用 ``wm_drv_pwm_channel_init`` 根据配置结构体初始化PWM通道
- 调用 ``wm_drv_pwm_channels_start`` 启动PWM通道，开始PWM信号的生成
- 根据需要，调用 ``wm_drv_pwm_set_channel_duty`` 来动态设置通道的占空比
- 根据需要设置周期、频率等

结果：

- 输出所设定占空比的波形

制动功能
^^^^^^^^^^^^^

起始条件：

- 定义并初始化 ``wm_drv_pwm_channel_cfg_t`` 结构体。设置PWM通道的基本参数，选择制动模式，初始化PWM通道

相关时序API：

- 调用 ``wm_drv_pwm_init`` 来初始化PWM设备，获取设备指针
- 使用 ``wm_drv_pwm_set_channel_brake_level`` 设置PWM通道的制动模式输出高/低电平
- 使用 ``wm_drv_pwm_channel_init`` 根据配置结构体初始化PWM通道
- 调用 ``wm_drv_pwm_channels_start`` 启动PWM通道


结果：

- 当制动输入GPIO检测到低电平时，相应通道输出指定的高/低电平
- 当制动输入GPIO检测到高电平时，相应通道输出指定的高/低电平

捕获功能
^^^^^^^^^^^^^

起始条件：

- 定义并初始化 ``wm_drv_pwm_channel_cfg_t`` 结构体。设置PWM通道的基本参数，选择捕获模式，初始化PWM通道

相关时序API：

- 调用 ``wm_drv_pwm_init`` 来初始化PWM设备，获取设备指针
- 使用 ``wm_drv_pwm_channel_init`` 根据配置结构体初始化PWM通道
- 调用 ``wm_drv_pwm_channels_start`` 启动PWM通道

结果：

- 捕获到脉冲

应用实例
-------------

    使用 PWM 基本示例请参照 :ref:`examples/peripheral/pwm<peripheral_example>`

.. code:: C

    int main(void)
    {
        wm_drv_pwm_channel_cfg_t cfg = { 0 };

        wm_device_t *pwm_device = wm_drv_pwm_init("pwm");
        if (pwm_device == NULL) {
            WM_PWM_DEMO_LOG_E("PWM driver init Failed!\n");

            return WM_ERR_FAILED;
        }

        cfg.channel      = WM_PWM_CHANNEL_0;
        cfg.mode         = WM_PWM_OUT_ALLSYNC;
        cfg.clkdiv       = WM_PWM_CLKDIV_DEFAULT;
        cfg.period_cycle = WM_PWM_PERIOD_DEFAULT;
        cfg.duty_cycle   = WM_PWM_DUTY_CYCLE_DEFAULT;
        cfg.autoload     = true;

        if (wm_drv_pwm_channel_init(pwm_device, &cfg) != WM_ERR_SUCCESS) {
            WM_PWM_DEMO_LOG_E("PWM driver channel init failed!\n");
        }

        if (wm_drv_pwm_channels_start(pwm_device)) {
            WM_PWM_DEMO_LOG_E("PWM driver channel start failed!\n");
        }
        /*以上PWM初始化完成后，调用各类功能函数，例如wm_drv_pwm_set_channel_duty来设置占空比*/

        return WM_ERR_SUCCESS;
    }


API参考
-------------
    查找 PWM 相关 API 请参考：

    :ref:`label_api_pwm`

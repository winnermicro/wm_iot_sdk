CSI DSP
==============

简介
--------------

CSI DSP 软件库，是一套用于玄铁处理器的通用信号处理函数，共分为 DSP 和 DSP2 两个版本。

DSP 库内的各个函数可以分类为:

- 基本数学函数
- 快速数学函数
- 复数函数
- 滤波函数
- 矩阵函数
- 变换函数
- 电机控制函数
- 统计函数
- 辅助函数
- 插值函数

库内的大多数函数都有 Q7，Q15，Q31 和单精度浮点四种版本。

DSP2 库为指令集加速的神经网络基本单元的集合，库内的各个函数可以分类为:

- 激活函数
- 卷积函数
- 全连接函数
- 池化函数
- softmax 函数
- 辅助函数

库内的大多数函数都有 Q7，Q15 两种版本。

使用
------------------------

在应用程序中包含 csi_math.h 头文件，就可以直接调用 DSP 库函数。

在应用程序中包含 csky_dsp2_nnfunctions.h 头文件，就可以直接调用 DSP2 库函数。

更详细用法请查看 `CSI DSP 接口说明手册 <../../../../download/csi_dsp/CSI_DSP_Interface_Manual_CN_20240605.pdf>`_ 和 `CSI NN 接口说明手册 <../../../../download/csi_dsp/csi-nn-dsp2.pdf>`_。

.. attention::
    当前存在以 ``csky_`` 开头和以 ``csi_`` 开头的两种接口，虽然名字不同但是功能完全一致。推荐使用以 ``csi_`` 的开头的接口，未来的版本中会移除以 ``csky_`` 开头的接口。

CSI DSP
==============

Overview
--------------

The CSI DSP software library is a set of general-purpose signal processing functions designed for Xuantie processors. It is divided into two versions: DSP and DSP2.

The functions within the DSP library can be categorized as:

- Basic Mathematical Functions
- Fast Mathematical Functions
- Complex Functions
- Filtering Functions
- Matrix Functions
- Transform Functions
- Motor Control Functions
- Statistical Functions
- Helper Functions
- Interpolation Functions

Most functions within the library are available in four versions: Q7, Q15, Q31, and single-precision floating-point.

The DSP2 library is a collection of instruction-set accelerated neural network basic units. The functions in the DSP2 library can be categorized into:

- Activation Functions
- Convolution Functions
- Fully Connected Functions
- Pooling Functions
- Softmax Functions
- Helper Functions

Most functions within the library are available in two versions: Q7 and Q15.

Usage
------------

To use the DSP library functions in your application, include the `csi_math.h` header file.

To use the DSP2 library functions in your application, include the `csky_dsp2_nnfunctions.h` header file.

For more detailed usage, please refer to the  `CSI DSP Interface Manual <../../../../download/csi_dsp/CSI_DSP_Interface_Manual_EN_20240605.pdf>`_ and the `CSI NN Interface Manual <../../../../download/csi_dsp/csi-nn-dsp2.pdf>`_.

.. attention::
    There are currently two types of interfaces, those starting with ``csky_`` and those starting with ``csi_`` . Although their names are different, they provide the same functionality. It is recommended to use the interfaces starting with ``csi_`` , as the  ``csky_`` interfaces will be removed in future versions.


错误码
===========


概述
---------------

错误码是一种用于表示特定函数或操作失败原因的标准编码方式。通过定义这些错误码，程序可以更明确地报告问题并作出响应。这些错误码通常根据不同的模块、操作和错误类型进行分类和分配。

定义
---------------

错误码的定义分为不同的类别：



1. 通用错误码
~~~~~~~~~~~~~~~~~~~~~~

    这些错误码可能反映程序的一般错误，例如内存不足（WM_ERR_NO_MEM），参数无效（WM_ERR_INVALID_PARAM）等等。使用这些错误码有助于识别和处理常见错误情况。

    通用错误码以宏定义的形式定义为  ``WM_ERR_xxx`` ，其中 ``xxx`` 是错误码的具体值。例如，一个错误码表示内存不足的情况，可以使用 ``WM_ERR_NO_MEM`` 。

    在 ``wm_error.h`` 中的具体定义如下：

    .. code-block:: c

        #define WM_ERR_SUCCESS          0
        #define WM_ERR_FAILED           -1
        #define WM_ERR_NO_MEM           -2
        #define WM_ERR_INVALID_PARAM    -3
        #define WM_ERR_BUSY             -4
        #define WM_ERR_TIMEOUT          -5
        #define WM_ERR_NO_SUPPORT       -6
        #define WM_ERR_NO_INITED        -7
        #define WM_ERR_NOT_FOUND        -8
        #define WM_ERR_NOT_ALLOWED      -9

下表描述了各个通用错误码的含义：

.. list-table:: 
    :widths: 25 25 
    :header-rows: 0
    :align: center

    * - 错误码
      - 描述

    * - WM_ERR_SUCCESS
      - 操作成功

    * - WM_ERR_FAILED
      - 操作失败

    * - WM_ERR_NO_MEM
      - 内存不足

    * - WM_ERR_INVALID_PARAM
      - 参数无效

    * - WM_ERR_BUSY
      - 操作忙碌

    * - WM_ERR_TIMEOUT
      - 操作超时

    * - WM_ERR_NO_SUPPORT
      - 不支持该操作

    * - WM_ERR_NO_INITED
      - 未初始化

    * - WM_ERR_NOT_FOUND
      - 未找到

    * - WM_ERR_NOT_ALLOWED
      - 不允许操作


2.  模块错误码
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    各模块会根据定义各自的错误码

    模块错误码的基址会定义在 ``wm_error.h`` 中

    在 ``wm_error.h`` 中的定义如下：

    .. code-block:: c

        #define WM_ERR_XXX_BASE    -4000

    ``WM_ERR_XXX_BASE``     表示 XXX 模块的错误码基址。

    模块具体的错误码则需要在模块api中进行定义，例如在 ``WM_XXX.h`` 中定义XXX模块特定的错误码

    .. code-block:: c

        #define WM_ERR_XXX_xxx    (WM_ERR_XXX_BASE -1)


    ``WM_ERR_XXX_xxx``      表示 XXX 模块的某一错误码。





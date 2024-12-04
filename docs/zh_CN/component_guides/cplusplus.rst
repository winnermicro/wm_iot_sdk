C++ 编程
==============

C++ 支持
--------------

WM IoT SDK 主要使用 C 语言编写，并提供 C 语言 API，但 WM IoT SDK 也支持使用 C++ 开发应用程序。

.. only:: w800

    WM IoT SDK 所支持的最高版本为 GNU++14（或 C++14）。但由于嵌入式设备的资源受限，并不支持全部的 C++ 特性，如：


- ``异常捕获机制``
- ``iostream``

C/C++ 混合编程
------------------------

WM IoT SDK 支持 C/C++ 混合编程，即 C 语言编写的代码可以调用 C++ 编写的代码，反之亦然。

当应用程序的不同部分使用 C 和 C++ 开发时，需要特别注意 C/C++ 混合编程的调用规则。

C 代码调用 C++ 代码
~~~~~~~~~~~~~~~~~~~~~~~~~~

为了能够从 C 代码中调用 C++ 函数，该 C++ 函数必须使用 C 链接 ``extern "C"`` 进行 声明 和 定义：

.. code-block:: c

    // 在 .h 文件中声明：
    #ifdef __cplusplus
    extern "C" {
    #endif

    void my_cpp_func(void);

    #ifdef __cplusplus
    }
    #endif

    // 在 .cpp 文件中进行定义：
    extern "C" void my_cpp_func(void) 
    {
        // ...
    }

C++ 代码调用 C 代码
~~~~~~~~~~~~~~~~~~~~~~~~~~


为了能够从 C++ 中调用 C 函数，该 C 函数必须使用 C 链接 声明：

.. code-block:: c

    // 在 .h 文件中声明：
    #ifdef __cplusplus
    extern "C" {
    #endif

    void my_c_func(void);

    #ifdef __cplusplus
    }
    #endif

    // 在 .c 文件中进行定义：
    void my_c_func(void) 
    {
        // ...
    }

在 C++ 中定义 ``main``
~~~~~~~~~~~~~~~~~~~~~~~~~~

WM IoT SDK 希望应用程序入口点 ``main``  以 C 链接定义。当 ``main``  在 .cpp 源文件中定义时，必须以 ``extern "C"``` 标识：

.. code-block:: c

    extern "C" void main()
    {
        // ...
    }

C++ Programming
===================

C++ Support
--------------------------

The WM IoT SDK is primarily written in C and provides C language APIs, but it also supports  the development of applications using C++.

.. only:: w800

    The highest version of C++ supported by the WM IoT SDK is GNU++14 (or C++14). However, due to the resource constraints of embedded devices, not all C++ features are supported, such as:

- ``Exception handling mechanism``
- ``iostream``

Mixed C/C++ Programming
------------------------------

The WM IoT SDK supports mixed C/C++ programming, meaning that code written in C can call code written in C++, and vice versa.

When different parts of an application are developed using C and C++, special attention must be paid to the calling rules for mixed C/C++ programming.

Calling C++ Code from C
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To be able to call a C++ function from C code, the C++ function must be declared and defined with C linkage using ``extern "C"``:

.. code-block:: c

    // Declaration in .h file:
    #ifdef __cplusplus
    extern "C" {
    #endif

    void my_cpp_func(void);

    #ifdef __cplusplus
    }
    #endif

    // Definition in .cpp file:
    extern "C" void my_cpp_func(void) 
    {
        // ...
    }

Calling C Code from C++
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To be able to call a C function from C++, the C function must be declared with C linkage:

.. code-block:: c

    // Declaration in .h file:
    #ifdef __cplusplus
    extern "C" {
    #endif

    void my_c_func(void);

    #ifdef __cplusplus
    }
    #endif

    // Definition in .c file:
    void my_c_func(void) 
    {
        // ...
    }

Defining ``main`` in C++
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The WM IoT SDK expects the application entry point ``main`` to be defined with C linkage. When ``main`` is defined in a .cpp source file, it must be marked with ``extern "C"``:

.. code-block:: c

    extern "C" void main()
    {
        // ...
    }

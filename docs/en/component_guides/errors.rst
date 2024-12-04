
Error Codes
============


Overview
---------------

Error codes are standardized codes used to indicate the reason for a specific function or operation failure. By defining these error codes, a program can more precisely report issues and respond accordingly. These error codes are typically categorized and assigned based on different modules, operations, and types of errors.

Definition
---------------

Error codes are defined in different categories:



1. General Error Codes
~~~~~~~~~~~~~~~~~~~~~~~~~

    These error codes may reflect general errors in the program, such as out of memory (WM_ERR_NO_MEM), invalid parameters (WM_ERR_INVALID_PARAM), and so on. Using these error codes helps to identify and handle common error situations.

    General error codes are defined as macros in the form ``WM_ERR_xxx`` , where ``xxx`` is the specific value of the error code. For example, an error code indicating an out of memory condition can be represented by ``WM_ERR_NO_MEM`` .

    The specific definitions in ``wm_error.h`` are as follows:

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

    The table below describes the meaning of each general error code:

.. list-table:: 
    :widths: 25 25 
    :header-rows: 0
    :align: center

    * - Error Code
      - Description

    * - WM_ERR_SUCCESS
      - Operation successful

    * - WM_ERR_FAILED
      - Operation failed

    * - WM_ERR_NO_MEM
      - Out of memory

    * - WM_ERR_INVALID_PARAM
      - Invalid parameter

    * - WM_ERR_BUSY
      - Operation busy

    * - WM_ERR_TIMEOUT
      - Operation timeout

    * - WM_ERR_NO_SUPPORT
      - Operation not supported

    * - WM_ERR_NO_INITED
      - Not initialized

    * - WM_ERR_NOT_FOUND
      - Not found

    * - WM_ERR_NOT_ALLOWED
      - Not allowed to operate


2.  Module Error Codes
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    Each module defines its own error codes.

    The base address for the  module's error codes will be defined in ``wm_error.h`` .

    The definitions in ``wm_error.h`` are as follows:

    .. code-block:: c

        #define WM_ERR_XXX_BASE    -4000

    ``WM_ERR_XXX_BASE``  represents the base address of error codes for the XXX module.

    Specific error codes for the module should be defined in the module API, such as defining specific error codes for the XXX module in ``WM_XXX.h``.

    .. code-block:: c

        #define WM_ERR_XXX_xxx    (WM_ERR_XXX_BASE -1)


    ``WM_ERR_XXX_xxx`` represents a specific error code for the XXX module.





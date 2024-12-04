:orphan:

Code Style
===============================================

About This Guide
-----------------
The WM IoT SDK coding style generally follows the Linux kernel coding standards with some modifications.

Purpose of this style guide is to encourage use of common coding practices within WM IoT SDK.

The purpose of this standard is to enhance the quality and maintainability of the source code, thereby improving software product productivity. The standard outlines the guidelines programmers must follow during software design, focusing on clarity, concise comments, understandable naming conventions, modular functions, readable and maintainable code, accurate functionality implementation, high space and time efficiency, and appropriate scalability.

Formatting
-----------

- Use 4 spaces for each indentation level. 
- Add blank lines between relatively independent blocks of code and after variable declarations.

Example of Incorrect Formatting:

.. code:: C

    if (!is_valid(ni)) {
        ... // program code
    }
    repssn_ind = ssn_data[index].repssn_index;
    repssn_ni = ssn_data[index].ni;

Correct Formatting:

.. code:: C

    if (!is_valid(ni)) {
        ... // program code
    }

    repssn_ind = ssn_data[index].repssn_index;
    repssn_ni = ssn_data[index].ni; 

- Break long statements (>120 characters) into multiple lines. Place the operators at the beginning of new lines and ensure proper indentation for readability.

Example:

.. code:: C

    perm_count_msg.head.len = NO7_TO_STAT_PERM_COUNT_LEN
                        + STAT_SIZE_PER_FRAM * sizeof( _UL );
    act_task_table[frame_id * STAT_TASK_CHECK_NUMBER + index].occupied
                        = stat_poi[index].occupied;
    act_task_table[taskno].duration_true_or_false
                        = SYS_get_sccp_statistic_state( stat_item );
    report_or_not_flag = ((taskno < MAX_ACT_TASK_NUMBER)
                        && (n7stat_stat_item_valid (stat_item))
                        && (act_task_table[taskno].result_data != 0));

- If there are long expressions or statements in loops, judgments, and other statements, adaptive partitioning should be performed. Long expressions should be partitioned into new rows at low priority operators, and operators should be placed at the beginning of the new rows.

Example:

.. code:: C

    if ((taskno < max_act_task_number)
        && (n7stat_stat_item_valid (stat_item))) {
        ... // program code
    }
    for (i = 0, j = 0; (i < BufferKeyword[word_index].word_length)
        && (j < NewKeyword.word_length); i++, j++) {
        ... // program code
    }


- Statements such as ``if``、 ``for``、 ``do``、 ``while``、 ``case``、 ``switch``、 ``default`` , etc. occupy a separate line and must be enclosed in parentheses {} regardless of the amount of execution. Except for the case, the left bracket is placed at the end of the line, the right bracket is placed at the beginning of the line, and the curly braces and preceding statements, as well as if and following statements, all retain a space`` Start a new line for 'case' and keep it aligned.

Example of Incorrect Formatting:

.. code:: C

    if (p_user == NULL) return;

    switch (event) {
        case event1:
            ... // program code
            break;
        case event2:
            ... // program code
            break;
        default:
            ... // program code
            break;
    }

Correct Formatting:

.. code:: C

    if (p_user == NULL) {
        return;
    }

    switch (event) {
        case event1:
        {
            ... // program code
            break;
        }
        case event2:
        {
            ... // program code
            break;
        }
        default:
        {
            ... // program code
            break;
        }
    }

- Use spaces for alignment, not TAB key.

.. hint::
    To avoid uneven program layout caused by different numbers of spaces set by the TAB key when reading programs with different editors.

    The code in statements such as the beginning of functions or procedures, the definition of structures, loops, and judgments should all adopt indentation style, and the handling statements in case statements should also comply with statement indentation requirements.


- When performing operations between two or more keywords, variables, or constants, spaces should be added before, after, or around the operators. For non-equal operations, if the operator is closely related (such as ``->`` ), a space should not be added after it.

.. hint::
    The purpose of writing code in this relaxed manner is to make the code clearer.

    Since the clarity provided by spaces is relative, there is no need to add spaces in already very clear statements. If the statement is clear enough, spaces are not needed inside parentheses (i.e., after the left parenthesis and before the right parenthesis). Multiple parentheses do not need spaces because, in C/C++ languages, parentheses are already the clearest marker.

    In long statements, if too many spaces are needed, the overall clarity should be maintained, and spaces should not be added locally. When adding spaces around operators, do not leave more than one space consecutively.



C Code Style
-----------------

.. highlight:: c

.. _style-guide-naming:


Vertical Space
"""""""""""""""""
Place one empty line between functions. Do not begin or end a function with an empty line.

.. code:: C

    void function1()
    {
        wifi_one_thing();
        wifi_another_thing();
                                    // INCORRECT, do not place empty line here
    }
                                    // place empty line here
    void function2()
    {
                                    // INCORRECT, do not use an empty line here
        int var = 0;
        while (var < WM_EVENT_WIFI) {
            wifi_stuff(&var);
        }
    }
    
Horizontal Space
"""""""""""""""""

- Blank lines must be added between relatively independent program blocks and after variable declarations. If possible, please set it to left alignment in the editor. The example is as follows:

.. code:: C
    
    int    conn_fd;
    int    ret;
    
    conn_fd = socket(AF_INET, SOCK_STREAM,0);
    if (conn_fd < 0) {
        perror("socket create");
    }

- Do not write multiple short statements on a single line.

.. code:: C

    /*The following statement is non-standard:*/
    min_port = 1;   max_port = 65535;
    /*It should be written as follows:*/
    min_port = 1;   
    max_port = 65535;

- Long statement with multiple lines of writing

For longer statements (exceeding 80 characters), they should be written on multiple lines, and the new lines should be appropriately indented to make the layout neat and the statement readable. For functions with long parameters, they should also be divided into multiple rows. The example is as follows:

.. code:: C

    ret = connect(conn_fd, 
                (struct sockaddr *)&serv_addr, 
                sizeof (struct sockaddr));

Control statement keywords
"""""""""""""""""""""""""""""
Statements such as ``if``、 ``for``、 ``do``、 ``while``、 ``case``、 ``switch``、 ``default`` , etc. each occupy one line, and the execution part of statements such as ``if``、 ``for``、 ``do``、 ``while`` must be marked with ``{ }`` no matter how much they are. The example is as follows:

.. code:: C

    /*The following statement is non-standard:*/
    if (conn_fd < 0) perror("socket create");
    /*It should be written as follows:*/
    if (conn_fd < 0) {
        perror("socket create");
    } else {
        /* do somethings */
    }

Indentation
"""""""""""""

- Statements within functions, structure definitions, loops, and code in ``if`` statements should all follow an indentation style. The statements following ``case`` should also be indented.
- Program blocks should be written with an indentation style, using 4 spaces for indentation. This increases program readability, and the use of the Tab key is prohibited. An example is as follows:

.. code:: C

    typedef struct _port_segment {
        struct in_addr     dest_ip;    /* struct is indented 4 spaces relative to typedef */
        unsigned short int min_port;
        unsigned short int max_port;
    } port_segment;

    if (conn_fd < 0) {
        perror("socket create");    /*  perror is indented 4 spaces */
    }

    for (i=portinfo.min_port; i<=portinfo.max_port; i++) {
        serv_addr.sin_port = htons(i); /* serve_addr.sin_port is indented 4 spaces */
    }


Space
""""""

1. Commas and semicolons should only be followed by a space. Example:

.. code:: C

    int min port, max port;

2. Comparison operators, assignment operators ``=`` 、 ``+=`` , arithmetic operators ``+`` 、 ``%`` , logical operators ``&&`` 、 ``&`` , bitwise operators ``<<`` 、 ``^`` , and other binary operators should have spaces before and after them. Example:

.. code:: C

    if (current_time >= MAX_TIME_VALUE)
    a = b + c;
    a *= 2;
    a = b ^ 2;

3. Comparison operators such as ``<`` 、 ``>`` should have a space on both sides. Example:

.. code:: C

    if (conn fd < 0)

4. Unary operators such as ``!`` 、 ``～`` 、 ``++`` 、 ``--`` 、 ``&`` (address operator) **should not** have spaces before or after them. Example:

.. code:: C

    *p = 'a'; // Dereference operator "*" and the content
    flag = !is_empty; // Negation operator "!" and the content
    p = &mem; // Address operator "&" and the content
    i++; // "++" and "--" and the content

5. The ``->`` 、 ``.`` operators **should not** have spaces before or after them. Example:

.. code:: C

    portinfo.min_port = i * seg_len + 1;

6. Keywords such as ``if``、 ``for`` 、 ``while`` 、 ``switch`` , etc., should be followed by a space before the opening parenthesis to make the keywords more prominent and clear. Example:

.. code:: C

    if (cinn fd < 0)

- The following keywords **should not** have a space after them:： ``sizeof`` , ``typeof`` , ``alignof`` , ``__attribute`` 

Identifiers, Variables, Macros
------------------------------------

- Naming identifiers should have a consistent style that should not be changed arbitrarily unless required for a team project to follow a unified style. For uncertain or disputed naming, discuss and decide within the project team.
- Newly written code must use the basic data types defined in ``wm_types.h`` for defining variables. Third-party applications can retain their code style within the scope of their app.
- Identifier names should be clear and meaningful, using complete words or commonly understood abbreviations to avoid misunderstandings.
- Avoid using single-character variable names (such as ``i`` 、 ``j`` 、 ``k`` ), but ``i`` 、 ``j`` 、 ``k`` are allowed as local loop variables.。
- Global variables should have the  ``g_``   prefix, and constants that do not require modification should be marked with ``const`` to reduce system memory consumption.

Naming Conventions and Styles
"""""""""""""""""""""""""""""""
- Naming conventions should be consistent with the system style.  We adopt the ``Linux`` style for variable naming, generally using lowercase with underscores, where each segment has an independent meaning.

::

    /*Generally used:*/
    int min_port ;
    /*Generally not used:*/
    int minPort ;

.. danger:: Other than in header files or structure definitions, do not use definitions that start and end with an underscore, like   ``_ourhead_h_`` .
 
The following example is allowed:

.. code:: C

    #ifndef  _ourhead_h_
    #define _ourhead_h_
    ...
    #endif

    typedef struct _port_segment {
        struct in_addr          dest_ip;     
        unsigned short int      min_port;
        unsigned short int      max_port;
    } port_segment;

- Pay attention to operator precedence and use parentheses to clearly express the order of operations.。 ``if`` statements, macros, statements with multiple operators, and intermediate expressions must use parentheses. Example:

::

    if ((a | b) < (c & d))

- Avoid naming local variables the same as global variables. Even though their scopes are different and do not cause syntax errors, it can cause confusion.
- Avoid using numbers that are not easy to understand. Use meaningful identifiers instead. For constants, do not use numbers directly; use meaningful enums or macros. Example:

.. code:: C

    #define BUFF_SIZE          1024
    input_data = (char *)malloc(BUFF_SIZE);
    /* Avoid code like this:*/
    p = (char *)malloc(1024);

- Do not use overly complex and hard-to-understand statements unless absolutely necessary. Example:

.. code:: C

    /*Avoid code like this:*/
    count ++ += 1;
    /*Instead, write:*/
    count  += 1;
    count++;

- When using global variables, clearly define their meaning, purpose, and range of values. Non-public global variables should be prefixed with the  ``static`` keyword to restrict their use to the file. Example:

.. code:: C

    static u16 g_napt_curr_port; /* Indicates the port number to be allocated, range 0-65535 */  

- Do not use uninitialized variables as right-hand values. In C programs, referencing an unassigned pointer often causes crashes. Variables must be initialized upon definition, but performance-critical code sections may skip this initialization.

.. code:: C

    char *p_string;
    p_sting[0] = 'a';
    /*Should be initialized first: */
    char *p_string;
    p_string = (char *)malloc(BUFF_SIZE);          /* Assuming BUFF_SIZE is defined */
    p_sting[0] = 'a';    
    
- Use complete parentheses when defining expressions with macros.

.. code:: C

    /*This macro definition is risky:*/
    #define GET_AREA(a,b)     a*b
    /*It should be defined as:*/
    #define GET_AREA(a,b)     ((a) * (b))

- Avoid using ``return`` 、 ``goto`` 、 ``continue`` 、 ``break`` , and other statements that change the program flow within macros.

.. hint:: Using such flow-changing statements in macros can easily cause resource leaks, and users may not easily detect them.

Example: Define the ``CHECK_AND_RETURN``  macro in a header file:

.. code:: C

    #define CHECK_AND_RETURN(cond, ret) {if (cond == NULL_PTR) {return ret;}} 

Then use it in a function (for demonstration purposes, the code is not complete):

.. code:: C

    p_mem1 = malloc(size);
    CHECK_AND_RETURN(p_mem1, ERR_CODE_XXX) 
    p_mem2 = malloc(size);
    CHECK_AND_RETURN(p_mem2, ERR_CODE_XXX) 

If  ``p_mem2 == NULL`` ， the function returns without freeing ``p_mem1`` , causing a memory leak. Therefore, macros like ``CHECK_AND_RETURN`` , while making the code concise, carry significant risks and should be used cautiously.

- If a macro contains multiple statements, enclose them in curly braces.。

In the following statement, only the first expression of the macro is executed.

.. code:: C

    #define INTI_RECT_VALUE( a, b )\
    a = 0;\
    b = 0;
    for (index = 0; index < RECT_TOTAL_NUM; index++)
                INTI_RECT_VALUE( rect.a, rect.b );
    //The correct usage should be:：
    #define INTI_RECT_VALUE( a, b ) {\
                a = 0;\
                b = 0;\
    }
    for (index = 0; index < RECT_TOTAL_NUM; index++) {
        INTI_RECT_VALUE( rect[index].a, rect[index].b );
    }

- File naming should follow the ``wm_filename`` format.

SDK Documentation Abbreviation
""""""""""""""""""""""""""""""""""

Example:

===============   ===============
Variable name       Abbreviation
===============   ===============
loop variable           i、j、k
temporary              tmp
message               msg
buffer                 buf
data                  data
length                 Len
width                  width
callback             callback
interrupt               irq
middle                 mid
maximum                max
min                    min
return value             ret
error value             err
parameter             arg
offset               offset
source              src
destination         dst
current             cur
configuration         cfg
packet              pkt
address              addr
count               count
retry              retry
index               index
sum                  sum
receive               rx
transmit               tx
power save               ps
request               req
respone               resp
frequency              freq
pointer               p
State                state
status              status
Flag                 flag
socket            skt、skt_number
Ssid length          ssid_len
keyword              key
key length           key_len
command              cmd
duty                duty
cycle              cycle
===============   ===============

Enumerations
--------------

Enumerations should be named according to their type and specific function. Example:

.. code:: C

    typedef enum {
        WM_EVENT_WIFI_SCAN_DONE = 0,    
        WM_EVENT_WIFI_STA_CONNECTED,                            
        WM_EVENT_WIFI_STA_DISCONNECTED,                         
        WM_EVENT_WIFI_STA_BSS_RSSI_LOW,                         
        WM_EVENT_WIFI_STA_BEACON_TIMEOUT,                       
    } wm_event_type_t;

Functions
----------
Function Naming
"""""""""""""""""
Function names should accurately describe their function, typically using a verb-object format. Public functions should follow the  `system_identifier_module_name_function_description` format, while non-public functions should follow the `module_name_function_description` format.

.. code:: C

    void wm_dma_print_record(struct *p_record, int record_len)

    static dma_alloc_resource(int param) 

Function Implementation
""""""""""""""""""""""""
- A function should accomplish a specific task and should not try to perform multiple unrelated tasks.
- Validate all input parameters of a function; for example, check if pointer parameters are NULL and if array indices are within bounds.
- Eliminate compiler warnings for unused input parameters by casting them to void, such as (void)a.
- The return value of a function should be clear and unambiguous, making it difficult for the user to ignore error conditions. The meaning of each error return value should be clear and precise to prevent misuse or misunderstanding by the user.

.. hint:: If multiple code segments repeat the same operation, consider implementing the repeated functionality as a single function.
.. hint:: When writing a function, strive to improve its independence, reduce its connections with other functions, and enhance code readability, maintainability, and efficiency.

Function Constraints
""""""""""""""""""""""
- Non-public functions should be restricted to file scope by adding the ``static`` keyword.
- A function should be limited to 200 lines, including blank lines and comments.
- A function should not have more than 5 parameters; if more are needed, design a data structure to encapsulate them.。
- Function names should ideally not exceed 30 characters.

Function Calls
""""""""""""""""
Minimize recursion within a function or between functions.

.. hint:: Recursion, especially between functions (e.g., A->B->C->A),reduces the program's comprehensibility, often consumes more system resources (such as stack space), and complicates program testing.


Program Efficiency
---------------------
When programming, it's important to keep an eye on code efficiency. Variables used on the task stack should be as small as possible, and debugging information should be added with consideration to `code` size. Aim to minimize program size while ensuring functionality.

Efficiency Classification
""""""""""""""""""""""""""

Efficiency can be classified into code efficiency、global efficiency、local efficiency、time efficiency、space efficiency.

1. Code efficiency is divided into global efficiency, local efficiency, time efficiency, and space efficiency.
#. Global efficiency refers to the efficiency from the perspective of the entire system;
#. Local efficiency refers to the efficiency from the perspective of modules or functions;
#. Time efficiency refers to the amount of time required for a program to process input tasks;
#. Space efficiency refers to the memory space required by a program, such as the size of machine code space, data space, stack space, etc.

Principles for Improving Efficiency
""""""""""""""""""""""""""""""""""""

Improve code efficiency while ensuring the correctness, stability, readability, and maintainability of the software system.

.. hint:: We cannot blindly pursue code efficiency and cause damage to the correctness, stability, readability, and maintainability of the software.

Method for Improving Efficiency
""""""""""""""""""""""""""""""""


1. Improve spatial efficiency by partitioning and organizing data structures, as well as optimizing program algorithms.
2. The workload within the circulatory system should be minimized. The example is as follows:

.. code:: C

    /*Less efficient code:*/
    for (ind = 0; ind < MAX_ADD_NUMBER; ind++) {
        sum += ind;
        back_sum = sum; /* backup sum */
    }
    /*Statement back_stum=sum; It can be placed after the for statement, as follows：*/
    for (ind = 0; ind < MAX_ADD_NUMBER; ind++) {
        sum += ind;
    }
    back_sum = sum; /* backup sum */

3. Put the most time-consuming loops in the innermost layer to reduce `CPU` context switching. For example:

.. code:: C

    /*Less efficient code:*/
    for (row = 0; row < 100; row++) {
        for (col = 0; col < 5; col++) {
            sum += a[row][col];
        }
    }

    /*Improved code:*/
    for (col = 0; col < 5; col++) {
        for (row = 0; row < 100; row++) {
            sum += a[row][col];
        }
    }

4. Avoid Conditional Statements in Loops: Move conditional statements out of loops to reduce the number of evaluations. For example:

.. code:: C

    /*Less efficient code:*/
    for (ind = 0; ind < MAX_RECT_NUMBER; ind++) {
        if (data_type == RECT_AREA) {
            area_sum += rect_area[ind];
        }
        else {
            rect_length_sum += rect[ind].length;
            rect_width_sum += rect[ind].width;
        }
    }
    /*Since the judgment statement is independent of loop variables, the following improvements can be made to reduce the number of judgments:*/
    if (data_type == RECT_AREA) {
        for (ind = 0; ind < MAX_RECT_NUMBER; ind++) {
            area_sum += rect_area[ind];
        }
    }     
    else {
        for (ind = 0; ind < MAX_RECT_NUMBER; ind++) {
            rect_length_sum += rect[ind].length;
            rect_width_sum  += rect[ind].width;
        }
    }

Code Commenting Style
------------------------

- Comments should adhere to the `doxygen <https://www.doxygen.nl/manual/install.html>`_ standards.  `doxygen`  can generate readable documentation from well-structured comments, making it easier to review and reference later.
- Write and update comments alongside your code to ensure they remain accurate and consistent. Remove any obsolete comments promptly.
- Comments should not generate compilation warnings. Use Doxygen to compile and check for any such warnings.

Block Comments
""""""""""""""""

For block comments, use the following style:

.. code:: C

    /**
      * ………………
      */

Line Comments
""""""""""""""

For single-line comments, use one of the following styles:

.. code:: C

    // c99 c++ style

    /* c style */

File Comments
""""""""""""""

File comments should be placed at the top of the file. Use the following format:

.. code:: C

    /**
     * @file     filename
     * @brief    Brief description
     * @details  Detailed description
     * @mainpage Project overview
     * @author   Author
     * @email    Email
     * @version  Version
     * @date     YYYY-MM-DD
     * @license  License
     */

Function Comments
--------------------

Concise Comments
""""""""""""""""""

Function comments should include a brief description( `@brief` ), parameter explanations ( `@param` ), return explanation( `@return` )and return value explanation( `@retval`  ). Example:：

.. code:: C
    
    /**
     * @brief      Brief description of the function
     *
     * @param      Description of formal parameter parameters
     * @param      Description of formal parameter parameters
     * @return     Description of the return value
     * @retval     Description of possible return values
     */

Detailed Comments
""""""""""""""""""

Detailed function comments can include additional sections like detailed description ( `@detail` ), notes( `@note` ), attention points( `@attention` ), warnings( `@warning` )或者异常( `@exception` ), and exceptions. Example:

.. code:: C

    /**
     * @brief      Brief description of the function
     * @detail     Detailed explanation of the function's behavior
     * 
     * @param      Description of formal parameter parameters
     * @param      Description of formal parameter parameters
     * @return     Description of the return value
     * @retval     Description of possible return values
     * @note       Additional notes about the function 
     * @attention  Important points to be aware of
     * @warning    Warnings about potential issues
     * @exception  Possible exceptions thrown by the function
     */

Enum Comments
""""""""""""""

For enums, use the following commenting style:

.. code:: C

    /**  ………………*/  
    enum COLOR {
        RED    = 0, /**< red. */ 
        GREEN  = 1, /**< green. */
        YELLOW = 2  /**< yellow. */
    };

Macro Definition Comments
"""""""""""""""""""""""""""""

Comments for macros can be placed before or after the macro definition, according to preference:

.. code:: C

    ///buf size
    #define BUFSIZE (1024 * 2)

    #define BUFSIZE (1024 * 2)          ///buf size


Others
--------

The following annotations can be chosen based on personal needs.

============  ===============     ================================
Command          Field Name             Explanation
============  ===============     ================================
@see           reference    
@class         class               Used for generating document connections
@var           variable             Used for generating document connections
@enum          enumeration             Used for generating document connections
@code          code start           Paired with @ endcode
@endcode       code end           Paired with @ code for use
@bug           bug                 Link to the defect list that summarizes all defects
@todo          TODO                 Link to the TODO list that summarizes all TODOs
@example       example
@remarks       remarks    
@pre           precondition     
@deprecated    deprecate
============  ===============     ================================

Code Quality and Safety
--------------------------

Classification
"""""""""""""""

Correctness (performing the intended function), stability, safety, testability, maintainability, readability, and efficiency of the code.

Precautions
""""""""""""

- Prevent referencing already freed memory.

.. hint:: In actual programming, it's easy to accidentally free a memory block in one module while another module references it later. This situation should be prevented.

- Memory allocated in a function should be freed before the function ends.

Especially for memory allocated in asynchronous operations, make sure to release it. Try to ensure that memory allocated in a task is released within that task.

- Prevent memory operations from exceeding bounds.

Memory operations mainly refer to the manipulation of arrays, pointers, memory addresses, etc. Memory operation out of bounds is one of the major errors in software systems, which can cause program crashes or leave security vulnerabilities.

.. code:: C

    /* The following code operates on array[10], causing out-of-bounds access: */
    int array[10], i;
    for (i = 1; i <= 10; i++) {
        array[i] = 10;
    } /* The compiler cannot detect such errors and considers them legal.*/

- After the program is written, check confusing operators such as ``==`` and ``=`` , ``&&`` and ``&`` , ``||`` and ``|`` .
 
.. danger:: New code must eliminate warnings.

- Pay attention to whether expressions will overflow or underflow. Variables involved in operations must be defined as signed types. For example:

.. code:: C

    unsigned char size = 5;

    while (size-- >= 0) {       /* Underflow will occur */
        ... /* Program code */
    }

When size is 0 and decrements by 1, the value becomes -1. In the system, -1 is represented as 0xFF, and 255 is also represented as 0xFF. Since size is an unsigned number, the system considers the value to be 255, resulting in an infinite loop. Modify it as follows:
    
.. code:: C
    
    char size;      // Change unsigned char to char

    while (size-- >= 0) {
        ... /* Program code */
    }

- Initialize relevant variables and the operating environment at the start of the system to prevent referencing uninitialized variables.
- Avoid using the ``goto``  statement. The ``goto`` statement can break the structure of the program unless absolutely necessary.
- Adhere to the following measures to prevent memory out-of-bounds: 

1. Consider the maximum size for arrays to avoid insufficient space allocation;
2. Avoid using dangerous functions like sprintf / vsprintf / strcpy / strcat / gets for string operations; use relatively safe functions like snprintf / strncpy / strncat / fgets instead;
3. Ensure the length does not exceed bounds when using memcpy / memset;
4. Consider the final '\0' for strings and ensure all strings end with '\0';
5. Consider the pointer type length when performing pointer addition/subtraction operations;
6. Check array indices;
7. Use sizeof or strlen to calculate structure/string length to avoid manual calculation;

- Adhere to the following measures to prevent memory leaks: 

1. Check if memory, timers / file handles / Sockets / queues / semaphores / GUI resources are fully released at abnormal exits;
2. When deleting structure pointers, delete from the bottom layer to the top layer in sequence;
3. Ensure each element pointer in the array has been released in advance when releasing pointer arrays;
4. Avoid re-allocating memory;
5. Be careful with macros containing return and break statements to ensure resources are released beforehand;
6. Check if each member in the queue is released;
7. Set the pointer to NULL after releasing memory;

**CMake** Code Style
---------------------

- Indent with four spaces.
- Maximum line length 120 characters. When splitting lines, try to focus on readability where possible (for example, by pairing up keyword/argument pairs on individual lines).
- Do not put anything in the optional parentheses after ``endforeach()`` 、 ``endif()`` , etc.
- Use lowercase for commands, functions, and macro names.
- Use lowercase for local scope variables.
- Use uppercase for global scope variables.
- Separate two independent statements with a blank line, and start independent statements at the beginning of a line, with the right bracket on a separate line and aligned.

Example:
::

 list(APPEND ADD_LINK_SEARCH_PATH "${CONFIG_TOOLCHAIN_PATH}/lib"
                                  )

 list(APPEND ADD_STATIC_LIB "lib/libtest.a"
                            )


Kconfig Configuration Files
----------------------------

The indentation for all Kconfig* configuration files throughout the source tree varies. The lines immediately following the config definitions are indented with 4 spaces, while the help information is indented with an additional 4 spaces. For example:

::

    config AUDIT
        bool "Auditing support"
        depends on NET
        help
            Enable auditing infrastructure that can be used with another
            kernel subsystem, such as SELinux (which requires this for
            logging of avc messages output).  Does not do system-call
            auditing without CONFIG_AUDITSYSCALL.

Dangerous features (like write support for certain file systems) should prominently declare this in their prompt strings:
::

    config ADFS_FS_RW
        bool "ADFS write support (DANGEROUS)"
        depends on ADFS_FS
        ...

Third-party Component Code Style
---------------------------------

The WM IoT SDK integrates many third-party open-source software components to provide more experiences for users. These third-party open-source components may retain their original code styles.However, it should be noted that in the WM IoT SDK, a switch is added to the corresponding menuconfig to control the DEBUG output of third-party open source software, and the output is redirected to the WM LOG module, which means that the third-party DEBUG output will be limited by the logging level of WM LOG. In addition, for modules without log priority configuration, such as LWIP, the default output level is DEBUG.

Submission
-----------

In any case, code for formal submission should be checked against the following requirements.

- [ ] No compiler warnings should exist
- [ ] No static analysis warnings should exist; the static analysis tool is tentatively cppcheck
- [ ] Coding standards should meet the aforementioned requirements
- [ ] Coding efficiency should meet the aforementioned requirements
- [ ] Coding security should meet the aforementioned requirements


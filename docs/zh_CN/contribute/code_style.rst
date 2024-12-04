:orphan:

代码风格
===============================================

关于本指南
----------------
WM IoT SDK 的编程代码风格大体上遵循 linux kernel 编程规范，并进行了一些修改。

本风格指南的目的是鼓励在WM IoT SDK中使用通用编码实践。

为了提高源程序的质量和可维护性，从而最终提高软件产品生产力，特编写此规范。本标准规定了程序设计人员进行程序设计时必须遵循的规范。本规范主要针对 WM IoT SDK C 语言编程。规范的基本规则为：格式清晰、注释简明扼要、命名规范易懂、函数模块化、程序易读易维护、功能准确实现、代码空间效率和时间效率高、适度的可扩展性。

排版
-----------

- 程序块要采用缩进风格编写，缩进的空格数为4个。
- 相对独立的程序块之间、变量说明之后必须加空行。

示例：如下例子不符合规范。

.. code:: C

    if (!is_valid(ni)) {
        ... // program code
    }
    repssn_ind = ssn_data[index].repssn_index;
    repssn_ni = ssn_data[index].ni;

应如下书写：

.. code:: C

    if (!is_valid(ni)) {
        ... // program code
    }

    repssn_ind = ssn_data[index].repssn_index;
    repssn_ni = ssn_data[index].ni; 

- 较长的语句（>120字符）要分成多行书写，长表达式要在低优先级操作符处划分新行，操作符放在新行之首，划分出的新行要进行适当的缩进，使排版整齐，语句可读。

示例：

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

- 循环、判断等语句中若有较长的表达式或语句，则要进行适应的划分，长表达式要在低优先级操作符处划分新行，操作符放在新行之首。

示例：

.. code:: C

    if ((taskno < max_act_task_number)
        && (n7stat_stat_item_valid (stat_item))) {
        ... // program code
    }
    for (i = 0, j = 0; (i < BufferKeyword[word_index].word_length)
        && (j < NewKeyword.word_length); i++, j++) {
        ... // program code
    }


- ``if``、``for``、``do``、``while``、``case``、``switch``、``default`` 等语句自占一行，且执行语句部分无论多少都要加括号{ }。其中除 case 外左括号放在行末，把右括号放在行首，并且大括号和前面的语句，以及if和后面的语句，都保留一个空格。``case`` 的 { 另起一行，保持对齐。

示例：如下例子不符合规范。

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

应如下书写：

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

- 对齐只使用空格键，不使用TAB键。

.. hint::
    以免用不同的编辑器阅读程序时，因TAB键所设置的空格数目不同而造成程序布局不整齐。

    函数或过程的开始、结构的定义及循环、判断等语句中的代码都要采用缩进风格，case语句下的情况处理语句也要遵从语句缩进要求。


- 在两个以上的关键字、变量、常量进行对等操作时，它们之间的操作符之前、之后或者前后要加空格；进行非对等操作时，如果是关系密切的立即操作符（如 ``->`` ），后不应加空格。

.. hint::
    采用这种松散方式编写代码的目的是使代码更加清晰。

    由于留空格所产生的清晰性是相对的，所以在已经非常清晰的语句中没有必要再留空格，如果语句已足够清晰则括号内侧(即左括号后面和右括号前面)不需要加空格，多重括号间不必加空格，因为在C/C++语言中括号已经是最清晰的标志了。

    在长语句中，如果需要加的空格非常多，那么应该保持整体清晰，而在局部不加空格。给操作符留空格时不要连续留两个以上空格。



C代码风格
-----------------

.. highlight:: c

.. _style-guide-naming:


垂直空间
""""""""
在函数之间放置一个空行。不要用空行开始或结束函数。

.. code:: C

    void function1()
    {
        wifi_one_thing();
        wifi_another_thing();
                                    // 错误, 这里不能使用空行结束函数体
    }
                                    // 函数之间需要放置一个空行
    void function2()
    {
                                    // 错误, 这里不能使用空行开始函数体
        int var = 0;
        while (var < WM_EVENT_WIFI) {
            wifi_stuff(&var);
        }
    }
    
水平空间
""""""""

- 相对独立的程序块之间、变量声明之后必须加空行。如有可能请在编辑器中设置为左对齐。示例如下：

.. code:: C
    
    int    conn_fd;
    int    ret;
    
    conn_fd = socket(AF_INET, SOCK_STREAM,0);
    if (conn_fd < 0) {
        perror("socket create");
    }

- 一行只写一条语句，不允许把多个短语句写在一行中。示例如下：

.. code:: C

    /*以下语句是不规范的：*/
    min_port = 1;   max_port = 65535;
    /*应该如下书写：*/
    min_port = 1;   
    max_port = 65535;

- 长语句多行书写

对于较长的语句（超过个80字符）要分成多行书写，划分出的新行要进行适当的缩进，使排版整齐，语句可读。对于参数较长的函数也要划分成多行。示例如下：

.. code:: C

    ret = connect(conn_fd, 
                (struct sockaddr *)&serv_addr, 
                sizeof (struct sockaddr));

控制语句关键字
""""""""""""""
``if``、``for``、``do``、``while``、``case``、``switch``、``default`` 等语句各自占一行，且 ``if``、``for``、``do``、``while`` 等语句的执行语句部分无论多少都要加 ``{ }``。示例如下：

.. code:: C

    /*以下语句是不规范的：*/
    if (conn_fd < 0) perror("socket create");
    /*应该如下书写*/
    if (conn_fd < 0) {
        perror("socket create");
    } else {
        /* do somethings */
    }

缩进
""""

- 函数内的语句、结构的定义、循环和 ``if`` 语句中的代码都要采用缩进风格， ``case`` 语句后的处理语句也要缩进。
- 程序块要采用缩进风格编写，缩进为4个空格，可以增加程序的可读性,禁止使用Tab键。示例如下：

.. code:: C

    typedef struct _port_segment {
        struct in_addr     dest_ip;    /* struct相对于typedef缩进4个字符 */
        unsigned short int min_port;
        unsigned short int max_port;
    } port_segment;

    if (conn_fd < 0) {
        perror("socket create");    /* perror缩进4个字符 */
    }

    for (i=portinfo.min_port; i<=portinfo.max_port; i++) {
        serv_addr.sin_port = htons(i); /* serve_addr.sin_port缩进4个字符 */
    }


空格
""""

1. 逗号、分号只在后面加空格。示例如下：

.. code:: C

    int min port, max port;

2. 比较操作符, 赋值操作符 ``=`` 、 ``+=`` ，算术操作符 ``+`` 、 ``%`` ，逻辑操作符 ``&&`` 、 ``&`` ，位域操作符 ``<<`` 、 ``^`` 等双目操作符的前后加空格。示例如下：

.. code:: C

    if (current_time >= MAX_TIME_VALUE)
    a = b + c;
    a *= 2;
    a = b ^ 2;

3. ``<`` 、 ``>`` 等比较操作符两边都有一个空格。示例如下：

.. code:: C

    if (conn fd < 0)

4. ``!`` 、 ``～`` 、 ``++`` 、 ``--`` 、 ``&`` （地址运算符）等单目操作符前后不加空格。示例如下：

.. code:: C

    *p = 'a'; // 内容操作"*"与内容之间
    flag = !is_empty; // 非操作"!"与内容之间
    p = &mem; // 地址操作"&" 与内容之间
    i++; // "++","--"与内容之间

5. ``->`` 、 ``.`` 前后不加空格。示例如下：

.. code:: C

    portinfo.min_port = i * seg_len + 1;

6. ``if``、 ``for`` 、 ``while`` 、 ``switch`` 等与后面的括号间应加空格，以便使 ``if`` 、 ``for`` 等关键字更为突出和明显。示例如下：

.. code:: C

    if (cinn fd < 0)

- 下列 keywords 后面 *不要* 追加一个空格： ``sizeof`` , ``typeof`` , ``alignof`` , ``__attribute`` 

标识符、变量、宏
------------------

- 对于标识符的命名，要有自己的风格，一旦形成不可随意变更，除非团队项目开发中要求使用统一的风格。对于无法确定或者有争论的命名可以提出项目组讨论确定。
- 新编写的代码必须要采用 ``wm_types.h`` 定义的基本数据数据类型来定义变量。移植的第三方app可在其app范围保持其代码风格不变。
- 标识符的命名要清晰明了，有明确含义，同时使用完整的单词或大家基本可以理解的缩写，避免使人产生误解。
- 对于变量命名，禁止使用单个字符（如 ``i`` 、 ``j`` 、 ``k`` ），但 ``i`` 、 ``j`` 、 ``k`` 作为局部循环变量是允许的。
- 全局变量应增加“``g_`` ” 前缀，对于不需要修改的全局变量增加 ``const`` 修饰，以减少对系统内存的消耗。

命名规范和命名风格
"""""""""""""""""""
- 命名规范必须与所使用的系统风格保持一致，我们采用 ``Linux`` 下变量命名方式，一般是全小写加下划线的风格，每个分段都可以看出独立的意义。

::

    /*一般使用：*/
    int min_port ;
    /*一般不使用：*/
    int minPort ;

.. danger:: 除了头文件或结构体定义，其余地方禁止使用   ``_ourhead_h_`` 之类以下划线开始和结尾的定义。
 
 以下示例是允许的：

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

- 注意运算符的优先级，并用括号明确表达式的操作顺序。 ``if`` 语句、宏、多个计算符连接的语句、中间表达式等必须使用括号。示例如下：

::

    if ((a | b) < (c & d))

- 防止局部变量与全局变量同名。尽管局部变量和全局变量的作用域不同而不会发生语法错误，但容易使人误解。
- 避免使用不易理解的数字，用有意义的标识来替代。对于常量，不应直接使用数字，必须用有意义的枚举或宏来代替。示例如下：

.. code:: C

    #define BUFF_SIZE          1024
    input_data = (char *)malloc(BUFF_SIZE);
    /*而应避免出现类似以下的代码：*/
    p = (char *)malloc(1024);

- 不要使用难懂的技巧性很高的语句，除非很有必要时。示例如下：

.. code:: C

    /*不应出现类似以下的代码：*/
    count ++ += 1;
    /*而应改为：*/
    count  += 1;
    count++;

- 使用全局变量时，应明确其含义、作用、取值范围。不对外公开的全局变量，要在其前添加 ``static`` 关键字将其限定在本文件内使用。示例如下：

.. code:: C

    static u16 g_napt_curr_port; /* 表示要分配的端口号，范围0-65535 */  

- 严禁使用未经初始化的变量作为右值。在C程序中，引用未经赋值的指针，经常会引起程序崩溃，变量定义时必须赋初值，但对性能要求严格的程序段可不赋初值 。

.. code:: C

    char *p_string;
    p_sting[0] = 'a';
    /*应先进行初始化：*/
    char *p_string;
    p_string = (char *)malloc(BUFF_SIZE);          /* 这里假设BUFF_SIZE已定义 */
    p_sting[0] = 'a';    
    
- 用宏定义表达式时，要使用完备的括号。

.. code:: C

    /*如下定义的宏存在一定的风险：*/
    #define GET_AREA(a,b)     a*b
    /*应该定义为：*/
    #define GET_AREA(a,b)     ((a) * (b))

- 宏定义中尽量不使用 ``return`` 、 ``goto`` 、 ``continue`` 、 ``break`` 等改变程序流程的语句。

.. hint:: 如果在宏定义中使用这些改变流程的语句，很容易引起资源泄漏问题，使用者很难自己察觉。

示例：在某头文件中定义宏 ``CHECK_AND_RETURN``：

.. code:: C

    #define CHECK_AND_RETURN(cond, ret) {if (cond == NULL_PTR) {return ret;}} 

然后在某函数中使用(只说明问题，代码并不完整)

.. code:: C

    p_mem1 = malloc(size);
    CHECK_AND_RETURN(p_mem1, ERR_CODE_XXX) 
    p_mem2 = malloc(size);
    CHECK_AND_RETURN(p_mem2, ERR_CODE_XXX) 

此时如果 p_mem2 == NULL ，则 ``p_mem1`` 未释放函数就返回了，造成内存泄漏。所以说，类似于 ``CHECK_AND_RETURN`` 这些宏，虽然能使代码简洁，但是隐患很大，使用须谨慎。

- 若宏中有多条语句，应该将这些语句放在一对大括号中。

下面语句中只有宏的第一条表达式被执行。

.. code:: C

    #define INTI_RECT_VALUE( a, b )\
    a = 0;\
    b = 0;
    for (index = 0; index < RECT_TOTAL_NUM; index++)
                INTI_RECT_VALUE( rect.a, rect.b );
    //正确的用法应为：
    #define INTI_RECT_VALUE( a, b ) {\
                a = 0;\
                b = 0;\
    }
    for (index = 0; index < RECT_TOTAL_NUM; index++) {
        INTI_RECT_VALUE( rect[index].a, rect[index].b );
    }

- 文件命名要遵守 ``wm_文件名`` 的格式。

SDK文档缩写的简写
"""""""""""""""""""""

示例：

===============   ===============
变量名            缩写
===============   ===============
循环变量           i、j、k
临时(temporary)    tmp
消息(message)      msg
缓冲区(buffer)     buf
数据(data)         data
长度(length)       Len
宽度               width
回调函数           callback
中断(interrupt)    irq
中间值(middle)     mid
最大值(maximum)    max
最小值(min)        min
返回值             ret
错误值             err
参数值             arg
偏移               offset
source            src
destination       dst
current            cur
configuration     cfg
packet            pkt
地址              addr
统计              count
重试              retry
索引              index
累加              sum
接收               rx
发送               tx
节能               ps
请求               req
回复               resp
频率              freq
指针               p
状态机的State      state
通常的状态         status
Flag标志           flag
socket            skt、skt_number
Ssid长度          ssid_len
键值              key
key长度           key_len
命令              cmd
占空比            duty
周期              cycle
===============   ===============

枚举
--------

枚举应该通过类型定义和具体功能来命名。示例如下：

.. code:: C

    typedef enum {
        WM_EVENT_WIFI_SCAN_DONE = 0,    
        WM_EVENT_WIFI_STA_CONNECTED,                            
        WM_EVENT_WIFI_STA_DISCONNECTED,                         
        WM_EVENT_WIFI_STA_BSS_RSSI_LOW,                         
        WM_EVENT_WIFI_STA_BEACON_TIMEOUT,                       
    } wm_event_type_t;

函数
----------
函数命名
""""""""
函数名应准确描述函数的功能，一般以动词加宾语的形式命名。对外公开的函数命名要遵守为 `系统标识_模块名称_功能描述` 这样的格式，不对外公开的函数命名要遵守 `模块名称_功能描述` 的格式。

.. code:: C

    void wm_dma_print_record(struct *p_record, int record_len)

    static dma_alloc_resource(int param) 

函数实现
"""""""""
- 一个函数完成一个特定的功能，不应尝试在一个函数中实现多个不相关的功能。
- 检查函数所有输入参数的有效性，比如指针型参数要判断是否为空，数组成员参数判断是否越界。
- 对函数中没有使用的入参需消除编译告警，如参数 a 则可使用 (void) a; 予以消除编译告警。
- 函数的返回值要清楚、明了，让使用者不容易忽视错误情况。函数的每种出错返回值的意义要清晰、明确，防止使用者误用，理解错误或忽视错误返回码。

.. hint:: 如果多段代码重复做同一件事情，那么应该考虑把重复功能实现为一个函数。
.. hint:: 编写函数时应注意提高函数的独立性，尽量减少与其他函数的联系；提高代码可读性、可维护性和效率。

函数限制
""""""""
- 不对外公开的函数，要添加 ``static`` 关键字予以限制在本文件内使用。
- 一个函数的规模应限制在200行以内（包括空行和注释行）。
- 一个函数的参数，不要超过5个，超过的需要设计数据结构包含传入。
- 函数的名字，尽量不要超过30个字符。

函数调用
""""""""
减少函数本身或函数间的递归调用。

.. hint:: 递归调用特别是函数间的递归调用（如 A->B->C->A ），影响程序的可理解性；递归调用一般都占用较多的系统资源（如栈空间）；递归调用对程序的测试不利。


程序效率
------------
编程时要经常注意代码的效率。在任务栈上使用的变量要尽可能的尽量小，调试信息的添加也要考虑 `code` 的大小，尽可能在保证功能前提下减小程序的大小。

效率分类
""""""""

分为代码效率、全局效率、局部效率、时间效率、空间效率。

1. 代码效率分为全局效率、局部效率、时间效率及空间效率。
#. 全局效率是站在整个系统角度上的效率；
#. 局部效率是站在模块或函数角度上的效率；
#. 时间效率是程序处理输入任务所需的时间长短；
#. 空间效率是程序所需内存空间，如机器代码空间大小、数据空间大小、栈空间大小等。

效率提高原则
""""""""""""

在保证软件系统的正确性、稳定性、可读性及可维护性的前提下，提高代码效率。

.. hint:: 不能一味地追求代码效率，而对软件的正确性、稳定性、可读性及可维护性造成损害。

效率提高方法
""""""""""""

1. 通过对数据结构的划分与组织的改进，以及对程序算法的优化来提高空间效率。
2. 循环体内的工作量应最小化。示例如下：

.. code:: C

    /*如下代码效率不高：*/
    for (ind = 0; ind < MAX_ADD_NUMBER; ind++) {
        sum += ind;
        back_sum = sum; /* backup sum */
    }
    /*语句 back_sum = sum; 完全可以放在for语句之后，如下：*/
    for (ind = 0; ind < MAX_ADD_NUMBER; ind++) {
        sum += ind;
    }
    back_sum = sum; /* backup sum */

3. 在多重循环中，应将最忙的循环放在最内层，以减少 `CPU` 切入循环层的次数。示例如下：

.. code:: C

    /*如下代码效率较低：*/
    for (row = 0; row < 100; row++) {
        for (col = 0; col < 5; col++) {
            sum += a[row][col];
        }
    }

    /*可以改为如下方式，以提高效率：*/
    for (col = 0; col < 5; col++) {
        for (row = 0; row < 100; row++) {
            sum += a[row][col];
        }
    }

4. 避免循环体内含判断语句，应将循环语句置于判断语句的代码块之中。示例如下：

.. code:: C

    /*如下代码效率较低：*/
    for (ind = 0; ind < MAX_RECT_NUMBER; ind++) {
        if (data_type == RECT_AREA) {
            area_sum += rect_area[ind];
        }
        else {
            rect_length_sum += rect[ind].length;
            rect_width_sum += rect[ind].width;
        }
    }
    /*因为判断语句与循环变量无关，可作如下改进，以减少判断次数：*/
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

代码注释风格
------------

- 注释风格要使用 `doxygen <https://www.doxygen.nl/manual/install.html>`_ 的规范， `doxygen` 能够将程序中按规则添加的的注释经过处生成可阅读文档，便于日后查看参考。
- 边写代码边注释，修改代码同时修改相应的注释，以保证注释与代码的一致性。无用的注释要及时删除。
- 添加的注释不能存在编译告警，可使用 doxygen 完成一轮编译查看编译过程。

块注释
""""""""""

对于每一块代码的注释建议统一使用以下示例：

.. code:: C

    /**
      * ………………
      */

行注释
""""""""""

对于单个行的注释建议统一使用以下示例：

.. code:: C

    // c99 c++ style

    /* c style */

文件注释
""""""""""

对于文件注释通常放在整个文件的开头。格式使用以下示例：

.. code:: C

    /**
     * @file     文件名
     * @brief    简介
     * @details  细节
     * @mainpage 工程概览
     * @author   作者
     * @email    邮箱
     * @version  版本号
     * @date     年-月-日
     * @license  版权
     */

函数注释
-----------

简约注释
""""""""

函数注释主要包含函数简介( `@brief` )、参数说明( `@param` )、返回说明( `@return` )和返回值说明( `@retval`  )四部分。示例如下：

.. code:: C
    
    /**
     * @brief      函数简介
     *
     * @param      形参 参数说明
     * @param      形参 参数说明
     * @return     返回说明
     * @retval     返回值说明
     */

详细注释
""""""""

对于函数注释的详细注释主要是添加细说明( `@detail` )、注解( `@note` )、注意( `@attention` )、警告( `@warning` )或者异常( `@exception` )等。示例如下：

.. code:: C

    /**
     * @brief      函数简介
     * @detail     详细说明
     * 
     * @param      形参 参数说明
     * @param      形参 参数说明
     * @return     返回说明
     * @retval     返回值说明
     * @note       注解
     * @attention  注意
     * @warning    警告
     * @exception  异常
     */

枚举注释
""""""""""

对于枚举的注释建议使用以下规范示例：

.. code:: C

    /**  ………………*/  
    enum COLOR {
        RED    = 0, /**< red. */ 
        GREEN  = 1, /**< green. */
        YELLOW = 2  /**< yellow. */
    };

宏定义注释
""""""""""""""

注释分为两种代码前注释和代码后注释，根据个人喜好选择，示例如下：

.. code:: C

    ///buf size
    #define BUFSIZE (1024 * 2)

    #define BUFSIZE (1024 * 2)          ///buf size


其他
--------

下面一些标注方式可以根据个人需要选择使用。

============  ===============     ================================
命令           生成字段名             说明
============  ===============     ================================
@see           参考    
@class         引用类               用于文档生成连接
@var           引用变量             用于文档生成连接
@enum          引用枚举             用于文档生成连接
@code          代码块开始           与@endcode成对使用
@endcode       代码块结束           与@code成对使用
@bug           缺陷                 链接到所有缺陷汇总的缺陷列表
@todo          TODO                 链接到所有TODO 汇总的TODO 列表
@example       使用例子说明
@remarks       备注说明    
@pre           函数前置条件     
@deprecated    函数过时说明
============  ===============     ================================

代码质量和安全
--------------

分类
""""""

代码的正确性（实现预期的功能）、稳定性、安全性、可测试性、可维护性、可读性和效率。

注意事项
""""""""""

- 防止引用已经释放的内存空间。

.. hint:: 在实际编程过程中，稍不留心就会出现在一个模块中释放了某个内存块，而另一模块在随后的某个时刻又引用了它，要防止这种情况发生。

- 函数中分配的内存，在函数结束前要释放。

尤其在异步操作流程中分配的内存，更要确保释放，尽量做到在本任务申请的内存在本任务里释放。

- 防止内存操作越界。

内存操作主要是指对数组、指针、内存地址等的操作。内存操作越界是软件系统主要的错误之一，后果是使运行中的程序崩溃，或者留下安全漏洞。

.. code:: C

    /* 以下代码对array[10]进行了操作，导致数组越界访问：*/
    int array[10], i;
    for (i = 1; i <= 10; i++) {
        array[i] = 10;
    } /* 对于类似的错误，编译器不能检测出，认为合法。*/

- 程序编写完成后，应该检查易混淆的操作符，如 ``==`` 和 ``=`` 、 ``&&`` 和 ``&`` 、 ``||`` 和 ``|`` 。
 
.. danger:: 新写的代码必须消告警

- 注意表达式是否会上溢、下溢。符号参与运算的，必须定义为有符号类型。示例如下：

.. code:: C

    unsigned char size = 5;

    while (size-- >= 0) {       /* 将出现下溢 */
        ... /* 程序代码 */
    }

当size等于0时，再减1值为-1。系统中-1表示为0xFF，255也表示为0xFF，由于size为无符号数，系统认为该值为255，故程序是一个死循环。应作如下修改：
    
.. code:: C
    
    char size;      // 从unsigned char 改为char

    while (size-- >= 0) {
        ... /* 程序代码 */
    }

- 系统运行之初，要初始化有关变量及运行环境，防止未经初始化的变量被引用。
- 尽量避免使用 ``goto`` 语句。 ``goto`` 语句会破坏程序的结构性，除非确实需要。
- 坚持下列措施可以避免内存越界： 

1. 数组的大小要考虑最大情况，避免数组分配空间不够；
2. 避免使用危险函数 sprintf /vsprintf/strcpy/strcat/gets 操作字符串，使用相对安全的函数 snprintf/strncpy/strncat/fgets 代替；
3. 使用 memcpy/memset 时一定要确保长度不要越界；
4. 字符串考虑最后的’\0’， 确保所有字符串是以’\0’结束；
5. 指针加减操作时，考虑指针类型长度；
6. 数组下标进行检查；
7. 使用时 sizeof 或者 strlen 计算结构/字符串长度，避免手工计算；

- 坚持下列措施可以避免内存泄漏： 

1. 异常出口处检查内存、定时器 / 文件句柄 / Socket / 队列/信号量 / GUI 等资源是否全部释放；
2. 删除结构指针时，必须从底层向上层顺序删除；
3. 使用指针数组时，确保在释放数组时，数组中的每个元素指针是否已经提前被释放了；
4. 避免重复分配内存；
5. 小心使用有 return、break 语句的宏，确保前面资源已经释放；
6. 检查队列中每个成员是否释放；
7. 内存释放后，把指针置为 NULL；

**CMake** 代码风格
------------------

- 缩进四个空格。
- 每一行最大为 120 个字符。当分割行时，尝试尽可能关注可读性(例如通过配对关键字或参数对在单独的行)。
- 不要在 ``endforeach()`` 、 ``endif()`` 等之后的可选括号中放入任何内容。
- 命令、函数和宏的名称使用小写。
- 对于局部作用域的变量，使用小写。
- 对于全局作用域变量，使用大写。
- 两条独立的语句之间要隔行分开，并且独立语句要顶格开始，右括号要独自一行并保持对齐。

示例：
::

 list(APPEND ADD_LINK_SEARCH_PATH "${CONFIG_TOOLCHAIN_PATH}/lib"
                                  )

 list(APPEND ADD_STATIC_LIB "lib/libtest.a"
                            )


Kconfig 配置文件
----------------------

对于遍布源码树的所有 Kconfig* 配置文件来说，它们缩进方式有所不同。紧挨着 config 定义的行，用4个空 格缩进， help 信息的缩进则额外增加 4 个空 格。举个例子:

::

    config AUDIT
        bool "Auditing support"
        depends on NET
        help
            Enable auditing infrastructure that can be used with another
            kernel subsystem, such as SELinux (which requires this for
            logging of avc messages output).  Does not do system-call
            auditing without CONFIG_AUDITSYSCALL.

而那些危险的功能 (比如某些文件系统的写支持) 应该在它们的提示字符串里显著的声 明这一点:
::

    config ADFS_FS_RW
        bool "ADFS write support (DANGEROUS)"
        depends on ADFS_FS
        ...

第三方组件代码风格
------------------

WM IoT SDK 为了带给用户更多的体验，集成了许多的第三方开源软件作为组件来使用，这些第三方开源组件可保持其本身的代码风格不变。但需要注意，WM IoT SDK 中对于第三方开源软件的 DEBUG 输出在对应的 menuconfig 中增加了开关进行控制，并将输出重定向到了 WM LOG 模块中，即第三方的 DEBUG 输出将受限于 WM LOG 的日志级别。另外对于无日志优先级配置的模块，如 LWIP 等，默认的输出级别为 DEBUG 。

提交
-----

无论如何，正式提交的代码，都应按下列要求进行检查。

- [ ] 不能存在任何编译器告警
- [ ] 不能存在静态检查告警，静态检查工具暂定为cppcheck
- [ ] 编码规范满足上述要求
- [ ] 编码效率要满足上述要求
- [ ] 编码安全性要满足上述要求


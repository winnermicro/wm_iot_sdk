Event
===========
概述
----------

Event 组件是一个用于事件分发的模块组件，各个模块把事件发送到 event 组件， event 再通过应用注册的回调分发给应用，方便用户集中订阅处理不同的模块事件。

Event 事件提供了分组管理，按分组注册事件回调，移除注册回调，向分组发送事件功能。

Event 事件涉及两个角色，发布方和订阅方，产生事件的模块就是发布方，注册事件回调接收处理事件的模块就是订阅方。

发布方在模块的头文件中使用 WM_EVENT_DECLARE_GROUP 宏声明事件分组，并用枚举定义组内的事件类型，用结构体定义事件的具体数据类型，再在模块 C 文件中使用 WM_EVENT_DEFINE_GROUP 定义事件分组，在产生事件的地方使用 wm_event_send 接口发送事件。

订阅方需要先使用 wm_event_add_callback 接口注册接收哪个组的事件，如果传的组是 WM_EVENT_ANY_GROUP 表示接收所有组事件。

示例功能
----------

初始化Event组件
^^^^^^^^^^^^^^^^^^

在配置中使能 Event 组件后，用户无需自己调用 ``wm_event_init()`` 进行初始化。


事件分组
^^^^^^^^^^^^^^^^^^^

``GROUP`` 定义如下：

.. code:: c

   #define WM_EVENT_DECLARE_GROUP(group) extern wm_event_group_t const group
   #define WM_EVENT_DEFINE_GROUP(group) wm_event_group_t const group = #group

上面2个宏用于声明事件分组和定义事件分组，用在产生事件的模块，头文件中声明事件分组， C 文件中定义分组，订阅事件的模块中直接使用即可。


``WM_EVENT_DECLARE_GROUP`` ：用于在发布方头文件中声明事件分组；订阅方包含头文件后可直接使用分组名字。
``WM_EVENT_DEFINE_GROUP``  ：用于在发布方 C 文件中定义事件分组，只定义一次。

.. warning::
   分组名称只能使用上面的宏去定义，使用时也只能使用模块头文件中用宏声明的分组，不能直接使用字符串，原因是模块内部是直接比较地址的，没有比较字符串本身。


注册回调函数
^^^^^^^^^^^^^^^^^^^

``wm_event_add_callback`` 定义如下：

.. code:: c

   int wm_event_add_callback(wm_event_group_t group, int event, wm_event_callback callback, void *priv);


向 event 模块注册事件组的回调函数， group 指定需要的事件分组，当该分组的事件发生时，注册的回调函数将被调用，当 group 为 WM_EVENT_ANY_GROUP 时，会收到所有组的事件。

``wm_event_group_t group`` ：要过滤的事件分组。

``event`` ：要过滤的 event 类型。

``wm_event_callback callback`` ：是添加的回调函数，在该函数中完成事件触发时所需要完成的功能。

.. code:: c

   typedef void (*wm_event_callback)(wm_event_group_t group, int event, void *data, void *priv);

``void *priv`` ：可在注册的同时添加该回调函数的私有数据，通常是用于同步的信号量。

.. code:: c

   typedef void (*wm_event_callback)(wm_event_group_t group, int event,void *data, void *priv);


移除回调函数
^^^^^^^^^^^^^^^^^^^

``wm_event_remove_callback`` 定义如下：

.. code:: c

   int wm_event_remove_callback(wm_event_group_t group, int event, wm_event_callback callback,void *priv);


``wm_event_group_t group`` ：事件分组。

``event`` ：要过滤的 event 类型。

``wm_event_callback callback`` ：是添加的回调函数，在该函数中完成事件触发时所需要完成的功能。其原型为：

.. code:: c

   typedef void (*wm_event_callback)(wm_event_group_t group, int event, void *data, void *priv);

``void *priv`` ：可在注册的同时添加该回调函数的私有数据，通常是用于同步的信号量。


移除回调，与注册回调对应， group ， event ， callback ， priv 四项都一样时才会移除，移除后不再收到事件。



发送事件
^^^^^^^^^^^^^^^

``wm_event_send`` 定义如下：

.. code:: c

   int wm_event_send(wm_event_group_t group,int event, void *data,size_t size);

``wm_event_group_t group`` ：事件分组。

``int event`` ：分组内的事件子类型，一般在发布方模块的头文件中用枚举定义。

``void* data`` ：事件分组数据，具体结构一般在接收方头文件中定义为结构体，如果事件 event 子类型已经能描述清楚，可以为 NULL 表示没有数据。

``size_t size`` ：data 数据的大小。

向指定分组发送事件。事件可以携带特定的数据 ``*data`` ，回调处理 data 时，需要转换成事件提供方的 struct 结构处理。




关闭事件组件
^^^^^^^^^^^^^^^^

``wm_event_deinit`` 定义如下：

.. code:: c

   int wm_event_deinit(void);



调用该函数将停止事件处理任务，并释放相关资源。


应用实例
----------

1. 产生事件模块的头文件 example_event.h 中定义如下：

.. code:: C

   //声明事件分组，等价于extern const char* EXAMPLE_USER_EV_GROUP;
   WM_EVENT_DECLARE_GROUP(EXAMPLE_USER_EV_GROUP);

   enum{
      EXAMPLE_EVENT_TYPE1 = 1, //分组内的事件类型，可以定义多个
   };

   //事件带的数据类型
   struct exampe_user_event_t
   {
      int param1;
      int param2;
   };

   //模块中产生事件的函数
   void start_example_event(void);


2. 产生事件模块的 C 文件 example_event.c 中实现如下：

.. code:: C

   #include "example_event.h"

   //定义事件，等价于：const char * EXAMPLE_USER_EV_GROUP="EXAMPLE_USER_EV_GROUP"
   WM_EVENT_DEFINE_GROUP(EXAMPLE_USER_EV_GROUP);

   void start_example_event(void)
   {
      //事件所带的具体数据
      struct exampe_user_event_t user_event = {1,2};

      //发送事件到分组
      wm_event_send(EXAMPLE_USER_EV_GROUP, EXAMPLE_EVENT_TYPE1, &user_event, sizeof(user_event));
   }


3. 订阅和处理事件：

.. code:: C

   #include <stdio.h>
   #include <stdint.h>

   #include "wm_types.h"
   #include "wm_event.h"
   #include "example_event.h"

   static void example_event_user_callback(wm_event_group_t group, int event,void *data, void *priv)
   {
      if (event == EXAMPLE_EVENT_TYPE1) //处理指定事件
      {
         struct exampe_user_event_t* p = (struct exampe_user_event_t*)(data);
         printf("user event %d %d\n",p->param1,p->param2);
      }
   }

   int main(void)
   {
      //订阅事件
      wm_event_add_callback(EXAMPLE_USER_EV_GROUP,WM_EVENT_ANY_TYPE,example_event_user_callback,NULL);

      //启动产生事件的模块
      start_example_event();

      return 0;
   }



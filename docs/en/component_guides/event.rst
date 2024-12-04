Event
===========
Overview
----------

The Event component is a module component used for event distribution. Various modules send events to the event component, which then distributes them to applications through registered callbacks, allowing users to centrally subscribe to and handle different module events.

Event management includes grouping events, registering event callbacks by group, removing registered callbacks, and sending events to groups.

There are two roles involved in event handling: publishers and subscribers. Modules that generate events are publishers, while modules that register event callbacks to receive and process events are subscribers.

Publishers use the WM_EVENT_DECLARE_GROUP macro in the module's header file to declare event groups and use enums to define event types within the group. They use structures to define the specific data types of events. In the module's C file, they use WM_EVENT_DEFINE_GROUP
to define event groups and wm_event_send to send events at the point of generation.  

Subscribers need to register which group's events they will receive using the ``wm_event_add_callback`` interface. Passing ``WM_EVENT_ANY_GROUP`` as the group indicates that the subscriber will receive events from all groups.

Example Functions
--------------------

Initializing the Event Component
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

After enabling the Event component in the configuration, users do not need to call ``wm_event_init()`` for initialization.


Event Grouping
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The ``GROUP`` definition is as follows:

.. code:: c

   #define WM_EVENT_DECLARE_GROUP(group) extern wm_event_group_t const group
   #define WM_EVENT_DEFINE_GROUP(group) wm_event_group_t const group = #group

The above two macros are used to declare and define event groups. They are used in the module that generates the events, with the event group declared in the header file and defined in the C file. The subscriber module can directly use the group.


``WM_EVENT_DECLARE_GROUP`` : Used to declare an event group in the publisher's header file. The subscriber can directly use the group name after including the header file.

``WM_EVENT_DEFINE_GROUP``  : Used to define an event group in the publisher's C file, which should be defined only once.

.. warning::
   The group name can only be defined using the above macros, and can only be used as declared in the module's header file using the macros, not as strings directly. This is because the module internally compares addresses rather than strings themselves.


Register Callback Function
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The ``wm_event_add_callback`` is defined as follows:

.. code:: c

   int wm_event_add_callback(wm_event_group_t group, int event, wm_event_callback callback, void *priv);


This function registers a callback function for a specific event group with the event module. When an event in that group occurs, the registered callback function will be called. If the group is WM_EVENT_ANY_GROUP, all group events will be received.

``wm_event_group_t group`` : The event group to filter.

``event`` :  The event type to filter.

``wm_event_callback callback``: The callback function to add, which completes the required functionality when the event is triggered.

.. code:: c

   typedef void (*wm_event_callback)(wm_event_group_t group, int event, void *data, void *priv);

``void *priv`` : Private data for the callback function that can be added during registration, typically used for synchronization semaphores.

.. code:: c

   typedef void (*wm_event_callback)(wm_event_group_t group, int event,void *data, void *priv);


Removing Callback Function
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The ``wm_event_remove_callback`` is defined as follows:

.. code:: c

   int wm_event_remove_callback(wm_event_group_t group, int event, wm_event_callback callback,void *priv);


``wm_event_group_t group``:  The event group.

``event``: The event type to filter.

``wm_event_callback callback``: The callback function to remove,which completes the required functionality when the event is triggered. The function prototype is:

.. code:: c

   typedef void (*wm_event_callback)(wm_event_group_t group, int event, void *data, void *priv);

``void *priv`` : Private data of the callback function that can be added during registration, typically used for synchronization semaphores.


Removing a callback corresponds to registering a callback. It will only be removed if the group, event, callback, and priv are all the same, and no more events will be received after removal.



Sending Events
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The ``wm_event_send`` is defined as follows:

.. code:: c

   int wm_event_send(wm_event_group_t group,int event, void *data,size_t size);

``wm_event_group_t group`` : The event group.

``int event`` : The event subtype within the group, generally defined  using enumeration in the publisher's module header file using an enum.

``void* data`` : Event data, with a specific structure typically defined in the receiver's header file as a structure. If the event subtype already describes everything, it can be NULL to indicate no data.

``size_t size`` : The size of the data.

This function sends an event to the specified group. The event can carry specific ``*data``,  which needs to be processed as a struct provided by the event publisher when handling data.




Deinitializing the Event Component
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The ``wm_event_deinit`` is defined as follows:

.. code:: c

   int wm_event_deinit(void);



Calling this function stops the event processing task and releases related resources.


Application Example
----------------------


1 The header file of the event-generating module example_event.h defines as follows:

.. code:: C

   // Declare the event group, equivalent to extern const char* EXAMPLE_USER_EV_GROUP;
   WM_EVENT_DECLARE_GROUP(EXAMPLE_USER_EV_GROUP);

   enum{
      EXAMPLE_EVENT_TYPE1 = 1,  // Event types within the group, multiple can be defined
   };

   // Data type carried by the event
   struct exampe_user_event_t
   {
      int param1;
      int param2;
   };

   // Function that generates the event in the module
   void start_example_event(void);


2 The C file of the event-generating module (example_event.c) implements as follows:

.. code:: C

   #include "example_event.h"

   //// Define the event, equivalent to: const char *  EXAMPLE_USER_EV_GROUP="EXAMPLE_USER_EV_GROUP"
   WM_EVENT_DEFINE_GROUP(EXAMPLE_USER_EV_GROUP);

   void start_example_event(void)
   {
      // Specific data carried by the event
      struct exampe_user_event_t user_event = {1,2};

      // Send the event to the group
      wm_event_send(EXAMPLE_USER_EV_GROUP, EXAMPLE_EVENT_TYPE1, &user_event, sizeof(user_event));
   }


3 Subscribing and Handling Events:

.. code:: C

   #include <stdio.h>
   #include <stdint.h>

   #include "wm_types.h"
   #include "wm_event.h"
   #include "example_event.h"

   static void example_event_user_callback(wm_event_group_t group, int event,void *data, void *priv)
   {
      if (event == EXAMPLE_EVENT_TYPE1)  // Handle the specified event
      {
         struct exampe_user_event_t* p = (struct exampe_user_event_t*)(data);
         printf("user event %d %d\n",p->param1,p->param2);
      }
   }

   int main(void)
   {
      // Subscribe to the event
      wm_event_add_callback(EXAMPLE_USER_EV_GROUP,WM_EVENT_ANY_TYPE,example_event_user_callback,NULL);

      // Start the module that generates the event
      start_example_event();

      return 0;
   }



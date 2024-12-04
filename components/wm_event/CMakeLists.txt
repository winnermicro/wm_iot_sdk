if(CONFIG_COMPONENT_EVENT_ENABLED)
    list(APPEND ADD_INCLUDE "include"
                            )

    list(APPEND ADD_SRCS "src/wm_event.c"
                         )

    register_component()
endif()

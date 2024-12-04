if(CONFIG_COMPONENT_LOG_ENABLED)
    list(APPEND ADD_INCLUDE "include"
                            )

    list(APPEND ADD_SRCS  "src/wm_log.c"
                         )

    register_component()
endif()

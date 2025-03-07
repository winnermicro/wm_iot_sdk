list(APPEND ADD_INCLUDE "include"
                        )

if(CONFIG_COMPONENT_PM_ENABLED)
    list(APPEND ADD_SRCS "src/wm_pm.c"
                         )

    register_component()
endif()

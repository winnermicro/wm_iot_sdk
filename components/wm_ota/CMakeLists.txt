if(CONFIG_COMPONENT_OTA_ENABLED)
    list(APPEND ADD_INCLUDE "include"
    )

    list(APPEND ADD_SRCS "src/wm_ota_ops.c"
                         "src/wm_ota.c"
    )

    if(CONFIG_COMPONENT_OTA_HTTP_ENABLED)
        list(APPEND ADD_SRCS "ota_http/wm_ota_http.c"
        )
    endif()

    register_component()
endif()

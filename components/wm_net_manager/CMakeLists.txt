if(CONFIG_COMPONENT_NET_MANAGER_ENABLED)
    list(APPEND ADD_INCLUDE "include"
                            )

    list(APPEND ADD_SRCS "src/wm_nm_api.c"
                         "src/wm_nm_core.c"
                         )

    if(CONFIG_COMPONENT_NM_WIFI_STA_ENABLED OR CONFIG_COMPONENT_NM_WIFI_SOFTAP_ENABLED)
        list(APPEND ADD_SRCS "src/wm_nm_wifi.c"
                            )
    endif()

    if(CONFIG_WM_NETIF_SELECT_LWIP)
        list(APPEND ADD_SRCS "src/lwip/wm_nm_lwip.c"
                             )
    endif()

    register_component()
endif()



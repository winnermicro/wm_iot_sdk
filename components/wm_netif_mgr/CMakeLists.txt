if(CONFIG_COMPONENT_LWIP_ENABLED)
    list(APPEND ADD_INCLUDE "include"
                            )

    list(APPEND ADD_SRCS "src/wm_netif.c"
                         )

    if(${CONFIG_WM_NETIF_SELECT_LWIP})
        list(APPEND ADD_PRIVATE_INCLUDE "src/lwip"
                                        )

        list(APPEND ADD_SRCS "src/lwip/wm_netif_lwip.c"
                             )

        if(CONFIG_COMPONENT_WIFI_ENABLED AND CONFIG_WIFI_API_ENABLED)
            if(${CONFIG_WM_NETIF_ENABLE_ETH})
                list(APPEND ADD_SRCS "src/lwip/wm_netif_lwip_eth.c"
                                     )
            else()
                if(${CONFIG_CHIP_W80X})
                    list(APPEND ADD_SRCS "src/lwip/wm_netif_lwip_w800.c"
                                         )
                endif()
            endif()
        endif()

        list(APPEND ADD_DEFINITIONS -DCONFIG_LWIP_ENABLE_DEINIT
                                    )
    endif()

    register_component()
endif()

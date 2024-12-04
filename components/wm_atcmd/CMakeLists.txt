if(CONFIG_COMPONENT_WM_ATCMD_ENABLED)
    list(APPEND ADD_INCLUDE "include"
                            )

    list(APPEND ADD_EMBEDDED_FILES "certs/cert.der"
                                   )

    list(APPEND ADD_EMBEDDED_TEXTFILES "certs/ca_cert_rsa_sha256.pem"
                                       "certs/cert_rsa.pem"
                                       "certs/private_key_rsa.pem"
                                       "certs/srv_cert_rsa_sha256.pem"
                                       "certs/srv_key_rsa.pem"
                                       "html/index.html"
                                       )

    append_srcs_dir(ADD_SRCS "src")

    set(__source_code_path ${CMAKE_CURRENT_SOURCE_DIR}/lib)
    is_path_component(__exist_source_code ${__source_code_path})

    if(NOT __exist_source_code)
        if(USE_CMAKE_BUILD_DEBUG)
            message(STATUS "atcmd use lib")
        endif()
        list(APPEND ADD_STATIC_LIB "${SDK_PATH}/components/wm_soc/${chip_family_type}/lib/libatcmd.a"
                                   )
    endif()

    set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,--wrap=wm_atcmd_wifi_init -Wl,--wrap=wm_atcmd_tcpip_init -Wl,--wrap=wm_atcmd_mqtt_init -Wl,--wrap=wm_atcmd_http_init -Wl,--wrap=wm_atcmd_bt_init" PARENT_SCOPE)

    register_component()

    if(__exist_source_code)
        if(USE_CMAKE_BUILD_DEBUG)
            message(STATUS "atcmd use source code: ${__source_code_path}")
        endif()
        add_subdirectory(${__source_code_path})
    endif()
endif()
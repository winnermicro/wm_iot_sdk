if(CONFIG_COMPONENT_DT_ENABLED)
    list(APPEND ADD_INCLUDE "include"
                            )

    list(APPEND ADD_SRCS "src/wm_dt.c"
                         )

    if(NOT CONFIG_DT_USE_C_STYLE_CONF)
        append_srcs_dir(ADD_SRCS ${wmsdk_dt_dir}
                                 )
    endif()

    register_component()
endif()

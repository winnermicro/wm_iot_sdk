if(CONFIG_COMPONENT_CJSON_ENABLED)
    list(APPEND ADD_INCLUDE "src"
                            )

    append_srcs_dir(ADD_SRCS "src/cJSON")

    register_component()
endif()

if(CONFIG_COMPONENT_LITTLEFS_ENABLED)
    list(APPEND ADD_INCLUDE "src"
                            "port"
                            )

    list(APPEND ADD_SRCS "src/lfs.c"
                         "src/lfs_util.c"
                         "port/wm_lfs_flash.c"
                         )

    register_component()
endif()

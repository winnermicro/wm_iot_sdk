list(APPEND ADD_SRCS "src/main.c"
                                )

list(APPEND ADD_EMBEDDED_FILES  "src/atcmd_ota.img"
                                )

register_component()

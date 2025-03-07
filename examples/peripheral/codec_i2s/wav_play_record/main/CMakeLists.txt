list(APPEND ADD_INCLUDE "."
                        )

list(APPEND ADD_SRCS "src/main.c"
                     "src/wm_wav.c"
                     )

register_component()

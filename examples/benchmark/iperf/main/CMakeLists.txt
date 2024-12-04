list(APPEND ADD_INCLUDE "."
                        )

list(APPEND ADD_SRCS "src/main.c"
                     "src/cmd_iperf.c"
                     )

register_component()

list(APPEND ADD_INCLUDE "include"
                        )

append_srcs_dir(ADD_SRCS "src")

list(REMOVE_ITEM ADD_SRCS "src/iperf_sctp.c"
                          "src/cjson.c"
                          "src/iperf_auth.c"
                          "src/t_api.c"
                          "src/t_auth.c"
                          "src/t_timer.c"
                          "src/t_units.c"
                          "src/t_uuid.c"
                          )

#list(APPEND ADD_DEFINITIONS -DHAVE_CONFIG_H
#                            -DNOCRYPT
#                            )

set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -lm" PARENT_SCOPE)

register_component()
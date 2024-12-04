list(APPEND ADD_INCLUDE "."
                        )

list(APPEND ADD_SRCS "src/main.c"
                     )

if("${CONFIG_EXAMPLE_IPV4}" STREQUAL y)
list(APPEND ADD_SRCS "src/tcp_client_example_v4.c"
                     )
endif()

if("${CONFIG_EXAMPLE_IPV6}" STREQUAL y)
list(APPEND ADD_SRCS "src/tcp_client_example_v6.c"
                     )
endif()

register_component()

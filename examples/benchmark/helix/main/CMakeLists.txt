list(APPEND ADD_PRIVATE_INCLUDE "src"
                                "src/pub"
                                "src/real"
                                "src/testwrap"
                                )

append_srcs_dir(ADD_SRCS "src"
                         "src/pub"
                         "src/real"
                         "src/testwrap"
                         )

list(APPEND ADD_DEFINITIONS -finline-limit=500
                            -fmessage-length=0
                            )

register_component()

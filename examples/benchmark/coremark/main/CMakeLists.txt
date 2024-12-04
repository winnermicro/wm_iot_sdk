append_srcs_dir(ADD_SRCS "src"
                         )

list(APPEND ADD_DEFINITIONS -funroll-all-loops
                            -fgcse-sm
                            -finline-limit=500
                            -fno-schedule-insns
                            )

register_component()

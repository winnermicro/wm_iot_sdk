############### Add include ###################
list(APPEND ADD_INCLUDE "include"
                        )

list(APPEND ADD_PRIVATE_INCLUDE ""
                                )
###############################################

############ Add source files #################
# list(APPEND ADD_SRCS "src/main.c"
#                      "src/wm_drv_dma_m2m_demo.c"
#                      )

# append source file in src dir to var ADD_SRCS
# aux_source_directory(src ADD_SRCS)
append_srcs_dir(ADD_SRCS "src"
                         )

register_component()

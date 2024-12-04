############### Add include ###################
list(APPEND ADD_INCLUDE "")

list(APPEND ADD_PRIVATE_INCLUDE "")
###############################################

# append source file in src dir to var ADD_SRCS
# aux_source_directory(src ADD_SRCS)
append_srcs_dir(ADD_SRCS "src"
                         )
###############################################

register_component()

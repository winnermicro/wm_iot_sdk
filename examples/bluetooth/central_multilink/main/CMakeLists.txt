############### Add include ###################
list(APPEND ADD_INCLUDE "")

list(APPEND ADD_PRIVATE_INCLUDE "")
###############################################

############ Add source files #################
# list(APPEND ADD_SRCS  "src/main.c"
#                       "src/test.c"
#                       )

# append source file in src dir to var ADD_SRCS
# aux_source_directory(src ADD_SRCS)
append_srcs_dir(ADD_SRCS "src"
                         )
###############################################

#### Add compile option for this component
# list(APPEND ADD_DEFINITIONS -DAAABBB=1
#                             -DBBBBDDDD=2
#                             )
###############################################

###### Add link search path for libs ######
# list(APPEND ADD_LINK_SEARCH_PATH "${CONFIG_TOOLCHAIN_PATH}/lib"
#                                  )
###############################################

############ Add static libs ##################
# list(APPEND ADD_STATIC_LIB "lib/libtest.a"
#                            )
###############################################

############ Add static libs ##################
#### Update parent's variables like CMAKE_C_LINK_FLAGS
# set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -lm" PARENT_SCOPE)  # add third libs, math lib for example
# set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,--start-group -Wl,--whole-archive test/libtest.a -ltest2 -Wl,--no-whole-archive -Wl,--end-group" PARENT_SCOPE)
###############################################

register_component()

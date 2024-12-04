############### Add include ###################
list(APPEND ADD_INCLUDE "include"
                        )

list(APPEND ADD_PRIVATE_INCLUDE ""
                                )
###############################################

############ Add source files #################
# list(APPEND ADD_SRCS "src/main.c"
#                      "src/test.c"
#                      )

# append source file in src dir to var ADD_SRCS
# aux_source_directory(src ADD_SRCS)
append_srcs_dir(ADD_SRCS "src"
                         )

list(REMOVE_ITEM ADD_SRCS "src/test2.c"
                          )
###############################################

########## Add device table config c files ####
# set(ADD_DT_C_FILES "dt/dt_config1.c"
#                    "dt/dt_config2.c"
#                    )
# set(ADD_DT_H_FILES "dt"
#                    )
###############################################

############### Add Embedded file #############
#list(APPEND ADD_EMBEDDED_FILES "src/png/logo.png"
#                               )

#list(APPEND ADD_EMBEDDED_TEXTFILES "src/pem/user.cert"
#                                   )
###############################################

############## Add custom files ###############
# list(APPEND ADD_CUSTOM_FILES "custom_pt1"
#                      "src/bin/data1_demos.txt 0 1024"
#                      "src/bin/data2_demos.bin 1024 2048"
#                     )
#
# list(APPEND ADD_CUSTOM_FILES "custom_pt2"
#                      "src/bin/data3_demos.txt 0 1024"
#                      )
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

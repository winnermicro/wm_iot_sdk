if(${CONFIG_CHIP_W80X})
    list(APPEND ADD_INCLUDE "include"
                            "include/csi_core"
                            #"include/csi_dsp"
                            )

    list(APPEND ADD_SRCS  "src/isr.c"
                          "src/cpu_init.c"
                          "src/libc_port.c"
                          "src/trap_c.c"
                          "src/startup.S"
                          "src/vectors.S"
                          )

    list(APPEND ADD_STATIC_LIB "${SDK_PATH}/components/wm_soc/${chip_family_type}/lib/libcsi_math.a"
                               "${SDK_PATH}/components/wm_soc/${chip_family_type}/lib/libcsi_dsp2_nn.a"
                               )

    register_component()
endif()

if(HIGH_VERSION_GCC)
    set(DEFINE_MACRO ${DEFINE_MACRO}
                     -D__clockid_t_defined
                     -D_CLOCKID_T_DECLARED
                     -D_MODE_T_DECLARED
                     -D__timer_t_defined
                     -D_TIMER_T_DECLARED
                     -D_SYS__PTHREADTYPES_H_
                     -D_SYS_TIMESPEC_H_
                     -D_SYS_TIME_H_
                     -D_SYS__TIMESPEC_H_
                     -D_SYS_SIGNAL_H
                     )
endif()

if(CONFIG_UNIT_TEST_ENABLE_CODE_COVERAGE)
    set(CODE_COVERAGE_OPTS -fprofile-arcs
                           -ftest-coverage
                           )
else()
    set(CODE_COVERAGE_OPTS "")
endif()

set(CMAKE_C_FLAGS -Wall
                  -mcpu=ck804ef
                  ${compiler_optimization}
                  -std=gnu99
                  -mhard-float
                  -fdata-sections
                  -ffunction-sections
                  -fno-builtin-printf
                  -fno-builtin-fprintf
                  ${DEFINE_MACRO}
                  ${CODE_COVERAGE_OPTS}
                  )

set(CMAKE_CXX_FLAGS -Wall
                    -mcpu=ck804ef
                    ${compiler_optimization}
                    -MMD
                    -MP
                    -mhard-float
                    -fdata-sections
                    -ffunction-sections
                    -fno-builtin-printf
                    -fno-builtin-fprintf
                    ${DEFINE_MACRO}
                    ${CODE_COVERAGE_OPTS}
                    )

set(CMAKE_ASM_FLAGS -Wall
                    -mcpu=ck804ef
                    ${compiler_optimization}
                    -std=gnu99
                    -mhard-float
                    -Wa,--gdwarf2
                    -fdata-sections
                    -ffunction-sections
                    ${DEFINE_MACRO}
                    ${CODE_COVERAGE_OPTS}
                    )

#set(CMAKE_AR_FLAGS ru
set(CMAKE_AR_FLAGS rc
                   )

set(LINK_FLAGS ${LINK_FLAGS}
               -mcpu=ck804ef
               -nostartfiles
               -mhard-float
               -Wl,--gc-sections
               -Wl,-zmax-page-size=1024
               -Wl,--print-memory-usage
               )

if(HIGH_VERSION_GCC)
    set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS}
                      -mistack
                      )

    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}
                        -mistack
                        )

    set(CMAKE_ASM_FLAGS ${CMAKE_ASM_FLAGS}
                        -mistack
                        )

    set(LINK_FLAGS ${LINK_FLAGS}
                   -Wl,--no-warn-rwx-segments
                   )
endif()

if(CONFIG_UNIT_TEST_ENABLE_CODE_COVERAGE)
    set(LINK_FLAGS ${LINK_FLAGS}
                   -lgcov
                   )
endif()

if(USE_CMAKE_BUILD_DEBUG)
    set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS}
                      -fstack-usage
                      )

    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}
                        -fstack-usage
                        )

    set(CMAKE_ASM_FLAGS ${CMAKE_ASM_FLAGS}
                        -fstack-usage
                        )

    set(LINK_FLAGS ${LINK_FLAGS}
                   -Wl,--callgraph
                   )
endif()

set(CMAKE_C_LINK_FLAGS ${CMAKE_C_LINK_FLAGS}
                       ${LINK_FLAGS}
                       )

set(CMAKE_CXX_LINK_FLAGS ${CMAKE_CXX_LINK_FLAGS}
                         ${LINK_FLAGS}
                         )

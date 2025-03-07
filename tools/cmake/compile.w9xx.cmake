set(CMAKE_C_FLAGS -Wall
                  -mcpu=ck804ef
                  ${compiler_optimization}
                  -std=gnu99
                  -mhard-float
                  -fdata-sections
                  -ffunction-sections
                  ${DEFINE_MACRO}
                  )

set(CMAKE_CXX_FLAGS -Wall
                    -mcpu=ck804ef
                    ${compiler_optimization}
                    -MMD
                    -MP
                    -mhard-float
                    -fdata-sections
                    -ffunction-sections
                    ${DEFINE_MACRO}
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

set(CMAKE_C_LINK_FLAGS ${CMAKE_C_LINK_FLAGS}
                       ${LINK_FLAGS}
                       )

set(CMAKE_CXX_LINK_FLAGS ${CMAKE_CXX_LINK_FLAGS}
                         ${LINK_FLAGS}
                         )

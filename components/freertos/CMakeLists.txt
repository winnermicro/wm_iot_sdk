list(APPEND ADD_INCLUDE "include"
                        "port/${CONFIG_CHIP_FAMILY_NAME}/include"
                        )

#list(APPEND ADD_PRIVATE_INCLUDE "include/freertos")

list(APPEND ADD_SRCS "src/croutine.c"
                     "src/event_groups.c"
                     "src/list.c"
                     "src/queue.c"
                     "src/stream_buffer.c"
                     "src/tasks.c"
                     "src/timers.c"
                     "port/${CONFIG_CHIP_FAMILY_NAME}/port.c"
                     "port/${CONFIG_CHIP_FAMILY_NAME}/cpu_task_sw.S"
                     )

if (CONFIG_UNIT_TEST_ENABLE_CODE_COVERAGE)
    list(APPEND ADD_DEFINITIONS -fno-profile-arcs -fno-test-coverage)
endif()

register_component()

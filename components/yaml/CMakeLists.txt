if(CONFIG_COMPONENT_YAML_ENABLED)
    list(APPEND ADD_INCLUDE "include"
                            )

    append_srcs_dir(ADD_SRCS "src")

    list(APPEND ADD_DEFINITIONS -DHAVE_CONFIG_H
                                )

    if (CONFIG_UNIT_TEST_ENABLE_CODE_COVERAGE)
        list(APPEND ADD_DEFINITIONS -fno-profile-arcs -fno-test-coverage)
    endif()

    register_component()
endif()

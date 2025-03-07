list(APPEND ADD_INCLUDE "include"
                        )

if(CONFIG_COMPONENT_CLI_ENABLED)

	list(APPEND ADD_SRCS "src/wm_cli.c"
                         "src/letter_shell.c"
	                     "src/letter_shell_ext.c"
	                     )

    if(CONFIG_CLI_COMMANDS_SYSTEM)
        list(APPEND ADD_SRCS "commands/cmd_system.c"
	                         )
    endif()

    if(CONFIG_CLI_COMMANDS_WIFI)
        list(APPEND ADD_SRCS "commands/cmd_wifi.c"
	                         )
    endif()

    if(CONFIG_CLI_COMMANDS_LWIP)
        list(APPEND ADD_SRCS "commands/cmd_lwip.c"
                             "commands/cmd_ping.c"
	                         )
    endif()

    if(CONFIG_CLI_COMMANDS_PARTITION_TABLE)
        list(APPEND ADD_SRCS "commands/cmd_partition_table.c"
	                         )
    endif()

    if(CONFIG_CLI_COMMANDS_HTTP)
        list(APPEND ADD_SRCS "commands/cmd_http.c"
	                         )
    endif()

    if(CONFIG_CLI_COMMANDS_NTP)
        list(APPEND ADD_SRCS "commands/cmd_ntp.c"
	                         )
    endif()

    if(CONFIG_UNIT_TEST_ENABLE_CODE_COVERAGE)
        list(APPEND ADD_DEFINITIONS -fno-profile-arcs -fno-test-coverage)
    endif()

	register_component()

endif()

add_custom_command(TARGET ${project_elf} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E echo "Image created successfully"
                   COMMAND ${CMAKE_COMMAND} -E echo "All build complete"
                   DEPENDS ${project_elf}
                   COMMENT ""
                   )

if(USE_CMAKE_BUILD_PROMPT)
    add_custom_command(TARGET ${project_elf} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E echo ""
                       COMMAND ${CMAKE_COMMAND} -E echo "to flash, run this command:"
                       COMMAND ${CMAKE_COMMAND} -E echo ""
                       DEPENDS ${project_elf}
                       COMMENT ""
                       )

    add_custom_command(TARGET ${project_elf} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E echo "    wm.py flash -p PORT"
                       DEPENDS ${project_elf}
                       COMMENT ""
                       )

    add_custom_command(TARGET ${project_elf} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E echo ""
                       DEPENDS ${project_elf}
                       COMMENT ""
                       )
endif()

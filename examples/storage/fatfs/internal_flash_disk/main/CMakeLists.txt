list(APPEND ADD_SRCS "src/main.c"
                                )

list(APPEND ADD_FATFS_FILES "../fatfs_folder/"
                            "../fatfs_file.txt"
                                )

register_component()

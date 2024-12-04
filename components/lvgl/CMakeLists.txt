if(CONFIG_COMPONENT_LVGL_ENABLED)
    
    list(APPEND ADD_INCLUDE "lvgl"
                            "lvgl/src"
                            "lvgl/demos"
                            "port"
                            )

    aux_source_directory(lvgl/demos/benchmark ADD_SRCS)
    aux_source_directory(lvgl/demos/benchmark/assets ADD_SRCS)
    aux_source_directory(lvgl/demos/stress ADD_SRCS)

    aux_source_directory(lvgl/demos/widgets ADD_SRCS)
    aux_source_directory(lvgl/demos/widgets/assets ADD_SRCS)

    aux_source_directory(lvgl/demos/music ADD_SRCS)
    aux_source_directory(lvgl/demos/music/assets ADD_SRCS)

    aux_source_directory(port ADD_SRCS)

    aux_source_directory(lvgl/src/core ADD_SRCS)

    aux_source_directory(lvgl/src/draw ADD_SRCS)
    aux_source_directory(lvgl/src/draw/arm2d ADD_SRCS)
    aux_source_directory(lvgl/src/draw/nxp ADD_SRCS)
    aux_source_directory(lvgl/src/draw/renesas ADD_SRCS)
    aux_source_directory(lvgl/src/draw/sdl ADD_SRCS)
    aux_source_directory(lvgl/src/draw/stm32_dma2d ADD_SRCS)
    aux_source_directory(lvgl/src/draw/sw ADD_SRCS)
    aux_source_directory(lvgl/src/draw/swm341_dma2d ADD_SRCS)

    aux_source_directory(lvgl/src/extra ADD_SRCS)
    aux_source_directory(lvgl/src/extra/layouts/flex ADD_SRCS)
    aux_source_directory(lvgl/src/extra/layouts/grid ADD_SRCS)

    aux_source_directory(lvgl/src/extra/libs/bmp ADD_SRCS)
    aux_source_directory(lvgl/src/extra/libs/ffmpeg ADD_SRCS)
    aux_source_directory(lvgl/src/extra/libs/freetype ADD_SRCS)
    aux_source_directory(lvgl/src/extra/libs/fsdrv ADD_SRCS)
    aux_source_directory(lvgl/src/extra/libs/gif ADD_SRCS)
    aux_source_directory(lvgl/src/extra/libs/png ADD_SRCS)
    aux_source_directory(lvgl/src/extra/libs/qrcode ADD_SRCS)
    aux_source_directory(lvgl/src/extra/libs/rlottie ADD_SRCS)
    aux_source_directory(lvgl/src/extra/libs/sjpg ADD_SRCS)
    aux_source_directory(lvgl/src/extra/libs/tiny_ttf ADD_SRCS)

    aux_source_directory(lvgl/src/extra/others/fragment ADD_SRCS)
    aux_source_directory(lvgl/src/extra/others/gridnav ADD_SRCS)
    aux_source_directory(lvgl/src/extra/others/ime ADD_SRCS)
    aux_source_directory(lvgl/src/extra/others/imgfont ADD_SRCS)
    aux_source_directory(lvgl/src/extra/others/monkey ADD_SRCS)
    aux_source_directory(lvgl/src/extra/others/msg ADD_SRCS)
    aux_source_directory(lvgl/src/extra/others/snapshot ADD_SRCS)

    aux_source_directory(lvgl/src/extra/themes/basic ADD_SRCS)
    aux_source_directory(lvgl/src/extra/themes/default ADD_SRCS)
    aux_source_directory(lvgl/src/extra/themes/mono ADD_SRCS)

    aux_source_directory(lvgl/src/extra/widgets/animimg ADD_SRCS)
    aux_source_directory(lvgl/src/extra/widgets/calendar ADD_SRCS)
    aux_source_directory(lvgl/src/extra/widgets/chart ADD_SRCS)
    aux_source_directory(lvgl/src/extra/widgets/colorwheel ADD_SRCS)
    aux_source_directory(lvgl/src/extra/widgets/imgbtn ADD_SRCS)
    aux_source_directory(lvgl/src/extra/widgets/keyboard ADD_SRCS)
    aux_source_directory(lvgl/src/extra/widgets/led ADD_SRCS)
    aux_source_directory(lvgl/src/extra/widgets/list ADD_SRCS)
    aux_source_directory(lvgl/src/extra/widgets/menu ADD_SRCS)
    aux_source_directory(lvgl/src/extra/widgets/meter ADD_SRCS)
    aux_source_directory(lvgl/src/extra/widgets/msgbox ADD_SRCS)
    aux_source_directory(lvgl/src/extra/widgets/span ADD_SRCS)
    aux_source_directory(lvgl/src/extra/widgets/spinbox ADD_SRCS)
    aux_source_directory(lvgl/src/extra/widgets/spinner ADD_SRCS)
    aux_source_directory(lvgl/src/extra/widgets/tabview ADD_SRCS)
    aux_source_directory(lvgl/src/extra/widgets/tileview ADD_SRCS)
    aux_source_directory(lvgl/src/extra/widgets/win ADD_SRCS)

    aux_source_directory(lvgl/src/font ADD_SRCS)
    aux_source_directory(lvgl/src/hal ADD_SRCS)
    aux_source_directory(lvgl/src/misc ADD_SRCS)
    aux_source_directory(lvgl/src/widgets ADD_SRCS)

    register_component()

endif()

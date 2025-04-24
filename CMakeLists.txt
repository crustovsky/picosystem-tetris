cmake_minimum_required(VERSION 3.12)

set(PROJECT_NAME VibeCodedTetris)

set(PROJECT_SOURCES
        tetris_main.cpp
        tetris_game.c
        tetris_display.c
)

if(NOT PICO_SDK_PATH)
    message(WARNING "PICO_SDK_PATH not specified. Forcing fetch from git!")
    set(PICO_SDK_FETCH_FROM_GIT true)
endif()

include(pico_sdk_import.cmake)

project(${PROJECT_NAME} C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

include(picosystem_sdk_import.cmake)

picosystem_executable(
        ${PROJECT_NAME}
        ${PROJECT_SOURCES}
)

#pixel_double(${PROJECT_NAME})          # 120x120 resolution game, pixel-doubled to 240x240
#disable_startup_logo(${PROJECT_NAME})  # Skip the PicoSystem splash
#no_font(${PROJECT_NAME})              # Omit the default font
#no_spritesheet(${PROJECT_NAME})       # Omit the default spritesheet
#no_overclock(${PROJECT_NAME})         # Don't overclock


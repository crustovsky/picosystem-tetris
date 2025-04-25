cmake_minimum_required(VERSION 3.12)

set(PROJECT_NAME VibeCodedTetris)

# Common source files for both platforms
set(COMMON_SOURCES
    tetris_game.c
    tetris_display.c
)

# Option to build for PicoSystem (default OFF)
option(USE_PICOSYSTEM "Build for PicoSystem" OFF)

if(USE_PICOSYSTEM)
    # PicoSystem build configuration
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

    # PicoSystem-specific sources
    set(PICO_SOURCES
        tetris_main_pico.cpp
        tetris_display_picosystem.cpp
        ${COMMON_SOURCES}
    )

    # Add compile definition for PicoSystem
    add_compile_definitions(USE_PICOSYSTEM)

    picosystem_executable(
        ${PROJECT_NAME}
        ${PICO_SOURCES}
    )

    #pixel_double(${PROJECT_NAME})          # 120x120 resolution game, pixel-doubled to 240x240
    disable_startup_logo(${PROJECT_NAME})  # Skip the PicoSystem splash
    #no_font(${PROJECT_NAME})              # Omit the default font
    no_spritesheet(${PROJECT_NAME})       # Omit the default spritesheet
    no_overclock(${PROJECT_NAME})         # Don't overclock

else()
    # SDL3 build configuration
    project(${PROJECT_NAME} C CXX)
    set(CMAKE_C_STANDARD 11)
    set(CMAKE_CXX_STANDARD 17)

    # SDL3-specific sources
    set(SDL_SOURCES
        tetris_main_sdl.cpp
        tetris_display_sdl.c
        ${COMMON_SOURCES}
    )

    # Find SDL3 package
    find_package(SDL3 REQUIRED)
    
    # Create the executable
    add_executable(${PROJECT_NAME} ${SDL_SOURCES})
    
    # Link SDL3 library
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)

    # Copy SDL3 DLLs to output directory on Windows
    if(WIN32)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:SDL3::SDL3>
                $<TARGET_FILE_DIR:${PROJECT_NAME}>
        )
    endif()
endif()

# Install target for both platforms
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)
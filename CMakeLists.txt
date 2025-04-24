# Set minimum required CMake version
cmake_minimum_required(VERSION 3.10)

# Project name and language
project(VibeCodedTetris C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Add executable
add_executable(tetris
    tetris_main.c
    tetris_game.c
    tetris_display.c
)

# Include directories for header files
target_include_directories(tetris PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

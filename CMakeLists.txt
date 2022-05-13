cmake_minimum_required(VERSION 3.22.2)

project(libtmx VERSION 0.1.0 DESCRIPTION "A library for loading Tiled maps in TMX and JSON format")

# Set output directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)

# Set options
option(TEST "Enable unit tests" OFF)
option(DEBUG "Build without optimization and include debugging symbols" OFF)

add_library(tmx SHARED)
target_sources(tmx
    PUBLIC
        "include/map.h"
    PRIVATE
        "src/log.c"
        "src/map.c"
)

# Set include directories
target_include_directories(tmx PUBLIC include)
target_include_directories(tmx PRIVATE src)

# Set library version
set_target_properties(tmx PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(tmx PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

# Set compiler/language options
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_FLAGS_RELEASE "-O2")
set(CMAKE_C_FLAGS_DEBUG "-O0 -g3")

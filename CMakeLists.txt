cmake_minimum_required(VERSION 3.22.2)

project(libtmx VERSION 0.1.0 DESCRIPTION "A library for loading Tiled maps in TMX and JSON format")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

# Set output directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)

# Set options
option(LIBTMX_TEST "Enable unit tests" OFF)
option(BUILD_SHARED_LIBS "Build as a shared library" ON)
# To enable debug builds, use -DCMAKE_BUILD_TYPE=Debug

find_package(Jansson)

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

# Set link libraries
target_link_libraries(tmx Jansson::Jansson)

# Set compiler/language options
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_FLAGS_RELEASE "-O2")
set(CMAKE_C_FLAGS_DEBUG "-O0 -g3")

# If tests are enabled, make tests
if(LIBTMX_TEST)
    enable_testing()

    add_executable(unit_tests test/tests.c test/Unity/src/unity.c)
    target_link_libraries(unit_tests tmx Jansson::Jansson)
    set_target_properties(unit_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY test/bin)

    add_test(NAME unit_tests COMMAND unit_tests WORKING_DIRECTORY test/bin)
endif()

cmake_minimum_required(VERSION 3.22.2)

project(libtmj VERSION 0.1.0 DESCRIPTION "A library for loading Tiled maps in TMJ and JSON format")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

# Set output directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)

# Set options
option(LIBTMJ_TEST "Enable unit tests" ON)
option(LIBTMJ_DOCS "Enable compiling documentation" ON)
option(LIBTMJ_ZSTD "Enable zstd decompression for tile layers" ON)
option(LIBTMJ_ZLIB "Enable zlib/gzip decompression for tile layers" ON)
option(BUILD_SHARED_LIBS "Build as a shared library" ON)
# To enable debug builds, use -DCMAKE_BUILD_TYPE=Debug

find_package(Jansson REQUIRED)

if(LIBTMJ_ZSTD)
    find_package(Zstd REQUIRED)
    add_compile_definitions("LIBTMJ_ZSTD")
endif()
if(LIBTMJ_ZLIB)
    find_package(ZLIB REQUIRED)
    add_compile_definitions("LIBTMJ_ZLIB")
endif()

add_library(tmj)
target_sources(tmj
    PUBLIC
        "include/tmj.h"
    PRIVATE
        "src/decode.h"
        "src/decode.c"
        "src/log.c"
        "src/map.c"
        "src/util.c"
)

# Set include directories
target_include_directories(tmj PUBLIC include)
target_include_directories(tmj PRIVATE src)

# Set library version
set_target_properties(tmj PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(tmj PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

# Set link libraries
target_link_libraries(tmj Jansson::Jansson)

if(LIBTMJ_ZSTD)
    target_link_libraries(tmj Zstd::Zstd)
endif()
if(LIBTMJ_ZLIB)
    target_link_libraries(tmj ZLIB::ZLIB)
endif()

# Set compiler/language options
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_FLAGS_RELEASE "-O2")
set(CMAKE_C_FLAGS_DEBUG "-O0 -g3")

# If tests are enabled, make tests
if(LIBTMJ_TEST)
    enable_testing()

    add_executable(map_tests test/map_tests.c test/Unity/src/unity.c)
    add_executable(decode_tests test/decode_tests.c test/Unity/src/unity.c)

    target_link_libraries(map_tests tmj Jansson::Jansson)
    target_link_libraries(decode_tests tmj Jansson::Jansson)

    if(LIBTMJ_ZSTD)
        target_link_libraries(map_tests tmj Zstd::Zstd)
        target_link_libraries(decode_tests tmj Zstd::Zstd)
    endif()
    if(LIBTMJ_ZLIB)
        target_link_libraries(map_tests tmj ZLIB::ZLIB)
        target_link_libraries(decode_tests tmj ZLIB::ZLIB)
    endif()

    set_target_properties(map_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY test/bin)
    set_target_properties(decode_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY test/bin)

    add_test(NAME map_tests COMMAND map_tests WORKING_DIRECTORY test/bin)
    add_test(NAME decode_tests COMMAND decode_tests WORKING_DIRECTORY test/bin)
endif()

# If documentation is enabled, compile docs
if(LIBTMJ_DOCS)
    find_package(Doxygen REQUIRED doxygen dot)

    add_custom_command(TARGET tmj POST_BUILD COMMAND Doxygen::doxygen)
endif()

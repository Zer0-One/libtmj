cmake_minimum_required(VERSION 3.22.2)

project(libtmj VERSION 0.1.0 DESCRIPTION "A C library for loading Tiled maps and tilesets in JSON format")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

# Set output directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY lib)

# Set options
option(LIBTMJ_TEST "Enable unit tests" OFF)
option(LIBTMJ_DOCS "Enable compiling documentation" OFF)
option(LIBTMJ_ZSTD "Enable zstd decompression for tile layers" OFF)
option(LIBTMJ_ZLIB "Enable zlib/gzip decompression for tile layers" OFF)
option(BUILD_SHARED_LIBS "Build as a shared library" ON)
# To enable debug builds, use -DCMAKE_BUILD_TYPE=Debug

if(WIN32)
    find_package(jansson CONFIG REQUIRED)
else()
    find_package(Jansson REQUIRED)
endif()

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
        "src/decode.c"
        "src/log.c"
        "src/tileset.c"
        "src/map.c"
        "src/util.c"
        "src/tmj.def"
)

target_compile_definitions(tmj PUBLIC
    LIBTMJ_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    LIBTMJ_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    LIBTMJ_VERSION_PATCH=${PROJECT_VERSION_PATCH}
    LIBTMJ_VERSION="${PROJECT_VERSION}"
)

# Control symbol visibility
if(NOT WIN32 AND NOT APPLE)
    target_link_options(tmj PRIVATE "LINKER:--version-script=src/tmj.sym")
endif()

# Set include directories
target_include_directories(tmj PUBLIC include)
target_include_directories(tmj PRIVATE src)

# Set library version
set_target_properties(tmj PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(tmj PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

# Set link libraries
target_link_libraries(tmj jansson::jansson)

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
if(WIN32)
    set(CMAKE_C_FLAGS_DEBUG "-Od -Z7")
else()
    set(CMAKE_C_FLAGS_DEBUG "-O0 -g3 -fsanitize=address -fsanitize=undefined") 
endif()

# If tests are enabled, make tests
if(LIBTMJ_TEST)
    enable_testing()

    add_executable(map_tests test/map_tests.c test/Unity/src/unity.c)
    add_executable(tileset_tests test/tileset_tests.c test/Unity/src/unity.c)
    add_executable(decode_tests test/decode_tests.c test/Unity/src/unity.c)
    #add_executable(util_tests test/util_tests.c test/Unity/src/unity.c)

    target_link_libraries(map_tests tmj jansson::jansson)
    target_link_libraries(tileset_tests tmj jansson::jansson)
    target_link_libraries(decode_tests tmj jansson::jansson)
    #target_link_libraries(util_tests tmj jansson::jansson)

    if(LIBTMJ_ZSTD)
        target_link_libraries(decode_tests tmj Zstd::Zstd)
    endif()
    if(LIBTMJ_ZLIB)
        target_link_libraries(decode_tests tmj ZLIB::ZLIB)
    endif()

    set_target_properties(map_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY test/bin)
    set_target_properties(tileset_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY test/bin)
    set_target_properties(decode_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY test/bin)
    #set_target_properties(util_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY test/bin)

    add_test(NAME map_tests COMMAND test/bin/map_tests)
    add_test(NAME tileset_tests COMMAND test/bin/tileset_tests)
    add_test(NAME decode_tests COMMAND test/bin/decode_tests)
    #add_test(NAME util_tests COMMAND test/bin/util_tests)

    # Add library output dir to PATH, because in Windows, the loader will have
    # no clue where to find anything
    if(WIN32)
        set_tests_properties(map_tests PROPERTIES ENVIRONMENT_MODIFICATION
            PATH=path_list_append:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE})
        set_tests_properties(tileset_tests PROPERTIES ENVIRONMENT_MODIFICATION
            PATH=path_list_append:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE})
        set_tests_properties(decode_tests PROPERTIES ENVIRONMENT_MODIFICATION
            PATH=path_list_append:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE})
    endif()
endif()

# If documentation is enabled, compile docs
if(LIBTMJ_DOCS)
    find_package(Doxygen REQUIRED doxygen dot)

    add_custom_command(TARGET tmj POST_BUILD COMMAND Doxygen::doxygen)
endif()

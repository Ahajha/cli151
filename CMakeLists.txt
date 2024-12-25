cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0135 NEW)
project(cli151 LANGUAGES CXX)

if (CMAKE_PROJECT_NAME STREQUAL cli151)
    set(CLI151_IS_TOP_LEVEL ON)
else()
    set(CLI151_IS_TOP_LEVEL OFF)
endif()

option(CLI151_AUTHOR_WARNINGS "Enable CLI151 CMake author warnings" ${CLI151_IS_TOP_LEVEL})
option(CLI151_USE_FETCHCONTENT "Use FetchContent for dependencies, otherwise use find_package" ON)
option(CLI151_ENABLE_TESTS "Enable tests" ${CLI151_IS_TOP_LEVEL})

if (CLI151_IS_TOP_LEVEL)
    if (MSVC)
        add_compile_options(/W4)
    else()
        add_compile_options(-Wall -Wextra -Wconversion -Wshadow -Wpedantic -fno-exceptions -Werror)
    endif()
endif()

# TODO: Flip to not
if (CLI151_USE_FETCHCONTENT)
    include(FetchContent)
    message(STATUS "cli151: Fetching dependencies with FetchContent")
endif()

include(cmake/frozen.cmake)
include(cmake/expected.cmake)
include(cmake/from_chars.cmake)
include(cmake/print.cmake)

add_library(cli151 INTERFACE)
target_include_directories(cli151 INTERFACE include)
target_compile_features(cli151 INTERFACE cxx_std_20)
target_link_libraries(cli151 INTERFACE frozen::frozen std_expected_compat std_from_chars_compat std_print_compat)
if (MSVC)
    # Enable conforming __cplusplus macro
    target_compile_options(cli151 INTERFACE /Zc:__cplusplus)
endif()

if (CLI151_ENABLE_TESTS)
    include (CTest)
    add_subdirectory(test)
endif()

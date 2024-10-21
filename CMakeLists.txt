cmake_minimum_required(VERSION 3.15)
project(cli151 LANGUAGES CXX)

if (MSVC)
    # Enable conforming __cplusplus macro
    add_compile_options(/W4 /Zc:__cplusplus)
else()
    add_compile_options(-Wall -Wextra -Wconversion -Wshadow -fno-exceptions)
endif()

find_package(frozen REQUIRED)

include(cmake/expected.cmake)
include(cmake/from_chars.cmake)
include(cmake/print.cmake)

add_library(cli151 INTERFACE)
target_include_directories(cli151 INTERFACE include)
target_compile_features(cli151 INTERFACE cxx_std_20)
target_link_libraries(cli151 INTERFACE frozen::frozen std_expected_compat std_from_chars_compat)

include (CTest)

add_subdirectory(test)

cmake_minimum_required(VERSION 3.15)
project(cli151 LANGUAGES CXX)

if (MSVC)
    # Enable conforming __cplusplus macro
    add_compile_options(/Zc:__cplusplus)
endif()

find_package(frozen REQUIRED)

# Some of this may need to be moved to the test directory to support use in FetchContent
find_package(tl-expected QUIET)
try_compile(HAS_STD_EXPECTED SOURCES ${CMAKE_SOURCE_DIR}/test/try_include_expected.cpp)

message(STATUS "Has tl-expected:   ${tl-expected_FOUND}")
message(STATUS "Has std::expected: ${HAS_STD_EXPECTED}")
if (${tl-expected_FOUND} AND ${HAS_STD_EXPECTED})
    message(SEND_ERROR "tl::expected added when std::expected is available. Remove the former.")
elseif(NOT ${tl-expected_FOUND} AND NOT ${HAS_STD_EXPECTED})
    message(SEND_ERROR "Neither tl::expected nor std::expected are available.")
endif()

add_library(cli151 INTERFACE)
target_include_directories(cli151 INTERFACE include)
target_link_libraries(cli151 INTERFACE frozen::frozen)
if (HAS_STD_EXPECTED)
    # Use C++23 for std::expected
    target_compile_features(cli151 INTERFACE cxx_std_23)
else()
    # Use tl::expected if either the compiler doesn't support it or we're in 20 mode.
    target_compile_features(cli151 INTERFACE cxx_std_20)
    target_link_libraries(cli151 INTERFACE tl::expected)
endif()

add_executable(unit_tests test/test.cpp)
target_link_libraries(unit_tests PRIVATE cli151)

include (CTest)

add_test(unit_tests unit_tests)

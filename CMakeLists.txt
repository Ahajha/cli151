cmake_minimum_required(VERSION 3.15)
project(cli151 LANGUAGES CXX)

find_package(visit_struct REQUIRED)

add_library(cli151 INTERFACE)
target_include_directories(cli151 INTERFACE include)
# For now use C++23 for std::expected, we will backport to 20 and use
# tl::expected if either the compiler doesn't support it or we're in 20 mode.
target_compile_features(cli151 INTERFACE cxx_std_23)
target_link_libraries(cli151 INTERFACE visit_struct::visit_struct)

add_executable(unit_tests test/test.cpp)
target_link_libraries(unit_tests PRIVATE cli151)

include (CTest)

add_test(unit_tests unit_tests)

add_executable(example test.cpp)
target_link_libraries(example PRIVATE cli151)

include (${CMAKE_CURRENT_SOURCE_DIR}/../cmake/doctest.cmake)

add_executable(unit_tests unit_tests.cpp)
target_link_libraries(unit_tests PRIVATE cli151 doctest::doctest)
target_compile_definitions(unit_tests PRIVATE DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS)

add_test(example example)
add_test(unit_tests unit_tests)

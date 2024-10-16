add_executable(example test.cpp)
target_link_libraries(example PRIVATE cli151)

find_package(doctest REQUIRED)

add_executable(unit_tests unit_tests.cpp)
target_link_libraries(unit_tests PRIVATE cli151 doctest::doctest)
target_compile_definitions(unit_tests PRIVATE DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS)

add_test(example example)
add_test(unit_tests unit_tests)

find_package(tl-expected QUIET)
try_compile(HAS_STD_EXPECTED SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/cmake/try_expected.cpp)

message(STATUS "Has tl-expected:   ${tl-expected_FOUND}")
message(STATUS "Has std::expected: ${HAS_STD_EXPECTED}")
if (${tl-expected_FOUND} AND ${HAS_STD_EXPECTED})
    # With clang/libstdc++ this check is likely to be wrong, but we can't check for libstdc++ usage.
    if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        message(AUTHOR_WARNING "tl::expected added when std::expected is available.")
    endif()
elseif(NOT ${tl-expected_FOUND} AND NOT ${HAS_STD_EXPECTED})
    message(SEND_ERROR "Neither tl::expected nor std::expected are available.")
endif()

add_library(std_expected_compat INTERFACE)
# Prefer std
if (${HAS_STD_EXPECTED})
    target_compile_features(std_expected_compat INTERFACE cxx_std_23)
else()
    target_link_libraries(std_expected_compat INTERFACE tl::expected)
endif()

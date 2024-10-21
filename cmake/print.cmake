find_package(fmt QUIET)
try_compile(HAS_STD_PRINT SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/cmake/try_print.cpp)

message(STATUS "Has fmt:        ${fmt_FOUND}")
message(STATUS "Has std::print: ${HAS_STD_PRINT}")
if (${fmt_FOUND} AND ${HAS_STD_PRINT})
    # With clang/libstdc++ this check is likely to be wrong, but we can't check for libstdc++ usage.
    if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        message(AUTHOR_WARNING "fmt::print added when std::print is available.")
    endif()
elseif(NOT ${fmt_FOUND} AND NOT ${HAS_STD_PRINT})
    message(SEND_ERROR "Neither fmt::print nor std::print are available.")
endif()

add_library(std_print_compat INTERFACE)
# Prefer std
if (${HAS_STD_PRINT})
    target_compile_features(std_print_compat INTERFACE cxx_std_23)
else()
    target_link_libraries(std_print_compat INTERFACE fmt::fmt)
endif()

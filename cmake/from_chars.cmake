find_package(FastFloat QUIET)
try_compile(HAS_STD_FROM_CHARS SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/cmake/try_from_chars.cpp)

message(STATUS "Has fast_float::from_chars: ${FastFloat_FOUND}")
message(STATUS "Has std::from_chars       : ${HAS_STD_FROM_CHARS}")
if (${FastFloat_FOUND} AND ${HAS_STD_FROM_CHARS})
    # With clang/libstdc++ this check is likely to be wrong, but we can't check for libstdc++ usage.
    if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        message(AUTHOR_WARNING "fast_float::from_chars added when std::from_chars is available.")
    endif()
elseif(NOT ${FastFloat_FOUND} AND NOT ${HAS_STD_FROM_CHARS})
    message(SEND_ERROR "Neither fast_float::from_chars nor std::from_chars are available.")
endif()

add_library(std_from_chars_compat INTERFACE)
# Prefer std
if (${HAS_STD_FROM_CHARS})
    target_compile_features(std_from_chars_compat INTERFACE cxx_std_17)
else()
    target_link_libraries(std_from_chars_compat INTERFACE FastFloat::fast_float)
endif()

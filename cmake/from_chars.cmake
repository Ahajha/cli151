try_compile(HAS_STD_FROM_CHARS SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/try_from_chars.cpp)
add_library(std_from_chars_compat INTERFACE)

# Prefer std
if (${HAS_STD_FROM_CHARS})
    message(STATUS "cli151: Using std::from_chars")
    target_compile_features(std_from_chars_compat INTERFACE cxx_std_17)

    if (CLI151_AUTHOR_WARNINGS)
        find_package(FastFloat QUIET)
        if (${FastFloat_FOUND})
            # With clang/libstdc++ this check is likely to be wrong, but we can't check for libstdc++ usage.
            if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
                message(AUTHOR_WARNING "fast_float::from_chars added when std::from_chars is available.")
            endif()
        endif()
    endif()
elseif (CLI151_USE_FETCHCONTENT)
    message(STATUS "cli151: Using fast_float::from_chars (FetchContent)")
    set(FASTFLOAT_CXX_STANDARD ${CMAKE_CXX_STANDARD})
    FetchContent_Declare(
        fast_float
        GIT_REPOSITORY https://github.com/fastfloat/fast_float.git
        GIT_TAG        v7.0.0
    )
    FetchContent_MakeAvailable(fast_float)

    target_link_libraries(std_from_chars_compat INTERFACE fast_float)
else()
    message(STATUS "cli151: Using fast_float::from_chars (find_package)")

    find_package(FastFloat REQUIRED)
    target_link_libraries(std_from_chars_compat INTERFACE fast_float)
endif()

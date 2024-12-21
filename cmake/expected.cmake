try_compile(HAS_STD_EXPECTED SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/try_expected.cpp)
add_library(std_expected_compat INTERFACE)

# Prefer std
if (${HAS_STD_EXPECTED})
    message(STATUS "cli151: Using std::expected")
    target_compile_features(std_expected_compat INTERFACE cxx_std_23)

    # Sanity check - make sure we're not also pulling in tl-expected
    if (CLI151_AUTHOR_WARNINGS)
        find_package(tl-expected QUIET)
        if (${tl-expected_FOUND})
            # With clang/libstdc++ this check is likely to be wrong, but we can't check for libstdc++ usage.
            if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
                message(AUTHOR_WARNING "tl::expected added when std::expected is available.")
            endif()
        endif()
    endif()
elseif (CLI151_USE_FETCHCONTENT)
    message(STATUS "cli151: Using tl-expected (FetchContent)")
    set(EXPECTED_BUILD_TESTS OFF)
    FetchContent_Declare(
        tl-expected
        GIT_REPOSITORY https://github.com/TartanLlama/expected.git
        GIT_TAG        v1.1.0
    )
    FetchContent_MakeAvailable(tl-expected)

    target_link_libraries(std_expected_compat INTERFACE tl::expected)
else()
    message(STATUS "cli151: Using tl-expected (find_package)")

    find_package(tl-expected REQUIRED)
    target_link_libraries(std_expected_compat INTERFACE tl::expected)
endif()

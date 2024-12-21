try_compile(HAS_STD_PRINT SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/try_print.cpp)
add_library(std_print_compat INTERFACE)

# Prefer std
if (${HAS_STD_PRINT})
    message(STATUS "cli151: Using std::print")
    target_compile_features(std_print_compat INTERFACE cxx_std_23)

    if (CLI151_AUTHOR_WARNINGS)
        find_package(fmt QUIET)
        if (${fmt_FOUND})
            # With clang/libstdc++ this check is likely to be wrong, but we can't check for libstdc++ usage.
            if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
                message(AUTHOR_WARNING "fmt::print added when std::print is available.")
            endif()
        endif()
    endif()
elseif (CLI151_USE_FETCHCONTENT)
    message(STATUS "cli151: Using fmt::print (FetchContent)")
    FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG        11.0.2
    )
    FetchContent_MakeAvailable(fmt)

    target_link_libraries(std_print_compat INTERFACE fmt::fmt)
else()
    message(STATUS "cli151: Using fmt::print (find_package)")

    find_package(fmt REQUIRED)
    target_link_libraries(std_print_compat INTERFACE fmt::fmt)
endif()

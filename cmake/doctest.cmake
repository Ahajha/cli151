if (CLI151_USE_FETCHCONTENT)
    message(STATUS "cli151: Using doctest (FetchContent)")
    FetchContent_Declare(
        doctest
        GIT_REPOSITORY https://github.com/doctest/doctest.git
        GIT_TAG        v2.4.11
        PATCH_COMMAND git apply ${CMAKE_CURRENT_LIST_DIR}/patches/doctest_suppress_cmake_warning.patch
        UPDATE_DISCONNECTED 1
    )
    FetchContent_MakeAvailable(doctest)
else()
    message(STATUS "cli151: Using doctest (find_package)")
    
    find_package(doctest REQUIRED)
endif()

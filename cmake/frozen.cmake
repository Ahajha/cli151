if (CLI151_USE_FETCHCONTENT)
    message(STATUS "cli151: Using frozen (FetchContent)")
    FetchContent_Declare(
        frozen
        GIT_REPOSITORY https://github.com/serge-sans-paille/frozen.git
        GIT_TAG        1.2.0
        PATCH_COMMAND git apply ${CMAKE_CURRENT_LIST_DIR}/patches/frozen_suppress_cmake_warning.patch
        UPDATE_DISCONNECTED 1
    )
    FetchContent_MakeAvailable(frozen)
else()
    message(STATUS "cli151: Using frozen (find_package)")

    find_package(frozen REQUIRED)
endif()

cmake_minimum_required(VERSION 3.24)
project(tklog LANGUAGES C)

include(FetchContent)

# ------------------------------------------------------------
# Options
# ------------------------------------------------------------
option(TKLOG_BUILD_TESTS  "Build test program"           ON)
option(TKLOG_INSTALL      "Install tklog target"         ON)

# ------------------------------------------------------------
# SDL3
# ------------------------------------------------------------
FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL
    GIT_TAG        release-3.2.0
)
set(SDL_SHARED OFF CACHE BOOL "Build SDL3 as a static library" FORCE)
set(SDL_STATIC ON  CACHE BOOL "Build SDL3 as a static library" FORCE)
FetchContent_MakeAvailable(SDL3)

# ------------------------------------------------------------
# Verstable (header-only)
# ------------------------------------------------------------
FetchContent_Declare(
    verstable
    GIT_REPOSITORY https://github.com/JacksonAllan/Verstable.git
    GIT_TAG        main
)
FetchContent_MakeAvailable(verstable)

# ------------------------------------------------------------
# tklog library
# ------------------------------------------------------------
add_library(tklog STATIC src/tklog.c)
add_library(tklog::tklog ALIAS tklog)
target_compile_definitions(tklog
    PUBLIC                    # or INTERFACE
        TKLOG_SCOPE
        TKLOG_MEMORY
        TKLOG_SHOW_TKLOG_LEVEL
        TKLOG_SHOW_TIME
        TKLOG_SHOW_THREAD
        TKLOG_SHOW_PATH
        TKLOG_DEBUG
        TKLOG_INFO
        TKLOG_NOTICE
        TKLOG_WARNING
        TKLOG_ERROR
        TKLOG_CRITICAL
        TKLOG_ALERT
        TKLOG_EMERGENCY
)


target_include_directories(tklog PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${verstable_SOURCE_DIR}
)

target_link_libraries(tklog
    PUBLIC
        SDL3::SDL3
)

set_target_properties(tklog PROPERTIES
    C_STANDARD 11
    POSITION_INDEPENDENT_CODE ON
)

# ------------------------------------------------------------
# Installation
# ------------------------------------------------------------
if(TKLOG_INSTALL AND CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(GNUInstallDirs)

    install(TARGETS tklog
        EXPORT tklogTargets
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        ARCHIVE       DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY       DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
    install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    include(CMakePackageConfigHelpers)
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/tklogConfigVersion.cmake"
        VERSION 0.1.0
        COMPATIBILITY AnyNewerVersion
    )
    configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/tklogConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/tklogConfig.cmake"
        INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/tklog"
    )
    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/tklogConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/tklogConfigVersion.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/tklog"
    )
    install(EXPORT tklogTargets
        NAMESPACE tklog::
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/tklog"
    )
endif()

# ------------------------------------------------------------
# Tests
# ------------------------------------------------------------
if(TKLOG_BUILD_TESTS)
    enable_testing()

    add_executable(test_tklog test/test_1.c)
    target_link_libraries(test_tklog PRIVATE tklog::tklog)
    add_test(NAME tklog.basic COMMAND test_tklog)
endif()
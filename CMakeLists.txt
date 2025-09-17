# CMakeLists.txt - Improved build configuration for tklog logging library
# Based on the provided working LOGOS CMakeLists.txt for enhanced features
# Minimal CMake required (updated to 3.16 for better FetchContent support)
cmake_minimum_required(VERSION 3.16)

# Project name and language (fixed: LANGUAGES before C for VERSION support)
project(tklog VERSION 1.0.0 LANGUAGES C)

# Set C standard (updated to C99 as per tklog requirements)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Build options (adapted from LOGOS: SAFE DEFAULTS)
option(TKLOG_ADDRESS_SANITIZER "Enable Address Sanitizer" OFF)
option(TKLOG_THREAD_SANITIZER "Enable Thread Sanitizer" OFF)
option(TKLOG_ENABLE_TIMER "Enable TKLOG_TIMER feature (requires verstable)" ON)

# Set output directories (adapted from LOGOS)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# Enable IPO/LTO for release builds if supported (from LOGOS)
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported LANGUAGES C)

# Platform detection (adapted from LOGOS, simplified for tklog)
if(WIN32)
    set(TKLOG_PLATFORM_WINDOWS ON)
elseif(APPLE)
    set(TKLOG_PLATFORM_MACOS ON)
else()
    set(TKLOG_PLATFORM_LINUX ON)
endif()

# Compiler detection and flags (adapted from LOGOS)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(TKLOG_COMPILER_GCC_CLANG ON)
    set(TKLOG_COMMON_FLAGS "-Werror" "-Werror=return-type")  # "-Wall" "-Wextra"
    set(TKLOG_DEBUG_FLAGS "-g" "-O1" "-DDEBUG")
    set(TKLOG_RELEASE_FLAGS "-Ofast" "-march=native" "-DNDEBUG")

    # Sanitizer flags (from LOGOS)
    if(TKLOG_ADDRESS_SANITIZER)
        set(TKLOG_SANITIZER_FLAGS "-fsanitize=address" "-fno-omit-frame-pointer")
        set(ENV{ASAN_OPTIONS} "detect_leaks=1:abort_on_error=1")
        message(STATUS "Address Sanitizer enabled")
    elseif(TKLOG_THREAD_SANITIZER)
        set(TKLOG_SANITIZER_FLAGS "-fsanitize=thread" "-fno-omit-frame-pointer")
        # Note: Suppressions file not included here; add if needed
        set(ENV{TSAN_OPTIONS} "abort_on_error=1:history_size=5:verbosity=2")
        message(STATUS "Thread Sanitizer enabled")
    endif()
elseif(MSVC)
    set(TKLOG_COMPILER_MSVC ON)
    set(TKLOG_COMMON_FLAGS "/W4" "/WX" "/D_CRT_SECURE_NO_WARNINGS")
    set(TKLOG_DEBUG_FLAGS "/Zi" "/Od" "/DDEBUG")
    set(TKLOG_RELEASE_FLAGS "/O2" "/DNDEBUG")

    # Sanitizers not supported on MSVC
    if(TKLOG_ADDRESS_SANITIZER OR TKLOG_THREAD_SANITIZER)
        message(WARNING "Sanitizers are not supported with MSVC compiler")
        set(TKLOG_ADDRESS_SANITIZER OFF)
        set(TKLOG_THREAD_SANITIZER OFF)
    endif()
endif()

# Find required packages (from LOGOS)
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)

# Fetch verstable (added as requested, from LOGOS)
include(FetchContent)
FetchContent_Declare(
    verstable
    GIT_REPOSITORY https://github.com/JacksonAllan/Verstable.git
    GIT_TAG main
)
FetchContent_MakeAvailable(verstable)

# Add the tklog library
add_library(tklog STATIC tklog.c)

# Apply compiler flags to tklog (from LOGOS)
if(TKLOG_COMPILER_GCC_CLANG)
    target_compile_options(tklog PRIVATE ${TKLOG_COMMON_FLAGS} $<$<CONFIG:Release>:${TKLOG_RELEASE_FLAGS}> $<$<CONFIG:Debug>:${TKLOG_DEBUG_FLAGS}> ${TKLOG_SANITIZER_FLAGS})
elseif(TKLOG_COMPILER_MSVC)
    target_compile_options(tklog PRIVATE ${TKLOG_COMMON_FLAGS} $<$<CONFIG:Release>:${TKLOG_RELEASE_FLAGS}> $<$<CONFIG:Debug>:${TKLOG_DEBUG_FLAGS}>)
endif()

# Link threads to tklog
target_link_libraries(tklog PRIVATE Threads::Threads)

# Include directories for tklog
target_include_directories(tklog PRIVATE . ${verstable_SOURCE_DIR})

# Compile definitions for tklog (base flags; timer conditional)
target_compile_definitions(tklog PRIVATE
    TKLOG_DEBUG
    TKLOG_INFO
    TKLOG_NOTICE
    TKLOG_WARNING
    TKLOG_ERROR
    TKLOG_CRITICAL
    TKLOG_ALERT
    TKLOG_EMERGENCY
    TKLOG_SHOW_LOG_LEVEL
    TKLOG_SHOW_TIME
    TKLOG_SHOW_THREAD
    TKLOG_SHOW_PATH
    TKLOG_SCOPE
    TKLOG_MEMORY
    TKLOG_MEMORY_PRINT_ON_EXIT
)
if(TKLOG_ENABLE_TIMER)
    target_compile_definitions(tklog PRIVATE TKLOG_TIMER)
endif()

# Enable LTO for tklog if supported (from LOGOS)
if(ipo_supported)
    set_property(TARGET tklog PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
endif()

# Add the test executable (tklog_test; assumes test.c is present, or replace with provided main file as test_tsm.c)
add_executable(tklog_test test.c)  # If using provided main, change to: add_executable(tklog_test test_tsm.c)

# Apply compiler flags to tklog_test (from LOGOS)
if(TKLOG_COMPILER_GCC_CLANG)
    target_compile_options(tklog_test PRIVATE ${TKLOG_COMMON_FLAGS} $<$<CONFIG:Release>:${TKLOG_RELEASE_FLAGS}> $<$<CONFIG:Debug>:${TKLOG_DEBUG_FLAGS}> ${TKLOG_SANITIZER_FLAGS})
elseif(TKLOG_COMPILER_MSVC)
    target_compile_options(tklog_test PRIVATE ${TKLOG_COMMON_FLAGS} $<$<CONFIG:Release>:${TKLOG_RELEASE_FLAGS}> $<$<CONFIG:Debug>:${TKLOG_DEBUG_FLAGS}>)
endif()

# Link tklog and threads to test executable
target_link_libraries(tklog_test PRIVATE tklog Threads::Threads)

# Include directories for test (added verstable)
target_include_directories(tklog_test PRIVATE . ${verstable_SOURCE_DIR})

# Compile definitions for test (same as library, with timer if enabled)
target_compile_definitions(tklog_test PRIVATE
    TKLOG_DEBUG
    TKLOG_INFO
    TKLOG_NOTICE
    TKLOG_WARNING
    TKLOG_ERROR
    TKLOG_CRITICAL
    TKLOG_ALERT
    TKLOG_EMERGENCY
    TKLOG_SHOW_LOG_LEVEL
    TKLOG_SHOW_TIME
    TKLOG_SHOW_THREAD
    TKLOG_SHOW_PATH
    TKLOG_SCOPE
    TKLOG_MEMORY
    TKLOG_MEMORY_PRINT_ON_EXIT
)
if(TKLOG_ENABLE_TIMER)
    target_compile_definitions(tklog_test PRIVATE TKLOG_TIMER)
endif()

# Enable LTO for test if supported (from LOGOS)
if(ipo_supported)
    set_property(TARGET tklog_test PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
endif()

# Add sanitizer libraries if enabled (adapted from LOGOS; must be first)
if(TKLOG_ADDRESS_SANITIZER AND TKLOG_COMPILER_GCC_CLANG)
    target_link_libraries(tklog_test PRIVATE asan)
elseif(TKLOG_THREAD_SANITIZER AND TKLOG_COMPILER_GCC_CLANG)
    target_link_libraries(tklog_test PRIVATE tsan)
endif()

# Optional: Install targets (from LOGOS)
# install(TARGETS tklog DESTINATION lib)
# install(FILES tklog.h DESTINATION include)

# Build and run test by default (from original)
enable_testing()
add_test(NAME tklog_test COMMAND tklog_test)

# Optional: Package configuration (adapted from LOGOS, simplified)
set(CPACK_PACKAGE_NAME "tklog")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "tklog Lightweight Logging Library")
set(CPACK_PACKAGE_VENDOR "tklog Team")
if(TKLOG_PLATFORM_WINDOWS)
    set(CPACK_GENERATOR "NSIS;ZIP")
elseif(TKLOG_PLATFORM_MACOS)
    set(CPACK_GENERATOR "DragNDrop;TGZ")
else()
    set(CPACK_GENERATOR "DEB;RPM;TGZ")
endif()
include(CPack)
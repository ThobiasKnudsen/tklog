cmake_minimum_required(VERSION 3.24)
project(tklog LANGUAGES C)
include(FetchContent)

# ------------------------------------------------------------
# Options
# ------------------------------------------------------------
option(TKLOG_ENABLE_SCOPE   "Enable scope-tracing helpers" ON)
option(TKLOG_ENABLE_MEMORY  "Enable malloc/realloc/free tracking" ON)
option(TKLOG_BUILD_TESTS    "Build test program" ON)
option(TKLOG_INSTALL        "Install header + package" ON)

# ------------------------------------------------------------
# SDL 3
# ------------------------------------------------------------
FetchContent_Declare(
  SDL3
  GIT_REPOSITORY https://github.com/libsdl-org/SDL
  GIT_TAG        release-3.2.0
)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON  CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(SDL3)

# ------------------------------------------------------------
# Verstable (header-only dep)
# ------------------------------------------------------------
FetchContent_Declare(
  verstable
  GIT_REPOSITORY https://github.com/JacksonAllan/Verstable.git
  GIT_TAG        main
)
FetchContent_MakeAvailable(verstable)

# ------------------------------------------------------------
# Header-only tklog target (INTERFACE library)
# ------------------------------------------------------------
# Create an INTERFACE library (header-only)
add_library(tklog INTERFACE)
add_library(tklog::tklog ALIAS tklog)

# Configure the include directories
target_include_directories(tklog INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${verstable_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Set compile definitions based on options
if(TKLOG_ENABLE_SCOPE)
  target_compile_definitions(tklog INTERFACE TKLOG_SCOPE)
endif()

if(TKLOG_ENABLE_MEMORY)
  target_compile_definitions(tklog INTERFACE TKLOG_MEMORY)
endif()

# Link with SDL3
target_link_libraries(tklog INTERFACE SDL3::SDL3)

# ------------------------------------------------------------
# Tests
# ------------------------------------------------------------
if(TKLOG_BUILD_TESTS)
  enable_testing()
  
  # Create a test implementation object
  add_library(tklog_impl OBJECT test/tklog_impl.c)
  target_compile_definitions(tklog_impl PRIVATE TKLOG_IMPLEMENTATION)
  target_link_libraries(tklog_impl PRIVATE tklog)
  
  # Create the test executable
  add_executable(test_tklog test/test_1.c $<TARGET_OBJECTS:tklog_impl>)
  target_link_libraries(test_tklog PRIVATE tklog)
  
  add_test(NAME tklog.basic COMMAND test_tklog)
endif()

# ------------------------------------------------------------
# Install (only the header + package files)
# ------------------------------------------------------------
if(TKLOG_INSTALL AND CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  include(GNUInstallDirs)
  
  # 1) Install header
  install(
    FILES       ${CMAKE_CURRENT_SOURCE_DIR}/tklog.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )
  
  # 2) Install CMake package configuration
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/tklogConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/tklogConfig.cmake
    @ONLY
  )
  
  include(CMakePackageConfigHelpers)
  write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/tklogConfigVersion.cmake"
    VERSION 0.1.0
    COMPATIBILITY AnyNewerVersion
  )
  
  install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/tklogConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/tklogConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/tklog"
  )
  
  # 3) Install targets
  install(TARGETS tklog
    EXPORT tklogTargets
  )
  
  install(EXPORT tklogTargets
    FILE tklogTargets.cmake
    NAMESPACE tklog::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/tklog"
  )
endif()
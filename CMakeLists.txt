cmake_minimum_required(VERSION 3.24)
project(TKLogTest LANGUAGES C)

include(FetchContent)

# Fetch SDL3
FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL
    GIT_TAG release-3.2.0
)
set(SDL_SHARED OFF CACHE BOOL "Build SDL3 as a static library" FORCE)
set(SDL_STATIC ON CACHE BOOL "Build SDL3 as a static library" FORCE)
FetchContent_MakeAvailable(SDL3)

# Build test program
add_executable(main main.c)
target_link_libraries(main PRIVATE SDL3::SDL3-static)
set_target_properties(main PROPERTIES
    C_STANDARD 11
)
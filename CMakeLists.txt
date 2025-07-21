cmake_minimum_required(VERSION 3.10)
project(sdlerror LANGUAGES C)

find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
find_package(PkgConfig REQUIRED)

# Check for SDL3 using pkg-config
pkg_check_modules(SDL3 REQUIRED sdl3)

# Add include directories
include_directories(${SDL3_INCLUDE_DIRS})
add_library(sdlerror STATIC
    src/error.c
)

# Specify include directories for the library's headers (if applicable)
target_include_directories(sdlerror PUBLIC
    include/
)

set(main_lib_dest "lib/my_library-${MY_LIBRARY_VERSION}")
install(TARGETS sdlerror DESTINATION "lib/")
install(FILES "include/sdlerror.h" DESTINATION "include/")

cmake_minimum_required(VERSION 3.10)
project(sdlerror LANGUAGES C)

find_package(PkgConfig REQUIRED)
find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-static)

# Check for SDL3 using pkg-config
pkg_check_modules(SDL3 REQUIRED sdl3)

# Add include directories
include_directories(${SDL3_INCLUDE_DIRS})
add_library(sdlerror STATIC
    src/error.c
)

add_executable(test_err_catch tests/err_catch.c)
add_executable(test_err_cleanup tests/err_cleanup.c)
add_executable(test_err_trace tests/err_trace.c)
add_test(NAME err_catch COMMAND test_err_catch)
add_test(NAME err_cleanup COMMAND test_err_cleanup)
add_test(NAME err_trace COMMAND test_err_trace)

# Specify include directories for the library's headers (if applicable)
target_include_directories(sdlerror PUBLIC
    include/
  )
target_link_libraries(test_err_catch PRIVATE sdlerror SDL3::SDL3)
target_link_libraries(test_err_cleanup PRIVATE sdlerror SDL3::SDL3)
target_link_libraries(test_err_trace PRIVATE sdlerror SDL3::SDL3)

set(main_lib_dest "lib/my_library-${MY_LIBRARY_VERSION}")
install(TARGETS sdlerror DESTINATION "lib/")
install(FILES "include/sdlerror.h" DESTINATION "include/")

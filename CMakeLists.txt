cmake_minimum_required(VERSION 3.10.2)
project(ST-3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(tool_dest "bin")
set(lib_dest "lib")
set(include_dest "include/")

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.0
)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_subdirectory(src)
add_subdirectory(test)

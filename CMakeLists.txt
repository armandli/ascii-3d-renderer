cmake_minimum_required(VERSION 3.28)

project(ascii3d VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

option(ASCII3D_BUILD_TESTS "Build tests" ON)

add_library(ascii3d_warnings INTERFACE)
target_compile_options(ascii3d_warnings INTERFACE
  -Wall -Wextra -Wpedantic -Wshadow -Wconversion)

add_subdirectory(src)

if(ASCII3D_BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()

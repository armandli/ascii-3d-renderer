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
option(ASCII3D_SANITIZE "Enable ASan + UBSan for the library, demo and tests" OFF)

# Applied directory-wide so the ascii3d library is instrumented along with the
# tests; sanitizing only the test executable would miss UB inside the library.
if(ASCII3D_SANITIZE)
  add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer
    -fno-sanitize-recover=undefined)
  add_link_options(-fsanitize=address,undefined)
endif()

add_library(ascii3d_warnings INTERFACE)
target_compile_options(ascii3d_warnings INTERFACE
  -Wall -Wextra -Wpedantic -Wshadow -Wconversion)

add_subdirectory(src)

if(ASCII3D_BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()

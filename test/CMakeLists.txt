include(FetchContent)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP ON
  SYSTEM
)
FetchContent_MakeAvailable(googletest)

# Silence a warning newer clang emits while compiling GoogleTest's own sources.
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-Wno-character-conversion ASCII3D_HAS_WNO_CHARACTER_CONVERSION)
if(ASCII3D_HAS_WNO_CHARACTER_CONVERSION)
  target_compile_options(gtest PRIVATE -Wno-character-conversion)
endif()

add_executable(ascii3d_tests
  smoke_test.cpp
)
target_link_libraries(ascii3d_tests PRIVATE ascii3d ascii3d_warnings GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(ascii3d_tests)

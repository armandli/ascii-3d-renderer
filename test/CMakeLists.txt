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
  helpers_test.cpp
  vector_test.cpp
)
target_link_libraries(ascii3d_tests PRIVATE ascii3d ascii3d_warnings GTest::gtest_main)
target_include_directories(ascii3d_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Expose the golden-file directory to test code as a compile-time string.
target_compile_definitions(ascii3d_tests PRIVATE
    GOLDEN_DIR="${CMAKE_CURRENT_SOURCE_DIR}/golden")

# Optional: enable AddressSanitizer + UBSanitizer for the test build.
option(ASCII3D_SANITIZE "Enable ASan + UBSan in the test build" OFF)
if(ASCII3D_SANITIZE)
  target_compile_options(ascii3d_tests PRIVATE -fsanitize=address,undefined)
  target_link_options(ascii3d_tests PRIVATE -fsanitize=address,undefined)
endif()

include(GoogleTest)
gtest_discover_tests(ascii3d_tests)

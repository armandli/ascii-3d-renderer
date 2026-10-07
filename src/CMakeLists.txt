add_library(ascii3d STATIC
  scene/mesh.cpp
  scene/object.cpp
  scene/camera.cpp
  scene/projection.cpp
  render/framebuffer.cpp
  render/rasterizer.cpp
  render/shader.cpp
  render/renderer.cpp
)
target_include_directories(ascii3d PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ascii3d PRIVATE ascii3d_warnings)

add_executable(ascii3d_demo main.cpp)
target_link_libraries(ascii3d_demo PRIVATE ascii3d ascii3d_warnings)

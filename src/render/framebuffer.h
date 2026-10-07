#pragma once

#include <cstddef>
#include <vector>

namespace ascii3d {

// Character grid plus depth buffer.
// TODO: clear, depth-tested set_pixel, present to the terminal.
class Framebuffer {
public:
  std::size_t width = 0;
  std::size_t height = 0;
  std::vector<char> chars;
  std::vector<float> depth;
};

} // namespace ascii3d

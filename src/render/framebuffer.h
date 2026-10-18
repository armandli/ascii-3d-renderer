#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <cstddef>

#include <vector>

namespace ascii3d {

// Depth is z_view (= clip w): smaller is closer, cleared to +infinity, and a
// write passes only on strict < (equal depth is rejected).
// TODO: clear, depth-tested set_pixel, present to the terminal.
struct Framebuffer {
  std::size_t width = 0;
  std::size_t height = 0;
  std::vector<char> chars;
  std::vector<float> depth;
};

}  // namespace ascii3d

#endif  // FRAMEBUFFER_H

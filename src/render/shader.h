#ifndef SHADER_H
#define SHADER_H

#include <string_view>

namespace ascii3d {

// Fragment stage: brightness from surface normal and light direction,
// mapped onto a character ramp from dark to bright.
// TODO: face normal normalize(cross(b - a, c - a)) (toward the viewer for
// clockwise front faces), Lambert term with the default headlight (0, 0, -1)
// in view space (direction toward the light), ramp lookup.
struct Shader {
  std::string_view ramp = ".;ox%@";
};

}  // namespace ascii3d

#endif  // SHADER_H

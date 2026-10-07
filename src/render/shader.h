#pragma once

#include <string_view>

namespace ascii3d {

// Fragment stage: brightness from surface normal and light direction,
// mapped onto a character ramp from dark to bright.
// TODO: face normal (cross product), Lambert term, ramp lookup.
class Shader {
public:
  std::string_view ramp = ".;ox%@";
};

} // namespace ascii3d

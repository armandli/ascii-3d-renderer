#pragma once

#include "math/vector.h"

#include <array>

namespace ascii3d {

// A triangle made of three vertices.
struct Polygon {
  std::array<Vector3, 3> vertices{};
};

} // namespace ascii3d

#pragma once

#include <array>

namespace ascii3d {

// 4x4 row-major matrix.
// TODO: identity, multiply (matrix * matrix, matrix * Vector4),
// translation / rotation / scale factories.
struct Matrix44 {
  std::array<std::array<float, 4>, 4> m{};
};

} // namespace ascii3d

#pragma once

#include "math/matrix.h"

namespace ascii3d {

// TODO: build the perspective projection matrix.
class Projection {
public:
  float fov_degrees = 60.0f;
  float aspect = 1.0f;
  float near_plane = 0.1f;
  float far_plane = 100.0f;
};

} // namespace ascii3d

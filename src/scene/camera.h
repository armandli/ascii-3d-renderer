#pragma once

#include "math/matrix.h"
#include "math/vector.h"

namespace ascii3d {

// TODO: build the view matrix from eye, look direction and up vector.
class Camera {
public:
  Vector3 eye{0.0f, 0.0f, -5.0f};
  Vector3 look{0.0f, 0.0f, 1.0f};
  Vector3 up{0.0f, 1.0f, 0.0f};
};

} // namespace ascii3d

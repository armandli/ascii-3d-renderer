#pragma once

#include "math/matrix.h"
#include "math/vector.h"
#include "scene/mesh.h"

namespace ascii3d {

// A mesh placed in the world.
// TODO: build the model (world) matrix from scale, rotation and position.
class Object {
public:
  Mesh mesh;
  Vector3 position{};
  Vector3 rotation{};
  Vector3 scale{1.0f, 1.0f, 1.0f};
};

} // namespace ascii3d

#ifndef OBJECT_H
#define OBJECT_H

#include <math/matrix.h>
#include <math/vector.h>
#include <scene/mesh.h>

namespace ascii3d {

// TODO: model_matrix() = T * Rz * Ry * Rx * S (column vectors: scale first,
// then rotate about x, y, z, then translate). Rotation is in radians.
struct Object {
  Mesh mesh;
  Vector3 position{};
  Vector3 rotation{};
  Vector3 scale{1.0f, 1.0f, 1.0f};
};

}  // namespace ascii3d

#endif  // OBJECT_H

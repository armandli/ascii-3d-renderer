#ifndef POLYGON_H
#define POLYGON_H

#include "math/vector.h"

#include <array>

namespace ascii3d {

// A triangle made of three vertices. Front faces are wound clockwise as seen
// from the camera, so cross(v[1] - v[0], v[2] - v[0]) points toward the viewer
// (outward on a closed mesh).
struct Polygon {
  std::array<Vector3, 3> vertices{};
};

} // namespace ascii3d

#endif  // POLYGON_H

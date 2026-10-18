#ifndef POLYGON_H
#define POLYGON_H

#include <array>

#include <math/vector.h>

namespace ascii3d {

// Always a triangle. Front faces are wound clockwise as seen from the camera,
// so cross(v[1] - v[0], v[2] - v[0]) points toward the viewer (outward on a
// closed mesh).
struct Polygon {
  std::array<Vector3, 3> vertices{};
};

}  // namespace ascii3d

#endif  // POLYGON_H

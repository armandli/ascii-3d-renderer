#ifndef PROJECTION_H
#define PROJECTION_H

#include <math/matrix.h>

namespace ascii3d {

// Left-handed, column-vector perspective projection: clip w = z_view and
// NDC z in [-1, 1] (near -> -1, far -> +1).
// TODO: matrix() with g = 1 / tan(fov / 2), rows [g / aspect, 0, 0, 0],
// [0, g, 0, 0], [0, 0, (f + n) / (f - n), -2fn / (f - n)], [0, 0, 1, 0].
struct Projection {
  // The only angle not in radians.
  float fov_degrees = 60.0f;
  // (width / cell_aspect) / height, with cell_aspect = 2 by default.
  float aspect = 1.0f;
  float near_plane = 0.1f;
  float far_plane = 100.0f;
};

}  // namespace ascii3d

#endif  // PROJECTION_H

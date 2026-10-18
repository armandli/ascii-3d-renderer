#ifndef CAMERA_H
#define CAMERA_H

#include "math/matrix.h"
#include "math/vector.h"

namespace ascii3d {

// Left-handed look-at camera: in view space the camera sits at the origin,
// x right, y up, looking down +Z (visible points have z_view > 0).
// TODO: view_matrix() with f = normalize(target - eye),
// r = normalize(cross(up, f)), u = cross(f, r); rows r, u, f and
// translation column (-dot(r, eye), -dot(u, eye), -dot(f, eye)).
class Camera {
public:
  Vector3 eye{0.0f, 0.0f, -5.0f};
  Vector3 target{0.0f, 0.0f, 0.0f}; // point looked at (the reference calls it `look`)
  Vector3 up{0.0f, 1.0f, 0.0f};
};

} // namespace ascii3d

#endif  // CAMERA_H

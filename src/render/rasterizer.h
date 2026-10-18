#ifndef RASTERIZER_H
#define RASTERIZER_H

namespace ascii3d {

// Turns projected triangles into screen-space fragments.
// TODO: perspective divide, viewport transform, triangle fill
// (bounding box + edge functions / barycentric), depth interpolation.
// Screen space: x in [0, W) right, y in [0, H) down; sample at pixel centers
// (x + 0.5, y + 0.5). The viewport's y flip reverses the sign of the signed
// area, so clockwise front faces have the opposite edge-function sign here.
class Rasterizer {
};

} // namespace ascii3d

#endif  // RASTERIZER_H

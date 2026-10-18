#ifndef MESH_H
#define MESH_H

#include "scene/polygon.h"

#include <vector>

namespace ascii3d {

// TODO: primitive builders (cube, sphere, torus) and model loading.
// Primitives emit triangles clockwise as seen from outside; the OBJ loader
// can reverse winding for counter-clockwise (OpenGL-style) assets.
class Mesh {
public:
  std::vector<Polygon> polygons;
};

} // namespace ascii3d

#endif  // MESH_H

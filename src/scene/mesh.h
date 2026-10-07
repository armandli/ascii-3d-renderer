#pragma once

#include "scene/polygon.h"

#include <vector>

namespace ascii3d {

// TODO: primitive builders (cube, sphere, torus) and model loading.
class Mesh {
public:
  std::vector<Polygon> polygons;
};

} // namespace ascii3d

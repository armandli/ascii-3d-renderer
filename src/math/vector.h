#pragma once

namespace ascii3d {

// TODO: arithmetic operators, dot, cross, length, normalize.
struct Vector2 {
  float x = 0.0f;
  float y = 0.0f;
};

struct Vector3 {
  float x = 0.0f;
  float y = 0.0f;
  float z = 0.0f;
};

// Homogeneous coordinate; w is used for the perspective divide.
struct Vector4 {
  float x = 0.0f;
  float y = 0.0f;
  float z = 0.0f;
  float w = 1.0f;
};

} // namespace ascii3d

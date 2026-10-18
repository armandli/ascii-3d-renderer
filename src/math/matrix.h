#ifndef MATRIX_H
#define MATRIX_H

// Stored row-major (m[row][col]) and applied to column vectors: v' = M * v,
// so A * B * v applies B first. Translation lives in column 3 (m[0..2][3]).
// Reference (JS, row-vector) matrices must be transposed. See
// docs/CONVENTIONS.md.

#include <cmath>

#include <array>

#include <math/vector.h>

namespace ascii3d {

struct Matrix44 {
  std::array<std::array<float, 4>, 4> m{};

  [[nodiscard]] constexpr bool operator==(const Matrix44&) const noexcept =
      default;
};

[[nodiscard]] constexpr Matrix44 identity() noexcept {
  // TODO: implement.
  return Matrix44{};
}

// (a * b) * v applies b first, then a.
[[nodiscard]] constexpr Matrix44 operator*(
    [[maybe_unused]] const Matrix44& a,
    [[maybe_unused]] const Matrix44& b) noexcept
{
  // TODO: implement.
  return Matrix44{};
}

// Column-vector transform: result[r] = sum over c of m.m[r][c] * v[c].
// Use w = 1 for points (translated) and w = 0 for directions (not translated).
[[nodiscard]] constexpr Vector4 operator*(
    [[maybe_unused]] const Matrix44& m,
    [[maybe_unused]] Vector4 v) noexcept
{
  // TODO: implement.
  return Vector4{};
}

[[nodiscard]] constexpr Matrix44 transpose(
    [[maybe_unused]] const Matrix44& m) noexcept
{
  // TODO: implement.
  return Matrix44{};
}

// The factories are the transposes of the layouts in reference matrix44.ts.

// t goes in column 3: m[0][3] = t.x, m[1][3] = t.y, m[2][3] = t.z.
[[nodiscard]] constexpr Matrix44 translate(
    [[maybe_unused]] Vector3 t) noexcept
{
  // TODO: implement.
  return Matrix44{};
}

[[nodiscard]] constexpr Matrix44 scale([[maybe_unused]] Vector3 s) noexcept {
  // TODO: implement.
  return Matrix44{};
}

// Upper-left 3x3 is [[1, 0, 0], [0, c, -s], [0, s, c]], so
// rotate_x(pi/2) * y-hat == z-hat.
[[nodiscard]] inline Matrix44 rotate_x(
    [[maybe_unused]] float radians) noexcept
{
  // TODO: implement.
  return Matrix44{};
}

// Upper-left 3x3 is [[c, 0, s], [0, 1, 0], [-s, 0, c]], so
// rotate_y(pi/2) * z-hat == x-hat.
[[nodiscard]] inline Matrix44 rotate_y(
    [[maybe_unused]] float radians) noexcept
{
  // TODO: implement.
  return Matrix44{};
}

// Upper-left 3x3 is [[c, -s, 0], [s, c, 0], [0, 0, 1]], so
// rotate_z(pi/2) * x-hat == y-hat.
[[nodiscard]] inline Matrix44 rotate_z(
    [[maybe_unused]] float radians) noexcept
{
  // TODO: implement.
  return Matrix44{};
}

// Optional. Affine input only (bottom row 0, 0, 0, 1), not projections:
// the inverse of [A | t] is [A^-1 | -A^-1 * t].
[[nodiscard]] inline Matrix44 inverse(
    [[maybe_unused]] const Matrix44& m) noexcept
{
  // TODO: implement.
  return Matrix44{};
}

}  // namespace ascii3d

#endif  // MATRIX_H

#ifndef MATRIX_H
#define MATRIX_H

// 4x4 matrix stored row-major: m[row][col].
// Column-vector convention: v' = M * v, so A * B * v applies B first.
// Translation lives in column 3 (m[0..2][3]); the bottom row of an affine
// matrix is (0, 0, 0, 1). Matrices from the JS reference (row vectors) must
// be transposed before use.
// All angles are in radians. See docs/CONVENTIONS.md.

#include "math/vector.h"

#include <array>
#include <cmath>

namespace ascii3d {

struct Matrix44 {
    std::array<std::array<float, 4>, 4> m{};

    [[nodiscard]] constexpr bool operator==(const Matrix44& rhs) const noexcept = default;
};

// ---------------------------------------------------------------------------
// Basic operations
// ---------------------------------------------------------------------------

// The identity matrix: 1 on the diagonal, 0 elsewhere.
[[nodiscard]] constexpr Matrix44 identity() noexcept {
    // TODO: implement.
    return Matrix44{};
}

// Matrix product: (a * b).m[r][c] = sum over k of a.m[r][k] * b.m[k][c].
// (a * b) * v applies b first, then a.
[[nodiscard]] constexpr Matrix44 operator*([[maybe_unused]] const Matrix44& a,
                                           [[maybe_unused]] const Matrix44& b) noexcept {
    // TODO: implement.
    return Matrix44{};
}

// Transforms a column vector: result[r] = sum over c of m.m[r][c] * v[c].
// Use w = 1 for points (translated) and w = 0 for directions (not translated).
[[nodiscard]] constexpr Vector4 operator*([[maybe_unused]] const Matrix44& m,
                                          [[maybe_unused]] Vector4 v) noexcept {
    // TODO: implement.
    return Vector4{};
}

// Swaps rows and columns: result.m[r][c] = m.m[c][r].
[[nodiscard]] constexpr Matrix44 transpose([[maybe_unused]] const Matrix44& m) noexcept {
    // TODO: implement.
    return Matrix44{};
}

// ---------------------------------------------------------------------------
// Factories (transposes of the layouts in reference matrix44.ts)
// ---------------------------------------------------------------------------

// Identity with t in column 3: m[0][3] = t.x, m[1][3] = t.y, m[2][3] = t.z.
[[nodiscard]] constexpr Matrix44 translate([[maybe_unused]] Vector3 t) noexcept {
    // TODO: implement.
    return Matrix44{};
}

// Diagonal (s.x, s.y, s.z, 1).
[[nodiscard]] constexpr Matrix44 scale([[maybe_unused]] Vector3 s) noexcept {
    // TODO: implement.
    return Matrix44{};
}

// Rotation about the x axis. Expected upper-left 3x3, with c = cos, s = sin:
//   [1  0  0]
//   [0  c -s]
//   [0  s  c]
// so rotate_x(pi/2) * y-hat == z-hat.
[[nodiscard]] inline Matrix44 rotate_x([[maybe_unused]] float radians) noexcept {
    // TODO: implement.
    return Matrix44{};
}

// Rotation about the y axis. Expected upper-left 3x3:
//   [ c  0  s]
//   [ 0  1  0]
//   [-s  0  c]
// so rotate_y(pi/2) * z-hat == x-hat.
[[nodiscard]] inline Matrix44 rotate_y([[maybe_unused]] float radians) noexcept {
    // TODO: implement.
    return Matrix44{};
}

// Rotation about the z axis. Expected upper-left 3x3:
//   [c -s  0]
//   [s  c  0]
//   [0  0  1]
// so rotate_z(pi/2) * x-hat == y-hat.
[[nodiscard]] inline Matrix44 rotate_z([[maybe_unused]] float radians) noexcept {
    // TODO: implement.
    return Matrix44{};
}

// ---------------------------------------------------------------------------
// Optional
// ---------------------------------------------------------------------------

// Inverse of an affine matrix (bottom row (0, 0, 0, 1)). For M = [A | t],
// the inverse is [A^-1 | -A^-1 * t]. Used to test view_matrix().
// Not valid for projection matrices.
[[nodiscard]] inline Matrix44 inverse([[maybe_unused]] const Matrix44& m) noexcept {
    // TODO: implement.
    return Matrix44{};
}

} // namespace ascii3d

#endif  // MATRIX_H

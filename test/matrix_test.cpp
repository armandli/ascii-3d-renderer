#include <array>
#include <numbers>

#include <gtest/gtest.h>

#include <math/matrix.h>
#include <math/vector.h>
#include <test_helpers.h>

namespace ascii3d::test {

namespace {

constexpr float kHalfPi = std::numbers::pi_v<float> / 2.0f;

constexpr Matrix44 from_rows(
    std::array<float, 4> r0,
    std::array<float, 4> r1,
    std::array<float, 4> r2,
    std::array<float, 4> r3)
{
  Matrix44 out;
  out.m = {r0, r1, r2, r3};
  return out;
}

// Neither matrix is symmetric, and A * B != B * A, so swapped operands or a
// transposed product shows up as a mismatch.
const Matrix44 kA = from_rows(
    {1, 2, 3, 4},
    {5, 6, 7, 8},
    {9, 10, 11, 12},
    {13, 14, 15, 16});
const Matrix44 kB = from_rows(
    {2, 0, 1, 0},
    {0, 1, 0, 3},
    {1, 0, 0, 1},
    {0, 2, 1, 0});

// An affine matrix with every kind of factory in it, for inverse tests.
Matrix44 sample_affine() {
  return translate(Vector3{1.0f, -2.0f, 3.0f}) * rotate_y(0.7f) *
      rotate_x(-0.3f) * scale(Vector3{2.0f, 3.0f, 4.0f});
}

}  // namespace

TEST(Matrix44, IdentityLayout) {
  const Matrix44 expected = from_rows(
      {1, 0, 0, 0},
      {0, 1, 0, 0},
      {0, 0, 1, 0},
      {0, 0, 0, 1});
  EXPECT_MAT44_NEAR(identity(), expected, kEps);
}

TEST(Matrix44, IdentityIsNeutral) {
  EXPECT_MAT44_NEAR(identity() * kA, kA, kEps);
  EXPECT_MAT44_NEAR(kA * identity(), kA, kEps);
  const Vector4 v{1.0f, -2.0f, 3.0f, 1.0f};
  EXPECT_VEC4_NEAR(identity() * v, v, kEps);
}

TEST(Matrix44, ProductKnownValues) {
  const Matrix44 ab = from_rows(
      {5, 10, 5, 9},
      {17, 22, 13, 25},
      {29, 34, 21, 41},
      {41, 46, 29, 57});
  const Matrix44 ba = from_rows(
      {11, 14, 17, 20},
      {44, 48, 52, 56},
      {14, 16, 18, 20},
      {19, 22, 25, 28});
  EXPECT_MAT44_NEAR(kA * kB, ab, kEps);
  EXPECT_MAT44_NEAR(kB * kA, ba, kEps);
}

TEST(Matrix44, ProductIsAssociative) {
  const Matrix44 c = rotate_z(0.4f);
  EXPECT_MAT44_NEAR((kA * kB) * c, kA * (kB * c), 1e-3f);
}

TEST(Matrix44, TransformColumnVector) {
  // Each result component is a row of kA dotted with v (column vector), not a
  // column of kA (row vector).
  EXPECT_VEC4_NEAR(
      kA * (Vector4{1, 2, 3, 4}), (Vector4{30, 70, 110, 150}), kEps);
}

TEST(Matrix44, ProductAppliesRightOperandFirst) {
  const Vector4 v{1.0f, 2.0f, 3.0f, 1.0f};
  EXPECT_VEC4_NEAR((kA * kB) * v, kA * (kB * v), kEps);
}

TEST(Matrix44, TransposeSwapsRowsAndColumns) {
  const Matrix44 expected = from_rows(
      {1, 5, 9, 13},
      {2, 6, 10, 14},
      {3, 7, 11, 15},
      {4, 8, 12, 16});
  EXPECT_MAT44_NEAR(transpose(kA), expected, kEps);
  EXPECT_MAT44_NEAR(transpose(transpose(kA)), kA, kEps);
}

TEST(Matrix44, TransposeOfProductReversesOrder) {
  EXPECT_MAT44_NEAR(
      transpose(kA * kB), transpose(kB) * transpose(kA), kEps);
}

TEST(Matrix44, TranslateLayoutUsesColumnThree) {
  const Matrix44 expected = from_rows(
      {1, 0, 0, 4},
      {0, 1, 0, 5},
      {0, 0, 1, 6},
      {0, 0, 0, 1});
  EXPECT_MAT44_NEAR(translate(Vector3{4, 5, 6}), expected, kEps);
}

TEST(Matrix44, TranslateMovesPointsNotDirections) {
  const Matrix44 t = translate(Vector3{4, 5, 6});
  EXPECT_VEC4_NEAR(t * (Vector4{1, 2, 3, 1}), (Vector4{5, 7, 9, 1}), kEps);
  EXPECT_VEC4_NEAR(t * (Vector4{1, 2, 3, 0}), (Vector4{1, 2, 3, 0}), kEps);
}

TEST(Matrix44, ScaleLayoutAndEffect) {
  const Matrix44 expected = from_rows(
      {2, 0, 0, 0},
      {0, 3, 0, 0},
      {0, 0, 4, 0},
      {0, 0, 0, 1});
  const Matrix44 s = scale(Vector3{2, 3, 4});
  EXPECT_MAT44_NEAR(s, expected, kEps);
  EXPECT_VEC4_NEAR(s * (Vector4{1, 1, 1, 1}), (Vector4{2, 3, 4, 1}), kEps);
}

TEST(Matrix44, RotateXQuarterTurn) {
  EXPECT_VEC4_NEAR(
      rotate_x(kHalfPi) * (Vector4{0, 1, 0, 0}), (Vector4{0, 0, 1, 0}), kEps);
}

TEST(Matrix44, RotateYQuarterTurn) {
  EXPECT_VEC4_NEAR(
      rotate_y(kHalfPi) * (Vector4{0, 0, 1, 0}), (Vector4{1, 0, 0, 0}), kEps);
}

TEST(Matrix44, RotateZQuarterTurn) {
  EXPECT_VEC4_NEAR(
      rotate_z(kHalfPi) * (Vector4{1, 0, 0, 0}), (Vector4{0, 1, 0, 0}), kEps);
}

TEST(Matrix44, RotationLeavesItsAxisAndWUnchanged) {
  EXPECT_VEC4_NEAR(
      rotate_x(0.8f) * (Vector4{1, 0, 0, 1}), (Vector4{1, 0, 0, 1}), kEps);
  EXPECT_VEC4_NEAR(
      rotate_y(0.8f) * (Vector4{0, 1, 0, 1}), (Vector4{0, 1, 0, 1}), kEps);
  EXPECT_VEC4_NEAR(
      rotate_z(0.8f) * (Vector4{0, 0, 1, 1}), (Vector4{0, 0, 1, 1}), kEps);
}

TEST(Matrix44, RotationByZeroIsIdentity) {
  EXPECT_MAT44_NEAR(rotate_x(0.0f), identity(), kEps);
  EXPECT_MAT44_NEAR(rotate_y(0.0f), identity(), kEps);
  EXPECT_MAT44_NEAR(rotate_z(0.0f), identity(), kEps);
}

TEST(Matrix44, RotationPreservesLength) {
  const Vector4 v{1.0f, 2.0f, 3.0f, 0.0f};
  const Matrix44 r = rotate_z(0.3f) * rotate_y(-1.1f) * rotate_x(2.0f);
  EXPECT_NEAR(length(r * v), length(v), kEps);
}

TEST(Matrix44, RotationInverseIsTranspose) {
  const Matrix44 r = rotate_z(0.3f) * rotate_y(-1.1f) * rotate_x(2.0f);
  EXPECT_MAT44_NEAR(r * transpose(r), identity(), kEps);
  EXPECT_MAT44_NEAR(rotate_y(0.5f) * rotate_y(-0.5f), identity(), kEps);
}

TEST(Matrix44, CompositionOrderMatters) {
  const Vector3 t{10.0f, 0.0f, 0.0f};
  const Vector4 p{1.0f, 0.0f, 0.0f, 1.0f};
  // translate * rotate: rotate x-hat to y-hat about the origin, then move it.
  EXPECT_VEC4_NEAR(
      (translate(t) * rotate_z(kHalfPi)) * p, (Vector4{10, 1, 0, 1}), kEps);
  // rotate * translate: move to (11, 0, 0) first, then rotate about the
  // origin.
  EXPECT_VEC4_NEAR(
      (rotate_z(kHalfPi) * translate(t)) * p, (Vector4{0, 11, 0, 1}), 1e-4f);
}

TEST(Matrix44, InverseOfTranslateAndScale) {
  EXPECT_MAT44_NEAR(
      inverse(translate(Vector3{1, -2, 3})),
      translate(Vector3{-1, 2, -3}),
      kEps);
  EXPECT_MAT44_NEAR(
      inverse(scale(Vector3{2, 4, 0.5f})),
      scale(Vector3{0.5f, 0.25f, 2}),
      kEps);
}

TEST(Matrix44, InverseOfAffineRoundTrips) {
  const Matrix44 m = sample_affine();
  EXPECT_MAT44_NEAR(m * inverse(m), identity(), 1e-4f);
  EXPECT_MAT44_NEAR(inverse(m) * m, identity(), 1e-4f);
}

TEST(Matrix44, ConstexprFactories) {
  // Fails to compile if any of these stop being usable in constant
  // expressions; the values are checked at runtime so stubs still build.
  constexpr Matrix44 i = identity();
  constexpr Matrix44 t = translate(Vector3{1, 2, 3});
  constexpr Matrix44 s = scale(Vector3{2, 2, 2});
  constexpr Matrix44 ts = t * s;
  constexpr Matrix44 tt = transpose(t);
  constexpr Vector4 p = ts * Vector4{1, 1, 1, 1};
  EXPECT_MAT44_NEAR(i * t, t, kEps);
  EXPECT_MAT44_NEAR(transpose(tt), t, kEps);
  EXPECT_VEC4_NEAR(p, (Vector4{3, 4, 5, 1}), kEps);
}

}  // namespace ascii3d::test

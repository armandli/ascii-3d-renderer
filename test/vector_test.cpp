#include "math/vector.h"
#include "test_helpers.h"

#include <cmath>
#include <gtest/gtest.h>

namespace ascii3d::test {

// ---------------------------------------------------------------------------
// constexpr smoke (compile-time checks; uncomment once implementations land)
// ---------------------------------------------------------------------------

// static_assert(cross(Vector3{1,0,0}, Vector3{0,1,0}) == Vector3{0,0,1});
// static_assert(dot(Vector3{1,0,0}, Vector3{0,1,0}) == 0.0f);
// static_assert(length_squared(Vector3{3,4,0}) == 25.0f);
// static_assert(xyz(to_vec4(Vector3{1,2,3})) == Vector3{1,2,3});
// static_assert(to_vec4(Vector3{1,2,3}, 0.0f).w == 0.0f);

// ---------------------------------------------------------------------------
// Vector2
// ---------------------------------------------------------------------------

TEST(Vector2, Arithmetic) {
    const Vector2 a{1.0f, 2.0f};
    const Vector2 b{3.0f, 4.0f};
    EXPECT_VEC2_NEAR(a + b, (Vector2{4.0f, 6.0f}), kEps);
    EXPECT_VEC2_NEAR(a - b, (Vector2{-2.0f, -2.0f}), kEps);
    EXPECT_VEC2_NEAR(a * 2.0f, (Vector2{2.0f, 4.0f}), kEps);
    EXPECT_VEC2_NEAR(2.0f * a, (Vector2{2.0f, 4.0f}), kEps);
    EXPECT_VEC2_NEAR(a / 2.0f, (Vector2{0.5f, 1.0f}), kEps);
    EXPECT_VEC2_NEAR(-a, (Vector2{-1.0f, -2.0f}), kEps);
}

TEST(Vector2, DotProduct) {
    EXPECT_NEAR(dot(Vector2{1, 0}, Vector2{0, 1}), 0.0f, kEps);
    EXPECT_NEAR(dot(Vector2{1, 0}, Vector2{1, 0}), 1.0f, kEps);
    EXPECT_NEAR(dot(Vector2{3, 4}, Vector2{3, 4}), 25.0f, kEps);
}

TEST(Vector2, LengthAndNormalize) {
    EXPECT_NEAR(length(Vector2{3.0f, 4.0f}), 5.0f, kEps);
    EXPECT_NEAR(length_squared(Vector2{3.0f, 4.0f}), 25.0f, kEps);
    const Vector2 n = normalize(Vector2{3.0f, 4.0f});
    EXPECT_NEAR(length(n), 1.0f, kEps);
}

TEST(Vector2, NormalizeZeroVector) {
    const Vector2 result = normalize(Vector2{});
    EXPECT_FALSE(std::isnan(result.x));
    EXPECT_FALSE(std::isnan(result.y));
    EXPECT_VEC2_NEAR(result, (Vector2{}), kEps);
}

// ---------------------------------------------------------------------------
// Vector3 — cross-product basis
// ---------------------------------------------------------------------------

TEST(Vector3, CrossProductBasis) {
    // x̂ × ŷ = ẑ
    EXPECT_VEC3_NEAR(cross(Vector3{1,0,0}, Vector3{0,1,0}), (Vector3{0,0,1}), kEps);
    // ŷ × ẑ = x̂
    EXPECT_VEC3_NEAR(cross(Vector3{0,1,0}, Vector3{0,0,1}), (Vector3{1,0,0}), kEps);
    // ẑ × x̂ = ŷ
    EXPECT_VEC3_NEAR(cross(Vector3{0,0,1}, Vector3{1,0,0}), (Vector3{0,1,0}), kEps);
}

TEST(Vector3, CrossProductAnticommutative) {
    const Vector3 a{1.0f, 2.0f, 3.0f};
    const Vector3 b{4.0f, 5.0f, 6.0f};
    const Vector3 ab = cross(a, b);
    const Vector3 ba = cross(b, a);
    EXPECT_VEC3_NEAR(ab, -ba, kEps);
}

TEST(Vector3, DotCrossOrthogonality) {
    // a · (a × b) must be 0 for any a, b
    const Vector3 a{1.0f, 2.0f, 3.0f};
    const Vector3 b{4.0f, 5.0f, 6.0f};
    EXPECT_NEAR(dot(a, cross(a, b)), 0.0f, kEps);
    EXPECT_NEAR(dot(b, cross(a, b)), 0.0f, kEps);
}

TEST(Vector3, DotProduct) {
    EXPECT_NEAR(dot(Vector3{1,0,0}, Vector3{0,1,0}), 0.0f, kEps);
    EXPECT_NEAR(dot(Vector3{1,0,0}, Vector3{1,0,0}), 1.0f, kEps);
}

TEST(Vector3, LengthAndNormalize) {
    EXPECT_NEAR(length(Vector3{0, 3.0f, 4.0f}), 5.0f, kEps);
    EXPECT_NEAR(length_squared(Vector3{1, 2, 2}), 9.0f, kEps);
    const Vector3 n = normalize(Vector3{1.0f, 2.0f, 3.0f});
    EXPECT_NEAR(length(n), 1.0f, kEps);
}

TEST(Vector3, NormalizeZeroVector) {
    const Vector3 result = normalize(Vector3{});
    EXPECT_FALSE(std::isnan(result.x));
    EXPECT_FALSE(std::isnan(result.y));
    EXPECT_FALSE(std::isnan(result.z));
    EXPECT_VEC3_NEAR(result, (Vector3{}), kEps);
}

TEST(Vector3, Arithmetic) {
    const Vector3 a{1, 2, 3};
    const Vector3 b{4, 5, 6};
    EXPECT_VEC3_NEAR(a + b, (Vector3{5, 7, 9}), kEps);
    EXPECT_VEC3_NEAR(a - b, (Vector3{-3, -3, -3}), kEps);
    EXPECT_VEC3_NEAR(a * 2.0f, (Vector3{2, 4, 6}), kEps);
    EXPECT_VEC3_NEAR(2.0f * a, (Vector3{2, 4, 6}), kEps);
    EXPECT_VEC3_NEAR(-a, (Vector3{-1, -2, -3}), kEps);
}

// ---------------------------------------------------------------------------
// Vector4
// ---------------------------------------------------------------------------

TEST(Vector4, Arithmetic) {
    const Vector4 a{1, 2, 3, 4};
    const Vector4 b{5, 6, 7, 8};
    EXPECT_VEC4_NEAR(a + b, (Vector4{6, 8, 10, 12}), kEps);
    EXPECT_VEC4_NEAR(a - b, (Vector4{-4, -4, -4, -4}), kEps);
    EXPECT_VEC4_NEAR(a * 2.0f, (Vector4{2, 4, 6, 8}), kEps);
    EXPECT_VEC4_NEAR(2.0f * a, (Vector4{2, 4, 6, 8}), kEps);
    EXPECT_VEC4_NEAR(-a, (Vector4{-1, -2, -3, -4}), kEps);
}

TEST(Vector4, NormalizeZeroVector) {
    const Vector4 zero{0, 0, 0, 0};
    const Vector4 result = normalize(zero);
    EXPECT_FALSE(std::isnan(result.x));
    EXPECT_FALSE(std::isnan(result.w));
}

// ---------------------------------------------------------------------------
// Conversion helpers
// ---------------------------------------------------------------------------

TEST(VectorConversion, ToVec4DefaultW) {
    const Vector3 v{1.0f, 2.0f, 3.0f};
    const Vector4 v4 = to_vec4(v);
    EXPECT_NEAR(v4.x, 1.0f, kEps);
    EXPECT_NEAR(v4.y, 2.0f, kEps);
    EXPECT_NEAR(v4.z, 3.0f, kEps);
    EXPECT_NEAR(v4.w, 1.0f, kEps);
}

TEST(VectorConversion, ToVec4DirectionVector) {
    const Vector3 v{1.0f, 2.0f, 3.0f};
    EXPECT_NEAR(to_vec4(v, 0.0f).w, 0.0f, kEps);
}

TEST(VectorConversion, XyzRoundTrip) {
    const Vector3 v{1.0f, 2.0f, 3.0f};
    EXPECT_VEC3_NEAR(xyz(to_vec4(v)), v, kEps);
}

} // namespace ascii3d::test

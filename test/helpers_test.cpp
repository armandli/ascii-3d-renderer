#include "test_helpers.h"

#include <filesystem>

#include <gtest/gtest-spi.h>
#include <gtest/gtest.h>

namespace ascii3d::test {

namespace {

constexpr const char* kRoundtripGolden = "helpers_roundtrip";
constexpr const char* kRoundtripContent = ".;ox%@\n@%xo;.\n";

bool file_exists(const std::string& path) {
    return std::filesystem::exists(path);
}

} // namespace

TEST(TestHelpers, EpsValue) {
    static_assert(kEps == 1e-5f, "kEps must be 1e-5");
    EXPECT_FLOAT_EQ(kEps, 1e-5f);
}

TEST(TestHelpers, VecNearAcceptsWithinTolerance) {
    EXPECT_VEC2_NEAR((Vector2{1.0f, 2.0f}), (Vector2{1.0f + 0.5f * kEps, 2.0f}), kEps);
    EXPECT_VEC3_NEAR((Vector3{1.0f, 2.0f, 3.0f}), (Vector3{1.0f, 2.0f - 0.5f * kEps, 3.0f}), kEps);
    EXPECT_VEC4_NEAR((Vector4{1.0f, 2.0f, 3.0f, 4.0f}),
                     (Vector4{1.0f, 2.0f, 3.0f, 4.0f + 0.5f * kEps}), kEps);
}

TEST(TestHelpers, VecNearRejectsOutsideTolerance) {
    EXPECT_NONFATAL_FAILURE(
        EXPECT_VEC3_NEAR((Vector3{1.0f, 2.0f, 3.0f}), (Vector3{1.0f, 2.0f, 3.1f}), kEps), "");
}

TEST(TestHelpers, Mat44NearComparesEveryElement) {
    Matrix44 a{};
    Matrix44 b{};
    EXPECT_MAT44_NEAR(a, b, kEps);

    b.m[2][3] = 1.0f;
    EXPECT_NONFATAL_FAILURE(EXPECT_MAT44_NEAR(a, b, kEps), "at row 2, col 3");
}

TEST(TestHelpers, GoldenMatchPasses) {
    if (golden_update_requested()) {
        GTEST_SKIP() << "Golden helper tests do not run in update mode.";
    }
    expect_golden(kRoundtripGolden, kRoundtripContent);
    EXPECT_FALSE(file_exists(golden_actual_path(kRoundtripGolden)));
}

TEST(TestHelpers, GoldenMismatchWritesActual) {
    if (golden_update_requested()) {
        GTEST_SKIP() << "Golden helper tests do not run in update mode.";
    }
    const std::string actual_path = golden_actual_path(kRoundtripGolden);
    EXPECT_NONFATAL_FAILURE(expect_golden(kRoundtripGolden, "different\n"), "Golden mismatch");
    EXPECT_TRUE(file_exists(actual_path));
    std::filesystem::remove(actual_path);
}

TEST(TestHelpers, GoldenMissingWritesActualNotGolden) {
    if (golden_update_requested()) {
        GTEST_SKIP() << "Golden helper tests do not run in update mode.";
    }
    const std::string name = "helpers_missing_golden";
    EXPECT_NONFATAL_FAILURE(expect_golden(name, "anything\n"), "Golden file not found");
    EXPECT_FALSE(file_exists(golden_path(name)));
    EXPECT_TRUE(file_exists(golden_actual_path(name)));
    std::filesystem::remove(golden_actual_path(name));
}

} // namespace ascii3d::test

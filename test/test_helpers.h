#pragma once

#include "math/matrix.h"
#include "math/vector.h"

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>

#include <gtest/gtest.h>

namespace ascii3d::test {

inline constexpr float kEps = 1e-5f;

// Compare actual against a stored golden file in test/golden/<name>.txt.
// Set ASCII3D_UPDATE_GOLDEN=1 to regenerate the golden file.
inline void expect_golden(const std::string& name, const std::string& actual) {
    const std::string golden_dir = GOLDEN_DIR;
    const std::string golden_path = golden_dir + "/" + name + ".txt";

    const char* update_env = std::getenv("ASCII3D_UPDATE_GOLDEN");
    const bool update = (update_env != nullptr && update_env[0] == '1');

    if (update) {
        std::ofstream out(golden_path);
        out << actual;
        SUCCEED() << "Updated golden: " << golden_path;
        return;
    }

    std::ifstream in(golden_path);
    if (!in.is_open()) {
        std::ofstream out(golden_path);
        out << actual;
        FAIL() << "Golden file not found; written for the first time: " << golden_path
               << "\nRe-run with ASCII3D_UPDATE_GOLDEN=1 to accept it as the baseline.";
        return;
    }

    std::ostringstream ss;
    ss << in.rdbuf();
    const std::string expected = ss.str();

    if (actual != expected) {
        std::ofstream out(golden_dir + "/" + name + ".actual.txt");
        out << actual;
    }
    EXPECT_EQ(actual, expected) << "Golden mismatch for '" << name
                                << "'. Diff against: " << name << ".actual.txt";
}

} // namespace ascii3d::test

// Per-component near-equality checks.  Wrap in do/while so the macro is safe
// inside an if-body that lacks braces.

#define EXPECT_VEC2_NEAR(a, b, eps)      \
    do {                                  \
        EXPECT_NEAR((a).x, (b).x, (eps)); \
        EXPECT_NEAR((a).y, (b).y, (eps)); \
    } while (false)

#define EXPECT_VEC3_NEAR(a, b, eps)      \
    do {                                  \
        EXPECT_NEAR((a).x, (b).x, (eps)); \
        EXPECT_NEAR((a).y, (b).y, (eps)); \
        EXPECT_NEAR((a).z, (b).z, (eps)); \
    } while (false)

#define EXPECT_VEC4_NEAR(a, b, eps)      \
    do {                                  \
        EXPECT_NEAR((a).x, (b).x, (eps)); \
        EXPECT_NEAR((a).y, (b).y, (eps)); \
        EXPECT_NEAR((a).z, (b).z, (eps)); \
        EXPECT_NEAR((a).w, (b).w, (eps)); \
    } while (false)

#define EXPECT_MAT44_NEAR(a, b, eps)                                              \
    do {                                                                           \
        for (int _r = 0; _r < 4; ++_r)                                            \
            for (int _c = 0; _c < 4; ++_c)                                        \
                EXPECT_NEAR((a).m[_r][_c], (b).m[_r][_c], (eps))                 \
                    << "  at row " << _r << ", col " << _c;                        \
    } while (false)

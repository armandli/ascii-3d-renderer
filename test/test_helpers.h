#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include "math/matrix.h"
#include "math/vector.h"

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>

#include <gtest/gtest.h>

namespace ascii3d::test {

inline constexpr float kEps = 1e-5f;

inline std::string golden_path(const std::string& name) {
    return std::string(GOLDEN_DIR) + "/" + name + ".txt";
}

inline std::string golden_actual_path(const std::string& name) {
    return std::string(GOLDEN_DIR) + "/" + name + ".actual.txt";
}

inline bool golden_update_requested() {
    const char* update_env = std::getenv("ASCII3D_UPDATE_GOLDEN");
    return update_env != nullptr && update_env[0] == '1';
}

inline void write_text_file(const std::string& path, const std::string& content) {
    std::ofstream out(path, std::ios::binary);
    out << content;
}

// Compare actual against a stored golden file in test/golden/<name>.txt.
// On mismatch or a missing golden, writes <name>.actual.txt next to it for
// diffing and fails; the golden itself is never written implicitly.
// Set ASCII3D_UPDATE_GOLDEN=1 to (re)generate the golden file.
inline void expect_golden(const std::string& name, const std::string& actual) {
    const std::string expected_path = golden_path(name);
    const std::string actual_path = golden_actual_path(name);

    if (golden_update_requested()) {
        write_text_file(expected_path, actual);
        std::remove(actual_path.c_str());
        SUCCEED() << "Updated golden: " << expected_path;
        return;
    }

    std::ifstream in(expected_path, std::ios::binary);
    if (!in.is_open()) {
        write_text_file(actual_path, actual);
        ADD_FAILURE() << "Golden file not found: " << expected_path
                      << "\nActual output written to: " << actual_path
                      << "\nRe-run with ASCII3D_UPDATE_GOLDEN=1 to accept it as the baseline.";
        return;
    }

    std::ostringstream ss;
    ss << in.rdbuf();
    const std::string expected = ss.str();

    if (actual == expected) {
        std::remove(actual_path.c_str());
        return;
    }
    write_text_file(actual_path, actual);
    EXPECT_EQ(actual, expected) << "Golden mismatch for '" << name
                                << "'. Diff against: " << actual_path;
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

#define EXPECT_MAT44_NEAR(a, b, eps)                             \
    do {                                                         \
        for (std::size_t _r = 0; _r < 4; ++_r)                   \
            for (std::size_t _c = 0; _c < 4; ++_c)               \
                EXPECT_NEAR((a).m[_r][_c], (b).m[_r][_c], (eps)) \
                    << "  at row " << _r << ", col " << _c;      \
    } while (false)

#endif  // TEST_HELPERS_H

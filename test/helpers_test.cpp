#include "test_helpers.h"

#include <gtest/gtest.h>

namespace ascii3d::test {

TEST(TestHelpers, EpsValue) {
    static_assert(kEps == 1e-5f, "kEps must be 1e-5");
    EXPECT_FLOAT_EQ(kEps, 1e-5f);
}

} // namespace ascii3d::test

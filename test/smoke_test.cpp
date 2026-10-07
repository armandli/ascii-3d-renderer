#include "render/renderer.h"
#include "render/shader.h"
#include "scene/object.h"

#include <gtest/gtest.h>

TEST(Smoke, StubsCompileAndLink) {
  ascii3d::Object object;
  ascii3d::Shader shader;
  EXPECT_EQ(object.scale.x, 1.0f);
  EXPECT_FALSE(shader.ramp.empty());
}

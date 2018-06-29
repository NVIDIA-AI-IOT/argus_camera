#include <memory>

#include "gtest/gtest.h"

#include "../src/ArgusCamera.h"

TEST(TestGTest, Hello) {
  ASSERT_STREQ("Hello", "Hello");
}

TEST(ArgusCamera, NonNullValidConfig) {
  auto config = ArgusCameraConfig::DEFAULT_DEVKIT_CONFIG();

  std::unique_ptr<ArgusCamera> camera;
  camera.reset(ArgusCamera::createArgusCamera(config));  

  ASSERT_NE(nullptr, camera.get());
}

TEST(ArgusCamera, Info0ValidConfig) {
  auto config = ArgusCameraConfig::DEFAULT_DEVKIT_CONFIG();

  int info;
  std::unique_ptr<ArgusCamera> camera;
  camera.reset(ArgusCamera::createArgusCamera(config, &info));  

  ASSERT_EQ(0, info);
}

int main(int argc, char *argv[])
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

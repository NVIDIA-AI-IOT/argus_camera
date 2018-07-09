#include <memory>

#include "gtest/gtest.h"

#include "../src/ArgusCamera.hpp"

TEST(TestGTest, Hello) {
  ASSERT_STREQ("Hello", "Hello");
}

TEST(ArgusCamera, NonNullValidConfig) {
  auto config = ArgusCameraConfig::createDefaultDevkitConfig();

  std::unique_ptr<IArgusCamera> camera;
  camera.reset(IArgusCamera::createArgusCamera(config));  

  ASSERT_NE(nullptr, camera.get());
}

TEST(ArgusCamera, Info0ValidConfig) {
  auto config = ArgusCameraConfig::createDefaultDevkitConfig();

  int info;
  std::unique_ptr<IArgusCamera> camera;
  camera.reset(IArgusCamera::createArgusCamera(config, &info));  

  ASSERT_EQ(0, info);
}

TEST(ArgusCamera, ReadReturnsSuccessStatus) {
  auto config = ArgusCameraConfig::createDefaultDevkitConfig();
  int info;
  std::unique_ptr<IArgusCamera> camera;
  camera.reset(IArgusCamera::createArgusCamera(config, &info));

  uint8_t data[640 * 480 * 4];
  int status = camera->read(data);

  ASSERT_EQ(0, status);
}

TEST(ArgusCamera, ReadSetsAlpha255) {
  auto config = ArgusCameraConfig::createDefaultDevkitConfig();
  int info;
  std::unique_ptr<IArgusCamera> camera;
  camera.reset(IArgusCamera::createArgusCamera(config, &info));

  uint8_t data[640 * 480 * 4];
  int status = camera->read(data);

  for (int i = 0; i < 640 * 480; i++) {
    ASSERT_EQ(255, data[i * 4 + 3]);
  }
}

// may fail if all white image
TEST(ArgusCamera, ReadSetsSomeNon255) {
  auto config = ArgusCameraConfig::createDefaultDevkitConfig();
  int info;
  std::unique_ptr<IArgusCamera> camera;
  camera.reset(IArgusCamera::createArgusCamera(config, &info));

  uint8_t data[640 * 480 * 4];
  int status = camera->read(data);

  bool all_255 = true;
  for (int i = 0; i < 640 * 480 * 4; i++) {
    if(255 != data[i]) {
      all_255 = false;
    }
  }

  ASSERT_FALSE(all_255);
}

int main(int argc, char *argv[])
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

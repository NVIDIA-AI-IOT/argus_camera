#include "gtest/gtest.h"

#include "../src/ArgusCamera.h"

TEST(TestGTest, Hello) {
  ASSERT_STREQ("Hello", "Hello");
}

int main(int argc, char *argv[])
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

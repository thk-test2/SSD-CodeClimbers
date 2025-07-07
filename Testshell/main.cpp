#include "gmock/gmock.h"
#include "test_shell.cpp"

TEST(SampleTest, HandlesTrue) {
  EXPECT_TRUE(true);
}

int main() {
#ifdef _DEBUG
  ::testing::InitGoogleTest();
  return RUN_ALL_TESTS();
#else
  TestShell testShell;
  testShell.run();
  return 0;
#endif
}

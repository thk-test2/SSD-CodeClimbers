#include "gmock/gmock.h"
#include "ssd_driver.cpp"

#if defined(_DEBUG)
int main() {
  testing::InitGoogleMock();
  return RUN_ALL_TESTS();
}
#else
int main(int argc, char *argv[]) {
  SSDDriver ssdDriver;
  ssdDriver.run(argc, argv);
  return 0;
}
#endif

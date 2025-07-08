#include "SSD_EXE.cpp"
#include "gmock/gmock.h"

//class MockSSD_EXE : public SSD_EXE {
//public:
//  MOCK_METHOD(void, read, (int lba), ());
//  MOCK_METHOD(void, write, (int lba, unsigned long value), ());
//  MOCK_METHOD(std::string, getResult, (), ());
//};

TEST(SSDEXE, readNormal) {
  using testing::internal::CaptureStdout;
  using testing::internal::GetCapturedStdout;
  SSD_EXE ssd;

  CaptureStdout();
  ssd.read(0);
  std::string output = GetCapturedStdout();
  EXPECT_TRUE(output.find("ssd.exe\" R 0") != std::string::npos);
}

TEST(SSDEXE, writeNormal) {
  using testing::internal::CaptureStdout;
  using testing::internal::GetCapturedStdout;
  SSD_EXE ssd;

  CaptureStdout();
  ssd.write(0, 0xAAAABBBB);
  std::string output = GetCapturedStdout();
  EXPECT_TRUE(output.find("ssd.exe\" W 0xAAAABBBB") != std::string::npos);
}

#include "SSD_EXE.cpp"
#include "gmock/gmock.h"
using testing::internal::CaptureStdout;
using testing::internal::GetCapturedStdout;

class SSDEXEFixture : public ::testing::Test {
public:
  SSD_EXE ssd;
};

TEST_F(SSDEXEFixture, readNormal) {
  CaptureStdout();
  ssd.read(0);
  std::string output = GetCapturedStdout();
  EXPECT_TRUE(output.find("ssd.exe\" R 0") != std::string::npos)
	  << "Actual stream : " << output;
}

TEST_F(SSDEXEFixture, writeNormal) {
  CaptureStdout();
  ssd.write(0, 0xAAAABBBB);
  std::string output = GetCapturedStdout();
  EXPECT_TRUE(output.find("ssd.exe\" W 0 0xAAAABBBB") != std::string::npos)
      << "Actual stream : " << output;
}

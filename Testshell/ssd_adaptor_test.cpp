#include "ssd_adaptor.h"
#include "gmock/gmock.h"
using testing::internal::CaptureStdout;
using testing::internal::GetCapturedStdout;

class SsdExeAdaptorFixture : public ::testing::Test {
public:
  SsdExeAdaptor ssd;
};

TEST_F(SsdExeAdaptorFixture, readNormal) {
  CaptureStdout();
  ssd.read(0);
  std::string output = GetCapturedStdout();
  EXPECT_TRUE(output.find("ssd.exe\" R 0") != std::string::npos)
	  << "Actual stream : " << output;
}

TEST_F(SsdExeAdaptorFixture, writeNormal) {
  CaptureStdout();
  ssd.write(0, 0xAAAABBBB);
  std::string output = GetCapturedStdout();
  EXPECT_TRUE(output.find("ssd.exe\" W 0 0xAAAABBBB") != std::string::npos)
      << "Actual stream : " << output;
}

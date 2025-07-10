#include "ssd_adaptor.h"
#include "gmock/gmock.h"
using testing::internal::CaptureStdout;
using testing::internal::GetCapturedStdout;

class SsdAdaptorFixture : public ::testing::Test {
public:
  SsdAdaptor ssd;
};

TEST_F(SsdAdaptorFixture, readNormal) {
  CaptureStdout();
  ssd.read(0);
  std::string output = GetCapturedStdout();
  EXPECT_TRUE(output.find("ssd.exe\" R 0") != std::string::npos)
	  << "Actual stream : " << output;
}

TEST_F(SsdAdaptorFixture, writeNormal) {
  CaptureStdout();
  ssd.write(0, 0xAAAABBBB);
  std::string output = GetCapturedStdout();
  EXPECT_TRUE(output.find("ssd.exe\" W 0 0xAAAABBBB") != std::string::npos)
      << "Actual stream : " << output;
}

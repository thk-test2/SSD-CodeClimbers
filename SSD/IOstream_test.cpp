#include "fileIOstream.h"
#include "gmock/gmock.h"

using namespace testing;
using std::getline;

class IOstreamTestFixture : public Test {
public:
  IoStream ioStream;
};

TEST_F(IOstreamTestFixture, IOStreamReadPass) {
  unsigned int value = ioStream.readStream(2);
  EXPECT_EQ(value, 0x0);
}

TEST_F(IOstreamTestFixture, IOStreamReadFail) {
  unsigned int value = ioStream.readStream(1);
  EXPECT_NE(value, 0x1);
}

TEST_F(IOstreamTestFixture, IOStreamWritePass) {
  ioStream.writeStream(1, 0xAAAAFFFF);

  unsigned int value = ioStream.readStream(1);
  EXPECT_EQ(value, 0xAAAAFFFF);
}

TEST_F(IOstreamTestFixture, IOStreamWriteFail) {
  ioStream.writeStream(1, 0xAAAAFFFF);

  unsigned int value = ioStream.readStream(1);
  EXPECT_NE(value, 0x0);
}

TEST_F(IOstreamTestFixture, IOStreamErrorMassageCheckAfterReadLBARangeFail) {
  ioStream.readStream(100);

  EXPECT_EQ("ERROR", ioStream.readFileAsString(ioStream.output_file_name));
}

TEST_F(IOstreamTestFixture, IOStreamErrorMassageCheckAfterWriteLBARangeFail) {
  ioStream.writeStream(100,0xAAAAAAAA);

  EXPECT_EQ("ERROR", ioStream.readFileAsString(ioStream.output_file_name));
}

TEST_F(IOstreamTestFixture, IOStreamCheckSsdNandFileInitValid) {
  ioStream.initSsdNand();
  std::istringstream iss(ioStream.readFileAsString(ioStream.nand_file_name));
  string getLine = "";
  string compareLine = "";

  for (int i = 0; i < MAX_NAND_MEMORY_MAP_SIZE; i++) {
    getline(iss, getLine);
    compareLine = std::to_string(i) + " 0x00000000";
    EXPECT_EQ(getLine, compareLine);
  }
}
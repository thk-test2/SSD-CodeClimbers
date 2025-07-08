#include "file_iostream.h"
#include "gmock/gmock.h"

using namespace testing;
using std::getline;

class IOstreamTestFixture : public Test {
protected:
  void SetUp() override { 
      ioStream = new IoStream(MAX_NAND_MEMORY_MAP_SIZE, testBuf);
  }
  static const int MAX_NAND_MEMORY_MAP_SIZE = 100;
  IoStream *ioStream;
  unsigned long testBuf[MAX_NAND_MEMORY_MAP_SIZE];
};

TEST_F(IOstreamTestFixture, WriteErrorTest) {
  string output = "";
  ioStream->writeError();

  istringstream iss(ioStream->readFileAsString(ioStream->output_file_name));
  getline(iss, output);
  EXPECT_EQ("ERROR", output);
}

TEST_F(IOstreamTestFixture, ClearOutputTest) {
  string output = "";
  ioStream->clearOutput();

  istringstream iss(ioStream->readFileAsString(ioStream->output_file_name));
  getline(iss, output);
  EXPECT_EQ("", output);
}

TEST_F(IOstreamTestFixture, IOStreamCheckSsdNandFileInitValid) {
  ioStream->initSsdNand();
  istringstream iss(ioStream->readFileAsString(ioStream->nand_file_name));
  string getLine = "";
  string compareLine = "";

  for (int i = 0; i < ioStream->getStorageSize(); i++) {
    getline(iss, getLine);
    compareLine = std::to_string(i) + " 0x00000000";
    EXPECT_EQ(getLine, compareLine);
  }
}
#include "ssd_cmd_buffer_control.h"
#include "gmock/gmock.h"
using std::cout;
using namespace testing;

class BufctrlFlushFixture : public Test {
public:
  CmdBufferControl &cmdBuffer = CmdBufferControl::getInstance();
  SSDDriver *driver = cmdBuffer.getDriver();
  IoStream *stream = driver->getIoStream();

  void SetUp() override { 
      cmdBuffer.clearAllBuffer(); 
      stream->initSsdNand();
      stream->clearOutput();
  }

  void TearDown() override {
    cmdBuffer.clearAllBuffer();
    stream->initSsdNand();
    stream->clearOutput();
  }

  void checkOutputFileValid(const string &expected_value) {
    EXPECT_EQ(expected_value, stream->readFileAsString(
                                  stream->output_file_name));
  }
  void checkSpecificLineInNandFile(const string &expected_value,
                                   int targetLine) {
    EXPECT_GE(targetLine, 0);
    EXPECT_LT(targetLine, stream->getStorageSize());

    std::istringstream iss(stream->readFileAsString(stream->nand_file_name));
    string getLine = "";
    string compareLine = std::to_string(targetLine) + " " + expected_value;

    for (int i = 0; i < stream->getStorageSize(); i++) {
      std::getline(iss, getLine);
      if (i == targetLine) {
        EXPECT_EQ(getLine, compareLine);
        break;
      }
    }
  }
};

TEST_F(BufctrlFlushFixture, FlushWrtiePass) {
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_1_0x11112222"));

  cmdBuffer.flush();
  checkOutputFileValid("");

  EXPECT_TRUE(driver->read(1));
  checkOutputFileValid("0x11112222");
}

TEST_F(BufctrlFlushFixture, Flush2WrtiePass) {
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_1_0x11111111"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("empty"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("empty"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("empty"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("empty"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("empty"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("empty"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_2_0x22222222"));

  cmdBuffer.flush();

  checkSpecificLineInNandFile("0x11111111", 1);
  checkSpecificLineInNandFile("0x22222222", 2);
  checkSpecificLineInNandFile("0x00000000", 3);
  checkSpecificLineInNandFile("0x00000000", 4);
  checkSpecificLineInNandFile("0x00000000", 5);
}

TEST_F(BufctrlFlushFixture, Flush5WrtiePass) {
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_1_0x11111111"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_2_0x22222222"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_3_0x33333333"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_4_0x44444444"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_5_0x55555555"));

  cmdBuffer.flush();
  checkOutputFileValid("");

  checkSpecificLineInNandFile("0x11111111", 1);
  checkSpecificLineInNandFile("0x33333333", 3);
  checkSpecificLineInNandFile("0x22222222", 2);
  checkSpecificLineInNandFile("0x55555555", 5);
  checkSpecificLineInNandFile("0x44444444", 4);
}

TEST_F(BufctrlFlushFixture, Flush4Wrtie1ErasePass) {
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_1_0x11111111"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_2_0x22222222"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_3_0x33333333"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_5_0x55555555"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("E_2_3"));

  cmdBuffer.flush();

  checkSpecificLineInNandFile("0x11111111", 1);
  checkSpecificLineInNandFile("0x00000000", 2);
  checkSpecificLineInNandFile("0x00000000", 3);
  checkSpecificLineInNandFile("0x00000000", 4);
  checkSpecificLineInNandFile("0x55555555", 5);
}

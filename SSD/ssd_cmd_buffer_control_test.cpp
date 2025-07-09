#include "ssd_cmd_buffer_control.h"
#include "gmock/gmock.h"
using std::cout;

using namespace testing;

class BufferControlFixture : public Test {
public:
  void SetUp() override { cmdBuffer.clearAllBuffer(); }
  CmdBufferControl &cmdBuffer = CmdBufferControl::getInstance();
  const std::string BUFFER_EMPTY = "1_empty,2_empty,3_empty,4_empty,5_empty,";

  void checkOutputFileValid(const string &expected_value) {
    EXPECT_EQ(expected_value,
              cmdBuffer.getDriver()->getIoStream()->readFileAsString(
                  cmdBuffer.getDriver()->getIoStream()->output_file_name));
  }
};

TEST_F(BufferControlFixture, BufferCheckAfterInit) {
  std::string expected_ret = BUFFER_EMPTY;
  EXPECT_EQ(expected_ret, cmdBuffer.getBufferNameList());
}

TEST_F(BufferControlFixture, BufferUpdateByIndexTest) {
  std::string expected_ret =
      "1_empty,2_W_20_0xABCDABCD,3_empty,4_empty,5_empty,";
  cmdBuffer.updateBufferByIndex(2, "W_20_0xABCDABCD");
  EXPECT_EQ(expected_ret, cmdBuffer.getBufferNameList());
}

TEST_F(BufferControlFixture, BufferUpdateByIndex2Test) {
  EXPECT_TRUE(cmdBuffer.updateBufferByIndex(2, "W_20_0xABCDABCD"));
  EXPECT_TRUE(cmdBuffer.updateBufferByIndex(2, "R_10"));
}

TEST_F(BufferControlFixture, BufferUpdateByIndexTestFail) {
  EXPECT_FALSE(cmdBuffer.updateBufferByIndex(0, "W_20_0xABCDABCD"));
  EXPECT_FALSE(cmdBuffer.updateBufferByIndex(6, "W_20_0xABCDABCD"));
  EXPECT_FALSE(cmdBuffer.updateBufferByIndex(100, "W_20_0xABCDABCD"));
}

TEST_F(BufferControlFixture, BufferClearTest) {
  std::string expected_ret = BUFFER_EMPTY;
  cmdBuffer.updateBufferByIndex(2, "W_20_0xABCDABCD");
  cmdBuffer.clearBufferByIndex(2);
  EXPECT_EQ(expected_ret, cmdBuffer.getBufferNameList());
}

TEST_F(BufferControlFixture, BufferAllClearTest) {
  std::string expected_ret = BUFFER_EMPTY;
  cmdBuffer.updateBufferByIndex(1, "R_20");
  cmdBuffer.updateBufferByIndex(2, "W_20_0xABCDABCD");
  cmdBuffer.updateBufferByIndex(3, "W_1_0xFFFFFFFF");
  cmdBuffer.clearAllBuffer();
  EXPECT_EQ(expected_ret, cmdBuffer.getBufferNameList());
}

TEST_F(BufferControlFixture, BufferUpdateToNextEmptyTest) {
  std::string expected_ret = "1_R_20,2_W_20_0xABCDABCD,3_W_1_0x11112222,4_W_10_"
                             "0xABCDABCD,5_W_99_0xBBBBAAAA,";
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("R_20"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_20_0xABCDABCD"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_1_0x11112222"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_10_0xABCDABCD"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_99_0xBBBBAAAA"));
  EXPECT_FALSE(cmdBuffer.updateToNextEmpty("W_99_0xBBBBAAAA"));
  EXPECT_EQ(expected_ret, cmdBuffer.getBufferNameList());
}

TEST_F(BufferControlFixture, BufferIsFullTest) {
  std::string expected_ret = "1_R_20,2_W_20_0xABCDABCD,3_W_1_0x11112222,4_W_10_"
                             "0xABCDABCD,5_W_99_0xBBBBAAAA,";
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("R_20"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_20_0xABCDABCD"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_1_0x11112222"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_10_0xABCDABCD"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_99_0xBBBBAAAA"));
  EXPECT_FALSE(cmdBuffer.updateToNextEmpty("W_99_0xBBBBAAAA"));

  EXPECT_TRUE(cmdBuffer.isBufferFull());
  EXPECT_EQ(expected_ret, cmdBuffer.getBufferNameList());
}

TEST_F(BufferControlFixture, BufferIsNotFullTest) {
  std::string expected_ret =
      "1_R_20,2_W_20_0xABCDABCD,3_W_1_0x11112222,4_empty,5_empty,";
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("R_20"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_20_0xABCDABCD"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_1_0x11112222"));

  EXPECT_FALSE(cmdBuffer.isBufferFull());
  EXPECT_EQ(expected_ret, cmdBuffer.getBufferNameList());
}

TEST_F(BufferControlFixture, BufferEmptyShiftTest) {

  std::string expected_ret =
      "1_E_10_10,2_W_1_0x11112222,3_W_99_0xBBBBAAAA,4_empty,5_empty,";

  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("E_10_10"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_20_0xABCDABCD"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_1_0x11112222"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_10_0xABCDABCD"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_99_0xBBBBAAAA"));

  cmdBuffer.clearBufferByIndex(2);
  cmdBuffer.clearBufferByIndex(4);
  cmdBuffer.emptyBufferShift();

  EXPECT_EQ(expected_ret, cmdBuffer.getBufferNameList());
}

TEST_F(BufferControlFixture, BufferGetParsingTest) {
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("E_10_10"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_20_0xABCDABCD"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_1_0x11112222"));

  EXPECT_TRUE(cmdBuffer.getBufferCmd(1), "E");
  EXPECT_TRUE(cmdBuffer.getBufferCmd(2), "W");
  EXPECT_TRUE(cmdBuffer.getBufferCmd(3), "W");

  EXPECT_TRUE(cmdBuffer.getBufferLba(1), 10);
  EXPECT_TRUE(cmdBuffer.getBufferLba(2), 20);
  EXPECT_TRUE(cmdBuffer.getBufferLba(3), 1);

  EXPECT_TRUE(cmdBuffer.getBufferLbaSize(1), 10);
  EXPECT_TRUE(cmdBuffer.getBufferValue(2), 0xABCDABCD);
  EXPECT_TRUE(cmdBuffer.getBufferValue(3), 0x11112222);
}

TEST_F(BufferControlFixture, BufferGetParsingInvalidIndexTest) {
  EXPECT_THROW(cmdBuffer.getBufferCmd(0), CmdBufferInvalidIdexException);
  EXPECT_THROW(cmdBuffer.getBufferCmd(-1), CmdBufferInvalidIdexException);
  EXPECT_THROW(cmdBuffer.getBufferCmd(10), CmdBufferInvalidIdexException);
}

TEST_F(BufferControlFixture, BufferRunCommand) {
  char *argv[5];
  argv[0] = const_cast<char *>("ssd.exe");
  argv[1] = const_cast<char *>("W");
  argv[2] = const_cast<char *>("10");
  argv[3] = const_cast<char *>("0x12345678");
  argv[4] = nullptr;
  cmdBuffer.runCommandBuffer(argv);

  argv[0] = const_cast<char *>("ssd.exe");
  argv[1] = const_cast<char *>("R");
  argv[2] = const_cast<char *>("10");
  argv[3] = nullptr;
  cmdBuffer.runCommandBuffer(argv);

  checkOutputFileValid("0x12345678");
}
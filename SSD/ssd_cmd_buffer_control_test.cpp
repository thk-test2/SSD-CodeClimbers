#include "ssd_cmd_buffer_control.h"
#include "gmock/gmock.h"
using std::cout;

using namespace testing;

class BufferControlFixture : public Test {
public:
  void SetUp() override { cmdBuffer.clearAllBuffer(); }
  CmdBufferControl &cmdBuffer = CmdBufferControl::getInstance();
  const std::string BUFFER_EMPTY = "1_empty,2_empty,3_empty,4_empty,5_empty,";
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

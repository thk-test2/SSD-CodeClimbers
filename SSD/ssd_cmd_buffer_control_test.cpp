#include "ssd_cmd_buffer_control.h"
#include "gmock/gmock.h"
using std::cout;

using namespace testing;

class BufferControlFixture : public Test {
public:
  CmdBufferControl &cmdBuffer = CmdBufferControl::getInstance    ();

  const std::string BUFFER_EMPTY = "1_empty,2_empty,3_empty,4_empty,5_empty,";
};

TEST_F(BufferControlFixture, BufferCheckAfterInit) {
  std::string expected_ret = BUFFER_EMPTY;

  EXPECT_EQ(expected_ret, cmdBuffer.getBufferNameList());
}

TEST_F(BufferControlFixture, BufferUpdateByIndexTest) {
  std::string expected_ret = "1_empty,2_W_20_0xABCDABCD,3_empty,4_empty,5_empty,";
  cmdBuffer.updateBufferByIndex(2, "2_W_20_0xABCDABCD");
  EXPECT_EQ(expected_ret, cmdBuffer.getBufferNameList());
}

TEST_F(BufferControlFixture, BufferUpdateByIndex2Test) {
  EXPECT_TRUE(cmdBuffer.updateBufferByIndex(2, "2_W_20_0xABCDABCD"));
  EXPECT_TRUE(cmdBuffer.updateBufferByIndex(2, "2_R_10"));
}

TEST_F(BufferControlFixture, BufferClearTest) {
  std::string expected_ret = BUFFER_EMPTY;
  cmdBuffer.updateBufferByIndex(2, "2_W_20_0xABCDABCD");
  cmdBuffer.clearBufferByIndex(2);
  EXPECT_EQ(expected_ret, cmdBuffer.getBufferNameList());
}

TEST_F(BufferControlFixture, BufferUpd) {
  auto &bc = CmdBufferControl::getInstance();
  std::string expected_ret = BUFFER_EMPTY;
  bc.updateBufferByIndex(2, "2_W_20_0xABCDABCD");
  bc.clearBufferByIndex(2);
  EXPECT_EQ(expected_ret, cmdBuffer.getBufferNameList());
}
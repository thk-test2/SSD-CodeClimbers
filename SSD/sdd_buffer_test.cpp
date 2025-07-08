#include "ssd_buffer_control.h"
#include "gmock/gmock.h"
using std::cout;

using namespace testing;

class BufferControlFixture : public Test {
public:
  BufferControl &bc = BufferControl::getInstance();

  const std::string BUFFER_EMPTY = "1_empty,2_empty,3_empty,4_empty,5_empty,";
};

TEST_F(BufferControlFixture, BufferCheckAfterInit) {
  auto &bc = BufferControl::getInstance();
  std::string expected_ret = BUFFER_EMPTY;

  EXPECT_EQ(expected_ret, bc.getBufferNameList());
}

TEST_F(BufferControlFixture, BufferUpdateByIndexTest) {
  auto &bc = BufferControl::getInstance();
  std::string expected_ret = "1_empty,2_W_20_0xABCDABCD,3_empty,4_empty,5_empty,";
  bc.updateBufferByIndex(2, "2_W_20_0xABCDABCD");
  EXPECT_EQ(expected_ret, bc.getBufferNameList());
}

TEST_F(BufferControlFixture, BufferUpdateByIndex2Test) {
  auto &bc = BufferControl::getInstance();
  EXPECT_TRUE(bc.updateBufferByIndex(2, "2_W_20_0xABCDABCD"));
  EXPECT_TRUE(bc.updateBufferByIndex(2, "2_R_10"));
}

TEST_F(BufferControlFixture, BufferClearTest) {
  auto &bc = BufferControl::getInstance();
  std::string expected_ret = BUFFER_EMPTY;
  bc.updateBufferByIndex(2, "2_W_20_0xABCDABCD");
  bc.clearBufferByIndex(2);
  EXPECT_EQ(expected_ret, bc.getBufferNameList());
}

TEST_F(BufferControlFixture, BufferUpd) {
  auto &bc = BufferControl::getInstance();
  std::string expected_ret = BUFFER_EMPTY;
  bc.updateBufferByIndex(2, "2_W_20_0xABCDABCD");
  bc.clearBufferByIndex(2);
  EXPECT_EQ(expected_ret, bc.getBufferNameList());
}
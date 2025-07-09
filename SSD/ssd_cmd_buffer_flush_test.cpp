#include "gmock/gmock.h"
#include "ssd_cmd_buffer_control.h"
#include "ssd_driver.cpp"
using std::cout;
using namespace testing;

class BufctrlFlushFixture : public Test {
public:
  CmdBufferControl &cmdBuffer = CmdBufferControl::getInstance();
  SSDDriver driver;
  std::vector<Cmd> result;

  void parse(const std::string &input) {
    result = cmdBuffer.parseCommands(input);
  }
};

TEST_F(BufctrlFlushFixture, ParsesSize) {
  parse("1_empty,2_W_42_0xDEADBEEF,3_empty,");
  ASSERT_EQ(result.size(), 5);
}

TEST_F(BufctrlFlushFixture, ParsesWrite) {
  parse("1_empty,2_W_42_0xDEADBEEF,3_empty,");
  EXPECT_EQ(result[0].cmdType, std::string("empty"));
}

TEST_F(BufctrlFlushFixture, ThrowsInvalid) {
  EXPECT_THROW(parse("1_W_42,"), std::invalid_argument); 
}

TEST_F(BufctrlFlushFixture, IgnoresUnknownCmdType) {
  parse("1_UNKNOWN_123_ABC,");
  EXPECT_TRUE(result.empty());
}

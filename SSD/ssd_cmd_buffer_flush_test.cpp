#include "gmock/gmock.h"
#include "ssd_cmd_buffer_control.h"

using std::cout;

using namespace testing;

class BufctrlFlushFixture : public Test {
public:
  CmdBufferControl &cmdBuffer = CmdBufferControl::getInstance();

  const std::string BUFFER_EMPTY = "1_empty,2_empty,3_empty,4_empty,5_empty,";
};

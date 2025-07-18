#include "gmock/gmock.h"
#include "ssd_cmd_buffer_control.h"

#if defined(_DEBUG)
int main() {
  testing::InitGoogleMock();
  return RUN_ALL_TESTS();
}
#else
int main(int argc, char *argv[]) {
  CmdBufferControl &cmdBuffer = CmdBufferControl::getInstance();
   
  if (!cmdBuffer.runCommandBuffer(argc, argv))
    cout << "SSD Opreation Failed" << std::endl;

  return 0;
}
#endif

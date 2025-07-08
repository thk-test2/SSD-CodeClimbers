#include "ssd_cmd_buffer_control.h"
#include <iostream>

void CmdBufferControl::flush() {
  for (auto &cmdBuffer : cmdBuffer) {
    cmdBuffer.clear();
  }
}

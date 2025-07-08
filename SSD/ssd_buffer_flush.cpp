#include "ssd_buffer_control.h"
#include <iostream>

void BufferControl::flush() {
  for (auto &buf : buffers) {
    buf.clear();
  }
}

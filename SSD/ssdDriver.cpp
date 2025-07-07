#include "device.h"

class SSDDriver : public Device{
public:
  bool read(int lba) override {
    if (lba < 0 || lba > 100)
      return false;
    else
      return true;
  }
  bool write(int lba, unsigned int value) override {
    if (lba < 0 || lba > 100)
      return false;
    else
      return true;
  }
};
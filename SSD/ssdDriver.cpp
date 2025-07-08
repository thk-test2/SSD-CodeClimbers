#include "device.h"
#include <vector>
#include <string>
#include <iostream>

using std::vector;
using std::string;

class SSDDriver : public Device{
public:
    void run(int argc, char* argv[]) {

    }

    vector<string> parseParams(int argc, char* argv[]) {
        vector<string> params;
        for (int i = 1; i < argc; ++i) {
            params.push_back(argv[i]);
        }
        return params;
    }

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
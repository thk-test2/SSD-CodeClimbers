#include "device.h"
#include <iostream>
#include <string>
#include <vector>

using std::string;
using std::vector;

class SSDDriver : public Device {
public:
  void run(int argc, char *argv[]) {
    if (argc < 3)
      std::exception();
    if (argv[0] != "ssd.exe")
      std::exception();

    vector<string> params = parseParams(argc, argv);

    string command = params[1];
    int lba = std::stoi(params[2]);
    int value = std::stoul(params[3]);

    if (command == "W") {
    } else if (command == "R") {
    } else {
      std::exception();
    }
  }

  vector<string> parseParams(int argc, char *argv[]) {
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
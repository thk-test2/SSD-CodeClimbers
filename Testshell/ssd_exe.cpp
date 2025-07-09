#include <fstream>
#include <iostream>
#include <sstream>
#include <direct.h>

#include "ssd_interface.h"

using std::string;

class SSD_EXE : public SSD_INTERFACE {
public:
  SSD_EXE() : ssdDir(getCurWorkingDir()) {}

  void read(int lba) override {
    std::ostringstream cmd;
    cmd << "\"" << ssdDir << "\\" << SSD_EXE_NAME << "\" R " << lba;
#ifdef _DEBUG
    std::cout << cmd.str() << "\n";
#else
    int ret = system(cmd.str().c_str());
    if (ret != 0) {
      lastResult = ERROR_MSG;
    } else {
      lastResult = readOutputFile();
    }
#endif
  }

  void write(int lba, unsigned long value) override {
    std::ostringstream cmd;
    cmd << "\"" << ssdDir << "\\" << SSD_EXE_NAME << "\" W " << lba << " 0x"
        << std::hex << std::uppercase << value;
#ifdef _DEBUG
    std::cout << cmd.str() << "\n";
#else
    int ret = system(cmd.str().c_str());

    if (ret != 0) {
      lastResult = ERROR_MSG;
    } else {
      string out = readOutputFile();
      lastResult = out.empty() ? "" : ERROR_MSG;
    }
#endif
  }

    void erase(int lba, int size) override {
    std::ostringstream cmd;
      cmd << "\"" << ssdDir << "\\" << SSD_EXE_NAME << "\" E " << lba
          << " " << size;
#ifdef _DEBUG
    std::cout << cmd.str() << "\n";
#else
    int ret = system(cmd.str().c_str());

    if (ret != 0) {
      lastResult = ERROR_MSG;
    } else {
      string out = readOutputFile();
      lastResult = out.empty() ? "" : ERROR_MSG;
    }
#endif
  }

  string getResult() override { return lastResult; }

private:
  string ssdDir;
  string lastResult;
  const string SSD_EXE_NAME = "ssd.exe";
  const string SSD_OUTPUT_FILE = "ssd_output.txt";
  const string ERROR_MSG = "ERROR";

  string getCurWorkingDir() {
    char cwd[1024];
    if (!_getcwd(cwd, sizeof(cwd))) {
      std::cerr << "[ERROR] 현재 경로를 얻을 수 없습니다." << std::endl;
      std::exit(1);
    }

    return string(cwd);
  }

  string readOutputFile() {
    std::ifstream infile(ssdDir + "\\" + SSD_OUTPUT_FILE);
    if (!infile.is_open()) {
      return ERROR_MSG;
    }

    std::string line;
    if (std::getline(infile, line)) {
      return line;
    }

    return "";
  }
};

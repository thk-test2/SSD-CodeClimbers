#include <fstream>
#include <iostream>
#include <sstream>
#include <direct.h>

using std::string;

class SSD_INTERFACE {
public:
  virtual void read(int lba) = 0;
  virtual void write(int lba, unsigned long value) = 0;
  virtual string getResult() = 0;
};

class SSD_EXE : public SSD_INTERFACE {
public:
  SSD_EXE() : ssdDir(getSSDExePath()) {}

  void read(int lba) override {
    std::ostringstream cmd;
    cmd << "\"" << ssdDir << "\\ssd.exe\" R " << lba;
    int ret = system(cmd.str().c_str());

    if (ret != 0) {
      lastResult = "ERROR";
    } else {
      lastResult = readOutputFile();
    }
  }

  void write(int lba, unsigned long value) override {
    std::ostringstream cmd;
    cmd << "\"" << ssdDir << "\\ssd.exe\" W " << lba << " 0x" << std::hex
        << value;
    int ret = system(cmd.str().c_str());

    if (ret != 0) {
      lastResult = "ERROR";
    } else {
      string out = readOutputFile();
      lastResult = out.empty() ? "" : "ERROR";
    }
  }

  string getResult() override { return lastResult; }

private:
  string ssdDir;
  string lastResult;

  // 현재 디렉토리 기준 ../SSD/x64/Release 를 절대경로로 변환
  string getSSDExePath() {
    char cwd[1024];
    if (!_getcwd(cwd, sizeof(cwd))) {
      std::cerr << "[ERROR] 현재 경로를 얻을 수 없습니다." << std::endl;
      std::exit(1);
    }

    std::string relPath = std::string(cwd) + "\\..\\SSD\\x64\\Release";
    char absPath[1024];
    if (!_fullpath(absPath, relPath.c_str(), sizeof(absPath))) {
      std::cerr << "[ERROR] 절대 경로 변환 실패." << std::endl;
      std::exit(1);
    }

    return std::string(absPath);
  }

  string readOutputFile() {
    std::ifstream infile(ssdDir + "\\ssd_output.txt");
    if (!infile.is_open()) {
      return "ERROR";
    }

    std::string line;
    if (std::getline(infile, line)) {
      return line;
    }

    return "";
  }
};
#include "ssd_exe.h"

using std::string;

void SSD_EXE::read(int lba) {
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

void SSD_EXE::write(int lba, unsigned long value) {
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

void SSD_EXE::erase(int lba, int size) {
  std::ostringstream cmd;
  cmd << "\"" << ssdDir << "\\" << SSD_EXE_NAME << "\" E " << lba << " "
      << size;
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

string SSD_EXE::getCurWorkingDir() {
  char cwd[1024];
  if (!_getcwd(cwd, sizeof(cwd))) {
    std::cerr << "[ERROR] 현재 경로를 얻을 수 없습니다." << std::endl;
    std::exit(1);
  }

  return string(cwd);
}

string SSD_EXE::readOutputFile() {
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

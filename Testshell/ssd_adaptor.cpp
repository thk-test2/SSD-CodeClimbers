#include "ssd_adaptor.h"

using std::string;

void SsdAdaptor::read(int lba) {
  logger.print("SsdAdaptor.read()", "Reading LBA: " + std::to_string(lba));
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

void SsdAdaptor::write(int lba, unsigned long value) {
  logger.print("SsdAdaptor.write()", "Writing LBA: " + std::to_string(lba) +
                   " with value: " + std::to_string(value));
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

void SsdAdaptor::erase(int lba, int size) {
  logger.print("SsdAdaptor.erase()", "Erasing LBA: " + std::to_string(lba) +
                                      " with size: " + std::to_string(size));
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


void SsdAdaptor::flush() {
  logger.print("SsdAdaptor.flush()", "Flushing SSD commands");
  std::ostringstream cmd;
  cmd << "\"" << ssdDir << "\\" << SSD_EXE_NAME << "\" F";
#ifdef _DEBUG
  std::cout << cmd.str() << "\n";
#else
  int ret = system(cmd.str().c_str());

  lastResult = ret != 0 ? ERROR_MSG : "";
#endif
}

string SsdAdaptor::getCurWorkingDir() {
  char cwd[1024];
  if (!_getcwd(cwd, sizeof(cwd))) {
    std::cerr << "[ERROR] 현재 경로를 얻을 수 없습니다." << std::endl;
    std::exit(1);
  }

  return string(cwd);
}

string SsdAdaptor::readOutputFile() {
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

#pragma once
#include <fstream>
#include <iostream>
#include <sstream>
#include <direct.h>

#include "ssd_interface.h"
#include "logger.h"

using std::string;

class SsdExeAdaptor : public SSD_INTERFACE {
public:
  SsdExeAdaptor() : ssdDir(getCurWorkingDir()) {}

  void read(int lba) override;
  void write(int lba, unsigned long value) override;
  void erase(int lba, int size) override;
  void flush() override;
  string getResult() override { return lastResult; }

private:
  string ssdDir;
  string lastResult;
  const string SSD_EXE_NAME = "ssd.exe";
  const string SSD_OUTPUT_FILE = "ssd_output.txt";
  const string ERROR_MSG = "ERROR";
  Logger &logger = Logger::getInstance();

  string getCurWorkingDir();
  string readOutputFile();
};

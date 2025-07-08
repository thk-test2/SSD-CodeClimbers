#pragma once
#include <string>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <iostream>
using std::string;
using std::cout;

const int MAX_NAND_MEMORY_MAP_SIZE = 100;

class NandMemoryMap {
public:
  unsigned int value[MAX_NAND_MEMORY_MAP_SIZE] = {
      0,
  };
};

class IoStream {
public:
  IoStream() {
    nand_file_name = "ssd_nand.txt";
    output_file_name = "ssd_output.txt";
    initSsdNand();
  };
  string readFileAsString(const string &filename);
  int writeStream(int lba, int value);
  unsigned int readStream(int lba);

  bool isValid_LBA_Range(int lba);

  void loadNandFile();
  void initSsdNand();

  NandMemoryMap nandMemoryMap;
  string nand_file_name;
  string output_file_name;

private:
  bool isValid_nand_file = true;
  bool isValid_output_file = true;
};
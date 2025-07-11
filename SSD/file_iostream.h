#pragma once
#include <string>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <vector>

using std::string;
using std::vector;
using std::string;
using std::ofstream;
using std::istringstream;
using std::ifstream;
using std::cout;

class IoStream {
public:
  IoStream(const int size, unsigned long* buf) {
    nand_file_name = "ssd_nand.txt";
    output_file_name = "ssd_output.txt";
    storageSize = size;
    buffer = buf;

    ifstream ifs(nand_file_name);
    if (!ifs)
      initSsdNand();

    clearOutput();
  };
  string readFileAsString(const string &filename);
  void writeError();
  void clearOutput();
  void WirteOuputFile(unsigned long value);
  void writeBufToNandFile(unsigned long *buf);
 
  void loadNandFiletoBuf();
  void initSsdNand();

  int getStorageSize();
  ofstream getOutputWriteStream();
  ofstream getNandWriteStream();
  istringstream getOutputReadStream();
  istringstream getNandReadStream();

  string nand_file_name;
  string output_file_name;

private:
  int storageSize = 0;
  unsigned long *buffer;
};

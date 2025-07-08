#pragma once
#include <string>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <iostream>

using namespace std;
using std::string;
using std::cout;

class IoStream {
public:
  IoStream(const int size, unsigned long* buf) {
    nand_file_name = "ssd_nand.txt";
    output_file_name = "ssd_output.txt";
    storageSize = size;
    buffer = buf;
    initSsdNand();
  };
  string readFileAsString(const string &filename);
  void writeError();
  void clearOutput();
 
  void loadNandFile();
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
  unsigned long *buffer ;
  bool isValid_nand_file = true;
  bool isValid_output_file = true;
};
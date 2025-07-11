#include "file_iostream.h"

void IoStream::writeError() {
  ofstream ofs(output_file_name);
  ofs << "ERROR";
}

void IoStream::clearOutput() {
  ofstream ofs(output_file_name, std::ios::trunc); // 빈 파일로 만듦
}

string IoStream::readFileAsString(const string &filename) {
  std::ifstream ifs(filename);
  if (!ifs) {
    std::cerr << "[ERROR] FileOpenFail: " << filename << "\n";
    return "";
  }

  std::ostringstream oss;
  oss << ifs.rdbuf();
  return oss.str();
}

void IoStream::initSsdNand() {
  ofstream ofs(nand_file_name);
  if (!ofs) {
    std::cerr << "file create fail : " << nand_file_name << std::endl;
    return;
  }

  for (int i = 0; i < storageSize; ++i) {
    ofs << std::dec << i << " 0x" << std::setfill('0') << std::setw(8)
        << std::hex << std::uppercase
        << 0
        << "\n";
  }
}

void IoStream::loadNandFiletoBuf() {
  ifstream ifs(nand_file_name);

  if (!ifs) {
    return;
  }

  int idx = 0;
  string hexVal;

  while (ifs >> idx >> hexVal) {

    if (idx < 0 || idx >= storageSize) {
      continue;
    }

    try {
      buffer[idx] = stoul(hexVal, nullptr, 16);
    } catch (const std::exception &e) {
      buffer[idx] = 0;
    }
  }
}

void IoStream::writeOutputFile(unsigned long value) {
  ofstream ofs(output_file_name);
  ofs << "0x" << std::setfill('0') << std::setw(8) << std::hex << std::uppercase
      << value;
}

void IoStream::writeBufToNandFile(unsigned long *buf) {
  ofstream ofs(nand_file_name);
  for (int i = 0; i < storageSize; ++i) {
    ofs << std::dec << i << " 0x" << std::setfill('0') << std::setw(8)
        << std::hex << std::uppercase << buf[i] << "\n";
  }
}

int IoStream::getStorageSize() { return storageSize; }

ofstream IoStream::getOutputWriteStream() {
  ofstream ofs(output_file_name);
  return ofs;
}

ofstream IoStream::getNandWriteStream() {
  ofstream ofs(nand_file_name);
  return ofs;
}

istringstream IoStream::getOutputReadStream() {
  istringstream iss(readFileAsString(output_file_name));
  return iss;
}

istringstream IoStream::getNandReadStream() {
  istringstream iss(readFileAsString(nand_file_name));
  return iss;
}
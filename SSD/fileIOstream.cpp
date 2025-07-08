#include "fileIOStream.h"

void IoStream::writeError() {
  ofstream ofs(output_file_name);
  ofs << "ERROR";
}

void IoStream::clearOutput() {
  ofstream ofs(output_file_name, ios::trunc); // 빈 파일로 만듦
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
    cerr << "file create fail : " << nand_file_name << endl;
    return;
  }

  for (int i = 0; i < storageSize; ++i) {
    ofs << dec << i << " 0x" << setfill('0') << setw(8) << hex << uppercase << 0
        << "\n";
  }
}

void IoStream::loadNandFile() {
  ifstream ifs(nand_file_name);

  if (!ifs) {
    isValid_nand_file = false;
    cout << "[DEBUG] nand_file_is_invalid: " << nand_file_name << "\n";
    return;
  }

  int idx = 0;
  string hexVal;

  while (ifs >> idx >> hexVal) {

    if (idx < 0 || idx >= storageSize) {
      cout << "[DEBUG] Skipped invalid idx=" << idx << "\n";
      continue;
    }

    try {
      buffer[idx] = stoul(hexVal, nullptr, 16);
    } catch (const std::exception &e) {
      cout << "[DEBUG] Error parsing hexVal at idx=" << idx << ": " << e.what()
           << "\n";
      buffer[idx] = 0;
    }
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
  istringstream iss(output_file_name);
  return iss;
}

istringstream IoStream::getNandReadStream() {
  istringstream iss(nand_file_name);
  return iss;
}
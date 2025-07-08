#include "fileIOStream.h"

using namespace std;
using std::cout;

namespace {
// 내부 함수
void writeError(const string &output_file) {
  ofstream ofs(output_file);
  ofs << "ERROR";
}

void clearOutput(const string &output_file) {
  ofstream ofs(output_file, ios::trunc); // 빈 파일로 만듦
}
} // namespace

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

  for (int i = 0; i < MAX_NAND_MEMORY_MAP_SIZE; ++i) {
    ofs << dec << i << " 0x" << setfill('0') << setw(8) << hex << uppercase << 0
        << "\n";
  }

  cout << "[INFO] " << nand_file_name << MAX_NAND_MEMORY_MAP_SIZE
       << " Init Success\n";
}

void IoStream::loadNandFile() {
  ifstream ifs(nand_file_name);

  if (!ifs) {
    isValid_nand_file = false;
    cout << "[DEBUG] nand_file_is_invalid: " << nand_file_name << "\n";
  } else {
    cout << "[DEBUG] nand_file_is_valid: " << nand_file_name << "\n";
  }

  int idx = 0;
  string hexVal;

  if (isValid_nand_file) {
    //cout << "[DEBUG] Start reading file contents...\n";
    while (ifs >> idx >> hexVal) {
      // cout << "[DEBUG] Read line: idx=" << idx << ", hexVal=" << hexVal <<
      // "\n";

      if (idx < 0 || idx >= MAX_NAND_MEMORY_MAP_SIZE) {
        cout << "[DEBUG] Skipped invalid idx=" << idx << "\n";
        continue;
      }

      try {
        nandMemoryMap.value[idx] = stoul(hexVal, nullptr, 16);
      } catch (const std::exception &e) {
        cout << "[DEBUG] Error parsing hexVal at idx=" << idx << ": "
             << e.what() << "\n";
        nandMemoryMap.value[idx] = 0;
      }
    }
    //cout << "[DEBUG] Finished reading file.\n";
  } else {
    for (int i = 0; i < MAX_NAND_MEMORY_MAP_SIZE; ++i) {
      nandMemoryMap.value[i] = 0;
    }
  }
}

unsigned int IoStream::readStream(int lba) {
  loadNandFile();

  if (!isValid_LBA_Range(lba)) {
    writeError(output_file_name);
    return -1;
  }

  ofstream ofs(output_file_name);

  ofs << "0x" << setfill('0') << setw(8) << hex << uppercase
      << nandMemoryMap.value[lba];

  return nandMemoryMap.value[lba];
}

int IoStream::writeStream(int lba, int value) {
  loadNandFile();

  if (!isValid_LBA_Range(lba)) {
    writeError(output_file_name);
    return -1;
  }

  nandMemoryMap.value[lba] = value;

  ofstream ofs(nand_file_name);
  for (int i = 0; i < MAX_NAND_MEMORY_MAP_SIZE; ++i) {
    ofs << dec << i << " 0x" << setfill('0') << setw(8) << hex << uppercase
        << nandMemoryMap.value[i] << "\n";
  }

  clearOutput(output_file_name);

  return 0;
}

bool IoStream::isValid_LBA_Range(int lba) {
  return (lba >= 0 && lba < MAX_NAND_MEMORY_MAP_SIZE);
}

#include "ssd_driver.h"
#include "file_iostream.h"

class SSDDriver : public Device {
public:
  SSDDriver() { stream = new IoStream(MAX_NAND_MEMORY_MAP_SIZE, buf); }

  bool isValid_LBA(int lba) {
    return (lba >= 0 && lba < MAX_NAND_MEMORY_MAP_SIZE);
  }
  bool isSizeAllowed(int size) {
    return size >= MIN_ERASE_SIZE && size <= MAX_ERASE_SIZE;
  }
  bool isWithinBounds(int lba, int size) {
    return (lba + size) <= MAX_NAND_MEMORY_MAP_SIZE;
  }

  bool isValidRWCondition(int lba) {
    if (!isValid_LBA(lba)) {
      stream->writeError();
      return false;
    } else
      return true;
  }

  bool isValidEraseCondition(int lba, int size) {
    if (!isValid_LBA(lba) || !isSizeAllowed(size) || !isWithinBounds(lba, size)){
      stream->writeError();
      return false;
    } else
      return true;
  }

  bool isValidParam(int argc, char *argv[], int &lba, int &size, unsigned long& value) {
    string command = argv[1];
    if (command == "W" && argc == 4) {
      lba = std::stoi(argv[2]);
      value = std::stoul(argv[3], nullptr, HEX_BASE);
      return isValidRWCondition(lba);
    } else if (command == "R" && argc == 3) {
      lba = std::stoi(argv[2]);
      return isValidRWCondition(lba);
    } else if (command == "E" && argc == 4) {
      lba = std::stoi(argv[2]);
      size = std::stoi(argv[3]);
      return isValidEraseCondition(lba,size);
    } else if (command == "F" && argc == 2)
      return true;
    else
      return false;
  }

  bool read(int lba) override {
    stream->loadNandFiletoBuf();

    ofstream ofs = stream->getOutputWriteStream();
    ofs << "0x" << std::setfill('0') << std::setw(8) << std::hex
        << std::uppercase << buf[lba];

    return true;
  }

  void writeBufToNandFile() {
    ofstream ofs = stream->getNandWriteStream();
    for (int i = 0; i < MAX_NAND_MEMORY_MAP_SIZE; ++i) {
      ofs << std::dec << i << " 0x" << std::setfill('0') << std::setw(8)
          << std::hex << std::uppercase << buf[i] << "\n";
    }
  }

  bool write(int lba, unsigned long value) override {
    stream->loadNandFiletoBuf();

    buf[lba] = value;

    writeBufToNandFile();

    stream->clearOutput();

    return true;
  }

  bool erase(int lba, int size) override {
    stream->loadNandFiletoBuf();

    for (int i = lba; i < lba + size; i++)
      buf[i] = 0;

    writeBufToNandFile();

    stream->clearOutput();

    return true;
  }

  IoStream *getIoStream() { return stream; }
  int getMaxNandSize() { return MAX_NAND_MEMORY_MAP_SIZE;  }
  int getMaxEraseSize() { return MAX_ERASE_SIZE; }


private:
  static const int HEX_BASE = 16;
  static const int MAX_NAND_MEMORY_MAP_SIZE = 100;
  static const int MIN_ERASE_SIZE = 1;
  static const int MAX_ERASE_SIZE = 10;
  unsigned long buf[MAX_NAND_MEMORY_MAP_SIZE] = {
      0,
  };
  IoStream *stream;
};

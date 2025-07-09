#include "ssd_driver.h"
#include "file_iostream.h"

class SSDDriver : public Device {
public:
  SSDDriver() { stream = new IoStream(MAX_NAND_MEMORY_MAP_SIZE, buf); }
  void run(int argc, char *argv[]) {
    vector<string> params = parseParams(argc, argv);

    string command = params[0];
    int lba = std::stoi(params[1]);

    bool result = false;
    if (command == "W" && argc == 4) {
      unsigned long value = std::stoul(params[2], nullptr, HEX_BASE);
      result = write(lba, value);
    } else if (command == "R" && argc == 3) {
      result = read(lba);
    } else if (command == "E" && argc == 4) {
      int size = std::stoi(params[2]);
      result = erase(lba, size);
    } else
      throw std::runtime_error("Need to check parameter.");

    if (!result)
      throw std::runtime_error("Device operation failed.");
  }

  vector<string> parseParams(int argc, char *argv[]) {
    vector<string> params;
    for (int i = 1; i < argc; ++i) {
      params.push_back(argv[i]);
    }
    return params;
  }

  bool isValid_LBA_Range(int lba) {
    if (lba >= 0 && lba < MAX_NAND_MEMORY_MAP_SIZE)
      return true;
    else
      return false;
  }
  bool read(int lba) override {
    if (!isValid_LBA_Range(lba)) {
      stream->writeError();
      return false;
    }

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
    if (!isValid_LBA_Range(lba)) {
      stream->writeError();
      return false;
    }

    stream->loadNandFiletoBuf();

    buf[lba] = value;

    writeBufToNandFile();

    stream->clearOutput();

    return true;
  }

  bool erase(int lba, int size) override {
    if (size < 1 || size > 10)
      return false;

    if (lba + size > 100)
      return false;

    if (!isValid_LBA_Range(lba)) {
      stream->writeError();
      return false;
    }

    stream->loadNandFiletoBuf();

    for (int i = lba; i < lba + size; i++)
      buf[i] = 0;

    writeBufToNandFile();

    return true;
  }

  IoStream *getIoStream() { return stream; }

private:
  static const int HEX_BASE = 16;
  static const int MAX_NAND_MEMORY_MAP_SIZE = 100;
  unsigned long buf[MAX_NAND_MEMORY_MAP_SIZE] = {
      0,
  };
  IoStream *stream;
};
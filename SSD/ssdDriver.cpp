#include "device.h"
#include "fileIOStream.h"

using std::ofstream;

class SSDDriver : public Device{
public:
  SSDDriver() { 
      stream = new IoStream(MAX_NAND_MEMORY_MAP_SIZE, buf);
      stream->initSsdNand();
  }
  bool isValid_LBA_Range(int lba) {
    if (lba >= 0 && lba < MAX_NAND_MEMORY_MAP_SIZE)
      return true;
    else
      return false;
  }
  bool read(int lba) override {
    stream->loadNandFile();

    if (!isValid_LBA_Range(lba)) {
      stream->writeError();
      return false;
    }

    ofstream ofs = stream->getOutputWriteStream();

    ofs << "0x" << setfill('0') << setw(8) << hex << uppercase << buf[lba];

    return true;
  }
  bool write(int lba, unsigned long value) override {
    stream->loadNandFile();

    if (!isValid_LBA_Range(lba)) {
      stream->writeError();
      return false;
    }

    buf[lba] = value;

    ofstream ofs = stream->getNandWriteStream();
    for (int i = 0; i < MAX_NAND_MEMORY_MAP_SIZE; ++i) {
      ofs << dec << i << " 0x" << setfill('0') << setw(8) << hex << uppercase
          << buf[i] << "\n";
    }

    stream->clearOutput();
    return true;
  }

  IoStream *getIoStream() { return stream; }

private:  
  static const int MAX_NAND_MEMORY_MAP_SIZE = 100;
  unsigned long buf[MAX_NAND_MEMORY_MAP_SIZE] = {
      0,
  };
  IoStream *stream;
};
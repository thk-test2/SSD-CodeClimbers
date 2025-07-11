#pragma once
#include "file_iostream.h"

class Device {
public:
  virtual bool write(int idx, unsigned long value) = 0;
  virtual bool read(int idx) = 0;
  virtual bool erase(int idx, int size) = 0;
};

class SSDDriver : public Device {
public:
  SSDDriver() { stream = new IoStream(MAX_NAND_MEMORY_MAP_SIZE, buf); }

  bool isValid_LBA(int lba);
  bool isSizeAllowed(int size);
  bool isWithinBounds(int lba, int size);
  bool isValidRWCondition(int lba);
  bool isValidEraseCondition(int lba, int size);
  bool isValidParam(int argc, char *argv[], int &lba, int &size,
                    unsigned long &value);
  bool read(int lba) override;
  bool readBuffer(int lba, unsigned long value);
  bool write(int lba, unsigned long value) override;
  bool erase(int lba, int size) override;

  IoStream *getIoStream() { return stream; }
  int getMaxNandSize() { return MAX_NAND_MEMORY_MAP_SIZE; }
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

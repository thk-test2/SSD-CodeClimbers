#include "ssd_driver.h"

bool SSDDriver::isValid_LBA(int lba) {
  return (lba >= 0 && lba < MAX_NAND_MEMORY_MAP_SIZE);
}

bool SSDDriver::isSizeAllowed(int size) {
  return size >= MIN_ERASE_SIZE && size <= MAX_ERASE_SIZE;
}

bool SSDDriver::isWithinBounds(int lba, int size) {
  return (lba + size) <= MAX_NAND_MEMORY_MAP_SIZE;
}

bool SSDDriver::isValidRWCondition(int lba) {
  if (!isValid_LBA(lba)) {
    getIoStream()->writeError();
    return false;
  } else
    return true;
}

bool SSDDriver::isValidEraseCondition(int lba, int size) {
  if (!isValid_LBA(lba) || !isSizeAllowed(size) || !isWithinBounds(lba, size)) {
    getIoStream()->writeError();
    return false;
  } else
    return true;
}

bool SSDDriver::isValidParam(int argc, char *argv[], int &lba, int &size,
                                    unsigned long &value) {
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
    return isValidEraseCondition(lba, size);
  } else if (command == "F" && argc == 2)
    return true;
  else
    return false;
}

bool SSDDriver::read(int lba) {
  getIoStream()->loadNandFiletoBuf();
  getIoStream()->writeOutputFile(buf[lba]);

  return true;
}

bool SSDDriver::readBuffer(int lba, unsigned long value) {
  getIoStream()->writeOutputFile(value);
  return true;
}

bool SSDDriver::write(int lba, unsigned long value) {
  getIoStream()->loadNandFiletoBuf();

  buf[lba] = value;

  getIoStream()->writeBufToNandFile(buf);
  getIoStream()->clearOutput();

  return true;
}

bool SSDDriver::erase(int lba, int size) {
  getIoStream()->loadNandFiletoBuf();

  for (int i = lba; i < lba + size; i++)
    buf[i] = 0;

  getIoStream()->writeBufToNandFile(buf);
  getIoStream()->clearOutput();

  return true;
}

#pragma once
#include "ssd_cmd_buffer.h"
#include "ssd_driver.h"
#include <string>
#include <vector>
#include <stdexcept>

class CmdBufferControl {
public:
  SSDDriver *getDriver();
  static CmdBufferControl &getInstance();
  std::string getBufferNameList() const;
  bool runCommandBuffer(int argc, char *argv[]);
  bool updateToNextEmpty(const std::string &cmd);
  bool updateBufferByIndex(int index, const std::string &cmd);
  std::string setBufferName(int index, const std::string &cmd);
  bool isValidBufferIndex(int index);
  bool clearBufferByIndex(int index);
  void clearAllBuffer(void);
  void clearEraseMap();
  void setEraseMap(int lba, int size, int setVal);
  bool flushEraseSeparated(int lba, int size);
  bool flush();
  bool isBufferFull() const;
  bool emptyBufferShift();
  char getBufferCmd(int index);
  int getBufferLba(int index);
  unsigned long getBufferValue(int index);
  int getBufferLbaSize(int index);
  bool isSameLbaBuffer(int lba, CmdBuffer &buffer);
  bool isValidRangeForErase(int lba, CmdBuffer &buffer);

  bool isBufferContainReadValue(int lba, unsigned long &value);
  void removeAndUpdateWriteCommand(int lba, char *argv[]);
  void mergeAndUpdateEraseCommand(int lba, int size);

  const int HEX_BASE = 16;
  const int MAX_BUFFER_SIZE = 5;

private:
  CmdBufferControl(); // 생성자에서 bufferPath 설정
  void updateExistingEraseMap();
  void updateExistingCmdBuffer();
  void createBufferDirectory();
  CmdBufferControl(const CmdBufferControl &) = delete;
  CmdBufferControl &operator=(const CmdBufferControl &) = delete;

  std::vector<CmdBuffer> cmdBuffer;
  std::string bufferPath = "./buffer";
  SSDDriver *driver;
  char *eraseMap;
};

class CmdBufferInvalidIdexException : public std::runtime_error {
public:
  CmdBufferInvalidIdexException()
      : std::runtime_error("Invalid Buffer Index") {}
};
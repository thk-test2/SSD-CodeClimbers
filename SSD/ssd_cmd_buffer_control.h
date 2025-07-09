#pragma once
#include "ssd_cmd_buffer.h"
#include <string>
#include <vector>

class CmdBufferControl {
public:
  static CmdBufferControl &getInstance();

  std::string getBufferNameList() const;
  bool updateToNextEmpty(const std::string &cmd);
  bool updateBufferByIndex(int index, const std::string &cmd);
  std::string setBufferName(int index, const std::string &cmd);
  bool isValidBufferIndex(int index);
  bool clearBufferByIndex(int index);
  void clearAllBuffer(void);
  void flush();
  bool isBufferFull() const;
  const int MAX_BUFFER_SIZE = 5;

private:
  CmdBufferControl(); // 생성자에서 bufferPath 설정
  CmdBufferControl(const CmdBufferControl &) = delete;
  CmdBufferControl &operator=(const CmdBufferControl &) = delete;

  std::vector<CmdBuffer> cmdBuffer;
  std::string bufferPath = "./buffer";
};
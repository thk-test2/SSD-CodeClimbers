#pragma once
#include "ssd_buffer.h"
#include <vector>
#include <string>

class BufferControl {
public:
  static BufferControl &getInstance();

  std::string getBufferNameList() const;
  bool updateToNextEmpty(const std::string &cmd);
  bool updateBufferByIndex(int index, const std::string &cmd);
  bool isValidBufferIndex(int index);
  bool clearBufferByIndex(int index);
  void clearAllBuffer(void); 
  void flush();
  bool isBufferFull() const;
  const int MAX_BUFFER_SIZE = 5;

private:
  BufferControl(); // 생성자에서 bufferPath 설정
  BufferControl(const BufferControl &) = delete;
  BufferControl &operator=(const BufferControl &) = delete;

  std::vector<Buffer> buffers;
  std::string bufferPath = "./buffer";
};
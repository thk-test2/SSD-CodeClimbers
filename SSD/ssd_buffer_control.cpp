#include "ssd_buffer_control.h"
#include <filesystem>
#include <iostream>
using std::cout;
namespace fs = std::filesystem;

BufferControl::BufferControl() {

  if (!fs::exists(bufferPath)) {
    fs::create_directory(bufferPath);
  }

  for (int i = 1; i <= MAX_BUFFER_SIZE; ++i) {
    std::string file = bufferPath + "/" + std::to_string(i) + "_empty";
    buffers.emplace_back(file);
  }
}

BufferControl &BufferControl::getInstance() {
  static BufferControl instance;
  return instance;
}

std::string BufferControl::getBufferNameList() const {
  std::string list = "";

  for (const auto &buf : buffers) {
    list += (buf.getName() + ",");
  }

  return list;
}

bool BufferControl::updateToNextEmpty(const std::string &cmd) {
  for (auto &buf : buffers) {
    if (buf.isEmpty()) {
      buf.updateCommand(cmd);
      return true;
    }
  }
  return false; // no empty slot
}

bool BufferControl::updateBufferByIndex(int index, const std::string &cmd) {

  if (!isValidBufferIndex(index))
    return false;

  buffers[index - 1].updateCommand(cmd);
  return true; // no empty slot
}

bool BufferControl::isValidBufferIndex(int index) {
  return (index <= MAX_BUFFER_SIZE && index >= 1);
}

bool BufferControl::clearBufferByIndex(int index) {

  if (index > MAX_BUFFER_SIZE || index < 1)
    return false;
  buffers[index - 1].clear();
  return true; // no empty slot
}

void BufferControl::clearAllBuffer(void) {
  for (auto &buf : buffers) {
    if (!buf.isEmpty()) {
      buf.clear();
    }
  }
}

bool BufferControl::isBufferFull() const {
  for (const auto &buf : buffers) {
    if (buf.isEmpty()) {
      return false;
    }
  }
  return true;
}
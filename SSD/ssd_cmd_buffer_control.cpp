#include "ssd_cmd_buffer_control.h"
#include <filesystem>
#include <iostream>
using std::cout;
namespace fs = std::filesystem;

CmdBufferControl::CmdBufferControl() {

  if (!fs::exists(bufferPath)) {
    fs::create_directory(bufferPath);
  }

  for (int i = 1; i <= MAX_BUFFER_SIZE; ++i) {
    std::string file = bufferPath + "/" + std::to_string(i) + "_empty";
    cmdBuffer.emplace_back(file);
  }
}

CmdBufferControl &CmdBufferControl::getInstance() {
  static CmdBufferControl instance;
  return instance;
}

std::string CmdBufferControl::getBufferNameList() const {
  std::string list = "";

  for (const auto &buf : cmdBuffer) {
    list += (buf.getName() + ",");
  }

  return list;
}

bool CmdBufferControl::updateToNextEmpty(const std::string &cmd) {
  for (auto &buf : cmdBuffer) {
    if (buf.isEmpty()) {
      buf.updateCommand(cmd);
      return true;
    }
  }
  return false; // no empty slot
}

bool CmdBufferControl::updateBufferByIndex(int index, const std::string &cmd) {

  if (!isValidBufferIndex(index))
    return false;

  cmdBuffer[index - 1].updateCommand(cmd);
  return true; // no empty slot
}

bool CmdBufferControl::isValidBufferIndex(int index) {
  return (index <= MAX_BUFFER_SIZE && index >= 1);
}

bool CmdBufferControl::clearBufferByIndex(int index) {

  if (index > MAX_BUFFER_SIZE || index < 1)
    return false;
  cmdBuffer[index - 1].clear();
  return true; // no empty slot
}

void CmdBufferControl::clearAllBuffer(void) {
  for (auto &buf : cmdBuffer) {
    if (!buf.isEmpty()) {
      buf.clear();
    }
  }
}

bool CmdBufferControl::isBufferFull() const {
  for (const auto &buf : cmdBuffer) {
    if (buf.isEmpty()) {
      return false;
    }
  }
  return true;
}
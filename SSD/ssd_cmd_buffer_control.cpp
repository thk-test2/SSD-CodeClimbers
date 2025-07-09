#include "ssd_cmd_buffer_control.h"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

CmdBufferControl::CmdBufferControl() {
  if (!fs::exists(bufferPath)) {
    fs::create_directory(bufferPath);
  }

  for (int i = 1; i <= MAX_BUFFER_SIZE; ++i) {
    std::string existingFile = "";
    for (const auto &entry : fs::directory_iterator(bufferPath)) {
      if (!entry.is_regular_file())
        continue;

      std::string fname = entry.path().filename().string();
      if (fname.find(std::to_string(i) + "_") == 0) {
        existingFile = fname;
        break;
      }
    }

    if (existingFile.empty()) {
      existingFile = std::to_string(i) + "_empty";
      std::ofstream ofs(bufferPath + "/" + existingFile);
      ofs.close();
    }

    cmdBuffer.emplace_back(bufferPath + "/" + existingFile);
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
  int bufferIndex = 1;
  for (auto &buf : cmdBuffer) {
    if (buf.isEmpty()) {
      buf.updateCommand(setBufferName(bufferIndex, cmd));
      return true;
    }
    bufferIndex++;
  }
  return false;
}

bool CmdBufferControl::updateBufferByIndex(int index, const std::string &cmd) {

  if (!isValidBufferIndex(index))
    return false;

  cmdBuffer[index - 1].updateCommand(setBufferName(index, cmd));
  return true;
}

std::string CmdBufferControl::setBufferName(int index, const std::string &cmd) {
  return std::to_string(index) + "_" + cmd;
}

bool CmdBufferControl::isValidBufferIndex(int index) {
  return (index <= MAX_BUFFER_SIZE && index >= 1);
}

bool CmdBufferControl::clearBufferByIndex(int index) {

  if (!isValidBufferIndex(index))
    return false;
  cmdBuffer[index - 1].clear();
  return true;
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
#include "ssd_cmd_buffer_control.h"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

CmdBufferControl::CmdBufferControl() {
  driver = new SSDDriver();
  eraseMap = new char[driver->getMaxNandSize()];
  std::memset(eraseMap, 0, driver->getMaxNandSize());

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

  for (auto buf : cmdBuffer) {
    if (buf.isEmpty())
      continue;
    if (buf.getCmd() == 'E') {
      std::memset(eraseMap + buf.getLba(), 1, buf.getLbaSize());
    }
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

string makdFullCmdString(char *argv[]) {
  string fullCmd = argv[1];
  int i = 2;
  while (argv[i] != nullptr) {
    fullCmd += "_";
    fullCmd += argv[i];
    i++;
  }
  return fullCmd;
}

bool CmdBufferControl::runCommandBuffer(char *argv[]) {
  bool ret = true;
  string cmdType = argv[1];
  int lba = std::stoi(argv[2]);

  if (isBufferFull())
    ret = flush();

  if (cmdType == "R") {
    // TO DO : Buffer Check
    ret = driver->read(lba);

  } else if (cmdType == "W") {
    unsigned long value = std::stoul(argv[3], nullptr, 16);

    bool result = writeCmdBuffer(argv);
    if (result)
      return result;

  } else if (cmdType == "E") {
    int size = std::stoi(argv[3]);
    std::memset(eraseMap + lba, 1, size); // set eraseMap

    mergeAndUpdateEraseCommand(lba, size);
  } else if (cmdType == "F") {
    // TO DO : Buffer Check
  }
  return ret;
}

bool CmdBufferControl::writeCmdBuffer(char *argv[]) {
  int lba = std::stoi(argv[2]);

  for (auto &buffer : cmdBuffer) {
    if (buffer.isEmpty())
      continue;
    if (buffer.getCmd() == 'W') {
      if (lba == buffer.getLba()) {
        buffer.clear();
        break;
      }
    }
  }
  updateToNextEmpty(makdFullCmdString(argv));
  emptyBufferShift();
  return true;
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

bool CmdBufferControl::emptyBufferShift() {
  std::vector<std::string> nonEmptyCommands;

  if (isBufferFull() == true)
    return false;

  for (const auto &buf : cmdBuffer) {
    if (!buf.isEmpty()) {
      std::string name = buf.getName();
      size_t pos = name.find('_');
      if (pos != std::string::npos) {
        nonEmptyCommands.push_back(name.substr(pos + 1));
      }
    }
  }

  for (int i = 0; i < cmdBuffer.size(); ++i) {
    if (i < nonEmptyCommands.size()) {
      cmdBuffer[i].updateCommand(setBufferName(i + 1, nonEmptyCommands[i]));
    } else {
      cmdBuffer[i].clear();
    }
  }

  return true;
}

char CmdBufferControl::getBufferCmd(int index) {
  if (!isValidBufferIndex(index))
    throw CmdBufferInvalidIdexException();

  return cmdBuffer[index - 1].getCmd();
}

int CmdBufferControl::getBufferLba(int index) {
  if (!isValidBufferIndex(index))
    throw CmdBufferInvalidIdexException();

  return cmdBuffer[index - 1].getLba();
}

unsigned long CmdBufferControl::getBufferValue(int index) {
  if (!isValidBufferIndex(index))
    throw CmdBufferInvalidIdexException();

  return cmdBuffer[index - 1].getValue();
}

int CmdBufferControl::getBufferLbaSize(int index) {
  if (!isValidBufferIndex(index))
    throw CmdBufferInvalidIdexException();

  return cmdBuffer[index - 1].getLbaSize();
}

bool CmdBufferControl::flushEraseSeparated(int lba, int size) {
  bool ret = false;
  int remain_size = size;
  int unit_size = std::min(getDriver()->getMaxEraseSize(), size);

  std::memset(eraseMap + lba, 0, size);

  while (remain_size > 0) {
    ret = getDriver()->erase(lba, unit_size);
    remain_size -= unit_size;
    lba += unit_size;
    unit_size = std::min(getDriver()->getMaxEraseSize(), remain_size);
  }
  return ret;
}

bool CmdBufferControl::flush() {
  bool ret = false;

  if (getDriver() == nullptr)
    return false;

  char cmd = 0;
  int lba = 0;
  for (auto buffer : cmdBuffer) {
    if (buffer.isEmpty())
      continue;

    cmd = buffer.getCmd();
    lba = buffer.getLba();

    if (cmd == 'W') {
      ret = getDriver()->write(lba, buffer.getValue());
    } else if (cmd == 'E') {
      ret = flushEraseSeparated(lba, buffer.getLbaSize());
    }
  }

  clearAllBuffer();
  return ret;
}

void CmdBufferControl::mergeAndUpdateEraseCommand(int lba, int size) {
  int targetLba = lba;
  int targetSize = size;
  int targetEnd = targetLba + targetSize;

  for (auto &buffer : cmdBuffer) {
    if (buffer.isEmpty())
      continue;
    if (buffer.getCmd() == 'E') {
      int bufferEnd = buffer.getLba() + buffer.getLbaSize();
      if (targetEnd >= buffer.getLba() && targetLba <= bufferEnd) // Duplicated
      {
        targetLba = std::min(buffer.getLba(), targetLba);
        targetEnd = std::max(bufferEnd, targetEnd);
        buffer.clear();
      }
    } else if (buffer.getCmd() == 'W' && eraseMap[buffer.getLba()] == 1) {
      buffer.clear();
    }
  }
  updateToNextEmpty("E_" + std::to_string(targetLba) + "_" +
                    std::to_string(targetEnd - targetLba));
  emptyBufferShift();
}

bool CmdBufferControl::isSameLbaBuffer(int lba, CmdBuffer &buffer) {
  if (buffer.isEmpty())
    return false;

  if (buffer.getCmd() == 'W' && buffer.getLba() == lba)
    return true;
  else if (buffer.getCmd() == 'E' && (isValidRangeForErase(lba, buffer))) {
    return true;
  } else
    return false;
}
bool CmdBufferControl::isValidRangeForErase(int lba, CmdBuffer &buffer) {
  return lba >= buffer.getLba() &&
         lba < (buffer.getLba() + buffer.getLbaSize());
}

bool CmdBufferControl::isBufferContainReadValue(int lba, unsigned long &value) {
  bool isContain = false;
  for (auto buffer : cmdBuffer) {
    if (isSameLbaBuffer(lba, buffer)) {
      if (buffer.getCmd() == 'W')
        value = buffer.getValue();
      else if (buffer.getCmd() == 'E')
        value = 0x00000000;

      isContain = true;
    }
  }
  return isContain;
}

SSDDriver *CmdBufferControl::getDriver() { return driver; }

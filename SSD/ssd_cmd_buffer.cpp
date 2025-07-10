#include "ssd_cmd_buffer.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace fs = std::filesystem;
using std::cout;

CmdBuffer::CmdBuffer(const std::string &path) : fileName(path) {
  if (!fs::exists(fileName)) {
    std::ofstream ofs(fileName);
    ofs.close();
  }
}

std::string CmdBuffer::getName() const {
  return fs::path(fileName).filename().string();
}

void CmdBuffer::updateCommand(const std::string &cmd) {
  fs::rename(fileName, fs::path(fileName).parent_path() / cmd);
  fileName = (fs::path(fileName).parent_path() / cmd).string();
}

void CmdBuffer::clear() {
  updateCommand(std::to_string(getIndex()) + "_empty");
}

bool CmdBuffer::isEmpty() const {
  return getName().find("empty") != std::string::npos;
}

int CmdBuffer::getIndex() const {
  std::string name = getName();
  return std::stoi(name.substr(0, name.find('_')));
}

std::vector<std::string> CmdBuffer::splitFileName() const {
  std::vector<std::string> result;
  std::stringstream filenameStream(fileName);
  std::string token;

  while (std::getline(filenameStream, token, '_')) {
    result.push_back(token);
  }

  return result;
}

char CmdBuffer::getCmd() const {
  if (isEmpty())
    return '0';

  auto parts = splitFileName();

  return parts[1][0];
}

int CmdBuffer::getLba() const {
  if (isEmpty())
    throw CmdBufferEmptyException();

  auto parts = splitFileName();

  return std::stoi(parts[2]);
}

unsigned long CmdBuffer::getValue() const {
  if (isEmpty())
    throw CmdBufferEmptyException();

  auto parts = splitFileName();
  char cmd = getCmd();

  if (cmd == 'W') {
    return std::stoul(parts[3], nullptr, 16);
  } else {
    throw std::runtime_error("Unknown command: " + std::string(1, cmd));
  }
}

int CmdBuffer::getLbaSize() const {
  if (isEmpty())
    throw CmdBufferEmptyException();

  auto parts = splitFileName();
  char cmd = getCmd();

  if (cmd == 'E') {
    return std::stoul(parts[3]);
  } else {
    throw std::runtime_error("Unknown command: " + std::string(1, cmd));
  }
}

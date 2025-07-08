#include "ssd_buffer.h"
#include <iostream>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;
using std::cout;

Buffer::Buffer(const std::string &path) : fileName(path) {
  if (!fs::exists(fileName)) {
    std::ofstream ofs(fileName);
    ofs.close();
  }
}

std::string Buffer::getName() const {
  return fs::path(fileName).filename().string();
}

void Buffer::updateCommand(const std::string &cmd) {
  fs::rename(fileName, fs::path(fileName).parent_path() / cmd);
  fileName = (fs::path(fileName).parent_path() / cmd).string();
}

void Buffer::clear() { updateCommand(std::to_string(getIndex()) + "_empty"); }

bool Buffer::isEmpty() const {
  return getName().find("empty") != std::string::npos;
}

int Buffer::getIndex() const {
  std::string name = getName();
  return std::stoi(name.substr(0, name.find('_')));
}

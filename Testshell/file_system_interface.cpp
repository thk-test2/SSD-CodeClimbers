#include "file_system_interface.h"
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <io.h>
#include <windows.h>


bool FileSystem::fileExists(const std::string &filename) {
  std::ifstream file(filename.c_str());
  return file.good();
}

size_t FileSystem::getFileSize(const std::string &filename) {
  std::ifstream file(filename.c_str(), std::ios::binary | std::ios::ate);
  if (file.is_open()) {
    return static_cast<size_t>(file.tellg());
  }
  return 0;
}

bool FileSystem::renameFile(const std::string &oldName,
                            const std::string &newName) {
  return std::rename(oldName.c_str(), newName.c_str()) == 0;
}

std::vector<std::string> FileSystem::getUntilLogFiles() {
  std::vector<std::string> untilFiles;

  WIN32_FIND_DATAA findFileData;
  HANDLE hFind = FindFirstFileA("until_*.log", &findFileData);

  if (hFind != INVALID_HANDLE_VALUE) {
    do {
      untilFiles.push_back(std::string(findFileData.cFileName));
    } while (FindNextFileA(hFind, &findFileData) != 0);
    FindClose(hFind);
  }

  return untilFiles;
}

void FileSystem::writeToFile(const std::string &filename,
                             const std::string &content) {
  std::ofstream file(filename.c_str(), std::ios::app);
  if (file.is_open()) {
    file << content << std::endl;
    file.close();
  }
}
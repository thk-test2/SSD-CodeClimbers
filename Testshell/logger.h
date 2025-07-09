#pragma once

#include "file_system_interface.h"
#include <memory>
#include <string>
#include <vector>

class Logger {
private:
  static Logger *instance;
  std::string currentLogFile;
  std::unique_ptr<IFileSystem> fileSystem;

  Logger(std::unique_ptr<IFileSystem> fs = std::make_unique<FileSystem>());

  std::string getCurrentTimestamp();
  std::string getRotationTimestamp();
  void writeToFile(const std::string &logEntry);
  void checkFileRotation();
  void rotateLogFile();
  void manageOldLogFiles();

public:
  static Logger &getInstance();
  static void setFileSystem(std::unique_ptr<IFileSystem> fs);
  static void resetInstance();

  void print(const std::string &className, const std::string &message);

  // Prevent copying
  Logger(const Logger &) = delete;
  Logger &operator=(const Logger &) = delete;
};
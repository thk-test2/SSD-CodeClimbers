#include "logger.h"
#include <algorithm>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

Logger *Logger::instance = nullptr;

Logger::Logger(std::unique_ptr<IFileSystem> fs)
    : currentLogFile("latest.log"), fileSystem(std::move(fs)) {}

Logger &Logger::getInstance() {
  if (instance == nullptr) {
    instance = new Logger();
  }
  return *instance;
}

void Logger::setFileSystem(std::unique_ptr<IFileSystem> fs) {
  if (instance != nullptr) {
    instance->fileSystem = std::move(fs);
  } else {
    instance = new Logger(std::move(fs));
  }
}

void Logger::resetInstance() {
  delete instance;
  instance = nullptr;
}

void Logger::print(const std::string &className, const std::string &message) {
  std::string timestamp = getCurrentTimestamp();
  
  std::ostringstream oss;
  oss << "[" << timestamp << "] " << std::left << std::setw(30) << className
      << ": " << message << ".";
  std::string logEntry = oss.str();

  writeToFile(logEntry);
  checkFileRotation();
}

std::string Logger::getCurrentTimestamp() {
  auto now = std::chrono::system_clock::now();
  auto time_t = std::chrono::system_clock::to_time_t(now);

  std::tm tm;
  localtime_s(&tm, &time_t);

  std::ostringstream oss;
  oss << std::setfill('0') << std::setw(2) << (tm.tm_year % 100) << "."
      << std::setfill('0') << std::setw(2) << (tm.tm_mon + 1) << "."
      << std::setfill('0') << std::setw(2) << tm.tm_mday << " "
      << std::setfill('0') << std::setw(2) << tm.tm_hour << ":"
      << std::setfill('0') << std::setw(2) << tm.tm_min;

  return oss.str();
}

std::string Logger::getRotationTimestamp() {
  auto now = std::chrono::system_clock::now();
  auto time_t = std::chrono::system_clock::to_time_t(now);

  std::tm tm;
  localtime_s(&tm, &time_t);

  std::ostringstream oss;
  oss << std::setfill('0') << std::setw(2) << (tm.tm_year % 100)
      << std::setfill('0') << std::setw(2) << (tm.tm_mon + 1)
      << std::setfill('0') << std::setw(2) << tm.tm_mday << "_"
      << std::setfill('0') << std::setw(2) << tm.tm_hour << "h_"
      << std::setfill('0') << std::setw(2) << tm.tm_min << "m_"
      << std::setfill('0') << std::setw(2) << tm.tm_sec << "s";

  return oss.str();
}

void Logger::writeToFile(const std::string &logEntry) {
  fileSystem->writeToFile(currentLogFile, logEntry);
}

void Logger::checkFileRotation() {
  if (fileSystem->fileExists(currentLogFile)) {
    size_t fileSize = fileSystem->getFileSize(currentLogFile);
    const size_t MAX_SIZE = 10 * 1024; // 10KB

    if (fileSize > MAX_SIZE) {
      rotateLogFile();
      manageOldLogFiles();
    }
  }
}

void Logger::rotateLogFile() {
  std::string timestamp = getRotationTimestamp();
  std::string newFilename = "until_" + timestamp + ".log";

  if (fileSystem->fileExists(currentLogFile)) {
    if (!fileSystem->renameFile(currentLogFile, newFilename)) {
      std::cerr << "Logger rotation error: Failed to rename " << currentLogFile
                << " to " << newFilename << std::endl;
    }
  }
}

void Logger::manageOldLogFiles() {
  std::vector<std::string> untilFiles = fileSystem->getUntilLogFiles();

  if (untilFiles.size() > 1) {
    std::sort(untilFiles.begin(), untilFiles.end());

    std::string oldestFile = untilFiles[0];
    size_t lastDot = oldestFile.find_last_of('.');
    if (lastDot != std::string::npos) {
      std::string zipFile = oldestFile.substr(0, lastDot) + ".zip";

      if (!fileSystem->renameFile(oldestFile, zipFile)) {
        std::cerr << "Logger management error: Failed to rename " << oldestFile
                  << " to " << zipFile << std::endl;
      }
    }
  }
}
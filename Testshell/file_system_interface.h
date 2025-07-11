#pragma once

#include <string>
#include <vector>

class IFileSystem {
public:
  virtual ~IFileSystem() = default;
  virtual bool fileExists(const std::string &filename) = 0;
  virtual size_t getFileSize(const std::string &filename) = 0;
  virtual bool renameFile(const std::string &oldName,
                          const std::string &newName) = 0;
  virtual std::vector<std::string> getUntilLogFiles() = 0;
  virtual void writeToFile(const std::string &filename,
                           const std::string &content) = 0;
  virtual bool createDirectory(const std::string &path) = 0;
  virtual bool directoryExists(const std::string &path) = 0;
};

class FileSystem : public IFileSystem {
public:
  bool fileExists(const std::string &filename) override;
  size_t getFileSize(const std::string &filename) override;
  bool renameFile(const std::string &oldName,
                  const std::string &newName) override;
  std::vector<std::string> getUntilLogFiles() override;
  void writeToFile(const std::string &filename,
                   const std::string &content) override;
  bool createDirectory(const std::string &path) override;
  bool directoryExists(const std::string &path) override;
};
#pragma once
#include <string>
#include <vector>

class CmdBuffer {
public:
  CmdBuffer(const std::string &path);

  std::string getName() const;
  void updateCommand(const std::string &cmd);
  void clear();
  bool isEmpty() const;
  int getIndex() const;
  char getCmd() const;
  int getLba() const;
  unsigned long getValue() const;
  int getLbaSize() const;
  std::vector<std::string> splitFileName() const;

private:
  std::string fileName;
};

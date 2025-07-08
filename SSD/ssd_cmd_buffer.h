#pragma once
#include <string>

class CmdBuffer {
public:
  CmdBuffer(const std::string &path);

  std::string getName() const;
  void updateCommand(const std::string &cmd);
  void clear();
  bool isEmpty() const;
  int getIndex() const;

private:
  std::string fileName;
};

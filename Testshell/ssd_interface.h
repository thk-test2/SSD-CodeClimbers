#pragma once

#include <string>

using std::string;

class SSD_INTERFACE {
public:
  virtual ~SSD_INTERFACE() = default;
  virtual void read(int lba) = 0;
  virtual void write(int lba, unsigned long value) = 0;
  virtual void erase(int lba, int size) = 0;
  virtual string getResult() = 0;
};
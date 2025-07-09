#pragma once
class Device {
public:
  virtual bool write(int idx, unsigned long value) = 0;
  virtual bool read(int idx) = 0;
  virtual bool erase(int idx, int size) = 0;
};


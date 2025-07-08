class Device {
public:
  virtual bool write(int idx, unsigned long value) = 0;
  virtual bool read(int idx) = 0;
  virtual void run(int argc, char* argv[]) = 0;
};


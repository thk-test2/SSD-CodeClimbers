class Device {
public:
  virtual bool write(int idx, unsigned int value) = 0;
  virtual bool read(int idx) = 0;
  virtual void run(int argc, char* argv[]) = 0;
};


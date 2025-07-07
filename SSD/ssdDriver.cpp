class SSDDriver {
public:
  bool read(int lba) {
    if (lba < 0 || lba > 100)
      return false;
    else
      return true;
  }
  bool write(int lba, int value) {
    if (lba < 0 || lba > 100)
      return false;
    else
      return true;
  }
};
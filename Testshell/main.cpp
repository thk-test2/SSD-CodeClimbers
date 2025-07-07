#include "gmock/gmock.h"

class SSD_INTERFACE {
public:
  virtual bool read(int lba);
  virtual bool write(int lba, int value);
};

// MockDirver
class MockSSD : public SSD_INTERFACE {
public:
  MOCK_METHOD(bool, read, (int lba), (override));
  MOCK_METHOD(bool, write, (int lba, int value), (override));
};

class stdInOut_Ctrl {

};

class ArgParseing {

};

class ssd_driver : public SSD_INTERFACE {

};

class TestShell {
private:
  stdInOut_Ctrl *ctrl;
  ArgParseing *arg;
  ssd_driver *ssd;
};


int main() {
    ::testing::InitGoogleMock();
    return RUN_ALL_TESTS();
}

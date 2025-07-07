#include "gmock/gmock.h"
#include <string>

using std::string;

class SSD_INTERFACE {
public:
  virtual bool read(int lba);
  virtual bool write(int lba, int value);
  virtual string getResult();
};

// MockDirver
class MockSSD : public SSD_INTERFACE {
public:
  MOCK_METHOD(bool, read, (int lba), (override));
  MOCK_METHOD(bool, write, (int lba, int value), (override));
  MOCK_METHOD(string, getResult, (), (override));
};

class StdInOutCtrl {

};

class ArgParser {

};

class SsdInterface : public SSD_INTERFACE {

};

class TestShell {
private:
  StdInOutCtrl *ctrl;
  ArgParser *parser;
  SsdInterface *ssd;
};


int main() {
    ::testing::InitGoogleMock();
    return RUN_ALL_TESTS();
}

#include "gmock/gmock.h"
#include "ssdDriver.cpp"

using namespace testing;

class MockSSD : public Device {
public:
  MOCK_METHOD(bool, write, (int, unsigned int), (override));
  MOCK_METHOD(bool, read, (int), (override));
};

TEST(SSD_TS, ReadPass) {
  MockSSD ssdDriver;
  EXPECT_CALL(ssdDriver, read(AllOf(Ge(0), Lt(100))))
	  .WillRepeatedly(Return(true));
  EXPECT_TRUE(ssdDriver.read(20));
}

TEST(SSD_TS, ReadFailOutOfRange) {
  MockSSD ssdDriver;
  EXPECT_CALL(ssdDriver, read(AnyOf(Lt(0), Ge(100))))
      .WillRepeatedly(Return(false));
  EXPECT_FALSE(ssdDriver.read(200));
}

TEST(SSD_TS, WritePass) {
  MockSSD ssdDriver;
  EXPECT_CALL(ssdDriver, write(AllOf(Ge(0), Lt(100)),_))
      .WillRepeatedly(Return(true));
  EXPECT_TRUE(ssdDriver.write(20,0x12345678));
}

TEST(SSD_TS, WriteFailOutOfRange) {
  MockSSD ssdDriver;
  EXPECT_CALL(ssdDriver, write(AnyOf(Lt(0), Ge(100)),_))
      .WillRepeatedly(Return(false));
  EXPECT_FALSE(ssdDriver.write(200, 0x12345678));
}

TEST(TS, RunInvalidParam1) {
    SSDDriver ssd;

    //ssd.exe R 10 0x00010001 
    int argc = 1;
    char* argv[1];
    argv[0] = const_cast<char*>("ssd.exe");

    vector<string> params = ssd.parseParams(argc, argv);

    EXPECT_EQ(params.size(), 0);
}

TEST(TS, RunInvalidParam2) {
    SSDDriver ssd;

    int argc = 3;
    char* argv[3];
    argv[0] = const_cast<char*>("ssd.exe");
    argv[1] = const_cast<char*>("R");
    argv[2] = const_cast<char*>("10");
    vector<string> params = ssd.parseParams(argc, argv);

    EXPECT_EQ(params.size(), 2);
}



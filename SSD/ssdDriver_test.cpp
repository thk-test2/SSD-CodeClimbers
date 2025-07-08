#include "gmock/gmock.h"
#include "ssdDriver.cpp"

using namespace testing;

class MockSSD : public Device {
public:
  MOCK_METHOD(bool, write, (int, unsigned long), (override));
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

class SSDDriverTestFixture : public Test {
public:
  SSDDriver ssd;
};

TEST_F(SSDDriverTestFixture, SSDReadInitialPass) {
  string output;
  int lba = 2;
  ssd.read(lba);

  istringstream iss = ssd.getIoStream()->getOutputReadStream();
  getline(iss, output);

  EXPECT_EQ("0x00000000", output);
}

TEST_F(SSDDriverTestFixture, SSDReadOutOfRangeFail) {
  string output;
  int lba = 100;
  ssd.read(lba);

  istringstream iss = ssd.getIoStream()->getOutputReadStream();
  getline(iss, output);

  EXPECT_EQ("ERROR", output);
}

TEST_F(SSDDriverTestFixture, SSDWritePass) {
  string output;
  int lba = 1;
  ssd.write(lba, 0xAAAAFFFF);

  istringstream iss = ssd.getIoStream()->getOutputReadStream();
  getline(iss, output);

  EXPECT_EQ("", output);
}

TEST_F(SSDDriverTestFixture, SSDWriteOutOfRange) {
  string output;
  int lba = 100;
  ssd.write(lba, 0xAAAAFFFF);

  istringstream iss = ssd.getIoStream()->getOutputReadStream();
  getline(iss, output);

  EXPECT_EQ("ERROR", output);
}

TEST_F(SSDDriverTestFixture, SSDReadWriteCompare) {
  string output;
  int lba = 1;
  ssd.write(1, 0xAAAAFFFF);
  ssd.read(1);

  istringstream iss = ssd.getIoStream()->getOutputReadStream();
  getline(iss, output);
  
  EXPECT_EQ("0xAAAAFFFF", output);
}

TEST_F(SSDDriverTestFixture, SSDWrite2TimesCompare) {
  string output;
  int lba = 1;
  ssd.write(1, 0xAAAAFFFF);
  ssd.write(1, 0xEEEEAAAA);
  ssd.read(1);

  istringstream iss = ssd.getIoStream()->getOutputReadStream();
  getline(iss, output);

  EXPECT_EQ("0xEEEEAAAA", output);
}

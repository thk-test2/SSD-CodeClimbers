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
protected:
  SSDDriver ssd;
};

TEST_F(SSDDriverTestFixture, SSDReadInitialPass) {
  string output;
  ssd.read(2);

  istringstream iss = ssd.getIoStream()->getOutputReadStream();
  getline(iss, output);

  EXPECT_EQ("0x00000000", output);
}
#if 0
TEST_F(IOstreamTestFixture, SSDReadFail) {
  unsigned long value = ssd.read(1);
  EXPECT_NE(value, 0x1);
}

TEST_F(IOstreamTestFixture, SSDWritePass) {
  ssd.write(1, 0xAAAAFFFF);

  unsigned long value = ssd.read(1);
  EXPECT_EQ(value, 0xAAAAFFFF);
}

TEST_F(IOstreamTestFixture, SSDErrorMassageCheckAfterReadLBARangeFail) {
  ssd.read(100);

  EXPECT_EQ("ERROR", ioStream.readFileAsString(ioStream.output_file_name));
}

TEST_F(IOstreamTestFixture, SSDErrorMassageCheckAfterWriteLBARangeFail) {
  ssd.write(100,0xAAAAAAAA);

  EXPECT_EQ("ERROR", ioStream.readFileAsString(ioStream.output_file_name));
}

#endif
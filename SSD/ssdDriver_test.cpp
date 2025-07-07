#include "gmock/gmock.h"
#include "ssdDriver.cpp"

TEST(SSD_TS, ReadPass) {
  SSDDriver ssdDriver;
  EXPECT_TRUE(ssdDriver.read(20));
}

TEST(SSD_TS, ReadFailOutOfRange) {
  SSDDriver ssdDriver;
  EXPECT_FALSE(ssdDriver.read(200));
}

TEST(SSD_TS, WritePass) {
  SSDDriver ssdDriver;
  EXPECT_TRUE(ssdDriver.write(20,0x12345678));
}

TEST(SSD_TS, WriteFailOutOfRange) {
  SSDDriver ssdDriver;
  EXPECT_FALSE(ssdDriver.write(200, 0x12345678));
}
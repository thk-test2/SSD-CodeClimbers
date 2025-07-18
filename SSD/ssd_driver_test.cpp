#include "ssd_driver.h"
#include "gmock/gmock.h"

using namespace testing;
class MockSSD : public Device {
public:
  MOCK_METHOD(bool, write, (int, unsigned long), (override));
  MOCK_METHOD(bool, read, (int), (override));
  MOCK_METHOD(bool, erase, (int, int), (override));
};

TEST(SSD_TS, ReadPass) {
  MockSSD ssdDriver;
  EXPECT_CALL(ssdDriver, read(AllOf(Ge(0), Lt(100))))
      .WillRepeatedly(Return(true));
  EXPECT_TRUE(ssdDriver.read(20));
}

TEST(SSD_TS, WritePass) {
  MockSSD ssdDriver;
  EXPECT_CALL(ssdDriver, write(AllOf(Ge(0), Lt(100)), _))
      .WillRepeatedly(Return(true));
  EXPECT_TRUE(ssdDriver.write(20, 0x12345678));
}

class SSDDriverTestData {
public:
  int lba;
  unsigned long value;

  SSDDriverTestData(int lba_, unsigned long value_)
      : lba(lba_), value(value_) {}
};

class SSDDriverTestFixture : public Test {
public:
  SSDDriver ssd;
  const int MAX_NAND_MEMORY_MAP_SIZE = 100;

  void SetUp() override {
    ssd.getIoStream()->initSsdNand();
    ssd.getIoStream()->clearOutput();
  }

  void TearDown() {
    ssd.getIoStream()->initSsdNand();
    ssd.getIoStream()->clearOutput();
  }

  void checkOutputFileValid(const string &expected_value) {
    EXPECT_EQ(expected_value, ssd.getIoStream()->readFileAsString(
                                  ssd.getIoStream()->output_file_name));
  }

  void checkSpecificLineInNandFile(const string &expected_value,
                                   int targetLine) {
    EXPECT_GE(targetLine, 0);
    EXPECT_LT(targetLine, MAX_NAND_MEMORY_MAP_SIZE);

    std::istringstream iss(
        ssd.getIoStream()->readFileAsString(ssd.getIoStream()->nand_file_name));
    string getLine = "";
    string compareLine = std::to_string(targetLine) + " " + expected_value;

    for (int i = 0; i < MAX_NAND_MEMORY_MAP_SIZE; i++) {
      std::getline(iss, getLine);
      if (i == targetLine) {
        EXPECT_EQ(getLine, compareLine);
        break;
      }
    }
  }
};

TEST_F(SSDDriverTestFixture, SSDReadInitialPass) {
  string output;
  int lba = 2;
  ssd.read(lba);

  checkOutputFileValid("0x00000000");
}

TEST_F(SSDDriverTestFixture, SSDWritePass) {
  string output;
  int lba = 1;
  ssd.write(lba, 0xAAAAFFFF);

  checkOutputFileValid("");
}

TEST_F(SSDDriverTestFixture, SSDReadAfterWritePass) {
  string output;
  int lba = 1;
  long value = 0;
  ssd.write(lba, 0xAAAAFFFF);

  checkOutputFileValid("");

  EXPECT_TRUE(ssd.read(lba));
  checkOutputFileValid("0xAAAAFFFF");
  checkSpecificLineInNandFile("0xAAAAFFFF", lba);
}

TEST_F(SSDDriverTestFixture, SSDReadWriteCompare) {
  string output;
  int lba = 1;
  ssd.write(1, 0xAAAAFFFF);
  ssd.read(1);

  checkOutputFileValid("0xAAAAFFFF");
}

TEST_F(SSDDriverTestFixture, SSDWrite2TimesCompare) {
  string output;
  int lba = 1;
  ssd.write(1, 0xAAAAFFFF);
  ssd.write(1, 0xEEEEAAAA);
  ssd.read(1);

  checkOutputFileValid("0xEEEEAAAA");
}

TEST_F(SSDDriverTestFixture, SSDCheckSsdNandFileValidAfterWrite) {
  std::vector<SSDDriverTestData> writeDataVector;
  std::istringstream backup_iss(ssd.getIoStream()->readFileAsString(ssd.getIoStream()->nand_file_name));
  string beforeTxtLine = "", AfterTxtLine = "";
  int lineNo = 0;
  int mismatchLineNum = 0;

  // ssd_nand.txt write
  writeDataVector.push_back(SSDDriverTestData(1, 0x00000001));
  writeDataVector.push_back(SSDDriverTestData(2, 0x00000002));
  writeDataVector.push_back(SSDDriverTestData(3, 0x00000003));
  writeDataVector.push_back(SSDDriverTestData(10, 0x00000010));
  writeDataVector.push_back(
      SSDDriverTestData(MAX_NAND_MEMORY_MAP_SIZE - 1, 0x00000099));

  for (const auto &data : writeDataVector) {
    ssd.write(data.lba, data.value);
  }

  // Read again nana_file.txt
  std::istringstream iss(
      ssd.getIoStream()->readFileAsString(ssd.getIoStream()->nand_file_name));

  while (std::getline(backup_iss, beforeTxtLine) &&
         std::getline(iss, AfterTxtLine)) {

    if (beforeTxtLine != AfterTxtLine) {
      EXPECT_EQ(lineNo, writeDataVector[mismatchLineNum].lba);
      EXPECT_EQ(stoul(AfterTxtLine, nullptr, 16),
                writeDataVector[mismatchLineNum].value);
      mismatchLineNum++;
    }
    lineNo++;
  }
}

TEST_F(SSDDriverTestFixture, SSDEraseAfterWrite) {
  string output;
  int idx, startLBA, eraseSize;
  startLBA = 5;
  eraseSize = 10;

  for (idx = startLBA; idx < startLBA + eraseSize; idx++)
    ssd.write(idx, 0x5a5a5a5a);

  ssd.erase(startLBA, eraseSize);

  for (idx = startLBA; idx < startLBA + eraseSize; idx++) {
    checkSpecificLineInNandFile("0x00000000", idx);
  }
}

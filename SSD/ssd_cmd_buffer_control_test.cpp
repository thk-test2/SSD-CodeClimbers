#include "ssd_cmd_buffer_control.h"
#include "gmock/gmock.h"
using std::cout;

using namespace testing;

class BufferControlFixture : public Test {
public:
  void SetUp() override {
    cmdBuffer.clearAllBuffer();
    cmdBuffer.clearEraseMap();
    stream->initSsdNand();
    stream->clearOutput();
  }

  void TearDown() override {
    cmdBuffer.clearAllBuffer();
    cmdBuffer.clearEraseMap();
    stream->initSsdNand();
    stream->clearOutput();
  }
  CmdBufferControl &cmdBuffer = CmdBufferControl::getInstance();
  IoStream *stream = cmdBuffer.getDriver()->getIoStream();
  const std::string BUFFER_EMPTY = "1_empty,2_empty,3_empty,4_empty,5_empty,";

  void checkOutputFileValid(const string &expected_value) {
    EXPECT_EQ(expected_value,
              stream->readFileAsString(stream->output_file_name));
  }
  void checkBufferValid(const string &expected_value) {
    EXPECT_EQ(expected_value,
              stream->readFileAsString(stream->output_file_name));
  }

  void checkSpecificLineInNandFile(const string &expected_value,
                                   int targetLine) {
    EXPECT_GE(targetLine, 0);
    EXPECT_LT(targetLine, stream->getStorageSize());

    std::istringstream iss(stream->readFileAsString(stream->nand_file_name));
    string getLine = "";
    string compareLine = std::to_string(targetLine) + " " + expected_value;

    for (int i = 0; i < stream->getStorageSize(); i++) {
      std::getline(iss, getLine);
      if (i == targetLine) {
        EXPECT_EQ(getLine, compareLine);
        break;
      }
    }
  }
};

TEST_F(BufferControlFixture, BufferCheckAfterInit) {
  std::string expected_ret = BUFFER_EMPTY;
  EXPECT_EQ(expected_ret, cmdBuffer.getBufferNameList());
}

TEST_F(BufferControlFixture, BufferUpdateByIndexTest) {
  std::string expected_ret =
      "1_empty,2_W_20_0xABCDABCD,3_empty,4_empty,5_empty,";
  cmdBuffer.updateBufferByIndex(2, "W_20_0xABCDABCD");
  EXPECT_EQ(expected_ret, cmdBuffer.getBufferNameList());
}

TEST_F(BufferControlFixture, BufferUpdateByIndex2Test) {
  EXPECT_TRUE(cmdBuffer.updateBufferByIndex(2, "W_20_0xABCDABCD"));
  EXPECT_TRUE(cmdBuffer.updateBufferByIndex(2, "R_10"));
}

TEST_F(BufferControlFixture, BufferUpdateByIndexTestFail) {
  EXPECT_FALSE(cmdBuffer.updateBufferByIndex(0, "W_20_0xABCDABCD"));
  EXPECT_FALSE(cmdBuffer.updateBufferByIndex(6, "W_20_0xABCDABCD"));
  EXPECT_FALSE(cmdBuffer.updateBufferByIndex(100, "W_20_0xABCDABCD"));
}

TEST_F(BufferControlFixture, BufferClearTest) {
  std::string expected_ret = BUFFER_EMPTY;
  cmdBuffer.updateBufferByIndex(2, "W_20_0xABCDABCD");
  cmdBuffer.clearBufferByIndex(2);
  EXPECT_EQ(expected_ret, cmdBuffer.getBufferNameList());
}

TEST_F(BufferControlFixture, BufferAllClearTest) {
  std::string expected_ret = BUFFER_EMPTY;
  cmdBuffer.updateBufferByIndex(1, "R_20");
  cmdBuffer.updateBufferByIndex(2, "W_20_0xABCDABCD");
  cmdBuffer.updateBufferByIndex(3, "W_1_0xFFFFFFFF");
  cmdBuffer.clearAllBuffer();
  EXPECT_EQ(expected_ret, cmdBuffer.getBufferNameList());
}

TEST_F(BufferControlFixture, BufferUpdateToNextEmptyTest) {
  std::string expected_ret = "1_R_20,2_W_20_0xABCDABCD,3_W_1_0x11112222,4_W_10_"
                             "0xABCDABCD,5_W_99_0xBBBBAAAA,";
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("R_20"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_20_0xABCDABCD"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_1_0x11112222"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_10_0xABCDABCD"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_99_0xBBBBAAAA"));
  EXPECT_FALSE(cmdBuffer.updateToNextEmpty("W_99_0xBBBBAAAA"));
  EXPECT_EQ(expected_ret, cmdBuffer.getBufferNameList());
}

TEST_F(BufferControlFixture, BufferIsFullTest) {
  std::string expected_ret = "1_R_20,2_W_20_0xABCDABCD,3_W_1_0x11112222,4_W_10_"
                             "0xABCDABCD,5_W_99_0xBBBBAAAA,";
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("R_20"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_20_0xABCDABCD"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_1_0x11112222"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_10_0xABCDABCD"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_99_0xBBBBAAAA"));
  EXPECT_FALSE(cmdBuffer.updateToNextEmpty("W_99_0xBBBBAAAA"));

  EXPECT_TRUE(cmdBuffer.isBufferFull());
  EXPECT_EQ(expected_ret, cmdBuffer.getBufferNameList());
}

TEST_F(BufferControlFixture, BufferIsNotFullTest) {
  std::string expected_ret =
      "1_R_20,2_W_20_0xABCDABCD,3_W_1_0x11112222,4_empty,5_empty,";
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("R_20"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_20_0xABCDABCD"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_1_0x11112222"));

  EXPECT_FALSE(cmdBuffer.isBufferFull());
  EXPECT_EQ(expected_ret, cmdBuffer.getBufferNameList());
}

TEST_F(BufferControlFixture, BufferEmptyShiftTest) {

  std::string expected_ret =
      "1_E_10_10,2_W_1_0x11112222,3_W_99_0xBBBBAAAA,4_empty,5_empty,";

  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("E_10_10"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_20_0xABCDABCD"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_1_0x11112222"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_10_0xABCDABCD"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_99_0xBBBBAAAA"));

  cmdBuffer.clearBufferByIndex(2);
  cmdBuffer.clearBufferByIndex(4);
  cmdBuffer.emptyBufferShift();

  EXPECT_EQ(expected_ret, cmdBuffer.getBufferNameList());
}

TEST_F(BufferControlFixture, BufferGetParsingCmdTest) {
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("E_10_10"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_20_0xABCDABCD"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_1_0x11112222"));

  EXPECT_TRUE(cmdBuffer.getBufferCmd(1), "E");
  EXPECT_TRUE(cmdBuffer.getBufferCmd(2), "W");
  EXPECT_TRUE(cmdBuffer.getBufferCmd(3), "W");
}

TEST_F(BufferControlFixture, BufferGetParsingLbaTest) {
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("E_10_10"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_20_0xABCDABCD"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_1_0x11112222"));

  EXPECT_TRUE(cmdBuffer.getBufferLba(1), 10);
  EXPECT_TRUE(cmdBuffer.getBufferLba(2), 20);
  EXPECT_TRUE(cmdBuffer.getBufferLba(3), 1);
}

TEST_F(BufferControlFixture, BufferGetParsingValueTest) {
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("E_10_10"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_20_0xABCDABCD"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_1_0x11112222"));

  EXPECT_TRUE(cmdBuffer.getBufferLbaSize(1), 10);
  EXPECT_TRUE(cmdBuffer.getBufferValue(2), 0xABCDABCD);
  EXPECT_TRUE(cmdBuffer.getBufferValue(3), 0x11112222);
}

TEST_F(BufferControlFixture, BufferGetParsingInvalidIndexTest) {
  EXPECT_THROW(cmdBuffer.getBufferCmd(0), CmdBufferInvalidIdexException);
  EXPECT_THROW(cmdBuffer.getBufferCmd(-1), CmdBufferInvalidIdexException);
  EXPECT_THROW(cmdBuffer.getBufferCmd(10), CmdBufferInvalidIdexException);
}

TEST_F(BufferControlFixture, BufferContainReadValuePass1) {

  unsigned long expected_value = 0xFFFFFFFF;
  unsigned long get_value = 0x0;

  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_19_0x11112222"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("E_10_10"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_20_0xABCDABCD"));
  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_1_0x11112222"));

  EXPECT_TRUE(cmdBuffer.isBufferContainReadValue(19, get_value));
  EXPECT_EQ(0, get_value);

  EXPECT_TRUE(cmdBuffer.isBufferContainReadValue(1, get_value));
  EXPECT_EQ(0x11112222, get_value);

  EXPECT_TRUE(cmdBuffer.isBufferContainReadValue(20, get_value));
  EXPECT_EQ(0xABCDABCD, get_value);

  EXPECT_FALSE(cmdBuffer.isBufferContainReadValue(99, get_value));
}

TEST_F(BufferControlFixture, BufferRunCommand) {
  char *argv[5];
  argv[0] = const_cast<char *>("ssd.exe");
  argv[1] = const_cast<char *>("W");
  argv[2] = const_cast<char *>("10");
  argv[3] = const_cast<char *>("0x12345678");
  argv[4] = nullptr;
  cmdBuffer.runCommandBuffer(4, argv);

  argv[0] = const_cast<char *>("ssd.exe");
  argv[1] = const_cast<char *>("F");
  argv[2] = nullptr;
  argv[3] = nullptr;
  cmdBuffer.runCommandBuffer(2, argv);

  argv[0] = const_cast<char *>("ssd.exe");
  argv[1] = const_cast<char *>("R");
  argv[2] = const_cast<char *>("10");
  argv[3] = nullptr;
  cmdBuffer.runCommandBuffer(3, argv);

  checkOutputFileValid("0x12345678");
}

TEST_F(BufferControlFixture, BufferRunEraseCommand) {
  char *argv[5];
  argv[0] = const_cast<char *>("ssd.exe");
  argv[1] = const_cast<char *>("W");
  argv[2] = const_cast<char *>("10");
  argv[3] = const_cast<char *>("0x12345678");
  argv[4] = nullptr;
  cmdBuffer.runCommandBuffer(4, argv);

  argv[0] = const_cast<char *>("ssd.exe");
  argv[1] = const_cast<char *>("E");
  argv[2] = const_cast<char *>("10");
  argv[3] = const_cast<char *>("1");
  argv[4] = nullptr;


  cmdBuffer.runCommandBuffer(4, argv);
}

TEST_F(BufferControlFixture, BufferWriteRemoveCase) {
  std::string expected_ret =
      "1_W_5_0xAAAAAAAA,2_E_6_2,3_empty,4_empty,5_empty,";
  char *argv[5];
  argv[0] = const_cast<char *>("ssd.exe");
  argv[1] = const_cast<char *>("W");
  argv[2] = const_cast<char *>("5");
  argv[3] = const_cast<char *>("0xAAAAAAAA");
  argv[4] = nullptr;
  cmdBuffer.runCommandBuffer(4, argv);

  argv[0] = const_cast<char *>("ssd.exe");
  argv[1] = const_cast<char *>("W");
  argv[2] = const_cast<char *>("6");
  argv[3] = const_cast<char *>("0xAAAAAAAA");
  argv[4] = nullptr;
  cmdBuffer.runCommandBuffer(4, argv);

  argv[0] = const_cast<char *>("ssd.exe");
  argv[1] = const_cast<char *>("W");
  argv[2] = const_cast<char *>("7");
  argv[3] = const_cast<char *>("0xAAAAAAAA");
  argv[4] = nullptr;
  cmdBuffer.runCommandBuffer(4, argv);

  argv[0] = const_cast<char *>("ssd.exe");
  argv[1] = const_cast<char *>("E");
  argv[2] = const_cast<char *>("6");
  argv[3] = const_cast<char *>("2");
  argv[4] = nullptr;
  cmdBuffer.runCommandBuffer(4, argv);

  EXPECT_EQ(expected_ret, cmdBuffer.getBufferNameList());
}

TEST_F(BufferControlFixture, BufferDuplicatedEraseCommand) {
  std::string expected_ret = "1_E_10_15,2_empty,3_empty,4_empty,5_empty,";
  char *argv[5];
  argv[0] = const_cast<char *>("ssd.exe");
  argv[1] = const_cast<char *>("E");
  argv[2] = const_cast<char *>("10");
  argv[3] = const_cast<char *>("10");
  argv[4] = nullptr;
  cmdBuffer.runCommandBuffer(4, argv);

  argv[0] = const_cast<char *>("ssd.exe");
  argv[1] = const_cast<char *>("E");
  argv[2] = const_cast<char *>("15");
  argv[3] = const_cast<char *>("10");
  argv[4] = nullptr;
  cmdBuffer.runCommandBuffer(4, argv);

  EXPECT_EQ(expected_ret, cmdBuffer.getBufferNameList());
}

TEST_F(BufferControlFixture, BufferSequentialEraseCommand) {
  std::string expected_ret = "1_E_80_20,2_empty,3_empty,4_empty,5_empty,";
  char *argv[5];
  argv[0] = const_cast<char *>("ssd.exe");
  argv[1] = const_cast<char *>("E");
  argv[2] = const_cast<char *>("80");
  argv[3] = const_cast<char *>("10");
  argv[4] = nullptr;
  cmdBuffer.runCommandBuffer(4, argv);

  argv[0] = const_cast<char *>("ssd.exe");
  argv[1] = const_cast<char *>("E");
  argv[2] = const_cast<char *>("90");
  argv[3] = const_cast<char *>("10");
  argv[4] = nullptr;
  cmdBuffer.runCommandBuffer(4, argv);

  EXPECT_EQ(expected_ret, cmdBuffer.getBufferNameList());
}

TEST_F(BufferControlFixture, Buffer2timeDubplicaedEraseCommand) {
  std::string expected_ret;
  char *argv[5];
  argv[0] = const_cast<char *>("ssd.exe");
  argv[1] = const_cast<char *>("E");
  argv[2] = const_cast<char *>("10");
  argv[3] = const_cast<char *>("10");
  argv[4] = nullptr;
  cmdBuffer.runCommandBuffer(4, argv);

  argv[0] = const_cast<char *>("ssd.exe");
  argv[1] = const_cast<char *>("E");
  argv[2] = const_cast<char *>("25");
  argv[3] = const_cast<char *>("10");
  argv[4] = nullptr;
  cmdBuffer.runCommandBuffer(4, argv);

  expected_ret = "1_E_10_10,2_E_25_10,3_empty,4_empty,5_empty,";
  EXPECT_EQ(expected_ret, cmdBuffer.getBufferNameList());

  argv[0] = const_cast<char *>("ssd.exe");
  argv[1] = const_cast<char *>("E");
  argv[2] = const_cast<char *>("17");
  argv[3] = const_cast<char *>("10");
  argv[4] = nullptr;

  cmdBuffer.runCommandBuffer(4, argv); 

  expected_ret = "1_E_10_25,2_empty,3_empty,4_empty,5_empty,";
  EXPECT_EQ(expected_ret, cmdBuffer.getBufferNameList());
}

TEST_F(BufferControlFixture, BufferWrite1) {
  std::string expected_ret =
      "1_W_5_0xAAAAAAAA,2_W_6_0xAAAAAAAA,3_W_7_0xAAAAAAAA,4_empty,5_empty,";
  char *argv[5];
  argv[0] = const_cast<char *>("ssd.exe");
  argv[1] = const_cast<char *>("W");
  argv[2] = const_cast<char *>("5");
  argv[3] = const_cast<char *>("0xAAAAAAAA");
  argv[4] = nullptr;
  cmdBuffer.runCommandBuffer(4, argv);

  argv[0] = const_cast<char *>("ssd.exe");
  argv[1] = const_cast<char *>("W");
  argv[2] = const_cast<char *>("6");
  argv[3] = const_cast<char *>("0xAAAAAAAA");
  argv[4] = nullptr;
  cmdBuffer.runCommandBuffer(4, argv);

  argv[0] = const_cast<char *>("ssd.exe");
  argv[1] = const_cast<char *>("W");
  argv[2] = const_cast<char *>("7");
  argv[3] = const_cast<char *>("0xAAAAAAAA");
  argv[4] = nullptr;
  cmdBuffer.runCommandBuffer(4, argv);

  EXPECT_EQ(expected_ret, cmdBuffer.getBufferNameList());
}

TEST_F(BufferControlFixture, BufferWrite2) {
  std::string expected_ret =
      "1_W_5_0xAAAAAAAA,2_W_7_0xBBBBBBBB,3_empty,4_empty,5_empty,";
  char *argv[5];
  argv[0] = const_cast<char *>("ssd.exe");
  argv[1] = const_cast<char *>("W");
  argv[2] = const_cast<char *>("5");
  argv[3] = const_cast<char *>("0xAAAAAAAA");
  argv[4] = nullptr;
  cmdBuffer.runCommandBuffer(4, argv);

  argv[0] = const_cast<char *>("ssd.exe");
  argv[1] = const_cast<char *>("W");
  argv[2] = const_cast<char *>("7");
  argv[3] = const_cast<char *>("0xAAAAAAAA");
  argv[4] = nullptr;
  cmdBuffer.runCommandBuffer(4, argv);

  argv[0] = const_cast<char *>("ssd.exe");
  argv[1] = const_cast<char *>("W");
  argv[2] = const_cast<char *>("7");
  argv[3] = const_cast<char *>("0xBBBBBBBB");
  argv[4] = nullptr;
  cmdBuffer.runCommandBuffer(4, argv);

}

TEST_F(BufferControlFixture, BufferEraseOverSize) {
  char *argv[5];
  argv[0] = const_cast<char *>("ssd.exe");
  argv[1] = const_cast<char *>("E");
  argv[2] = const_cast<char *>("5");
  argv[3] = const_cast<char *>("15");
  argv[4] = nullptr;
  cmdBuffer.runCommandBuffer(4, argv);

  std::string expected_ret = "1_empty,2_empty,3_empty,4_empty,5_empty,";
  EXPECT_EQ(expected_ret, cmdBuffer.getBufferNameList());
}

TEST_F(BufferControlFixture, BufferEraseOverRange) {
  char *argv[5];
  argv[0] = const_cast<char *>("ssd.exe");
  argv[1] = const_cast<char *>("E");
  argv[2] = const_cast<char *>("99");
  argv[3] = const_cast<char *>("5");
  argv[4] = nullptr;
  cmdBuffer.runCommandBuffer(4, argv);

  std::string expected_ret = "1_empty,2_empty,3_empty,4_empty,5_empty,";
  EXPECT_EQ(expected_ret, cmdBuffer.getBufferNameList());
}

TEST_F(BufferControlFixture, BufferReadWithRunCommandPass) {

  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("W_19_0x11112222"));

  char *argv[5];
  argv[0] = const_cast<char *>("ssd.exe");
  argv[1] = const_cast<char *>("R");
  argv[2] = const_cast<char *>("19");
  argv[3] = nullptr;

  cmdBuffer.runCommandBuffer(3, argv);

  checkOutputFileValid("0x11112222");
}

TEST_F(BufferControlFixture, BufferReadWithRunCommandPass2) {

  EXPECT_TRUE(cmdBuffer.updateToNextEmpty("E_10_10"));

  char *argv[5];
  argv[0] = const_cast<char *>("ssd.exe");
  argv[1] = const_cast<char *>("R");
  argv[2] = const_cast<char *>("19");
  argv[3] = nullptr;

  cmdBuffer.runCommandBuffer(3, argv);

  checkOutputFileValid("0x00000000");
}

TEST_F(BufferControlFixture, BufferIntgTestWithRunCommandPass1) {

  char *argv[5];
  argv[0] = const_cast<char *>("ssd.exe");
  argv[1] = const_cast<char *>("R");
  argv[2] = const_cast<char *>("19");
  argv[3] = nullptr;

  cmdBuffer.runCommandBuffer(3, argv);

  checkOutputFileValid("0x00000000");

  argv[0] = const_cast<char *>("ssd.exe");
  argv[1] = const_cast<char *>("W");
  argv[2] = const_cast<char *>("7");
  argv[3] = const_cast<char *>("0xBBBBBBBB");
  argv[4] = nullptr;
  cmdBuffer.runCommandBuffer(4, argv);

  checkOutputFileValid("");
  checkSpecificLineInNandFile("0x00000000", 7);

  cmdBuffer.flush();
  checkOutputFileValid("");
  checkSpecificLineInNandFile("0xBBBBBBBB", 7);
}

TEST_F(BufferControlFixture, BufferIntgTestWithRunCommandPass2) {

  std::string expected_ret = "1_E_20_1,2_empty,3_empty,4_empty,5_empty,";
  char *argv[5];

  argv[0] = const_cast<char *>("ssd.exe");
  argv[1] = const_cast<char *>("W");
  argv[2] = const_cast<char *>("20");
  argv[3] = const_cast<char *>("0xABCDABCD");
  argv[4] = nullptr;

  cmdBuffer.runCommandBuffer(4, argv);

  checkOutputFileValid("");

  argv[0] = const_cast<char *>("ssd.exe");
  argv[1] = const_cast<char *>("W");
  argv[2] = const_cast<char *>("20");
  argv[3] = const_cast<char *>("0x12341234");
  argv[4] = nullptr;
  cmdBuffer.runCommandBuffer(4, argv);

  argv[0] = const_cast<char *>("ssd.exe");
  argv[1] = const_cast<char *>("E");
  argv[2] = const_cast<char *>("20");
  argv[3] = const_cast<char *>("1");
  argv[4] = nullptr;
  cmdBuffer.runCommandBuffer(4, argv);

  EXPECT_EQ(expected_ret, cmdBuffer.getBufferNameList());
}

TEST_F(BufferControlFixture, BufferIntgTestWithRunCommandPass3) {

  std::string expected_ret = "1_W_20_0xABCDABCD,2_E_10_5,3_empty,4_empty,5_empty,";
  char *argv[5];

  argv[0] = const_cast<char *>("ssd.exe");
  argv[1] = const_cast<char *>("W");
  argv[2] = const_cast<char *>("20");
  argv[3] = const_cast<char *>("0xABCDABCD");
  argv[4] = nullptr;
  cmdBuffer.runCommandBuffer(4, argv);

  checkOutputFileValid("");

  argv[0] = const_cast<char *>("ssd.exe");
  argv[1] = const_cast<char *>("E");
  argv[2] = const_cast<char *>("10");
  argv[3] = const_cast<char *>("4");
  argv[4] = nullptr;
  cmdBuffer.runCommandBuffer(4, argv);

  argv[0] = const_cast<char *>("ssd.exe");
  argv[1] = const_cast<char *>("E");
  argv[2] = const_cast<char *>("12");
  argv[3] = const_cast<char *>("3");
  argv[4] = nullptr;
  cmdBuffer.runCommandBuffer(4, argv);

  EXPECT_EQ(expected_ret, cmdBuffer.getBufferNameList());
}

TEST_F(BufferControlFixture, BufferFullWriteTestWithRunCommandPass) {

  std::string expected_ret =
      "1_W_5_0xABCDABCD,2_empty,3_empty,4_empty,5_empty,";
  char *argv[5];
  
  for (int i = 0; i < cmdBuffer.MAX_BUFFER_SIZE + 1; i++) {
    std::string value = std::to_string(i);

    std::vector<std::string> args = {"ssd.exe", "W", value, "0xABCDABCD"};

    std::vector<char *> argv;
    for (auto &s : args) {
      argv.push_back(const_cast<char *>(s.c_str()));
    }
    argv.push_back(nullptr);

    cmdBuffer.runCommandBuffer(4, argv.data());
  }
  for (int i = 0; i < cmdBuffer.MAX_BUFFER_SIZE; i++) {
    checkSpecificLineInNandFile("0xABCDABCD", i);
  }

  EXPECT_EQ(expected_ret, cmdBuffer.getBufferNameList());
}
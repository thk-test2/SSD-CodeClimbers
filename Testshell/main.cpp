#include "test_shell.h"
#include "SSD_INTERFACE.h"
#include "gmock/gmock.h"
#include "ssd_exe.cpp"

#include <random>

// stdout 캡처/해제 함수
using testing::internal::CaptureStdout;
using testing::internal::GetCapturedStdout;
using namespace testing;

// MockDriver
class MockSSD : public SSD_INTERFACE {
public:
  MOCK_METHOD(void, read, (int lba), (override));
  MOCK_METHOD(void, write, (int lba, unsigned long value), (override));
  MOCK_METHOD(string, getResult, (), (override));
};

class TestShellFixture : public ::testing::Test {
public:
  NiceMock<MockSSD> ssd;
  TestShell ts{&ssd};

  const string TEST_SCRIPT_1_FULLNAME = "1_FullWriteAndReadCompare";
  const string TEST_SCRIPT_1_SHORTCUT = "1_";
  const string TEST_SCRIPT_2_FULLNAME = "2_PartialLBAWrite";
  const string TEST_SCRIPT_2_SHORTCUT = "2_";
  const string TEST_SCRIPT_3_FULLNAME = "3_WriteReadAging";
  const string TEST_SCRIPT_3_SHORTCUT = "3_";

  string getCallHelpOutput() {
    CaptureStdout();
    ts.executeCommand(Command{"help", {}});
    return GetCapturedStdout();
  }
};

TEST_F(TestShellFixture, DisplaysCorrectHeader) {
  std::string output = getCallHelpOutput();

  EXPECT_TRUE(
      output.find("SSD Test Shell - Simple and Powerful SSD Testing Tool") !=
      std::string::npos);
}

TEST_F(TestShellFixture, DisplaysTeamMembers) {
  std::string output = getCallHelpOutput();

  EXPECT_TRUE(output.find("Team Members:") != std::string::npos);
  EXPECT_TRUE(output.find("Taehyun Kyong") != std::string::npos);
  EXPECT_TRUE(output.find("Sunghwan Kim") != std::string::npos);
  EXPECT_TRUE(output.find("Hyeonseok Sim") != std::string::npos);
  EXPECT_TRUE(output.find("Yerim Yun") != std::string::npos);
  EXPECT_TRUE(output.find("Hoenhwi Jeong") != std::string::npos);
  EXPECT_TRUE(output.find("Jeseong Kim") != std::string::npos);
}

TEST_F(TestShellFixture, DisplaysCommandsSection) {
  std::string output = getCallHelpOutput();

  EXPECT_TRUE(output.find("Commands:") != std::string::npos);

  EXPECT_TRUE(output.find("read") != std::string::npos);
  EXPECT_TRUE(output.find("read 10") != std::string::npos);

  EXPECT_TRUE(output.find("write") != std::string::npos);
  EXPECT_TRUE(output.find("write 5 0xFF") != std::string::npos);

  EXPECT_TRUE(output.find("fullread") != std::string::npos);
  EXPECT_TRUE(output.find("Read the entire SSD") != std::string::npos);

  EXPECT_TRUE(output.find("fullwrite") != std::string::npos);
  EXPECT_TRUE(output.find("fullwrite 0x00") != std::string::npos);

  EXPECT_TRUE(output.find("help") != std::string::npos);
  EXPECT_TRUE(output.find("Show this help message") != std::string::npos);

  EXPECT_TRUE(output.find("exit") != std::string::npos);
  EXPECT_TRUE(output.find("Exit the shell") != std::string::npos);
}

TEST_F(TestShellFixture, DisplayTestScriptsSection) {
  std::string output = getCallHelpOutput();

  EXPECT_TRUE(output.find("Test Scripts:") != std::string::npos);
  EXPECT_TRUE(output.find("1_FullWriteAndReadCompare") != std::string::npos);
  EXPECT_TRUE(output.find("2_PartialLBAWrite") != std::string::npos);
  EXPECT_TRUE(output.find("3_WriteReadAging") != std::string::npos);
}

TEST_F(TestShellFixture, ReadNormalCase) {
  Command command{"read", vector<string>{"0"}};

  EXPECT_CALL(ssd, read(_)).Times(1);
  EXPECT_CALL(ssd, getResult()).Times(1).WillOnce(Return("0xAAAABBBB"));

  ts.executeCommand(command);
}

TEST_F(TestShellFixture, ReadInvalidCase) {
  Command OutOfLbaCmd{"read", vector<string>{"100"}};
  EXPECT_CALL(ssd, read(_)).Times(1);
  EXPECT_CALL(ssd, getResult()).Times(1).WillOnce(Return("ERROR"));

  ts.executeCommand(OutOfLbaCmd);

  Command InvalidFormatCmd{"read", vector<string>{"s", "2"}};
  EXPECT_CALL(ssd, read(_)).Times(0);
  EXPECT_CALL(ssd, getResult()).Times(0);

  ts.executeCommand(InvalidFormatCmd);

  Command OverflowLbaCmd{"read", vector<string>{"2147483648"}};
  EXPECT_CALL(ssd, read(_)).Times(0);
  EXPECT_CALL(ssd, getResult()).Times(0);

  ts.executeCommand(OverflowLbaCmd);
}

TEST_F(TestShellFixture, WriteNormalCase) {
  Command command{"write", vector<string>{"0", "0xAAAABBBB"}};

  EXPECT_CALL(ssd, write(_, _)).Times(1);
  EXPECT_CALL(ssd, getResult()).Times(1).WillOnce(Return(""));

  ts.executeCommand(command);
}

TEST_F(TestShellFixture, WriteInvalidCase) {
  Command OutOfLbaCmd{"write", vector<string>{"100", "0xAAAABBBB"}};
  EXPECT_CALL(ssd, write(_, _)).Times(1);
  EXPECT_CALL(ssd, getResult()).Times(1).WillOnce(Return("ERROR"));

  ts.executeCommand(OutOfLbaCmd);

  Command InvalidFormatCmd{"write", vector<string>{"s", "2"}};
  EXPECT_CALL(ssd, write(_, _)).Times(0);
  EXPECT_CALL(ssd, getResult()).Times(0);

  ts.executeCommand(InvalidFormatCmd);

  Command OverflowLbaCmd{"write", vector<string>{"2147483648", "0xAAAABBBB"}};
  EXPECT_CALL(ssd, write(_, _)).Times(0);
  EXPECT_CALL(ssd, getResult()).Times(0);

  ts.executeCommand(OverflowLbaCmd);

  Command OverflowValueCmd{"write", vector<string>{"0", "0xAAAABBBBAAAABBBB"}};
  EXPECT_CALL(ssd, write(_, _)).Times(0);
  EXPECT_CALL(ssd, getResult()).Times(0);

  ts.executeCommand(OverflowValueCmd);
}

TEST_F(TestShellFixture, FullReadNormalCase) {
  Command command{"fullread", vector<string>{}};
  
  EXPECT_CALL(ssd, read(0)).Times(1);
  EXPECT_CALL(ssd, read(1)).Times(1);
  EXPECT_CALL(ssd, read(2)).Times(1);
  EXPECT_CALL(ssd, read(3)).Times(1);

  EXPECT_CALL(ssd, getResult())
      .Times(4)
      .WillOnce(Return("0xAAAABBBB"))
      .WillOnce(Return("0xCCCCDDDD"))
      .WillOnce(Return("0xEEEEFFFF"))
      .WillOnce(Return("ERROR"));

  CaptureStdout();
  ts.executeCommand(command);
  std::string output = GetCapturedStdout();

  EXPECT_TRUE(output.find("[Full Read] LBA: 0 Result: 0xAAAABBBB") !=
              std::string::npos);
  EXPECT_TRUE(output.find("[Full Read] LBA: 1 Result: 0xCCCCDDDD") !=
              std::string::npos);
  EXPECT_TRUE(output.find("[Full Read] LBA: 2 Result: 0xEEEEFFFF") !=
              std::string::npos);
}

TEST_F(TestShellFixture, FullReadInvalidUsage) {
  Command command{"fullread", vector<string>{"extra"}};
  
  EXPECT_CALL(ssd, read(_)).Times(0);
  EXPECT_CALL(ssd, getResult()).Times(0);

  CaptureStdout();
  ts.executeCommand(command);
  std::string output = GetCapturedStdout();
  EXPECT_EQ("INVALID COMMAND\n", output);
}

// fullwrite 테스트들
TEST_F(TestShellFixture, FullWriteNormalCase) {
  Command command{"fullwrite", vector<string>{"0xABCDABCD"}};

  // 3개의 LBA에 성공적으로 쓰고 4번째에서 ERROR 반환
  EXPECT_CALL(ssd, write(0, 0xABCDABCD)).Times(1);
  EXPECT_CALL(ssd, write(1, 0xABCDABCD)).Times(1);
  EXPECT_CALL(ssd, write(2, 0xABCDABCD)).Times(1);
  EXPECT_CALL(ssd, write(3, 0xABCDABCD)).Times(1);

  EXPECT_CALL(ssd, getResult())
      .Times(4)
      .WillOnce(Return(""))
      .WillOnce(Return(""))
      .WillOnce(Return(""))
      .WillOnce(Return("ERROR"));

  CaptureStdout();
  ts.executeCommand(command);
  std::string output = GetCapturedStdout();

  EXPECT_TRUE(output.find("[Full Write] LBA: 0 Done") !=
              std::string::npos);
  EXPECT_TRUE(output.find("[Full Write] LBA: 1 Done") !=
              std::string::npos);
  EXPECT_TRUE(output.find("[Full Write] LBA: 2 Done") !=
              std::string::npos);

  // 4번째 LBA는 ERROR이므로 출력되지 않아야 함
  EXPECT_TRUE(output.find("[Full Write] LBA: 3") == std::string::npos);
}

TEST_F(TestShellFixture, FullWriteInvalidArgumentCount) {
  Command command{"fullwrite", vector<string>{"0xABCDABCD", "extraArgs"}};

  EXPECT_CALL(ssd, write(_, _)).Times(0);
  EXPECT_CALL(ssd, getResult()).Times(0);

  CaptureStdout();
  ts.executeCommand(command);
  std::string output = GetCapturedStdout();

  EXPECT_EQ("INVALID COMMAND\n", output);
}

TEST_F(TestShellFixture, FullWriteNoArguments) {
  Command command{"fullwrite", vector<string>{}};

  EXPECT_CALL(ssd, write(_, _)).Times(0);
  EXPECT_CALL(ssd, getResult()).Times(0);

  CaptureStdout();
  ts.executeCommand(command);
  std::string output = GetCapturedStdout();

  EXPECT_EQ("INVALID COMMAND\n", output);
}

TEST_F(TestShellFixture, FullWriteInvalidValue) {
  Command command{"fullwrite", vector<string>{"invalid_number"}};

  EXPECT_CALL(ssd, write(_, _)).Times(0);
  EXPECT_CALL(ssd, getResult()).Times(0);

  CaptureStdout();
  ts.executeCommand(command);
  std::string output = GetCapturedStdout();

  EXPECT_EQ("INVALID COMMAND\n", output);
}

TEST_F(TestShellFixture, ExitNormally) {
  Command cmd{"exit", {}};
  EXPECT_THROW(ts.executeCommand(cmd), std::exception);
}

TEST_F(TestShellFixture, ExitInvalidArgs) {
  Command cmd{"exit", {"..."}};
  CaptureStdout();
  EXPECT_NO_THROW(ts.executeCommand(cmd));
  std::string output = GetCapturedStdout();
}

TEST_F(TestShellFixture, TestScript1FAIL) {
  Command cmd{TEST_SCRIPT_1_FULLNAME};

  EXPECT_CALL(ssd, write(_, 0xAAAABBBB)).Times(AtLeast(1));
  EXPECT_CALL(ssd, getResult()).WillRepeatedly(Return("0xFFFFFFFF"));

  CaptureStdout();
  ts.executeCommand(cmd);
  std::string output = GetCapturedStdout();

  EXPECT_TRUE(output.find("Script 1 execution failed.\n") != std::string::npos);
}

TEST_F(TestShellFixture, TestScript1ShortcutFAIL) {
  Command cmd{TEST_SCRIPT_1_SHORTCUT};

  EXPECT_CALL(ssd, write(_, 0xAAAABBBB)).Times(AtLeast(1));
  EXPECT_CALL(ssd, getResult()).WillRepeatedly(Return("0xFFFFFFFF"));

  CaptureStdout();
  ts.executeCommand(cmd);
  std::string output = GetCapturedStdout();

  EXPECT_TRUE(output.find("Script 1 execution failed.\n") != std::string::npos);
}

TEST_F(TestShellFixture, TestScript1SUCCESS) {
  Command cmd{TEST_SCRIPT_1_FULLNAME};

  EXPECT_CALL(ssd, write(_, 0xAAAABBBB)).Times(100);
  EXPECT_CALL(ssd, getResult()).WillRepeatedly(Return("0xAAAABBBB"));

  CaptureStdout();
  ts.executeCommand(cmd);
  std::string output = GetCapturedStdout();

  EXPECT_EQ("Script 1 executed successfully.\n", output);
}

TEST_F(TestShellFixture, TestScript1ShortcutSUCCESS) {
  Command cmd{TEST_SCRIPT_1_SHORTCUT};

  EXPECT_CALL(ssd, write(_, 0xAAAABBBB)).Times(100);
  EXPECT_CALL(ssd, getResult()).WillRepeatedly(Return("0xAAAABBBB"));

  CaptureStdout();
  ts.executeCommand(cmd);
  std::string output = GetCapturedStdout();

  EXPECT_EQ("Script 1 executed successfully.\n", output);
}

TEST_F(TestShellFixture, TestScript2FAIL) {
  Command cmd{TEST_SCRIPT_2_FULLNAME};

  EXPECT_CALL(ssd, write(_, 0xAAAABBBB)).Times(AtLeast(1));
  EXPECT_CALL(ssd, getResult()).WillRepeatedly(Return("0xFFFFFFFF"));

  CaptureStdout();
  ts.executeCommand(cmd);
  std::string output = GetCapturedStdout();

  EXPECT_TRUE(output.find("Script 2 execution failed.\n") != std::string::npos);
}

TEST_F(TestShellFixture, TestScript2ShortcutFAIL) {
  Command cmd{TEST_SCRIPT_2_SHORTCUT};

  EXPECT_CALL(ssd, write(_, 0xAAAABBBB)).Times(AtLeast(1));
  EXPECT_CALL(ssd, getResult()).WillRepeatedly(Return("0xFFFFFFFF"));

  CaptureStdout();
  ts.executeCommand(cmd);
  std::string output = GetCapturedStdout();

  EXPECT_TRUE(output.find("Script 2 execution failed.\n") != std::string::npos);
}

TEST_F(TestShellFixture, TestScript2SUCCESS) {
  Command cmd{TEST_SCRIPT_2_FULLNAME};

  EXPECT_CALL(ssd, write(_, 0xAAAABBBB)).Times(150);
  EXPECT_CALL(ssd, getResult()).WillRepeatedly(Return("0xAAAABBBB"));

  CaptureStdout();
  ts.executeCommand(cmd);
  std::string output = GetCapturedStdout();

  EXPECT_EQ("Script 2 executed successfully.\n", output);
}

TEST_F(TestShellFixture, TestScript2ShortcutSUCCESS) {
  Command cmd{TEST_SCRIPT_2_SHORTCUT};

  EXPECT_CALL(ssd, write(_, 0xAAAABBBB)).Times(150);
  EXPECT_CALL(ssd, getResult()).WillRepeatedly(Return("0xAAAABBBB"));

  CaptureStdout();
  ts.executeCommand(cmd);
  std::string output = GetCapturedStdout();

  EXPECT_EQ("Script 2 executed successfully.\n", output);
}

TEST_F(TestShellFixture, TestScript3FAIL) {
  Command cmd{TEST_SCRIPT_3_FULLNAME};

  EXPECT_CALL(ssd, write(_, _)).Times(AtLeast(1));
  EXPECT_CALL(ssd, read(_)).Times(AtLeast(1));

  CaptureStdout();
  ts.executeCommand(cmd);
  std::string output = GetCapturedStdout();

  EXPECT_TRUE(output.find("Script 3 execution failed.\n") != std::string::npos);
}

TEST_F(TestShellFixture, TestScript3ShortcutFAIL) {
  Command cmd{TEST_SCRIPT_3_SHORTCUT};

  EXPECT_CALL(ssd, write(_, _)).Times(AtLeast(1));
  EXPECT_CALL(ssd, read(_)).Times(AtLeast(1));

  CaptureStdout();
  ts.executeCommand(cmd);
  std::string output = GetCapturedStdout();

  EXPECT_TRUE(output.find("Script 3 execution failed.\n") != std::string::npos);
}

TEST_F(TestShellFixture, TestScript3SUCCESS) {
  Command cmd{TEST_SCRIPT_3_FULLNAME};

  EXPECT_CALL(ssd, write(_, _)).Times(400);
  EXPECT_CALL(ssd, read(_)).Times(400);
  EXPECT_CALL(ssd, getResult()).WillRepeatedly(Return(""));

  CaptureStdout();
  ts.executeCommand(cmd);
  std::string output = GetCapturedStdout();

  EXPECT_EQ("Script 3 executed successfully.\n", output);
}

TEST_F(TestShellFixture, TestScript3ShortcutSUCCESS) {
  Command cmd{TEST_SCRIPT_3_SHORTCUT};

  EXPECT_CALL(ssd, write(_, _)).Times(400);
  EXPECT_CALL(ssd, read(_)).Times(400);
  EXPECT_CALL(ssd, getResult()).WillRepeatedly(Return(""));

  CaptureStdout();
  ts.executeCommand(cmd);
  std::string output = GetCapturedStdout();

  EXPECT_EQ("Script 3 executed successfully.\n", output);
}

TEST_F(TestShellFixture, InvalidCommand) {
  Command cmd{"INVALID"};

  CaptureStdout();
  ts.executeCommand(cmd);
  std::string output = GetCapturedStdout();

  EXPECT_EQ("INVALID COMMAND\n", output);
}

int main() {
#ifdef _DEBUG
  ::testing::InitGoogleTest();
  return RUN_ALL_TESTS();
#else
  SSD_EXE ssd;
  TestShell testShell{&ssd};
  testShell.run();
  return 0;
#endif
}
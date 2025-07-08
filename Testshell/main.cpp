#include "gmock/gmock.h"
#include "test_shell.cpp"

// stdout 캡처/해제 함수
using testing::internal::CaptureStdout;
using testing::internal::GetCapturedStdout;
using namespace testing;

class TestShellFixture : public ::testing::Test {
public:
  NiceMock<MockSSD> ssd;
  TestShell ts{&ssd};

  const string TEST_SCRIPT_1_FULLNAME = "1_FullWriteAndReadCompare";
  const string TEST_SCRIPT_1_SHORTCUT = "1_";
};

class TestShellHelpTest : public ::testing::Test {
protected:
  void SetUp() override {
    original_cout_buffer = std::cout.rdbuf();
    std::cout.rdbuf(captured_output.rdbuf());
  }

  void TearDown() override { std::cout.rdbuf(original_cout_buffer); }

  std::string GetCapturedOutput() { return captured_output.str(); }

  void CallHelp() { shell.help(); }

  TestShell shell;
  std::stringstream captured_output;
  std::streambuf *original_cout_buffer;
};

TEST_F(TestShellHelpTest, DisplaysCorrectHeader) {
  CallHelp();
  std::string output = GetCapturedOutput();

  EXPECT_TRUE(
    output.find("SSD Test Shell - Simple and Powerful SSD Testing Tool") !=
    std::string::npos);
}

TEST_F(TestShellHelpTest, DisplaysTeamMembers) {
  CallHelp();
  std::string output = GetCapturedOutput();

  EXPECT_TRUE(output.find("Team Members:") != std::string::npos);
  EXPECT_TRUE(output.find("Taehyun Kyong") != std::string::npos);
  EXPECT_TRUE(output.find("Sunghwan Kim") != std::string::npos);
  EXPECT_TRUE(output.find("Hyeonseok Sim") != std::string::npos);
  EXPECT_TRUE(output.find("Yerim Yun") != std::string::npos);
  EXPECT_TRUE(output.find("Hoenhwi Jeong") != std::string::npos);
  EXPECT_TRUE(output.find("Jeseong Kim") != std::string::npos);
}

TEST_F(TestShellHelpTest, DisplaysCommandsSection) {
  CallHelp();
  std::string output = GetCapturedOutput();

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

TEST_F(TestShellHelpTest, DisplayTestScriptsSection) {
  CallHelp();
  std::string output = GetCapturedOutput();

  EXPECT_TRUE(output.find("Test Scripts:") != std::string::npos);
  EXPECT_TRUE(output.find("1_FullWriteAndReadCompare") != std::string::npos);
  EXPECT_TRUE(output.find("2_PartialLBAWrite") != std::string::npos);
  EXPECT_TRUE(output.find("3_WriteReadAging") != std::string::npos);
}

TEST_F(TestShellFixture, ReadNormalCase) {
  Command command{"read", vector<string>{"0"}};

  EXPECT_CALL(ssd, read(_)).Times(1);

  EXPECT_CALL(ssd, getResult())
    .Times(1)
    .WillOnce(Return("0xAAAABBBB"));

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
  vector<string> args{};
  // Total LBA 수를 3인 경우로 가정
  // 3개의 LBA를 성공적으로 읽고 4번째에서 ERROR 반환
  EXPECT_CALL(ssd, read(0))
    .Times(1);
  EXPECT_CALL(ssd, read(1))
    .Times(1);
  EXPECT_CALL(ssd, read(2))
    .Times(1);
  EXPECT_CALL(ssd, read(3))
    .Times(1);

  EXPECT_CALL(ssd, getResult())
    .Times(4)
    .WillOnce(Return("0xAAAABBBB"))
    .WillOnce(Return("0xCCCCDDDD"))
    .WillOnce(Return("0xEEEEFFFF"))
    .WillOnce(Return("ERROR"));

  CaptureStdout();
  ts.fullread(args);
  std::string output = GetCapturedStdout();

  EXPECT_TRUE(output.find("[Full Read] LBA: 0 Result: 0xAAAABBBB") !=
              std::string::npos);
  EXPECT_TRUE(output.find("[Full Read] LBA: 1 Result: 0xCCCCDDDD") !=
              std::string::npos);
  EXPECT_TRUE(output.find("[Full Read] LBA: 2 Result: 0xEEEEFFFF") !=
              std::string::npos);
}

TEST_F(TestShellFixture, FullReadInvalidUsage) {
  vector<string> args{"extra"};
  EXPECT_CALL(ssd, read(_))
    .Times(0);
  EXPECT_CALL(ssd, getResult())
    .Times(0);

  CaptureStdout();
  ts.fullread(args);
  std::string output = GetCapturedStdout();
  EXPECT_EQ("INVALID COMMAND\n", output);
}

// fullwrite 테스트들
TEST_F(TestShellFixture, FullWriteNormalCase) {
  vector<string> args{"42"};

  // 3개의 LBA에 성공적으로 쓰고 4번째에서 ERROR 반환
  EXPECT_CALL(ssd, write(0, 42))
    .Times(1);
  EXPECT_CALL(ssd, write(1, 42))
    .Times(1);
  EXPECT_CALL(ssd, write(2, 42))
    .Times(1);
  EXPECT_CALL(ssd, write(3, 42))
    .Times(1);

  EXPECT_CALL(ssd, getResult())
    .Times(4)
    .WillOnce(Return("SUCCESS"))
    .WillOnce(Return("SUCCESS"))
    .WillOnce(Return("SUCCESS"))
    .WillOnce(Return("ERROR"));

  CaptureStdout();
  ts.fullwrite(args);
  std::string output = GetCapturedStdout();

  EXPECT_TRUE(output.find("[Full Write] LBA: 0 Value: 42") !=
              std::string::npos);
  EXPECT_TRUE(output.find("[Full Write] LBA: 1 Value: 42") !=
              std::string::npos);
  EXPECT_TRUE(output.find("[Full Write] LBA: 2 Value: 42") !=
              std::string::npos);

  // 4번째 LBA는 ERROR이므로 출력되지 않아야 함
  EXPECT_TRUE(output.find("[Full Write] LBA: 3") == std::string::npos);
}

TEST_F(TestShellFixture, FullWriteInvalidArgumentCount) {
  vector<string> args{"42", "extraArgs"};

  EXPECT_CALL(ssd, write(_, _))
    .Times(0);
  EXPECT_CALL(ssd, getResult())
    .Times(0);

  CaptureStdout();
  ts.fullwrite(args);
  std::string output = GetCapturedStdout();

  EXPECT_EQ("INVALID COMMAND\n", output);
}

TEST_F(TestShellFixture, FullWriteNoArguments) {
  vector<string> args{};

  EXPECT_CALL(ssd, write(_, _))
    .Times(0);
  EXPECT_CALL(ssd, getResult())
    .Times(0);

  CaptureStdout();
  ts.fullwrite(args);
  std::string output = GetCapturedStdout();

  EXPECT_EQ("INVALID COMMAND\n", output);
}

TEST_F(TestShellFixture, FullWriteInvalidValue) {
  vector<string> args{"invalid_number"};

  EXPECT_CALL(ssd, write(_, _))
    .Times(0);
  EXPECT_CALL(ssd, getResult())
    .Times(0);

  CaptureStdout();
  ts.fullwrite(args);
  std::string output = GetCapturedStdout();

  EXPECT_EQ("INVALID COMMAND\n", output);
}

TEST_F(TestShellFixture, TestScript1FAIL) {
  Command cmd{TEST_SCRIPT_1_FULLNAME, {"0xAAAABBBB"}};

  EXPECT_CALL(ssd, write(_, 0xAAAABBBB)).Times(AtLeast(1));
  EXPECT_CALL(ssd, getResult()).WillRepeatedly(Return("0xFFFFFFFF"));

  CaptureStdout();
  ts.executeCommand(cmd);
  std::string output = GetCapturedStdout();

  EXPECT_EQ("Script 1 execution failed.\n", output);
}

TEST_F(TestShellFixture, TestScript1ShortcutFAIL) {
  Command cmd{TEST_SCRIPT_1_SHORTCUT, {"0xAAAABBBB"}};

  EXPECT_CALL(ssd, write(_, 0xAAAABBBB)).Times(AtLeast(1));
  EXPECT_CALL(ssd, getResult()).WillRepeatedly(Return("0xFFFFFFFF"));

  CaptureStdout();
  ts.executeCommand(cmd);
  std::string output = GetCapturedStdout();

  EXPECT_EQ("Script 1 execution failed.\n", output);
}

TEST_F(TestShellFixture, TestScript1SUCCESS) {
  Command cmd{TEST_SCRIPT_1_FULLNAME, {"0xAAAABBBB"}};

  EXPECT_CALL(ssd, write(_, 0xAAAABBBB)).Times(100);
  EXPECT_CALL(ssd, getResult()).WillRepeatedly(Return("0xAAAABBBB"));

  CaptureStdout();
  ts.executeCommand(cmd);
  std::string output = GetCapturedStdout();

  EXPECT_EQ("Script 1 executed successfully.\n", output);
}

TEST_F(TestShellFixture, TestScript1ShortcutSUCCESS) {
  Command cmd{TEST_SCRIPT_1_SHORTCUT, {"0xAAAABBBB"}};

  EXPECT_CALL(ssd, write(_, 0xAAAABBBB)).Times(100);
  EXPECT_CALL(ssd, getResult()).WillRepeatedly(Return("0xAAAABBBB"));

  CaptureStdout();
  ts.executeCommand(cmd);
  std::string output = GetCapturedStdout();

  EXPECT_EQ("Script 1 executed successfully.\n", output);
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
  TestShell testShell;
  testShell.run();
  return 0;
#endif
}
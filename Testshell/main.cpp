#include "gmock/gmock.h"
#include "test_shell.cpp"

// stdout 캡처/해제 함수
using testing::internal::CaptureStdout;
using testing::internal::GetCapturedStdout;
using namespace testing;

class TestShellFixture: public ::testing::Test {
public:
  NiceMock<MockSSD> ssd;
  TestShell ts{&ssd};
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

TEST_F(TestShellFixture, ReadInvalidLbaCase) {
  Command command{"read", vector<string>{"100"}};

  EXPECT_CALL(ssd, read(_)).Times(1);
  
  EXPECT_CALL(ssd, getResult())
	  .Times(1)
	  .WillOnce(Return("ERROR"));

  ts.executeCommand(command);
}

TEST_F(TestShellFixture, ReadInvalidUsage) {
  Command command{"read", vector<string>{"s", "2"}};

  EXPECT_CALL(ssd, read(_)).Times(0);

  EXPECT_CALL(ssd, getResult()).Times(0);

  ts.executeCommand(command);
}

TEST_F(TestShellFixture, ReadLbaOverThanMaxOfInt) {
  int MAX_INT = 2147483647; // Maximum value for a 32-bit signed integer
  Command command{"read", vector<string>{"2147483648"}};

  EXPECT_CALL(ssd, read(_)).Times(0);

  EXPECT_CALL(ssd, getResult()).Times(0);

  ts.executeCommand(command);
}

TEST_F(TestShellFixture, TestScript1FAIL) {
  Command cmd{"1_FullWriteAndReadCompare", {"0xAAAABBBB"}};

  EXPECT_CALL(ssd, write(_, 0xAAAABBBB)).Times(AtLeast(1));
  EXPECT_CALL(ssd, getResult()).WillRepeatedly(Return("0xFFFFFFFF"));

  CaptureStdout();
  ts.executeCommand(cmd);
  std::string output = GetCapturedStdout();

  EXPECT_EQ("Script 1 execution failed.\n", output);
}

TEST_F(TestShellFixture, TestScript1ShortcutFAIL) {
  Command cmd{"1_", {"0xAAAABBBB"}};

  EXPECT_CALL(ssd, write(_, 0xAAAABBBB)).Times(AtLeast(1));
  EXPECT_CALL(ssd, getResult()).WillRepeatedly(Return("0xFFFFFFFF"));

  CaptureStdout();
  ts.executeCommand(cmd);
  std::string output = GetCapturedStdout();

  EXPECT_EQ("Script 1 execution failed.\n", output);
}

TEST_F(TestShellFixture, TestScript1SUCCESS) {
  Command cmd{"1_FullWriteAndReadCompare", {"0xAAAABBBB"}};

  EXPECT_CALL(ssd, write(_, 0xAAAABBBB)).Times(100);
  EXPECT_CALL(ssd, getResult()).WillRepeatedly(Return("0xAAAABBBB"));

  CaptureStdout();
  ts.executeCommand(cmd);
  std::string output = GetCapturedStdout();

  EXPECT_EQ("Script 1 executed successfully.\n", output);
}

TEST_F(TestShellFixture, TestScript1ShortcutSUCCESS) {
  Command cmd{"1_", {"0xAAAABBBB"}};

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

#include "gmock/gmock.h"
#include "test_shell.cpp"

// stdout 캡처/해제 함수
using testing::internal::CaptureStdout;
using testing::internal::GetCapturedStdout;

TEST(SampleTest, HandlesTrue) {
  EXPECT_TRUE(true);
}

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

TEST_F(TestShellHelpTest, DisplaysTeamMembers) {
  CallHelp();
  std::string output = GetCapturedOutput();

  EXPECT_TRUE(output.find("Team Members:") != std::string::npos);
  EXPECT_TRUE(output.find("Taehyeon Kyung") != std::string::npos);
  EXPECT_TRUE(output.find("Sunghwan Kim") != std::string::npos);
  EXPECT_TRUE(output.find("Hyeonseok Sim") != std::string::npos);
  EXPECT_TRUE(output.find("Yerim Yun") != std::string::npos);
  EXPECT_TRUE(output.find("Hoenhwi Jeong") != std::string::npos);
  EXPECT_TRUE(output.find("Jeseong Kim") != std::string::npos);
}

TEST(TestShell, InvalidCommand) {
  TestShell ts;
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

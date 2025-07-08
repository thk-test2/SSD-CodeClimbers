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

TEST(TestShell, TestScript1FAIL) {
  TestShell ts;
  Command cmd{"1_FullWriteAndReadCompare"};

  CaptureStdout();
  ts.executeCommand(cmd);
  std::string output = GetCapturedStdout();

  EXPECT_EQ("Script 1 execution failed.", output);
}

TEST(TestShell, TestScript1SUCCESS) {
  TestShell ts;
  Command cmd{"1_FullWriteAndReadCompare"};

  CaptureStdout();
  ts.executeCommand(cmd);
  std::string output = GetCapturedStdout();

  EXPECT_EQ("Script 1 executed successfully.", output);
}

TEST(TestShell, TestScript1ShortcutFAIL) {
  TestShell ts;
  Command cmd{"1_"};

  CaptureStdout();
  ts.executeCommand(cmd);
  std::string output = GetCapturedStdout();

  EXPECT_EQ("Script 1 execution failed.", output);
}

TEST(TestShell, TestScript1ShortcutSUCCESS) {
  TestShell ts;
  Command cmd{"1_"};

  CaptureStdout();
  ts.executeCommand(cmd);
  std::string output = GetCapturedStdout();

  EXPECT_EQ("Script 1 executed successfully.", output);
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

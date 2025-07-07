#include "gmock/gmock.h"
#include "test_shell.cpp"

// stdout 캡처/해제 함수
using testing::internal::CaptureStdout;
using testing::internal::GetCapturedStdout;
using namespace testing;

TEST(SampleTest, HandlesTrue) {
  EXPECT_TRUE(true);
}

TEST(TestShell, InvalidCommand) {
  TestShell ts;
  Command cmd{"INVALID"};
  
  CaptureStdout();
  ts.executeCommand(cmd);
  std::string output = GetCapturedStdout();

  EXPECT_EQ("INVALID COMMAND\n", output);
}

TEST(Read, NomalCase) {
  NiceMock<MockSSD> ssd;
  TestShell ts{&ssd};
  Command command{"read", vector<string>{"0"}};

  EXPECT_CALL(ssd, read(_)).Times(1).WillRepeatedly(Return(true));

  EXPECT_CALL(ssd, getResult()).Times(1).WillRepeatedly(Return("0xAAAABBBB"));

  ts.executeCommand(command);
}

TEST(Read, InvalidLbaCase) {
  NiceMock<MockSSD> ssd;
  TestShell ts{&ssd};
  Command command{"read", vector<string>{"100"}};

  EXPECT_CALL(ssd, read(_)).Times(1).WillRepeatedly(Return(true));

  EXPECT_CALL(ssd, getResult()).Times(1).WillRepeatedly(Return("ERROR"));

  ts.executeCommand(command);
}

TEST(Read, InvalidUsage) {
  NiceMock<MockSSD> ssd;
  TestShell ts{&ssd};
  Command command{"read", vector<string>{"s", "2"}};

  EXPECT_CALL(ssd, read(_)).Times(0);

  EXPECT_CALL(ssd, getResult()).Times(0);

  ts.executeCommand(command);
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

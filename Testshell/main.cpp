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


TEST(SampleTest, HandlesTrue) {
  EXPECT_TRUE(true);
}

TEST_F(TestShellFixture, InvalidCommand) {
  Command cmd{"INVALID"};
  
  CaptureStdout();
  ts.executeCommand(cmd);
  std::string output = GetCapturedStdout();

  EXPECT_EQ("INVALID COMMAND\n", output);
}

TEST_F(TestShellFixture, ReadNomalCase) {
  Command command{"read", vector<string>{"0"}};

  EXPECT_CALL(ssd, read(_))
	  .Times(1)
	  .WillRepeatedly(Return(true));

  EXPECT_CALL(ssd, getResult())
	  .Times(1)
	  .WillRepeatedly(Return("0xAAAABBBB"));

  ts.executeCommand(command);
}

TEST_F(TestShellFixture, ReadInvalidLbaCase) {
  Command command{"read", vector<string>{"100"}};

  EXPECT_CALL(ssd, read(_))
	  .Times(1)
	  .WillRepeatedly(Return(true));
  
  EXPECT_CALL(ssd, getResult())
	  .Times(1)
	  .WillRepeatedly(Return("ERROR"));

  ts.executeCommand(command);
}

TEST_F(TestShellFixture, ReadInvalidUsage) {
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

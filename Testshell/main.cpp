#include <fstream>

#include "test_shell.h"
#include "ssd_adaptor.h"
#include "gmock/gmock.h"

// stdout 캡처/해제 함수
using testing::internal::CaptureStdout;
using testing::internal::GetCapturedStdout;
using namespace testing;

// MockDriver
class MockSsdAdaptor : public SSD_INTERFACE {
public:
  MOCK_METHOD(void, read, (int lba), (override));
  MOCK_METHOD(void, write, (int lba, unsigned long value), (override));
  MOCK_METHOD(void, erase, (int lba, int size), (override));
  MOCK_METHOD(void, flush, (), (override));
  MOCK_METHOD(string, getResult, (), (override));
};

class TestShellFixture : public ::testing::Test {
public:
  NiceMock<MockSsdAdaptor> ssd;
  TestShell ts{&ssd};

  string randomValue = convertHexToString(getRandomValue());

  const string TEST_SCRIPT_1_FULLNAME = "1_FullWriteAndReadCompare";
  const string TEST_SCRIPT_1_SHORTCUT = "1_";
  const string TEST_SCRIPT_2_FULLNAME = "2_PartialLBAWrite";
  const string TEST_SCRIPT_2_SHORTCUT = "2_";
  const string TEST_SCRIPT_3_FULLNAME = "3_WriteReadAging";
  const string TEST_SCRIPT_3_SHORTCUT = "3_";
  const string TEST_SCRIPT_4_FULLNAME = "4_EraseAndWriteAging";
  const string TEST_SCRIPT_4_SHORTCUT = "4_";

  const string SCRIPT_FAIL = "FAIL!";
  const string SCRIPT_PASS = "Pass";

  const string INVALID_COMMAND = "INVALID COMMAND\n";

  string getCallHelpOutput() {
    CaptureStdout();
    ts.getCommandHandler()->executeCommand(CommandLine{"help", {}});
    return GetCapturedStdout();
  }

  string getCommandOutput(const CommandLine &command) {
    CaptureStdout();
    ts.getCommandHandler()->executeCommand(command);
    return GetCapturedStdout();
  }

  void checkExpectedStrInOutput(const std::string &output,
                                const std::string &expectedStr) {
    EXPECT_TRUE(output.find(expectedStr) != std::string::npos)
        << "Expected string not found in output: " << expectedStr << endl
        << "Actual string: " << output << endl;
  }

  void checkExpectedStrsInOutput(const std::string &output,
                                 const vector<std::string> &expectedStrs) {
    for (auto &expectedStr : expectedStrs)
      EXPECT_TRUE(output.find(expectedStr) != std::string::npos)
          << "Expected string not found in output: " << expectedStr << endl
          << "Actual string: " << output << endl;
  }
};

TEST_F(TestShellFixture, DisplaysCorrectHeader) {
  checkExpectedStrInOutput(
      getCallHelpOutput(),
      "SSD Test Shell - Simple and Powerful SSD Testing Tool");
}

TEST_F(TestShellFixture, DisplaysTeamMembers) {
  vector<string> expectedMembers = {"Taehyun Kyong", "Sunghwan Kim",
                                    "Hyeonseok Sim", "Yerim Yun",
                                    "Hoenhwi Jeong", "Jeseong Kim"};
  checkExpectedStrsInOutput(getCallHelpOutput(), expectedMembers);
}

TEST_F(TestShellFixture, DisplaysCommandsSection) {
  vector<string> expectedCommands = {"Commands:",
                                     "read",
                                     "read 10",
                                     "write",
                                     "write 5 0xFF",
                                     "fullread",
                                     "Read the entire SSD",
                                     "fullwrite",
                                     "fullwrite 0x00",
                                     "help",
                                     "Show this help message",
                                     "exit",
                                     "Exit the shell",
                                     "erase",
                                     "erase 0 10",
                                     "erase_range",
                                     "erase_range 0 99",
                                     "flush",
                                     "Flush all buffered commands to SSD"};
  checkExpectedStrsInOutput(getCallHelpOutput(), expectedCommands);
}

TEST_F(TestShellFixture, DisplayTestScriptsSection) {
  vector<string> expectedScripts = {"1_FullWriteAndReadCompare",
                                    "2_PartialLBAWrite", "3_WriteReadAging",
                                    "4_EraseAndWriteAging"};
  checkExpectedStrsInOutput(getCallHelpOutput(), expectedScripts);
}

TEST_F(TestShellFixture, ReadNormalCase) {
  CommandLine command{"read", vector<string>{"0"}};

  EXPECT_CALL(ssd, read(_)).Times(1);
  EXPECT_CALL(ssd, getResult()).Times(1).WillOnce(Return("0xAAAABBBB"));

  ts.getCommandHandler()->executeCommand(command);
}

TEST_F(TestShellFixture, ReadInvalidCase) {
  CommandLine OutOfLbaCmd{"read", vector<string>{"100"}};
  EXPECT_CALL(ssd, read(_)).Times(1);
  EXPECT_CALL(ssd, getResult()).Times(1).WillOnce(Return("ERROR"));

  ts.getCommandHandler()->executeCommand(OutOfLbaCmd);

  CommandLine InvalidFormatCmd{"read", vector<string>{"s", "2"}};
  EXPECT_CALL(ssd, read(_)).Times(0);
  EXPECT_CALL(ssd, getResult()).Times(0);

  ts.getCommandHandler()->executeCommand(InvalidFormatCmd);

  CommandLine OverflowLbaCmd{"read", vector<string>{"2147483648"}};
  EXPECT_CALL(ssd, read(_)).Times(0);
  EXPECT_CALL(ssd, getResult()).Times(0);

  ts.getCommandHandler()->executeCommand(OverflowLbaCmd);
}

TEST_F(TestShellFixture, WriteNormalCase) {
  CommandLine command{"write", vector<string>{"0", "0xAAAABBBB"}};

  EXPECT_CALL(ssd, write(_, _)).Times(1);
  EXPECT_CALL(ssd, getResult()).Times(1).WillOnce(Return(""));

  ts.getCommandHandler()->executeCommand(command);
}

TEST_F(TestShellFixture, WriteInvalidCase) {
  CommandLine OutOfLbaCmd{"write", vector<string>{"100", "0xAAAABBBB"}};
  EXPECT_CALL(ssd, write(_, _)).Times(1);
  EXPECT_CALL(ssd, getResult()).Times(1).WillOnce(Return("ERROR"));

  ts.getCommandHandler()->executeCommand(OutOfLbaCmd);

  CommandLine InvalidFormatCmd{"write", vector<string>{"s", "2"}};
  EXPECT_CALL(ssd, write(_, _)).Times(0);
  EXPECT_CALL(ssd, getResult()).Times(0);

  ts.getCommandHandler()->executeCommand(InvalidFormatCmd);

  CommandLine OverflowLbaCmd{"write",
                             vector<string>{"2147483648", "0xAAAABBBB"}};
  EXPECT_CALL(ssd, write(_, _)).Times(0);
  EXPECT_CALL(ssd, getResult()).Times(0);

  ts.getCommandHandler()->executeCommand(OverflowLbaCmd);

  CommandLine OverflowValueCmd{"write",
                               vector<string>{"0", "0xAAAABBBBAAAABBBB"}};
  EXPECT_CALL(ssd, write(_, _)).Times(0);
  EXPECT_CALL(ssd, getResult()).Times(0);

  ts.getCommandHandler()->executeCommand(OverflowValueCmd);
}

TEST_F(TestShellFixture, EraseNormalCase) {
  CommandLine command{"erase", vector<string>{"0", "12"}};

  EXPECT_CALL(ssd, erase(_, _)).Times(2);
  EXPECT_CALL(ssd, getResult()).Times(2).WillRepeatedly(Return(""));

  ts.getCommandHandler()->executeCommand(command);
}

TEST_F(TestShellFixture, EraseInvalidCase) {

  vector<CommandLine> commands = {
      {"erase", vector<string>{"-1", "10"}},
      {"erase", vector<string>{"99", "10"}},
      {"erase", vector<string>{"10", "0"}},
      {"erase", vector<string>{"10", "101"}},
  };
  for (auto command : commands) {
    EXPECT_CALL(ssd, erase(_, _)).Times(0);
    EXPECT_CALL(ssd, getResult()).Times(0);

    checkExpectedStrInOutput(getCommandOutput(command), INVALID_COMMAND);
  }
}

TEST_F(TestShellFixture, EraseRangeNormalCase) {
  CommandLine command{"erase_range", vector<string>{"0", "12"}};

  EXPECT_CALL(ssd, erase(_, _)).Times(2);
  EXPECT_CALL(ssd, getResult()).Times(2).WillRepeatedly(Return(""));

  ts.getCommandHandler()->executeCommand(command);
}

TEST_F(TestShellFixture, EraseRangeInvalidCase) {

  vector<CommandLine> commands = {
      {"erase_range", vector<string>{"-1", "10"}},
      {"erase_range", vector<string>{"99", "10"}},
      {"erase_range", vector<string>{"10", "0"}},
      {"erase_range", vector<string>{"10", "101"}},
  };
  for (auto command : commands) {
    EXPECT_CALL(ssd, erase(_, _)).Times(0);
    EXPECT_CALL(ssd, getResult()).Times(0);

    checkExpectedStrInOutput(getCommandOutput(command), INVALID_COMMAND);
  }
}

TEST_F(TestShellFixture, FlushNormalCase) {
  CommandLine command{"flush", vector<string>{}};

  EXPECT_CALL(ssd, flush()).Times(1);
  EXPECT_CALL(ssd, getResult()).Times(1).WillOnce(Return(""));

  checkExpectedStrInOutput(getCommandOutput(command), "[Flush] Done\n");
}

TEST_F(TestShellFixture, FlushErrorCase) {
  CommandLine command{"flush", vector<string>{}};

  EXPECT_CALL(ssd, flush()).Times(1);
  EXPECT_CALL(ssd, getResult()).Times(1).WillOnce(Return("ERROR"));

  checkExpectedStrInOutput(getCommandOutput(command), "[Flush] ERROR\n");
}

TEST_F(TestShellFixture, FlushInvalidUsage) {
  vector<CommandLine> commands = {{"flush", vector<string>{"extra_arg"}},
                                  {"flush", vector<string>{"arg1", "arg2"}}};
  for (auto command : commands) {
    EXPECT_CALL(ssd, flush()).Times(0);
    EXPECT_CALL(ssd, getResult()).Times(0);

    checkExpectedStrInOutput(getCommandOutput(command), INVALID_COMMAND);
  }
}

TEST_F(TestShellFixture, FullReadSuccessCase) {
  CommandLine command{"fullread", vector<string>{}};

  EXPECT_CALL(ssd, read(_)).Times(100);
  EXPECT_CALL(ssd, getResult()).Times(100).WillRepeatedly(Return("0xAAAABBBB"));

  checkExpectedStrInOutput(getCommandOutput(command), "[Full Read] Done\n");
}

TEST_F(TestShellFixture, FullReadFailCase) {
  CommandLine command{"fullread", vector<string>{}};

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

  vector<string> expectedOutput = {"LBA 00 : 0xAAAABBBB", "LBA 01 : 0xCCCCDDDD",
                                   "LBA 02 : 0xEEEEFFFF"};
  checkExpectedStrsInOutput(getCommandOutput(command), expectedOutput);
}

TEST_F(TestShellFixture, FullReadInvalidUsage) {
  CommandLine command{"fullread", vector<string>{"extra"}};

  EXPECT_CALL(ssd, read(_)).Times(0);
  EXPECT_CALL(ssd, getResult()).Times(0);

  checkExpectedStrInOutput(getCommandOutput(command), INVALID_COMMAND);
}

// fullwrite 테스트들
TEST_F(TestShellFixture, FullWriteSuccessCase) {
  CommandLine command{"fullwrite", vector<string>{"0xABCDABCD"}};

  // 3개의 LBA에 성공적으로 쓰고 4번째에서 ERROR 반환
  EXPECT_CALL(ssd, write(_, _)).Times(100);
  EXPECT_CALL(ssd, getResult()).Times(100).WillRepeatedly(Return("0xABCDABCD"));

  checkExpectedStrInOutput(getCommandOutput(command), "[Full Write] Done");
}

TEST_F(TestShellFixture, FullWriteFailCase) {
  CommandLine command{"fullwrite", vector<string>{"0xABCDABCD"}};

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

  checkExpectedStrInOutput(getCommandOutput(command), "[Full Write] Failed");
}

TEST_F(TestShellFixture, FullWriteInvalidArgumentCount) {
  vector<CommandLine> commands = {
      {"fullwrite", vector<string>{"0xABCDABCD", "extraArgs"}},
      {"fullwrite", vector<string>{}},
      {"fullwrite", vector<string>{"invalid_number"}}};
  for (auto command : commands) {
    EXPECT_CALL(ssd, write(_, _)).Times(0);
    EXPECT_CALL(ssd, getResult()).Times(0);

    checkExpectedStrInOutput(getCommandOutput(command), INVALID_COMMAND);
  }
}

TEST_F(TestShellFixture, ExitNormally) {
  CommandLine cmd{"exit", {}};
  EXPECT_THROW(ts.getCommandHandler()->executeCommand(cmd), std::exception);
}

TEST_F(TestShellFixture, ExitInvalidArgs) {
  CommandLine cmd{"exit", {"..."}};
  CaptureStdout();
  EXPECT_NO_THROW(ts.getCommandHandler()->executeCommand(cmd));
  std::string output = GetCapturedStdout();
}

TEST_F(TestShellFixture, TestScript1_2FAIL) {
  vector<CommandLine> commands = {
      {TEST_SCRIPT_1_FULLNAME, {}},
      {TEST_SCRIPT_1_SHORTCUT, {}},
      {TEST_SCRIPT_2_FULLNAME, {}},
      {TEST_SCRIPT_2_SHORTCUT, {}},
  };
  for (auto command : commands) {
    EXPECT_CALL(ssd, write(_, 0xAAAABBBB)).Times(AtLeast(1));
    EXPECT_CALL(ssd, getResult()).WillRepeatedly(Return("0xFFFFFFFF"));

    checkExpectedStrInOutput(getCommandOutput(command), SCRIPT_FAIL);
  }
}

TEST_F(TestShellFixture, TestScript1_2SUCCESS) {
  vector<CommandLine> commands = {
      {TEST_SCRIPT_1_FULLNAME, {}},
      {TEST_SCRIPT_1_SHORTCUT, {}},
      {TEST_SCRIPT_2_FULLNAME, {}},
  };
  for (auto command : commands) {
    EXPECT_CALL(ssd, write(_, 0xAAAABBBB)).Times(AtLeast(1));
    EXPECT_CALL(ssd, getResult()).WillRepeatedly(Return("0xAAAABBBB"));

    checkExpectedStrInOutput(getCommandOutput(command), SCRIPT_PASS);
  }
}

TEST_F(TestShellFixture, TestScript3FAIL) {
  CommandLine cmd{TEST_SCRIPT_3_FULLNAME, {randomValue}};

  EXPECT_CALL(ssd, write(_, _)).Times(AtLeast(1));
  EXPECT_CALL(ssd, read(_)).Times(AtLeast(1));

  CaptureStdout();
  ts.getCommandHandler()->executeCommand(cmd);
  std::string output = GetCapturedStdout();

  EXPECT_TRUE(output.find(SCRIPT_FAIL) != std::string::npos);
}

TEST_F(TestShellFixture, TestScript3ShortcutFAIL) {
  CommandLine cmd{TEST_SCRIPT_3_SHORTCUT, {randomValue}};

  EXPECT_CALL(ssd, write(_, _)).Times(AtLeast(1));
  EXPECT_CALL(ssd, read(_)).Times(AtLeast(1));

  checkExpectedStrInOutput(getCommandOutput(cmd), SCRIPT_FAIL);
}

TEST_F(TestShellFixture, TestScript3SUCCESS) {
  CommandLine cmd{TEST_SCRIPT_3_FULLNAME, {randomValue}};

  EXPECT_CALL(ssd, write(_, _)).Times(400);
  EXPECT_CALL(ssd, read(_)).Times(400);
  EXPECT_CALL(ssd, getResult()).WillRepeatedly(Return(randomValue));

  checkExpectedStrInOutput(getCommandOutput(cmd), SCRIPT_PASS);
}

TEST_F(TestShellFixture, TestScript3ShortcutSUCCESS) {
  CommandLine cmd{TEST_SCRIPT_3_SHORTCUT, {randomValue}};

  EXPECT_CALL(ssd, write(_, _)).Times(400);
  EXPECT_CALL(ssd, read(_)).Times(400);
  EXPECT_CALL(ssd, getResult()).WillRepeatedly(Return(randomValue));

  checkExpectedStrInOutput(getCommandOutput(cmd), SCRIPT_PASS);
}

TEST_F(TestShellFixture, TestScript4FAIL) {
  vector<CommandLine> commands = {{TEST_SCRIPT_4_FULLNAME, {}},
                                  {TEST_SCRIPT_4_SHORTCUT, {}}};

  for (auto cmd : commands) {
    EXPECT_CALL(ssd, erase(_, _)).Times(AtLeast(1));
    EXPECT_CALL(ssd, write(_, _)).Times(AtLeast(1));
    EXPECT_CALL(ssd, getResult())
        .Times(AtLeast(1))
        .WillOnce(Return(""))
        .WillOnce(Return("ERROR"));

    checkExpectedStrInOutput(getCommandOutput(cmd), SCRIPT_FAIL);
  }
}

TEST_F(TestShellFixture, TestScript4SUCCESS) {
  vector<CommandLine> commands = {{TEST_SCRIPT_4_FULLNAME, {}},
                                  {TEST_SCRIPT_4_SHORTCUT, {}}};

  for (auto cmd : commands) {
    EXPECT_CALL(ssd, erase(_, _)).Times(49 * 30 + 1);
    EXPECT_CALL(ssd, write(_, _)).Times(98 * 30);
    EXPECT_CALL(ssd, getResult()).WillRepeatedly(Return(""));

    checkExpectedStrInOutput(getCommandOutput(cmd), SCRIPT_PASS);
  }
}

TEST_F(TestShellFixture, InvalidCommand) {
  CommandLine cmd{"INVALID"};

  checkExpectedStrInOutput(getCommandOutput(cmd), INVALID_COMMAND);
}

std::vector<std::string> getScripts(const std::string &filename) {
  std::ifstream in(filename);
  std::vector<std::string> result;
  if (!in) {
    std::cerr << "파일 열기 실패: " << filename << "\n";
    return result;
  }
  std::string script;
  while (std::getline(in, script)) {
    result.push_back(script);
  }
  return result;
}

int main(int argc, char *argv[]) {
#ifdef _DEBUG
  ::testing::InitGoogleTest();
  return RUN_ALL_TESTS();
#else
  SsdExeAdaptor ssd;
  TestShell testShell{&ssd};

  if (argc > 1) {
    testShell.setShellScripts(getScripts(argv[1]));
  }
  testShell.run();
  return 0;
#endif
}

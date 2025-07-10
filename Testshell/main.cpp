#include <fstream>

#include "test_shell.h"
#include "gmock/gmock.h"

// stdout 캡처/해제 함수
using testing::internal::CaptureStdout;
using testing::internal::GetCapturedStdout;
using namespace testing;

// MockDriver
class MockSSD : public SSD_INTERFACE {
public:
  MOCK_METHOD(void, read, (int lba), (override));
  MOCK_METHOD(void, write, (int lba, unsigned long value), (override));
  MOCK_METHOD(void, erase, (int lba, int size), (override));
  MOCK_METHOD(void, flush, (), (override));
  MOCK_METHOD(string, getResult, (), (override));
};

class TestShellFixture : public ::testing::Test {
public:
  NiceMock<MockSSD> ssd;
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

  string getCallHelpOutput() {
    CaptureStdout();
    ts.getCommandHandler()->executeCommand(CommandLine{"help", {}});
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

  EXPECT_TRUE(output.find("erase") != std::string::npos);
  EXPECT_TRUE(output.find("erase 0 10") != std::string::npos);

  EXPECT_TRUE(output.find("erase_range") != std::string::npos);
  EXPECT_TRUE(output.find("erase_range 0 99") != std::string::npos);

  EXPECT_TRUE(output.find("flush") != std::string::npos);
  EXPECT_TRUE(output.find("Flush all buffered commands to SSD") != std::string::npos);
}

TEST_F(TestShellFixture, DisplayTestScriptsSection) {
  std::string output = getCallHelpOutput();

  EXPECT_TRUE(output.find("Test Scripts:") != std::string::npos);
  EXPECT_TRUE(output.find("1_FullWriteAndReadCompare") != std::string::npos);
  EXPECT_TRUE(output.find("2_PartialLBAWrite") != std::string::npos);
  EXPECT_TRUE(output.find("3_WriteReadAging") != std::string::npos);
  EXPECT_TRUE(output.find("4_EraseAndWriteAging") != std::string::npos);
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

  CommandLine OverflowLbaCmd{"write", vector<string>{"2147483648", "0xAAAABBBB"}};
  EXPECT_CALL(ssd, write(_, _)).Times(0);
  EXPECT_CALL(ssd, getResult()).Times(0);

  ts.getCommandHandler()->executeCommand(OverflowLbaCmd);

  CommandLine OverflowValueCmd{"write", vector<string>{"0", "0xAAAABBBBAAAABBBB"}};
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

    CaptureStdout();
    ts.getCommandHandler()->executeCommand(command);
    std::string output = GetCapturedStdout();

    EXPECT_EQ("INVALID COMMAND\n", output);
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

    CaptureStdout();
    ts.getCommandHandler()->executeCommand(command);
    std::string output = GetCapturedStdout();

    EXPECT_EQ("INVALID COMMAND\n", output);
  }
}

TEST_F(TestShellFixture, FlushNormalCase) {
  CommandLine command{"flush", vector<string>{}};

  EXPECT_CALL(ssd, flush()).Times(1);
  EXPECT_CALL(ssd, getResult()).Times(1).WillOnce(Return(""));

  CaptureStdout();
  ts.getCommandHandler()->executeCommand(command);
  std::string output = GetCapturedStdout();

  EXPECT_EQ("[Flush] Done\n", output);
}

TEST_F(TestShellFixture, FlushErrorCase) {
  CommandLine command{"flush", vector<string>{}};

  EXPECT_CALL(ssd, flush()).Times(1);
  EXPECT_CALL(ssd, getResult()).Times(1).WillOnce(Return("ERROR"));

  CaptureStdout();
  ts.getCommandHandler()->executeCommand(command);
  std::string output = GetCapturedStdout();

  EXPECT_EQ("[Flush] ERROR\n", output);
}

TEST_F(TestShellFixture, FlushInvalidUsage) {
  CommandLine command{"flush", vector<string>{"extra_arg"}};

  EXPECT_CALL(ssd, flush()).Times(0);
  EXPECT_CALL(ssd, getResult()).Times(0);

  CaptureStdout();
  ts.getCommandHandler()->executeCommand(command);
  std::string output = GetCapturedStdout();

  EXPECT_EQ("INVALID COMMAND\n", output);
}

TEST_F(TestShellFixture, FlushMultipleArgsInvalidUsage) {
  CommandLine command{"flush", vector<string>{"arg1", "arg2"}};

  EXPECT_CALL(ssd, flush()).Times(0);
  EXPECT_CALL(ssd, getResult()).Times(0);

  CaptureStdout();
  ts.getCommandHandler()->executeCommand(command);
  std::string output = GetCapturedStdout();

  EXPECT_EQ("INVALID COMMAND\n", output);
}

TEST_F(TestShellFixture, FullReadSuccessCase) {
  CommandLine command{"fullread", vector<string>{}};

  EXPECT_CALL(ssd, read(_)).Times(100);
  EXPECT_CALL(ssd, getResult()).Times(100).WillRepeatedly(Return("0xAAAABBBB"));

  CaptureStdout();
  ts.getCommandHandler()->executeCommand(command);
  std::string output = GetCapturedStdout();

  EXPECT_TRUE(output.find("[Full Read] Done != std::string::npos"));
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

  CaptureStdout();
  ts.getCommandHandler()->executeCommand(command);
  std::string output = GetCapturedStdout();

  EXPECT_TRUE(output.find("LBA 00 : 0xAAAABBBB") != std::string::npos);
  EXPECT_TRUE(output.find("LBA 01 : 0xCCCCDDDD") != std::string::npos);
  EXPECT_TRUE(output.find("LBA 02 : 0xEEEEFFFF") != std::string::npos);
}

TEST_F(TestShellFixture, FullReadInvalidUsage) {
  CommandLine command{"fullread", vector<string>{"extra"}};

  EXPECT_CALL(ssd, read(_)).Times(0);
  EXPECT_CALL(ssd, getResult()).Times(0);

  CaptureStdout();
  ts.getCommandHandler()->executeCommand(command);
  std::string output = GetCapturedStdout();
  EXPECT_EQ("INVALID COMMAND\n", output);
}

// fullwrite 테스트들
TEST_F(TestShellFixture, FullWriteSuccessCase) {
  CommandLine command{"fullwrite", vector<string>{"0xABCDABCD"}};

  // 3개의 LBA에 성공적으로 쓰고 4번째에서 ERROR 반환
  EXPECT_CALL(ssd, write(_, _)).Times(100);
  EXPECT_CALL(ssd, getResult()).Times(100).WillRepeatedly(Return("0xABCDABCD"));

  CaptureStdout();
  ts.getCommandHandler()->executeCommand(command);
  std::string output = GetCapturedStdout();

  EXPECT_TRUE(output.find("[Full Write] Done") != std::string::npos);
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

  CaptureStdout();
  ts.getCommandHandler()->executeCommand(command);
  std::string output = GetCapturedStdout();

  EXPECT_TRUE(output.find("[Full Write] Failed") != std::string::npos);
}

TEST_F(TestShellFixture, FullWriteInvalidArgumentCount) {
  CommandLine command{"fullwrite", vector<string>{"0xABCDABCD", "extraArgs"}};

  EXPECT_CALL(ssd, write(_, _)).Times(0);
  EXPECT_CALL(ssd, getResult()).Times(0);

  CaptureStdout();
  ts.getCommandHandler()->executeCommand(command);
  std::string output = GetCapturedStdout();

  EXPECT_EQ("INVALID COMMAND\n", output);
}

TEST_F(TestShellFixture, FullWriteNoArguments) {
  CommandLine command{"fullwrite", vector<string>{}};

  EXPECT_CALL(ssd, write(_, _)).Times(0);
  EXPECT_CALL(ssd, getResult()).Times(0);

  CaptureStdout();
  ts.getCommandHandler()->executeCommand(command);
  std::string output = GetCapturedStdout();

  EXPECT_EQ("INVALID COMMAND\n", output);
}

TEST_F(TestShellFixture, FullWriteInvalidValue) {
  CommandLine command{"fullwrite", vector<string>{"invalid_number"}};

  EXPECT_CALL(ssd, write(_, _)).Times(0);
  EXPECT_CALL(ssd, getResult()).Times(0);

  CaptureStdout();
  ts.getCommandHandler()->executeCommand(command);
  std::string output = GetCapturedStdout();

  EXPECT_EQ("INVALID COMMAND\n", output);
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

TEST_F(TestShellFixture, TestScript1FAIL) {
  CommandLine cmd{TEST_SCRIPT_1_FULLNAME};

  EXPECT_CALL(ssd, write(_, 0xAAAABBBB)).Times(AtLeast(1));
  EXPECT_CALL(ssd, getResult()).WillRepeatedly(Return("0xFFFFFFFF"));

  CaptureStdout();
  ts.getCommandHandler()->executeCommand(cmd);
  std::string output = GetCapturedStdout();

  EXPECT_TRUE(output.find("FAIL!\n") != std::string::npos);
}

TEST_F(TestShellFixture, TestScript1ShortcutFAIL) {
  CommandLine cmd{TEST_SCRIPT_1_SHORTCUT};

  EXPECT_CALL(ssd, write(_, 0xAAAABBBB)).Times(AtLeast(1));
  EXPECT_CALL(ssd, getResult()).WillRepeatedly(Return("0xFFFFFFFF"));

  CaptureStdout();
  ts.getCommandHandler()->executeCommand(cmd);
  std::string output = GetCapturedStdout();

  EXPECT_TRUE(output.find("FAIL!\n") != std::string::npos);
}

TEST_F(TestShellFixture, TestScript1SUCCESS) {
  CommandLine cmd{TEST_SCRIPT_1_FULLNAME};

  EXPECT_CALL(ssd, write(_, 0xAAAABBBB)).Times(100);
  EXPECT_CALL(ssd, getResult()).WillRepeatedly(Return("0xAAAABBBB"));

  CaptureStdout();
  ts.getCommandHandler()->executeCommand(cmd);
  std::string output = GetCapturedStdout();

  EXPECT_EQ("Pass\n", output);
}

TEST_F(TestShellFixture, TestScript1ShortcutSUCCESS) {
  CommandLine cmd{TEST_SCRIPT_1_SHORTCUT};

  EXPECT_CALL(ssd, write(_, 0xAAAABBBB)).Times(100);
  EXPECT_CALL(ssd, getResult()).WillRepeatedly(Return("0xAAAABBBB"));

  CaptureStdout();
  ts.getCommandHandler()->executeCommand(cmd);
  std::string output = GetCapturedStdout();

  EXPECT_EQ("Pass\n", output);
}

TEST_F(TestShellFixture, TestScript2FAIL) {
  CommandLine cmd{TEST_SCRIPT_2_FULLNAME};

  EXPECT_CALL(ssd, write(_, 0xAAAABBBB)).Times(AtLeast(1));
  EXPECT_CALL(ssd, getResult()).WillRepeatedly(Return("0xFFFFFFFF"));

  CaptureStdout();
  ts.getCommandHandler()->executeCommand(cmd);
  std::string output = GetCapturedStdout();

  EXPECT_TRUE(output.find("FAIL!\n") != std::string::npos);
}

TEST_F(TestShellFixture, TestScript2ShortcutFAIL) {
  CommandLine cmd{TEST_SCRIPT_2_SHORTCUT};

  EXPECT_CALL(ssd, write(_, 0xAAAABBBB)).Times(AtLeast(1));
  EXPECT_CALL(ssd, getResult()).WillRepeatedly(Return("0xFFFFFFFF"));

  CaptureStdout();
  ts.getCommandHandler()->executeCommand(cmd);
  std::string output = GetCapturedStdout();

  EXPECT_TRUE(output.find("FAIL!\n") != std::string::npos);
}

TEST_F(TestShellFixture, TestScript2SUCCESS) {
  CommandLine cmd{TEST_SCRIPT_2_FULLNAME};

  EXPECT_CALL(ssd, write(_, 0xAAAABBBB)).Times(150);
  EXPECT_CALL(ssd, getResult()).WillRepeatedly(Return("0xAAAABBBB"));

  CaptureStdout();
  ts.getCommandHandler()->executeCommand(cmd);
  std::string output = GetCapturedStdout();

  EXPECT_EQ("Pass\n", output);
}

TEST_F(TestShellFixture, TestScript2ShortcutSUCCESS) {
  CommandLine cmd{TEST_SCRIPT_2_SHORTCUT};

  EXPECT_CALL(ssd, write(_, 0xAAAABBBB)).Times(150);
  EXPECT_CALL(ssd, getResult()).WillRepeatedly(Return("0xAAAABBBB"));

  CaptureStdout();
  ts.getCommandHandler()->executeCommand(cmd);
  std::string output = GetCapturedStdout();

  EXPECT_EQ("Pass\n", output);
}

TEST_F(TestShellFixture, TestScript3FAIL) {
  CommandLine cmd{TEST_SCRIPT_3_FULLNAME, {randomValue}};

  EXPECT_CALL(ssd, write(_, _)).Times(AtLeast(1));
  EXPECT_CALL(ssd, read(_)).Times(AtLeast(1));

  CaptureStdout();
  ts.getCommandHandler()->executeCommand(cmd);
  std::string output = GetCapturedStdout();

  EXPECT_TRUE(output.find("FAIL!\n") != std::string::npos);
}

TEST_F(TestShellFixture, TestScript3ShortcutFAIL) {
  CommandLine cmd{TEST_SCRIPT_3_SHORTCUT, {randomValue}};

  EXPECT_CALL(ssd, write(_, _)).Times(AtLeast(1));
  EXPECT_CALL(ssd, read(_)).Times(AtLeast(1));

  CaptureStdout();
  ts.getCommandHandler()->executeCommand(cmd);
  std::string output = GetCapturedStdout();

  EXPECT_TRUE(output.find("FAIL!\n") != std::string::npos);
}

TEST_F(TestShellFixture, TestScript3SUCCESS) {
  CommandLine cmd{TEST_SCRIPT_3_FULLNAME, {randomValue}};

  EXPECT_CALL(ssd, write(_, _)).Times(400);
  EXPECT_CALL(ssd, read(_)).Times(400);
  EXPECT_CALL(ssd, getResult()).WillRepeatedly(Return(randomValue));

  CaptureStdout();
  ts.getCommandHandler()->executeCommand(cmd);
  std::string output = GetCapturedStdout();

  EXPECT_EQ("Pass\n", output);
}

TEST_F(TestShellFixture, TestScript3ShortcutSUCCESS) {
  CommandLine cmd{TEST_SCRIPT_3_SHORTCUT, {randomValue}};

  EXPECT_CALL(ssd, write(_, _)).Times(400);
  EXPECT_CALL(ssd, read(_)).Times(400);
  EXPECT_CALL(ssd, getResult()).WillRepeatedly(Return(randomValue));

  CaptureStdout();
  ts.getCommandHandler()->executeCommand(cmd);
  std::string output = GetCapturedStdout();

  EXPECT_EQ("Pass\n", output);
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

    CaptureStdout();
    ts.getCommandHandler()->executeCommand(cmd);
    std::string output = GetCapturedStdout();

    EXPECT_TRUE(output.find("FAIL!\n") != std::string::npos);
  }
}

TEST_F(TestShellFixture, TestScript4SUCCESS) {
  vector<CommandLine> commands = {{TEST_SCRIPT_4_FULLNAME, {}},
                              {TEST_SCRIPT_4_SHORTCUT, {}}};

  for (auto cmd : commands) {
    EXPECT_CALL(ssd, erase(_, _)).Times(49 * 30 + 1);
    EXPECT_CALL(ssd, write(_, _)).Times(98 * 30);
    EXPECT_CALL(ssd, getResult()).WillRepeatedly(Return(""));

    CaptureStdout();
    ts.getCommandHandler()->executeCommand(cmd);
    std::string output = GetCapturedStdout();

    EXPECT_EQ("Pass\n", output);
  }
}

TEST_F(TestShellFixture, InvalidCommand) {
  CommandLine cmd{"INVALID"};

  CaptureStdout();
  ts.getCommandHandler()->executeCommand(cmd);
  std::string output = GetCapturedStdout();

  EXPECT_EQ("INVALID COMMAND\n", output);
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
  SSD_EXE ssd;
  TestShell testShell{&ssd};

  if (argc > 1) {
    testShell.setShellScripts(getScripts(argv[1]));
  }
  testShell.run();
  return 0;
#endif
}

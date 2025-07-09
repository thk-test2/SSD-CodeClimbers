#include "test_shell.h"
#include "command.h"
#include "ssd_exe.cpp"
#include "gmock/gmock.h"
#include "logger.h"

TestShell::TestShell() : ctrl(new StdInOutCtrl()), parser(new ArgParser()) {
  initializeCommandHandlers();
}

TestShell::TestShell(SSD_INTERFACE *_ssd)
    : ctrl(new StdInOutCtrl()), parser(new ArgParser()), ssd{_ssd} {
  initializeCommandHandlers();
}

TestShell::~TestShell() {
  delete ctrl;
  delete parser;
}

// MockDriver
class MockSSD : public SSD_INTERFACE {
public:
  MOCK_METHOD(void, read, (int lba), (override));
  MOCK_METHOD(void, write, (int lba, unsigned long value), (override));
  MOCK_METHOD(string, getResult, (), (override));
};

void TestShell::initializeCommandHandlers() {
  // Regular commands
  commandHandlers["read"] = std::make_unique<ReadCommand>();
  commandHandlers["write"] = std::make_unique<WriteCommand>();
  commandHandlers["fullread"] = std::make_unique<FullReadCommand>();
  commandHandlers["fullwrite"] = std::make_unique<FullWriteCommand>();
  commandHandlers["help"] = std::make_unique<HelpCommand>();
  commandHandlers["exit"] = std::make_unique<ExitCommand>();
  commandHandlers["erase"] = std::make_unique<EraseCommand>();
  commandHandlers["erase_range"] = std::make_unique<EraseRangeCommand>();

  // Test scripts
  commandHandlers["1_"] = std::make_unique<TestScript1>();
  commandHandlers["1_FullWriteAndReadCompare"] =
      std::make_unique<TestScript1>();
  commandHandlers["2_"] = std::make_unique<TestScript2>();
  commandHandlers["2_PartialLBAWrite"] = std::make_unique<TestScript2>();
  commandHandlers["3_"] = std::make_unique<TestScript3>();
  commandHandlers["3_WriteReadAging"] = std::make_unique<TestScript3>();
}

void TestShell::run() {
  if (!shellScripts.empty()) {
    runScripts();
    return;
  }
  runInteractive();
}

void TestShell::runScripts() {
  for (const auto &script : shellScripts) {
    cout << std::left << std::setw(30) << script << "___   Run...";
    command = parsing(script);
    executeCommand(command);
  }
}

void TestShell::runInteractive() {
  string userInput;
  while (true) {
    cout << "> ";
    getline(std::cin, userInput);
    command = parsing(userInput);
    if (command.command == "exit") {
      break;
    }
    executeCommand(command);
  }
}

void TestShell::executeCommand(const Command &command) {
  string cmd = command.command;

  auto commandIt = commandHandlers.find(cmd);
  if (commandIt != commandHandlers.end()) {
    commandIt->second->execute(this, command);
    return;
  }

  cout << "INVALID COMMAND" << endl;
}

Command TestShell::parsing(const string &userInput) {
  std::istringstream iss(userInput);
  string cmd;
  vector<string> args;
  if (iss >> cmd) {
    string arg;
    while (iss >> arg) {
      args.push_back(arg);
    }
  }
  return Command{cmd, args};
}

void TestShell::printHeader() {
  cout << "\033[1;36m"
       << "SSD Test Shell - Simple and Powerful SSD Testing Tool\n"
       << "\033[0m";
}

void TestShell::printTeamInfo() {
  cout << "\n\033[1mTeam: CodeClimbers\n\033[0m";
  cout << "\n\033[1mTeam Members:\033[0m\n"
       << "  Taehyun Kyong, Sunghwan Kim, Hyeonseok Sim\n"
       << "  Yerim Yun, Hoenhwi Jeong, Jeseong Kim\n"
       << "  Repository: https://github.com/thk-test2/SSD-CodeClimbers\n";
}

void TestShell::printCommands() {
  cout << "\n\033[1mCommands:\033[0m\n";

  // 명령어 순서를 고정하여 출력
  vector<string> commandOrder = {"read",      "write", "fullread",
                                 "fullwrite", "help",  "exit"};

  for (const string &cmdName : commandOrder) {
    auto it = commandHandlers.find(cmdName);
    if (it != commandHandlers.end()) {
      printCommandInfo(cmdName, it->second->getUsage(),
                       it->second->getDescription(), it->second->getExample());
    }
  }
}

void TestShell::printTestScripts() {
  cout << "\033[1mTest Scripts:\033[0m\n";
  printCommandInfo("1_FullWriteAndReadCompare", "",
                   "Run comprehensive write/read test for entire SSD",
                   "'1_' or '1_FullWriteAndReadCompare'");
  printCommandInfo("2_PartialLBAWrite", "",
                   "Run partial LBA write consistency test (30 iterations)",
                   "'2_' or '2_PartialLBAWrite'");
  printCommandInfo("3_WriteReadAging", "",
                   "Run write/read aging test (200 iterations)",
                   "'3_' or '3_WriteReadAging'");
}

void TestShell::printCommandInfo(const string &command, const string &args,
                                 const string &description,
                                 const string &example) {
  cout << "  \033[1m" << command << "\033[0m";
  if (!args.empty()) {
    cout << " " << args;
  }
  cout << "\n    Description: " << description << "\n"
       << "    Example: " << example << "\n\n";
}
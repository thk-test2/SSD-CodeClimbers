#include "test_shell.h"
#include "gmock/gmock.h"

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
    CommandLine cli = commandHandler->parseCommand(script);
    if (!commandHandler->executeCommand(cli))
      break;
  }
}

void TestShell::runInteractive() {
  string userInput;
  while (true) {
    cout << "> ";
    getline(std::cin, userInput);
    CommandLine cli = commandHandler->parseCommand(userInput);
    logger.print("TestShell.run()",
        "Successfully parsed command - " + cli.command);
    if (cli.command == "exit") {
      logger.print("TestShell.run()", "Exiting shell.");
      break;
    }
    commandHandler->executeCommand(cli);
  }
}


//void TestShell::printHeader() {
//  cout << "\033[1;36m"
//       << "SSD Test Shell - Simple and Powerful SSD Testing Tool\n"
//       << "\033[0m";
//}
//
//void TestShell::printTeamInfo() {
//  cout << "\n\033[1mTeam: CodeClimbers\n\033[0m";
//  cout << "\n\033[1mTeam Members:\033[0m\n"
//       << "  Taehyun Kyong, Sunghwan Kim, Hyeonseok Sim\n"
//       << "  Yerim Yun, Hoenhwi Jeong, Jeseong Kim\n"
//       << "  Repository: https://github.com/thk-test2/SSD-CodeClimbers\n";
//}
//
//void TestShell::printCommands() {
//  cout << "\n\033[1mCommands:\033[0m\n";
//
//  // 명령어 순서를 고정하여 출력
//  vector<string> commandOrder = {"read", "write", "fullread", "fullwrite",
//                                 "help", "exit",  "erase",    "erase_range", "flush"};
//
//  for (const string& cmdName : commandOrder) {
//    auto it = commandHandlers.find(cmdName);
//    if (it != commandHandlers.end()) {
//      printCommandInfo(cmdName, it->second->getUsage(),
//                       it->second->getDescription(), it->second->getExample());
//    }
//  }
//}
//
//void TestShell::printTestScripts() {
//  cout << "\033[1mTest Scripts:\033[0m\n";
//  printCommandInfo("1_FullWriteAndReadCompare", "",
//                   "Run comprehensive write/read test for entire SSD",
//                   "'1_' or '1_FullWriteAndReadCompare'");
//  printCommandInfo("2_PartialLBAWrite", "",
//                   "Run partial LBA write consistency test (30 iterations)",
//                   "'2_' or '2_PartialLBAWrite'");
//  printCommandInfo("3_WriteReadAging", "",
//                   "Run write/read aging test (200 iterations)",
//                   "'3_' or '3_WriteReadAging'");
//  printCommandInfo("4_EraseAndWriteAging", "",
//                   "Repeatedly erase LBA ranges and write random data to "
//                   "following LBAs in sequential loops (30 iterations)",
//                   "'4_' or '4_EraseAndWriteAging'");
//}
//
//void TestShell::printCommandInfo(const string &command, const string &args,
//                                 const string &description,
//                                 const string &example) {
//  cout << "  \033[1m" << command << "\033[0m";
//  if (!args.empty()) {
//    cout << " " << args;
//  }
//  cout << "\n    Description: " << description << "\n"
//       << "    Example: " << example << "\n\n";
//}
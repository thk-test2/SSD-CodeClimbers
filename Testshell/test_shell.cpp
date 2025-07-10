#include "test_shell.h"
#include "logger.h"
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
    logger.print("TestShell.runScripts()", "Executed script: " + script);
  }
}

void TestShell::runInteractive() {
  string userInput;
  while (true) {
    cout << "> ";
    getline(std::cin, userInput);
    logger.print("TestShell.runInteractive()", "User input: " + userInput);
    CommandLine cli = commandHandler->parseCommand(userInput);
    if (cli.command == "exit") {
      logger.print("TestShell.runInteractive()", "Exiting shell.");
      break;
    }
    commandHandler->executeCommand(cli);
    logger.print("TestShell.runInteractive()",
                 "Command executed: " + cli.command);
  }
}

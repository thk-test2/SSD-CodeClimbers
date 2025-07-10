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

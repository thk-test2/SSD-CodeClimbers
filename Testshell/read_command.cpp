#include "command.h"
#include "test_shell.h"

bool ReadCommand::execute(TestShell* shell, const Command& command) {
  if (!isValidReadUsage(command)) {
    cout << "INVALID COMMAND\n";
    return false;
  }

  int lba = stoi(command.args[0]);
  shell->getSSD()->read(lba);
  cout << "[Read] " << command.args[0] << " : " << shell->getSSD()->getResult() << "\n";
  return true;
}

bool ReadCommand::isValidReadUsage(const Command &command) {
  if (command.args.size() != 1)
    return false;
  try {
    int lba = stoi(command.args[0]);
  } catch (std::exception &e) {
    return false;
  }
  return true;
}
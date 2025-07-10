#include "command.h"

bool ReadCommand::execute(TestShell *shell, const Command &command) {
  if (!isValidReadUsage(command)) {
    cout << "INVALID COMMAND\n";
    return false;
  }

  int lba = stoi(command.args[0]);
  shell->getSSD()->read(lba);

  string result = shell->getSSD()->getResult();
  if (result == "ERROR") {
    cout << "[Read] ERROR\n";
    return false;
  }
  cout << "[Read] LBA " << std::setw(2) << std::setfill('0') << command.args[0]
       << " : " << result << "\n";
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
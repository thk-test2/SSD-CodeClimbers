#include "command.h"
#include "test_shell.h"

void WriteCommand::execute(TestShell* shell, const Command& command) {
  if (!isValidWriteUsage(command, shell->getHexBase())) {
    cout << "INVALID COMMAND\n";
    return;
  }
  
  shell->getSSD()->write(stoi(command.args[0]), 
                        stoul(command.args[1], nullptr, shell->getHexBase()));
  string result = shell->getSSD()->getResult();
  if (result == "")
    result = "Done";
  cout << "[Write] " << result << "\n";
}

bool WriteCommand::isValidWriteUsage(const Command &command, int hexBase) {
  if (command.args.size() != 2)
    return false;
  try {
    int lba = stoi(command.args[0]);
    unsigned long value = stoul(command.args[1], nullptr, hexBase);
  } catch (std::exception &e) {
    return false;
  }
  return true;
}
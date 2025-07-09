#include "command.h"
#include "test_shell.h"

bool FullWriteCommand::execute(TestShell* shell, const Command& command) {
  if (command.args.size() != 1) {
    cout << "INVALID COMMAND\n";
    return false;
  }
  
  unsigned long value;
  try {
    value = stoul(command.args[0], nullptr, shell->getHexBase());
  } catch (std::exception &e) {
    cout << "INVALID COMMAND\n";
    return false;
  }
  
  int lba = 0;
  shell->getSSD()->write(lba, value);
  string result = shell->getSSD()->getResult();
  while (result != "ERROR") {
    cout << "[Full Write] LBA: " << lba << " Done" << endl;
    lba++;
    shell->getSSD()->write(lba, value);
    result = shell->getSSD()->getResult();
  }
  return true;
}
#include "command.h"
#include "test_shell.h"

void FullWriteCommand::execute(TestShell* shell, const Command& command) {
  if (command.args.size() != 1) {
    cout << "INVALID COMMAND\n";
    return;
  }
  
  unsigned long value;
  try {
    value = stoul(command.args[0], nullptr, shell->getHexBase());
  } catch (std::exception &e) {
    cout << "INVALID COMMAND\n";
    return;
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
}
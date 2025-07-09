#include "command.h"
#include "test_shell.h"

bool FullReadCommand::execute(TestShell* shell, const Command& command) {
  if (command.args.size() != 0) {
    cout << "INVALID COMMAND\n";
    return false;
  }
  
  int lba = 0;
  shell->getSSD()->read(lba);
  string result = shell->getSSD()->getResult();
  while (result != "ERROR") {
    cout << "[Full Read] LBA: " << lba << " Result: " << result << endl;
    lba++;
    shell->getSSD()->read(lba);
    result = shell->getSSD()->getResult();
  }
  return true;
}
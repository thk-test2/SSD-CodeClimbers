#include "command.h"
#include "test_shell.h"

bool FullReadCommand::execute(TestShell* shell, const Command& command) {
  if (command.args.size() != 0) {
    cout << "INVALID COMMAND\n";
    return false;
  }
  
  for (int lba = 0; lba < MAX_LBA_COUNT; lba++) {
    shell->getSSD()->read(lba);
    string result = shell->getSSD()->getResult();
    if (result == "ERROR") {
      break;
    }
    cout << "[Full Read] LBA: " << lba << " Result: " << result << endl;
  }
  return true;
}
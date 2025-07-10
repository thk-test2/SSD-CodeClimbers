#include "command.h"

bool FullReadCommand::execute(TestShell *shell, const Command &command) {
  if (command.args.size() != 0) {
    cout << "INVALID COMMAND\n";
    return false;
  }
  cout << "[Full Read]\n";
  for (int lba = 0; lba < MAX_LBA_COUNT; lba++) {
    shell->getSSD()->read(lba);
    string result = shell->getSSD()->getResult();
    if (result == "ERROR") {
      return false;
    }
    cout << "LBA " << std::setw(2) << std::setfill('0') << lba << " : "
         << result << "\n";
  }
  cout << "[Full Read] Done\n";
  return true;
}
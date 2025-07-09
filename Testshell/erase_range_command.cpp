#include "command.h"
#include "test_shell.h"
#include <algorithm>

void EraseRangeCommand::execute(TestShell *shell, const Command &command) {
  if (!isValidEraseRangeUsage(command)) {
    cout << "INVALID COMMAND\n";
    return;
  }
  int st_lba = stoi(command.args[0]);
  int en_lba = stoi(command.args[1]);


  int remain_size = en_lba - st_lba + 1;
  int unit_size = std::min(MAX_SSD_ERASE_SIZE, remain_size);
  int lba = st_lba;
  while (remain_size > 0) {
    shell->getSSD()->erase(lba, unit_size);
    string result = shell->getSSD()->getResult();
    if (result == "ERROR") {
      cout << "ERROR during erase operation. st_lba : " << st_lba
           << " en_lba : " << en_lba << "\n";
      return;
    }

    remain_size -= unit_size;
    lba += unit_size;
    unit_size = std::min(10, remain_size);
  }
}

bool EraseRangeCommand::isValidEraseRangeUsage(const Command &command) {
  int st_lba, en_lba;
  try {
    st_lba = stoi(command.args[0]);
    en_lba = stoi(command.args[1]);
  } catch (std::exception &e) {
    return false;
  }

  if (st_lba < 0 || st_lba >= 100)
    return false;
  if (en_lba < 0 || en_lba >= 100)
    return false;
  if (st_lba > en_lba)
    return false;

  return true;
}
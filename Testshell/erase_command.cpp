#include "command.h"
#include "test_shell.h"
#include <algorithm>

void EraseCommand::execute(TestShell *shell, const Command &command) {
  int lba, size;
  try {
    lba = stoi(command.args[0]);
    size = stoi(command.args[1]);
  } catch (std::exception &e) {
    cout << "INVALID COMMAND\n";
    return;
  }

  // LBA 가 0 ~ 99 범위에서 벗어나거나, SIZE 가 1 미만 100 초과
  if (lba < 0 || lba >= 100) {
    cout << "INVALID COMMAND\n";
    return;
  }
  if (size < 1 || size > 100) {
    cout << "INVALID COMMAND\n";
    return;
  }
  if (lba + size > 100) {
    cout << "INVALID COMMAND\n";
    return;
  }

  int remain_size = size;
  int unit_size = std::min(10, size);
  while (remain_size > 0) {
    shell->getSSD()->erase(lba, unit_size);
    string result = shell->getSSD()->getResult();
    if (result == "ERROR") {
      cout << "ERROR during erase operation. lba : " << lba
           << " size : " << size << "\n";
      return;
    }

    remain_size -= unit_size;
    unit_size = std::min(10, remain_size);
  }
}

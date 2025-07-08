#include "command.h"
#include "test_shell.h"

void TestScript1::execute(TestShell* shell, const Command& command) {
  string valueStr = command.args[0];
  unsigned long value = std::stoul(valueStr, nullptr, shell->getHexBase());

  for (int lba = 0; lba < 100; lba += 4) {
    for (int i = 0; i < 4; i++) {
      shell->getSSD()->write(lba + i, value);
    }

    for (int i = 0; i < 4; i++) {
      shell->getSSD()->read(lba);
      string result = shell->getSSD()->getResult();
      if (result != valueStr) {
        cout << "Script 1 execution failed." << endl;
        return;
      }
    }
  }
  cout << "Script 1 executed successfully." << endl;
}

void TestScript2::execute(TestShell *shell, const Command &command) {
  string valueStr = command.args[0];
  unsigned long value = std::stoul(valueStr, nullptr, 16);
  vector<int> lbaList{4, 0, 3, 1, 2};

  for (int turn = 0; turn < 30; ++turn) {
    for (auto lba : lbaList) {
      shell->getSSD()->write(lba, value);
      if (!checkPartialWriteSuccess(shell, lba, valueStr)) {
        cout << "Script 2 execution failed." << endl;
        return;
      }
    }
  }
  cout << "Script 2 executed successfully." << endl;
}

bool TestScript2::checkPartialWriteSuccess(TestShell *shell, int lba,
                                           string &valueStr) {
  shell->getSSD()->read(lba);

  if (shell->getSSD()->getResult() == "ERROR" ||
      shell->getSSD()->getResult() != valueStr) {
    return false;
  }
  return true;
}

void TestScript3::execute(TestShell *shell, const Command &command) {
  string valueStr = command.args[0];
  unsigned long value = std::stoul(valueStr, nullptr, 16);

  for (int i = 0; i < 200; i++) {
    shell->getSSD()->write(0, value);
    if (!checkPartialWriteSuccess(shell, 0, valueStr)) {
      cout << "Script 3 execution failed." << endl;
      return;
    }

    shell->getSSD()->write(99, value);
    if (!checkPartialWriteSuccess(shell, 99, valueStr)) {
      cout << "Script 3 execution failed." << endl;
      return;
    }
  }
  cout << "Script 3 executed successfully." << endl;
}

bool TestScript3::checkPartialWriteSuccess(TestShell *shell, int lba,
                                           string &valueStr) {
  shell->getSSD()->read(lba);

  if (shell->getSSD()->getResult() == "ERROR" ||
      shell->getSSD()->getResult() != valueStr) {
    return false;
  }
  return true;
}
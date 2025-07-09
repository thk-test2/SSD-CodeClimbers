#include "command.h"
#include "test_shell.h"

bool checkPartialWriteSuccess(TestShell *shell, int lba, unsigned long value) {
  shell->getSSD()->read(lba);

  string valueStr = convertHexToString(value);
  if (shell->getSSD()->getResult() == "ERROR" ||
      shell->getSSD()->getResult() != valueStr) {
    return false;
  }
  return true;
}

void TestScript1::execute(TestShell *shell, const Command &command) {
  if (command.args.size() != 0) {
    std::cout << "INVALID_COMMAND\n";
    return;
  }

  unsigned long value = 0xAAAABBBB;
  string valueStr = convertHexToString(value);

  for (int lba = 0; lba < 100; lba += 4) {
    for (int i = 0; i < 4; i++) {
      shell->getSSD()->write(lba + i, value);
    }

    for (int i = 0; i < 4; i++) {
      shell->getSSD()->read(lba);
      string result = shell->getSSD()->getResult();
      if (result != valueStr) {
        cout << "FAIL!" << endl;
        return;
      }
    }
  }
  cout << "Pass" << endl;
}

void TestScript2::execute(TestShell *shell, const Command &command) {
  if (command.args.size() != 0) {
    std::cout << "INVALID_COMMAND\n";
    return;
  }

  unsigned long value = 0xAAAABBBB;
  vector<int> lbaList{4, 0, 3, 1, 2};

  for (int turn = 0; turn < 30; ++turn) {
    for (auto lba : lbaList) {
      shell->getSSD()->write(lba, value);
      if (!checkPartialWriteSuccess(shell, lba, value)) {
        cout << "FAIL!" << endl;
        return;
      }
    }
  }
  cout << "Pass" << endl;
}

void TestScript3::execute(TestShell *shell, const Command &command) {
#ifdef _DEBUG
  unsigned long value = stoul(command.args[0], nullptr, 16);
#else
  if (command.args.size() != 0) {
    std::cout << "INVALID_COMMAND\n";
    return;
  }
  unsigned long value = getRandomValue();
#endif

  for (int i = 0; i < 200; i++) {
    shell->getSSD()->write(0, value);
    if (!checkPartialWriteSuccess(shell, 0, value)) {
      cout << "FAIL!" << endl;
      return;
    }

    shell->getSSD()->write(99, value);
    if (!checkPartialWriteSuccess(shell, 99, value)) {
      cout << "FAIL!" << endl;
      return;
    }
  }
  cout << "Pass" << endl;
}

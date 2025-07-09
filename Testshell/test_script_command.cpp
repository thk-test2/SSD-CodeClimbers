#include "command.h"
#include "test_shell.h"

#include <random>

unsigned long getRandomValue() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<uint32_t> dist(0x00000000u, 0xFFFFFFFFu);
  return dist(gen);
}

std::string convertHexToString(unsigned long value) {
  char buf[11];
  std::snprintf(buf, sizeof(buf), "0x%08lX", value);
  return std::string{buf};
}

bool checkPartialWriteSuccess(TestShell *shell, int lba, unsigned long value) {
  shell->getSSD()->read(lba);

  string valueStr = convertHexToString(value);
  if (shell->getSSD()->getResult() == "ERROR" ||
      shell->getSSD()->getResult() != valueStr) {
    return false;
  }
  return true;
}

void TestScript1::execute(TestShell* shell, const Command& command) {
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
        cout << "Script 1 execution failed." << endl;
        return;
      }
    }
  }
  cout << "Script 1 executed successfully." << endl;
}

void TestScript2::execute(TestShell *shell, const Command &command) {
  unsigned long value = 0xAAAABBBB;
  vector<int> lbaList{4, 0, 3, 1, 2};

  for (int turn = 0; turn < 30; ++turn) {
    for (auto lba : lbaList) {
      shell->getSSD()->write(lba, value);
      if (!checkPartialWriteSuccess(shell, lba, value)) {
        cout << "Script 2 execution failed." << endl;
        return;
      }
    }
  }
  cout << "Script 2 executed successfully." << endl;
}

void TestScript3::execute(TestShell *shell, const Command &command) {
  unsigned long value = getRandomValue();

  for (int i = 0; i < 200; i++) {
    shell->getSSD()->write(0, value);
    if (!checkPartialWriteSuccess(shell, 0, value)) {
      cout << "Script 3 execution failed." << endl;
      return;
    }

    shell->getSSD()->write(99, value);
    if (!checkPartialWriteSuccess(shell, 99, value)) {
      cout << "Script 3 execution failed." << endl;
      return;
    }
  }
  cout << "Script 3 executed successfully." << endl;
}

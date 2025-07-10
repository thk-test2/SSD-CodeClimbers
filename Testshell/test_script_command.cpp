#include "command.h"

bool checkPartialWriteSuccess(SSD_INTERFACE &ssd, int lba, unsigned long value) {
  ssd.read(lba);

  string valueStr = convertHexToString(value);
  if (ssd.getResult() == "ERROR" ||
      ssd.getResult() != valueStr) {
    return false;
  }
  return true;
}

bool preConditionCheck(const CommandLine &cli) {
  if (cli.args.size() != 0) {
    cout << "INVALID_COMMAND\n";
    return false;
  }
  return true;
}

bool TestScript1::execute(SSD_INTERFACE &ssd, const CommandLine &cli) {
  if (!preConditionCheck(cli))
    return false;

  unsigned long value = 0xAAAABBBB;
  string valueStr = convertHexToString(value);

if (!onExecute(ssd, value, valueStr))
    return false;
  return true;
}

bool TestScript1::onExecute(SSD_INTERFACE &ssd, unsigned long value,
                            std::string &valueStr) {
  for (int lba = 0; lba < 100; lba += 4) {
    for (int i = 0; i < 4; i++) {
      ssd.write(lba + i, value);
    }

    for (int i = 0; i < 4; i++) {
      ssd.read(lba);
      string result = ssd.getResult();
      if (result != valueStr) {
        cout << "FAIL!" << endl;
        return false;
      }
    }
  }
  cout << "Pass" << endl;
  return true;
}

bool TestScript2::execute(SSD_INTERFACE &ssd, const CommandLine &cli) {
  if (!preConditionCheck(cli))
    return false;

  unsigned long value = 0xAAAABBBB;
  vector<int> lbaList{4, 0, 3, 1, 2};

  for (int turn = 0; turn < 30; ++turn) {
    for (auto lba : lbaList) {
      ssd.write(lba, value);
      if (!checkPartialWriteSuccess(ssd, lba, value)) {
        cout << "FAIL!" << endl;
        return false;
      }
    }
  }
  cout << "Pass" << endl;
  return true;
}

bool TestScript3::execute(SSD_INTERFACE &ssd, const CommandLine &cli) {
#ifdef _DEBUG
  unsigned long value = stoul(cli.args[0], nullptr, 16);
#else
  if (!preConditionCheck(cli))
    return false;
  unsigned long value = getRandomValue();
#endif

  for (int i = 0; i < 200; i++) {
    ssd.write(0, value);
    if (!checkPartialWriteSuccess(ssd, 0, value)) {
      cout << "FAIL!" << endl;
      return false;
    }

    ssd.write(99, value);
    if (!checkPartialWriteSuccess(ssd, 99, value)) {
      cout << "FAIL!" << endl;
      return false;
    }
  }
  cout << "Pass" << endl;
  return true;
}

bool TestScript4::execute(SSD_INTERFACE &ssd, const CommandLine &cli) {
  if (!preConditionCheck(cli))
    return false;
  unsigned long value1 = getRandomValue();
  unsigned long value2 = getRandomValue();

  ssd.erase(0, 3);
  if (ssd.getResult() == "ERROR") {
    cout << "FAIL!" << endl;
    return false;
  }

  for (int i = 0; i < 30; i++) {
    int slba = 2, elba = 4;
    while (slba < 99) {
      ssd.write(slba, value1);
      if (ssd.getResult() == "ERROR") {
        cout << "FAIL!" << endl;
        return false;
      }

      ssd.write(slba, value2);
      if (ssd.getResult() == "ERROR") {
        cout << "FAIL!" << endl;
        return false;
      }

      int size = elba - slba + 1;
      ssd.erase(slba, size);
      if (ssd.getResult() == "ERROR") {
        cout << "FAIL!" << endl;
        return false;
      }

      slba = elba;
      elba += 2;
      if (elba == 100)
        elba = 99;
    }
  }
  cout << "Pass" << endl;
  return true;
}
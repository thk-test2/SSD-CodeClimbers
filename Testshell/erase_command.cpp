#include "command.h"
#include <algorithm>

bool EraseCommand::execute(SSD_INTERFACE &ssd, const CommandLine &cli) {
  logger.print("EraseCommand.execute()", "Executing erase command");
  if (!isValidEraseUsage(cli)) {
    std::cout << "INVALID COMMAND\n";
    return false;
  }
  int lba = std::stoi(cli.args[0]);
  int size = std::stoi(cli.args[1]);

  int remain_size = size;
  int unit_size = std::min(MAX_SSD_ERASE_SIZE, size);
  while (remain_size > 0) {
    ssd.erase(lba, unit_size);
    std::string result = ssd.getResult();
    if (result == "ERROR") {
      std::cout << "ERROR during erase operation. lba : " << lba
                << " size : " << size << "\n";
      return false;
    }

    remain_size -= unit_size;
    lba += unit_size;
    unit_size = std::min(MAX_SSD_ERASE_SIZE, remain_size);
  }
  cout << "[Erase] Done\n";
  return true;
}

bool EraseCommand::isValidEraseUsage(const CommandLine &cli) {
  int lba, size;
  try {
    lba = std::stoi(cli.args[0]);
    size = std::stoi(cli.args[1]);
  } catch (std::exception &e) {
    return false;
  }

  // LBA 가 0 ~ 99 범위에서 벗어나거나, SIZE 가 1 미만 100 초과
  if (lba < 0 || lba >= 100)
    return false;
  if (size < 1 || size > 100)
    return false;
  if (lba + size > 100)
    return false;

  return true;
}
#include "command.h"
#include <algorithm>

bool EraseRangeCommand::execute(SSD_INTERFACE &ssd, const CommandLine &cli) {
  logger.print("EraseRangeCommand.execute()", "Executing erase range command");
  if (!isValidEraseRangeUsage(cli)) {
    std::cout << "INVALID COMMAND\n";
    return false;
  }
  int st_lba = std::stoi(cli.args[0]);
  int en_lba = std::stoi(cli.args[1]);

  int remain_size = en_lba - st_lba + 1;
  int unit_size = std::min(MAX_SSD_ERASE_SIZE, remain_size);
  int lba = st_lba;
  while (remain_size > 0) {
    ssd.erase(lba, unit_size);
    std::string result = ssd.getResult();
    if (result == "ERROR") {
      std::cout << "ERROR during erase operation. st_lba : " << st_lba
                << " en_lba : " << en_lba << "\n";
      return false;
    }

    remain_size -= unit_size;
    lba += unit_size;
    unit_size = std::min(10, remain_size);
  }
  cout << "[Erase Range] Done\n";
  return true;
}

bool EraseRangeCommand::isValidEraseRangeUsage(const CommandLine &cli) {
  int st_lba, en_lba;
  try {
    st_lba = std::stoi(cli.args[0]);
    en_lba = std::stoi(cli.args[1]);
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
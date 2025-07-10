#include "command.h"

bool FullWriteCommand::execute(SSD_INTERFACE &ssd, const CommandLine &cli) {
  if (cli.args.size() != 1) {
    std::cout << "INVALID COMMAND\n";
    return false;
  }

  unsigned long value;
  try {
    value = std::stoul(cli.args[0], nullptr, HEX_BASE);
  } catch (std::exception &e) {
    std::cout << "INVALID COMMAND\n";
    return false;
  }

  for (int lba = 0; lba < MAX_LBA_COUNT; lba++) {
    ssd.write(lba, value);
    string result = ssd.getResult();
    if (result == "ERROR") {
      cout << "[Full Write] Failed\n";
      return false;
    }
  }
  cout << "[Full Write] Done\n";
  return true;
}
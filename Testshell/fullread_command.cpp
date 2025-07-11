#include "command.h"

bool preConditionCheck(const CommandLine &cli) {
  if (cli.args.size() != 0) {
    cout << "INVALID_COMMAND\n";
    return false;
  }
  return true;
}

bool FullReadCommand::execute(SSD_INTERFACE &ssd, const CommandLine &cli) {
  logger.print("FullReadCommand.execute()", "Executing full read command");
  if (!preConditionCheck(cli))
    return false;

  cout << "[Full Read]\n";
  for (int lba = 0; lba < MAX_LBA_COUNT; lba++) {
    ssd.read(lba);
    string result = ssd.getResult();
    if (result == "ERROR") {
      cout << "[Full Read] Failed\n";
      return false;
    }
    cout << "LBA " << std::setw(2) << std::setfill('0') << lba << " : "
         << result << "\n";
  }
  cout << "[Full Read] Done\n";
  return true;
}

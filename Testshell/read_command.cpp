#include "command.h"

bool ReadCommand::execute(SSD_INTERFACE &ssd, const CommandLine &cli) {
  if (!isValidReadUsage(cli)) {
    cout << "INVALID COMMAND\n";
    return false;
  }

  int lba = stoi(cli.args[0]);
  ssd.read(lba);
  string result = ssd.getResult();
  if (result == "ERROR") {
    cout << "[Read] ERROR\n";
    return false;
  }
  cout << "[Read] LBA " << std::setw(2) << std::setfill('0') << cli.args[0]
       << " : " << result << "\n";

  return true;
}

bool ReadCommand::isValidReadUsage(const CommandLine &cli) {
  if (cli.args.size() != 1)
    return false;
  try {
    int lba = stoi(cli.args[0]);
  } catch (std::exception &e) {
    return false;
  }
  return true;
}
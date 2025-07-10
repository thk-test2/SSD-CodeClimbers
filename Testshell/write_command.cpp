#include "command.h"

bool WriteCommand::execute(SSD_INTERFACE &ssd, const CommandLine& cli) {
  logger.print("WriteCommand.execute()", "Executing write command");
  if (!isValidWriteUsage(cli, HEX_BASE)) {
    cout << "INVALID COMMAND\n";
    return false;
  }

  ssd.write(stoi(cli.args[0]), 
             stoul(cli.args[1], nullptr, HEX_BASE));
  string result = ssd.getResult();
  if (result == "")
    result = "Done";
  cout << "[Write] " << result << "\n";
  return true;
}

bool WriteCommand::isValidWriteUsage(const CommandLine &cli, int hexBase) {
  if (cli.args.size() != 2)
    return false;
  try {
    int lba = stoi(cli.args[0]);
    unsigned long value = stoul(cli.args[1], nullptr, hexBase);
  } catch (std::exception &e) {
    return false;
  }
  return true;
}
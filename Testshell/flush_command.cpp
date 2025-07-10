#include "command.h"

bool FlushCommand::execute(SSD_INTERFACE &ssd, const CommandLine &cli) {
  if (cli.args.size() != 0) {
    cout << "INVALID COMMAND\n";
    return false;
  }

  ssd.flush();
  string result = ssd.getResult();

  if (result == "ERROR") {
    cout << "[Flush] ERROR\n";
  } else {
    cout << "[Flush] Done\n";
  }
  return true;
}
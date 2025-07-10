#include "command.h"

bool ExitCommand::execute(SSD_INTERFACE &ssd, const CommandLine &cli) {
  logger.print("ExitCommand.execute()", "Executing exit command");
  if (cli.args.size() != 0) {
    cout << "INVALID COMMAND\n";
    return false;
  }
  cout << "Exiting shell..." << endl;
  throw ShellExit();
}
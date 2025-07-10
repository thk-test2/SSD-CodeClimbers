#include "command.h"

bool ExitCommand::execute(TestShell* shell, const Command& command) {
  if (command.args.size() != 0) {
    cout << "INVALID COMMAND\n";
    return false;
  }
  cout << "Exiting shell..." << endl;
  throw ShellExit();
}
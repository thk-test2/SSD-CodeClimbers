#include "command.h"
#include "test_shell.h"

void ExitCommand::execute(TestShell* shell, const Command& command) {
  if (command.args.size() != 0) {
    cout << "INVALID COMMAND\n";
    return;
  }
  cout << "Exiting shell..." << endl;
  throw ShellExit();
}
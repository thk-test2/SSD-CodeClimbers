#include "command.h"
#include "test_shell.h"

void FlushCommand::execute(TestShell *shell, const Command &command) {
  if (command.args.size() != 0) {
    cout << "INVALID COMMAND\n";
    return;
  }

  shell->getSSD()->flush();
  string result = shell->getSSD()->getResult();

  if (result == "ERROR") {
    cout << "[Flush] ERROR\n";
  } else {
    cout << "[Flush] Done\n";
  }
}
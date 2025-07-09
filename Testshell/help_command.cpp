#include "command.h"
#include "test_shell.h"

bool HelpCommand::execute(TestShell *shell, const Command &command) {
  shell->printHeader();
  shell->printTeamInfo();
  shell->printCommands();
  shell->printTestScripts();
  return true;
}
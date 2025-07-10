#include "command.h"

bool HelpCommand::execute(TestShell *shell, const Command &command) {
  shell->printHeader();
  shell->printTeamInfo();
  shell->printCommands();
  shell->printTestScripts();
  return true;
}
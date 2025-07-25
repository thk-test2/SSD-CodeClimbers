#include <sstream>

#include "command.h"
#include "logger.h"

bool CommandHandler::executeCommand(const CommandLine &cli) {
  auto command = getCommand(cli.command);
  if (command) {
    return command->execute(*_ssdAdaptor, cli);
  }
  cout << "INVALID COMMAND" << endl;
  return false;
}

void CommandHandler::addCommand(ICommand *command) {
  commands.push_back(command);
}

vector<string> CommandHandler::getAvailableCommands() {
  vector<string> commandNames;
  for (const auto &command : commands) {
    commandNames.push_back(command->getCommandName());
  }
  return commandNames;
}

ICommand *CommandHandler::getCommand(const string &commandName) {
  for (const auto &command : commands) {
    if (command->isCmdMatch(commandName)) {
      return command;
    }
  }
  return nullptr;
}

CommandLine CommandHandler::parseCommand(const string &input) {
  std::istringstream iss(input);
  string cmd;
  vector<string> args;
  if (iss >> cmd) {
    logger.print("CommandHandler.parseCommand()", "Parsed command: " + cmd);
    string arg;
    while (iss >> arg) {
      args.push_back(arg);
    }
  }
  return CommandLine{cmd, args};
}

void CommandHandler::initialize() {
  addCommand(new ExitCommand());
  addCommand(new FullReadCommand());
  addCommand(new FullWriteCommand());
  addCommand(new HelpCommand(this));
  addCommand(new ReadCommand());
  addCommand(new WriteCommand());
  addCommand(new EraseCommand());
  addCommand(new EraseRangeCommand());
  addCommand(new FlushCommand());
  addCommand(new TestScript1());
  addCommand(new TestScript2());
  addCommand(new TestScript3());
  addCommand(new TestScript4());
}
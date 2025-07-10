#pragma once

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <iomanip>
#include <algorithm>

#include "command.h"
#include "logger.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;

class TestShell {
private:
  CommandHandler *commandHandler;
  Logger &logger = Logger::getInstance();

  vector<string> shellScripts;

  void runInteractive();
  void runScripts();

public:
  TestShell() : commandHandler{new CommandHandler()} {};
  TestShell(CommandHandler *_commandHandler) : commandHandler{_commandHandler} {};
  TestShell(SSD_INTERFACE *_ssdDriver) : commandHandler{new CommandHandler(_ssdDriver)} {};
  ~TestShell() { delete commandHandler; }

  void run();

  CommandHandler *getCommandHandler() const {
    return commandHandler;
  }

  void setShellScripts(const vector<string> &scripts) {
    this->shellScripts = scripts;
  }
};
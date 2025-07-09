#pragma once

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <numeric>
#include <random>

#include "ssd_interface.h"
#include "logger.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;

struct ShellExit : public std::exception {
  const char *what() const noexcept override { return "Shell exited."; }
};

class Command {
public:
  string command;
  vector<string> args;
};

// Forward declarations
class TestShell;

// Abstract base class for command handlers
class ICommandHandler {
public:
  virtual ~ICommandHandler() = default;
  virtual void execute(TestShell *shell, const Command &command) = 0;
  virtual string getUsage() const = 0;
  virtual string getDescription() const = 0;
  virtual string getExample() const = 0;
};

class TestShell {
private:
  SSD_INTERFACE *ssd;
  Command command;
  std::unordered_map<string, std::unique_ptr<ICommandHandler>> commandHandlers;
  Logger &logger = Logger::getInstance();

  const int HEX_BASE = 16;

  void initializeCommandHandlers();

public:
  TestShell();
  TestShell(SSD_INTERFACE *_ssd);
  ~TestShell();

  void run();
  void executeCommand(const Command &command);
  
  // Public methods for command handlers to access
  SSD_INTERFACE* getSSD() { return ssd; }
  int getHexBase() const { return HEX_BASE; }
  
  Command parsing(const string &userInput);
  
  // Helper methods for commands
  void printHeader();
  void printTeamInfo();
  void printCommands();
  void printTestScripts();
  void printCommandInfo(const string &command, const string &args,
                        const string &description, const string &example);
};

static unsigned long getRandomValue() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<uint32_t> dist(0x00000000u, 0xFFFFFFFFu);
  return dist(gen);
}

static std::string convertHexToString(unsigned long value) {
  char buf[11];
  std::snprintf(buf, sizeof(buf), "0x%08lX", value);
  return std::string{buf};
}

#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <iomanip>

#include "ssd_interface.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;

// Forward declaration of CommandHandler
class CommandHandler;

class CommandLine {
public:
  string command;
  vector<string> args;
};

struct ShellExit : public std::exception {
  const char *what() const noexcept override { return "Shell exited."; }
};

// Abstract base class for command
class ICommand {
public:
  ICommand() = default;  // Add default constructor
  virtual ~ICommand() = default;
  virtual bool execute(SSD_INTERFACE &ssd, const CommandLine &cli) = 0;
  virtual string getUsage() const = 0;
  virtual string getDescription() const = 0;
  virtual string getExample() const = 0;
  virtual void setCommandName(const string &name) {
    commandName = name;
  }
  virtual bool isCmdMatch(const string command) const = 0;
  virtual string getCommandName() const { return commandName; }

protected:
  const int MAX_LBA_COUNT = 100;
  const int HEX_BASE = 16;
  string commandName;
};

class ExitCommand : public ICommand {
public:
  ExitCommand() { setCommandName("exit"); }
  bool execute(SSD_INTERFACE &ssd, const CommandLine &cli) override;
  string getUsage() const override { return ""; }
  string getDescription() const override { return "Exit the shell"; }
  string getExample() const override { return "exit"; }
  bool isCmdMatch(const string command) const override { return "exit" == command;};
};

class FullReadCommand : public ICommand {
public:
  FullReadCommand() { setCommandName("fullread"); }
  bool execute(SSD_INTERFACE &ssd, const CommandLine &cli) override;
  string getUsage() const override { return ""; }
  string getDescription() const override { return "Read the entire SSD"; }
  string getExample() const override { return "fullread"; }
  bool isCmdMatch(const string command) const override { return "fullread" == command;};
};

class FullWriteCommand : public ICommand {
public:
  FullWriteCommand() { setCommandName("fullwrite"); }
  bool execute(SSD_INTERFACE &ssd, const CommandLine &cli) override;
  string getUsage() const override { return "<value>"; }
  string getDescription() const override { return "Write <value> to the entire SSD"; }
  string getExample() const override { return "fullwrite 0x00"; }
  bool isCmdMatch(const string command) const override { return "fullwrite" == command;};
};

class HelpCommand : public ICommand {
public:
  HelpCommand(CommandHandler *commandHandler)
      : _commandHandler(commandHandler) {
    setCommandName("help");
  }

  bool execute(SSD_INTERFACE &ssd, const CommandLine &cli) override;
  string getUsage() const override { return ""; }
  string getDescription() const override { return "Show this help message"; }
  string getExample() const override { return "help"; }
  bool isCmdMatch(const string command) const override { return "help" == command;};

private:
  CommandHandler *_commandHandler;

  void printHeader();
  void printTeamInfo();
  void printCommands();
  void printCommandInfo(const string &command, const string &args,
                        const string &description, const string &example);
};

class ReadCommand : public ICommand {
private:
  bool isValidReadUsage(const CommandLine &cli);

public:
  ReadCommand() { setCommandName("read"); }
  bool execute(SSD_INTERFACE &ssd, const CommandLine &cli) override;
  string getUsage() const override { return "<lba>"; }
  string getDescription() const override { return "Read from SSD at logical block address <lba>"; }
  string getExample() const override { return "read 10"; }
  bool isCmdMatch(const string command) const override { return "read" == command;};
};

class WriteCommand : public ICommand {
private:
  bool isValidWriteUsage(const CommandLine &cli, int hexBase);

public:
  WriteCommand() { setCommandName("write"); }
  bool execute(SSD_INTERFACE &ssd, const CommandLine &cli) override;
  string getUsage() const override { return "<lba> <value>"; }
  string getDescription() const override { return "Write <value> to SSD at logical block address <lba>"; }
  string getExample() const override { return "write 5 0xFF"; }
  bool isCmdMatch(const string command) const override { return "write" == command;};
};

class EraseCommand : public ICommand {
private:
  bool isValidEraseUsage(const CommandLine &cli);
  const int MAX_SSD_ERASE_SIZE = 10;

public:
  EraseCommand() { setCommandName("erase"); }
  bool execute(SSD_INTERFACE &ssd, const CommandLine &cli) override;
  string getUsage() const override { return "<lba> <size>"; }
  string getDescription() const override {
    return "Erase the logical blocks from <lba> for the specified <size>.";
  }
  string getExample() const override { return "erase 0 10"; }
  bool isCmdMatch(const string command) const override { return "erase" == command;};
};

class EraseRangeCommand : public ICommand {
private:
  bool isValidEraseRangeUsage(const CommandLine &cli);
  const int MAX_SSD_ERASE_SIZE = 10;

public:
  EraseRangeCommand() { setCommandName("erase_range"); }
  bool execute(SSD_INTERFACE &ssd, const CommandLine &cli) override;
  string getUsage() const override { return "<st_lba> <en_lba>"; }
  string getDescription() const override {
    return "Erase the SSD's logical blocks from st_lba to en_lba";
  }
  string getExample() const override { return "erase_range 0 99"; }
  bool isCmdMatch(const string command) const override { return "erase_range" == command;};
};

class FlushCommand : public ICommand {
public:
  FlushCommand() { setCommandName("flush"); }
  bool execute(SSD_INTERFACE &ssd, const CommandLine &cli) override;
  string getUsage() const override { return ""; }
  string getDescription() const override {
    return "Flush all buffered commands to SSD";
  }
  string getExample() const override { return "flush"; }
  bool isCmdMatch(const string command) const override { return "flush" == command;};
};

class TestScript1 : public ICommand {
public:
  TestScript1() { setCommandName("1_FullWriteAndReadCompare"); }
  bool execute(SSD_INTERFACE &ssd, const CommandLine &cli) override;
  string getUsage() const override { return ""; }
  string getDescription() const override { return "Run comprehensive write/read test for entire SSD"; }
  string getExample() const override { return "'1_' or '1_FullWriteAndReadCompare'"; }
  bool isCmdMatch(const string command) const override { return "1_" == command || "1_FullWriteAndReadCompare" == command;};
};

class TestScript2 : public ICommand {
public:
  TestScript2() { setCommandName("2_PartialLBAWrite"); }
  bool execute(SSD_INTERFACE &ssd, const CommandLine &cli) override;
  string getUsage() const override { return ""; }
  string getDescription() const override { return "Run partial LBA write consistency test (30 iterations)"; }
  string getExample() const override { return "'2_' or '2_PartialLBAWrite'"; }
  bool isCmdMatch(const string command) const override { return "2_" == command || "2_PartialLBAWrite" == command;};
};

class TestScript3 : public ICommand {
public:
  TestScript3() { setCommandName("3_WriteReadAging"); }
  bool execute(SSD_INTERFACE &ssd, const CommandLine &cli) override;
  string getUsage() const override { return ""; }
  string getDescription() const override { return "Run write/read aging test (200 iterations)"; }
  string getExample() const override { return "'3_' or '3_WriteReadAging'"; }
  bool isCmdMatch(const string command) const override { return "3_" == command || "3_WriteReadAging" == command;};
};

class TestScript4 : public ICommand {
public:
  TestScript4() { setCommandName("4_EraseAndWriteAging"); }
  bool execute(SSD_INTERFACE &ssd, const CommandLine &cli) override;
  string getUsage() const override { return ""; }
  string getDescription() const override {
    return "Repeatedly erase LBA ranges and write random data to following LBAs in sequential loops (30 iterations)";
  }
  string getExample() const override { return "'4_' or '4_EraseAndWriteAging'"; }
  bool isCmdMatch(const string command) const override { return "4_" == command || "4_EraseAndWriteAging" == command;};
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

// CommandHandler class for executing commands
class CommandHandler {
public:
  ~CommandHandler() = default;
  CommandHandler() { initialize(); }
  CommandHandler(SSD_INTERFACE *ssdDriver) : _ssdDriver(ssdDriver) {
    initialize();
  }

  void initialize();
  ICommand *getCommand(const string &commandName);
  bool executeCommand(const CommandLine &cli);
  vector<string> getAvailableCommands();
  CommandLine parseCommand(const string &input);

private:
  vector<ICommand *> commands;
  SSD_INTERFACE *_ssdDriver;

  void addCommand(ICommand *command);
  void removeCommand(const string &commandName);
};
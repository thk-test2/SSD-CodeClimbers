#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <random>

#include "ssd_interface.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;

class CommandLine {
public:
  string command;
  vector<string> args;
};

// Abstract base class for command
class ICommand {
public:
  virtual ~ICommand() = default;
  virtual bool execute(SSD_INTERFACE &ssd, const CommandLine &cli) = 0;
  virtual string getUsage() const = 0;
  virtual string getDescription() const = 0;
  virtual string getExample() const = 0;

protected:
  const int MAX_LBA_COUNT = 100;
  const int HEX_BASE = 16;
};

class ExitCommand : public ICommand {
public:
  bool execute(SSD_INTERFACE &ssd, const CommandLine &cli) override;
  string getUsage() const override { return ""; }
  string getDescription() const override { return "Exit the shell"; }
  string getExample() const override { return "exit"; }
};

class FullReadCommand : public ICommand {
public:
  bool execute(SSD_INTERFACE &ssd, const CommandLine &cli) override;
  string getUsage() const override { return ""; }
  string getDescription() const override { return "Read the entire SSD"; }
  string getExample() const override { return "fullread"; }
};

class FullWriteCommand : public ICommand {
public:
  bool execute(SSD_INTERFACE &ssd, const CommandLine &cli) override;
  string getUsage() const override { return "<value>"; }
  string getDescription() const override { return "Write <value> to the entire SSD"; }
  string getExample() const override { return "fullwrite 0x00"; }
};

class HelpCommand : public ICommand {
public:
  bool execute(SSD_INTERFACE &ssd, const CommandLine &cli) override;
  string getUsage() const override { return ""; }
  string getDescription() const override { return "Show this help message"; }
  string getExample() const override { return "help"; }
};

class ReadCommand : public ICommand {
private:
  bool isValidReadUsage(const CommandLine &cli);

public:
  bool execute(SSD_INTERFACE &ssd, const CommandLine &cli) override;
  string getUsage() const override { return "<lba>"; }
  string getDescription() const override { return "Read from SSD at logical block address <lba>"; }
  string getExample() const override { return "read 10"; }
};

class WriteCommand : public ICommand {
private:
  bool isValidWriteUsage(const CommandLine &cli, int hexBase);

public:
  bool execute(SSD_INTERFACE &ssd, const CommandLine &cli) override;
  string getUsage() const override { return "<lba> <value>"; }
  string getDescription() const override { return "Write <value> to SSD at logical block address <lba>"; }
  string getExample() const override { return "write 5 0xFF"; }
};

class EraseCommand : public ICommand {
private:
  bool isValidEraseUsage(const CommandLine &cli);
  const int MAX_SSD_ERASE_SIZE = 10;

public:
  bool execute(SSD_INTERFACE &ssd, const CommandLine &cli) override;
  string getUsage() const override { return "<lba> <size>"; }
  string getDescription() const override {
    return "Erase the logical blocks from <lba> for the specified <size>.";
  }
  string getExample() const override { return "erase 0 10"; }
};

class EraseRangeCommand : public ICommand {
private:
  bool isValidEraseRangeUsage(const CommandLine &cli);
  const int MAX_SSD_ERASE_SIZE = 10;

public:
  bool execute(SSD_INTERFACE &ssd, const CommandLine &cli) override;
  string getUsage() const override { return "<st_lba> <en_lba>"; }
  string getDescription() const override {
    return "Erase the SSD's logical blocks from st_lba to en_lba";
  }
  string getExample() const override { return "erase_range 0 99"; }
};

class FlushCommand : public ICommand {
public:
  bool execute(SSD_INTERFACE &ssd, const CommandLine &cli) override;
  string getUsage() const override { return ""; }
  string getDescription() const override {
    return "Flush all buffered commands to SSD";
  }
  string getExample() const override { return "flush"; }
};

class TestScript1 : public ICommand {
public:
  bool execute(SSD_INTERFACE &ssd, const CommandLine &cli) override;
  string getUsage() const override { return "<value>"; }
  string getDescription() const override { return "Run comprehensive write/read test for entire SSD"; }
  string getExample() const override { return "'1_' or '1_FullWriteAndReadCompare'"; }
};

class TestScript2 : public ICommand {
public:
  bool execute(SSD_INTERFACE &ssd, const CommandLine &cli) override;
  string getUsage() const override { return "<value>"; }
  string getDescription() const override { return "Run partial LBA write consistency test (30 iterations)"; }
  string getExample() const override { return "'2_' or '2_PartialLBAWrite'"; }
};

class TestScript3 : public ICommand {
public:
  bool execute(SSD_INTERFACE &ssd, const CommandLine &cli) override;
  string getUsage() const override { return ""; }
  string getDescription() const override { return "Run write/read aging test (200 iterations)"; }
  string getExample() const override { return "'3_' or '3_WriteReadAging'"; }
};

class TestScript4 : public ICommand {
public:
  bool execute(SSD_INTERFACE &ssd, const CommandLine &cli) override;
  string getUsage() const override { return ""; }
  string getDescription() const override {
    return "Repeatedly erase LBA ranges and write random data to following LBAs in sequential loops (30 iterations)";
  }
  string getExample() const override { return "'4_' or '4_EraseAndWriteAging'"; }
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
  void initialize();
  bool executeCommand(const CommandLine &cli);
  vector<string> getAvailableCommands();
  CommandLine parseCommand(const string &input);
  CommandHandler() { initialize(); }
  CommandHandler(SSD_INTERFACE *ssdDriver) : _ssdDriver(ssdDriver) {
    initialize();
  }

private:
  vector<ICommand *> commands;
  SSD_INTERFACE *_ssdDriver;

  ICommand *getCommand(const string &commandName);
  void addCommand(ICommand *command);
  void removeCommand(const string &commandName);
};
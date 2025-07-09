#pragma once

#include "test_shell.h"

class ExitCommand : public ICommandHandler {
public:
  void execute(TestShell *shell, const Command &command) override;
  string getUsage() const override { return ""; }
  string getDescription() const override { return "Exit the shell"; }
  string getExample() const override { return "exit"; }
};

class FullReadCommand : public ICommandHandler {
public:
  void execute(TestShell *shell, const Command &command) override;
  string getUsage() const override { return ""; }
  string getDescription() const override { return "Read the entire SSD"; }
  string getExample() const override { return "fullread"; }
};

class FullWriteCommand : public ICommandHandler {
public:
  void execute(TestShell *shell, const Command &command) override;
  string getUsage() const override { return "<value>"; }
  string getDescription() const override { return "Write <value> to the entire SSD"; }
  string getExample() const override { return "fullwrite 0x00"; }
};

class HelpCommand : public ICommandHandler {
public:
  void execute(TestShell *shell, const Command &command) override;
  string getUsage() const override { return ""; }
  string getDescription() const override { return "Show this help message"; }
  string getExample() const override { return "help"; }
};

class ReadCommand : public ICommandHandler {
private:
  bool isValidReadUsage(const Command &command);

public:
  void execute(TestShell *shell, const Command &command) override;
  string getUsage() const override { return "<lba>"; }
  string getDescription() const override { return "Read from SSD at logical block address <lba>"; }
  string getExample() const override { return "read 10"; }
};

class WriteCommand : public ICommandHandler {
private:
  bool isValidWriteUsage(const Command &command, int hexBase);

public:
  void execute(TestShell *shell, const Command &command) override;
  string getUsage() const override { return "<lba> <value>"; }
  string getDescription() const override { return "Write <value> to SSD at logical block address <lba>"; }
  string getExample() const override { return "write 5 0xFF"; }
};

class EraseCommand : public ICommandHandler {
private:
  bool isValidEraseUsage(const Command &command);
  const int MAX_SSD_ERASE_SIZE = 10;

public:
  void execute(TestShell *shell, const Command &command) override;
  string getUsage() const override { return "<lba> <size>"; }
  string getDescription() const override {
    return "Erase the logical blocks from <lba> for the specified <size>.";
  }
  string getExample() const override { return "erase 0 10"; }
};

class EraseRangeCommand : public ICommandHandler {
private:
  bool isValidEraseRangeUsage(const Command &command);
  const int MAX_SSD_ERASE_SIZE = 10;

public:
  void execute(TestShell *shell, const Command &command) override;
  string getUsage() const override { return "<st_lba> <en_lba>"; }
  string getDescription() const override {
    return "Erase the SSD's logical blocks from st_lba to en_lba";
  }
  string getExample() const override { return "erase_range 0 99"; }
};

class TestScript1 : public ICommandHandler {
public:
  void execute(TestShell *shell, const Command &command) override;
  string getUsage() const override { return "<value>"; }
  string getDescription() const override { return "Run comprehensive write/read test for entire SSD"; }
  string getExample() const override { return "'1_' or '1_FullWriteAndReadCompare'"; }
};

class TestScript2 : public ICommandHandler {
public:
  void execute(TestShell *shell, const Command &command) override;
  string getUsage() const override { return "<value>"; }
  string getDescription() const override { return "Run partial LBA write consistency test (30 iterations)"; }
  string getExample() const override { return "'2_' or '2_PartialLBAWrite'"; }
};

class TestScript3 : public ICommandHandler {
public:
  void execute(TestShell *shell, const Command &command) override;
  string getUsage() const override { return ""; }
  string getDescription() const override { return "Run write/read aging test (200 iterations)"; }
  string getExample() const override { return "'3_' or '3_WriteReadAging'"; }
};

class TestScript4 : public ICommandHandler {
public:
  void execute(TestShell *shell, const Command &command) override;
  string getUsage() const override { return ""; }
  string getDescription() const override {
    return "Repeatedly erase LBA ranges and write random data to following LBAs in sequential loops (30 iterations)";
  }
  string getExample() const override { return "'4_' or '4_EraseAndWriteAging'"; }
};
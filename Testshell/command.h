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
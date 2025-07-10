#pragma once
#include "ssd_cmd_buffer_control.h"
class ICommand {
public:
  virtual void execute(int lba, int size, unsigned long value, char *argv[],
                       CmdBufferControl &control) = 0;
  virtual ~ICommand() = default;
};

class CommandAlgorithm {
private:
  std::unique_ptr<ICommand> currentCmd;

public:
  void selectCommand(std::unique_ptr <ICommand> cmd) {
    currentCmd = std::move(cmd);
  }
  void execute(int lba, int size, unsigned long value, char *argv[],
               CmdBufferControl &control) {
    return currentCmd->execute(lba, size, value, argv, control);
  }
};

class ReadCommand : public ICommand {
public:
  void execute(int lba, int size, unsigned long value, char *argv[],
               CmdBufferControl &control);
};

class WriteCommand : public ICommand {
public:
  void execute(int lba, int size, unsigned long value, char *argv[],
               CmdBufferControl &control);
};

class EraseCommand : public ICommand {
public:
  void execute(int lba, int size, unsigned long value, char *argv[],
               CmdBufferControl &control);
};

class FlushCommand : public ICommand {
public:
  void execute(int lba, int size, unsigned long value, char *argv[],
               CmdBufferControl &control);
};
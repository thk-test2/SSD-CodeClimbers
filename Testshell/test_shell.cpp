#include "gmock/gmock.h"

#include <iostream>
#include <string>
#include <vector>

using std::cout;
using std::endl;
using std::string;
using std::vector;

class SSD_INTERFACE {
public:
  virtual void read(int lba) = 0;
  virtual void write(int lba, int value) = 0;
  virtual string getResult() = 0;
};

// MockDriver
class MockSSD : public SSD_INTERFACE {
public:
  MOCK_METHOD(void, read, (int lba), (override));
  MOCK_METHOD(void, write, (int lba, int value), (override));
  MOCK_METHOD(string, getResult, (), (override));
};

class Command {
public:
  string command;
  vector<string> args;
};

class StdInOutCtrl {
public:
};

class ArgParser {};

class TestShell {
private:
  StdInOutCtrl *ctrl;
  ArgParser *parser;
  SSD_INTERFACE *ssd;
  Command command;

public:
  TestShell() : ctrl(new StdInOutCtrl()), parser(new ArgParser()) {}
  TestShell(SSD_INTERFACE *_ssd)
      : ctrl(new StdInOutCtrl()), parser(new ArgParser()), ssd{_ssd} {}

  void run() {
    string userInput;
    cout << "SSD Test Shell - Type 'help' for userInputs" << endl;
    while (true) {
      cout << "> ";
      getline(std::cin, userInput);
      command = parsing(userInput);
      if (command.command == "exit") {
        break;
      }
      executeCommand(command);
    }
  }

  void executeCommand(const Command &command) {
    if (command.command == "read") {
      read(command);
    } else if (command.command == "write") {

    } else if (command.command == "fullread") {

    } else if (command.command == "fullwrite") {

    } else if (command.command == "help") {

    } else if (command.command == "1_") {

    } else if (command.command == "2_") {

    } else if (command.command == "3_") {

    } else {
      cout << "INVALID COMMAND" << endl;
    }
  }

  Command parsing(const string &userInput) {
    return Command{userInput,
                   vector<string>()}; // Simplified parsing for demonstration
  }

  void read(const Command &command) {
    if (!isValidReadUsage(command)) {
      cout << "INVALID COMMAND\n";
      return;
    }

    int lba = stoi(command.args[0]);
    ssd->read(lba);
    cout << "[Read] " << command.args[0] << " : " << ssd->getResult() << "\n";
  }

  bool isValidReadUsage(const Command &command) {
    if (command.args.size() != 1)
      return false;
    try {
      int lba = stoi(command.args[0]);
    } catch (std::exception &e) {
      return false;
    }
    return true;
  }
};

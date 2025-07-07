#include "gmock/gmock.h"
#include <string>
#include <iostream>
#include <vector>

using std::string;
using std::vector;
using std::cout;
using std::endl;

class SSD_INTERFACE {
public:
  virtual bool read(int lba) = 0;
  virtual bool write(int lba, int value) = 0;
  virtual string getResult() = 0;
};

// MockDriver
class MockSSD : public SSD_INTERFACE {
public:
  MOCK_METHOD(bool, read, (int lba), (override));
  MOCK_METHOD(bool, write, (int lba, int value), (override));
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

class ArgParser {

};

class SsdInterface : public SSD_INTERFACE {
public:
  bool read(int lba) override { return true; }
  bool write(int lba, int value) override { return true; }
  string getResult() override { return ""; }
};

class TestShell {
private:
  StdInOutCtrl *ctrl;
  ArgParser *parser;
  SsdInterface *ssd;
  Command command;

public:
  TestShell() : ctrl(new StdInOutCtrl()), parser(new ArgParser()), ssd(new SsdInterface()) {}

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

  void executeCommand(const Command& command) {
    if (command.command == "read") {

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

  Command parsing(const string& userInput) {
    return Command{userInput,
                   vector<string>()}; // Simplified parsing for demonstration
  }
};

int main() {
#ifdef _DEBUG
  ::testing::InitGoogleTest();
  return RUN_ALL_TESTS();
#else
  TestShell testShell;
  testShell.run();
  return 0;
#endif
}
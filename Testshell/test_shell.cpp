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

class ArgParser {};

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
  TestShell()
      : ctrl(new StdInOutCtrl()), parser(new ArgParser()),
        ssd(new SsdInterface()) {}

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

    } else if (command.command == "write") {

    } else if (command.command == "fullread") {

    } else if (command.command == "fullwrite") {

    } else if (command.command == "help") {
      help();
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
  void help() {
    cout << "\033[1;36m"
         << "SSD Test Shell - Simple and Powerful SSD Testing Tool\n"
         << "\033[0m";

    cout << "\n\033[1mTeam: CodeClimbers\n\033[0m";

    cout << "\n\033[1mTeam Members:\033[0m\n"
         << "  Taehyeon Kyung, Sunghwan Kim, Hyeonseok Sim\n"
         << "  Yerim Yun, Hoenhwi Jeong, Jeseong Kim\n"
         << "  Repository: https://github.com/thk-test2/SSD-CodeClimbers\n";

    cout << "\n\033[1mCommands:\033[0m\n"
         << "  \033[1mread\033[0m <lba>\n"
         << "    Description: Read from SSD at logical block address <lba>\n"
         << "    Example: read 10\n\n"

         << "  \033[1mwrite\033[0m <lba> <value>\n"
         << "    Description: Write <value> to SSD at logical block address "
            "<lba>\n"
         << "    Example: write 5 0xFF\n\n"

         << "  \033[1mfullread\033[0m\n"
         << "    Description: Read the entire SSD\n"
         << "    Example: fullread\n\n"

         << "  \033[1mfullwrite\033[0m <value>\n"
         << "    Description: Write <value> to the entire SSD\n"
         << "    Example: fullwrite 0x00\n\n"

         << "  \033[1mhelp\033[0m\n"
         << "    Description: Show this help message\n"
         << "    Example: help\n\n"

         << "  \033[1mexit\033[0m\n"
         << "    Description: Exit the shell\n"
         << "    Example: exit\n";
  }
};

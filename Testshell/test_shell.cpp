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
    printHeader();
    printTeamInfo();
    printCommands();
  }

  void printHeader() {
    cout << "\033[1;36m"
         << "SSD Test Shell - Simple and Powerful SSD Testing Tool\n"
         << "\033[0m";
  }

  void printTeamInfo() {
    cout << "\n\033[1mTeam: CodeClimbers\n\033[0m";
    cout << "\n\033[1mTeam Members:\033[0m\n"
         << "  Taehyeon Kyung, Sunghwan Kim, Hyeonseok Sim\n"
         << "  Yerim Yun, Hoenhwi Jeong, Jeseong Kim\n"

         << "  Repository: https://github.com/thk-test2/SSD-CodeClimbers\n";
  }

  void printCommands() {
    cout << "\n\033[1mCommands:\033[0m\n";
    printCommandInfo("read", "<lba>",
                     "Read from SSD at logical block address <lba>", "read 10");
    printCommandInfo("write", "<lba> <value>",
                     "Write <value> to SSD at logical block address <lba>",
                     "write 5 0xFF");
    printCommandInfo("fullread", "", "Read the entire SSD", "fullread");
    printCommandInfo("fullwrite", "<value>", "Write <value> to the entire SSD",
                     "fullwrite 0x00");
    printCommandInfo("help", "", "Show this help message", "help");
    printCommandInfo("exit", "", "Exit the shell", "exit");
  }

  void printCommandInfo(const string &command, const string &args,
                        const string &description, const string &example) {
    cout << "  \033[1m" << command << "\033[0m";
    if (!args.empty()) {
      cout << " " << args;
    }
    cout << "\n    Description: " << description << "\n"
         << "    Example: " << example << "\n\n";
  }
};

#include "gmock/gmock.h"

#include <iostream>
#include <string>
#include <vector>

using std::cout;
using std::endl;
using std::string;
using std::vector;

struct ShellExit : public std::exception {
  const char *what() const noexcept override { return "Shell exited."; }
};

class SSD_INTERFACE {
public:
  virtual void read(int lba) = 0;
  virtual void write(int lba, unsigned long value) = 0;
  virtual string getResult() = 0;
};

// MockDriver
class MockSSD : public SSD_INTERFACE {
public:
  MOCK_METHOD(void, read, (int lba), (override));
  MOCK_METHOD(void, write, (int lba, unsigned long value), (override));
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
      write(command);
    } else if (command.command == "exit") {
      exitTestShell(command);
    } else if (command.command == "fullread") {
      fullread(command.args);
    } else if (command.command == "fullwrite") {
      fullwrite(command.args);
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

  void exitTestShell(const Command &command) {
    if (command.args.size() != 0) {
      cout << "INVALID COMMAND\n";
      return;
    }
    cout << "Exiting shell..." << endl;
    throw ShellExit();
  }

  void fullread(vector<string> args) {
    if (args.size() != 0) {
      cout << "INVALID COMMAND\n";
      return;
    }
    int lba = 0;
    ssd->read(lba);
    string result = ssd->getResult();
    while (result != "ERROR") {
      cout << "[Full Read] LBA: " << lba << " Result: " << result << endl;
      lba++;
      ssd->read(lba);
      result = ssd->getResult();
    }
  }

  void fullwrite(vector<string> args) {
    if (args.size() != 1) {
      cout << "INVALID COMMAND\n";
      return;
    }
    unsigned long value;
    try {
      value = stoul(args[0]);
    } catch (std::exception &e) {
      cout << "INVALID COMMAND\n";
      return;
    }
    int lba = 0;
    ssd->write(lba, value);
    while (ssd->getResult() != "ERROR") {
      cout << "[Full Write] LBA: " << lba << " Value: " << value << endl;
      lba++;
      ssd->write(lba, value);
    }
  }

  void help() {
    printHeader();
    printTeamInfo();
    printCommands();
    printTestScripts();
  }

  void printHeader() {
    cout << "\033[1;36m"
         << "SSD Test Shell - Simple and Powerful SSD Testing Tool\n"
         << "\033[0m";
  }

  void printTeamInfo() {
    cout << "\n\033[1mTeam: CodeClimbers\n\033[0m";
    cout << "\n\033[1mTeam Members:\033[0m\n"
         << "  Taehyun Kyong, Sunghwan Kim, Hyeonseok Sim\n"
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

  void printTestScripts() {
    cout << "\033[1mTest Scripts:\033[0m\n";
    printCommandInfo("1_FullWriteAndReadCompare", "",
                     "Run comprehensive write/read test for entire SSD",
                     "'1_' or '1_FullWriteAndReadCompare'");
    printCommandInfo("2_PartialLBAWrite", "",
                     "Run partial LBA write consistency test (30 iterations)",
                     "'2_' or '2_PartialLBAWrite'");
    printCommandInfo("3_WriteReadAging", "",
                     "Run write/read aging test (200 iterations)",
                     "'3_' or '3_WriteReadAging'");
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

  void write(const Command &command) {
    if (!isValidWriteUsage(command)) {
      cout << "INVALID COMMAND\n";
      return;
    }
    ssd->write(stoi(command.args[0]), stringToUnsignedLong(command.args[1]));
    string result = ssd->getResult();
    if (result == "")
      result = "Done";
    cout << "[Write] " << result << "\n";
  }

  bool isValidWriteUsage(const Command &command) {
    if (command.args.size() != 2)
      return false;
    try {
      int lba = stoi(command.args[0]);
      unsigned long value = stringToUnsignedLong(command.args[1]);
    } catch (std::exception &e) {
      return false;
    }
    return true;
  }

  unsigned long stringToUnsignedLong(const string &str) {
    if (str.substr(0, 2) == "0x") {
      return stoul(str.substr(2), nullptr, 16);
    } else {
      return stoul(str);
    }
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
